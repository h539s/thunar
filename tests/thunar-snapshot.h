#ifndef __THUNAR_SNAPSHOT_H__
#define __THUNAR_SNAPSHOT_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

/**
 * thunar_snapshot_assert:
 * @test_name: The name of the test, used as the filename for the snapshot.
 * @surface: The cairo surface containing the rendered output to test.
 *
 * Compares the @surface with a previously saved snapshot in tests/snapshots/@test_name.png.
 * If the snapshot does not exist, it is created.
 * If it exists and differs, @test_name_err.png is created and the test fails.
 */
void thunar_snapshot_assert (const gchar *test_name,
                             cairo_surface_t *surface);

G_END_DECLS

#endif /* !__THUNAR_SNAPSHOT_H__ */
