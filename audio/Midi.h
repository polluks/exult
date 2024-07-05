/*
 *  Copyright (C) 2000-2022  The Exult Team
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

#ifndef MIDI_H
#define MIDI_H

#include "XMidiFile.h"
#include "common_types.h"
#include "exceptions.h"
#include "exult_constants.h"
#include "fnames.h"

#include <string>
#include <vector>

class MidiDriver;

namespace Pentagram {
	class AudioSample;
}

//---- MyMidiPlayer -----------------------------------------------------------

class MyMidiPlayer : nonreplicatable {
	friend class SoundTester;

public:
	enum TimbreLibrary {
		TIMBRE_LIB_GM       = 0,    // General Midi/GS output mode
		TIMBRE_LIB_INTRO    = 1,    // Intro
		TIMBRE_LIB_MAINMENU = 2,    // Main Menu
		TIMBRE_LIB_GAME     = 3,    // In Game
		TIMBRE_LIB_ENDGAME  = 4     // Endgame
	};

	MyMidiPlayer();
	~MyMidiPlayer();

	void destroyMidiDriver();

	enum ForceType {
		Force_None,
		Force_Midi,
		Force_Ogg
	};

	bool start_music(
			int num, bool repeat = false, ForceType force = Force_None,
			std::string flex = MAINMUS);
	bool start_music(
			std::string fname, int num, bool repeat = false,
			ForceType force = Force_None);
	void stop_music(bool quitting = false);

	bool is_track_playing(int num);
	int  get_current_track() const;

	bool is_repeating() {
		return repeating;
	}

	void set_repeat(bool newrepeat);

	void set_timbre_lib(TimbreLibrary lib);

	TimbreLibrary get_timbre_lib() const {
		return timbre_lib;
	}

	void set_midi_driver(const std::string& desired_driver, bool use_oggs);

	std::string get_midi_driver() {
		return midi_driver_name;
	}

	bool get_ogg_enabled() const {
		return ogg_enabled;
	}

	void set_music_conversion(int conv);

	int get_music_conversion() const {
		return music_conversion;
	}

	//! Set Volume of Current Midi device.
	//!
	//! If Music is disabled this does nothing
	//!
	//! \param vol new volume to set [0-100]
	//! \param saveconfig Save the new value to exult.cfg
	void SetMidiMusicVolume(int vol, bool savetoconfig);
	//! Get volume of current Midi device volume
	//! \return The current midi device volume. This will return 0 if Music is disabled
	int GetMidiMusicVolume();

#ifdef ENABLE_MIDISFX
	void start_sound_effect(int num);
	void stop_sound_effects();

	void set_effects_conversion(int conv);

	int get_effects_conversion() const {
		return effects_conversion;
	}
#endif

	void produceSamples(sint16* stream, uint32 bytes);
	void load_timbres();
	bool is_mt32() const;     // Check for true mt32, mt32emu or fakemt32
	bool is_adlib() const;    // Check for adlib

	// ! Can music be played at all
	bool can_play() {
		return ogg_enabled || midi_driver || init_device(true);
	}

	bool can_play_midi() {
		if (!midi_driver) {
			init_device(true);
		}
		return midi_driver != nullptr;
	}

private:
	bool repeating     = false;
	int  current_track = -1;

	std::string                 midi_driver_name = "default";
	std::shared_ptr<MidiDriver> midi_driver      = nullptr;
	bool                        initialized      = false;
	bool                        init_device(bool timbre_load = false);

	TimbreLibrary timbre_lib = TIMBRE_LIB_GM;
	std::string   timbre_lib_filename;
	int           timbre_lib_index   = 0;
	int           timbre_lib_game    = NONE;
	int           music_conversion   = XMIDIFILE_CONVERT_MT32_TO_GM;
	int           effects_conversion = XMIDIFILE_CONVERT_GS127_TO_GS;
	int           setup_timbre_for_track(std::string& str);

	// Ogg Stuff
	bool        ogg_enabled     = false;
	sint32      ogg_instance_id = -1;
	int         ogg_volume      = 100;
	std::string oggfailed;

public:
	//! Set Volume for Ogg Music Playback. This will
	//! \param vol new volume to set [0-100]
	//! \param saveconfig Save the new value to exult.cfg
	void SetOggMusicVolume(int vol, bool savetoconfig);

	//! Get current Set OggMusic volume
	//! \return The current Ogg music volume. This will return the correct value
	//!  even if Ogg music is disabled.
	int GetOggMusicVolume() {
		return ogg_volume;
	}

	const std::string& GetOggFailed() {
		return oggfailed;
	}

	bool ogg_is_playing() const;

private:
	bool ogg_play_track(const std::string& filename, int num, bool repeat);
	void ogg_stop_track();
	void ogg_set_repeat(bool newrepeat);

	void ogg_mix(sint16* stream, uint32 bytes);

private:
	uint16 looping_egg_count;

public:
	void set_egg_count(uint16 new_count) {
		looping_egg_count = new_count;
	}

	uint16 get_egg_count() const {
		return looping_egg_count;
	}
};

std::unique_ptr<IDataSource> open_music_flex(const std::string& flex, int num);

#endif
