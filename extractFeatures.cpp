//./extract_features <image_directory> <output_csv>


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

    struct dirent *dp;
    char filename[512];

    while ((dp = readdir(dirp)) != NULL) {
        if (!isImageFile(dp->d_name))
            continue;

        sprintf(filename, "%s/%s", dirname, dp->d_name);
        Mat img = imread(filename, IMREAD_GRAYSCALE);
        if (img.empty())
            continue;

        int cx = img.cols / 2;
        int cy = img.rows / 2;

        if (cx < 3 || cy < 3)
            continue;

        fprintf(fp, "%s", dp->d_name);

        for (int y = -3; y <= 3; y++) {
            for (int x = -3; x <= 3; x++) {
                int val = img.at<uchar>(cy + y, cx + x);
                fprintf(fp, ",%d", val);
            }
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
    closedir(dirp);
    return 0;
}
