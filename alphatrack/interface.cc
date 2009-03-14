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

#include <control_protocol/control_protocol.h>
#include "alphatrack_control_protocol.h"

using namespace ARDOUR;

ControlProtocol*
new_alphatrack_protocol (ControlProtocolDescriptor* descriptor, Session* s)
{
	AlphatrackControlProtocol* tcp = new AlphatrackControlProtocol (*s);

	if (tcp->set_active (true)) {
		delete tcp;
		return 0;
	}

	return tcp;
	
}

void
delete_alphatrack_protocol (ControlProtocolDescriptor* descriptor, ControlProtocol* cp)
{
	delete cp;
}

bool
probe_alphatrack_protocol (ControlProtocolDescriptor* descriptor)
{
	return AlphatrackControlProtocol::probe();
}

static ControlProtocolDescriptor alphatrack_descriptor = {
	name : "Alphatrack",
	id : "uri://ardour.org/surfaces/alphatrack:0",
	ptr : 0,
	module : 0,
	mandatory : 0,
	supports_feedback : true,
	probe : probe_alphatrack_protocol,
	initialize : new_alphatrack_protocol,
	destroy : delete_alphatrack_protocol
};
	

extern "C" {
ControlProtocolDescriptor* 
protocol_descriptor () {
	return &alphatrack_descriptor;
}
}

