#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>

// Extract 7x7 center patch
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

// Manual SSD (no OpenCV)
float ssd(const std::vector<float> &a,
          const std::vector<float> &b) {
    float sum = 0.0f;
    for (size_t i = 0; i < a.size(); i++) {
        float d = a[i] - b[i];
        sum += d * d;
    }
    return sum;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("usage: %s <target_image> <feature_csv> <N>\n", argv[0]);
        return -1;
    }

    cv::Mat target = cv::imread(argv[1]);
    if (target.empty()) {
        printf("Could not read target image\n");
        return -1;
    }

    if (target.rows < 7 || target.cols < 7) {
        printf("Target image too small\n");
        return -1;
    }

    std::vector<float> targetFeat;
    extractCenter7x7(target, targetFeat);

    std::ifstream in(argv[2]);
    if (!in.is_open()) {
        printf("Could not open feature file\n");
        return -1;
    }

    std::vector<std::pair<float, std::string>> results;
    std::string line;

    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string name, val;

        std::getline(ss, name, ',');

        std::vector<float> feat;
        while (std::getline(ss, val, ',')) {
            feat.push_back(std::stof(val));
        }

        float dist = ssd(targetFeat, feat);
        results.push_back({dist, name});
    }

    std::sort(results.begin(), results.end());

    int N = atoi(argv[3]);
    for (int i = 0; i < N && i < (int)results.size(); i++) {
        printf("%s %f\n",
               results[i].second.c_str(),
               results[i].first);
    }

    return 0;
}
