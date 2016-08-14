#include <corvis/common.h>
#include <corvis/imgutils.h>

#include <gtk/gtk.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glib.h>

#include <lcm/lcm.h>
#include <bot_vis/bot_vis.h>
#include <lcmtypes/corvis_pointcloud_t.h>

#define RENDERER_NAME       "Pointcloud"
#define PARAM_RENDER_IN     "Show"
#define PARAM_POINT_SIZE    "Point Size"
#define PARAM_SCAN_MEMORY   "Scan Memory"
#define PARAM_COLOR_MODE    "Color Mode"

#define MAX_POINT_SIZE      (10)
#define MAX_SCAN_MEMORY     (5*10)

enum
{
    COLOR_DRAB,
    COLOR_Z,
};

typedef struct
{
    lcm_t* lcm;

    BotRenderer renderer;
    BotViewer* viewer;
    BotGtkParamWidget* pw;

    corvis_pointcloud_t_subscription_t* hid;
    GHashTable* handlers;
    
    int param_scan_memory = 8;
    int param_color_mode = COLOR_Z;
    int param_point_size = 0;

} renderer_pointcloud_t;

typedef struct
{
    int enabled;
    char* channel;
    BotPtrCircular* scans;

    int msg_received;
} handler_t;

static void
handler_destroy(handler_t* h)
{
    if(h->scans)
        bot_ptr_circular_destroy(h->scans);
    free(h);
}

static void
renderer_pointcloud_destroy(BotRenderer* r)
{
    if(!r)
        return;
    renderer_pointcloud_t* s = (renderer_pointcloud_t*) r->user;
    if(!s)
        return;

    if(s->lcm)
        corvis_pointcloud_t_unsubscribe(s->lcm, s->hid);

    if(s->handlers)
    {
        GList* plist = g_hash_table_get_values(s->handlers);
        for(int i=0; i<g_list_length(plist); i++)
        {
            handler_t* h = (handler_t*) g_list_nth_data(plist, i);
            handler_destroy(h);
        }
        g_list_free(plist);
        g_hash_table_destroy(s->handlers);
    }
    free(s);
}

static void
renderer_pointcloud_draw(BotViewer* v, BotRenderer* r)
{
    renderer_pointcloud_t* s = (renderer_pointcloud_t*) r->user;

    glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_POINT_BIT | GL_CURRENT_BIT);

    glPointSize(s->param_point_size);

    glBegin(GL_POINTS);
    GList* plist = g_hash_table_get_values(s->handlers);
    for(int hidx=0; hidx<g_list_length(plist); hidx++)
    {
        handler_t* h = (handler_t*) g_list_nth_data(plist, hidx);

        if(!h->enabled)
            continue;

        int scan_count = MIN(s->param_scan_memory, bot_ptr_circular_size(h->scans));
        for(int sidx =0; sidx < scan_count; sidx++)
        {
            corvis_pointcloud_t* p = (corvis_pointcloud_t*) bot_ptr_circular_index(h->scans, sidx);
            for(int i=0; i<p->size; i++)
            {
                switch(s->param_color_mode)
                {
                    case COLOR_DRAB:
                        glColor3d(h->color[0], h->color[1], h->color[2]);
                        break;
                    case COLOR_MODE_Z:
                        glColor3fv(colormap_jet(5));
                        break;
                    default:
                        glColor3d(0.3, 0.3, 0.3);
                        break;
                }
                float tmp[3] = {p->points[i][0], p->points[i][1], p->points[i][2]};
                glVertex3fv(tmp);
            }
        }

    }
    g_list_free(plist);

    glEnd();
    glPopAttrib();
}

static void
on_pointcloud(const lcm_recv_buf_t* rbuf, const char* c,
        const corvis_pointcloud_t* msg, void* user)
{
    // enabled, color, store scans
}

static void
on_clear_button(GtkWidget* button, void* user)
{
    renderer_pointcloud_t* s = (renderer_pointcloud_t*) user;
    if(!s->viewer)
        return;

    GList* plist = g_hash_table_get_values(s->handlers);
    for(int i=0; i<g_list_length(plist); i++)
    {
        handler_t* h = (handler_t*) g_list_nth_data(plist, i);
        bot_ptr_circular_clear(h->scans);
    }
    g_list_free(plist);
    bot_viewer_request_redraw(s->viewer);
}

static void
on_param_widget_changed(BotGtkParamWidget *pw, const char *name, void *user)
{
    renderer_pointcloud_t* s = (renderer_pointcloud_t*) user;

    s->param_scan_memory = bot_gtk_param_widget_get_int(s->pw, PARAM_SCAN_MEMORY);
    s->param_color_mode = bot_gtk_param_widget_get_enum(s->pw, PARAM_COLOR_MODE);

    GList* plist = g_hash_table_get_values(s->handlers);
    for (unsigned int i = 0; i < g_list_length(plist); i++)
    {
        handler_t* h = (handler_t*) g_list_nth_data(plist, i);
        bot_ptr_circular_resize(h->scans, s->param_scan_memory);

        h->enabled = bot_gtk_param_widget_get_bool(s->pw, h->channel);
    }
    g_list_free(plist);
    bot_viewer_request_redraw(s->viewer);
}

static void
on_load_preferences(BotViewer *viewer, GKeyFile *keyfile, void *user_data)
{
    renderer_pointcloud_t* s = (renderer_pointcloud_t*) user_data;
    bot_gtk_param_widget_load_from_key_file(s->pw, keyfile, RENDERER_NAME);
}

static void
on_save_preferences(BotViewer *viewer, GKeyFile *keyfile, void *user_data)
{
    renderer_pointcloud_t* s = (renderer_pointcloud_t*) user_data;
    bot_gtk_param_widget_save_to_key_file(s->pw, keyfile, RENDERER_NAME);
}

BotRenderer* renderer_pointcloud_new(BotViewer* _v)
{
    if(!_v)
        return NULL;

    renderer_pointcloud_t* s = new renderer_pointcloud_t();
    s->viewer = _v;

    BotRenderer* r = &s->renderer;
    r->draw = renderer_pointcloud_draw;
    r->destroy = renderer_pointcloud_destroy;
    r->user = s;
    r->name = (char*)RENDERER_NAME;
    r->enabled = 1;

    s->lcm = bot_lcm_get_global(NULL);
    if(!s->lcm)
        return NULL;

    s->param_scan_memory = 8;
    s->param_color_mode = COLOR_Z;
    s->param_point_size = 2;

    s->pw = BOT_GTK_PARAM_WIDGET(bot_gtk_param_widget_new());
    r->widget = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(r->widget), GTK_WIDGET(s->pw), TRUE, TRUE, 0);

    bot_gtk_param_widget_add_int(s->pw, PARAM_POINT_SIZE,
            BOT_GTK_PARAM_WIDGET_SLIDER, 1,
            MAX_POINT_SIZE, 1, s->param_point_size);
    bot_gtk_param_widget_add_int(s->pw, PARAM_SCAN_MEMORY,
            BOT_GTK_PARAM_WIDGET_SLIDER, 1,
            MAX_SCAN_MEMORY, 1, s->param_scan_memory);
    bot_gtk_param_widget_add_enum(s->pw, PARAM_COLOR_MODE,
            BOT_GTK_PARAM_WIDGET_MENU,
            s->param_color_mode,
            "Drab", COLOR_DRAB,
            "Height", COLOR_Z, NULL);

    GtkWidget* clear_button = gtk_button_new_with_label("Clear memory");
    gtk_box_pack_start(GTK_BOX(r->widget), clear_button, FALSE, FALSE, 0);
    g_signal_connect(G_OBJECT(clear_button), "clicked", G_CALLBACK(on_clear_button), s);

    gtk_widget_show_all(r->widget);

    g_signal_connect(G_OBJECT(s->pw), "changed",
            G_CALLBACK(on_param_widget_changed), s);
    g_signal_connect(G_OBJECT(s->viewer), "load-preferences",
            G_CALLBACK(on_load_preferences), s);
    g_signal_connect(G_OBJECT(s->viewer), "save-preferences",
            G_CALLBACK(on_save_preferences), s);

    // subscribe to all channels
    s->handlers = g_hash_table_new(g_str_hash, g_str_equal);
    s->hid = corvis_pointcloud_t_subscribe(s->lcm, "POINTCLOUD_.*", on_pointcloud, s);

    return &s->renderer;
}

void setup_renderer_pointcloud(BotViewer* v, int p)
{
    BotRenderer* r = renderer_pointcloud_new(v);
    if(v && r)
        bot_viewer_add_renderer(v, r, p);
}
