#include <lcm/lcm-cpp.hpp>
#include <bot_core/bot_core.h>
#include <lcmtypes/corvis/image_t.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <corvis/common.h>
#include <corvis/imgutils.h>

#include <bot_vis/bot_vis.h>
using namespace std;

typedef struct options_t
{
    char* channel;
    int idx, width, height, freq, grayscale, quality;

    options_t()
    {
        channel = (char*)"CAMERA0";
        idx = 0;
        width = 640;
        height = 480;
        freq = 30;
        grayscale = 1;
        quality = 95;
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
        {"quality",     'q', 0,     G_OPTION_ARG_INT,       &options.quality,   "Quality, default: 95",         NULL},
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
    
    printf("Initializing camera [%d]: %dx%d, %d [Hz] on %s, quality: %d\n",
            options.idx, options.width, options.height, options.freq, options.channel, options.quality);
    
    return 0;
}

int main(int argc, char* argv[])
{
    if(parse_options(argc, argv))
        return 1;

    lcm::LCM lcm;
    
    cv::VideoCapture cap(options.idx);
    if(!cap.isOpened())
    {
        printf("Cannot open camera\n");
        return 1;
    }
    cap.set(CV_CAP_PROP_FRAME_WIDTH, options.width);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, options.height);

    int wait_time_ms = 1000/(float)options.freq;

    char channel_thumb[256];
    sprintf(channel_thumb, "%s_THUMB", options.channel);

    while(1)
    {
        cv::Mat m;
        
        bool res = cap.read(m);
        if(!res)
        {
            printf("Cannot read from camera: %d\n", options.idx);
            return 1;
        }

        corvis::image_t msg;
        msg.camera_name = "LOGITECH_USB";
        msg.device_utime = bot_timestamp_now();

        if(options.grayscale)
            cv::cvtColor(m, m, CV_BGR2GRAY);

        msg.data = cvmat_to_jpeg(m, options.quality);
        msg.width = m.cols;
        msg.height = m.rows;
        msg.size = msg.data.size();
        msg.utime = bot_timestamp_now();
        lcm.publish(options.channel, &msg);

        // also publish the thumbnail
        if(!options.grayscale)
            cv::cvtColor(m, m, CV_BGR2GRAY);
        cv::resize(m, m, cv::Size(640, 480));
        msg.width = m.cols;
        msg.height = m.rows;
        msg.data = cvmat_to_jpeg(m, options.quality);
        msg.size = msg.data.size();
        lcm.publish(channel_thumb, &msg);
        
        usleep(wait_time_ms*1000);
    }

    return 0;
}
