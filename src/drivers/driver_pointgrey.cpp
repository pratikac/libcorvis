#include <corvis/common.h>
#include <corvis/imgutils.h>
#include <flycapture/FlyCapture2.h>

#include <lcm/lcm-cpp.hpp>
#include <bot_core/bot_core.h>
#include <lcmtypes/corvis/image_t.hpp>
#include <lcmtypes/corvis/image_type_t.hpp>

typedef struct options_t
{
    char* channel;
    int mode, grayscale, quality;

    options_t()
    {
        channel = (char*)"CAMERA0";
        mode = 1;
        grayscale = 1;
        quality = 95;
    }
}options_t;

options_t options;

int parse_options(int argc, char* argv[])
{
    GOptionEntry entries[] = {
        {"channel",     'c', 0,     G_OPTION_ARG_STRING,    &options.channel,   "LCM channel name",             NULL},
        {"mode",        'm', 0,     G_OPTION_ARG_INT,       &options.mode,      "Mode, defaulut: 1, Full = 0, Half = 1", NULL},
        {"grayscale",   'g', 0,     G_OPTION_ARG_INT,       &options.grayscale, "Grayscale, defaulut: 1", NULL},
        {"quality",     'q', 0,     G_OPTION_ARG_INT,       &options.quality,   "Quality, default: 95",         NULL},
        {NULL}
    };
    GError* error = NULL;
    GOptionContext* context = g_option_context_new("pointgrey camera driver");
    
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
    
    printf("Initializing pointgrey camera: mode: [%d], grayscale: %d on %s, quality: %d\n",
            options.mode, options.grayscale, options.channel, options.quality);
    
    return 0;
}

int main(int argc, char* argv[])
{
    if(parse_options(argc, argv))
        return 1;
    
    lcm::LCM lcm;

    FlyCapture2::GigECamera gcam;
    gcam.Connect(0);

    uint packet_size;
    gcam.DiscoverGigEPacketSize(&packet_size);

    FlyCapture2::GigEProperty prop;
    prop.propType = FlyCapture2::PACKET_SIZE;
    gcam.GetGigEProperty(&prop);
    prop.value = packet_size;
    gcam.SetGigEProperty(&prop);

    FlyCapture2::GigEProperty prop2;
    prop2.propType = FlyCapture2::PACKET_DELAY;
    prop2.value = 4000;
    gcam.SetGigEProperty(&prop2);
    
    FlyCapture2::BusManager bus_manager;
    uint num_cameras;
    if(bus_manager.GetNumOfCameras(&num_cameras) != FlyCapture2::PGRERROR_OK || num_cameras == 0)
    {
        printf("No pointgrey camera found\n");
        exit(1);
    }
    printf("Number of cameras: %d\n", num_cameras);

    FlyCapture2::Camera camera;
    if(camera.Connect(0) != FlyCapture2::PGRERROR_OK)
    {
        printf("Cannot connect to the first camera\n");
        exit(1);
    }

    FlyCapture2::CameraInfo cam_info;
    camera.GetCameraInfo(&cam_info);
    printf("Connected to camera: %d\n", cam_info.serialNumber);

    // set Format7 information
    FlyCapture2::Format7Info fmt_info;
    bool supported;
    if(options.mode == 0)
        fmt_info.mode = FlyCapture2::MODE_0;
    else if(options.mode == 1)
        fmt_info.mode = FlyCapture2::MODE_1;
    else
    {
        printf("Unsupported mode: %d\n", options.mode);
        exit(1);
    }
    
    camera.GetFormat7Info(&fmt_info, &supported);
    FlyCapture2::Format7ImageSettings fmt_image_settings;
    fmt_image_settings.mode = fmt_info.mode;
    fmt_image_settings.pixelFormat = FlyCapture2::PIXEL_FORMAT_RGB8;
    fmt_image_settings.width = fmt_info.maxWidth;
    fmt_image_settings.height = fmt_info.maxHeight;
    fmt_image_settings.offsetX = 0;
    fmt_image_settings.offsetY  = 0;
    FlyCapture2::Format7PacketInfo fmt_packet_info;
    
    bool valid;
    camera.ValidateFormat7Settings(&fmt_image_settings, &valid, &fmt_packet_info);
    camera.SetFormat7Configuration(&fmt_image_settings, fmt_packet_info.recommendedBytesPerPacket);
    printf("fmt recommended bytes per packets: %d\n", fmt_packet_info.recommendedBytesPerPacket);

    // set frame rate
    FlyCapture2::Property property;
    property.type = FlyCapture2::PropertyType::FRAME_RATE;
    camera.GetProperty(&property);
    property.onOff = true;
    property.autoManualMode = false;
    property.absControl = true;
    property.absValue = 100;
    camera.SetProperty(&property);
    camera.GetProperty(&property);

    int wait_time_ms = 1000/(float)100;

    camera.StartCapture();
    FlyCapture2::Image raw_image;
    while(1)
    {
        if(camera.RetrieveBuffer(&raw_image) == FlyCapture2::PGRERROR_OK)
        {
            corvis::image_t msg;
            msg.camera_name = "BFLY-PGE-31S4C-C";
            msg.device_utime = bot_timestamp_now();
            msg.type = corvis::image_type_t::jpeg;
            
            msg.width = raw_image.GetCols();
            msg.height = raw_image.GetRows();
            
            FlyCapture2::Image rgb_image;
            raw_image.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &rgb_image);
            size_t row_bytes = (double)rgb_image.GetReceivedDataSize()/(double)rgb_image.GetRows(); 
            cv::Mat m = cv::Mat(rgb_image.GetRows(),
                                rgb_image.GetCols(),
                                CV_8UC3,
                                rgb_image.GetData(),
                                row_bytes);
            if(options.grayscale)
                cv::cvtColor(m, m, CV_BGR2GRAY);

            msg.data = cvmat_to_jpeg(m, options.quality);
            msg.size = msg.data.size();
            
            msg.utime = bot_timestamp_now();
            lcm.publish(options.channel, &msg);
        }
        else
            printf("Dropped frame\n");
        
        usleep(wait_time_ms*1000);
    }
    return 0;
}

