enum BlingColors {
    BlingGreen,
    BlingRed,
    BlingYellow,
    BlingBlue
  };
   

class Bling {
 public:
  enum BlingMode {
    BlingOff,
    BlingKit,
    BlingRotating,
    BlingPairs,
    BlingRows,
    BlingFlashAll,
    BlingStar,
    BlingRandom,
    BlingFall,
    BlingRise,
    BlingCylon,
    BlingPong,
    BlingFlashGreen,
    BlingFlashRed,
    BlingFlashYellow,
    BlingEnter,
    BlingExit,
    BlingEnd
  };

  enum BlingColors {
    BlingGreen,
    BlingRed,
    BlingYellow,
    BlingBlue
  };
   
  Bling(void) {
    _enabled = false;
    maxrow = maxcol = oldlights = newlights = 0;
    _next_interval = _interval = 10; 
    _mode = BlingOff;
  };

  Bling(int maxrows, int maxcolumns) { 
    _enabled = false;
    oldlights = newlights = 0;
    maxrow = maxrows; maxcol = maxcolumns;
    _next_interval = _interval = 10; 
    _mode = BlingOff;
  };

  // void ~bling();
  // describe_row();
  // describe_light_colors();
  unsigned long get_lights();
  void set_mode(BlingMode b) { _mode = b; } 
  void set_random_mode() { 
    do { _mode = (BlingMode) (rand() % (((long) BlingEnd)-1));
    } while (_mode != BlingOff && _mode != BlingEnter & _mode != BlingExit);
  }

  BlingMode get_mode() { return _mode; }
  void set_interval(long a) { _interval = a ; }
  long get_interval() { return _interval; }
  long run();
  void use_timestamps(bool t) { _using_timestamps = t; }
  bool enable() { int temp = _enabled; _enabled = true; return(temp); }
  bool disable() { int temp = _enabled; _enabled = false; return(temp); }
  void next();
  void prev();

 private:

  void do_star();
  void do_kit();
  void do_pairs();
  void do_rotate();
  void do_random();
  void do_rows();
  void do_enter();
  void do_exit();
  void do_flashall();
  void do_pong();
  void do_cylon();
  void do_fall();
  void do_rise();
  void do_colors(BlingColors);
  void do_off();
  void do_on();
  void do_rotating();

  long get_timestamp();
  bool _enabled;
  int maxrow;
  int maxcol;
  //  vector<int,int> light_layout;
  // vector<int,int> screen_layout;
  long _interval;
  long _next_interval;
  unsigned oldlights;
  unsigned oldscreen;
  unsigned newlights;
  unsigned newscreen;
  bool _using_timestamps;
  enum BlingMode _mode;
};
