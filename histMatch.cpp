#include <opencv2/opencv.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

struct Match {
    string filename;
    double distance;
};

const int H_BINS = 16;
const int S_BINS = 8;

vector<double> computeHist(const Mat &img) {
    Mat hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);

    vector<double> hist(H_BINS * S_BINS, 0);

    for (int y = 0; y < hsv.rows; y++) {
        for (int x = 0; x < hsv.cols; x++) {
            Vec3b pixel = hsv.at<Vec3b>(y, x);
            int h = pixel[0];
            int s = pixel[1];

            int hBin = (h * H_BINS) / 180;
            int sBin = (s * S_BINS) / 256;

            if (hBin >= H_BINS) hBin = H_BINS - 1;
            if (sBin >= S_BINS) sBin = S_BINS - 1;

            hist[hBin * S_BINS + sBin]++;
        }
    }

    double total = img.rows * img.cols;
    for (int i = 0; i < hist.size(); i++) {
        hist[i] /= total;
    }
    return hist;
}

double histogramIntersection(const vector<double> &a, const vector<double> &b) {
    double intersection = 0;
    for (int i = 0; i < a.size(); i++) {
        intersection += min(a[i], b[i]);
    }
    return 1.0 - intersection;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("usage: %s <target_image> <feature_csv> <N>\n", argv[0]);
        return -1;
    }

    char *targetName = argv[1];
    char *csvName = argv[2];
    int N = atoi(argv[3]);

    Mat target = imread(targetName);
    if (target.empty()) {
        printf("Cannot read target image\n");
        return -1;
    }

    vector<double> targetHist = computeHist(target);

    FILE *fp = fopen(csvName, "r");
    if (!fp) {
        printf("Cannot open feature file\n");
        return -1;
    }

    vector<Match> matches;
    char line[4096];

    while (fgets(line, sizeof(line), fp)) {
        char *token = strtok(line, ",");
        string filename = token;

        vector<double> hist;
        while ((token = strtok(NULL, ",")) != NULL) {
            hist.push_back(atof(token));
        }

        if (hist.size() != H_BINS * S_BINS)
            continue;

        double dist = histogramIntersection(targetHist, hist);
        matches.push_back({filename, dist});
    }

    fclose(fp);

    sort(matches.begin(), matches.end(), [](const Match &a, const Match &b) {
        return a.distance < b.distance;
    });

    for (int i = 0; i < N && i < matches.size(); i++) {
        printf("%s %.6f\n", matches[i].filename.c_str(), matches[i].distance);
    }

    return 0;
}
