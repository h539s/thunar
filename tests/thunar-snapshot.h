#ifndef __THUNAR_SNAPSHOT_H__
#define __THUNAR_SNAPSHOT_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

void thunar_snapshot_assert (const gchar *test_name,
                             cairo_surface_t *surface);

G_END_DECLS

#endif /* !__THUNAR_SNAPSHOT_H__ */
