#include <corvis/common.h>
#include <corvis/imgutils.h>

#include <gtk/gtk.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glib.h>

#include <lcm/lcm.h>
#include <bot_vis/bot_vis.h>
#include <lcmtypes/corvis_image_t.h>

#define RENDERER_NAME       "Camera"
#define PARAM_RENDER_IN     "Show"

enum
{
    RENDER_IN_WIDGET,
    RENDER_IN_TOP_RIGHT,
    RENDER_IN_TOP_CENTER,
    RENDER_IN_TOP_LEFT,
    RENDER_IN_BOTTOM_RIGHT,
    RENDER_IN_BOTTOM_CENTER,
    RENDER_IN_BOTTOM_LEFT
};

typedef struct
{
    lcm_t* lcm;
    BotRenderer renderer;
    BotViewer* viewer;
    BotGtkParamWidget* pw;

    GHashTable* cam_handlers;
} renderer_camera_t;

typedef struct
{
    char* channel;
    BotGtkGlDrawingArea* gl_area;

    corvis_image_t* last_image;
    BotGlTexture* texture;

    renderer_camera_t* renderer;
    BotGtkParamWidget* pw;
    GtkWidget* expander;
    
    uint8_t* uncompressed_buffer;
    int uncompressed_buffer_size;
    int width, height;
    int is_uploaded;

    int msg_received;
    int render_place;
    int expanded;
} thumbnail_t;


static void
thumbnail_destroy(thumbnail_t* t)
{
    if(t->last_image)
        corvis_image_t_destroy(t->last_image);
    if(t->uncompressed_buffer)
    {
        free(t->uncompressed_buffer);
        t->uncompressed_buffer = NULL;
    }
    free(t->channel);
    free(t);
}

static int
on_image(const char* channel, const corvis_image_t* msg, void* user_data);

static void
thumbnail_draw(thumbnail_t* t);

static void
renderer_camera_draw(BotViewer* viewer, BotRenderer* renderer)
{
    renderer_camera_t* s = (renderer_camera_t*) renderer->user;
    GLint viewport[4];

    glGetIntegerv(GL_VIEWPORT, viewport);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, viewport[2], 0, viewport[3]);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0, viewport[3], 0);
    glScalef(1, -1, 1);

    double vp_width = viewport[2] - viewport[0];
    double vp_height = viewport[3] - viewport[1];

    GList* tlist = g_hash_table_get_values(s->cam_handlers);
    for(int titer = 0; titer < g_list_length(tlist); titer++)
    {
        thumbnail_t* t = (thumbnail_t*) g_list_nth_data(tlist, titer);

        if(!t->last_image)
            continue;

        double aspect = t->last_image->width / (double) t->last_image->height;

        double thumb_width, thumb_height;
        if ((vp_width / 3) / aspect > vp_height / 3)
        {
            thumb_height = vp_height / 3;
            thumb_width = thumb_height * aspect;
        }
        else
        {
            thumb_width = vp_width / 3;
            thumb_height = thumb_width / aspect;
        }

        int rmode = bot_gtk_param_widget_get_enum(t->pw, PARAM_RENDER_IN);
        if (rmode == RENDER_IN_WIDGET)
            continue;

        int p1[2] = {viewport[0], viewport[1]};

        switch (rmode)
        {
            case RENDER_IN_BOTTOM_RIGHT:
                p1[0] = vp_width - thumb_width;
                p1[1] = vp_height - thumb_height;
                break;
            case RENDER_IN_BOTTOM_CENTER:
                p1[0] = vp_width / 3;
                p1[1] = vp_height - thumb_height;
                break;
            case RENDER_IN_BOTTOM_LEFT:
                p1[0] = 0;
                p1[1] = vp_height - thumb_height;
                break;
            case RENDER_IN_TOP_LEFT:
                p1[0] = 0;
                p1[1] = 0;
                break;
            case RENDER_IN_TOP_CENTER:
                p1[0] = vp_width / 3;
                p1[1] = 0;
                break;
            case RENDER_IN_TOP_RIGHT:
                p1[0] = vp_width - thumb_width;
                p1[1] = 0;
                break;
            default:
                break;
        }

        glPushMatrix();
        glTranslatef(p1[0], p1[1], 1);
        glScalef(thumb_width, thumb_height, 1);
        thumbnail_draw(t);
        glPopMatrix();
    }
    g_list_free(tlist);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

static void
on_load_preferences (BotViewer *viewer, GKeyFile *keyfile, void *user_data)
{
    renderer_camera_t* s = (renderer_camera_t*)user_data;

    GError* gerr = NULL;
    char** keys = g_key_file_get_keys(keyfile, RENDERER_NAME, NULL, &gerr);
    if (gerr)
    {
        g_error_free (gerr);
        return;
    }
    for (int i=0; keys[i]; i++)
    {
        char* key = keys[i];
        thumbnail_t* t = (thumbnail_t*) g_hash_table_lookup(s->cam_handlers, key);
        if (!t)
        {
            t = (thumbnail_t*) calloc (1, sizeof (thumbnail_t));
            t->channel = strdup(key);
            t->renderer = s;
            g_hash_table_replace(s->cam_handlers, t->channel, t);
        }
        char *val = g_key_file_get_string(keyfile, RENDERER_NAME, key, NULL);
        t->render_place = 0;
        t->expanded = 0;
        if (val)
            sscanf(val, "%d %d", &t->render_place, &t->expanded);
    }
    g_strfreev (keys);
}

static void
on_save_preferences (BotViewer *viewer, GKeyFile *keyfile, void *user_data)
{
    renderer_camera_t* s = (renderer_camera_t*) user_data;
    GList* keys = g_hash_table_get_keys(s->cam_handlers);

    for (int k = 0; k < g_list_length(keys); k++)
    {
        char* key = (char*) g_list_nth_data(keys, k);
        thumbnail_t* t = (thumbnail_t*) g_hash_table_lookup(s->cam_handlers, key);

        char str[80];
        sprintf(str, "%d %d", t->render_place, t->expanded);
        g_key_file_set_string(keyfile, RENDERER_NAME, key, str);
    }
}

static void
renderer_camera_free(BotRenderer* renderer)
{
    renderer_camera_t* s = (renderer_camera_t*) renderer;

    g_hash_table_destroy(s->cam_handlers);
    free(s);
}

void on_renderer_param_widget_changed(BotGtkParamWidget* pw, const char* name,
        renderer_camera_t* s)
{

}

BotRenderer* renderer_camera_new(BotViewer* _v)
{
    renderer_camera_t* s = new renderer_camera_t();
    
    s->viewer = _v;
    s->lcm = bot_lcm_get_global(NULL);
    
    s->renderer.draw = renderer_camera_draw;
    s->renderer.destroy = renderer_camera_free;
    s->renderer.name = (char*)RENDERER_NAME;
    s->renderer.user = s;
    s->renderer.enabled = 1;

    s->cam_handlers = g_hash_table_new_full(g_str_hash, g_str_equal,
            NULL, (GDestroyNotify) thumbnail_destroy);

    s->pw = BOT_GTK_PARAM_WIDGET(bot_gtk_param_widget_new());
    gtk_container_add(GTK_CONTAINER(s->renderer.widget), GTK_WIDGET(s->pw));
    gtk_widget_show(GTK_WIDGET(s->pw));

    g_signal_connect(G_OBJECT(s->pw), "changed",
            G_CALLBACK(on_renderer_param_widget_changed), s);

    return &s->renderer;
}

static void
renderer_camera_draw(renderer_camera_t* s)
{
}

void setup_renderer_camera(BotViewer* viewer, int render_priority)
{
    bot_viewer_add_renderer(viewer, renderer_camera_new(viewer), render_priority);
}

static void
thumbnail_draw(thumbnail_t* t)
{
    if(!t->renderer->renderer.enabled || !t->last_image)
        return;

    // create the texture object if necessary
    if(!t->texture)
    {
        t->texture = bot_gl_texture_new(t->last_image->width,
                t->last_image->height,
                t->last_image->width * 3 * t->last_image->height);
    }

    // upload the texture to the graphics card if necessary
    if(!t->is_uploaded)
    {
        int stride = 0;
        GLenum gl_format;
        uint8_t *tex_src = NULL;

        if(t->last_image->pixelformat == 0 ||
                t->last_image->pixelformat == PIXEL_FORMAT_GRAY ||
                t->last_image->pixelformat == PIXEL_FORMAT_BAYER_BGGR ||
                t->last_image->pixelformat == PIXEL_FORMAT_BAYER_RGGB ||
                t->last_image->pixelformat == PIXEL_FORMAT_BAYER_GRBG ||
                t->last_image->pixelformat == PIXEL_FORMAT_BAYER_GBRG)
        {

            stride = t->last_image->width;
            gl_format = GL_LUMINANCE;
            tex_src = t->last_image->image;
        }
        else if(t->last_image->pixelformat == PIXEL_FORMAT_MJPEG)
        {
            corvis_image_t* msg = t->last_image;

            // might need to JPEG decompress...
            stride = t->last_image->width * 3;
            int buf_size = msg->height * stride;
            if (t->uncompressed_buffer_size < buf_size)
            {
                t->uncompressed_buffer =
                    (uint8_t*)realloc(t->uncompressed_buffer, buf_size);
                t->uncompressed_buffer_size = buf_size;
            }
            jpeg_decompress_to_8u_rgb(msg->image, msg->size,
                    t->uncompressed_buffer, msg->width, msg->height, stride);

            gl_format = GL_RGB;
            tex_src = t->uncompressed_buffer;
        }
        else
            return;

        bot_gl_texture_upload(t->texture, gl_format, GL_UNSIGNED_BYTE,
                stride, tex_src);
        t->is_uploaded = 1;
    }

    // draw the image
    glColor3f (1, 1, 1);
    bot_gl_texture_draw(t->texture);
}

static gboolean
on_gl_area_expose(GtkWidget * widget, GdkEventExpose * event, void* user_data)
{
    thumbnail_t *t = (thumbnail_t*) user_data;

    bot_gtk_gl_drawing_area_set_context(t->gl_area);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, 1, 1, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    thumbnail_draw(t);

    bot_gtk_gl_drawing_area_swap_buffers(t->gl_area);
    return TRUE;
}

static void
on_gl_area_size(GtkWidget * widget, GtkAllocation * alloc, thumbnail_t * t)
{
    gtk_widget_set_size_request(widget, -1,
            alloc->width * t->height / t->width);
}

static void
on_expander_expanded (GtkExpander* expander, void* user_data)
{
    thumbnail_t *t = (thumbnail_t*) user_data;
    t->expanded = gtk_expander_get_expanded(expander);
}

static void
on_thumbnail_param_widget_changed(BotGtkParamWidget* pw, const char *param,
        void *user_data)
{
    thumbnail_t *t = (thumbnail_t*) user_data;

    // delete the old texture object if it exists.  make sure that we've
    // selected the correct OpenGL context
    if (t->texture)
    {
        if (t->render_place == RENDER_IN_WIDGET)
            bot_gtk_gl_drawing_area_set_context(t->gl_area);
        else
            bot_gtk_gl_drawing_area_set_context(t->renderer->viewer->gl_area);

        bot_gl_texture_free(t->texture);
        t->texture = NULL;
    }

    t->render_place = bot_gtk_param_widget_get_enum(pw, PARAM_RENDER_IN);
    if (t->render_place == RENDER_IN_WIDGET)
        gtk_widget_show (GTK_WIDGET(t->gl_area));
    else
        gtk_widget_hide (GTK_WIDGET(t->gl_area));

    t->is_uploaded = 0;
    bot_viewer_request_redraw (t->renderer->viewer);
}

static void
on_image(const lcm_recv_buf_t *rbuf, const char *channel,
        const corvis_image_t *msg, void *user_data)
{
    renderer_camera_t* s = (renderer_camera_t*) user_data;

    if (!s->renderer.enabled)
        return;

    thumbnail_t *t = (thumbnail_t*)g_hash_table_lookup(s->cam_handlers, channel);
    if (!t)
    {
        t = (thumbnail_t*) calloc(1, sizeof (thumbnail_t));
        t->renderer = s;
        t->render_place = 0;
        t->channel = strdup(channel);
        g_hash_table_replace(s->cam_handlers, t->channel, t);
    }

    if (!t->msg_received)
    {
        t->gl_area = BOT_GTK_GL_DRAWING_AREA(bot_gtk_gl_drawing_area_new(FALSE));

        t->pw = BOT_GTK_PARAM_WIDGET(bot_gtk_param_widget_new());
        bot_gtk_param_widget_add_enum(t->pw, PARAM_RENDER_IN,
                (BotGtkParamWidgetUIHint)0,
                t->render_place,
                "Here", RENDER_IN_WIDGET,
                "Top Left", RENDER_IN_TOP_LEFT,
                "Top Cent.", RENDER_IN_TOP_CENTER,
                "Top Right", RENDER_IN_TOP_RIGHT,
                "Bot. Left", RENDER_IN_BOTTOM_LEFT,
                "Bot. Cent.", RENDER_IN_BOTTOM_CENTER,
                "Bot. Right", RENDER_IN_BOTTOM_RIGHT,
                NULL);

        t->expander = gtk_expander_new(channel);
        gtk_box_pack_start(GTK_BOX (s->renderer.widget),
                t->expander, TRUE, TRUE, 0);
        GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
        gtk_container_add(GTK_CONTAINER (t->expander), vbox);

        gtk_box_pack_start(GTK_BOX (vbox),
               GTK_WIDGET(t->pw), TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX (vbox),
               GTK_WIDGET(t->gl_area), TRUE, TRUE, 0);

        g_signal_connect(G_OBJECT (t->gl_area), "size-allocate",
                G_CALLBACK(on_gl_area_size), t);
        t->width = msg->width;
        t->height = msg->height;

        gtk_widget_show_all(GTK_WIDGET (t->expander));
        gtk_expander_set_expanded(GTK_EXPANDER(t->expander), t->expanded);

        if (t->render_place == RENDER_IN_WIDGET)
            gtk_widget_show(GTK_WIDGET(t->gl_area));
        else
            gtk_widget_hide(GTK_WIDGET(t->gl_area));

        g_signal_connect(G_OBJECT(t->pw), "changed",
                G_CALLBACK(on_thumbnail_param_widget_changed), t);
        g_signal_connect(G_OBJECT(t->gl_area), "expose-event",
                G_CALLBACK(on_gl_area_expose), t);

        g_signal_connect(G_OBJECT(t->expander), "notify::expanded",
                G_CALLBACK(on_expander_expanded), t);

        t->texture = NULL;
        t->last_image = NULL;
        t->renderer = s;
        t->uncompressed_buffer_size = msg->width * msg->height * 3;
        t-> uncompressed_buffer =
            (uint8_t*) malloc (t->uncompressed_buffer_size);

        t->msg_received = 1;
    }

    if (t->last_image)
        corvis_image_t_destroy (t->last_image);

    t->last_image = corvis_image_t_copy(msg);
    t->is_uploaded = 0;

    switch(bot_gtk_param_widget_get_enum(t->pw, PARAM_RENDER_IN))
    {
        case RENDER_IN_WIDGET:
            if (gtk_expander_get_expanded (GTK_EXPANDER (t->expander)))
                bot_gtk_gl_drawing_area_invalidate(t->gl_area);
        default:
            bot_viewer_request_redraw (s->viewer);
            break;
    }
}
