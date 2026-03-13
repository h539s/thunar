#include "tests/thunar-snapshot.h"

#include <glib/gstdio.h>
#include <string.h>

void
thunar_snapshot_assert (const gchar     *test_name,
                        cairo_surface_t *surface)
{
  g_autofree gchar *snapshot_dir = g_build_filename (g_test_get_dir (G_TEST_DIST), "snapshots", NULL);
  g_autofree gchar *snapshot_filename = g_strdup_printf ("%s.png", test_name);
  g_autofree gchar *error_filename = g_strdup_printf ("%s_err.png", test_name);
  g_autofree gchar *snapshot_path = g_build_filename (snapshot_dir, snapshot_filename, NULL);
  g_autofree gchar *error_path = g_build_filename (snapshot_dir, error_filename, NULL);

  if (g_mkdir_with_parents (snapshot_dir, 0755) != 0)
    g_error ("Failed to create snapshot directory: %s", snapshot_dir);

  if (!g_file_test (snapshot_path, G_FILE_TEST_EXISTS))
    {
      cairo_status_t status = cairo_surface_write_to_png (surface, snapshot_path);
      if (status != CAIRO_STATUS_SUCCESS)
        g_error ("Failed to save initial snapshot %s: %s", snapshot_path, cairo_status_to_string (status));
      g_message ("Initial snapshot created: %s", snapshot_path);
      return;
    }

  cairo_surface_t *gold_surface = cairo_image_surface_create_from_png (snapshot_path);
  if (cairo_surface_status (gold_surface) != CAIRO_STATUS_SUCCESS)
    g_error ("Failed to load existing snapshot %s: %s", snapshot_path, cairo_status_to_string (cairo_surface_status (gold_surface)));

  int width = cairo_image_surface_get_width (surface);
  int height = cairo_image_surface_get_height (surface);
  int gold_width = cairo_image_surface_get_width (gold_surface);
  int gold_height = cairo_image_surface_get_height (gold_surface);

  gboolean match = TRUE;
  if (width != gold_width || height != gold_height)
    {
      match = FALSE;
    }
  else
    {
      cairo_surface_flush (surface);
      cairo_surface_flush (gold_surface);

      unsigned char *data = cairo_image_surface_get_data (surface);
      unsigned char *gold_data = cairo_image_surface_get_data (gold_surface);
      int stride = cairo_image_surface_get_stride (surface);
      int gold_stride = cairo_image_surface_get_stride (gold_surface);

      for (int y = 0; y < height; y++)
        {
          if (memcmp (data + y * stride, gold_data + y * gold_stride, width * 4) != 0)
            {
              match = FALSE;
              break;
            }
        }
    }

  if (!match)
    {
      cairo_surface_write_to_png (surface, error_path);
      cairo_surface_destroy (gold_surface);
      g_error ("Snapshot mismatch for '%s'. Check %s for the current output.", test_name, error_path);
    }

  cairo_surface_destroy (gold_surface);
}
