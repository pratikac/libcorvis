#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <unistd.h>
#include <chrono>
#include <ctime>

using namespace std;

int main(int argc, char* argv[])
{
    int cam_idx = 0;
    if(argc > 1)
        cam_idx = atoi(argv[1]);

    int32_t width = 1366, height = 768;

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
        chrono::time_point<chrono::high_resolution_clock> time_start;
        time_start = chrono::high_resolution_clock::now();
        bool res = cap.read(frame);
        chrono::duration<double> dt = (chrono::high_resolution_clock::now() - time_start);
        if(!res)
        {
            printf("Cannot read from camera: %d\n", cam_idx);
            break;
        }
        if(show_video)
        {
            cv::imshow("vid0", frame);
            if(cv::waitKey(5) == 27)
                break;
        }
        double freq = 1.0/(dt.count());
        printf("%3f [Hz]\n", freq);

        usleep(100);
    }

    return 0;
}
