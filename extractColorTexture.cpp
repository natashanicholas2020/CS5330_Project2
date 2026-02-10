#include <opencv2/opencv.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <vector>

using namespace cv;
using namespace std;

bool isImageFile(const char *name) {
    return strstr(name, ".jpg") || strstr(name, ".png") ||
           strstr(name, ".ppm") || strstr(name, ".tif");
}

const int H_BINS = 16;
const int S_BINS = 8;
const int TEX_BINS = 16;

/* ---------- Color Histogram ---------- */
vector<double> colorHist(const Mat &img) {
    Mat hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);

    vector<double> hist(H_BINS * S_BINS, 0);

    for (int y = 0; y < hsv.rows; y++) {
        for (int x = 0; x < hsv.cols; x++) {
            Vec3b p = hsv.at<Vec3b>(y, x);
            int hBin = (p[0] * H_BINS) / 180;
            int sBin = (p[1] * S_BINS) / 256;

            if (hBin >= H_BINS) hBin = H_BINS - 1;
            if (sBin >= S_BINS) sBin = S_BINS - 1;

            hist[hBin * S_BINS + sBin]++;
        }
    }

    double total = img.rows * img.cols;
    for (double &v : hist) v /= total;

    return hist;
}

/* ---------- Texture Histogram ---------- */
vector<double> textureHist(const Mat &img) {
    Mat gray, gx, gy, mag;
    cvtColor(img, gray, COLOR_BGR2GRAY);

    Sobel(gray, gx, CV_32F, 1, 0, 3);
    Sobel(gray, gy, CV_32F, 0, 1, 3);
    magnitude(gx, gy, mag);

    double maxVal;
    minMaxLoc(mag, nullptr, &maxVal);

    vector<double> hist(TEX_BINS, 0);

    for (int y = 0; y < mag.rows; y++) {
        for (int x = 0; x < mag.cols; x++) {
            float v = mag.at<float>(y, x);
            int bin = (v / maxVal) * TEX_BINS;
            if (bin >= TEX_BINS) bin = TEX_BINS - 1;
            hist[bin]++;
        }
    }

    double total = mag.rows * mag.cols;
    for (double &v : hist) v /= total;

    return hist;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("usage: %s <image_dir> <output_csv>\n", argv[0]);
        return -1;
    }

    DIR *dirp = opendir(argv[1]);
    FILE *fp = fopen(argv[2], "w");

    struct dirent *dp;
    char filename[512];

    while ((dp = readdir(dirp)) != NULL) {
        if (!isImageFile(dp->d_name)) continue;

        sprintf(filename, "%s/%s", argv[1], dp->d_name);
        Mat img = imread(filename);
        if (img.empty()) continue;

        vector<double> cHist = colorHist(img);
        vector<double> tHist = textureHist(img);

        fprintf(fp, "%s", dp->d_name);
        for (double v : cHist) fprintf(fp, ",%.6f", v);
        for (double v : tHist) fprintf(fp, ",%.6f", v);
        fprintf(fp, "\n");
    }

    fclose(fp);
    closedir(dirp);
    return 0;
}
