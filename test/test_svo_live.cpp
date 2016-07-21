#include <svo/config.h>
#include <svo/frame_handler_mono.h>
#include <svo/map.h>
#include <svo/frame.h>
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
            cam = new vk::ATANCamera(1920, 1080, 0.976421, 1.31701, 0.514889, 0.535875, 0);
            h = new FrameHandlerMono(cam);
            h->start();

            lcm.subscribe("CAM_MAIN", &node_t::on_image, this);
        }
        ~node_t()
        {
            if(h)
                delete h;
            if(cam)
                delete cam;
        }

        void on_image(const lcm::ReceiveBuffer*, const std::string&,
                const corvis::image_t* msg)
        {
            int64_t tmp = msg->utime - 1468658014000000; 
            double t = (tmp)/(1e6);

            cv::Mat m = jpeg_to_cvmat(msg->data);
            //printf("Recv: [%dx%d]\n", m.cols, m.rows);

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
        }
};

int main()
{
    node_t n;
    
    while(!n.lcm.handle());

    return 0;
}
