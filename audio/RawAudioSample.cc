/*
Copyright (C) 2005 The Pentagram team
Copyright (C) 2010-2022 The Exult Team

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
#include "RawAudioSample.h"
#include "databuf.h"
#include <cstring>
#include <new>

namespace Pentagram {

RawAudioSample::RawAudioSample(std::unique_ptr<uint8[]> buffer_, uint32 size_, uint32 rate_,
							   bool signeddata_, bool stereo_)
	: AudioSample(std::move(buffer_), size_), signeddata(signeddata_)
{
	sample_rate = rate_;
	bits = 8;
	stereo = stereo_;
	frame_size = 512;
	decompressor_size = sizeof(RawDecompData);
	decompressor_align = alignof(RawDecompData);
	length = size_;
	start_pos = 0;
	byte_swap = false;
}

void RawAudioSample::initDecompressor(void *DecompData) const
{
	auto *decomp = new (DecompData) RawDecompData;
	decomp->pos = start_pos;
}

void RawAudioSample::freeDecompressor(void *DecompData) const
{
	auto *decomp = static_cast<RawDecompData *>(DecompData);
	decomp->~RawDecompData();
}

uint32 RawAudioSample::decompressFrame(void *DecompData, void *samples) const
{
	auto *decomp = static_cast<RawDecompData *>(DecompData);

	if (decomp->pos == buffer_size) return 0;

	uint32 count = frame_size;
	if (decomp->pos + count > buffer_size)
		count = buffer_size - decomp->pos;

	count &= bits==16?0xFFFFFFFE:0xFFFFFFFF;

	if (count == 0) return 0;

	// 8 bit unsigned, or 16 Bit signed
	if ((!signeddata && bits==8) || (signeddata && bits==16 && !byte_swap)) {	
		std::memcpy(samples, buffer.get()+decomp->pos, count);
	// 8 bit signed
	} else if (bits == 8) {
		auto *dest = static_cast<uint8*>(samples);
		uint8 *end =  static_cast<uint8*>(samples)+count;
		const uint8 *src = buffer.get() + decomp->pos;
		while (dest != end) {
			*dest++ = *src++ + 128;
		}
	}
	// 16 bit signed with byte swap
	else if (signeddata && bits==16 && byte_swap) {
		auto *dest = static_cast<sint16*>(samples);
		sint16 *end =  static_cast<sint16*>(samples)+count/2;
		const uint8 *src = buffer.get() + decomp->pos;
		while (dest != end) {
			sint16 s;
			reinterpret_cast<uint8*>(&s)[1] = *src++;
			reinterpret_cast<uint8*>(&s)[0] = *src++;
			*dest++ = s;
		}
	}
	// 16 bit unsigned
	else if (!signeddata && bits==16 && !byte_swap) {
		auto *dest = static_cast<sint16*>(samples);
		sint16 *end =  static_cast<sint16*>(samples)+count/2;
		const uint8 *src = buffer.get() + decomp->pos;
		auto Read2 = [&src]() {
			uint16 val;
			std::memcpy(&val, src, sizeof(uint16));
			src += sizeof(uint16);
			return val;
		};
		while (dest != end) {
			*dest++ = Read2() - 32768;
		}
	}
	// 16 bit unsigned with byte swap
	else if (!signeddata && bits==16 && byte_swap) {
		auto *dest = static_cast<sint16*>(samples);
		sint16 *end =  static_cast<sint16*>(samples)+count/2;
		const uint8 *src = buffer.get() + decomp->pos;
		while (dest != end) {
			uint16 s;
			reinterpret_cast<uint8*>(&s)[1] = *src++;
			reinterpret_cast<uint8*>(&s)[0] = *src++;
			*dest++ = s - 32768;
		}
	}

	decomp->pos += count;
	return count;
}



}
