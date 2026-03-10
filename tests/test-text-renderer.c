#include "thunar/thunar-text-renderer.h"
#include "thunar/thunar-util.h"
#include "tests/thunar-snapshot.h"

#include <gtk/gtk.h>

GtkCssProvider *provider;
/* Mock theme */
const gchar *THEME1 =
  "@define-color theme_selected_bg_color #3584e4;\n"
  "@define-color theme_unfocused_selected_bg_color #767676;\n"
  "window { background-color: white; }\n";

const gchar *THEME2 =
  "@define-color theme_selected_bg_color #3584e4;\n"
  "window { background-color: white; }\n";

static void
run_test (const gchar *test_name,
          const gchar *text,
          gboolean     highlighting_enabled,
          const gchar *highlight_bg,
          const gchar *highlight_fg,
          gboolean     rounded_corners,
          GtkCellRendererState flags,
          gboolean     window_active,
          const gchar     *theme)
{
  GtkCellRenderer *renderer = thunar_text_renderer_new ();
  GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  cairo_surface_t *surface;
  cairo_t *cairo;
  GdkRectangle cell_area = { 0, 0, 200, 30 };
  GdkRectangle background_area = { 0, 0, 200, 30 };
  GdkRGBA fg_rgba;


  gtk_css_provider_load_from_data (provider, theme, -1, NULL);
  gtk_style_context_add_provider_for_screen (gdk_screen_get_default (),
                                             GTK_STYLE_PROVIDER (provider),
                                             GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  if (window_active)
    gtk_window_present (GTK_WINDOW (window));
  else
    gtk_widget_show (window);

  g_object_set (renderer,
                "text", text,
                "highlighting-enabled", highlighting_enabled,
                "highlight-color", highlight_bg,
                "rounded-corners", rounded_corners,
                NULL);
  if (highlight_fg != NULL)
    {
      gdk_rgba_parse (&fg_rgba, highlight_fg);
      g_object_set (renderer, "foreground-rgba", &fg_rgba, NULL);
    }
  
  if (window_active)
    {
      gtk_widget_set_state_flags (window, GTK_STATE_FLAG_FOCUSED, TRUE);
      gtk_widget_unset_state_flags (window, GTK_STATE_FLAG_BACKDROP);
    }
  else
    {
      gtk_widget_unset_state_flags (window, GTK_STATE_FLAG_FOCUSED | GTK_STATE_FLAG_CHECKED);
      gtk_widget_set_state_flags (window, GTK_STATE_FLAG_BACKDROP, TRUE);
    }

  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 200, 30);
  cairo = cairo_create (surface);

  /* Clear surface */
  cairo_set_source_rgb (cairo, 1, 1, 1);
  cairo_paint (cairo);

  gtk_cell_renderer_render (renderer, cairo, window, &background_area, &cell_area, flags);

  thunar_snapshot_assert (test_name, surface);

  cairo_destroy (cairo);
  cairo_surface_destroy (surface);
  g_object_unref (renderer);
  gtk_widget_destroy (window);
}



static void
test_all_states (void)
{
  run_test ("folder-simple", "dirname", FALSE, NULL, NULL, FALSE, 0, TRUE, THEME1);
  run_test ("folder-selected", "dirname", FALSE, NULL, NULL, FALSE, GTK_CELL_RENDERER_SELECTED, TRUE, THEME1);
  run_test ("folder-selected-inactive", "dirname", FALSE, NULL, NULL, FALSE, GTK_CELL_RENDERER_SELECTED, FALSE, THEME1);
  run_test ("folder-highlight-bg", "dirname", TRUE, "#FF0000", NULL, TRUE, 0, TRUE, THEME1);
  run_test ("folder-highlight-fg", "dirname", TRUE, NULL, "#00FF00", FALSE, 0, TRUE, THEME1);
  run_test ("folder-highlight-both", "dirname", TRUE, "#FF0000", "#FFFF00", TRUE, 0, TRUE, THEME1);
  run_test ("folder-highlight-selected", "dirname", TRUE, "#0000FF", "#FFFFFF", TRUE, GTK_CELL_RENDERER_SELECTED, TRUE, THEME1);
  run_test ("folder-backdrop", "dirname", FALSE, NULL, NULL, FALSE, 0, FALSE, THEME1);
  run_test ("folder-selected-inactive-no-unfocused-variable", "dirname", TRUE, NULL, NULL, TRUE, GTK_CELL_RENDERER_SELECTED, FALSE, THEME2);
}



int
main (int argc, char **argv)
{

  gtk_init (&argc, &argv);
  g_test_init (&argc, &argv, NULL);
  provider = gtk_css_provider_new ();
  g_test_add_func ("/text-renderer/test_all_states", test_all_states);

  return g_test_run ();
}
