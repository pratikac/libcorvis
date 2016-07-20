#include <corvis/common.h>
#include <corvis/config.h>

#include <gtk/gtk.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glib.h>

#include <lcm/lcm.h>
#include <bot_vis/bot_vis.h>
#include <lcmtypes/pointcloud_t.h>

#define RENDERER_NAME       "Pointcloud"
#define PARAM_RENDER_IN     "Show"
#define PARAM_SCAN_MEMORY   "Scan Memory"
#define COLOR_MENU          "Color"

#define MAX_SCAN_MEMORY     (5*10)

enum
{
    COLOR_DRAB,
    COLOR_Z,
};

typedef struct
{
    lcm_t* lcm;

    corvis_config_t* config;

    BotRenderer renderer;
    BotViewer* viewer;
    BotGtkParamWidget* pw;

    GHashTable* handlers;
} renderer_pointcloud_t;

typedef struct
{
    char* channel;
    BotPtrCircular* scans;
    
    renderer_camera_t* renderer;
    BotGtkParamWidget* pw;
    GtkWidget* expander;
    
    int msg_received;
    int render_place;
    int expanded;
} handler_t;

static void
handler_destroy(handler_t* h)
{
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
    s->param_big_points = FALSE;

    s->pw = BOT_GTK_PARAM_WIDGET(bot_gtk_param_widget_new());
    r->widget = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(r->widget), GTK_WIDGET(s->pw), TRUE, TRUE, 0);

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
    g_signal_connect(G_OBJECT(r->widget), "clicked", G_CALLBACK(on_clear_button), s);

    gtk_widget_show_all(r->widget);

    g_signal_connect(G_OBJECT(s->pw), "changed",
            G_CALLBACK(on_param_widget_changed), s);
    g_signal_connect(G_OBJECT(s->viewer), "load-preferences",
            G_CALLBACK(on_load_preferences), s);
    g_signal_connect(G_OBJECT(s->viewer), "save-preferences",
            G_CALLBACK(on_save_preferences), s);

    // subscribe to all channels
    s->handlers = g_hash_table_new(g_str_hash, g_str_equal);
    s->channels = g_ptr_array_new();

    corvis_pointcloud_t_subscription_t* hid;
    hid = corvis_pointcloud_t_subscribe(s->lcm, "POINTCLOUD_.*", on_pointcloud, s);
    s->lcm_hids = NULL;
    s->lcm_hids = g_list_append(s->lcm_hids, hid);

    return &s->renderer;
}

void setup_renderer_pointcloud(BotViewer* v, int p)
{
    BotRenderer* r = renderer_pointcloud_new(v);
    if(v && r)
        bot_viewer_add_renderer(v, r, p);
}
