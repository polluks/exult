/*
 *  Copyright (C) 2000-2013  The Exult Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef MENULIST_H
#define MENULIST_H

#include <memory>
#include <string>
#include <vector>
#include "SDL_events.h"

class Game_window;
class Shape_frame;
class Font;
class Mouse;

class MenuObject {
private:
	bool has_id = false;
	int id = 0;
public:
	int x, y, x1, y1, x2, y2;
	bool selected;
	bool dirty;

	virtual ~MenuObject() = default;

	void set_selected(bool sel) {
		dirty |= (selected != sel);
		selected = sel;
	}
	bool is_selected() {
		return selected;
	}
	bool is_mouse_over(int mx, int my) {
		return (mx >= x1) && (mx <= x2) && (my >= y1) && (my <= y2);
	}

	virtual void paint(Game_window *gwin) = 0;
	virtual bool handle_event(SDL_Event &event) = 0;

	bool get_has_id() const {
		return has_id;
	}
	int get_id() const {
		return id;
	}
	void set_id(int newid) {
		has_id = true;
		id = newid;
	}
	//Don't think it will ever be needed, but:
	void delete_id() {
		has_id = false;
		id = 0;
	}
};

class MenuEntry: public MenuObject {
public:
	Shape_frame *frame_on, *frame_off;
	MenuEntry(Shape_frame *on, Shape_frame *off, int xpos, int ypos);

	void paint(Game_window *gwin) override;
	bool handle_event(SDL_Event &event) override;
};

class MenuTextObject: public MenuObject {
public:
	Font *font;
	Font *font_on;
	std::string text;
	MenuTextObject(Font *fnt, Font *fnton, std::string txt)
		: font(fnt), font_on(fnton), text(std::move(txt)) {
	}

	virtual int get_height() {
		return y2 - y1;
	}
	void paint(Game_window *gwin) override = 0;
	bool handle_event(SDL_Event &event) override = 0;
};

class MenuTextEntry: public MenuTextObject {
private:
	bool enabled;
public:
	MenuTextEntry(Font *fnton, Font *fnt, const char *txt, int xpos, int ypos);

	void paint(Game_window *gwin) override;
	bool handle_event(SDL_Event &event) override;
	bool is_enabled() const {
		return enabled;
	}
	void set_enabled(bool en) {
		enabled = en;
	}
};

class MenuGameEntry: public MenuTextEntry {
private:
	Shape_frame *sfxicon;
public:
	MenuGameEntry(Font *fnton, Font *fnt, const char *txt, Shape_frame *sfx,
	              int xpos, int ypos);

	void paint(Game_window *gwin) override;
	bool handle_event(SDL_Event &event) override;
};

class MenuTextChoice: public MenuTextObject {
private:
	std::vector<std::string> choices;
	int choice;
	int max_choice_width;
public:
	MenuTextChoice(Font *fnton, Font *fnt, const char *txt, int xpos, int ypos);
	void add_choice(const char *s);
	int get_choice() {
		return choice;
	}
	void set_choice(int c) {
		choice = c;
	}

	void paint(Game_window *gwin) override;
	bool handle_event(SDL_Event &event) override;
};

class MenuList {
private:
	std::vector<std::unique_ptr<MenuObject>> entries;
	bool selected = false;
	int selection = 0;
public:
	int add_entry(MenuObject *entry) {
		entries.emplace_back(entry);
		return entries.size() - 1;
	}
	void paint(Game_window *gwin);
	int handle_events(Game_window *gwin, Mouse *mouse);
	int get_selection() {
		return selection;
	}
	void set_selection(int sel);
	void set_selection(int x, int y);
};

#endif
