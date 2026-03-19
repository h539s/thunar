#ifndef __THUNAR_JOB_CONTROL_H__
#define __THUNAR_JOB_CONTROL_H__

#include <glib-object.h>

#ifndef THUNAR_JOB_CONTROL
#define THUNAR_JOB_CONTROL
#endif

G_BEGIN_DECLS

#define THUNAR_TYPE_JOB_CONTROL_INST (thunar_job_control_get_type ())
G_DECLARE_FINAL_TYPE (ThunarJobControl, thunar_job_control, THUNAR, JOB_CONTROL_INST, GObject)

void              thunar_job_control_setup         (void);
void              thunar_job_control_shutdown      (void);
void              thunar_job_control_increment     (void);
void              thunar_job_control_decrement     (void);
guint             thunar_job_control_get_jobs_count (void);
void              thunar_job_control_set_callback  (void (*callback) (void));

G_END_DECLS

#endif /* !__THUNAR_JOB_CONTROL_H__ */
