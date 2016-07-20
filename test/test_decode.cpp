#include <corvis/imgutils.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vikit/timer.h>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("Usage: test_decoder [img_name]\n");
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

    vector<unsigned char> buf;
    cvmat_to_jpeg(m, buf, 50);

    cv::Mat m2;
    jpeg_to_cvmat(buf, m2);

#if 1
    vk::Timer t;
    t.start();
    for(int i=0; i<100; i++)
        cvmat_to_jpeg(m, buf, 50);
    t.stop();
    printf("[encode]: %.3f [ms]\n", t.getTime()*10);
    
    t.start();
    for(int i=0; i<100; i++)
        jpeg_to_cvmat(buf, m2);
    t.stop();
    printf("[decode]: %.3f [ms]\n", t.getTime()*10);
#endif

    namedWindow("org-img", WINDOW_AUTOSIZE);
    namedWindow("decoded-img", WINDOW_AUTOSIZE);
    imshow("org-img", m);
    imshow("decoded-img", m2);
    
    waitKey(0);
    destroyAllWindows();

    return 0;
}
