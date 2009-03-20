/*
 *   Copyright (C) 2006 Paul Davis 
 *   Copyright (C) 2009 David Taht
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

#include <alphatrack_common.h>
#include <alphatrack_control_protocol.h>
#include <slider_gain.h>

#include <inttypes.h>
#include <float.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>


using namespace ARDOUR;
using namespace std;
using namespace sigc;
using namespace PBD;

#include "i18n.h"
#include <pbd/abstract_ui.cc>

void
AlphatrackControlProtocol::button_event_trackleft_press (bool shifted)
{
	prev_track ();
	// not really the right layer for this
	if(display_mode == DisplayBigMeter) { 
		if (route_table[0] != 0) {
			notify(route_get_name (0).substr (0, 15).c_str());
		}
	}
}

void
AlphatrackControlProtocol::button_event_trackleft_release (bool shifted)
{
}

void
AlphatrackControlProtocol::button_event_trackright_press (bool shifted)
{
	next_track (); 
	// not really the right layer for this
	if(display_mode == DisplayBigMeter) { 
		if (route_table[0] != 0) {
			notify(route_get_name (0).substr (0, 15).c_str());
		}
	}
}

void
AlphatrackControlProtocol::button_event_trackright_release (bool shifted)
{
}

void
AlphatrackControlProtocol::button_event_trackrec_press (bool shifted)
{
	if (shifted) {
		toggle_all_rec_enables ();
	} else {
		route_set_rec_enable (0, !route_get_rec_enable (0));
	}
}

void
AlphatrackControlProtocol::button_event_trackrec_release (bool shifted)
{
}

void
AlphatrackControlProtocol::button_event_trackmute_press (bool shifted)
{
	if (shifted) {
		// Mute ALL? Something useful when a phone call comes in. Mute master?
	} else {
		route_set_muted (0, !route_get_muted (0));
	}
}

void
AlphatrackControlProtocol::button_event_trackmute_release (bool shifted)
{
}

void
AlphatrackControlProtocol::button_event_tracksolo_press (bool shifted)
{
#if DEBUG_ALPHATRACK
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
AlphatrackControlProtocol::button_event_tracksolo_release (bool shifted)
{
#if DEBUG_ALPHATRACK
	printf("solo released\n");
#endif
}

void
AlphatrackControlProtocol::button_event_undo_press (bool shifted)
{
// undohistory->get_state(1);
//XMLNode&
//UndoHistory::get_state (uint32_t depth)

	if (shifted) {
		access_action("Editor/redo"); // redo (); // FIXME: flash the screen with what was redone
		notify("Redone!!");
	} else {
		access_action("Editor/undo"); // redo (); // FIXME: flash the screen with what was redone
		notify("Undone!!");
	}
}

void
AlphatrackControlProtocol::button_event_undo_release (bool shifted)
{
}

void
AlphatrackControlProtocol::button_event_in_press (bool shifted)
{
		ControlProtocol::ZoomIn (); /* EMIT SIGNAL */
}

void
AlphatrackControlProtocol::button_event_in_release (bool shifted)
{
}

void
AlphatrackControlProtocol::button_event_out_press (bool shifted)
{
		ControlProtocol::ZoomOut (); /* EMIT SIGNAL */
}

void
AlphatrackControlProtocol::button_event_out_release (bool shifted)
{
}

void
AlphatrackControlProtocol::button_event_punch_press (bool shifted)
{
}

void
AlphatrackControlProtocol::button_event_punch_release (bool shifted)
{
}

void
AlphatrackControlProtocol::button_event_loop_press (bool shifted)
{
		loop_toggle ();
}

void
AlphatrackControlProtocol::button_event_loop_release (bool shifted)
{
}

void
AlphatrackControlProtocol::button_event_prev_press (bool shifted)
{
	if (shifted) {
		ControlProtocol::ZoomToSession (); /* EMIT SIGNAL */
	} else {
		prev_marker ();
	}
}

void
AlphatrackControlProtocol::button_event_prev_release (bool shifted)
{
}

void
AlphatrackControlProtocol::button_event_add_press (bool shifted)
{
	add_marker ();
}

void
AlphatrackControlProtocol::button_event_add_release (bool shifted)
{
}

void
AlphatrackControlProtocol::button_event_next_press (bool shifted)
{
	if (shifted) {
		next_wheel_mode ();
	} else {
		next_marker ();
	}
}

void
AlphatrackControlProtocol::button_event_next_release (bool shifted)
{
}

void
AlphatrackControlProtocol::button_event_rewind_press (bool shifted)
{
	if (shifted) {
		goto_start ();
	} else {
		rewind ();
	}
}

void
AlphatrackControlProtocol::button_event_rewind_release (bool shifted)
{
}

void
AlphatrackControlProtocol::button_event_fastforward_press (bool shifted)
{
	if (shifted) {
		goto_end ();
	} else {
		ffwd ();
	}
}

void
AlphatrackControlProtocol::button_event_fastforward_release (bool shifted)
{
}

void
AlphatrackControlProtocol::button_event_stop_press (bool shifted)
{
	if (shifted) {
		next_display_mode ();
	} else {
		transport_stop ();
	}
}

void
AlphatrackControlProtocol::button_event_stop_release (bool shifted)
{
}

void
AlphatrackControlProtocol::button_event_play_press (bool shifted)
{
	if (shifted) {
		set_transport_speed (1.0f);
	} else {
		transport_play ();
	}
}

void
AlphatrackControlProtocol::button_event_play_release (bool shifted)
{
}

void
AlphatrackControlProtocol::button_event_record_press (bool shifted)
{
	if (shifted) {
		save_state ();
	} else {
		rec_enable_toggle ();
	}
}

void
AlphatrackControlProtocol::button_event_record_release (bool shifted)
{
}

void
AlphatrackControlProtocol::button_event_footswitch_press (bool shifted)
{
	if (shifted) {
		next_marker (); // think this through, we could also do punch in
	} else {
		prev_marker (); // think this through, we could also do punch in
	}
}

void
AlphatrackControlProtocol::button_event_footswitch_release (bool shifted)
{
	if(session->transport_speed() == 0.0)
	{
		transport_play ();
	}
}

#define ALPHATRACK_BUTTON_HANDLER(callback, button_arg) if (button_changes & button_arg) { \
		if (buttonmask & button_arg) {				\
			callback##_press (buttonmask&ButtonShift); } else { callback##_release (buttonmask&ButtonShift); } }

int
AlphatrackControlProtocol::process (uint8_t* buf)
{

	uint32_t this_button_mask;
	uint32_t button_changes;

	_device_status = buf[1];

#if DEBUG_ALPHATRACK > 10 
	// Perhaps the device can go offline due to flow control, print command bits to see if we have anything interesting
	if(_device_status == STATUS_ONLINE) {
		printf("ONLINE   : %02x %02x %02x %02x %02x %02x %02x %02x\n", 
		       buf[0],buf[1],buf[2], buf[3], buf[4], buf[5],buf[6],buf[7]); 
	}
	if(_device_status == STATUS_OFFLINE) {
		printf("OFFLINE  : %02x %02x %02x %02x %02x %02x %02x %02x\n", 
		       buf[0],buf[1],buf[2], buf[3], buf[4], buf[5],buf[6],buf[7]); 
	}

	if(_device_status != STATUS_OK) { return 1; }

#endif


	this_button_mask = 0;
	this_button_mask |= buf[2] << 24;
	this_button_mask |= buf[3] << 16;
	this_button_mask |= buf[4] << 8;
	this_button_mask |= buf[5];
	_datawheel = buf[6];
	
#if DEBUG_ALPHATRACK_STATE > 1
	// Is the state machine incomplete?
	const unsigned int knownstates = 0x00004000|0x00008000|
		0x04000000|    0x40000000|    0x00040000|    0x00400000|
		0x00000400|    0x80000000|    0x02000000|    0x20000000|
		0x00800000|    0x00080000|    0x00020000|    0x00200000|
		0x00000200|    0x01000000|    0x10000000|    0x00010000|
		0x00100000|    0x00000100|    0x08000000|    0x00001000;

	std::bitset<32> bi(knownstates);
	std::bitset<32> vi(this_button_mask);

	//  if an bi & vi == vi the same - it's a valid set

	if(vi != (bi & vi)) {
		printf("UNKNOWN STATE: %s also, datawheel= %d\n", vi.to_string().c_str(), _datawheel);
	}
#endif

	button_changes = (this_button_mask ^ buttonmask);
	buttonmask = this_button_mask;

	// FIXME - I had to handle shift around here somewhere before
	//	if (_datawheel) {
	//	datawheel ();
	// }

	// SHIFT + STOP + PLAY for bling mode?
	// if (button_changes & ButtonPlay & ButtonStop) {
	// bling_mode_toggle();  
	// } or something like that

	ALPHATRACK_BUTTON_HANDLER(button_event_trackleft,ButtonTrackLeft);
	ALPHATRACK_BUTTON_HANDLER(button_event_trackright,ButtonTrackRight);
	ALPHATRACK_BUTTON_HANDLER(button_event_trackrec,ButtonTrackRec);
	ALPHATRACK_BUTTON_HANDLER(button_event_trackmute,ButtonTrackMute);
	ALPHATRACK_BUTTON_HANDLER(button_event_tracksolo,ButtonTrackSolo);
	// ALPHATRACK_BUTTON_HANDLER(button_event_undo,ButtonUndo);
	// ALPHATRACK_BUTTON_HANDLER(button_event_in,ButtonIn);
	// ALPHATRACK_BUTTON_HANDLER(button_event_out,ButtonOut);
	ALPHATRACK_BUTTON_HANDLER(button_event_punch,ButtonPunch);
	ALPHATRACK_BUTTON_HANDLER(button_event_loop,ButtonLoop);
	// ALPHATRACK_BUTTON_HANDLER(button_event_prev,ButtonPrev);
	// ALPHATRACK_BUTTON_HANDLER(button_event_add,ButtonAdd);
	// ALPHATRACK_BUTTON_HANDLER(button_event_next,ButtonNext);
	ALPHATRACK_BUTTON_HANDLER(button_event_rewind,ButtonRewind);
	ALPHATRACK_BUTTON_HANDLER(button_event_fastforward,ButtonFastForward);
	ALPHATRACK_BUTTON_HANDLER(button_event_stop,ButtonStop);
	ALPHATRACK_BUTTON_HANDLER(button_event_play,ButtonPlay);
	ALPHATRACK_BUTTON_HANDLER(button_event_record,ButtonRecord);
	ALPHATRACK_BUTTON_HANDLER(button_event_footswitch,ButtonFootswitch);
	return 0;
}

//   FIXME, flash recording light when recording and transport is moving
int AlphatrackControlProtocol::lights_show_recording() 
{
	return     lights_show_normal();
}

void AlphatrackControlProtocol::show_bling() {
	lights_show_bling();
	screen_show_bling();
}

void AlphatrackControlProtocol::notify(const char *msg) {
	last_notify=100;
	if(strlen(msg) < COLUMNS+1) {
		strcpy(last_notify_msg,msg);
	} else {
		strncpy(last_notify_msg,msg,COLUMNS);
		last_notify_msg[COLUMNS] = '\n';
	}
}

void AlphatrackControlProtocol::show_notify() {
// FIXME: Get width of the notify area somehow
	if(last_notify==0) {
		print(1,0,"                ");
		last_notify=-1;
	}
	if(last_notify > 0) {
		print(1,0,last_notify_msg);
		--last_notify;
	}
}

// Need more bling!

int AlphatrackControlProtocol::lights_show_bling() 
{
	switch (bling_mode) {
	case BlingOff: break;
	case BlingKit: break; // rotate rec/mute/solo/any solo back and forth
	case BlingRotating: break; // switch between lights
	case BlingPairs: break; // Show pairs of lights
	case BlingRows: break; // light each row in sequence
	case BlingFlashAll: break; // Flash everything randomly
	case BlingEnter: lights_on(); // Show intro 	
	case BlingExit: 
		lights_off();
		break;
	}
	return 0;
}

int AlphatrackControlProtocol::screen_show_bling() 
{
	switch (bling_mode) {
	case BlingOff: break;
	case BlingKit: break; // rotate rec/mute/solo/any solo back and forth
	case BlingRotating: break; // switch between lights
	case BlingPairs: break; // Show pairs of lights
	case BlingRows: break; // light each row in sequence
	case BlingFlashAll: break; // Flash everything randomly
	case BlingEnter: // Show intro 	
		print(0,0,"!!Welcome to Ardour!");
		print(1,0,"Peace through Music!");
		break;
	case BlingExit: 
		break;
	}
	return 0;
}

int AlphatrackControlProtocol::lights_show_normal() 
{
	/* Track only */

	if (route_table[0]) {
		boost::shared_ptr<AudioTrack> at = boost::dynamic_pointer_cast<AudioTrack> (route_table[0]);
		lights_pending[LightTrackrec]  = at && at->record_enabled();
		lights_pending[LightTrackmute] = route_get_muted(0); 
		lights_pending[LightTracksolo] = route_get_soloed(0);
	} else {
		lights_pending[LightTrackrec]  = false;
		lights_pending[LightTracksolo] = false;
		lights_pending[LightTrackmute] = false;
	}

	/* Global settings */

	lights_pending[LightLoop]        = session->get_play_loop() || Config->get_punch_in() || Config->get_punch_out();
	lights_pending[LightRecord]      = session->get_record_enabled();
	lights_pending[LightAnysolo]     = session->soloing();

	return 0;
}

int AlphatrackControlProtocol::lights_show_tempo() 
{
	// someday soon fiddle with the lights more sanely based on the tempo 
	return     lights_show_normal();
}

int
AlphatrackControlProtocol::update_state ()
{
	/* do the text and light updates */

	switch (display_mode) {
	case DisplayBigMeter:
		lights_show_tempo();
		show_meter ();
		break;

	case DisplayNormal:
		lights_show_normal();
		normal_update();
		break;

	case DisplayConfig:
		break;

	case DisplayRecording:
		lights_show_recording();
		normal_update();
		break;

	case DisplayRecordingMeter:
		lights_show_recording();
		show_meter(); 
		break;

	case DisplayBling:
		show_bling();
		break;

	case DisplayBlingMeter:
		lights_show_bling();
		show_meter();
		break;
	}
	show_notify();

	return 0;

}

void
AlphatrackControlProtocol::prev_marker ()
{
	Location *location = session->locations()->first_location_before (session->transport_frame());
	
	if (location) {
		session->request_locate (location->start(), session->transport_rolling());
		notify(location->name().c_str());
	} else {
		session->goto_start ();
		notify("START");
	}
     
}

void
AlphatrackControlProtocol::next_marker ()
{
	Location *location = session->locations()->first_location_after (session->transport_frame());

	if (location) {
		session->request_locate (location->start(), session->transport_rolling());
		notify(location->name().c_str());
	} else {
		session->request_locate (session->current_end_frame());
		notify("END ");
	}
}


void
AlphatrackControlProtocol::show_current_track ()
{
	char pad[COLUMNS];
	char *v;
	int len;
	if (route_table[0] == 0) {
		print (0, 0, "---------------");
		last_track_gain = FLT_MAX;
	} else {
		strcpy(pad,"               ");
		v =  (char *)route_get_name (0).substr (0, 14).c_str();
		if((len = strlen(v)) > 0) {
			strncpy(pad,(char *)v,len);
		}
		print (0, 0, pad);
	}
}


#if 0
void
AlphatrackControlProtocol::step_gain (float increment)
{
// FIXME: buttonstop is used elsewhere
	if (buttonmask & ButtonStop) {
		gain_fraction += 0.001*increment;
	} else {
		gain_fraction += 0.01*increment;
	}

	if (fabsf(gain_fraction) > 2.0) {
		gain_fraction = 2.0*sign(gain_fraction);
	}
	
	route_set_gain (0, slider_position_to_gain (gain_fraction));
}
#endif

void
AlphatrackControlProtocol::step_gain_up ()
{
	if (buttonmask & ButtonStop) {
		gain_fraction += 0.001;
	} else {
		gain_fraction += 0.01;
	}

	if (gain_fraction > 2.0) {
		gain_fraction = 2.0;
	}
	
	route_set_gain (0, slider_position_to_gain (gain_fraction));
}

void
AlphatrackControlProtocol::step_gain_down ()
{
	if (buttonmask & ButtonStop) {
		gain_fraction -= 0.001;
	} else {
		gain_fraction -= 0.01;
	}

	if (gain_fraction < 0.0) {
		gain_fraction = 0.0;
	}
	
	route_set_gain (0, slider_position_to_gain (gain_fraction));
}


void
AlphatrackControlProtocol::next_track ()
{
	ControlProtocol::next_track (current_track_id);
	gain_fraction = gain_to_slider_position (route_get_effective_gain (0));
//	notify("NextTrak"); // not needed til we have more modes
}

void
AlphatrackControlProtocol::prev_track ()
{
	ControlProtocol::prev_track (current_track_id);
	gain_fraction = gain_to_slider_position (route_get_effective_gain (0));
//	notify("PrevTrak");
}

// This should kind of switch to using notify

// Was going to keep state around saying to retry or not
// haven't got to it yet, still not sure it's a good idea

void
AlphatrackControlProtocol::print (int row, int col, const char *text) {
	print_noretry(row,col,text);
}

// -1 on failure
// 0 on no damage
// count of bit set on damage?

void
AlphatrackControlProtocol::print_noretry (int row, int col, const char *text)
{
	uint32_t length = strlen (text);
	if (row*COLUMNS+col+length > (ROWS*COLUMNS)) {
		return;
	}
	// FIXME - be able to print the whole screen at a go.
	uint32_t t,r,c;
	std::bitset<ROWS*COLUMNS> mask(screen_invalid);
	for(r = row, c = col, t = 0 ; t < length; c++,t++) {
		screen_pending[r][c] = text[t];
		mask[r*COLUMNS+c] = (screen_current[r][c] != screen_pending[r][c]);
	}
	screen_invalid = mask;
}

void AlphatrackControlProtocol::invalidate() 
{
	lcd_damage(); lights_invalidate(); screen_invalidate(); // one of these days lcds can be fine but screens not
}

void*
AlphatrackControlProtocol::_monitor_work (void* arg)
{
	return static_cast<AlphatrackControlProtocol*>(arg)->monitor_work ();
}

AlphatrackControlProtocol::~AlphatrackControlProtocol ()
{
	set_active (false);
}

int
AlphatrackControlProtocol::set_active (bool yn)
{
	if (yn != _active) {

		if (yn) {

			if (open ()) {
				return -1;
			}

			if (pthread_create_and_store (X_("alphatrack monitor"), &thread, 0, _monitor_work, this) == 0) {
				_active = true;
#if ALPHATRACK_THREADS                      
			if (pthread_create_and_store (X_("alphatrack read"), &thread_read, 0, _read_work, this) == 0) {
				_active_read = true;
			if (pthread_create_and_store (X_("alphatrack write"), &thread_write, 0, _write_work, this) == 0) {
				_active_write = true;
			if (pthread_create_and_store (X_("alphatrack process"), &thread_process, 0, _process_work, this) == 0) {
				_active_process = true;
			if (pthread_create_and_store (X_("alphatrack timer"), &thread_timer, 0, _process_timer, this) == 0) {
				_active_process = true;
#endif
			} else {
				return -1;
			}

		} else {
			cerr << "Begin alphatrack shutdown\n";
//                      if we got here due to an error, prettifying things will only make it worse
//                      And with threads involved, oh boy...
			if(!(last_write_error || last_read_error)) {
				bling_mode   = BlingExit;
				enter_bling_mode();
// thread FIXME - wait til all writes are done
				for(int x = 0; (x < 20/MAX_ALPHATRACK_INFLIGHT) && flush(); x++) { usleep(100); }
			}
#if ALPHATRACK_THREADS                      
			pthread_cancel_one (_thread_timer);
			pthread_cancel_one (_thread_process);
			pthread_cancel_one (_thread_read);
			pthread_cancel_one (_thread_write);
#endif
			pthread_cancel_one (thread);

			cerr << "Alphatrack Thread dead\n";
			close ();
			_active = false;
			cerr << "End alphatrack shutdown\n";
		} 
	}

	return 0;
}

AlphatrackControlProtocol::AlphatrackControlProtocol (Session& s)
	: ControlProtocol  (s, X_("Alphatrack"))
{
	/* alphatrack controls one track at a time */

	set_route_table_size (1);
	timeout = 6000; // what is this for?
	buttonmask = 0;
	_datawheel = 0;
	_device_status = STATUS_OFFLINE;
	udev = 0;
	current_track_id = 0;
	last_where = max_frames;
	wheel_mode = WheelTimeline;
	wheel_shift_mode = WheelShiftGain;
	wheel_increment = WheelIncrScreen;
	bling_mode = BlingEnter;
	last_notify_msg[0] = '\0';
	last_notify = 0;
	timerclear (&last_wheel_motion);
	last_wheel_dir = 1;
	last_track_gain = FLT_MAX;
	last_write_error = 0;
	last_read_error = 0;
	display_mode = DisplayBling;
	gain_fraction = 0.0;
	invalidate();
	screen_init();
	lights_init();
// FIXME: Wait til device comes online somewhere
// About 3 reads is enough
// enter_bling_mode();

}

void*
AlphatrackControlProtocol::monitor_work ()
{
	uint8_t buf[12]; //  = { 0,0,0,0,0,0,0,0 };
	int val = 0, pending = 0;
	bool first_time = true;
	uint8_t offline = 0;
	int timeout = 10;
	PBD::notify_gui_about_thread_creation (pthread_self(), X_("Alphatrack"));
	pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, 0);
	pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	inflight=0;
	display_mode = DisplayNormal;
	struct pollfd readfd;
	// struct pollfd writefd;
	readfd.fd = udev;
	readfd.events = POLLIN | POLLOUT;
	printf("polling device\n");
	poll(&readfd,1,timeout);
	
       	while (true) {
	  if(first_time == true) {
	    read(buf,DEFAULT_USB_TIMEOUT); 
	    invalidate();
	    screen_init();
	    lights_init();
	    update_state();
	    flush();
	    first_time = false;
	  }
	  poll(&readfd,1,timeout);
	  if(!((readfd.revents & POLLIN) | (readfd.revents & POLLOUT))) {
	    if(_device_status == STATUS_OFFLINE) {
	      printf("Offline\n");
	      timeout = 400;
	    } else {
	      timeout = 40;
	      update_state();
	      pending = flush();
	      // one day see if the session is running
	    }
	  }

	  if(readfd.revents & POLLIN) {
	    read(buf,0);
	    process (buf);	    
	    pending += 1;
	    timeout = 1;
	    readfd.events = POLLIN | POLLOUT;
	    printf("Had pollin\n");
	  }
	  if(readfd.revents & POLLOUT || pending > 0) {
	    inflight = 0;
	    update_state();
	    if(pending == 0) {
	      pending = flush();
	      printf("Pending was %d\n", pending);
	      if(pending == 0) {
		timeout = 2;
		readfd.events = POLLIN;
	        inflight = 0;
	      } else {
		readfd.events = POLLIN | POLLOUT;
		timeout = pending;
	        inflight = 0;
	      }
	    } else {
	      pending += flush();
	      timeout = 1;
	      readfd.events = POLLIN | POLLOUT;
	      inflight = 0;
	    }
	    inflight = 0;
	    if(pending > 0) pending--;
	  }

	}
	return (void*) 0;
}
			

int
AlphatrackControlProtocol::flush ()
{
	int pending = 0;

	pending = lights_flush();
	pending += screen_flush();

#if DEBUG_ALPHATRACK_BITS > 9
	int s;
	if(s = (screen_invalid.count())) { //  + lights_invalid.count())) {
		printf("VALID  : %s %s\n",
		       screen_invalid.to_string().c_str(),
		       lights_invalid.to_string().c_str());
		printf("CURR   : %s %s\n",
		       screen_invalid.to_string().c_str(),
		       lights_current.to_string().c_str());
		printf("PENDING  : %s %s\n",
		       screen_invalid.to_string().c_str(),
		       lights_pending.to_string().c_str());
#if DEBUG_ALPHATRACK_BITS > 10
		printf("invalid bits: %d\n",s);
#endif
	}
#endif
	return pending;
}


int
AlphatrackControlProtocol::lights_flush ()
{
	std::bitset<LIGHTS> light_state;
	light_state = lights_pending ^ lights_current;
	if ( (light_state.none() || lights_invalid.none()))
	{ 
		return (0); 
	}

#if DEBUG_ALPHATRACK_LIGHTS
	printf("LPEND   : %s\n", lights_pending.to_string().c_str());
	printf("LCURR   : %s\n", lights_current.to_string().c_str());
#endif
		
	// if ever we thread reads/writes STATUS_OK will have to move into the loop
	int i;
		
	if ( _device_status == STATUS_OK || _device_status == STATUS_ONLINE) {
		for (i = 0; i<LIGHTS; i++) {
			if(light_state[i]) { 
				if(light_set ((LightID)i,lights_pending[i])) {
#if DEBUG_ALPHATRACK_LIGHTS > 2
					printf("Did %d light writes\n",i);
#endif
				} else {
					light_state[i] = 0;
				}

			}
		}
	}
	light_state = lights_pending ^ lights_current;
#if DEBUG_ALPHATRACK_LIGHTS > 2
	printf("Did %d light writes, left: %d\n",i, light_state.count());
#endif
		
	return light_state.count();
}


bool
AlphatrackControlProtocol::probe ()
{
  if(::access(ALPHATRACK_DEVICE, R_OK | W_OK)==0) return true;
	return false;
}

int
AlphatrackControlProtocol::open ()
{
  if((udev = ::open(ALPHATRACK_DEVICE, O_RDWR | O_EXCL)) < 1) {
		cerr << _("Alphatrack: no device detected") << endmsg;
		return -1;
	}
  return 0;
}

int
AlphatrackControlProtocol::close ()
{
	int ret = 0;

	if (udev < 1) {
		return 0;
	}

	if ((ret = ::close (udev))!=0) {
		cerr << _("Alphatrack: cannot close device") << endmsg;
		udev = 0;
	}

	return ret;
}

int AlphatrackControlProtocol::read(uint8_t *buf, uint32_t timeout_override) 
{
	::read(udev, (char *) buf, 8);
	last_read_error = errno;
	switch(last_read_error) {
	case -ENOENT:
	case -ENXIO:
	case -ECONNRESET:
	case -ESHUTDOWN: 
	case -ENODEV: 
		cerr << "Alphatrack disconnected, errno: " << last_read_error;
		set_active(false);
	case -ETIMEDOUT: // This is normal
		break;
	default: 
#if DEBUG_ALPHATRACK
		cerr << "Got an unknown error on read:" << last_read_error "\n";
#endif
		break;
	}

	return last_read_error;
} 

	
int
AlphatrackControlProtocol::write_noretry (uint8_t* cmd, uint32_t timeout_override)
{
	int val;
	if(inflight > MAX_ALPHATRACK_INFLIGHT) { return (-1); }
	val = ::write(udev, (char*) cmd, 8);

	if (val <= 0 && val !=8) {
		last_write_error = errno;
#if DEBUG_ALPHATRACK
		printf("usb_interrupt_write failed: %d\n", val);
#endif
		switch(last_write_error) {
		case -ENOENT:
		case -ENXIO:
		case -ECONNRESET:
		case -ESHUTDOWN: 
		case -ENODEV: 
			cerr << "Alphatrack disconnected, errno: " << last_write_error;
			set_active(false);
		case -ETIMEDOUT: // This is normal
			break;
		default: 
#if DEBUG_ALPHATRACK
			cerr << "Got an unknown error on read:" << last_write_error "\n";
#endif
			break;
		}
		return val;
	}

	last_write_error = 0;
	++inflight;

	return 0;

}	

int
AlphatrackControlProtocol::write (uint8_t* cmd, uint32_t timeout_override)
{
#if MAX_RETRY > 1
	int val;
	int retry = 0;
	if(inflight > MAX_ALPHATRACK_INFLIGHT) { return (-1); }
	
	while((val = ::write(udev, (char*) cmd, 8))!=8 && retry++ < MAX_RETRY) {
		printf("usb_interrupt_write failed, retrying: %d\n", val);
	}

	if (retry == MAX_RETRY) {
		printf("Too many retries on a alphatrack write, aborting\n");
	}

	if (val < 0) {
		printf("usb_interrupt_write failed: %d\n", val);
		return val;
	}
	if (val != 8) {
		printf("usb_interrupt_write failed: %d\n", val);
		return -1;
	}
	++inflight;
	return 0;
#else
	return (write_noretry(cmd,timeout_override));
#endif

}	


	
bool AlphatrackControlProtocol::lcd_damage() 
{
	screen_invalidate();
	return true;
}

bool AlphatrackControlProtocol::lcd_damage (int row, int col, int length)
{
	std::bitset<ROWS*COLUMNS> mask1(0);
	// there's an intrinsic to do this fast, darn it, or I'm just sleepy
	for (int i = 0; i < length; i++) { mask1[i] = 1; }
	std::bitset<ROWS*COLUMNS> mask(mask1 << (row*COLUMNS+col));
	screen_invalid |= mask;
	return true;
}

bool AlphatrackControlProtocol::lcd_isdamaged () 
{
	if(screen_invalid.any()) {
#if DEBUG_ALPHATRACK > 5	
		printf("LCD is damaged somewhere, should redraw it\n");
#endif
		return true;
	}
	return false;
}

bool AlphatrackControlProtocol::lcd_isdamaged (int row, int col, int length)
{
	// there's an intrinsic to do this fast, darn it
	std::bitset<ROWS*COLUMNS> mask1(0);
	for (int i = 0; i < length; i++) { mask1[i] = 1; }
	std::bitset<ROWS*COLUMNS> mask(mask1 << (row*COLUMNS+col));
	mask &= screen_invalid;
	if(mask.any()) {
#if DEBUG_ALPHATRACK > 5	
		printf("row: %d,col: %d is damaged, should redraw it\n", row,col);
#endif
		return true;
	}
	return false; 
}

// lcd_clear would be a separate function for a smart display
// here it does nothing, but for the sake of completeness it should
// probably write the lcd, and while I'm on the topic it should probably
// take a row, col, length argument....

void
AlphatrackControlProtocol::lcd_clear ()
{

}

// These lcd commands are not universally used yet and may drop out of the api

int
AlphatrackControlProtocol::lcd_flush ()
{
	return 0; 
}

int 
AlphatrackControlProtocol::lcd_write(uint8_t* cmd, uint32_t timeout_override)
{
	int result;
#if (DEBUG_ALPHATRACK_SCREEN > 0)
	printf("VALID  : %s\n", (screen_invalid.to_string()).c_str()); 
#endif
	if ((result = write(cmd,timeout_override))) {
#if DEBUG_ALPHATRACK > 4
		printf("usb screen update failed for some reason... why? \nresult, cmd and data were %d %02x %02x %02x %02x %02x %02x %02x %02x\n", 
		       result, cmd[0],cmd[1],cmd[2], cmd[3], cmd[4], cmd[5],cmd[6],cmd[7]); 
#endif
	}
	return result;
}

void 
AlphatrackControlProtocol::lcd_fill (uint8_t fill_char) 
{
}

void 
AlphatrackControlProtocol::lcd_print (int row, int col, const char* text) 
{
	print(row,col,text);
}

void AlphatrackControlProtocol::lcd_print_noretry (int row, int col, const char* text)
{
	print(row,col,text);
}

// Lights are buffered, and arguably these functions should be eliminated or inlined

void
AlphatrackControlProtocol::lights_on ()
{
	lights_pending.set();
}

void
AlphatrackControlProtocol::lights_off ()
{
	lights_pending.reset();
}

int
AlphatrackControlProtocol::light_on (LightID light)
{
	lights_pending.set(light);
	return 0;
}

int
AlphatrackControlProtocol::light_off (LightID light)
{
	lights_pending.reset(light);
	return 0;
}

void AlphatrackControlProtocol::lights_init()
{
	lights_invalid.set();
	lights_flash = lights_pending = lights_current.reset(); 
}


// Now that all this is bitsets, I don't see much 
// need for these 4 to remain in the API

void AlphatrackControlProtocol::light_validate (LightID light) 
{
	lights_invalid.reset(light);
}

void AlphatrackControlProtocol::light_invalidate (LightID light) 
{
	lights_invalid.set(light);
}

void AlphatrackControlProtocol::lights_validate () 
{
	lights_invalid.reset();
}

void AlphatrackControlProtocol::lights_invalidate () 
{
	lights_invalid.set();
}

int
AlphatrackControlProtocol::light_set (LightID light, bool offon)
{
	uint8_t cmd[8];
	cmd[0] = 0x00;  cmd[1] = 0x00;  cmd[2] = light;  cmd[3] = offon;
	cmd[4] = 0x00;  cmd[5] = 0x00;  cmd[6] = 0x00;  cmd[7] = 0x00;

	if (write (cmd) == 0) {
		lights_current[light] = offon;
		lights_invalid.reset(light);
		return 0;
	} else {
		return 1;
	}
}

void
AlphatrackControlProtocol::normal_update ()
{
	show_current_track ();
	show_transport_time ();
	show_track_gain ();
	show_wheel_mode ();
}

void
AlphatrackControlProtocol::next_display_mode ()
{
	switch (display_mode) {

	case DisplayNormal:
		enter_big_meter_mode();
		break;

	case DisplayBigMeter:
		enter_normal_display_mode();
		break;

	case DisplayRecording:
		enter_normal_display_mode();
		break;

	case DisplayRecordingMeter:
		enter_big_meter_mode();
		break;

	case DisplayConfig: 
	case DisplayBling:
	case DisplayBlingMeter:
		enter_normal_display_mode();
		break;
	}
}

// FIXME: There should be both enter and exits
// EXIT would erase the portions of the screen being written
// to.
/* not sure going macro crazy is a good idea

#define DECLARE_ENTER_MODE(mode,modename) void AlphatrackControlProtocol::enter_##mode##_mode() \{\screen_clear(); lights_off(); display_mode=Display##modename;\;
*/
void
AlphatrackControlProtocol::enter_recording_mode ()
{
	screen_clear ();
	lights_off ();
	display_mode = DisplayRecording;
}

void
AlphatrackControlProtocol::enter_bling_mode ()
{
	screen_clear ();
	lights_off ();
	display_mode = DisplayBling;
}

void
AlphatrackControlProtocol::enter_config_mode ()
{
	lights_off ();
	screen_clear ();
	display_mode = DisplayConfig;
}


void
AlphatrackControlProtocol::enter_big_meter_mode ()
{
	lights_off (); // it will clear the screen for you
	last_meter_fill = 0;
	display_mode = DisplayBigMeter;
}

void
AlphatrackControlProtocol::enter_normal_display_mode ()
{
	lights_off ();
	screen_clear ();
	display_mode = DisplayNormal;
}

void
AlphatrackControlProtocol::step_pan_right ()
{
}

void
AlphatrackControlProtocol::step_pan_left ()
{
}


void
AlphatrackControlProtocol::screen_clear ()
{
	const char *blank = "               ";
	print(0,0,blank); 
	print(1,0,blank);
}

void AlphatrackControlProtocol::screen_invalidate ()
{
	screen_invalid.set();
	for(int row = 0; row < ROWS; row++) {
		for(int col = 0; col < COLUMNS; col++) {
			screen_current[row][col] = 0x7f;
			screen_pending[row][col] = ' ';
		}
	}
}

void AlphatrackControlProtocol::screen_validate ()
{
}

void AlphatrackControlProtocol::screen_init ()
{
	screen_invalidate();
}

// FIXME: Switch to a column oriented flush to make the redraw of the 
// meters look better

int
AlphatrackControlProtocol::screen_flush ()
{
	int cell = 0, row=0, col_base, pending = 0;
	const unsigned long CELL_BITS = 0x0F;
	if ( _device_status == STATUS_OFFLINE) { return (-1); }

	std::bitset<ROWS*COLUMNS> mask(CELL_BITS);
	std::bitset<ROWS*COLUMNS> imask(CELL_BITS);
	for(cell = 0; cell < ROWS*COLUMNS/4; cell++) {
		mask = imask << (cell*4);
		if((screen_invalid & mask).any()) {
			/* something in this cell is different, so dump the cell to the device. */
#if DEBUG_ALPHATRACK_SCREEN
			printf("MASK   : %s\n", mask.to_string().c_str());
#endif
			if(cell > (COLUMNS/4-1)) { row = 1; } else { row = 0; }
			col_base = (cell*4)%COLUMNS;
        
			uint8_t cmd[8]; 
			cmd[0] = 0x00; 
			cmd[1] = 0x01; 
			cmd[2] = cell; 
			cmd[3] = screen_pending[row][col_base]; 
			cmd[4] = screen_pending[row][col_base+1];
			cmd[5] = screen_pending[row][col_base+2]; 
			cmd[6] = screen_pending[row][col_base+3];
			cmd[7] = 0x00;

			if(lcd_write(cmd) == 0) {
				/* successful write: copy to current cached display */
				screen_invalid &= mask.flip(); 
				memcpy (&screen_current[row][col_base], &screen_pending[row][col_base], 4);
			pending++;
			}
		}
	}
	return pending;
}


float
log_meter (float db)
{
	float def = 0.0f; /* Meter deflection %age */

	if (db < -70.0f) return 0.0f;
	if (db > 6.0f) return 1.0f;

	if (db < -60.0f) {
		def = (db + 70.0f) * 0.25f;
	} else if (db < -50.0f) {
		def = (db + 60.0f) * 0.5f + 2.5f;
	} else if (db < -40.0f) {
		def = (db + 50.0f) * 0.75f + 7.5f;
	} else if (db < -30.0f) {
		def = (db + 40.0f) * 1.5f + 15.0f;
	} else if (db < -20.0f) {
		def = (db + 30.0f) * 2.0f + 30.0f;
	} else if (db < 6.0f) {
		def = (db + 20.0f) * 2.5f + 50.0f;
	}

	/* 115 is the deflection %age that would be 
	   when db=6.0. this is an arbitrary
	   endpoint for our scaling.
	*/

	return def/115.0f;
}

#define TRANZ_U  0x1 /* upper */
#define TRANZ_BL 0x2 /* lower left */ 
#define TRANZ_Q2 0x3 /* 2 quadrant block */
#define TRANZ_ULB 0x4 /* Upper + lower left */ 
#define TRANZ_L 0x5  /* lower  */ 
#define TRANZ_UBL 0x6 /* upper left + bottom all */ 
#define TRANZ_Q4 0x7 /* 4 quadrant block */ 
#define TRANZ_UL 0x08 /* upper left */

// Shift Space - switches your "view"
// Currently defined views are:
// BigMeter
// 
// Shift Record - SAVE SNAPSHOT
// Somewhere I was rewriting this
// Other meters
// Inverted - show meters "inside out" For example 4 meters covering 2 cells each, and the
// 
// each 4 character cell could be an 8 bar meter = 10 meters!
// Dual Meter mode - master and current track
// We have 16 rows of pixels so we COULD do a vertical meter
// BEAT BLOCKS - For each beat, flash a 8 block (could use the center for vertical meters) 
// Could have something generic that could handle up to /20 time
// Odd times could flash the whole top bar for the first beat


// Vertical Meter _ .colon - + ucolon A P R I H FULLBLACK 
// MV@$%&*()-

// 3 char block  rotating beat `\'/
// 1 char rotating beat {/\}
// 4 char in block rotating beat {/\}
//                               {\/)
 
void AlphatrackControlProtocol::show_mini_meter()
{
	// FIXME - show the current marker in passing
	const int meter_buf_size = 41; 
	static uint32_t last_meter_fill_l = 0;
	static uint32_t last_meter_fill_r = 0;
	uint32_t meter_size = COLUMNS;

	float speed = fabsf(session->transport_speed());
	char buf[meter_buf_size];


	// you only seem to get a route_table[0] == 0 on moving forward - bug in next_track?

	if (route_table[0] == 0) {
		// Principle of least surprise
		print (1, 0, "NoAUDIO  ");
		return;
	}

	float level_l = route_get_peak_input_power (0, 0);
	float fraction_l = log_meter (level_l);

	// how to figure out if we are mono?

	float level_r = route_get_peak_input_power (0, 1);
	float fraction_r = log_meter (level_r);

	uint32_t fill_left  = (uint32_t) floor (fraction_l * ((int) meter_size));
	uint32_t fill_right  = (uint32_t) floor (fraction_r * ((int) meter_size));

	if (fill_left == last_meter_fill_l && fill_right == last_meter_fill_r && !lcd_isdamaged(1,0,meter_size/2)) {
		/* nothing to do */
		return;
	}

	last_meter_fill_l = fill_left;	last_meter_fill_r = fill_right;
	
	// give some feedback when overdriving - override yellow and red lights
	// I really don't think I want to use the scaled values here.

	if (level_l > 0.94 || level_r > 0.94) {
		light_on (LightLoop);
	}

	if (level_l > 0.98 || level_r > 0.98) {
		light_on (LightTrackrec);
	}
	
	const uint8_t char_map[16] = { ' ', TRANZ_UL, 
				       TRANZ_U, TRANZ_U,
				       TRANZ_BL, TRANZ_Q2, 
				       TRANZ_Q2, TRANZ_ULB,
				       TRANZ_L, TRANZ_UBL, 
				       ' ',' ',
				       TRANZ_L, TRANZ_UBL,
				       TRANZ_Q4,TRANZ_Q4
	};  
	unsigned int val,j,i;

	for(j = 1, i = 0; i < meter_size/2; i++, j+=2) {
		val = (fill_left >= j) | ((fill_left >= j+1) << 1) | 
			((fill_right >=j) << 2) | ((fill_right >= j+1) << 3);
		buf[i] = char_map[val];
	}
		
	/* print() requires this */

	buf[meter_size/2] = '\0';

	print (1, 0, buf);

	/* Add a peak bar, someday do falloff */
		
	//		char peak[2]; peak[0] = ' '; peak[1] = '\0';
	//		if(fraction_l == 1.0 || fraction_r == 1.0) peak[0] = 'P';
	//		print (1,8,peak); // Put a peak meter - P in if we peaked. 
		
}

void
AlphatrackControlProtocol::show_meter ()
{
	// you only seem to get a route_table[0] on moving forward - bug elsewhere
	if (route_table[0] == 0) {
		// Principle of least surprise
		print (0, 0, "No audio to meter!!!");
		print (1, 0, "Select another track"); 
		return;
	}

	float level_l = route_get_peak_input_power (0, 0);
	float fraction_l = log_meter (level_l);

	float fraction_r = 0.0;
	float level_r = 0.0; // FIXME for stereo

	/* Someday add a peak bar*/

	/* we draw using a choice of a sort of double colon-like character ("::") or a single, left-aligned ":".
	   the screen is 16 chars wide, so we can display 32 different levels. compute the level,
	   then figure out how many "::" to fill. if the answer is odd, make the last one a ":"
	*/

	uint32_t fill  = (uint32_t) floor (fraction_l * COLUMNS*2);
	char buf[COLUMNS+1];
	uint32_t i;

	if (fill == last_meter_fill) {
		/* nothing to do */
		return;
	}

	last_meter_fill = fill;

	bool add_single_level = (fill % 2 != 0);
	fill /= 2;

	if (level_l > 0.94 || level_r > 0.94) {
		light_on (LightLoop);
	}

	if (level_l > .98 || level_r > 0.98) {
		light_on (LightTrackrec);
	}


	/* add all full steps */

	for (i = 0; i < fill; ++i) {
		buf[i] = 0x07; /* alphatrack special code for 4 quadrant LCD block */
	} 
	
	/* add a possible half-step */

	if (i < COLUMNS*2 && add_single_level) {
		buf[i] = 0x03; /* alphatrack special code for 2 left quadrant LCD block */
		++i;
	}

	/* fill rest with space */

	for (; i < COLUMNS; ++i) {
		buf[i] = ' ';
	}

	/* print() requires this */

	buf[COLUMNS] = '\0';

	print (0, 0, buf);
	print (1, 0, buf);
}

void
AlphatrackControlProtocol::show_bbt (nframes_t where)
{ 
	if (where != last_where) {
		char buf[COLUMNS];
		BBT_Time bbt;

		// When recording or playing back < 1.0 speed do 1 or 2
		// FIXME - clean up state machine & break up logic
		// this has to co-operate with the mini-meter and
		// this is NOT the right way.

		session->tempo_map().bbt_time (where, bbt);
		last_bars = bbt.bars;
		last_beats = bbt.beats;
		last_ticks = bbt.ticks;
		last_where = where;

		float speed = fabsf(session->transport_speed());

		if (speed == 1.0)  { 
			sprintf (buf, "%03" PRIu32 "%1" PRIu32, bbt.bars,bbt.beats); // switch to hex one day
			print (1, 16, buf); 
		}

		if (speed == 0.0) { 
			sprintf (buf, "%03" PRIu32 "|%1" PRIu32 "|%04" PRIu32, bbt.bars,bbt.beats,bbt.ticks);
			print (1, 10, buf); 
		}

		if (speed > 0.0 && (speed < 1.0)) { 
			sprintf (buf, "%03" PRIu32 "|%1" PRIu32 "|%04" PRIu32, bbt.bars,bbt.beats,bbt.ticks);
			print (1, 10, buf); 
		}

		if (speed > 1.0 && (speed < 2.0)) { 
			sprintf (buf, "%03" PRIu32 "|%1" PRIu32 "|%04" PRIu32, bbt.bars,bbt.beats,bbt.ticks);
			print (1, 10, buf); 
		}

		if (speed >= 2.0) {
			sprintf (buf, "%03" PRIu32 "|%1" PRIu32 "|%02" PRIu32, bbt.bars,bbt.beats,bbt.ticks); 
			print (1, 12, buf); 
		} 

		TempoMap::Metric m (session->tempo_map().metric_at (where));
    
		// the lights stop working well at above 100 bpm so don't bother
		if(m.tempo().beats_per_minute() < 101.0 && (speed > 0.0)) {

			// something else can reset these, so we need to

			lights_pending[LightRecord] = false;
			lights_pending[LightAnysolo] = false;
			switch(last_beats) {
			case 1: if(last_ticks < 250 || last_ticks >= 0) lights_pending[LightRecord] = true; break;
			default: if(last_ticks < 250) lights_pending[LightAnysolo] = true;
			}
		}
	}
}

void
AlphatrackControlProtocol::show_transport_time ()
{
	nframes_t where = session->transport_frame();
	show_bbt(where);
}	

void
AlphatrackControlProtocol::show_smpte (nframes_t where)
{
	if ((where != last_where) || lcd_isdamaged(1,9,10)) {

		char buf[5];
		SMPTE::Time smpte;

		session->smpte_time (where, smpte);

		if (smpte.negative) {
			sprintf (buf, "-%02" PRIu32 ":", smpte.hours);
		} else {
			sprintf (buf, " %02" PRIu32 ":", smpte.hours);
		}
		print (1, 8, buf);

		sprintf (buf, "%02" PRIu32 ":", smpte.minutes);
		print (1, 12, buf);

		sprintf (buf, "%02" PRIu32 ":", smpte.seconds);
		print (1, 15, buf);

		sprintf (buf, "%02" PRIu32, smpte.frames);
		print_noretry (1, 18, buf); 

		last_where = where;
	}
}

void
AlphatrackControlProtocol::show_track_gain ()
{
// FIXME last_track gain has to become meter/track specific
	if (route_table[0]) {
		gain_t g = route_get_gain (0);
		if ((g != last_track_gain) || lcd_isdamaged(0,12,8)) {
			char buf[16]; 
			snprintf (buf, sizeof (buf), "%6.1fdB", coefficient_to_dB (route_get_effective_gain (0)));
			print (0, 12, buf); 
			last_track_gain = g;
		}
	} else {
		print (0, 9, "        "); 
	}
}

// FIXME: How to handle multiple alphatracks in a system?

XMLNode&
AlphatrackControlProtocol::get_state () 
{
	XMLNode* node = new XMLNode (X_("Protocol"));
	node->add_property (X_("name"), _name);
	return *node;
}

int
AlphatrackControlProtocol::set_state (const XMLNode& node)
{
	cout << "AlphatrackControlProtocol::set_state: active " << _active << endl;
	int retval = 0;

// I think I want to make these strings rather than numbers
#if 0		
	// fetch current display mode
	if ( node.property( X_("display_mode") ) != 0 )
	{
		string display = node.property( X_("display_mode") )->value();
		try
		{
			set_active( true );
			int32_t new_display = atoi( display.c_str() );
			if ( display_mode != new_display ) display_mode = (DisplayMode)new_display;
		}
		catch ( exception & e )
		{
			cout << "exception in AlphatrackControlProtocol::set_state: " << e.what() << endl;
			return -1;
		}
	}

	if ( node.property( X_("wheel_mode") ) != 0 )
	{
		string wheel = node.property( X_("wheel_mode") )->value();
		try
		{
			int32_t new_wheel = atoi( wheel.c_str() );
			if ( wheel_mode != new_wheel ) wheel_mode = (WheelMode) new_wheel;
		}
		catch ( exception & e )
		{
			cout << "exception in AlphatrackControlProtocol::set_state: " << e.what() << endl;
			return -1;
		}
	}

	// fetch current bling mode
	if ( node.property( X_("bling") ) != 0 )
	{
		string bling = node.property( X_("bling_mode") )->value();
		try
		{
			int32_t new_bling = atoi( bling.c_str() );
			if ( bling_mode != new_bling ) bling_mode = (BlingMode) new_bling;
		}
		catch ( exception & e )
		{
			cout << "exception in AlphatrackControlProtocol::set_state: " << e.what() << endl;
			return -1;
		}
	}
#endif 

	return retval;

}

// These are intended for the day we have more options for alphatrack modes
// And perhaps we could load up sessions this way, too

int
AlphatrackControlProtocol::save (char *name) 
{
	// Presently unimplemented
	return 0;
}

int
AlphatrackControlProtocol::load (char *name) 
{
	// Presently unimplemented
	return 0;
}

int
AlphatrackControlProtocol::save_config (char *name) 
{
	// Presently unimplemented
	return 0;
}

int
AlphatrackControlProtocol::load_config (char *name) 
{
	// Presently unimplemented
	return 0;
}

BaseUI::RequestType LEDChange = BaseUI::new_request_type ();
BaseUI::RequestType Print = BaseUI::new_request_type ();
BaseUI::RequestType SetCurrentTrack = BaseUI::new_request_type ();

// void
// AlphatrackControlProtocol::datawheel ()
// {
// 	if ((buttonmask & ButtonTrackRight) || (buttonmask & ButtonTrackLeft)) {

// 		/* track scrolling */
		
// 		if (_datawheel < WheelDirectionThreshold) {
// 			next_track ();
// 		} else {
// 			prev_track ();
// 		}

// 		timerclear (&last_wheel_motion);
		
// 	} else if ((buttonmask & ButtonPrev) || (buttonmask & ButtonNext)) {

// 		if (_datawheel < WheelDirectionThreshold) {
// 			next_marker ();
// 		} else {
// 			prev_marker ();
// 		}
		
// 		timerclear (&last_wheel_motion);
		
// 	} else if (buttonmask & ButtonShift) {
		
// 		/* parameter control */
		
// 		if (route_table[0]) {
// 			switch (wheel_shift_mode) {
// 			case WheelShiftGain:
// 				if (_datawheel < WheelDirectionThreshold) {
// 					step_gain_up ();
// 				} else {
// 					step_gain_down ();
// 				}
// 				break;
// 			case WheelShiftPan:
// 				if (_datawheel < WheelDirectionThreshold) {
// 					step_pan_right ();
// 				} else {
// 					step_pan_left ();
// 				}
// 				break;
				
// 			case WheelShiftMarker:
// 				break;
				
// 			case WheelShiftMaster:
// 				break;
				
// 			}
// 		}
		
// 		timerclear (&last_wheel_motion);
		
// 	} else {
		
// 		switch (wheel_mode) {
// 		case WheelTimeline:
// 			scroll ();
// 			break;
			
// 		case WheelScrub:
// 			scrub ();
// 			break;
			
// 		case WheelShuttle:
// 			shuttle ();
// 			break;
// 		}
// 	}
// }

// void
// AlphatrackControlProtocol::scroll ()
// {
// 	float m = 1.0;
// 	if (_datawheel < WheelDirectionThreshold) {
// 		m = 1.0;
// 	} else {
// 		m = -1.0;
// 	}
// 	switch(wheel_increment) {
// 	case WheelIncrScreen: ScrollTimeline (0.2*m); break;
// 	case WheelIncrSlave:
// 	case WheelIncrSample:
// 	case WheelIncrBeat:
// 	case WheelIncrBar:
// 	case WheelIncrSecond:
// 	case WheelIncrMinute:
// 	default: break; // other modes unimplemented as yet
// 	}
// }

// void
// AlphatrackControlProtocol::scrub ()
// {
// 	float speed;
// 	struct timeval now;
// 	struct timeval delta;
// 	int dir;
	
// 	gettimeofday (&now, 0);
	
// 	if (_datawheel < WheelDirectionThreshold) {
// 		dir = 1;
// 	} else {
// 		dir = -1;
// 	}
	
// 	if (dir != last_wheel_dir) {
// 		/* changed direction, start over */
// 		speed = 0.1f;
// 	} else {
// 		if (timerisset (&last_wheel_motion)) {

// 			timersub (&now, &last_wheel_motion, &delta);
			
// 			/* 10 clicks per second => speed == 1.0 */
			
// 			speed = 100000.0f / (delta.tv_sec * 1000000 + delta.tv_usec);
			
// 		} else {
			
// 			/* start at half-speed and see where we go from there */
			
// 			speed = 0.5f;
// 		}
// 	}
	
// 	last_wheel_motion = now;
// 	last_wheel_dir = dir;
	
// 	set_transport_speed (speed * dir);
// }

// void
// AlphatrackControlProtocol::shuttle ()
// {
// 	if (_datawheel < WheelDirectionThreshold) {
// 		if (session->transport_speed() < 0) {
// 			session->request_transport_speed (1.0);
// 		} else {
// 			session->request_transport_speed (session->transport_speed() + 0.1);
// 		}
// 	} else {
// 		if (session->transport_speed() > 0) {
// 			session->request_transport_speed (-1.0);
// 		} else {
// 			session->request_transport_speed (session->transport_speed() - 0.1);
// 		}
// 	}
// }

void
AlphatrackControlProtocol::next_wheel_shift_mode ()
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
AlphatrackControlProtocol::next_wheel_mode ()
{
	switch (wheel_mode) {
	case WheelTimeline:
		wheel_mode = WheelScrub;
		break;
	case WheelScrub:
		wheel_mode = WheelShuttle;
		break;
	case WheelShuttle:
		wheel_mode = WheelTimeline;
	}

	show_wheel_mode ();
}

void
AlphatrackControlProtocol::show_wheel_mode ()
{
	string text;

	if(session->transport_speed() != 0) {
		show_mini_meter(); 
	} else {
		
		switch (wheel_mode) {
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
		
		print (1, 0, text.c_str());
	} 
}
