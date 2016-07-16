#include <string>
#include <cstdlib>

#include <gtk/gtk.h>

#include <bot_vis/bot_vis.h>

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    glutInit(&argc, argv);
    g_thread_init(0);
    
    BotViewer* viewer = bot_viewer_new("corvis-viewer");

    char *fname = g_build_filename(g_get_user_config_dir(), ".corvis-viewerrc", NULL);
    
    bot_viewer_load_preferences(viewer, fname);

    // setup renderers
    bot_viewer_add_stock_renderer(viewer, BOT_VIEWER_STOCK_RENDERER_GRID, 1);

    gtk_main();

    bot_viewer_save_preferences(viewer, fname);

    bot_viewer_unref(viewer);
}
