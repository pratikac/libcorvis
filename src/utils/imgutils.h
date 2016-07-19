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

void cvmat_to_jpeg(Mat& m, vector<unsigned char>& buf)
{
    imencode(".jpg", m, buf);
}

#endif
