/*
 *  Copyright (C) 2009-2022 Exult Team
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA  02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "scale_2xSaI.h"

#include "common_types.h"
#include "imagewin.h"
#include "manip.h"

#include <cstring>

//
// 2xSaI Filtering
//
void Image_window::show_scaled8to16_2xSaI(
		int x, int y, int w, int h    // Area to show.
) {
	const Manip8to16 manip(
			paletted_surface->format->palette->colors, inter_surface->format);
	Scale_2xSaI<unsigned char, uint16, Manip8to16>(
			static_cast<uint8*>(draw_surface->pixels), x, y, w, h,
			ibuf->line_width, ibuf->height,
			static_cast<uint16*>(inter_surface->pixels),
			inter_surface->pitch / inter_surface->format->BytesPerPixel, manip);
}

void Image_window::show_scaled8to555_2xSaI(
		int x, int y, int w, int h    // Area to show.
) {
	const Manip8to555 manip(
			paletted_surface->format->palette->colors, inter_surface->format);
	Scale_2xSaI<unsigned char, uint16, Manip8to555>(
			static_cast<uint8*>(draw_surface->pixels), x + guard_band,
			y + guard_band, w, h, ibuf->line_width, ibuf->height,
			static_cast<uint16*>(inter_surface->pixels),
			inter_surface->pitch / inter_surface->format->BytesPerPixel, manip);
}

void Image_window::show_scaled8to565_2xSaI(
		int x, int y, int w, int h    // Area to show.
) {
	const Manip8to565 manip(
			paletted_surface->format->palette->colors, inter_surface->format);
	Scale_2xSaI<unsigned char, uint16, Manip8to565>(
			static_cast<uint8*>(draw_surface->pixels), x + guard_band,
			y + guard_band, w, h, ibuf->line_width, ibuf->height,
			static_cast<uint16*>(inter_surface->pixels),
			inter_surface->pitch / inter_surface->format->BytesPerPixel, manip);
}

void Image_window::show_scaled8to32_2xSaI(
		int x, int y, int w, int h    // Area to show.
) {
	const Manip8to32 manip(
			paletted_surface->format->palette->colors, inter_surface->format);
	Scale_2xSaI<unsigned char, uint32, Manip8to32>(
			static_cast<uint8*>(draw_surface->pixels), x + guard_band,
			y + guard_band, w, h, ibuf->line_width, ibuf->height,
			static_cast<uint32*>(inter_surface->pixels),
			inter_surface->pitch / inter_surface->format->BytesPerPixel, manip);
}

//
// Super2xSaI Filtering
//
void Image_window::show_scaled8to16_Super2xSaI(
		int x, int y, int w, int h    // Area to show.
) {
	const Manip8to16 manip(
			paletted_surface->format->palette->colors, inter_surface->format);
	Scale_Super2xSaI<unsigned char, uint16, Manip8to16>(
			static_cast<uint8*>(draw_surface->pixels), x + guard_band,
			y + guard_band, w, h, ibuf->line_width, ibuf->height + guard_band,
			static_cast<uint16*>(inter_surface->pixels),
			inter_surface->pitch / inter_surface->format->BytesPerPixel, manip);
}

void Image_window::show_scaled8to555_Super2xSaI(
		int x, int y, int w, int h    // Area to show.
) {
	const Manip8to555 manip(
			paletted_surface->format->palette->colors, inter_surface->format);
	Scale_Super2xSaI<unsigned char, uint16, Manip8to555>(
			static_cast<uint8*>(draw_surface->pixels), x + guard_band,
			y + guard_band, w, h, ibuf->line_width, ibuf->height + guard_band,
			static_cast<uint16*>(inter_surface->pixels),
			inter_surface->pitch / inter_surface->format->BytesPerPixel, manip);
}

void Image_window::show_scaled8to565_Super2xSaI(
		int x, int y, int w, int h    // Area to show.
) {
	const Manip8to565 manip(
			paletted_surface->format->palette->colors, inter_surface->format);
	Scale_Super2xSaI<unsigned char, uint16, Manip8to565>(
			static_cast<uint8*>(draw_surface->pixels), x + guard_band,
			y + guard_band, w, h, ibuf->line_width, ibuf->height + guard_band,
			static_cast<uint16*>(inter_surface->pixels),
			inter_surface->pitch / inter_surface->format->BytesPerPixel, manip);
}

void Image_window::show_scaled8to32_Super2xSaI(
		int x, int y, int w, int h    // Area to show.
) {
	const Manip8to32 manip(
			paletted_surface->format->palette->colors, inter_surface->format);
	Scale_Super2xSaI<unsigned char, uint32, Manip8to32>(
			static_cast<uint8*>(draw_surface->pixels), x + guard_band,
			y + guard_band, w, h, ibuf->line_width, ibuf->height + guard_band,
			static_cast<uint32*>(inter_surface->pixels),
			inter_surface->pitch / inter_surface->format->BytesPerPixel, manip);
}

//
// SuperEagle Filtering
//
void Image_window::show_scaled8to16_SuperEagle(
		int x, int y, int w, int h    // Area to show.
) {
	const Manip8to16 manip(
			paletted_surface->format->palette->colors, inter_surface->format);
	Scale_SuperEagle<unsigned char, uint16, Manip8to16>(
			static_cast<uint8*>(draw_surface->pixels), x + guard_band,
			y + guard_band, w, h, ibuf->line_width, ibuf->height + guard_band,
			static_cast<uint16*>(inter_surface->pixels),
			inter_surface->pitch / inter_surface->format->BytesPerPixel, manip);
}

void Image_window::show_scaled8to555_SuperEagle(
		int x, int y, int w, int h    // Area to show.
) {
	const Manip8to555 manip(
			paletted_surface->format->palette->colors, inter_surface->format);
	Scale_SuperEagle<unsigned char, uint16, Manip8to555>(
			static_cast<uint8*>(draw_surface->pixels), x + guard_band,
			y + guard_band, w, h, ibuf->line_width, ibuf->height + guard_band,
			static_cast<uint16*>(inter_surface->pixels),
			inter_surface->pitch / inter_surface->format->BytesPerPixel, manip);
}

void Image_window::show_scaled8to565_SuperEagle(
		int x, int y, int w, int h    // Area to show.
) {
	const Manip8to565 manip(
			paletted_surface->format->palette->colors, inter_surface->format);
	Scale_SuperEagle<unsigned char, uint16, Manip8to565>(
			static_cast<uint8*>(draw_surface->pixels), x + guard_band,
			y + guard_band, w, h, ibuf->line_width, ibuf->height + guard_band,
			static_cast<uint16*>(inter_surface->pixels),
			inter_surface->pitch / inter_surface->format->BytesPerPixel, manip);
}

void Image_window::show_scaled8to32_SuperEagle(
		int x, int y, int w, int h    // Area to show.
) {
	const Manip8to32 manip(
			paletted_surface->format->palette->colors, inter_surface->format);
	Scale_SuperEagle<unsigned char, uint32, Manip8to32>(
			static_cast<uint8*>(draw_surface->pixels), x + guard_band,
			y + guard_band, w, h, ibuf->line_width, ibuf->height + guard_band,
			static_cast<uint32*>(inter_surface->pixels),
			inter_surface->pitch / inter_surface->format->BytesPerPixel, manip);
}
