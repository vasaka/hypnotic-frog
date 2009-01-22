#include <libglademm/xml.h>
#include <gtkmm.h>
#include "cellgui.h"
 
int main(int argc, char* argv[])
{
  Gtk::Main kit(argc, argv);
  Glib::RefPtr<Gnome::Glade::Xml> refXml = Gnome::Glade::Xml::create("cell1.glade");
  cell_gui frog(refXml); 
  kit.run();
  return 0;
}
