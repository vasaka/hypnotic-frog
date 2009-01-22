#ifndef CELL_GUI_H
#define CELL_GUI_H

#include <libglademm/xml.h>
#include <gtkmm.h>

class cell_gui : public sigc::trackable
{
  //data
  Gtk::DrawingArea* ptrDrawArea_m;
  Gtk::Window* ptrCellWin_m;
  Glib::RefPtr<Gdk::Pixbuf> refPixbuf_m;
  Glib::RefPtr<Gdk::Pixbuf> refPixbufBack_m;
  //methods
  bool on_draw_area_button_release_event(GdkEventButton *ev);
  bool on_draw_area_expose(GdkEventExpose *ev);
  bool redraw() {ptrDrawArea_m->queue_draw(); return true;}
  bool time_tick();
public:
  cell_gui(Glib::RefPtr<Gnome::Glade::Xml> refXml);
  Gtk::Window& get_window() { return *ptrCellWin_m; }

  ~cell_gui();
};

#endif

