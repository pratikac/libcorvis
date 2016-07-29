#ifndef __imgutils_h__
#define __imgutils_h__

#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat jpeg_to_cvmat(const vector<unsigned char>& buf, int is_color = CV_LOAD_IMAGE_ANYDEPTH)
{
    return imdecode(buf, is_color);
}

vector<unsigned char> cvmat_to_jpeg(const Mat& m, int quality = 95)
{
    vector<unsigned char> buf;
    vector<int> params = {CV_IMWRITE_JPEG_QUALITY, quality};
    imencode(".jpg", m, buf, params);
    return buf;
}

void draw_crosshair(Mat& m, Point2f p, int len, Scalar c, int thickness=1)
{
    int rows = m.rows, cols = m.cols;
    Point2f t1 = p, t2 = p;

    t1.x = p.x + len/2;
    t1.y = p.y;
    t2.x = p.x - len/2;
    t2.y = p.y;
    line(m, t1, t2, c, thickness, 8);

    t1.x = p.x;
    t1.y = p.y + len/2;
    t2.x = p.x;
    t2.y = p.y - len/2;
    line(m, t1, t2, c, thickness, 8);
}

#endif
