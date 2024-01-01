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

#ifndef STUDIO_H

#define STUDIO_H

#ifdef __GNUC__
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#	pragma GCC diagnostic ignored "-Wold-style-cast"
#	if !defined(__llvm__) && !defined(__clang__)
#		pragma GCC diagnostic ignored "-Wuseless-cast"
#	else
#		pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#		if __clang_major__ >= 16
#			pragma GCC diagnostic ignored "-Wcast-function-type-strict"
#		endif
#	endif
#endif    // __GNUC__
#ifdef USE_STRICT_GTK
#	define GTK_DISABLE_SINGLE_INCLUDES
#	define GSEAL_ENABLE
#	define GNOME_DISABLE_DEPRECATED
#	define GTK_DISABLE_DEPRECATED
#	define GDK_DISABLE_DEPRECATED
#endif    // USE_STRICT_GTK
#include <gtk/gtk.h>
#ifndef __GDK_KEYSYMS_H__
#	include <gdk/gdkkeysyms.h>
#endif
#ifdef __GNUC__
#	pragma GCC diagnostic pop
#endif    // __GNUC__

#include "exult_constants.h"
#include "gtk_redefines.h"
#include "servemsg.h"
#include "vgafile.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

#ifndef ATTR_PRINTF
#	ifdef __GNUC__
#		define ATTR_PRINTF(x, y) __attribute__((format(printf, (x), (y))))
#	else
#		define ATTR_PRINTF(x, y)
#	endif
#endif

const int HMARGIN = 2;
const int VMARGIN = 1;

inline void widget_set_margins(GtkWidget *w, int s, int e, int t, int b) {
	gtk_widget_set_margin_start  (w, s);
	gtk_widget_set_margin_end    (w, e);
	gtk_widget_set_margin_top    (w, t);
	gtk_widget_set_margin_bottom (w, b);
}

class Shape_info;
class Shapes_vga_file;
struct Equip_row_widgets;
class Shape_file_set;
class Shape_file_info;
class Shape_group_file;
class Shape_single;
class Object_browser;
class Shape_group;
class Locator;
class Usecode_browser;
class Combo_editor;
class Exec_box;
class BaseGameInfo;
// Callback for msgs.
using Msg_callback = void (*)(Exult_server::Msg_type id,
                              const unsigned char *data, int datalen, void *client);

#ifndef _WIN32
#	define C_EXPORT extern "C"
#else
#	define C_EXPORT extern "C" __declspec(dllexport)
#endif

class ExultStudio {
private:
	char            *glade_path;    // Where our .glade file is.
	char            *css_path;      // Where our .css file is.
	GtkWidget       *app;
	GtkBuilder      *app_xml;
	GtkCssProvider  *css_provider;
	char            *static_path;
	char            *image_editor;
	char            *default_game;
	guint32         background_color;
	static ExultStudio  *self;
	// Shape scaling
	int             shape_scale;    // Zoom half-units : 100% is 2, 150% is 3 ...
	bool            shape_bilinear; // True is Bilinear, False is Nearest.
	GtkWidget       *shape_zlabel;  // Zoom shape label, set to 50 * shape_scale.
	GtkWidget       *shape_zup;     // Zoom up arrow.
	GtkWidget       *shape_zdown;   // Zoom down arrow.
	static gboolean on_app_key_press(
	    GtkEntry *entry, GdkEventKey *event, gpointer user_data);
	// Modified one of the .dat's?
	bool            shape_info_modified, shape_names_modified;
	bool            npc_modified;
	Shape_file_set      *files;     // All the shape files.
	std::vector<GtkWindow *> group_windows; // All 'group' windows.
	Shape_file_info     *curfile;   // Current browser file info.
	Shape_file_info     *vgafile;   // Main 'shapes.vga'.
	Shape_file_info     *facefile;  // 'faces.vga'.
	Shape_file_info     *fontfile;  // 'font.vga'.
	Shape_file_info     *gumpfile;  // 'gumps.vga'.
	Shape_file_info     *spritefile;    // 'sprites.vga'.
	Shape_file_info     *paperdolfile;  // 'paperdol.vga'.
	Object_browser      *browser;
	std::unique_ptr<unsigned char[]> palbuf;    // 3*256 rgb's, each 0-63.
	// Barge editor:
	GtkWidget       *bargewin;// Barge window.
	int             barge_ctx;
	guint           barge_status_id;
	// Egg editor:
	GtkWidget       *eggwin;// Egg window.
	Shape_single    *egg_monster_single;
	Shape_single    *egg_missile_single;
	int             egg_ctx;
	guint           egg_status_id;
	// Npc editor:
	GtkWidget       *npcwin;
	Shape_single    *npc_single, *npc_face_single;
	int             npc_ctx;
	guint           npc_status_id;
	// Object editor:
	GtkWidget       *objwin;
	Shape_single    *obj_single;
	// Container editor:
	GtkWidget       *contwin;
	Shape_single    *cont_single;
	// Shape info. editor:
	GtkWidget       *shapewin;
	Shape_single    *shape_single, *gump_single,
	                *body_single, *explosion_single;
	Shape_single    *weapon_family_single, *weapon_projectile_single;
	Shape_single    *ammo_family_single, *ammo_sprite_single;
	Shape_single    *cntrules_shape_single;
	Shape_single    *frameflags_frame_single, *effhps_frame_single,
	                *framenames_frame_single, *frameusecode_frame_single;
	Shape_single    *objpaperdoll_wframe_single, *objpaperdoll_spotframe_single;
	Shape_single    *brightness_frame_single, *warmth_frame_single;
	Shape_single    *npcpaperdoll_aframe_single, *npcpaperdoll_atwohanded_single,
	                *npcpaperdoll_astaff_single, *npcpaperdoll_bframe_single,
	                *npcpaperdoll_hframe_single, *npcpaperdoll_hhelm_single;
	Shape_single    *objpaperdoll_frame0_single, *objpaperdoll_frame1_single,
	                *objpaperdoll_frame2_single, *objpaperdoll_frame3_single;
	GtkWidget       *equipwin;
	// Map locator:
	Locator         *locwin;
	// Combo editor:
	Combo_editor    *combowin;
	// Compile window:
	GtkWidget       *compilewin;
	Exec_box        *compile_box;
	// Usecode browser:
	Usecode_browser *ucbrowsewin;
	// Game info. editor:
	GtkWidget       *gameinfowin;
	// Game info. editor:
	GtkNotebook     *mainnotebook;
	// Which game type:
	Exult_Game game_type;
	bool expansion, sibeta;
	int curr_game;  // Which game is loaded
	int curr_mod;   // Which mod is loaded, or -1 for none
	std::string game_encoding;  // Character set for current game/mod.
	// Server data.
	int         server_socket;
	gint            server_input_tag;
	Msg_callback        waiting_for_server;
	void            *waiting_client;
	std::map<std::string, int> misc_name_map;
	// Window size at close.
	int w_at_close;
	int h_at_close;

public:
	ExultStudio(int argc, char **argv);
	~ExultStudio();
	bool okay_to_close();
	int get_shape_scale() { return shape_scale; }
	bool get_shape_bilinear() { return shape_bilinear; }
	GtkWidget *get_widget(GtkBuilder *xml, const char *name) {
		return GTK_WIDGET(gtk_builder_get_object(xml, name));
	}
	GtkWidget *get_widget(const char *name) {
		return get_widget(app_xml, name);
	}
	static ExultStudio *get_instance() {
		return self;
	}
	int find_misc_name(const char *id) const;
	int add_misc_name(const char *id);
	GtkBuilder *get_xml() {
		return app_xml;
	}
	int get_server_socket() const {
		return server_socket;
	}
	guint32 get_background_color() const {
		return background_color;
	}
	void set_background_color(const guint32 color) {
		background_color = color;
	}
	const char *get_shape_name(int shnum);
	const char *get_image_editor() {
		return image_editor;
	}
	Shape_file_set *get_files() {
		return files;
	}
	Object_browser *get_browser() {
		return browser;
	}
	unsigned char *get_palbuf() {
		return palbuf.get();
	}
	Shape_file_info *get_vgafile() const { // 'shapes.vga'.
		return vgafile;
	}
	Combo_editor *get_combowin() {
		return combowin;
	}
	void set_msg_callback(Msg_callback cb, void *client) {
		waiting_for_server = cb;
		waiting_client = client;
	}
	Shape_group_file *get_cur_groups();
	void set_browser(const char *name, Object_browser *obj);
	bool has_focus();       // Any of our windows has focus?

	void create_new_game(const char *dir);
	void new_game();
	Object_browser  *create_browser(const char *fname);
	void set_game_path(const std::string &gamename,
	                   const std::string &modname = "");
	void setup_file_list();
	void save_all();        // Write out everything.
	bool need_to_save();        // Anything modified?
	void write_map();
	void read_map();
	void write_shape_info(bool force = false);
	void reload_usecode();
	void set_play(gboolean play);
	void set_tile_grid(gboolean grid);
	void set_edit_lift(int lift);
	void set_hide_lift(int lift);
	void set_edit_terrain(gboolean terrain);
	void set_edit_mode(int md);
	void show_unused_shapes(const unsigned char *data, int datalen);
	// Open/create shape files:
	Shape_file_info *open_shape_file(const char *basename);
	void new_shape_file(bool single);
	static void create_shape_file(const char *pathname, gpointer udata);
	// Groups:
	void setup_groups();
	void setup_group_controls();
	void add_group();
	void del_group();
	void groups_changed(GtkTreeModel *model, GtkTreePath *path,
	                    GtkTreeIter *loc, bool value = false);
	void open_group_window();
	void open_builtin_group_window();
	void open_group_window(Shape_group *grp);
	void close_group_window(GtkWidget *grpwin);
	void save_groups();
	bool groups_modified();
	void update_group_windows(Shape_group *grp);
	// Objects:
	void open_obj_window(unsigned char *data, int datalen);
	void close_obj_window();
	int init_obj_window(unsigned char *data, int datalen);
	int save_obj_window();
	void rotate_obj();
	// Containers:
	void open_cont_window(unsigned char *data, int datalen);
	void close_cont_window();
	int init_cont_window(unsigned char *data, int datalen);
	int save_cont_window();
	void rotate_cont();
	// Barges:
	void open_barge_window(unsigned char *data = nullptr, int datalen = 0);
	void close_barge_window();
	int init_barge_window(unsigned char *data, int datalen);
	int save_barge_window();
	// Eggs:
	void open_egg_window(unsigned char *data = nullptr, int datalen = 0);
	void close_egg_window();
	int init_egg_window(unsigned char *data, int datalen);
	int save_egg_window();
	// NPC's:
	void open_npc_window(unsigned char *data = nullptr, int datalen = 0);
	void close_npc_window();
	void init_new_npc();
	int init_npc_window(unsigned char *data, int datalen);
	int save_npc_window();
	void update_npc(); // updates the npc browser if it is open
	static void schedule_btn_clicked(GtkWidget *btn, gpointer data);
	// Shapes:
	GdkPixbuf *shape_image(     // The GdkPixbuf should be g_object_unrefed.
	    Vga_file *shpfile, int shnum, int frnum, bool transparent);
	void init_equip_window(int recnum);
	void save_equip_window();
	void open_equip_window(int recnum);
	void close_equip_window();
	void new_equip_record();
	void set_shape_notebook_frame(int frnum);
	void init_shape_notebook(const Shape_info &info, GtkWidget *book,
	                         int shnum, int frnum);
	void save_shape_notebook(Shape_info &info, int shnum, int frnum);
	void open_shape_window(int shnum, int frnum,
	                       Shape_file_info *file_info,
	                       const char *shname, Shape_info *info = nullptr);
	void save_shape_window();
	void close_shape_window();
	void create_zoom_controls();
	static void on_zoom_bilinear(GtkToggleButton *btn, gpointer user_data);
	static void on_zoom_up(GtkButton *btn, gpointer user_data);
	static void on_zoom_down(GtkButton *btn, gpointer user_data);
	// Map locator.
	void open_locator_window();
	// Combo editor.
	void open_combo_window();   // Combo-object editor.
	void save_combos();
	void close_combo_window();
	// Compile.
	void open_compile_window();
	void compile(bool if_needed = false);
	void halt_compile();
	void close_compile_window();
	void run();
	// Maps.
	void new_map_dialog();
	void setup_maps_list();
	// Usecode browser.
	const char *browse_usecode(bool want_objfun = false);
	// Games.
	void open_game_dialog(bool createmod = false);

	// Game info.
	void set_game_information();
	void show_charset();

	bool send_to_server(Exult_server::Msg_type id,
	                    unsigned char *data = nullptr, int datalen = 0);
	void read_from_server();
	bool connect_to_server();
	void disconnect_from_server();
	// Message from Exult.
	void info_received(unsigned char *data, int datalen);
	void set_edit_menu(bool sel, bool clip);
	// Preferences.
	static gboolean on_prefs_background_expose_event(
	    GtkWidget *widget, cairo_t *cairo, gpointer data);
	void open_preferences();
	void save_preferences();
	// GTK/CSS utils:
	void reload_css();
	// GTK/Glade utils:
	bool get_toggle(const char *name);
	void set_toggle(const char *name, bool val, bool sensitive = true);
	void set_bit_toggles(const char **names, int num, unsigned int bits);
	unsigned int get_bit_toggles(const char **names, int num);
	int get_optmenu(const char *name);
	void set_optmenu(const char *name, int val, bool sensitive = true);
	int get_spin(const char *name);
	void set_spin(const char *name, int val, bool sensitive = true);
	void set_spin(const char *name, int low, int high);
	void set_spin(const char *name, int val, int low, int high);
	int get_num_entry(const char *name);
	static int get_num_entry(GtkWidget *field, int if_empty);
	const gchar *get_text_entry(const char *name);
	void set_entry(const char *name, int val, bool hex = false,
	               bool sensitive = true);
	void set_entry(const char *name, const char *val, bool sensitive = true);
	guint set_statusbar(const char *name, int context, const char *msg);
	void remove_statusbar(const char *name, int context, guint msgid);
	void set_button(const char *name, const char *text);
	void set_visible(const char *name, bool vis);
	void set_sensitive(const char *name, bool tf);
	int prompt(const char *msg, const char *choice0,
	           const char *choice1 = nullptr, const char *choice2 = nullptr);
	int find_palette_color(int r, int g, int b);
	Exult_Game get_game_type() const {
		return game_type;
	}
	bool has_expansion() const {
		return expansion;
	}
	bool is_si_beta() {
		return sibeta;
	}
	void set_shapeinfo_modified() {
		shape_info_modified = true;
	}
	void set_npc_modified() {
		npc_modified = true;
	}
	const std::string &get_encoding() const {
		return game_encoding;
	}
	void set_encoding(const std::string &enc) {
		game_encoding = enc;
	}
	BaseGameInfo *get_game() const;
};

// Utilities:
namespace EStudio {
int Prompt(const char *msg, const char *choice0,
           const char *choice1 = nullptr, const char *choice2 = nullptr);
void Alert(const char *msg, ...) ATTR_PRINTF(1, 2);
GtkWidget *Add_menu_item(GtkWidget *menu,
                         const char *label = nullptr,
                         GCallback func = nullptr,
                         gpointer func_data = nullptr,
                         GSList *group = nullptr);
GtkWidget *Create_arrow_button(GtkArrowType dir,
                               GCallback clicked,
                               gpointer func_data);
bool Copy_file(const char *src, const char *dest);
}

inline int ZoomUp( int size ) {
	return ( (size * (ExultStudio::get_instance()->get_shape_scale())) / 2 );
}
inline int ZoomDown( int size ) {
	return ( (size * 2) / (ExultStudio::get_instance()->get_shape_scale()) );
}
inline int ZoomGet() {
	return (ExultStudio::get_instance()->get_shape_scale());
}

std::string          convertToUTF8(const char *src_str, const char *enc);
inline std::string   convertToUTF8(const char *src_str) {
	return   convertToUTF8(src_str,
	                       ExultStudio::get_instance()->get_encoding().c_str());
}
std::string        convertFromUTF8(const char *src_str, const char *enc);
inline std::string convertFromUTF8(const char *src_str) {
	return convertFromUTF8(src_str,
	                       ExultStudio::get_instance()->get_encoding().c_str());
}

struct ExultRgbCmap {
	guint32 colors[256];
};

#endif
