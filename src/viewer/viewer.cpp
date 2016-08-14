#include <string>
#include <cstdlib>

#include <gtk/gtk.h>

#include <lcm/lcm.h>
#include <bot_core/bot_core.h>
#include <bot_vis/bot_vis.h>
#include <bot_lcmgl_render/lcmgl_bot_renderer.h>
#include <bot_frames/bot_frames_renderers.h>

#include "udp_util.h"
#include "view_menu.h"

void setup_renderer_camera(BotViewer*, int);
void setup_renderer_pointcloud(BotViewer* v, int p);

static int
logplayer_remote_on_key_press(BotViewer *viewer, BotEventHandler *ehandler,
        const GdkEventKey *event)
{
    int keyval = event->keyval;

    switch (keyval)
    {
        case 'P':
        case 'p':
            udp_send_string("127.0.0.1", 53261, "PLAYPAUSETOGGLE");
            break;
        case 'N':
        case 'n':
            udp_send_string("127.0.0.1", 53261, "STEP");
            break;
        case '=':
        case '+':
            udp_send_string("127.0.0.1", 53261, "FASTER");
            break;
        case '_':
        case '-':
            udp_send_string("127.0.0.1", 53261, "SLOWER");
            break;
        case '[':
            udp_send_string("127.0.0.1", 53261, "BACK5");
            break;
        case ']':
            udp_send_string("127.0.0.1", 53261, "FORWARD5");
            break;
        default:
            return 0;
    }

    return 1;
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    glutInit(&argc, argv);
    setlinebuf(stdout);

    lcm_t* lcm = bot_lcm_get_global(NULL);
    BotParam* param = bot_param_new_from_server(lcm, 1);
    BotFrames* frames = bot_frames_get_global(lcm, param);

    BotViewer* viewer = bot_viewer_new("corvis-viewer");
    bot_glib_mainloop_attach_lcm(lcm);
    setup_view_menu(viewer);

    // setup renderers
    bot_viewer_add_stock_renderer(viewer, BOT_VIEWER_STOCK_RENDERER_GRID, 1);
    bot_lcmgl_add_renderer_to_viewer(viewer, lcm, 0);
    bot_frames_add_renderer_to_viewer(viewer, 1, frames);

    setup_renderer_camera(viewer, 1);
    setup_renderer_pointcloud(viewer, 1);

    // logplayer controls
    BotEventHandler *ehandler = (BotEventHandler*) calloc(1, sizeof(BotEventHandler));
    ehandler->name = (char*)"LogPlayer Remote";
    ehandler->enabled = 1;
    ehandler->key_press = logplayer_remote_on_key_press;
    bot_viewer_add_event_handler(viewer, ehandler, 0);

    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    char *fname = g_build_filename(g_get_user_config_dir(), ".corvis-viewerrc", NULL);
    bot_viewer_load_preferences(viewer, fname);
    gtk_main();

    bot_viewer_save_preferences(viewer, fname);
    free(fname);

    bot_viewer_unref(viewer);
    bot_glib_mainloop_detach_lcm(lcm);

    if(param)
        bot_param_destroy(param);
    if(frames)
        bot_frames_destroy(frames);

    return 0;
}
