#include <lcm/lcm-cpp.hpp>

#include <lcmtypes/corvis/image_t.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <corvis/common.h>

#include <bot_vis/bot_vis.h>
using namespace std;

typedef struct options_t
{
    char* channel;
    int idx, width, height, freq, grayscale;

    options_t()
    {
        channel = (char*)"CAMERA0";
        idx = 0;
        width = 640;
        height = 480;
        freq = 30;
        grayscale = 1;
    }
}options_t;

options_t options;

int parse_options(int argc, char* argv[])
{
    GOptionEntry entries[] = {
        {"channel",     'c', 0,     G_OPTION_ARG_STRING,    &options.channel,   "LCM channel name",             NULL},
        {"device",      'i', 0,     G_OPTION_ARG_INT,       &options.idx,       "Device Id, default: 0",        NULL},
        {"width",       'w', 0,     G_OPTION_ARG_INT,       &options.width,     "Frame width, default: 640",    NULL},
        {"height",      'h', 0,     G_OPTION_ARG_INT,       &options.height,    "Frame height, default: 480",   NULL},
        {"grayscale",   'g', 0,     G_OPTION_ARG_INT,       &options.grayscale, "Grayscale, default: 1",        NULL},
        {"freq",        'f', 0,     G_OPTION_ARG_INT,       &options.freq,      "Frequency, default: 30",       NULL},
        {NULL}
    };
    GError* error = NULL;
    GOptionContext* context = g_option_context_new("usb camera driver");
    
    g_option_context_add_main_entries(context, entries, NULL);
    if(!g_option_context_parse(context, &argc,&argv, &error))
    {
        g_assert(error);
        ERR("Error: %s\n%s", error->message, g_option_context_get_help(context, TRUE, NULL));
        return 1;
    }
    if(argc > 1)
    {
        ERR("Error: Unknown options\n%s", g_option_context_get_help(context, TRUE, NULL));
        return 1;
    }
    
    printf("Initializing camera [%d]: %dx%d, %d [Hz] on %s\n",
            options.idx, options.width, options.height, options.freq, options.channel);
    
    return 0;
}

int main(int argc, char* argv[])
{

    if(parse_options(argc, argv))
        return 1;



    return 0;
}
