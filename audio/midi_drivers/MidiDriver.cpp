/*
Copyright (C) 2005  The Pentagram Team
Copyright (C) 2008-2022  The ExultTeam

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

#include "pent_include.h"

#include "MidiDriver.h"

#include "ALSAMidiDriver.h"
#include "Configuration.h"
#include "CoreAudioMidiDriver.h"
#include "CoreMidiDriver.h"
#include "FMOplMidiDriver.h"
#include "FluidSynthMidiDriver.h"
#include "MT32EmuMidiDriver.h"
#include "TimidityMidiDriver.h"
#include "UnixSeqMidiDriver.h"
#include "WindowsMidiDriver.h"

#include <vector>

static std::shared_ptr<MidiDriver> Disabled_CreateInstance() {
	return nullptr;
}

static const MidiDriver::MidiDriverDesc Disabled_desc
		= MidiDriver::MidiDriverDesc("Disabled", Disabled_CreateInstance);

static std::vector<const MidiDriver::MidiDriverDesc*> midi_drivers;

static void InitMidiDriverVector() {
	if (!midi_drivers.empty()) {
		return;
	}

#ifdef USE_FMOPL_MIDI
	midi_drivers.push_back(FMOplMidiDriver::getDesc());
#endif
#ifdef USE_CORE_AUDIO_MIDI
	midi_drivers.push_back(CoreAudioMidiDriver::getDesc());
#endif
#ifdef USE_CORE_MIDI
	midi_drivers.push_back(CoreMidiDriver::getDesc());
#endif
#ifdef USE_WINDOWS_MIDI
	midi_drivers.push_back(WindowsMidiDriver::getDesc());
#endif
#ifdef USE_TIMIDITY_MIDI
	midi_drivers.push_back(TimidityMidiDriver::getDesc());
#endif
#ifdef USE_MT32EMU_MIDI
	midi_drivers.push_back(MT32EmuMidiDriver::getDesc());
#endif
#ifdef USE_ALSA_MIDI
	midi_drivers.push_back(ALSAMidiDriver::getDesc());
#endif
#ifdef USE_UNIX_SEQ_MIDI
	midi_drivers.push_back(UnixSeqMidiDriver::getDesc());
#endif
#ifdef USE_FLUIDSYNTH_MIDI
	midi_drivers.push_back(FluidSynthMidiDriver::getDesc());
#endif

	midi_drivers.push_back(&Disabled_desc);
}

// Get the number of devices
int MidiDriver::getDriverCount() {
	InitMidiDriverVector();
	return midi_drivers.size();
}

// Get the name of a driver
std::string MidiDriver::getDriverName(uint32 index) {
	InitMidiDriverVector();

	if (index >= midi_drivers.size()) {
		return "";
	}

	return midi_drivers[index]->name;
}

// Create an Instance of a MidiDriver
std::shared_ptr<MidiDriver> MidiDriver::createInstance(
		const std::string& desired_driver, uint32 sample_rate, bool stereo) {
	InitMidiDriverVector();

	std::shared_ptr<MidiDriver> new_driver = nullptr;

	const char* drv = desired_driver.c_str();

	// Has the config file specified disabled midi?
	if (Pentagram::strcasecmp(drv, "disabled") != 0) {
		// Ok, it hasn't so search for the driver
		for (const auto* midi_driver : midi_drivers) {
			// Found it (case insensitive)
			if (!Pentagram::strcasecmp(drv, midi_driver->name)) {
				pout << "Trying config specified Midi driver: `"
					 << midi_driver->name << "'" << std::endl;

				new_driver = midi_driver->createInstance();
				if (new_driver) {
					if (new_driver->initMidiDriver(sample_rate, stereo)) {
						pout << "Failed!" << std::endl;
						new_driver = nullptr;
					} else {
						pout << "Success!" << std::endl;
						break;
					}
				}
			}
		}

		// Uh oh, we didn't manage to load a driver!
		// Search for the first working one
		if (!new_driver) {
			for (const auto* midi_driver : midi_drivers) {
				pout << "Trying: `" << midi_driver->name << "'" << std::endl;

				new_driver = midi_driver->createInstance();

				if (new_driver) {
					// Got it
					if (!new_driver->initMidiDriver(sample_rate, stereo)) {
						pout << "Success!" << std::endl;
						break;
					}

					pout << "Failed!" << std::endl;

					// Oh well, try the next one
					new_driver = nullptr;
				}
			}
		}
	} else {
		new_driver = nullptr;    // silence :-)
	}
	pout << "Midi Output: " << (new_driver != nullptr ? "Enabled" : "Disabled")
		 << std::endl;

	return new_driver;
}

std::string MidiDriver::getConfigSetting(
		const std::string& name, const std::string& defaultval) {
	std::string key = "config/audio/midi/";
	key += name;
	std::string val;
	config->value(key, val, defaultval);

	return val;
}
