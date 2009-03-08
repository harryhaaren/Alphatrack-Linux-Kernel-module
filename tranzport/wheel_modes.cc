/*
 *   Copyright (C) 2006 Paul Davis 
 *   Copyright (C) 2007 Michael Taht
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *  
 */

#include <iostream>
#include <algorithm>
#include <cmath>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <float.h>
#include <sys/time.h>
#include <errno.h>

#if HAVE_TRANZPORT_KERNEL_DRIVER
#include <fcntl.h>
#include <poll.h>
#endif

#include <pbd/pthread_utils.h>

#include <ardour/route.h>
#include <ardour/audio_track.h>
#include <ardour/session.h>
#include <ardour/tempo.h>
#include <ardour/location.h>
#include <ardour/dB.h>

#include <tranzport_control_protocol.h>

using namespace ARDOUR;
using namespace std;
using namespace sigc;
using namespace PBD;

#include "i18n.h"

#include <pbd/abstract_ui.cc>



void
TranzportControlProtocol::next_wheel_shift_mode ()
{
switch (wheel_shift_mode) {
	case WheelShiftGain:
		wheel_shift_mode = WheelShiftPan;
		break;
	case WheelShiftPan:
		wheel_shift_mode = WheelShiftMaster;
		break;
	case WheelShiftMaster:
		wheel_shift_mode = WheelShiftGain;
		break;
	case WheelShiftMarker: // Not done yet, disabled
		wheel_shift_mode = WheelShiftGain;
		break;
	}

	show_wheel_mode ();
}

void
TranzportControlProtocol::next_wheel_mode ()
{
	switch (wheel_mode) {
	case WheelTimelineSlave:
	case WheelTimeline:
		wheel_mode = WheelScrub;
		break;
	case WheelScrub:
		wheel_mode = WheelShuttle;
		break;
	case WheelShuttle:
		wheel_mode = WheelTimelineSlave;
	}

	show_wheel_mode ();
}

void
TranzportControlProtocol::show_wheel_mode ()
{
	string text;

	switch (wheel_mode) {

		case WheelTimelineSlave:
			text = "Slav";
			break;
		case WheelTimeline:
			text = "Time";
			break;
		case WheelScrub:
			text = "Scrb";
			break;
		case WheelShuttle:
			text = "Shtl";
			break;
		}
		
	switch (wheel_shift_mode) {
		case WheelShiftGain:
			text += ":Gain";
			break;
					
		case WheelShiftPan:
			text += ":Pan ";
			break;
					
		case WheelShiftMaster:
			text += ":Mstr";
			break;
					
		case WheelShiftMarker:
			text += ":Mrkr";
			break;
		}
		
	notify(text.c_str());
}

