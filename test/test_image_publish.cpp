#include <corvis/common.h>
#include <corvis/imgutils.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <lcm/lcm.h>
#include <bot_core/bot_core.h>
#include <lcmtypes/corvis_image_t.h>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        printf("Usage: test_decoder [img_name] [channel_name]\n");
        return 0;
    }
    
    char* fname = argv[1];
    auto m = imread(fname, CV_LOAD_IMAGE_COLOR);
    cv::cvtColor(m, m, CV_BGR2GRAY);
    
    if(!m.data)
    {
        printf("Could not read file\n");
        return 1;
    }

    vector<uint8_t> buf = cvmat_to_jpeg(m, 25);
        
    corvis_image_t msg;
    msg.utime = bot_timestamp_now();
    msg.width = m.cols;
    msg.height = m.rows;

    msg.data = (uint8_t*)malloc(buf.size()*sizeof(uint8_t));
    msg.size = buf.size();
    memcpy(msg.data, &(buf[0]), buf.size()*sizeof(uint8_t));

    lcm_t* lcm = bot_lcm_get_global(NULL);
    while(1)
    {
        usleep(1000*100);

        msg.utime = bot_timestamp_now();
        corvis_image_t_publish(lcm, argv[2], &msg);
    }
    return 0;
}
