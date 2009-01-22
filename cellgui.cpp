#include "cellgui.h"
#include <iostream>
using namespace std;

cell_gui::cell_gui(Glib::RefPtr<Gnome::Glade::Xml> refXml):
	ptrDrawArea_m(0),ptrCellWin_m(0)
{
  refXml->get_widget("draw_area", ptrDrawArea_m);
  refXml->get_widget("cell", ptrCellWin_m);

  refPixbuf_m = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, FALSE, 8, ptrDrawArea_m->get_width(), ptrDrawArea_m->get_height());

  ptrDrawArea_m->signal_button_release_event().connect(sigc::mem_fun(*this, &cell_gui::on_draw_area_button_release_event));
  ptrDrawArea_m->signal_expose_event().connect(sigc::mem_fun(*this, &cell_gui::on_draw_area_expose));

  //Glib::signal_timeout().connect(sigc::mem_fun(*this, &cell_gui::redraw), 100);
  //Glib::signal_idle().connect(sigc::mem_fun(*this, &cell_gui::time_tick),50);
}

cell_gui::~cell_gui()
{
  delete ptrDrawArea_m;
}

bool cell_gui::on_draw_area_button_release_event(GdkEventButton *ev)
{  
	int x = ev->x;
	int y = ev->y;
	int w = ptrDrawArea_m->get_width();
	int h = ptrDrawArea_m->get_height();
	int rowstride = refPixbuf_m->get_rowstride();
	int nchannels = refPixbuf_m->get_n_channels ();
	guchar* pixels = refPixbuf_m->get_pixels();
	guchar* p = pixels + y * rowstride + x * nchannels;
	p[0] = ((x+y)*768)%255; p[1] = (y*378)%255; p[2] = (x*998)%255;
  
	time_tick();
  ptrDrawArea_m->queue_draw();
	return 0;
}

bool cell_gui::on_draw_area_expose(GdkEventExpose *ev)
{
  int w = refPixbuf_m->get_width();
  int h = refPixbuf_m->get_height();
  int rowstride = refPixbuf_m->get_rowstride();

  guchar* pixels = refPixbuf_m->get_pixels();

  Glib::RefPtr<Gdk::Window> refWindow = ptrDrawArea_m->get_window();
  Glib::RefPtr<Gdk::GC> refGC = ptrDrawArea_m->get_style()->get_black_gc();
  refWindow->draw_rgb_image_dithalign(refGC,0,0,w,h,Gdk::RGB_DITHER_NONE,pixels,rowstride,0, 0);
}

bool cell_gui::time_tick()
{
  int w = refPixbuf_m->get_width();
  int h = refPixbuf_m->get_height();
  int rowstride = refPixbuf_m->get_rowstride();
  int nchannels = refPixbuf_m->get_n_channels ();

  guchar* in = refPixbuf_m->get_pixels();
  guchar* out = in;

//  cout << w << ' ' << h << ' ' << rowstride << ' ' << nchannels << "\n";
  
  for (int x=0;x<w-1;++x)
    for (int y=0;y<h-1;++y)
    {
      ////////////////////////////////////////////###################################################
      unsigned char* p1 = in + y * rowstride + x * nchannels;
      unsigned char* p2 = in + ((y+1)%h) * rowstride + x * nchannels;
      unsigned char* p = out + ((y+1)%h) * rowstride + x * nchannels;

      int p1_type = p1[0]>p1[1] ? (p1[0]>=p1[2]? 0 : 2):(p1[1]>=p1[2]? 1:2);
      int p2_type = p2[0]>p2[1] ? (p2[0]>=p2[2]? 0 : 2):(p2[1]>=p2[2]? 1:2);
      if ((p2_type+1)%3 == p1_type) {p[p1_type]=255;p[(p1_type+1)%3]=0;p[(p1_type+2)%3]=0;}
      
      ////////////////////////////////////////////###################################################
      p2 = in + y * rowstride + ((x+1)%w) * nchannels;
      p = out + y * rowstride + ((x+1)%w) * nchannels;

      p1_type = p1[0]>p1[1] ? (p1[0]>=p1[2]? 0 : 2):(p1[1]>=p1[2]? 1:2);
      p2_type = p2[0]>p2[1] ? (p2[0]>=p2[2]? 0 : 2):(p2[1]>=p2[2]? 1:2);
      if ((p2_type+1)%3 == p1_type) {p[p1_type]=255;p[(p1_type+1)%3]=0;p[(p1_type+2)%3]=0;}
      ////////////////////////////////////////////###################################################
      p2 = in + y * rowstride + abs((x-1)%w) * nchannels;
      p = out + y * rowstride + abs((x-1)%w) * nchannels;

      p1_type = p1[0]>p1[1] ? (p1[0]>=p1[2]? 0 : 2):(p1[1]>=p1[2]? 1:2);
      p2_type = p2[0]>p2[1] ? (p2[0]>=p2[2]? 0 : 2):(p2[1]>=p2[2]? 1:2);
      if ((p2_type+2)%3 == p1_type) {p[p1_type]=255;p[(p1_type+1)%3]=0;p[(p1_type+2)%3]=0;}
      ////////////////////////////////////////////###################################################
      p2 = in + abs((y-1)%h) * rowstride + x * nchannels;
      p = out + abs((y-1)%h) * rowstride + x * nchannels;

      p1_type = p1[0]>p1[1] ? (p1[0]>=p1[2]? 0 : 2):(p1[1]>=p1[2]? 1:2);
      p2_type = p2[0]>p2[1] ? (p2[0]>=p2[2]? 0 : 2):(p2[1]>=p2[2]? 1:2);
      if ((p2_type+2)%3 == p1_type) {p[p1_type]=255;p[(p1_type+1)%3]=0;p[(p1_type+2)%3]=0;}
    }
    ptrDrawArea_m->queue_draw(); return true;
}
