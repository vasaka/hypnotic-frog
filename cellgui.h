#ifndef CELL_GUI_H
#define CELL_GUI_H

#include "service_defs.h"
#include <libglademm/xml.h>
#include <gtkmm.h>
#include <string>
#include <list>

class cell_gui : public sigc::trackable
{
  //data
  Gtk::DrawingArea* ptrDrawArea_m;//actual window where droing is done
  Gtk::Window* ptrCellWin_m;//parent application window, we need it to pass to Gtk+ main loop so it will know when to terminate
  Glib::RefPtr<Gdk::Pixbuf> refPixbuf_m;//image rendered to screen
  Glib::RefPtr<Gdk::Pixbuf> refPixbufBack_m;//image where drawing is done
  int iBrushSize_m;
  bool guard_m;//mutex flag, should change it to standart realization
  int iNs_m;//size of cell areal
  double dStabilizer_m;//inner variable
  int iBrushModesNum_m;
  int iBrushMode_m;
  int iBrushX_m;
  int iBrushY_m;
  int iFadeMode_m;
  int iFadeModeNum;
  //events
  bool on_draw_area_button_release_event(GdkEventButton *ev);
  bool on_cell_key_press_event(GdkEventKey *ev);
  bool on_draw_area_expose(GdkEventExpose *ev);
  bool on_draw_area_motion_notify_event(GdkEventMotion* ev);
  bool on_draw_area_scroll_event(GdkEventScroll* ev);
  //methods
  bool save(std::string filename);
  bool load(std::string filename);
  void PaintBrush(Glib::RefPtr<Gdk::Pixbuf> refPixbuf);
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

