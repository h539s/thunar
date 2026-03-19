#include <stdio.h>
#include <stdlib.h>
#include <glib/gstdio.h>
#include <xfconf/xfconf.h>
#include <libxfce4util/libxfce4util.h>

#include "thunar/thunar-application.h"
#include "thunar/thunar-gobject-extensions.h"
#include "thunar/thunar-notify.h"
#include "thunar/thunar-preferences.h"
#include "thunar/thunar-private.h"
#include "thunar/thunar-session-client.h"
#include "thunar-test-control.h"
#include "thunar-test-snapshots.h"
#include "tests/thunar-job-control.h"

static GtkWindow *test_window = NULL;
static gint       test_step = 0;

static void
job_control_ready (void)
{
  if (test_window == NULL)
    return;

  if (!gtk_widget_get_visible (GTK_WIDGET (test_window)))
    return;

  if (gtk_widget_get_window (GTK_WIDGET (test_window)) == NULL)
    return;

  g_print ("Job control: all jobs finished, executing step %d...\n", test_step);
  
  if (thunar_run_test (test_window, test_step))
    {
      test_step++;
    }
  else
    {
      /* No more steps, take snapshot */
      take_snapshot_and_compare (test_window, thunar_test_get_name ());
    }
}

static void
window_added (GtkApplication *app,
              GtkWindow      *window,
              gpointer        user_data)
{
  static gboolean test_started = FALSE;
  if (!test_started)
    {
      test_started = TRUE;
      test_window = window;

      /* In environments without a window manager (like Xvfb), gtk_window_maximize ()
       * only sets a hint but doesn't resize the window. We manually set the size
       * to match the screen to ensure it is effectively maximized. */
      gtk_window_unmaximize (window);
      gtk_window_resize (window, 800, 600);
      gtk_window_move (window, 0, 0);
    }
}

int
main (int argc, char **argv)
{
  ThunarApplication *application;
  GError            *error = NULL;

  /* setup translation domain */
  xfce_textdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

  /* setup application name */
  g_set_application_name (_("Thunar"));

  /* initialize xfconf */
  if (!xfconf_init (&error))
    {
      g_warning (PACKAGE_NAME ": Failed to initialize Xfconf: %s", error->message);
      g_clear_error (&error);

      /* disable get/set properties */
      thunar_preferences_xfconf_init_failed ();
    }

  /* register additional transformation functions */
  thunar_g_initialize_transformations ();

  /* acquire a reference on the global application */
  application = thunar_application_get ();

  thunar_job_control_set_callback (job_control_ready);

  g_signal_connect (application, "window-added", G_CALLBACK (window_added), NULL);

  /* use the Thunar icon as default for new windows */
  gtk_window_set_default_icon_name ("org.xfce.thunar");

  /* override default handler for some common signals */
  thunar_application_posix_signal_init (application);

  /* do further processing inside gapplication */
  g_application_run (G_APPLICATION (application), argc, argv);

  /* release the application reference */
  g_object_unref (G_OBJECT (application));

#ifdef HAVE_LIBNOTIFY
  thunar_notify_uninit ();
#endif

  return EXIT_SUCCESS;
}
