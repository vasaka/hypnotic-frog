#include "cellgui.h"

cell_gui::cell_gui(Glib::RefPtr<Gnome::Glade::Xml> refXml):
	draw_area_m(0)
{
  refXml->get_widget("draw_area", draw_area_m);
  draw_area_m->signal_button_release_event().connect(sigc::mem_fun(*this, &cell_gui::on_draw_area_button_release_event));

}

cell_gui::~cell_gui()
{
  delete draw_area_m;
}

bool cell_gui::on_draw_area_button_release_event(GdkEventButton *ev)
{  return 0;
}

