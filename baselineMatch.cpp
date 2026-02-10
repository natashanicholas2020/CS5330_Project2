// ./baselineMatch <target_image> <feature_csv> <N>

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

double computeSSD(const vector<int> &a, const vector<int> &b) {
    double ssd = 0.0;
    for (int i = 0; i < 49; i++) {
        double diff = a[i] - b[i];
        ssd += diff * diff;
    }
    return ssd;
}

vector<int> extractCenter7x7(const Mat &img) {
    vector<int> feature;
    int cx = img.cols / 2;
    int cy = img.rows / 2;

    for (int y = -3; y <= 3; y++) {
        for (int x = -3; x <= 3; x++) {
            feature.push_back(img.at<uchar>(cy + y, cx + x));
        }
    }
    return feature;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("usage: %s <target_image> <feature_csv> <N>\n", argv[0]);
        return -1;
    }

    char *targetName = argv[1];
    char *csvName = argv[2];
    int N = atoi(argv[3]);

    Mat target = imread(targetName, IMREAD_GRAYSCALE);
    if (target.empty()) {
        printf("Cannot read target image\n");
        return -1;
    }

    vector<int> targetFeature = extractCenter7x7(target);

    FILE *fp = fopen(csvName, "r");
    if (!fp) {
        printf("Cannot open feature file\n");
        return -1;
    }

    vector<Match> matches;
    char line[2048];

    while (fgets(line, sizeof(line), fp)) {
        char *token = strtok(line, ",");
        string filename = token;

        vector<int> feature;
        while ((token = strtok(NULL, ",")) != NULL) {
            feature.push_back(atoi(token));
        }

        if (feature.size() != 49)
            continue;

        double dist = computeSSD(targetFeature, feature);
        matches.push_back({filename, dist});
    }

    fclose(fp);

    sort(matches.begin(), matches.end(),
         [](const Match &a, const Match &b) {
             return a.distance < b.distance;
         });

    for (int i = 0; i < N && i < matches.size(); i++) {
        printf("%s %.2f\n", matches[i].filename.c_str(), matches[i].distance);
    }

    return 0;
}

