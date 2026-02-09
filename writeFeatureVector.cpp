#include <opencv2/opencv.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <vector>
#include <string>
#include <fstream>

// Extract 7x7 center patch (BGR, row-major)
void extractCenter7x7(const cv::Mat &img, std::vector<float> &feat) {
    feat.clear();

    int cx = img.cols / 2;
    int cy = img.rows / 2;

    for (int y = cy - 3; y <= cy + 3; y++) {
        for (int x = cx - 3; x <= cx + 3; x++) {
            cv::Vec3b p = img.at<cv::Vec3b>(y, x);
            feat.push_back((float)p[0]);
            feat.push_back((float)p[1]);
            feat.push_back((float)p[2]);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("usage: %s <image_directory> <output_csv>\n", argv[0]);
        return -1;
    }

    char dirname[256];
    strcpy(dirname, argv[1]);

    DIR *dirp = opendir(dirname);
    if (!dirp) {
        printf("Cannot open directory %s\n", dirname);
        return -1;
    }

    std::ofstream out(argv[2]);
    if (!out.is_open()) {
        printf("Cannot open output file\n");
        return -1;
    }

    struct dirent *dp;
    char buffer[512];

    while ((dp = readdir(dirp)) != NULL) {
        if (strstr(dp->d_name, ".jpg") ||
            strstr(dp->d_name, ".png"_
