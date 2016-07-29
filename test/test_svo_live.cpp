#include <svo/config.h>
#include <svo/frame_handler_mono.h>
#include <svo/map.h>
#include <svo/frame.h>
#include <svo/feature_detection.h>
#include <svo/depth_filter.h>
#include <svo/feature.h>

#include <vikit/math_utils.h>
#include <vikit/vision.h>
#include <vikit/abstract_camera.h>
#include <vikit/atan_camera.h>
#include <vikit/pinhole_camera.h>
#include <sophus/se3.hpp>

#include <corvis/common.h>
#include <corvis/imgutils.h>

#include <lcm/lcm-cpp.hpp>
#include <bot_core/bot_core.h>
#include <lcmtypes/corvis/image_t.hpp>

#include <iostream>

using namespace std;
using namespace svo;

class node_t
{
    public:
        lcm::LCM lcm;

        vk::AbstractCamera* cam;
        FrameHandlerMono* h;
        
        node_t()
        {
            cam = new vk::PinholeCamera(
                        1024, 768,
                        574.9515, 574.6672,
                        530.0537, 397.5126,
                        -0.3565, 0.2036, -0.0693, 0, 0);

            h = new FrameHandlerMono(cam);
            h->start();

            lcm.subscribe("CAM_MAIN", &node_t::on_image, this);
            
            cv::namedWindow("debug", CV_WINDOW_AUTOSIZE);
        }
        ~node_t()
        {
            if(h)
                delete h;
            if(cam)
                delete cam;
        }

        void show_tracker(cv::Mat& m)
        {
            svo::FramePtr frame(new svo::Frame(cam, m, 0.0));

            svo::feature_detection::FastDetector fast_detector(m.cols, m.rows, 50, 3);
            svo::Features fts;
            for(int i=0; i<10; i++)
                fast_detector.detect(frame.get(), frame->img_pyr_, svo::Config::triangMinCornerScore(), fts);

            for(auto& i : fts)
                draw_crosshair(m, cv::Point2f(i->px[0], i->px[1]), 4*(i->level+1), cv::Scalar(255,255,255), 1);

            cv::imshow("debug", m);
            cv::waitKey(10);
        }

        void on_image(const lcm::ReceiveBuffer*, const std::string&,
                const corvis::image_t* msg)
        {
            int64_t tmp = msg->utime - 1468658014000000; 
            double t = (tmp)/(1e6);

            cv::Mat m = jpeg_to_cvmat(msg->data);
            //printf("Recv: [%dx%d]\n", m.cols, m.rows);

            /*
            h->addImage(m, t);
            if(h->lastFrame())
            {
                printf("idx: %d, #features: %d, dt: %.3f [ms]\n",
                        h->lastFrame()->id_,
                        h->lastNumObservations(),
                        h->lastProcessingTime()*1000);

                //cout << "Pose: " << h.lastFrame()->T_f_w_ << endl;
                //cout << h->lastFrame()->T_f_w_.inverse().rotationMatrix() << endl;
                cout << h->lastFrame()->T_f_w_.inverse().translation().transpose() << endl;
            }
            */

            show_tracker(m);
        }
};

int main()
{
    node_t n;
    
    while(!n.lcm.handle());

    return 0;
}
