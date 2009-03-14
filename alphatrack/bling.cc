#include <math.h>
#include <stdlib.h>
#include "bling.h"

long Bling::run() {
  bool changed = false;
  if(_enabled && get_timestamp() > _next_interval) {
    BlingMode Colors;
    switch(_mode) {
    case BlingOff: do_off(); break;
    case BlingEnter: do_enter(); break;
    case BlingExit: do_exit(); break;
    case BlingKit: do_kit(); break;
    case BlingRotating: do_rotating(); break;
    case BlingPairs: do_pairs(); break;
    case BlingRows: do_rows(); break;
    case BlingFlashAll: do_flashall(); break;
    case BlingStar: do_star(); break;
    case BlingRandom: do_random(); break;
    case BlingCylon: do_cylon(); break;
    case BlingFall: do_fall(); break;
    case BlingRise: do_rise(); break;
    case BlingPong: do_pong(); break;
    case BlingFlashGreen: do_colors(BlingGreen); break;
    case BlingFlashRed: do_colors(BlingRed); break;
    case BlingFlashYellow: do_colors(BlingYellow); break;
    default: break;
    }
  }
  if(oldlights != newlights) {
    changed = true;
    oldlights = newlights;
    oldscreen = newscreen;
  }
  // return if anything actually changed
  return(changed);
}

unsigned long  Bling::get_lights() {}

void  Bling::do_star(){}
void  Bling::do_kit(){}
void  Bling::do_pairs(){}
void  Bling::do_rotate(){}
void  Bling::do_random(){}
void  Bling::do_rows(){}
void  Bling::do_enter(){}
void  Bling::do_exit(){}
void  Bling::do_flashall(){}
void  Bling::do_pong(){}
void  Bling::do_cylon() {}
void  Bling::do_rotating() {}
void  Bling::do_off() {}
void  Bling::do_on() {}
void  Bling::do_fall() {}
void  Bling::do_rise() {}
void  Bling::do_colors(BlingColors color) {}
long  Bling::get_timestamp() {}
