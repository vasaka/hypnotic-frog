#include "cellgui.h"
#include "pixbuf_index.h"

cell_gui::cell_gui(Glib::RefPtr<Gnome::Glade::Xml> refXml):
	ptrDrawArea_m(0),ptrCellWin_m(0),guard_m(true),iBrushSize_m(10),dStabilizer_m(1),iNs_m(1),iBrushModesNum_m(5),iBrushMode_m(0),iBrushX_m(0),iBrushY_m(0),iFadeMode_m(2),iFadeModeNum(3)
{
  refXml->get_widget("draw_area", ptrDrawArea_m);
  refXml->get_widget("cell", ptrCellWin_m);

  //refPixbuf_m = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, FALSE, 8, ptrDrawArea_m->get_width(), ptrDrawArea_m->get_height());
  refPixbuf_m = Gdk::Pixbuf::create_from_file("frog.bmp");
  refPixbufBack_m = refPixbuf_m->copy();

  ptrDrawArea_m->signal_button_release_event().connect(sigc::mem_fun(*this, &cell_gui::on_draw_area_button_release_event));
  ptrDrawArea_m->signal_scroll_event().connect(sigc::mem_fun(*this, &cell_gui::on_draw_area_scroll_event));
  ptrDrawArea_m->signal_expose_event().connect(sigc::mem_fun(*this, &cell_gui::on_draw_area_expose));
  ptrDrawArea_m->signal_motion_notify_event().connect(sigc::mem_fun(*this, &cell_gui::on_draw_area_motion_notify_event));
  ptrCellWin_m->signal_key_press_event().connect(sigc::mem_fun(*this, &cell_gui::on_cell_key_press_event));

  Glib::signal_idle().connect(sigc::mem_fun(*this, &cell_gui::init_runtime));
  guard_m = false;
}

bool cell_gui::init_runtime()
{
  Glib::signal_timeout().connect(sigc::mem_fun(*this, &cell_gui::redraw), 50);
  Glib::signal_idle().connect(sigc::mem_fun(*this, &cell_gui::time_tick));
  return false;
}

cell_gui::~cell_gui()
{
  delete ptrDrawArea_m;
}
// events //////////////////////////////////////////////////////////
bool cell_gui::on_draw_area_button_release_event(GdkEventButton *ev)
{  
  if (ev->state==GDK_BUTTON3_MASK)
    iBrushMode_m=(iBrushMode_m+1)%iBrushModesNum_m;
	return 0;
}

bool cell_gui::on_cell_key_press_event(GdkEventKey *ev)
{
  if (ev->keyval==GDK_s)
    save("save.bmp");
  if (ev->keyval==GDK_l)
    load("save.bmp");
  if (ev->keyval==GDK_f)
    iFadeMode_m=(iFadeMode_m+1)%iFadeModeNum;
}

bool cell_gui::on_draw_area_scroll_event(GdkEventScroll* ev)
{
  if (ev->direction==GDK_SCROLL_UP)
    ++iBrushSize_m;
  else
    --iBrushSize_m;
  iBrushSize_m = iBrushSize_m < 1 ? 1:iBrushSize_m;
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
  while (guard_m) return true;
  guard_m = true;
  
  iBrushX_m = ev->x;
  iBrushY_m = ev->y;
 
  if (ev->state==GDK_BUTTON1_MASK)
    PaintBrush(refPixbufBack_m);
  else
    PaintBrush(refPixbuf_m);

  guard_m = false;
  return true;
}

// main thread //////////////////////////////////////////////////
bool cell_gui::time_tick()
{
  if (guard_m) return true;
  guard_m = true;

  int w = refPixbufBack_m->get_width();
  int h = refPixbufBack_m->get_height();
  Index<Glib::RefPtr<Gdk::Pixbuf> > I(refPixbufBack_m);
  refPixbufBack_m->copy_area(0,0,w,h,refPixbuf_m,0,0);

  guchar* in = refPixbufBack_m->get_pixels();
  guchar* out = refPixbuf_m->get_pixels();

  for (int x=0;x<w;x++)
    for (int y=0;y<h;y++)
    {
      unsigned char* p1 = in + I(x,y);
      unsigned char* p = out + I(x,y);
      int p1_type = p1[0]>p1[1] ? (p1[0]>p1[2]? 0 : 2):(p1[1]>p1[2]? 1:2);
      int population[3] = {0,0,0};
          
      for (int xs=-iNs_m;xs<=iNs_m;++xs)
        for (int ys=-iNs_m;ys<=iNs_m;++ys)
        {
          unsigned char* p2 = in + I(abs((x+xs)%w),abs((y+ys)%h));
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
      switch(iFadeMode_m)
      {
        case 0/*hard*/:{p[p1_type]+=(255-p[p1_type])*0.1;p[(p1_type+1)%3]*=0.7;p[(p1_type+2)%3]*=0.7;break;}
        case 1/*soft*/:{p[p1_type]+=(255-p[p1_type])*0.01;p[(p1_type+1)%3]*=0.99;p[(p1_type+2)%3]*=0.99;break;}
      }
    }
    refPixbuf_m.swap(refPixbufBack_m);
    PaintBrush(refPixbuf_m);
    guard_m = false;
    return true;
}

// other methods
bool cell_gui::save(std::string filename)
{
  while (guard_m) {};
  guard_m = true;

  refPixbuf_m->save(filename,"bmp");
  guard_m = false;
}
bool cell_gui::load(std::string filename)
{
  while (guard_m) {};
  guard_m = true;
  Glib::RefPtr<Gdk::Pixbuf> temp = Gdk::Pixbuf::create_from_file(filename);
  temp->copy_area(0,0,temp->get_width(),temp->get_height(),refPixbufBack_m,0,0);
  guard_m = false;
}

void cell_gui::PaintBrush(Glib::RefPtr<Gdk::Pixbuf> refPixbuf)
{
  int x_ = iBrushX_m;
  int y_ = iBrushY_m;
  int w = refPixbufBack_m->get_width();
  int h = refPixbufBack_m->get_height();
  Index<Glib::RefPtr<Gdk::Pixbuf> > I(refPixbufBack_m);
  guchar* pixels = refPixbuf->get_pixels();

  for (int _x = -iBrushSize_m;_x<=iBrushSize_m;_x++)
    for (int _y = -sqrt(iBrushSize_m*iBrushSize_m-_x*_x);_y<=sqrt(iBrushSize_m*iBrushSize_m-_x*_x);_y++)
    {
      int x = abs((x_ + _x)%w);
      int y = abs((_y + y_)%h);
      guchar* p = pixels + I(x,y);
      switch(iBrushMode_m)
      {
        case 0/*color*/: {p[0] = ((x*y)/255)%255; p[1] = ((y*y)/255)%255; p[2] = ((x*x)/255)%255; break;}
        case 1/*black*/: {p[0] = 0; p[1] = 0; p[2] = 0; break;}
        case 2/*red*/: {p[0] = 255; p[1] = 0; p[2] = 0; break;}
        case 3/*green*/: {p[0] = 0; p[1] = 255; p[2] = 0; break;}
        case 4/*blue*/: {p[0] = 0; p[1] = 0; p[2] = 255; break;}
      }
    }
}
