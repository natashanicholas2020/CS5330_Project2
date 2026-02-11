#include <opencv2/opencv.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <vector>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

const int H_BINS = 16;
const int S_BINS = 8;
const int ORI_BINS = 16;

// 2D color histogram (H,S)
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

// Gradient orientation histogram
vector<double> orientationHist(const Mat &img) {
    Mat gray, gx, gy, angle;
    cvtColor(img, gray, COLOR_BGR2GRAY);

    Sobel(gray, gx, CV_32F, 1, 0, 3);
    Sobel(gray, gy, CV_32F, 0, 1, 3);

    phase(gx, gy, angle, true);

    vector<double> hist(ORI_BINS, 0);
    for (int y = 0; y < angle.rows; y++) {
        for (int x = 0; x < angle.cols; x++) {
            float a = angle.at<float>(y, x);
            int bin = (int)(a / 360.0 * ORI_BINS);
            if (bin >= ORI_BINS) bin = ORI_BINS - 1;
            hist[bin]++;
        }
    }

    double total = angle.rows * angle.cols;
    for (double &v : hist) v /= total;
    return hist;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("usage: %s <image_dir> <output_csv>\n", argv[0]);
        return -1;
    }

    char *dirname = argv[1];
    char *outname = argv[2];

    DIR *dirp = opendir(dirname);
    if (!dirp) return -1;

    FILE *fp = fopen(outname, "w");
    if (!fp) return -1;

    struct dirent *dp;
    while ((dp = readdir(dirp)) != NULL) {
        if (!strstr(dp->d_name, ".jpg")) continue;

        string path = string(dirname) + "/" + dp->d_name;
        Mat img = imread(path);
        if (img.empty()) continue;

        vector<double> c = colorHist(img);
        vector<double> o = orientationHist(img);

        fprintf(fp, "%s", dp->d_name);
        for (double v : c) fprintf(fp, ",%.6f", v);
        for (double v : o) fprintf(fp, ",%.6f", v);
        fprintf(fp, "\n");
    }

    fclose(fp);
    closedir(dirp);
    return 0;
}
