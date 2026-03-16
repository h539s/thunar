#ifndef __THUNAR_TEST_SNAPSHOTS_H__
#define __THUNAR_TEST_SNAPSHOTS_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

void take_snapshot_and_compare (GtkWindow *window, const gchar *test_case);

G_END_DECLS

#endif /* !__THUNAR_TEST_SNAPSHOTS_H__ */
