#include <corvis/common.h>
#include <corvis/imgutils.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <lcm/lcm-cpp.hpp>
#include <bot_core/bot_core.h>
#include <lcmtypes/corvis/image_t.hpp>
#include <lcmtypes/corvis/image_type_t.hpp>

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
    //cv::cvtColor(m, m, CV_BGR2GRAY);
    
    if(!m.data)
    {
        printf("Could not read file\n");
        return 1;
    }

    corvis::image_t msg;
    msg.camera_name = "DUMMY_IMAGE";
    msg.utime = bot_timestamp_now();
    msg.encoding = corvis::image_type_t::jpeg;

    msg.width = m.cols;
    msg.height = m.rows;

    msg.data = cvmat_to_jpeg(m, 75);
    msg.size = msg.data.size();

    lcm::LCM lcm;
    while(1)
    {
        usleep(1000*100);

        msg.utime = bot_timestamp_now();
        lcm.publish(argv[2], &msg);
    }
    return 0;
}
