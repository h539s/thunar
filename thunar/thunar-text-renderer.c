/* vi:set et ai sw=2 sts=2 ts=2: */
/*-
 * Copyright (c) 2022 Amrit Borah <elessar1802@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "thunar/thunar-text-renderer.h"
#include "thunar/thunar-file.h"
#include "thunar/thunar-util.h"



enum
{
  PROP_0,
  PROP_FILE,
};



static void
thunar_text_renderer_finalize (GObject *object);
static void
thunar_text_renderer_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec);
static void
thunar_text_renderer_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec);
static void
thunar_text_renderer_render (GtkCellRenderer     *renderer,
                             cairo_t             *cr,
                             GtkWidget           *widget,
                             const GdkRectangle  *background_area,
                             const GdkRectangle  *cell_area,
                             GtkCellRendererState flags);



struct _ThunarTextRendererClass
{
  GtkCellRendererTextClass __parent__;

  void (*default_render_function) (GtkCellRenderer     *cell,
                                   cairo_t             *cr,
                                   GtkWidget           *widget,
                                   const GdkRectangle  *background_area,
                                   const GdkRectangle  *cell_area,
                                   GtkCellRendererState flags);
};

struct _ThunarTextRenderer
{
  GtkCellRendererText __parent__;

  ThunarFile *file;
};



G_DEFINE_TYPE (ThunarTextRenderer, thunar_text_renderer, GTK_TYPE_CELL_RENDERER_TEXT);



static void
thunar_text_renderer_class_init (ThunarTextRendererClass *klass)
{
  GObjectClass         *object_class = G_OBJECT_CLASS (klass);
  GtkCellRendererClass *cell_class = GTK_CELL_RENDERER_CLASS (klass);

  object_class->finalize = thunar_text_renderer_finalize;

  object_class->get_property = thunar_text_renderer_get_property;
  object_class->set_property = thunar_text_renderer_set_property;

  klass->default_render_function = cell_class->render;
  cell_class->render = thunar_text_renderer_render;

  /**
   * ThunarTextRenderer:file:
   *
   * The file whose text to render.
   **/
  g_object_class_install_property (object_class,
                                   PROP_FILE,
                                   g_param_spec_object ("file", "file", "file",
                                                        THUNAR_TYPE_FILE,
                                                        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}



static void
thunar_text_renderer_init (ThunarTextRenderer *text_renderer)
{
  text_renderer->file = NULL;
}



static void
thunar_text_renderer_finalize (GObject *object)
{
  ThunarTextRenderer *text_renderer = THUNAR_TEXT_RENDERER (object);

  if (G_LIKELY (text_renderer->file != NULL))
    g_object_unref (G_OBJECT (text_renderer->file));

  G_OBJECT_CLASS (thunar_text_renderer_parent_class)->finalize (object);
}



static void
thunar_text_renderer_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  ThunarTextRenderer *text_renderer = THUNAR_TEXT_RENDERER (object);

  switch (prop_id)
    {
    case PROP_FILE:
      g_value_set_object (value, text_renderer->file);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
thunar_text_renderer_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  ThunarTextRenderer *text_renderer = THUNAR_TEXT_RENDERER (object);

  switch (prop_id)
    {
    case PROP_FILE:
      if (G_LIKELY (text_renderer->file != NULL))
        g_object_unref (G_OBJECT (text_renderer->file));
      text_renderer->file = (gpointer) g_value_dup_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



/**
 * thunar_text_renderer_new:
 *
 * Creates a new #ThunarTextRenderer. Adjust rendering
 * parameters using object properties. Object properties can be
 * set globally with #g_object_set. Also, with #GtkTreeViewColumn,
 * you can bind a property to a value in a #GtkTreeModel.
 *
 * Return value: (transfer full) The newly allocated #ThunarTextRenderer.
 **/
GtkCellRenderer *
thunar_text_renderer_new (void)
{
  return g_object_new (THUNAR_TYPE_TEXT_RENDERER, NULL);
}



static void
thunar_text_renderer_render (GtkCellRenderer     *cell,
                             cairo_t             *cr,
                             GtkWidget           *widget,
                             const GdkRectangle  *background_area,
                             const GdkRectangle  *cell_area,
                             GtkCellRendererState flags)
{
  ThunarTextRenderer *text_renderer = THUNAR_TEXT_RENDERER (cell);
  GtkStyleContext    *context;
  GtkCssProvider     *provider;

  if (G_LIKELY (text_renderer->file != NULL))
    {
      provider = thunar_file_get_highlight_css_provider (text_renderer->file);
      if (G_UNLIKELY (provider != NULL))
        {
          context = gtk_widget_get_style_context (widget);
          gtk_style_context_save (context);
          gtk_style_context_add_provider (context, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
          gtk_render_background (context, cr, background_area->x, background_area->y, background_area->width, background_area->height);
          gtk_style_context_restore (context);
        }
    }

  /* we only needed to manipulate the background_area, otherwise everything remains the same.
     Hence, we are simply running the original render function now */
  THUNAR_TEXT_RENDERER_GET_CLASS (THUNAR_TEXT_RENDERER (cell))
  ->default_render_function (cell, cr, widget, background_area, cell_area, flags);
}
