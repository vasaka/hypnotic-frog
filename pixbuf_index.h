#ifndef PIXBUF_INDEX_H
#define PIXBUF_INDEX_H
#include "index.h"
#include <gtkmm.h>
template<>
Index< Glib::RefPtr<Gdk::Pixbuf> >::Index(Glib::RefPtr<Gdk::Pixbuf> Pixbuf): xStep_m(Pixbuf->get_n_channels()), yStep_m(Pixbuf->get_rowstride())
{};
#endif //PIXBUF_INDEX_H
