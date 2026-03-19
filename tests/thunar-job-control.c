#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "tests/thunar-job-control.h"
#include <gtk/gtk.h>

#ifdef THUNAR_JOB_CONTROL

struct _ThunarJobControl
{
  GObject parent_instance;

  guint jobs_count;
  void (*callback) (void);
  guint idle_id;
};

static ThunarJobControl *global_control = NULL;

G_DEFINE_TYPE (ThunarJobControl, thunar_job_control, G_TYPE_OBJECT)

static void
thunar_job_control_finalize (GObject *object)
{
  ThunarJobControl *control = THUNAR_JOB_CONTROL_INST (object);

  if (control->idle_id != 0)
    g_source_remove (control->idle_id);

  G_OBJECT_CLASS (thunar_job_control_parent_class)->finalize (object);
}

static void
thunar_job_control_class_init (ThunarJobControlClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = thunar_job_control_finalize;
}

static void
thunar_job_control_init (ThunarJobControl *control)
{
  control->jobs_count = 0;
  control->callback = NULL;
  control->idle_id = 0;
}

static gboolean
thunar_job_control_idle (gpointer data)
{
  ThunarJobControl *control = THUNAR_JOB_CONTROL_INST (data);

  if (control->jobs_count == 0)
    {
      if (control->callback != NULL)
	  {
		  while (gtk_events_pending())
		  {
			gtk_main_iteration();
		  }
        control->callback ();
	  }
      
      control->idle_id = 0;
      return FALSE; /* Remove from idle */
    }
  
  return TRUE; /* Keep in idle until jobs_count is 0 */
}

void
thunar_job_control_setup (void)
{
  if (global_control == NULL)
    global_control = g_object_new (THUNAR_TYPE_JOB_CONTROL_INST, NULL);
}

void
thunar_job_control_shutdown (void)
{
  if (global_control != NULL)
    {
      g_object_unref (global_control);
      global_control = NULL;
    }
}

void
thunar_job_control_increment (void)
{
  g_return_if_fail (THUNAR_IS_JOB_CONTROL_INST (global_control));
  global_control->jobs_count++;
}

void
thunar_job_control_decrement (void)
{
  g_return_if_fail (THUNAR_IS_JOB_CONTROL_INST (global_control));

  if (global_control->jobs_count > 0)
    global_control->jobs_count--;

  if (global_control->jobs_count == 0 && global_control->idle_id == 0)
    {
      global_control->idle_id = g_idle_add_full (G_PRIORITY_LOW, thunar_job_control_idle, global_control, NULL);
    }
}

guint
thunar_job_control_get_jobs_count (void)
{
  g_return_val_if_fail (THUNAR_IS_JOB_CONTROL_INST (global_control), 0);
  return global_control->jobs_count;
}

void
thunar_job_control_set_callback (void (*callback) (void))
{
  g_return_if_fail (THUNAR_IS_JOB_CONTROL_INST (global_control));

  global_control->callback = callback;
  
  /* If we already have 0 jobs, trigger idle check */
  if (global_control->jobs_count == 0 && global_control->idle_id == 0)
    global_control->idle_id = g_idle_add_full (G_PRIORITY_LOW, thunar_job_control_idle, global_control, NULL);
}

#endif /* THUNAR_JOB_CONTROL */
