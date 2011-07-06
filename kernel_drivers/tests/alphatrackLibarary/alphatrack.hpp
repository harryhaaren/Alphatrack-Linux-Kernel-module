
/*
 * This file is an interface to read / write an Alphatrack,
 * using the kernel module now included in the linux kernel.
 * 
 * Author: Harry van Haaren, July 2011
 * 
 * This file takes inspiration from arthur@artcmusic.com 's
 * work on the tranzport.  tranzport 0.1 <tranzport.sf.net>
 */

#include <fstream>
#include <iostream>

#define ALPHATRACK_VENDORID  0x165b
#define ALPHATRACK_PRODUCTID 0x8101

enum AlphatrackLed
{
	ALPHATRACK_LIGHT_EQ = 0,
	ALPHATRACK_LIGHT_RIGHT,
	ALPHATRACK_LIGHT_F2,
	ALPHATRACK_LIGHT_SEND,
	ALPHATRACK_LIGHT_LEFT,
	ALPHATRACK_LIGHT_F1,
	ALPHATRACK_LIGHT_PAN,
	ALPHATRACK_LIGHT_SHIFT = 9, // dunno what happens 8
	ALPHATRACK_LIGHT_MUTE,
	ALPHATRACK_LIGHT_SOLO,
	ALPHATRACK_LIGHT_TRACKREC,
	ALPHATRACK_LIGHT_AUTO_READ,
	ALPHATRACK_LIGHT_AUTO_WRITE,
	ALPHATRACK_LIGHT_ANY_SOLO,
	ALPHATRACK_LIGHT_AUTO,
	ALPHATRACK_LIGHT_F4,
	ALPHATRACK_LIGHT_RECORD,
	ALPHATRACK_LIGHT_FLIP,
	ALPHATRACK_LIGHT_PLUGIN,
	ALPHATRACK_LIGHT_F3,
	ALPHATRACK_LIGHT_LOOP
}; 


#define ALPHATRACK_BUTTONMASK_POT_LEFT    0x00000008
#define ALPHATRACK_BUTTONMASK_POT_MIDDLE  0x00000010
#define ALPHATRACK_BUTTONMASK_POT_RIGHT   0x00000020

#define ALPHATRACK_BUTTONMASK_PAN         0x00000200
#define ALPHATRACK_BUTTONMASK_SEND        0x00000800
#define ALPHATRACK_BUTTONMASK_EQ          0x00004000
#define ALPHATRACK_BUTTONMASK_PLUGIN      0x00000400
#define ALPHATRACK_BUTTONMASK_AUTO        0x00000100

#define ALPHATRACK_BUTTONMASK_F1          0x00100000
#define ALPHATRACK_BUTTONMASK_F2          0x00400000
#define ALPHATRACK_BUTTONMASK_F3          0x00200000
#define ALPHATRACK_BUTTONMASK_F4          0x00080000

#define ALPHATRACK_BUTTONMASK_TRACKLEFT   0x80000000
#define ALPHATRACK_BUTTONMASK_TRACKRIGHT  0x00020000
#define ALPHATRACK_BUTTONMASK_TRACKREC    0x00001000
#define ALPHATRACK_BUTTONMASK_TRACKMUTE   0x00040000
#define ALPHATRACK_BUTTONMASK_TRACKSOLO   0x00800000
#define ALPHATRACK_BUTTONMASK_LOOP        0x00010000
#define ALPHATRACK_BUTTONMASK_FLIP        0x40000000
#define ALPHATRACK_BUTTONMASK_PREV        0x00020000
#define ALPHATRACK_BUTTONMASK_NEXT        0x00000200
#define ALPHATRACK_BUTTONMASK_REWIND      0x01000000
#define ALPHATRACK_BUTTONMASK_FASTFORWARD 0x04000000
#define ALPHATRACK_BUTTONMASK_STOP        0x10000000
#define ALPHATRACK_BUTTONMASK_PLAY        0x08000000
#define ALPHATRACK_BUTTONMASK_RECORD      0x02000000
#define ALPHATRACK_BUTTONMASK_SHIFT       0x20000000

#define ALPHATRACK_BUTTONMASK_STRIP_SINGLE_TOUCH 0x00000002
#define ALPHATRACK_BUTTONMASK_STRIP_DOUBLE_TOUCH 0x20000004

#define ALPHATRACK_STATUS_OFFLINE 0xff
#define ALPHATRACK_STATUS_ONLINE  0x01
#define ALPHATRACK_STATUS_OK      0x00


class Alphatrack
{
	public:
		
		std::ofstream file;
		
		Alphatrack(){};
		
		void connect(std::string name) {
			file.open("/dev/alphatrack0", std::ios::out | std::ios::app );
			if ( file.good() )
			{
				std::cout << "Connected to Alphatrack!" << std::endl;
				lightOn( ALPHATRACK_LIGHT_PAN );
				lightOn( ALPHATRACK_LIGHT_SEND );
				lightOn( ALPHATRACK_LIGHT_EQ );
				lightOn( ALPHATRACK_LIGHT_PLUGIN);
				lightOn( ALPHATRACK_LIGHT_AUTO );
				printToScreen(name);
				sleep(0.03);
				lightOff( ALPHATRACK_LIGHT_PAN );
				lightOff( ALPHATRACK_LIGHT_SEND );
				lightOff( ALPHATRACK_LIGHT_EQ );
				lightOff( ALPHATRACK_LIGHT_PLUGIN);
				lightOff( ALPHATRACK_LIGHT_AUTO );
			}
			else
			{
				std::cout << "Error connecting to Alphatrack! Check permissions for /dev/alphatrack0" << std::endl;
			}
						
		}
		
		void allLightsOn()
		{
			for(int i = 0; i < 23; i++)
				lightOn(i);
		}
		void allLightsOff()
		{
			for(int i = 0; i < 23; i++)
				lightOff(i);
		}
		
		void lightOn(int lightNum)
		{
			char cmd[8];
			
			cmd[0] = 0x00;
			cmd[1] = 0x00;
			cmd[2] = lightNum;
			cmd[3] = 0x01;
			cmd[4] = 0x00;
			cmd[5] = 0x00;
			cmd[6] = 0x00;
			cmd[7] = 0x00;
			
			write( &cmd[0] );
		}
		void lightOff(int lightNum)
		{
			char cmd[8];
			cmd[0] = 0x00;
			cmd[1] = 0x00;
			cmd[2] = lightNum;
			cmd[3] = 0x00;
			cmd[4] = 0x00;
			cmd[5] = 0x00;
			cmd[6] = 0x00;
			cmd[7] = 0x00;
			
			write( &cmd[0] );
		}
		
		void printToScreen(std::string text)
		{
			char cmd[8];
			
			// write string
			for (int i = 0; i < 4; i++)
			{
				cmd[0] = 0x00;
				cmd[1] = 0x01;
				cmd[2] = (char)i;
				if ( i*4 < text.size() )
				{
					cmd[3] = text[i*4];
					if ((i*4)+1 < text.size()){ cmd[4] = text[(i*4)+1]; } else { cmd[4] = ' '; }
					if ((i*4)+2 < text.size()){ cmd[5] = text[(i*4)+2]; } else { cmd[5] = ' '; }
					if ((i*4)+3 < text.size()){ cmd[6] = text[(i*4)+3]; } else { cmd[6] = ' '; }
				}
				else
				{
					cmd[3] = ' ';
					cmd[4] = ' ';
					cmd[5] = ' ';
					cmd[6] = ' ';
				}
				
				cmd[7] = 0x00;
				
				write( &cmd[0] );
			}
		}
	
	private:
		
		void write(char* cmd)
		{
			file.write( &cmd[0], 8);
			file << std::endl;
			
			if ( file.bad() )
				std::cout << "Error in writing message" << std::endl;
			else
				std::cout << "Writing message DONE!" << std::endl;
			
			return ;
		}
		
		
		
		
};










