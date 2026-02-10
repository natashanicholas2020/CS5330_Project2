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

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("usage: %s <image_directory> <output_csv>\n", argv[0]);
        return -1;
    }

    char *dirname = argv[1];
    char *csvname = argv[2];

    DIR *dirp = opendir(dirname);
    if (!dirp) {
        printf("Cannot open directory %s\n", dirname);
        return -1;
    }

    FILE *fp = fopen(csvname, "w");
    if (!fp) {
        printf("Cannot open output file %s\n", csvname);
        return -1;
    }

    const int H_BINS = 16;
    const int S_BINS = 8;

    struct dirent *dp;
    char filename[512];

    while ((dp = readdir(dirp)) != NULL) {
        if (!isImageFile(dp->d_name))
            continue;

        sprintf(filename, "%s/%s", dirname, dp->d_name);
        Mat img = imread(filename);
        if (img.empty())
            continue;

        Mat hsv;
        cvtColor(img, hsv, COLOR_BGR2HSV);

        // histogram counts
        vector<int> hist(H_BINS * S_BINS, 0);

        for (int y = 0; y < hsv.rows; y++) {
            for (int x = 0; x < hsv.cols; x++) {
                Vec3b pixel = hsv.at<Vec3b>(y, x);
                int h = pixel[0]; // 0..179
                int s = pixel[1]; // 0..255

                int hBin = (h * H_BINS) / 180;
                int sBin = (s * S_BINS) / 256;

                if (hBin >= H_BINS) hBin = H_BINS - 1;
                if (sBin >= S_BINS) sBin = S_BINS - 1;

                hist[hBin * S_BINS + sBin]++;
            }
        }

        // Normalize histogram
        double total = img.rows * img.cols;
        fprintf(fp, "%s", dp->d_name);
        for (int i = 0; i < hist.size(); i++) {
            double val = hist[i] / total;
            fprintf(fp, ",%.6f", val);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
    closedir(dirp);
    return 0;
}
