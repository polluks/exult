/*
Copyright (C) 2001 The Exult Team

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

#ifndef COMBATSTATS_GUMP_H
#define COMBATSTATS_GUMP_H

#include "Gump.h"
#include "misc_buttons.h"
#include "Face_button.h"
#include "ignore_unused_variable_warning.h"

class Actor;

/*
 *  A rectangular area showing party combat statistics:
 */
class CombatStats_gump : public Gump {
	UNREPLICATABLE_CLASS_I(CombatStats_gump, Gump())

public:
	CombatStats_gump(int initx, int inity);
	virtual ~CombatStats_gump();
	// Add object.
	virtual int add(Game_object *obj, int mx = -1, int my = -1,
	                int sx = -1, int sy = -1, bool dont_check = false,
	                bool combine = false) {
		ignore_unused_variable_warning(obj, mx, my, sx, sy, dont_check, combine);
		return 0;    // Can't drop onto it.
	}
	// Paint it and its contents.
	virtual void paint();

private:
	Actor *party[9];
	int party_size;
};

#endif
