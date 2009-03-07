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
 *   */

#include <tranzport_common.h>

using namespace ARDOUR;
using namespace std;
using namespace sigc;
using namespace PBD;

#include "i18n.h"

#include <pbd/abstract_ui.cc>

BaseUI::RequestType LEDChange = BaseUI::new_request_type ();
BaseUI::RequestType Print = BaseUI::new_request_type ();
BaseUI::RequestType SetCurrentTrack = BaseUI::new_request_type ();

#include <tranzport_control_protocol.h>

void
TranzportControlProtocol::datawheel ()
{
  if ((buttonmask & ButtonTrackRight) || (buttonmask & ButtonTrackLeft)) {
    change_track(_datawheel);    
  } else if ((buttonmask & ButtonPrev) || (buttonmask & ButtonNext)) {
    change_marker(_datawheel);
  } else if (buttonmask & ButtonShift) {
    if (route_table[0]) {
      switch (wheel_shift_mode) {
      case WheelShiftGain: step_gain(_datawheel);
				break;
      case WheelShiftPan:
	step_pan(_datawheel);
	break;
	
      case WheelShiftMarker:
	break;
	
      case WheelShiftMaster:
	break;
	
      }
    }
  } else {
    nframes64_t start = session->transport_frame();
/* FIXME: hack to determine if our new start location is negative 
    if start >  */
    switch (wheel_mode) {
    case WheelTimelineSlave:
    case WheelTimeline:
      get_snapto(); // strikes me as latency intensive - maybe only do after 1 seconds has elapsed
      snap_to_internal(start,_datawheel,0); // start is a ref here
      session->request_locate (ZEROIFNEG(start), session->transport_rolling());    
      
      break;
      
    case WheelScrub:
      scrub ();
      break;
      
    case WheelShuttle:
      shuttle ();
      break;
    }
  }
}

void
TranzportControlProtocol::scroll ()
{
  switch(wheel_mode) {
  case WheelTimelineSlave:
  case WheelTimeline: 
    ScrollTimeline (0.1*_datawheel); break;
  case WheelScrub:
  case WheelShuttle: break;
  default: break;
  }
}

/* The only difference between scrub and shuttle modes is that
   we move slower. Not sure if this is right */

void
TranzportControlProtocol::scrub ()
{
	float speed = session->transport_speed();
	int dir = 0;
	dir = 0;

	if (_datawheel < 0) {
		dir = -1;
	} else {
		dir = 1;
	}	
	if(dir == 0) return; // not reached

	if (dir != last_wheel_dir) {
		/* changed direction, start over */
		speed = 0.1f;
		last_wheel_dir = dir; // always reduce initial movement
      		session->request_transport_speed(speed*dir);
	} else {
		session->request_transport_speed (speed + .1 * _datawheel);
	}
	
}

void
TranzportControlProtocol::shuttle ()
{
  float speed = session->transport_speed();
  if(_datawheel > 0) {
    if(speed < 0.0) {
      session->request_transport_speed(1.0);
    } else {
      session->request_transport_speed(speed + .1 * _datawheel);
    }
  }
  
  if (_datawheel < 0) {
    if(speed > 0.0) {
      session->request_transport_speed(-1.0);
    } else {
      session->request_transport_speed(speed + .1 * _datawheel);
    }
  }
}
