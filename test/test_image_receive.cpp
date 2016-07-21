#include <corvis/common.h>
#include <corvis/imgutils.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <lcm/lcm.h>
#include <bot_core/bot_core.h>
#include <lcmtypes/corvis_image_t.h>
#include <vikit/timer.h>

using namespace cv;
using namespace std;


static void
handle(const lcm_recv_buf_t* rbuf, const char* channel,
        const corvis_image_t* msg, void* user)
{
    vector<unsigned char> buf;
    buf.assign(msg->data, msg->data + msg->size);

    vk::Timer t;
    t.start();
    Mat m = jpeg_to_cvmat(buf);
    t.stop();
    printf("[%lu]: %.3f [ms]\n", msg->utime, t.getTime()*1000);

    imshow("recv-img", m);
    waitKey(10);
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("Usage: test_lcm_camera_decode [channel_name]\n");
        return 0;
    }

    lcm_t* lcm = bot_lcm_get_global(NULL);
    corvis_image_t_subscribe(lcm, argv[1], &handle, NULL);
    
    namedWindow("recv-img", WINDOW_AUTOSIZE);

    while(1)
    {
        lcm_handle(lcm);
    }
    lcm_destroy(lcm);

    return 0;
}
