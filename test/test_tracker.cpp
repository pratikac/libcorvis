#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <unistd.h>

#include <svo/global.h>
#include <svo/config.h>
#include <svo/frame.h>
#include <svo/feature_detection.h>
#include <svo/depth_filter.h>
#include <svo/feature.h>
#include <vikit/timer.h>
#include <vikit/vision.h>
#include <vikit/abstract_camera.h>
#include <vikit/atan_camera.h>

using namespace Eigen;
using namespace std;

void corner_detector(cv::Mat& img)
{

    vk::Timer t;
    vk::AbstractCamera* cam = new vk::ATANCamera(640, 480, 0.511496, 0.802603, 0.530199, 0.496011, 0.934092);
    svo::FramePtr frame(new svo::Frame(cam, img, 0.0));

    svo::feature_detection::FastDetector fast_detector(img.cols, img.rows, 25, 3);
    svo::Features fts;
    for(int i=0; i<10; i++)
        fast_detector.detect(frame.get(), frame->img_pyr_, svo::Config::triangMinCornerScore(), fts);

    for(auto& i : fts)
        cv::circle(img, cv::Point2f(i->px[0], i->px[1]), 4*(i->level+1), cv::Scalar(255,255,255), 1);
}

int main(int argc, char* argv[])
{
    int cam_idx = 0;
    if(argc > 1)
        cam_idx = atoi(argv[1]);

    int32_t width = 640, height = 480;

    bool show_video = true;

    cv::VideoCapture cap(cam_idx);
    if(!cap.isOpened())
    {
        printf("Cannot open camera\n");
        return 1;
    }
    cap.set(CV_CAP_PROP_FRAME_WIDTH, width);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, height);

    if(show_video)
        cv::namedWindow("vid0", CV_WINDOW_AUTOSIZE);

    while(1)
    {
        cv::Mat frame;
        bool res = cap.read(frame);
        cv::cvtColor(frame, frame, CV_BGR2GRAY);
        
        corner_detector(frame);

        if(show_video)
        {
            cv::imshow("vid0", frame);
            if(cv::waitKey(5) == 27)
                break;
        }
        usleep(100);
    }   
    return 0;
}
