/*
 * tranzport 0.1 <tranzport.sf.net>
 * oct 18, 2005
 * arthur@artcmusic.com
 * 
 * updated work with Alphatrack
 * july 06, 2011 by Harry van Haaren <harryhaaren@gmail.com>
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <malloc.h>

#define VENDORID  0x165b
#define PRODUCTID 0x8101

#define READ_ENDPOINT  0x81
#define WRITE_ENDPOINT 0x02

#define LIGHT_ANY_SOLO        0x0F
#define LIGHT_AUTO_WRITE      0x0E
#define LIGHT_AUTO_READ       0x0D

#define LIGHT_TRACKREC        0x0C
#define LIGHT_SOLO            0x0B
#define LIGHT_MUTE            0x0A
#define LIGHT_SHIFT           0x09

#define LIGHT_PAN             0x06
#define LIGHT_SEND            0x03
#define LIGHT_EQ              0x00
#define LIGHT_PLUGIN          0x14
#define LIGHT_AUTO            0x10

#define LIGHT_F1              0x05
#define LIGHT_F2              0x02
#define LIGHT_F3              0x15
#define LIGHT_F4              0x11

#define LIGHT_LEFT            0x04
#define LIGHT_RIGHT           0x01
#define LIGHT_LOOP            0x16
#define LIGHT_FLIP            0x13

#define LIGHT_RECORD          0x12


#define BUTTONMASK_POT_LEFT    0x00000008
#define BUTTONMASK_POT_MIDDLE  0x00000010
#define BUTTONMASK_POT_RIGHT   0x00000020

#define BUTTONMASK_PAN         0x00000200
#define BUTTONMASK_SEND        0x00000800
#define BUTTONMASK_EQ          0x00004000
#define BUTTONMASK_PLUGIN      0x00000400
#define BUTTONMASK_AUTO        0x00000100

#define BUTTONMASK_F1          0x00100000
#define BUTTONMASK_F2          0x00400000
#define BUTTONMASK_F3          0x00200000
#define BUTTONMASK_F4          0x00080000

#define BUTTONMASK_TRACKLEFT   0x80000000
#define BUTTONMASK_TRACKRIGHT  0x00020000
#define BUTTONMASK_TRACKREC    0x00001000
#define BUTTONMASK_TRACKMUTE   0x00040000
#define BUTTONMASK_TRACKSOLO   0x00800000
#define BUTTONMASK_LOOP        0x00010000
#define BUTTONMASK_FLIP        0x40000000
#define BUTTONMASK_PREV        0x00020000
#define BUTTONMASK_NEXT        0x00000200
#define BUTTONMASK_REWIND      0x01000000
#define BUTTONMASK_FASTFORWARD 0x04000000
#define BUTTONMASK_STOP        0x10000000
#define BUTTONMASK_PLAY        0x08000000
#define BUTTONMASK_RECORD      0x02000000
#define BUTTONMASK_SHIFT       0x20000000

#define BUTTONMASK_STRIP_SINGLE_TOUCH 0x00000002
#define BUTTONMASK_STRIP_DOUBLE_TOUCH 0x20000004

#define STATUS_OFFLINE 0xff
#define STATUS_ONLINE  0x01
#define STATUS_OK      0x00

struct tranzport_s {
	int *dev;
	int udev;
};

typedef struct tranzport_s tranzport_t;

void log_entry(FILE *fp, char *format, va_list ap)
{
	vfprintf(fp, format, ap);
	fputc('\n', fp);
}

void log_error(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	log_entry(stderr, format, ap);
	va_end(ap);
}

void vlog_error(char *format, va_list ap)
{
	log_entry(stderr, format, ap);
}

void die(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vlog_error(format, ap);
	va_end(ap);
	exit(1);
}

tranzport_t *open_tranzport_core()
{
	tranzport_t *z;

	z = malloc(sizeof(tranzport_t));
	if (!z)
		die("not enough memory");
	memset(z, 0, sizeof(tranzport_t));

	z->udev = open("/dev/alphatrack0",O_RDWR);
	if (!z->udev)
		die("unable to open tranzport");

	return z;
}

tranzport_t *open_tranzport()
{
return open_tranzport_core();	
}

void close_tranzport(tranzport_t *z)
{
	int val;

	val = close(z->udev);
	if (val < 0)
		log_error("unable to release tranzport");

	free(z);
}

int tranzport_write_core(tranzport_t *z, uint8_t *cmd, int timeout)
{
	int val;
	val = write(z->udev, cmd, 8);
	if (val < 0)
		return val;
	if (val != 8)
		return -1;
	return 0;
}

int tranzport_lcdwrite(tranzport_t *z, uint8_t cell, char *text, int timeout)
{
	uint8_t cmd[8];

	if (cell > 9) {
		return -1;
	}

	cmd[0] = 0x00;
	cmd[1] = 0x01;
	cmd[2] = cell;
	cmd[3] = text[0];
	cmd[4] = text[1];
	cmd[5] = text[2];
	cmd[6] = text[3];
	cmd[7] = 0x00;

	return tranzport_write_core(z, cmd, timeout);
}

int tranzport_lighton(tranzport_t *z, uint8_t light, int timeout)
{
	uint8_t cmd[8];

	cmd[0] = 0x00;
	cmd[1] = 0x00;
	cmd[2] = light;
	cmd[3] = 0x01;
	cmd[4] = 0x00;
	cmd[5] = 0x00;
	cmd[6] = 0x00;
	cmd[7] = 0x00;

	return tranzport_write_core(z, &cmd[0], timeout);
}

int tranzport_lightoff(tranzport_t *z, uint8_t light, int timeout)
{
	uint8_t cmd[8];

	cmd[0] = 0x00;
	cmd[1] = 0x00;
	cmd[2] = light;
	cmd[3] = 0x00;
	cmd[4] = 0x00;
	cmd[5] = 0x00;
	cmd[6] = 0x00;
	cmd[7] = 0x00;

	return tranzport_write_core(z, &cmd[0], timeout);
}

int tranzport_read(tranzport_t *z, uint8_t *status, uint32_t *buttons, uint8_t *fader, int timeout)
{
	uint8_t buf[12];
	int val;
	
	memset(buf, 0xff, 12);
	
	val = read(z->udev, buf, 12);
	
	if (val < 0) {
		//printf("errno: %d\n",errno);
		return val;
	}
	else
	{
		printf ("tranzport_read: val from read = %i\n", val);
	}
	
	// Alphatrack device seems to use 12 rather than 8 for the tranzport
	if (val != 12)
		return -1;
	
	printf("read: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9], buf[10], buf[11]);
	
	*status = buf[1];
	
	*buttons = 0;
	*buttons |= buf[2] << 24;
	*buttons |= buf[3] << 16;
	*buttons |= buf[4] << 8;
	*buttons |= buf[5];

	*fader = buf[6];

	return 0;
}

void lights_core(tranzport_t *z, uint32_t buttons, uint32_t buttonmask, uint8_t light)
{
	if (buttons & buttonmask) {
		if (buttons & BUTTONMASK_SHIFT) {
			tranzport_lightoff(z, light, 1000);
		} else {
			tranzport_lighton(z, light, 1000);
		}
	}
}

void do_lights(tranzport_t *z, uint32_t buttons)
{
	lights_core(z, buttons, BUTTONMASK_RECORD, LIGHT_RECORD);
	/*
	lights_core(z, buttons, BUTTONMASK_TRACKREC, LIGHT_TRACKREC);
	lights_core(z, buttons, BUTTONMASK_TRACKMUTE, LIGHT_TRACKMUTE);
	lights_core(z, buttons, BUTTONMASK_TRACKSOLO, LIGHT_TRACKSOLO);
	lights_core(z, buttons, BUTTONMASK_TRACKSOLO, LIGHT_ANYSOLO);
	lights_core(z, buttons, BUTTONMASK_LOOP, LIGHT_LOOP); */
}

void buttons_core(tranzport_t *z, uint32_t buttons, uint32_t buttonmask, char *str)
{
	if (buttons & buttonmask)
		printf(" %s", str);
}

void do_buttons(tranzport_t *z, uint32_t buttons, uint8_t fader)
{
	printf("buttons: %x ", buttons);
	
	buttons_core(z, buttons, BUTTONMASK_POT_LEFT, "pot-left");
	buttons_core(z, buttons, BUTTONMASK_POT_MIDDLE, "pot-middle");
	buttons_core(z, buttons, BUTTONMASK_POT_RIGHT, "pot-right");
	
	buttons_core(z, buttons, BUTTONMASK_PAN, "pan");
	buttons_core(z, buttons, BUTTONMASK_SEND, "send");
	buttons_core(z, buttons, BUTTONMASK_EQ, "eq");
	buttons_core(z, buttons, BUTTONMASK_PLUGIN, "plugin");
	buttons_core(z, buttons, BUTTONMASK_AUTO, "auto");

	buttons_core(z, buttons, BUTTONMASK_F1, "f1");
	buttons_core(z, buttons, BUTTONMASK_F2, "f2");
	buttons_core(z, buttons, BUTTONMASK_F3, "f3");
	buttons_core(z, buttons, BUTTONMASK_F4, "f4");

	buttons_core(z, buttons, BUTTONMASK_TRACKLEFT, "trackleft");
	buttons_core(z, buttons, BUTTONMASK_TRACKRIGHT, "trackright");
	buttons_core(z, buttons, BUTTONMASK_TRACKREC, "trackrec");
	buttons_core(z, buttons, BUTTONMASK_TRACKMUTE, "trackmute");
	buttons_core(z, buttons, BUTTONMASK_TRACKSOLO, "tracksolo");
	buttons_core(z, buttons, BUTTONMASK_LOOP, "loop");
	buttons_core(z, buttons, BUTTONMASK_FLIP, "flip");
	buttons_core(z, buttons, BUTTONMASK_REWIND, "rewind");
	buttons_core(z, buttons, BUTTONMASK_FASTFORWARD, "fastforward");
	buttons_core(z, buttons, BUTTONMASK_STOP, "stop");
	buttons_core(z, buttons, BUTTONMASK_PLAY, "play");
	buttons_core(z, buttons, BUTTONMASK_RECORD, "record");
	buttons_core(z, buttons, BUTTONMASK_SHIFT, "shift");
	
	buttons_core(z, buttons, BUTTONMASK_STRIP_SINGLE_TOUCH, "strip-single");
	buttons_core(z, buttons, BUTTONMASK_STRIP_DOUBLE_TOUCH, "strip-double");
	
	if (fader)
		printf(" Fader=%02x", fader);
	printf("\n");
}

void do_lcd(tranzport_t *z)
{
	tranzport_lcdwrite(z, 0, "  TE", 1000);
	tranzport_lcdwrite(z, 1, "ST T", 1000);
	tranzport_lcdwrite(z, 2, "EXT ", 1000);
	tranzport_lcdwrite(z, 3, "FOR ", 1000);
	tranzport_lcdwrite(z, 4, "   A", 1000);
	tranzport_lcdwrite(z, 5, "LPHA", 1000);
	tranzport_lcdwrite(z, 6, "TRAC", 1000);
	tranzport_lcdwrite(z, 7, "K   ", 1000);
}

void lights_off(tranzport_t *z) {
int i;
	for(i=0;i<23;i++) {
	tranzport_lightoff(z, i, 1000);
	}
}

void lights_on(tranzport_t *z) {
	int i;
	for(i=0;i<23;i++) {
	tranzport_lighton(z, i, 1000);
	sleep(1);
	}
}

int main()
{
	tranzport_t *z;
	uint8_t status = -1;
	uint32_t buttons = -1;
	uint8_t fader = -1;
	int val = -1;

	z = open_tranzport();

	//do_lcd(z);
	
	lights_off(z);

	for(;;)
	{
		do_lcd(z);
		//lights_on(z);
		//do_lcd2(z);
		//lights_off(z);
		
		val = tranzport_read(z, &status, &buttons, &fader, 6000);
		
		if ( status == -1 || buttons == -1 || fader == -1)
		{
			printf("error in tranzport_read(), returning -1");
		}
		
		if (val < 0)
			continue;
		
		if (status == STATUS_OFFLINE) {
			printf("offline: ");
			continue;
		}
		
		if (status == STATUS_ONLINE) {
			printf("online: ");
			do_lcd(z);
		}
		
		do_lights(z, buttons);
		do_buttons(z, buttons, fader);
	}
	
	close_tranzport(z);
	
	return 0;
}

