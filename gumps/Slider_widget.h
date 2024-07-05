/*
Copyright (C) 2024 The Exult Team

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
#ifndef SLIDER_WIDGET_H_INCLUDED
#define SLIDER_WIDGET_H_INCLUDED
#include "Gump_widget.h"

class Slider_widget;

//! Slider_Widget implements most of the functionality to create a slider like
//! used by Slider_Gump exccpt for the text display and background. Text or a
//! background needs to be drawn by the owner if desired.
class Slider_widget : public Gump_widget {
public:
	// Interface to recieve Messages from a Slider Widget
	class ICallback {
	public:
		virtual void OnSliderValueChanged(Slider_widget* sender, int newvalue)
				= 0;

		virtual ~ICallback() {}
	};

	ICallback* callback;

public:
	//! Construct a Slider Widget
	//! /param par Parent of this Widget, If it implemets ICallback, it will be
	//! used as the default callback
	//! /param width. How wide the sliding region should be.
	//!  determines how far apart the left and right buttons should be
	Slider_widget(
			Gump_Base* par, int px, int py, ShapeID sidLeft, ShapeID sidRight,
			ShapeID sidDiamond, int mival, int mxval, int step, int defval,
			int width = 64, bool logarithmic = false);

	// By default the callback is set to par by the construcor if par implements
	// ISlider_widget_callback but if not the callback can be set here
	// call with nullptr to clear the callback
	void setCallback(ICallback* newcalllback) {
		callback = newcalllback;
	}

private:
	bool logarithmic;
	int  diamondx;    // Rel. pos. where diamond is shown.
	int  diamondy;
	int  min_val, max_val;    // Max., min. values to choose from.
	int  step_val;            // Amount to step by.
	int  val;                 // Current value.
	int  prev_dragx;          // Prev. x-coord. of mouse.
	// Coords:
	int leftbtnx, rightbtnx, btny;
	int xmin, xmax, xdist;

	ShapeID      diamond;    // Diamond
	Gump_button *left, *right;

	Gump_button* pushed;

public:
	int get_val() {    // Get last value set.
		return val;
	}

	// An arrow was clicked on.
	void clicked_left_arrow();
	void clicked_right_arrow();
	void move_diamond(int dir);
	void set_val(int newval, bool recalcdiamond = true);    // Set to new value.

	TileRect get_rect() override;

	// Paint it and its contents.
	void paint() override;

	Gump_button* on_button(int mx, int my) override;

	// Handle events:
	bool mouse_down(int mx, int my, MouseButton button) override;
	bool mouse_up(int mx, int my, MouseButton button) override;
	bool mouse_drag(int mx, int my) override;
	bool key_down(int chr) override;    // Character typed.

	bool mousewheel_up(int mx, int my) override;
	bool mousewheel_down(int mx, int my) override;

	int logtolinear(int linearvalue, int base);
	int lineartolog(int logvalue, int base);

	bool is_dragging() {
		return prev_dragx != INT_MIN;
	}
};
#endif
