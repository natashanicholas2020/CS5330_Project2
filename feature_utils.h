#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

enum FeatureType {
    BASELINE,
    COLOR,
    MULTIHIST,
    COLOR_TEXTURE,
    CUSTOM,
    DNN_EMB
};

struct ImageFeature {
    string name;
    FeatureType type;
    vector<int> intFeat;  
    vector<double> dblFeat; 
};

ImageFeature computeFeatures(const Mat &img, FeatureType type, const string &name);

vector<ImageFeature> extractDirFeatures(const string &dir, FeatureType type);

void writeFeatureCSV(const string &filename, const vector<ImageFeature> &features);

vector<ImageFeature> readFeatureCSV(const string &filename, FeatureType type);

vector<ImageFeature> readDNNCSV(const string &filename);
