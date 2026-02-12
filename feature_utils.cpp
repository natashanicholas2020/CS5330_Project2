//Name: Natasha Nicholas
//Date: Feb. 11, 2026
//File: feature_utils.cpp
//
//Image feature extraction utilities.
// Includes baseline patch features, color histograms,
// texture histograms, and CSV read/write helpers.

#include "feature_utils.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace fs = std::filesystem;

//Extract a simple baseline feature: pixel values from the center 7x7 patch.
static vector<int> baseline7x7(const Mat &img) {
    int cx = img.cols / 2;
    int cy = img.rows / 2;
    int half = 3;

    vector<int> feat;
    for (int y = cy - half; y <= cy + half; y++) {
        for (int x = cx - half; x <= cx + half; x++) {
            Vec3b p = img.at<Vec3b>(y, x);
            feat.push_back(p[0]);
            feat.push_back(p[1]);
            feat.push_back(p[2]);
        }
    }
    return feat;
}

// Compute normalized 2D rg histogram.
static vector<double> rgHistogram(const Mat &img, int bins = 16) {
    vector<double> hist(bins * bins, 0.0);

    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            Vec3b p = img.at<Vec3b>(y, x);
            double B = p[0], G = p[1], R = p[2];
            double sum = R + G + B;
            if (sum == 0) continue;

            double r = R / sum;
            double g = G / sum;

            int ri = min(bins - 1, int(r * bins));
            int gi = min(bins - 1, int(g * bins));

            hist[ri * bins + gi] += 1.0;
        }
    }

    double total = img.rows * img.cols;
    for (double &v : hist) v /= total;
    return hist;
}

// Compute normalized RGB histogram with 3D bins (R,G,B).
static vector<double> rgbHistogram(const Mat &img, int bins = 8) {
    vector<double> hist(bins * bins * bins, 0.0);

    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            Vec3b p = img.at<Vec3b>(y, x);
            int b = p[0], g = p[1], r = p[2];

            int bi = b * bins / 256;
            int gi = g * bins / 256;
            int ri = r * bins / 256;

            bi = min(bi, bins - 1);
            gi = min(gi, bins - 1);
            ri = min(ri, bins - 1);

            hist[ri * bins * bins + gi * bins + bi] += 1.0;
        }
    }

    double total = img.rows * img.cols;
    for (double &v : hist) v /= total;
    return hist;
}

// Compute histogram of Sobel gradient magnitudes.
static vector<double> sobelMagnitudeHist(const Mat &img, int bins = 16) {
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);

    Mat gx, gy;
    Sobel(gray, gx, CV_32F, 1, 0);
    Sobel(gray, gy, CV_32F, 0, 1);

    Mat mag;
    magnitude(gx, gy, mag);

    double maxVal = 0;
    for (int y = 0; y < mag.rows; y++) {
        for (int x = 0; x < mag.cols; x++) {
            maxVal = std::max(maxVal, (double)mag.at<float>(y, x));
        }
    }
    if (maxVal == 0) maxVal = 1;

    vector<double> hist(bins, 0.0);
    for (int y = 0; y < mag.rows; y++) {
        for (int x = 0; x < mag.cols; x++) {
            float v = mag.at<float>(y, x);
            int bin = min(bins - 1, int(v / maxVal * bins));
            hist[bin] += 1.0;
        }
    }

    double total = mag.rows * mag.cols;
    for (double &v : hist) v /= total;
    return hist;
}

// Extract RGB histograms for top and bottom halves of the image.
static vector<double> multiHistTopBottom(const Mat &img) {
    int mid = img.rows / 2;
    Mat top = img(Range(0, mid), Range::all());
    Mat bot = img(Range(mid, img.rows), Range::all());

    vector<double> h1 = rgbHistogram(top, 8);
    vector<double> h2 = rgbHistogram(bot, 8);

    vector<double> feat;
    feat.insert(feat.end(), h1.begin(), h1.end());
    feat.insert(feat.end(), h2.begin(), h2.end());
    return feat;
}

// Combine color histogram + texture histogram.
static vector<double> colorTextureFeat(const Mat &img) {
    vector<double> c = rgbHistogram(img, 8);
    vector<double> t = sobelMagnitudeHist(img, 16);

    vector<double> feat;
    feat.insert(feat.end(), c.begin(), c.end());
    feat.insert(feat.end(), t.begin(), t.end());
    return feat;
}

// Custom feature wrapper
static vector<double> customFeature(const Mat &img) {
    return colorTextureFeat(img);
}

//Compute features based on requested type.
ImageFeature computeFeatures(const Mat &img, FeatureType type, const string &name) {
    ImageFeature f;
    f.name = name;
    f.type = type;

    if (type == BASELINE) {
        f.intFeat = baseline7x7(img);
    }
    else if (type == COLOR) {
        f.dblFeat = rgHistogram(img, 16);
    }
    else if (type == MULTIHIST) {
        f.dblFeat = multiHistTopBottom(img);
    }
    else if (type == COLOR_TEXTURE) {
        f.dblFeat = colorTextureFeat(img);
    }
    else if (type == CUSTOM) {
        f.dblFeat = customFeature(img);
    }
    return f;
}

// Extract features for all images in a directory.
vector<ImageFeature> extractDirFeatures(const string &dir, FeatureType type) {
    vector<ImageFeature> db;

    for (auto &p : fs::directory_iterator(dir)) {
        if (!p.is_regular_file()) continue;
        string fn = p.path().filename().string();
        string ext = p.path().extension().string();
        if (ext != ".jpg" && ext != ".png") continue;

        Mat img = imread(p.path().string());
        if (img.empty()) continue;

        db.push_back(computeFeatures(img, type, fn));
    }
    return db;
}

// Write feature vectors to CSV.
void writeFeatureCSV(const string &filename, const vector<ImageFeature> &features) {
    ofstream file(filename);
    for (auto &f : features) {
        file << f.name;

        if (f.type == BASELINE) {
            for (int v : f.intFeat) file << "," << v;
        } else {
            for (double v : f.dblFeat) file << "," << v;
        }
        file << "\n";
    }
}

// Read feature CSV into memory.
vector<ImageFeature> readFeatureCSV(const string &filename, FeatureType type) {
    vector<ImageFeature> db;
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;

        ImageFeature f;
        f.type = type;

        stringstream ss(line);
        string token;

        getline(ss, token, ',');
        f.name = token;

        if (type == BASELINE) {
            while (getline(ss, token, ',')) {
                f.intFeat.push_back(stoi(token));
            }
        } else {
            while (getline(ss, token, ',')) {
                f.dblFeat.push_back(stod(token));
            }
        }
        db.push_back(f);
    }
    return db;
}

// Read CSV produced by a DNN embedding extractor.
vector<ImageFeature> readDNNCSV(const string &filename) {
    vector<ImageFeature> db;
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;

        ImageFeature f;
        f.type = DNN_EMB;

        stringstream ss(line);
        string token;

        getline(ss, token, ',');
        f.name = token;

        while (getline(ss, token, ',')) {
            f.dblFeat.push_back(stod(token));
        }
        db.push_back(f);
    }
    return db;
}
