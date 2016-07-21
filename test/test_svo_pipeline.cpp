#include <svo/config.h>
#include <svo/frame_handler_mono.h>
#include <svo/map.h>
#include <svo/frame.h>
#include <vector>
#include <string>
#include <vikit/math_utils.h>
#include <vikit/vision.h>
#include <vikit/abstract_camera.h>
#include <vikit/atan_camera.h>
#include <vikit/pinhole_camera.h>
#include <opencv2/opencv.hpp>
#include <sophus/se3.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>

using namespace std;
using namespace svo;

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("Usage: ./test_svo_pipeline [dirname]\n");
        exit(1);
    }

    vk::PinholeCamera cam(
                        1024, 768,
                        742.45, 749.25,
                        526.89, 396.70,
                        -0.307174, 0.096686, 0.000891, 0.000528, 0.0);

    FrameHandlerMono h(&cam);
    h.start();

    char* dirname = strdup(argv[1]);
    char tfname[256];
    sprintf(tfname, "%s/time.txt", dirname);
    ifstream tfp(tfname);

    for(int i=0; i< 500; i++)
    {
        char ifname[256];
        sprintf(ifname, "%s/img/%05d.jpg", dirname, i);
        cv::Mat m(cv::imread(ifname));
        cv::cvtColor(m, m, CV_BGR2GRAY);

        int64_t utime, idx;
        assert(!m.empty());
        tfp >> idx >> utime;
        assert(idx == i);

        double t = utime/(1e6);
        h.addImage(m, t);

        if(h.lastFrame())
        {
            printf("idx: %d, #features: %d, dt: %.3f [ms]\n",
                                h.lastFrame()->id_,
                                h.lastNumObservations(),
                                h.lastProcessingTime()*1000);

            //cout << "Pose: " << h.lastFrame()->T_f_w_ << endl;
           cout << h.lastFrame()->T_f_w_.rotationMatrix() << endl;
           cout << h.lastFrame()->T_f_w_.translation() << endl;
        }
    }

    tfp.close();
    return 0;
};

