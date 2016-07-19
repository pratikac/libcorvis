#include <corvis/common.h>
#include <corvis/imgutils.h>

#include <lcm/lcm.h>

#include <bot_vis/bot_vis.h>

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
} renderer_camera_t;

typedef struct
{
    char* channel;
    BotGtkGlDrawingArea* gl_area;

    corvis_image_t* last_image;
    BotGlTexture* texture;

    renderer_camera_t* renderer_camera;
    BotGtkParamWidget* pw;
    
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
        corvis_image_t_destory(t->last_image);
    if(t->uncompressed_buffer)
    {
        free(uncompressed_buffer);
        uncompressed_buffer = NULL;
    }
    free(t->channel);
    free(t);
}

static int
on_image(const char* channel, const image_t* msg, void* user_data);
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
}


BotRenderer* renderer_camera_new(BotViewer* _v)
{
    renderer_camera_t* s = new renderer_camera_t();
    
    s->viewer = _v;
    s->lcm = bot_lcm_get_global(NULL);
    
    s->renderer.draw = on_draw;
    
    s->pw = BOT_GTK_PARAM_WIDGET(bot_gtk_param_widget_new());
    gtk_container_add(GTK_CONTAINER(s->renderer.widget), GTK_WIDGET(s->pw));
    gtk_widget_show(GTK_WIDGET(s->pw));

    g_signal_connect(G_OBJECT(s->pw), "changed",
            G_CALLBACK(on_param_widget_changed), self);
}

void on_param_widget_changed(BotGtkParamWidget* pw, const char* name,
        renderer_camera_t* s)
{

}

static int
on_image(const char* channel, const image_t* msg, void* user_data)
{
    renderer_camera_t* s = (renderer_camera_t*) user_data;
    if(!s->parent.enabled)
        return 0;


}

void setup_renderer_camera(BotViewer* viewer, int render_priority)
{
    bot_viewer_add_renderer(viewer, renderer_camera_new(viewer), render_priority);
}
