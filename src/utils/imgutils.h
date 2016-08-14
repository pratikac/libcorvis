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

/** Given an array of colors, a palette is created that linearly interpolates through all the colors. **/
static void
color_util_build_color_table(double color_palette[][3], int palette_size, float lut[][3], int lut_size)
{
    for (int idx = 0; idx < lut_size; idx++) {
        double znorm = ((double) idx) / lut_size;

        int color_index = (palette_size - 1) * znorm;
        double alpha = (palette_size - 1) * znorm - color_index;

        for (int i = 0; i < 3; i++) {
            lut[idx][i] = color_palette[color_index][i] * (1.0 - alpha) + color_palette[color_index+1][i]*alpha;
        }    
    }
}

#define JET_COLORS_LUT_SIZE 1024
static float jet_colors[JET_COLORS_LUT_SIZE][3];
static int jet_colors_initialized = 0;

static void init_color_table_jet()
{
    double jet[][3] = {{ 0,   0,   1 },
        { 0,  .5,  .5 },
        { .8, .8,   0 },
        { 1,   0,   0 }};

    color_util_build_color_table(jet, sizeof(jet)/(sizeof(double)*3), jet_colors, JET_COLORS_LUT_SIZE);
    jet_colors_initialized = 1;
}

static inline
float *colormap_jet(double v)
{
    if (!jet_colors_initialized)
        init_color_table_jet();

    v = fmax(0, v);
    v = fmin(1, v);

    int idx = (JET_COLORS_LUT_SIZE - 1) * v;
    return jet_colors[idx];
}


#endif
