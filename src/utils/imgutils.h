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

#endif
