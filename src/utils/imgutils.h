#ifndef __imgutils_h__
#define __imgutils_h__

#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void jpeg_to_cvmat(vector<unsigned char>& buf, Mat& m)
{
    m = imdecode(buf, -1);
}

void cvmat_to_jpeg(Mat& m, vector<unsigned char>& buf, int quality = 95)
{
    vector<int> params = {CV_IMWRITE_JPEG_QUALITY, quality};
    imencode(".jpg", m, buf, params);
}

void jpeg_decompress_8u_rgb(const uint8_t* src, int sz,
        uint8_t* dest, int width, int height, int stride)
{
    vector<uint8_t> tmp;
    tmp.assign(src, src + sz);

    Mat m;
    jpeg_to_cvmat(tmp, m);
    memcpy(dest, m.data, width*height*sizeof(uint8_t));
}

#endif
