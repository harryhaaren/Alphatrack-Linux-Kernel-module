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
#include <tranzport_control_protocol.h>

using namespace ARDOUR;
using namespace std;
using namespace sigc;
using namespace PBD;

#include "i18n.h"

#include <pbd/abstract_ui.cc>



void
TranzportControlProtocol::button_event_battery_press (bool shifted)
{
}

void
TranzportControlProtocol::button_event_battery_release (bool shifted)
{
#if DEBUG_TRANZPORT
	printf("battery released\n");
#endif
// The tranzport refuses to respond to events
// for about 5 seconds after the battery button is released.
// It does work in the case of a rebind (shifted).
// So, doing a notify here forces a delayed screen redraw
// which is not long enough, still. Grumble.
// FIXME: notify should use a fixed timeout rather than loopcount

	screen_invalidate();
	last_where += 1; /* force time redisplay */
	last_track_gain = FLT_MAX;
	if(shifted) notify("Ardour Reconnected");
	else notify("Battery Pressed     ");
}

void
TranzportControlProtocol::button_event_backlight_press (bool shifted)
{
}

void
TranzportControlProtocol::button_event_backlight_release (bool shifted)
{
#if DEBUG_TRANZPORT
	printf("backlight released, redrawing (and possibly crashing) display\n");
#endif
	screen_invalidate();
	last_where += 1; /* force time redisplay */
	last_track_gain = FLT_MAX;
}

void
TranzportControlProtocol::button_event_trackleft_press (bool shifted)
{
	change_track (-1);
	// not really the right layer for this
	if(display_mode == DisplayBigMeter) { 
		if (route_table[0] != 0) {
			notify(route_get_name (0).substr (0, 15).c_str());
		}
	}
}

void
TranzportControlProtocol::button_event_trackleft_release (bool shifted)
{
}

void
TranzportControlProtocol::button_event_trackright_press (bool shifted)
{
	change_track (1); 
	// not really the right layer for this
	if(display_mode == DisplayBigMeter) { 
		if (route_table[0] != 0) {
			notify(route_get_name (0).substr (0, 15).c_str());
		}
	}
}

void
TranzportControlProtocol::button_event_trackright_release (bool shifted)
{
}

void
TranzportControlProtocol::button_event_trackrec_press (bool shifted)
{
	if (shifted) {
		toggle_all_rec_enables ();
	} else {
		route_set_rec_enable (0, !route_get_rec_enable (0));
	}
}

void
TranzportControlProtocol::button_event_trackrec_release (bool shifted)
{
}

void
TranzportControlProtocol::button_event_trackmute_press (bool shifted)
{
	if (shifted) {
	// Mute ALL? Do something useful when a phone call comes in. 
	// Mute master? Mute Master and Busses? I think mute master
	// would be best. FIXME
	} else {
		route_set_muted (0, !route_get_muted (0));
	}
}

void
TranzportControlProtocol::button_event_trackmute_release (bool shifted)
{
}

void
TranzportControlProtocol::button_event_tracksolo_press (bool shifted)
{
#if DEBUG_TRANZPORT
	printf("solo pressed\n");
#endif
	if (display_mode == DisplayBigMeter) {
		light_off (LightAnysolo);
		return;
	}

	if (shifted) {
		session->set_all_solo (!session->soloing());
	} else {
		route_set_soloed (0, !route_get_soloed (0));
	}
}

void
TranzportControlProtocol::button_event_tracksolo_release (bool shifted)
{
#if DEBUG_TRANZPORT
	printf("solo released\n");
#endif
}

void
TranzportControlProtocol::button_event_undo_press (bool shifted)
{
  // FIXME: Display what is to be undone
// undohistory->get_state(1);
//XMLNode&
//UndoHistory::get_state (uint32_t depth)

}

void
TranzportControlProtocol::button_event_undo_release (bool shifted)
{
	if (shifted) {
		redo (); // FIXME: flash the screen with what was redone
		notify("Redone!!");
	} else {
		undo (); // FIXME: flash the screen with what was undone
		notify("Undone!!");
	}
}

void
TranzportControlProtocol::button_event_in_press (bool shifted)
{
	if (shifted) {
	  // FIXME: Having ControlProtocol:ZoomToRegion makes the most sense to me
	  // Select the region under the playhead on the current tranzport track 
	  // and zoom. 
	  // Editor/zoom-to-region does this to the selected on screen track... which is
	  // not what we want, we want the tranzport's track and region under 
	  // the playhead
	  // to be selected and zoomed
	  // and although I LOVE both-axes
	  // I need a sane way to get back to a more normal zoomed state
	  //	  access_action("Editor/zoom-to-region-both-axes");
	  access_action("Editor/zoom-to-region");
	  notify("Zoomed To Region");
	} else {
	  ControlProtocol::ZoomIn (); /* EMIT SIGNAL */
	  notify("Zoomed IN");
	}
}

void
TranzportControlProtocol::button_event_in_release (bool shifted)
{
}

void
TranzportControlProtocol::button_event_out_press (bool shifted)
{
	if (shifted) {
	  ControlProtocol::ZoomToSession (); /* EMIT SIGNAL */
	  notify("Zoomed To Session");
	} else {
	  ControlProtocol::ZoomOut (); /* EMIT SIGNAL */
	  notify("Zooming Out");
	}
}

void
TranzportControlProtocol::button_event_out_release (bool shifted)
{
}

void
TranzportControlProtocol::button_event_punch_press (bool shifted)
{
  punch_held = 1;
}

void
TranzportControlProtocol::button_event_punch_release (bool shifted)
{
  if(complex_mode_change) {
    complex_mode_change = 0;
  } else {
    if (shifted) {
      toggle_punch_out ();
    } else {
      toggle_punch_in ();
    }
  }
    punch_held = 0;
}

void
TranzportControlProtocol::button_event_loop_press (bool shifted)
{
    loop_held = 1;
}

void
TranzportControlProtocol::button_event_loop_release (bool shifted)
{
  if(complex_mode_change) {
    complex_mode_change = 0;
  } else {
    if (shifted) {
      // FIXME: Do something more interesting than this?
    } else {
      loop_mode = loop_mode ? 0 : 1;
    }
  }
  loop_held = 0;
}


void
TranzportControlProtocol::button_event_prev_press (bool shifted)
{
	if (shifted) {
	  next_wheel_shift_mode ();
	} else {
	  prev_marker_any ();
	}
}

void
TranzportControlProtocol::button_event_prev_release (bool shifted)
{
}

// FIXME - add_marker should adhere to the snap to setting
// maybe session->audible_frame does that?

void
TranzportControlProtocol::button_event_add_press (bool shifted)
{
  add_held = 1;
}

/*
        session->begin_reversible_command (_("clear locations"));

                Location * looploc = session->locations()->auto_loop_location();
                Location * punchloc = session->locations()->auto_punch_location();

                session->locations()->clear_ranges ();
                // re-add these
                if (looploc) session->locations()->add (looploc);
                if (punchloc) session->locations()->add (punchloc);

       session->commit_reversible_command ();


     if (session->transport_rolling()) {
                session->request_stop (with_abort);
                if (session->get_play_loop()) {
                        session->request_play_loop (false);
                }
        } else {
                session->request_transport_speed (1.0f);
        }



*/

void
TranzportControlProtocol::button_event_add_release (bool shifted)
{
  // FIXME nframes64_t for ardour3?
  string loop;
  Location *loc;
  Location *newloc;
  bool wasnull = 0;
  // The usual sequence is [ shift ] - [ loop or punch ] - add 
  if(loop_held | punch_held) {
      complex_mode_change = 1;
    if (loop_held) {
      loc = session->locations()->auto_loop_location();
      if(loc==0) {
	newloc = new Location(0.0,session->current_end_frame(),_("Loop"), Location::Flags(Location::IsAutoLoop));
	wasnull = 1;
      } else {
	newloc = new Location(*loc);
      }
      
      session->begin_reversible_command (_("change loop range")); // not so clever
      XMLNode &before = session->locations()->get_state();

      if(wasnull == 0) session->locations()->remove(loc);

      if(shifted) {
	newloc->set_end(session->transport_frame());
      } else {
	newloc->set_start(session->transport_frame());
      }

      if (newloc->start() >= newloc->end()) {
	newloc->set_end (newloc->start() + 1);
      }
      session->locations()->add (newloc,true);
      session->set_auto_loop_location (newloc);
      XMLNode &after = session->locations()->get_state();
      session->add_command(new MementoCommand<Locations>(*(session->locations()), &before, &after));
      session->commit_reversible_command ();

      if(shifted) {
	notify("LOOP END ADDED ");
      } else {
	notify("LOOP START ADD ");
      }


    } else {
      if (punch_held) {
      loc = session->locations()->auto_punch_location();
      if(loc==0) {
	newloc = new Location(0.0,session->current_end_frame(),_("Loop"), Location::Flags(Location::IsAutoPunch));
	wasnull = 1;
      } else {
	newloc = new Location(*loc);
      }
      
      session->begin_reversible_command (_("change loop range")); // not so clever
      XMLNode &before = session->locations()->get_state();

      if(wasnull == 0) session->locations()->remove(loc);

      if(shifted) {
	newloc->set_end(session->transport_frame());
      } else {
	newloc->set_start(session->transport_frame());
      }

      if (newloc->start() >= newloc->end()) {
	newloc->set_end (newloc->start() + 1);
      }
      session->locations()->add (newloc,true);
      session->set_auto_punch_location (newloc);
      XMLNode &after = session->locations()->get_state();
      session->add_command(new MementoCommand<Locations>(*(session->locations()), &before, &after));
      session->commit_reversible_command ();

      if(shifted) {
	notify("PNCH END ADDED ");
      } else {
	notify("PNCH START ADD ");
      }
      }
    }
  } else {
    add_marker_snapped();
  }
  add_held = 0;
}


void
TranzportControlProtocol::button_event_next_press (bool shifted)
{
	if (shifted) {
		next_wheel_mode ();
	} else {
		next_marker_any ();
	}
}

void
TranzportControlProtocol::button_event_next_release (bool shifted)
{
}

void
TranzportControlProtocol::button_event_rewind_press (bool shifted)
{
	if (shifted) {
	  if(loop_mode) {
	    Location *l = session->locations()->auto_loop_location();
	    session->request_locate(l->start(),session->transport_rolling());
	  } else {
		goto_start ();
	  }
	} else {
		float speed = session->transport_speed();
		if(speed > -2.0) {
			rewind ();
		} else {
		if(speed <= -2.0) {
			speed += -1.0;
			session->request_transport_speed(speed < -8.0 ? -8.0 : speed);
			}
		}	
	}
}

void
TranzportControlProtocol::button_event_rewind_release (bool shifted)
{
}

void
TranzportControlProtocol::button_event_fastforward_press (bool shifted)
{
	if (shifted) {
	  if(loop_mode) {
	    Location *l = session->locations()->auto_loop_location();
	    session->request_locate(l->end(),session->transport_rolling());
	  } else {
		goto_end ();
	  }
	} else {
		float speed = session->transport_speed();
		if(speed < 2.0) {
			ffwd ();
		} else {
		if(speed >= 2.0) {
			speed += 1.0;
			session->request_transport_speed(speed > 8.0 ? 8.0 : speed);
			}
		}	
	}
}

void
TranzportControlProtocol::button_event_fastforward_release (bool shifted)
{
}

void
TranzportControlProtocol::button_event_stop_press (bool shifted)
{
	if (shifted) {
		next_display_mode ();
	} else {
		transport_stop ();
	}
}

void
TranzportControlProtocol::button_event_stop_release (bool shifted)
{
}

/*
                IsAutoPunch = 0x2,
                IsAutoLoop = 0x4,
                IsHidden = 0x8,
                IsCDMarker = 0x10,
*/

void
TranzportControlProtocol::button_event_play_press (bool shifted)
{
  // FIXME: Does not always record, even when the record light is lit

  if(loop_mode == 1) {
    session->request_play_loop (true);
    if (!session->transport_rolling()) {
      session->request_transport_speed (1.0);
    }
  } else {
    session->request_play_loop (false);
    if (shifted) {
      set_transport_speed (1.0f);
    } else {
      transport_play ();
    }
  }
}

void
TranzportControlProtocol::button_event_play_release (bool shifted)
{
}

void
TranzportControlProtocol::button_event_record_press (bool shifted)
{
	if (shifted) {
		save_state ();
	} else {
		rec_enable_toggle ();
	}
}

void
TranzportControlProtocol::button_event_record_release (bool shifted)
{
}

void
TranzportControlProtocol::button_event_footswitch_press (bool shifted)
{
	if (shifted) {
		next_marker_any (); // think this through, we could also do punch
	} else {
		prev_marker_any (); // think this through, we could also do loop
	}
}

void
TranzportControlProtocol::button_event_footswitch_release (bool shifted)
{
	if(session->transport_speed() == 0.0)
	{
	  if(loop_mode) {
	    session->request_play_loop (true);
	    if (!session->transport_rolling()) {
	      session->request_transport_speed (1.0);
	    } 
	  } else {
	  session->request_play_loop (false);
	  transport_play ();
	  }
	} else {
	  session->request_play_loop (false);
	  transport_play ();
	}
}
