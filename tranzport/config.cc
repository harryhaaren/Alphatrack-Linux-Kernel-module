/* We need some limited communication with the editor for the
   snap to settings */

#include <tranzport_base.h>
#include <tranzport_common.h>
#include <tranzport_control_protocol.h>
#include <ardour/location.h>

#include "i18n.h"

using namespace Editing;

static inline void FUDGE_64BIT_INC(nframes64_t &a, int dir) {
  switch(dir) {
  case 0: break;
  case 1: a = MIN(++a,UINT_MAX); 
  case -1: a = ZEROIFNEG(--a);
  default: break;
  }
}

XMLNode *TranzportControlProtocol::editor_settings ()
{
        XMLNode* node = 0;

        if (session) {
                node = session->instant_xml(X_("Editor"), session->path());
//        } else {
//             node = Config->instant_xml(X_("Editor"), get_user_ardour_path());
        }
        return node;
}


SnapType TranzportControlProtocol::get_snapto () 
{
	const XMLProperty* prop;
	XMLNode *node = 0;
	if ((node = editor_settings()) == 0) {
	  snap_to = SnapToSMPTESeconds;
	  snap_mode = SnapNormal; // Grid
	  return snap_to;
	}

        if ((prop = node->property ("snap-to"))) {
	  snap_to = (SnapType) atoi (prop->value().c_str());
        }

        if ((prop = node->property ("snap-mode"))) {
          snap_mode = (SnapMode) atoi (prop->value().c_str());
        }

return (snap_to);
}

// Having these translated would be bad as we don't have 
// an international character set on the tranzport

const char * TranzportControlProtocol::snapto_string(SnapType snap) {
  switch (snap) {
  case SnapToCDFrame: return("CD Frame"); break;
  case SnapToSMPTEFrame: return("SMPTE Frame"); break;
  case SnapToSMPTESeconds: return("SMPTE Secs"); break;
  case SnapToSMPTEMinutes: return("SMTPE Mins"); break;
  case SnapToSeconds: return("Seconds"); break;
  case SnapToMinutes: return("Minutes"); break;
  case SnapToBar:     return("Bars   "); break;
  case SnapToBeat:    return("Beats  "); break;
  case SnapToAThirtysecondBeat: return("1/32 Beat"); break;
  case SnapToASixteenthBeat: return("1/16 Beat"); break;
  case SnapToAEighthBeat:  return(" 1/8 Beat"); break;
  case SnapToAQuarterBeat: return(" 1/4 Beat"); break;
  case SnapToAThirdBeat: return(" 1/3 Beat"); break;
  case SnapToMark:       return(" Marks   "); break;
  case SnapToRegionStart:return("Reg Start"); break;
  case SnapToRegionEnd: return("Reg End  "); break;
  case SnapToRegionSync: return("Reg Sync "); break;
  case SnapToRegionBoundary: return("Reg Bound"); break;
  default: return("Unk Snap"); break;
  }
 return("Unk Snap"); /* not reached */
}  

void
TranzportControlProtocol::next_snapto_mode ()
{
  switch (snap_to) {
  case SnapToCDFrame: snap_to = SnapToSMPTEFrame; break;
  case SnapToSMPTEFrame: snap_to = SnapToSMPTESeconds; break;
  case SnapToSMPTESeconds: snap_to = SnapToSMPTEMinutes; break;
  case SnapToSMPTEMinutes: snap_to = SnapToSeconds; break;
  case SnapToSeconds: snap_to = SnapToMinutes; break;
  case SnapToMinutes: snap_to = SnapToBar; break;
  case SnapToBar: snap_to = SnapToBeat; break;
  case SnapToBeat: snap_to = SnapToAThirtysecondBeat; break;
  case SnapToAThirtysecondBeat: snap_to = SnapToASixteenthBeat; break;
  case SnapToASixteenthBeat: snap_to = SnapToAEighthBeat; break;
  case SnapToAEighthBeat: snap_to = SnapToAQuarterBeat; break;
  case SnapToAQuarterBeat: snap_to = SnapToAThirdBeat; break;
  case SnapToAThirdBeat: snap_to = SnapToMark; break;
  case SnapToMark: snap_to = SnapToCDFrame; break;
    // Haven't figured these out yet
  case SnapToRegionStart:
  case SnapToRegionEnd:
  case SnapToRegionSync:
  case SnapToRegionBoundary: 
  default: snap_to = SnapToCDFrame; break;
  }
  string note = string_compose("SnapTo: %1", snapto_string(snap_to));
  notify(note.c_str()); 
}

/*
        snprintf (buf, sizeof(buf), "%d", (int) snap_type);
        node->add_property ("snap-to", buf);
        snprintf (buf, sizeof(buf), "%d", (int) snap_mode);
        node->add_property ("snap-mode", buf);

       if (regions.size() == 1) {
                switch (snap_type) {
                case SnapToRegionStart:
                case SnapToRegionSync:
                case SnapToRegionEnd:
                        break;
                default:
                        snap_to (where);
                }
        } else {
                snap_to (where);
        }

*/


void
TranzportControlProtocol::go_snap_to (nframes64_t& start, int32_t direction, bool for_mark)
{
  if (!session || snap_mode == SnapOff) {
		return;
	}

	snap_to_internal (start, direction, for_mark);
}

/* Don't understand what these do yet

nframes64_t unit_to_frame (double unit) const {
  return (nframes64_t) rint (unit * frames_per_unit);
}

double frame_to_unit (nframes64_t frame) const {
  return rint ((double) frame / (double) frames_per_unit);
}

double frame_to_unit (double frame) const {
  return rint (frame / frames_per_unit);
}

*/

// Ardour strives to be correct in all cases.
// This strives to be FAST... and 99.999% correct
// (actually I'd like to see how correct it is for inputs of -127 ... 127)
// Get tempo
// Calculate bar width at our sample rate
// If the same as cached
// multiply request by the distance, multiply by the subdivision's reciprocol, round to nearest
// then let ardour be pendantic and round to the closest beat on that subdivision
// call it a day

nframes64_t
TranzportControlProtocol::snap_to_beat_subdivision(nframes64_t start, SnapType snap, int32_t direction) 
{
  // static TempoMap t;
  //  TempoMap temp(start);
  double subdivision = 0.0;
  int r = 0;
  double distance = 0.0 ; //   Beatsamples = temp->distance;

  switch(snap) {
  case SnapToBar: r = 1; subdivision = 1.0; break;
  case SnapToBeat: r = 4; subdivision = 1.0/4.0; break; // FIXME for weird time signatures 
  case SnapToAThirtysecondBeat: r = 32; subdivision = 1.0/32.0; break;
  case SnapToASixteenthBeat: r = 16; subdivision = 1.0/16.0 ; break;
  case SnapToAEighthBeat: r = 8; subdivision = 1.0/8.0; break;
  case SnapToAQuarterBeat: r = 4; subdivision = 1.0/4.0 ; break;
  case SnapToAThirdBeat: r = 3; subdivision= 1.0/3.0; break;
  default: break;
  }

  start += lrint(distance * direction * subdivision);
  // MINMAXCHECK() pesky unsigned ints
  start = session->tempo_map().round_to_beat_subdivision(start,r);
  //  MINMAXCHECK() pesky unsigned ints;
  return start;
}

// FIXME: This is big and buggy. Break it apart.

void
TranzportControlProtocol::snap_to_internal (nframes64_t& start, int32_t direction, bool for_mark)
{
  if(direction == 0 || !session) { return; }
  nframes64_t newstart = start;
  int32_t dir = 0 ;
  if(direction < 0) dir = -1;
  if(direction > 0) dir = 1;

  ARDOUR::Location* before = 0;
  ARDOUR::Location* after = 0;

  const nframes64_t one_second = session->frame_rate();
  const nframes64_t one_minute = session->frame_rate() * 60;
  const nframes64_t one_smpte_second = 
	(nframes64_t)(rint(session->smpte_frames_per_second()) *
	session->frames_per_smpte_frame());
  nframes64_t one_smpte_minute = 
	(nframes64_t)(rint(session->smpte_frames_per_second()) * 
	session->frames_per_smpte_frame() * 60);
  nframes64_t presnap = start;
  float speed = session->transport_speed();

  // FIXME: When the transport is moving it does you very little good
  // to try to move by CD frames or SMPTE Frames, or anything less than a bar.

  if(speed != 0.0) {
    switch(snap_to) {
    case SnapToSMPTESeconds: break;
    case SnapToSMPTEMinutes: break;
    case SnapToSeconds: break;
    case SnapToMinutes: break;
    case SnapToMark: break;
    case SnapToBar: 
    case SnapToBeat: 
    case SnapToAThirtysecondBeat: 
    case SnapToASixteenthBeat: 
    case SnapToAEighthBeat: 
    case SnapToAQuarterBeat: 
    case SnapToAThirdBeat: 
      start = snap_to_beat_subdivision(start,snap_to,direction); return; break;

    case SnapToRegionStart: 
    case SnapToRegionEnd: 
    case SnapToRegionSync: 
    case SnapToRegionBoundary: notify("No snap to regions"); break;
    default: break;
    }
  } else {

  switch (snap_to) {
  case SnapToCDFrame:
    if(snap_mode == SnapOff) {
      FUDGE_64BIT_INC(start,dir); // move by samples instead
    } else {
      FUDGE_64BIT_INC(start,dir); // FIXME move by CD frames instead?

      if (((dir == 0) && 
	   (start % (one_second/75) > (one_second/75) / 2)) || 
	   (dir > 0)) {
		start = (nframes64_t) ceil ((double) start / (one_second / 75)) * (one_second / 75);
      } else {
	start = (nframes64_t) floor ((double) start / (one_second / 75)) * (one_second / 75);
      }
    } 
    break;
    
  case SnapToSMPTEFrame:
      FUDGE_64BIT_INC(start,dir); 
    if (((dir == 0) && (fmod((double)start, (double)session->frames_per_smpte_frame()) > (session->frames_per_smpte_frame() / 2))) || (dir > 0)) {
      start = (nframes64_t) (ceil ((double) start / session->frames_per_smpte_frame()) * session->frames_per_smpte_frame());
    } else {
      start = (nframes64_t) (floor ((double) start / session->frames_per_smpte_frame()) *  session->frames_per_smpte_frame());
    }
    break;
    
  case SnapToSMPTESeconds:
    FUDGE_64BIT_INC(start,dir); 
    if (session->smpte_offset_negative())
      {
	start += session->smpte_offset ();
      } else {
      start -= session->smpte_offset ();
    }    
    if (((dir == 0) && (start % one_smpte_second > one_smpte_second / 2)) || dir > 0) {
      start = (nframes64_t) ceil ((double) start / one_smpte_second) * one_smpte_second;
    } else {
      start = (nframes64_t) floor ((double) start / one_smpte_second) * one_smpte_second;
    }
    
    if (session->smpte_offset_negative())
      {
	start -= session->smpte_offset ();
      } else {
      start += session->smpte_offset ();
    }
    break;
    
  case SnapToSMPTEMinutes:
    FUDGE_64BIT_INC(start,dir); 
    if (session->smpte_offset_negative())
      {
	start += session->smpte_offset ();
      } else {
      start -= session->smpte_offset ();
    }
    if (((dir == 0) && (start % one_smpte_minute > one_smpte_minute / 2)) || dir > 0) {
      start = (nframes64_t) ceil ((double) start / one_smpte_minute) * one_smpte_minute;
    } else {
      start = (nframes64_t) floor ((double) start / one_smpte_minute) * one_smpte_minute;
    }
    if (session->smpte_offset_negative())
      {
	start -= session->smpte_offset ();
      } else {
      start += session->smpte_offset ();
    }
    break;
    
  case SnapToSeconds:
    FUDGE_64BIT_INC(start,dir);
    if (((dir == 0) && (start % one_second > one_second / 2)) || (dir > 0)) {
      start = (nframes64_t) ceil ((double) start / one_second) * one_second;
    } else {
      start = (nframes64_t) floor ((double) start / one_second) * one_second;
    }
    break;
    
  case SnapToMinutes:
          FUDGE_64BIT_INC(start,dir);
if (((dir == 0) && (start % one_minute > one_minute / 2)) || (dir > 0)) {
      start = (nframes64_t) ceil ((double) start / one_minute) * one_minute;
    } else {
      start = (nframes64_t) floor ((double) start / one_minute) * one_minute;
    }
    break;
    
  case SnapToBar:
        FUDGE_64BIT_INC(start,dir);
	start = session->tempo_map().round_to_bar (start, dir);
    break;
    
  case SnapToBeat:
    FUDGE_64BIT_INC(start,dir);
    newstart = start;
    start = session->tempo_map().round_to_beat (start, dir);
    break;
    
  case SnapToAThirtysecondBeat:
    FUDGE_64BIT_INC(start,dir);
    start = session->tempo_map().round_to_beat_subdivision (start, 32);
    break;
    
  case SnapToASixteenthBeat:
    FUDGE_64BIT_INC(start,dir);
    start = session->tempo_map().round_to_beat_subdivision (start, 16);
    break;
    
  case SnapToAEighthBeat:
    FUDGE_64BIT_INC(start,dir);
    start = session->tempo_map().round_to_beat_subdivision (start, 8);
    break;
    
  case SnapToAQuarterBeat:
    FUDGE_64BIT_INC(start,dir);
    start = session->tempo_map().round_to_beat_subdivision (start, 4);
    break;
    
  case SnapToAThirdBeat:
    FUDGE_64BIT_INC(start,dir);
    start = session->tempo_map().round_to_beat_subdivision (start, 3);
    break;
    
  case SnapToMark:
    if (for_mark) {
      return;
    }
    FUDGE_64BIT_INC(start,dir);
    
    before = session->locations()->first_location_before (start);
    after = session->locations()->first_location_after (start);

    if (dir < 0) {
      if (before) {
	start = before->start();
      } else {
	start = 0;
      }
    } else if (dir > 0) {
      if (after) {
	start = after->start();
      } else {
	start = session->current_end_frame();
      }
    } else {
      if (before) {
	if (after) {
	  /* find nearest of the two */
	  if ((start - before->start()) < (after->start() - start)) {
	    start = before->start();
	  } else {
	    start = after->start();
	  }
	} else {
	  start = before->start();
	}
      } else if (after) {
	start = after->start();
      } else {
	/* relax */
      }
    }
    break;

  case SnapToRegionStart:
  case SnapToRegionEnd:
  case SnapToRegionSync:
  case SnapToRegionBoundary: notify("No snap to regions"); 
    /*		if (!region_boundary_cache.empty()) {
		vector<nframes64_t>::iterator i;
		
		if (direction > 0) {
		i = std::upper_bound (region_boundary_cache.begin(), region_boundary_cache.end(), start);
		} else {
		i = std::lower_bound (region_boundary_cache.begin(), region_boundary_cache.end(), start);
		}
		
		if (i != region_boundary_cache.end()) {
		
		// lower bound doesn't quite to the right thing for our purposes 
		
		if (direction < 0 && i != region_boundary_cache.begin()) {
		--i;
		}
		
		start = *i;
		
		} else {
		start = region_boundary_cache.back();
		}
		} 
    */
    break;
  }
  
/* don't pay attention to the snap_mode for now

	switch (snap_mode) {
	case SnapNormal:
		return;			
		
	case SnapMagnetic:
		
		if (presnap > start) {
			if (presnap > (start + unit_to_frame(snap_threshold))) {
				start = presnap;
			}
			
		} else if (presnap < start) {
			if (presnap < (start - unit_to_frame(snap_threshold))) {
				start = presnap;
			}
		}
		
	default:
		return;
		
	}
*/

/* FIXME: Recursion, with a reference var, no less. No mi gusta. 
   The right way to fix this would be to extend the various snapto
   calls in libardour to treat the direction argument as a count.

   This would also do nice things for the disk butler which otherwise
   is going nuts trying to keep up with a very rapid string of requests.
*/

	if(dir > 0) { 
	  --direction; // start++;  
	  snap_to_internal(start, direction, for_mark); 
	} else {
	  if(dir < 0) {
	    ++direction; // start--;
	  snap_to_internal(start, direction, for_mark); 
	  }
	}
  }
}


