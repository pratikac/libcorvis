#ifndef __imgutils_h__
#define __imgutils_h__

#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void jpeg_to_cvmat(const vector<unsigned char>& buf, Mat& m, int is_color = CV_LOAD_IMAGE_ANYDEPTH)
{
    m = imdecode(buf, is_color);
}

void cvmat_to_jpeg(const Mat& m, vector<unsigned char>& buf, int quality = 95)
{
    vector<int> params = {CV_IMWRITE_JPEG_QUALITY, quality};
    imencode(".jpg", m, buf, params);
}

#endif
