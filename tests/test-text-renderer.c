#include "thunar/thunar-text-renderer.h"
#include "thunar/thunar-util.h"
#include "thunar/thunar-private.h"
#include "thunar/thunar-file.h"
#include "thunar/thunar-gobject-extensions.h"
#include "thunar/thunar-preferences.h"
#include "tests/thunar-snapshot.h"

#include <gtk/gtk.h>

static GtkCssProvider *css_provider = NULL;

static void
set_theme (const gchar *theme_name)
{
  GtkSettings *settings = gtk_settings_get_default ();
  const gchar *test_srcdir = g_getenv ("G_TEST_SRCDIR");
  g_autofree gchar *css_path = NULL;

  if (test_srcdir == NULL)
    test_srcdir = ".";

  css_path = g_build_filename (test_srcdir, "themes", theme_name, "gtk-3.0", "gtk.css", NULL);

  if (css_provider != NULL)
    {
      gtk_style_context_remove_provider_for_screen (gdk_screen_get_default (),
                                                    GTK_STYLE_PROVIDER (css_provider));
      g_object_unref (css_provider);
      css_provider = NULL;
    }

  if (g_file_test (css_path, G_FILE_TEST_EXISTS))
    {
      css_provider = gtk_css_provider_new ();
      gtk_css_provider_load_from_path (css_provider, css_path, NULL);
      gtk_style_context_add_provider_for_screen (gdk_screen_get_default (),
                                                 GTK_STYLE_PROVIDER (css_provider),
                                                 GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }
  else
    {
      g_error ("Theme CSS file not found: %s", css_path);
    }

  g_object_set (settings, "gtk-theme-name", theme_name, NULL);
}

static void
mock_cell_layout_data_func (GtkCellRenderer *cell,
                            const gchar     *highlight_bg,
                            const gchar     *highlight_fg,
                            GtkWidget       *widget)
{
  GdkRGBA foreground_rgba;
  GtkWidget *toplevel;

  if (highlight_fg != NULL)
    {
      gdk_rgba_parse (&foreground_rgba, highlight_fg);
      foreground_rgba.alpha = ALPHA_FOCUSED;

      /* check the state of our window (active/backdrop) */
      toplevel = gtk_widget_get_toplevel (widget);
      if (GTK_IS_WINDOW (toplevel) && !gtk_window_is_active (GTK_WINDOW (toplevel)))
        foreground_rgba.alpha = ALPHA_BACKDROP;

      g_object_set (G_OBJECT (cell), "foreground-rgba", &foreground_rgba, NULL);
    }
  else
    {
      g_object_set (G_OBJECT (cell), "foreground-set", FALSE, NULL);
    }

  g_object_set (G_OBJECT (cell), "highlight-color", highlight_bg, NULL);
}

static void
run_test (const gchar *test_name,
          const gchar *theme_name,
          const gchar *text,
          gboolean     highlighting_enabled,
          const gchar *highlight_bg,
          const gchar *highlight_fg,
          gboolean     rounded_corners,
          GtkCellRendererState flags,
          gboolean     window_active)
{
  GtkCellRenderer *renderer = thunar_text_renderer_new ();
  GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  cairo_surface_t *surface;
  cairo_t *cr;
  GdkRectangle cell_area = { 0, 0, 200, 30 };
  GdkRectangle background_area = { 0, 0, 200, 30 };

  /* Set theme */
  set_theme (theme_name);

  if (window_active)
    gtk_window_present (GTK_WINDOW (window));
  else
    gtk_widget_show (window);

  /* ThunarTextRenderer basic properties */
  g_object_set (renderer,
                "text", text,
                "highlighting-enabled", highlighting_enabled,
                "rounded-corners", rounded_corners,
                NULL);
  
  mock_cell_layout_data_func (renderer, highlight_bg, highlight_fg, window);

  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 200, 30);
  cr = cairo_create (surface);

  /* Clear surface - Thunar usually renders on top of a view background */
  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_paint (cr);

  gtk_cell_renderer_render (renderer, cr, window, &background_area, &cell_area, flags);

  thunar_snapshot_assert (test_name, surface);

  cairo_destroy (cr);
  cairo_surface_destroy (surface);
  g_object_unref (renderer);
  gtk_widget_destroy (window);
}



static void
test_all_states (void)
{
  const gchar *themes[] = { "Adwaita", "Mint-L-Aqua" };

  for (guint i = 0; i < G_N_ELEMENTS (themes); i++)
    {
      const gchar *theme = themes[i];
      g_autofree gchar *prefix = g_strdup_printf ("%s-", theme);

      run_test (g_strdup_printf("%sfolder-simple", prefix), theme, "Minha Pasta", FALSE, NULL, NULL, FALSE, 0, TRUE);
      run_test (g_strdup_printf("%sfolder-selected", prefix), theme, "Selecionada", FALSE, NULL, NULL, FALSE, GTK_CELL_RENDERER_SELECTED, TRUE);
      run_test (g_strdup_printf("%sfolder-highlight-both", prefix), theme, "Destaque Tudo", TRUE, "#FF0000", "#FFFF00", TRUE, 0, TRUE);
      run_test (g_strdup_printf("%sfolder-highlight-selected", prefix), theme, "Destaque Sel", TRUE, "#0000FF", "#FFFFFF", TRUE, GTK_CELL_RENDERER_SELECTED, TRUE);
      run_test (g_strdup_printf("%sfolder-backdrop", prefix), theme, "Backdrop", TRUE, "#FF0000", "#00FF00", TRUE, 0, FALSE);
      run_test (g_strdup_printf("%sfolder-selected-backdrop", prefix), theme, "Backdrop", TRUE, NULL, NULL, TRUE, GTK_CELL_RENDERER_SELECTED, FALSE);
    }
}



int
main (int argc, char **argv)
{
  gtk_init (&argc, &argv);
  g_test_init (&argc, &argv, NULL);

  thunar_g_initialize_transformations ();

  g_test_add_func ("/text-renderer/test_all_states", test_all_states);

  return g_test_run ();
}
