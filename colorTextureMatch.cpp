#include <opencv2/opencv.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

/* -------------------- Parameters -------------------- */
const int H_BINS = 16;
const int S_BINS = 8;
const int TEX_BINS = 16;
const int COLOR_SIZE = H_BINS * S_BINS;

/* -------------------- Color Histogram -------------------- */
vector<double> colorHist(const Mat &img) {
    Mat hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);

    vector<double> hist(COLOR_SIZE, 0);

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
    for (double &v : hist)
        v /= total;

    return hist;
}

/* -------------------- Texture Histogram -------------------- */
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
    for (double &v : hist)
        v /= total;

    return hist;
}

/* -------------------- Histogram Intersection -------------------- */
double histIntersection(const vector<double> &a, const vector<double> &b) {
    double sum = 0;
    for (int i = 0; i < a.size(); i++)
        sum += min(a[i], b[i]);
    return 1.0 - sum;
}

/* -------------------- Main -------------------- */
int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("usage: %s <target_image> <feature_csv> <N>\n", argv[0]);
        return -1;
    }

    Mat target = imread(argv[1]);
    if (target.empty()) {
        printf("Cannot read target image\n");
        return -1;
    }

    vector<double> targetColor = colorHist(target);
    vector<double> targetTexture = textureHist(target);

    FILE *fp = fopen(argv[2], "r");
    if (!fp) {
        printf("Cannot open feature file\n");
        return -1;
    }

    struct Match {
        string name;
        double dist;
    };

    vector<Match> matches;
    char line[8192];

    while (fgets(line, sizeof(line), fp)) {
        char *tok = strtok(line, ",");
        string filename = tok;

        vector<double> colorHistDB;
        vector<double> textureHistDB;

        for (int i = 0; i < COLOR_SIZE; i++) {
            tok = strtok(NULL, ",");
            colorHistDB.push_back(atof(tok));
        }

        for (int i = 0; i < TEX_BINS; i++) {
            tok = strtok(NULL, ",");
            textureHistDB.push_back(atof(tok));
        }

        double dColor = histIntersection(targetColor, colorHistDB);
        double dTexture = histIntersection(targetTexture, textureHistDB);

        double finalDist = 0.5 * dColor + 0.5 * dTexture;
        matches.push_back({filename, finalDist});
    }

    fclose(fp);

    sort(matches.begin(), matches.end(),
         [](const Match &a, const Match &b) {
             return a.dist < b.dist;
         });

    int N = atoi(argv[3]);
    for (int i = 0; i < N && i < matches.size(); i++) {
        printf("%s %.6f\n", matches[i].name.c_str(), matches[i].dist);
    }

    return 0;
}
