#ifndef CELL_GUI_H
#define CELL_GUI_H

#include <libglademm/xml.h>
#include <gtkmm.h>

class cell_gui : public sigc::trackable
{
  //data
  Gtk::DrawingArea* draw_area_m;
  //methods
  bool on_draw_area_button_release_event(GdkEventButton *ev) ;
public:
  cell_gui(Glib::RefPtr<Gnome::Glade::Xml> refXml);
  ~cell_gui();
};

#endif

