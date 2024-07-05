/**
 ** Npcedit.cc - Npc-editing methods.
 **
 ** Written: 6/8/01 - JSF
 **/

/*
Copyright (C) 2000-2022 The Exult Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "exult_constants.h"
#include "npclst.h"
#include "objserial.h"
#include "servemsg.h"
#include "shapedraw.h"
#include "shapefile.h"
#include "shapevga.h"
#include "studio.h"
#include "u7drag.h"

#include <cstdlib>
#include <cstring>

using std::cout;
using std::endl;

class Actor;

/*
 *  Open npc window.
 */

C_EXPORT void on_open_npc_activate(GtkMenuItem* menuitem, gpointer user_data) {
	ignore_unused_variable_warning(menuitem, user_data);
	ExultStudio* studio = ExultStudio::get_instance();
	studio->open_npc_window();
}

/*
 *  Npc window's Apply button.
 */
C_EXPORT void on_npc_apply_btn_clicked(GtkButton* btn, gpointer user_data) {
	ignore_unused_variable_warning(btn, user_data);
	ExultStudio::get_instance()->set_npc_modified();
	ExultStudio::get_instance()->save_npc_window();
}

/*
 *  Npc window's Cancel button.
 */
C_EXPORT void on_npc_cancel_btn_clicked(GtkButton* btn, gpointer user_data) {
	ignore_unused_variable_warning(btn, user_data);
	ExultStudio::get_instance()->close_npc_window();
}

/*
 *  Display the npc's gump.
 */
C_EXPORT void on_npc_show_gump_clicked(GtkButton* btn, gpointer user_data) {
	ignore_unused_variable_warning(user_data);
	cout << "In on_npc_show_gump_clicked()" << endl;
	unsigned char data[Exult_server::maxlength];
	// Get container address.
	auto           addr = reinterpret_cast<uintptr>(g_object_get_data(
            G_OBJECT(gtk_widget_get_toplevel(GTK_WIDGET(btn))), "user_data"));
	unsigned char* ptr  = &data[0];
	Serial_out     io(ptr);
	io << addr;

	ExultStudio::get_instance()->send_to_server(
			Exult_server::cont_show_gump, data, ptr - data);
	cout << "Sent npc container data to server" << endl;
}

/*
 *  Npc window's close button.
 */
C_EXPORT gboolean on_npc_window_delete_event(
		GtkWidget* widget, GdkEvent* event, gpointer user_data) {
	ignore_unused_variable_warning(widget, event, user_data);
	ExultStudio::get_instance()->close_npc_window();
	return true;
}

/*
 *  Browse for usecode.
 */
C_EXPORT void on_npc_usecode_browse_clicked(
		GtkButton* button, gpointer user_data) {
	ignore_unused_variable_warning(button, user_data);
	ExultStudio* studio = ExultStudio::get_instance();
	const char*  uc     = studio->browse_usecode(true);
	if (*uc) {
		studio->set_entry("npc_usecode_entry", uc, true);
	}
}

// Schedule names.

static const char* sched_names[32]
		= {"Combat",    "Horiz. Pace", "Vert. Pace", "Talk",       "Dance",
		   "Eat",       "Farm",        "Tend Shop",  "Miner",      "Hound",
		   "Stand",     "Loiter",      "Wander",     "Blacksmith", "Sleep",
		   "Wait",      "Sit",         "Graze",      "Bake",       "Sew",
		   "Shy",       "Lab Work",    "Thief",      "Waiter",     "Special",
		   "Kid Games", "Eat at Inn",  "Duel",       "Preach",     "Patrol",
		   "Desk Work", "Follow"};

/*
 *  Set a line in the schedule page.
 */

static void Set_schedule_line(
		ExultStudio* studio,
		int          time,            // 0-7.
		int          type,            // Activity (0-31, or -1 for none).
		int tx, int ty, int tz = 0    // Location.
) {
	char*     lname = g_strdup_printf("npc_sched%d", time);
	GtkLabel* label = GTK_LABEL(studio->get_widget(lname));
	g_free(lname);
	// User data = schedule #.
	g_object_set_data(
			G_OBJECT(label), "user_data",
			reinterpret_cast<gpointer>(uintptr(type)));
	gtk_label_set_text(
			label, type >= 0 && type < 32 ? sched_names[type] : "-----");
	// Set location.
	char*   locname = g_strdup_printf("sched_loc%d", time);
	GtkBox* box     = GTK_BOX(studio->get_widget(locname));
	g_free(locname);
	GList* children
			= g_list_first(gtk_container_get_children(GTK_CONTAINER(box)));
	GList*     list = children;
	GtkWidget* spin = GTK_WIDGET(list->data);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), tx);
	list = g_list_next(list);
	spin = GTK_WIDGET(list->data);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), ty);
	list = g_list_next(list);
	spin = GTK_WIDGET(list->data);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), tz);
	g_list_free(children);
}

/*
 *  Get info. from line in the schedule page.
 *
 *  Output: False if schedule isn't set.
 */

static bool Get_schedule_line(
		ExultStudio*     studio,
		int              time,    // 0-7.
		Serial_schedule& sched    // Filled in if 'true' returned.
) {
	char*     lname = g_strdup_printf("npc_sched%d", time);
	GtkLabel* label = GTK_LABEL(studio->get_widget(lname));
	g_free(lname);
	// User data = schedule #.
	sched.type = reinterpret_cast<sintptr>(
			g_object_get_data(G_OBJECT(label), "user_data"));
	if (sched.type < 0 || sched.type > 31) {
		return false;
	}
	// Get location.
	char*   locname = g_strdup_printf("sched_loc%d", time);
	GtkBox* box     = GTK_BOX(studio->get_widget(locname));
	g_free(locname);
	GList* children
			= g_list_first(gtk_container_get_children(GTK_CONTAINER(box)));
	GList*     list = children;
	GtkWidget* spin = GTK_WIDGET(list->data);
	sched.tx        = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin));
	list            = g_list_next(list);
	spin            = GTK_WIDGET(list->data);
	sched.ty        = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin));
	list            = g_list_next(list);
	spin            = GTK_WIDGET(list->data);
	sched.tz        = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin));
	sched.time      = time;
	g_list_free(children);
	return true;
}

/*
 *  Open the npc-editing window.
 */

void ExultStudio::open_npc_window(
		unsigned char* data,    // Serialized npc, or null.
		int            datalen) {
	if (!npcwin) {    // First time?
		npcwin = get_widget("npc_window");

		if (vgafile && palbuf) {
			npc_single = new Shape_single(
					get_widget("npc_shape"), nullptr,
					[](int shnum) -> bool {
						return (shnum >= c_first_obj_shape)
							   && (shnum < c_max_shapes);
					},
					get_widget("npc_frame"), U7_SHAPE_SHAPES,
					vgafile->get_ifile(), palbuf.get(), get_widget("npc_draw"));
		}
		if (facefile && palbuf) {
			npc_face_single = new Shape_single(
					get_widget("npc_face_frame"), nullptr,
					[](int shnum) -> bool {
						return (shnum >= 0) && (shnum < 1024);
					},
					nullptr, U7_SHAPE_FACES, facefile->get_ifile(),
					palbuf.get(), get_widget("npc_face_draw"));
		}
		npc_ctx = gtk_statusbar_get_context_id(
				GTK_STATUSBAR(get_widget("npc_status")), "Npc Editor");
		for (int i = 0; i < 24 / 3; i++) {    // Init. schedules' user_data.
			Set_schedule_line(this, i, -1, 0, 0, 0);
		}
	}
	// Init. npc address to null.
	g_object_set_data(G_OBJECT(npcwin), "user_data", nullptr);
	// Make 'apply', 'cancel' sensitive.
	set_sensitive("npc_apply_btn", true);
	set_sensitive("npc_cancel_btn", true);
	remove_statusbar("npc_status", npc_ctx, npc_status_id);
	if (data) {
		if (!init_npc_window(data, datalen)) {
			return;
		}
	} else {
		init_new_npc();
	}
	gtk_widget_set_visible(npcwin, true);
}

/*
 *  Close the npc-editing window.
 */

void ExultStudio::close_npc_window() {
	if (npcwin) {
		gtk_widget_set_visible(npcwin, false);
	}
}

/*
 *  Get one of the NPC 'property' spin buttons from the table in the NPC
 *  dialog box.
 *
 *  Output: true if successful, with spin, pnum returned.
 */

static bool Get_prop_spin(
		GList*          list,    // Entry in table of properties.
		GtkSpinButton*& spin,    // Spin button returned.
		int&            pnum     // Property number (0-11) returned.
) {
	GtkBin* frame = GTK_BIN(list->data);
	spin          = GTK_SPIN_BUTTON(gtk_bin_get_child(frame));
	assert(spin != nullptr);
	const char* name = gtk_buildable_get_name(GTK_BUILDABLE(spin));
	// Names: npc_prop_nn.
	if (strncmp(name, "npc_prop_", 9) != 0) {
		return false;
	}
	pnum = atoi(name + 9);
	return pnum >= 0 && pnum < 12;
}

/*
 *  Get one of the NPC flag checkbox's from the table in the NPC
 *  dialog box.
 *
 *  Output: true if successful, with cbox, fnum returned.
 */

static bool Get_flag_cbox(
		GList*           list,          // Entry in table of flags.
		unsigned long*   oflags,        // Object flags.
		unsigned long*   xflags,        // Extra object flags.
		unsigned long*   type_flags,    // Type (movement) flags.
		GtkCheckButton*& cbox,          // Checkbox returned.
		unsigned long*&  bits,          // ->one of 3 flags above.
		int&             fnum           // Flag # (0-31) returned.
) {
	cbox = GTK_CHECK_BUTTON(list->data);
	assert(cbox != nullptr);
	const char* name = gtk_buildable_get_name(GTK_BUILDABLE(cbox));
	// Names: npc_flag_xx_nn, where
	//   xx = si, of, tf.
	if (strncmp(name, "npc_flag_", 9) != 0) {
		return false;
	}
	// Which flag.
	if (strncmp(name + 9, "si", 2) == 0) {
		bits = xflags;
	} else if (strncmp(name + 9, "of", 2) == 0) {
		bits = oflags;
	} else if (strncmp(name + 9, "tf", 2) == 0) {
		bits = type_flags;
	} else {
		return false;
	}
	fnum = atoi(name + 9 + 3);
	return fnum >= 0 && fnum < 32;
}

/*
 *  Init. the npc editor for a new NPC.
 */

void ExultStudio::init_new_npc() {
	int npc_num = -1;    // Got to get what new NPC # will be.
	if (Send_data(server_socket, Exult_server::npc_unused) == -1) {
		cout << "Error sending data to server." << endl;
	}
	// Should get immediate answer.
	unsigned char          data[Exult_server::maxlength];
	Exult_server::Msg_type id;
	Exult_server::wait_for_response(server_socket, 100);
	Exult_server::Receive_data(server_socket, id, data, sizeof(data));
	const unsigned char* ptr = &data[0];
	little_endian::Read2(ptr);    // Snip number of NPCs
	const int first_unused = little_endian::Read2(ptr);
	npc_num                = first_unused;
	set_entry("npc_num_entry", npc_num, true, false);
	// Usually, usecode = 0x400 + num.
	set_entry("npc_usecode_entry", 0x400 + npc_num, true, npc_num >= 256);
	// Usually, face = npc_num.
	{
		GtkWidget* widget = get_widget("npc_face_frame");
		g_object_set_data(
				G_OBJECT(widget), "user_data",
				reinterpret_cast<gpointer>(uintptr(npc_num)));
		char* label = g_strdup_printf("Face #%d", npc_num);
		gtk_frame_set_label(GTK_FRAME(widget), label);
		g_free(label);
	}
	set_entry("npc_name_entry", "");
	set_entry("npc_ident_entry", 0);
	set_entry("npc_shape", -1);
	set_entry("npc_frame", 0);
	set_optmenu("npc_attack_mode", 0);
	set_optmenu("npc_alignment", 0);
	// Clear flag buttons.
	GtkContainer* ftable = GTK_CONTAINER(get_widget("npc_flags_table"));
	// Set flag checkboxes.
	GList* children = g_list_first(gtk_container_get_children(ftable));
	for (GList* list = children; list; list = g_list_next(list)) {
		GtkCheckButton* cbox = GTK_CHECK_BUTTON(list->data);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbox), false);
	}
	g_list_free(children);
	// Make sure the "default" NPC can walk around.
	set_toggle("npc_flag_tf_05", true);
	// Set properties.
	GtkContainer* ptable = GTK_CONTAINER(get_widget("npc_props_table"));
	children             = g_list_first(gtk_container_get_children(ptable));
	for (GList* list = children; list; list = g_list_next(list)) {
		GtkSpinButton* spin;
		int            pnum;
		if (Get_prop_spin(list, spin, pnum)) {
			gtk_spin_button_set_value(spin, 12);
		}
	}
	g_list_free(children);
	// Clear schedules.
	for (int i = 0; i < 24 / 3; i++) {
		Set_schedule_line(this, i, -1, 0, 0, 0);
	}
}

/*
 *  Init. the npc editor with data from Exult.
 *
 *  Output: 0 if error (reported).
 */

int ExultStudio::init_npc_window(unsigned char* data, int datalen) {
	Actor*          addr;
	int             tx;
	int             ty;
	int             tz;
	int             shape;
	int             frame;
	int             face;
	std::string     name;
	short           npc_num;
	short           ident;
	int             usecode;
	std::string     usecodefun;
	int             properties[12];
	short           attack_mode;
	short           alignment;
	unsigned long   oflags;        // Object flags.
	unsigned long   xflags;        // Extra object flags.
	unsigned long   type_flags;    // Movement flags.
	short           num_schedules;
	Serial_schedule schedules[8];
	if (!Npc_actor_in(
				data, datalen, addr, tx, ty, tz, shape, frame, face, name,
				npc_num, ident, usecode, usecodefun, properties, attack_mode,
				alignment, oflags, xflags, type_flags, num_schedules,
				schedules)) {
		cout << "Error decoding npc" << endl;
		return 0;
	}
	// Store address with window.
	g_object_set_data(G_OBJECT(npcwin), "user_data", addr);
	// Store name, ident, num.
	const std::string utf8name(convertToUTF8(name.c_str()));
	set_entry("npc_name_entry", utf8name.c_str());
	// (Not allowed to change npc#.).
	set_entry("npc_num_entry", npc_num, true, false);
	set_entry("npc_ident_entry", ident);
	// Shape/frame.
	set_entry("npc_shape", shape);
	set_entry("npc_frame", 16);
	{
		GtkWidget* widget = get_widget("npc_face_frame");
		g_object_set_data(
				G_OBJECT(widget), "user_data",
				reinterpret_cast<gpointer>(uintptr(face)));
		char* label = g_strdup_printf("Face #%d", face);
		gtk_frame_set_label(GTK_FRAME(widget), label);
		g_free(label);
	}
	// Usecode #.
	if (npc_num >= 256 && !usecodefun.empty()) {
		set_entry("npc_usecode_entry", usecodefun.c_str(), true);
	} else {
		set_entry("npc_usecode_entry", usecode, true, npc_num >= 256);
	}
	// Combat:
	set_optmenu("npc_attack_mode", attack_mode);
	set_optmenu("npc_alignment", alignment);
	// Set flag buttons.
	GtkContainer* ftable = GTK_CONTAINER(get_widget("npc_flags_table"));
	// Set flag checkboxes.
	GList* children = g_list_first(gtk_container_get_children(ftable));
	for (GList* list = children; list; list = g_list_next(list)) {
		GtkCheckButton* cbox;
		unsigned long*  bits;
		int             fnum;
		if (Get_flag_cbox(
					list, &oflags, &xflags, &type_flags, cbox, bits, fnum)) {
			gtk_toggle_button_set_active(
					GTK_TOGGLE_BUTTON(cbox), (*bits & (1 << fnum)) != 0);
		}
	}
	g_list_free(children);
	// Set properties.
	GtkContainer* ptable = GTK_CONTAINER(get_widget("npc_props_table"));
	children             = g_list_first(gtk_container_get_children(ptable));
	for (GList* list = children; list; list = g_list_next(list)) {
		GtkSpinButton* spin;
		int            pnum;
		if (Get_prop_spin(list, spin, pnum)) {
			gtk_spin_button_set_value(spin, properties[pnum]);
		}
	}
	g_list_free(children);
	// Set schedules.
	for (int i = 0; i < 24 / 3; i++) {    // First init. to empty.
		Set_schedule_line(this, i, -1, 0, 0, 0);
	}
	for (int i = 0; i < num_schedules; i++) {
		const Serial_schedule& sched = schedules[i];
		const int              time  = sched.time;    // 0-7.
		if (time < 0 || time > 7) {
			continue;    // Invalid.
		}
		Set_schedule_line(this, time, sched.type, sched.tx, sched.ty, sched.tz);
	}
	return 1;
}

/*
 * This is used because calling npcchoose->update_npc(npc_num) in
 * save_npc_window() means "Click on map at place to insert npc" will not
 * show up until after the npc is placed and the npc editor window won't
 * close. You can't call it in Npc_response because browser is private.
 */
void ExultStudio::update_npc() {    // update npc browser display (if open)
	auto* npcchoose = dynamic_cast<Npc_chooser*>(browser);
	if (npcchoose) {
		const short npc_num = get_num_entry("npc_num_entry");
		npcchoose->update_npc(npc_num);
	}
}

/*
 *  Callback for when user clicked where NPC should be inserted.
 */

static void Npc_response(
		Exult_server::Msg_type id, const unsigned char* data, int datalen,
		void* /* client */
) {
	ignore_unused_variable_warning(data, datalen);
	ExultStudio* studio = ExultStudio::get_instance();
	if (id == Exult_server::user_responded) {
		studio->close_npc_window();
		studio->update_npc();
	}
	//+++++cancel??
}

/*
 *  Send updated NPC info. back to Exult.
 *
 *  Output: 0 if error (reported).
 */

int ExultStudio::save_npc_window() {
	cout << "In save_npc_window()" << endl;
	// Get npc (null if creating new).
	auto* addr = static_cast<Actor*>(
			g_object_get_data(G_OBJECT(npcwin), "user_data"));
	const int         tx = -1;
	const int         ty = -1;
	const int         tz = -1;    // +++++For now.
	const std::string name(convertFromUTF8(get_text_entry("npc_name_entry")));
	const short       npc_num = get_num_entry("npc_num_entry");
	const short       ident   = get_num_entry("npc_ident_entry");
	const int         shape   = get_num_entry("npc_shape");
	const int         frame   = get_num_entry("npc_frame");
	GtkWidget*        fw      = get_widget("npc_face_frame");
	const int         face    = reinterpret_cast<sintptr>(
            g_object_get_data(G_OBJECT(fw), "user_data"));
	const int   usecode    = get_num_entry("npc_usecode_entry");
	const char* usecodefun = "";
	if (!usecode) {
		usecodefun = get_text_entry("npc_usecode_entry");
	}
	const short attack_mode = get_optmenu("npc_attack_mode");
	const short alignment   = get_optmenu("npc_alignment");

	unsigned long oflags     = 0;    // Object flags.
	unsigned long xflags     = 0;    // Extra object flags.
	unsigned long type_flags = 0;    // Movement flags.

	if (shape < 0 || shape >= vgafile->get_ifile()->get_num_shapes()) {
		EStudio::Alert("You must set a valid shape #.");
		return 0;
	}
	// Set flag buttons.
	GtkContainer* ftable = GTK_CONTAINER(get_widget("npc_flags_table"));
	// Get flags.
	GList* children = g_list_first(gtk_container_get_children(ftable));
	for (GList* list = children; list; list = g_list_next(list)) {
		GtkCheckButton* cbox;
		unsigned long*  bits;
		int             fnum;
		if (Get_flag_cbox(
					list, &oflags, &xflags, &type_flags, cbox, bits, fnum)) {
			if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cbox))) {
				*bits |= (1 << fnum);
			}
		}
	}
	g_list_free(children);
	int           properties[12];    // Get properties.
	GtkContainer* ptable = GTK_CONTAINER(get_widget("npc_props_table"));
	children             = g_list_first(gtk_container_get_children(ptable));
	for (GList* list = children; list; list = g_list_next(list)) {
		GtkSpinButton* spin;
		int            pnum;
		if (Get_prop_spin(list, spin, pnum)) {
			properties[pnum] = gtk_spin_button_get_value_as_int(spin);
		}
	}
	g_list_free(children);
	short           num_schedules = 0;
	Serial_schedule schedules[8];
	for (int i = 0; i < 8; i++) {
		if (Get_schedule_line(this, i, schedules[num_schedules])) {
			num_schedules++;
		}
	}
	if (Npc_actor_out(
				server_socket, addr, tx, ty, tz, shape, frame, face, name,
				npc_num, ident, usecode, usecodefun, properties, attack_mode,
				alignment, oflags, xflags, type_flags, num_schedules, schedules)
		== -1) {
		cout << "Error sending npc data to server" << endl;
		return 0;
	}
	cout << "Sent npc data to server" << endl;
	if (!addr) {
		npc_status_id = set_statusbar(
				"npc_status", npc_ctx, "Click on map at place to insert npc");
		// Make 'apply' insensitive.
		set_sensitive("npc_apply_btn", false);
		set_sensitive("npc_cancel_btn", false);
		waiting_for_server = Npc_response;
		return 1;    // Leave window open.
	}
	// Update NPC browser information.
	auto* npcchoose = dynamic_cast<Npc_chooser*>(browser);
	if (npcchoose) {
		npcchoose->update_npc(npc_num);
	}
	close_npc_window();
	return 1;
}

/*
 *  A choice was clicked in the 'schedule' dialog.
 */

void ExultStudio::schedule_btn_clicked(
		GtkWidget* btn,    // Button on the schedule dialog.
		gpointer   data    // Dialog itself.
) {
	ExultStudio* studio = ExultStudio::get_instance();
	// Get name assigned in Glade.
	const char* name     = gtk_buildable_get_name(GTK_BUILDABLE(btn));
	const char* numptr   = name + 5;    // Past "sched".
	const int   num      = atoi(numptr);
	auto*       schedwin = static_cast<GtkWidget*>(data);
	auto*       label    = static_cast<GtkLabel*>(
            g_object_get_data(G_OBJECT(schedwin), "user_data"));
	// User data = schedule #.
	g_object_set_data(
			G_OBJECT(label), "user_data",
			reinterpret_cast<gpointer>(uintptr(num)));
	gtk_label_set_text(
			label, num >= 0 && num < 32 ? sched_names[num] : "-----");
	cout << "Chose schedule " << num << endl;
	gtk_widget_set_visible(studio->get_widget("schedule_dialog"), false);
}

/*
 *  Set signal handler for each schedule button.
 */

static void Set_sched_btn(GtkWidget* btn, gpointer data) {
	g_signal_connect(
			G_OBJECT(btn), "clicked",
			G_CALLBACK(ExultStudio::schedule_btn_clicked), data);
}

/*
 *  Npc window's "set schedule" button.
 */
C_EXPORT void on_npc_set_sched(
		GtkWidget* btn,    // One of the 'set' buttons.
		gpointer   user_data) {
	ignore_unused_variable_warning(user_data);
	static int  first  = 1;    // To initialize signal handlers.
	const char* name   = gtk_buildable_get_name(GTK_BUILDABLE(btn));
	const char* numptr = name + strlen(name) - 1;
	char        lname[20];    // Set up label name.
	strcpy(lname, "npc_sched");
	strcat(lname, numptr);    // Same number as button.
	ExultStudio*  studio   = ExultStudio::get_instance();
	GtkLabel*     label    = GTK_LABEL(studio->get_widget(lname));
	GtkContainer* btns     = GTK_CONTAINER(studio->get_widget("sched_btns"));
	GtkWidget*    schedwin = studio->get_widget("schedule_dialog");
	if (!label || !btns || !schedwin) {
		return;
	}
	if (first) {    // First time?  Set handlers.
		first = 0;
		gtk_container_foreach(btns, Set_sched_btn, schedwin);
	}
	// Store label as dialog's data.
	g_object_set_data(G_OBJECT(schedwin), "user_data", label);
	gtk_widget_set_visible(schedwin, true);
}

/*
 *  Received game position for schedule.
 */
static void Game_loc_response(
		Exult_server::Msg_type id, const unsigned char* data, int datalen,
		void* client) {
	ignore_unused_variable_warning(datalen);
	if (id != Exult_server::game_pos) {
		return;
	}
	// Get box with loc. spin btns.
	auto*     box = static_cast<GtkBox*>(client);
	const int tx  = little_endian::Read2(data);
	const int ty  = little_endian::Read2(data);
	const int tz  = little_endian::Read2(data);
	GList*    children
			= g_list_first(gtk_container_get_children(GTK_CONTAINER(box)));
	GList*     list = children;
	GtkWidget* spin = GTK_WIDGET(list->data);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), tx);
	list = g_list_next(list);
	spin = GTK_WIDGET(list->data);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), ty);
	list = g_list_next(list);
	spin = GTK_WIDGET(list->data);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), tz);
	g_list_free(children);
}

/*
 *  One of the "Game" buttons to set location from current game position.
 */

C_EXPORT void on_sched_loc_clicked(
		GtkWidget* btn,    // One of the 'Game' buttons.
		gpointer   user_data) {
	ignore_unused_variable_warning(user_data);
	ExultStudio* studio = ExultStudio::get_instance();
	// Get location box.
	GtkBox* box = GTK_BOX(gtk_widget_get_parent(btn));
	if (Send_data(studio->get_server_socket(), Exult_server::game_pos) == -1) {
		cout << "Error sending message to server" << endl;
	} else {
		studio->set_msg_callback(Game_loc_response, box);
	}
}
