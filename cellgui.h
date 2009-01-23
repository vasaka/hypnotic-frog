#ifndef CELL_GUI_H
#define CELL_GUI_H

#include "service_defs.h"
#include <libglademm/xml.h>
#include <gtkmm.h>
#include <string.h>

class cell_gui : public sigc::trackable
{
  //data
  Gtk::DrawingArea* ptrDrawArea_m;
  Gtk::Window* ptrCellWin_m;
  Glib::RefPtr<Gdk::Pixbuf> refPixbuf_m;
  Glib::RefPtr<Gdk::Pixbuf> refPixbufBack_m;
  int iBrushSize_m;
  bool guard_m;
  int iNs_m;
  double dStabilizer_m;
  //events
  bool on_draw_area_button_release_event(GdkEventButton *ev);
  bool on_cell_key_press_event(GdkEventKey *ev);
  bool on_draw_area_expose(GdkEventExpose *ev);
  bool on_draw_area_motion_notify_event(GdkEventMotion* ev);
  bool on_draw_area_scroll_event(GdkEventScroll* ev);
  //methods
  bool save(std::string filename);
  bool load(std::string filename);
  bool redraw() {ptrDrawArea_m->queue_draw(); return true;}
  bool time_tick();
  DISALLOW_COPY_AND_ASSIGN(cell_gui);
public:
  cell_gui(Glib::RefPtr<Gnome::Glade::Xml> refXml);
  Gtk::Window& get_window() { return *ptrCellWin_m; }
  bool init_runtime();

  ~cell_gui();
};

#endif

