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

/* This header file is basically where all the alphatrack debuggable options go.
   Try to only check it in with minimal debugging enabled so production
   systems don't have to fiddle with it. */

/* Design notes: The Alphatrack is a unique device, basically a 
   16x2 character lcd gui with (almost) 22 shift keys and 17 blinking lights. 

   The alphatrack kernel driver retries writes for you and also buffers and 
   compresses incoming fader events - it will rarely, if ever, drop data.

   A more complex surface might have hundreds of lights and several displays.

   mike@taht.net
*/

#ifndef ardour_alphatrack_base
#define ardour_alphatrack_base

#define DEFAULT_USB_TIMEOUT 10
#define MAX_RETRY 1
#define MAX_ALPHATRACK_INFLIGHT 4
#define DEBUG_ALPHATRACK 0
#define ALPHATRACK_THREADS 0

#ifndef HAVE_ALPHATRACK_KERNEL_DRIVER
#ifdef __linux__
#define HAVE_ALPHATRACK_KERNEL_DRIVER 1
#else
#define HAVE_ALPHATRACK_KERNEL_DRIVER 0
#endif
#endif

#ifndef HAVE_ALPHATRACK_MIDI_DRIVER
#define HAVE_ALPHATRACK_MIDI_DRIVER 0
#endif

// for now, this is what the device is called
#define ALPHATRACK_DEVICE "/dev/alphatrack0"

#if DEBUG_ALPHATRACK > 0 
#define DEBUG_ALPHATRACK_SCREEN 10
#define DEBUG_ALPHATRACK_BITS 10
#define DEBUG_ALPHATRACK_LIGHTS 10
#define DEBUG_ALPHATRACK_STATE 10
#else
#define DEBUG_ALPHATRACK 0
#define DEBUG_ALPHATRACK_BITS 0
#define DEBUG_ALPHATRACK_SCREEN 0
#define DEBUG_ALPHATRACK_LIGHTS 0
#define DEBUG_ALPHATRACK_STATE 0
#endif
#endif /* ardour_alphatrack_base */

