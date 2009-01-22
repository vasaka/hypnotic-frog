#include "cellgui.h"
#include <iostream>
using namespace std;

cell_gui::cell_gui(Glib::RefPtr<Gnome::Glade::Xml> refXml):
	ptrDrawArea_m(0),ptrCellWin_m(0),guard_m(false),iBrushSize_m(10),dStabilizer_m(1),iNs_m(1)
{
  refXml->get_widget("draw_area", ptrDrawArea_m);
  refXml->get_widget("cell", ptrCellWin_m);

  //refPixbuf_m = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, FALSE, 8, ptrDrawArea_m->get_width(), ptrDrawArea_m->get_height());
  refPixbuf_m = Gdk::Pixbuf::create_from_file("frog.bmp");
  refPixbufBack_m = refPixbuf_m->copy();

  ptrDrawArea_m->signal_button_release_event().connect(sigc::mem_fun(*this, &cell_gui::on_draw_area_button_release_event));
  ptrDrawArea_m->signal_expose_event().connect(sigc::mem_fun(*this, &cell_gui::on_draw_area_expose));
  ptrDrawArea_m->signal_motion_notify_event().connect(sigc::mem_fun(*this, &cell_gui::on_draw_area_motion_notify_event));

  Glib::signal_timeout().connect(sigc::mem_fun(*this, &cell_gui::redraw), 50);
  Glib::signal_idle().connect(sigc::mem_fun(*this, &cell_gui::time_tick));
}

cell_gui::~cell_gui()
{
  delete ptrDrawArea_m;
}

bool cell_gui::on_draw_area_button_release_event(GdkEventButton *ev)
{  
  if (!(ev->state&GDK_BUTTON1_MASK))
    ++iBrushSize_m;
	return 0;
}

bool cell_gui::on_draw_area_expose(GdkEventExpose *ev)
{
  if (guard_m) return true;
    guard_m = true;

  int w = refPixbuf_m->get_width();
  int h = refPixbuf_m->get_height();
  int rowstride = refPixbuf_m->get_rowstride();

  guchar* pixels = refPixbuf_m->get_pixels();

  Glib::RefPtr<Gdk::Window> refWindow = ptrDrawArea_m->get_window();
  Glib::RefPtr<Gdk::GC> refGC = ptrDrawArea_m->get_style()->get_black_gc();
  refWindow->draw_pixbuf(refGC,refPixbuf_m,0,0,0,0,w,h,Gdk::RGB_DITHER_NONE,0,0);

  guard_m = false;
  return true;
}

bool cell_gui::on_draw_area_motion_notify_event(GdkEventMotion* ev)
{
  if (!(ev->state&GDK_BUTTON1_MASK))
    return true; 
  if (guard_m) return true;
      guard_m = true;

  int x_ = ev->x;
  int y_ = ev->y;
  int w = refPixbuf_m->get_width();
  int h = refPixbuf_m->get_height();
  int rowstride = refPixbuf_m->get_rowstride();
  int nchannels = refPixbuf_m->get_n_channels();
  guchar* pixels = refPixbuf_m->get_pixels();
  for (int _x = 0;_x<iBrushSize_m;_x++)
    for (int _y = 0;_y<iBrushSize_m;_y++)
    {
      int x = (x_ + _x)%w;
      int y = (_y + y_)%h;
      guchar* p = pixels + y * rowstride + x * nchannels;
      p[0] = ((x*y)/255)%255; p[1] = ((y*y)/255)%255; p[2] = ((x*x)/255)%255;
      //p[0] = p[1] = p[2] = 0;
    }
  guard_m = false;
  return true;
}


bool cell_gui::time_tick()
{
  if (guard_m) return true;
  guard_m = true;

  int w = refPixbuf_m->get_width();
  int h = refPixbuf_m->get_height();
  int rowstride = refPixbuf_m->get_rowstride();
  int nchannels = refPixbuf_m->get_n_channels ();
  refPixbuf_m->copy_area(0,0,w,h,refPixbufBack_m,0,0);

  guchar* in = refPixbuf_m->get_pixels();
  guchar* out = refPixbufBack_m->get_pixels();

//  cout << w << ' ' << h << ' ' << rowstride << ' ' << nchannels << "\n";
  
  for (int x=0;x<w;++x)
    for (int y=0;y<h;++y)
    {
      unsigned char* p1 = in + y * rowstride + x * nchannels;
      unsigned char* p = out + y * rowstride + x * nchannels;
      int p1_type = p1[0]>p1[1] ? (p1[0]>p1[2]? 0 : 2):(p1[1]>p1[2]? 1:2);
      //p[p1_type]+=(255-p[p1_type])/10;p[(p1_type+1)%3]*=0.8;p[(p1_type+2)%3]*=0.8;
      //p[p1_type]=255;p[(p1_type+1)%3]=0;p[(p1_type+2)%3]=0;
      int population[3] = {0,0,0};
          
      for (int xs=-iNs_m;xs<=iNs_m;++xs)
        for (int ys=-iNs_m;ys<=iNs_m;++ys)
        {
          unsigned char* p2 = in + ((y+ys)%h) * rowstride + ((x+xs)%w) * nchannels;
          int p2_type = p2[0]>p2[1] ? (p2[0]>p2[2]? 0 : 2):(p2[1]>p2[2]? 1:2);
          ++population[p2_type];
        }
//      cout << population[0] << ' ' << population[1] << ' '  <<population[2] << "\n";
//      population[2]-=population[2]/3.3;
      if (population[(p1_type+1)%3]>((population[p1_type]/2)-dStabilizer_m))
      {
        p[(p1_type+1)%3] += (255-p[(p1_type+1)%3])*0.5;
        p[(p1_type+0)%3] *= 0.8;
        p[(p1_type+2)%3] *= 0.8;
      }
    }
    refPixbufBack_m.swap(refPixbuf_m);
    guard_m = false;
    return true;
}
