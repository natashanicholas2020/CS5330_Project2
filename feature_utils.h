//Name: Natasha Nicholas
//Date: Feb. 11, 2026
//File: feature_utils.h
//
//Header file for feature_utils.h

#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

// Types of feature extractors supported.
enum FeatureType {
    BASELINE,
    COLOR,
    MULTIHIST,
    COLOR_TEXTURE,
    CUSTOM,
    DNN_EMB
};

//Stores a feature vector for one image.
struct ImageFeature {
    string name;
    FeatureType type;
    vector<int> intFeat;  
    vector<double> dblFeat; 
};

//Compute features for an image.
ImageFeature computeFeatures(const Mat &img, FeatureType type, const string &name);

//Extract features for all images in a directory.
vector<ImageFeature> extractDirFeatures(const string &dir, FeatureType type);

//Write image features to a CSV file.
void writeFeatureCSV(const string &filename, const vector<ImageFeature> &features);

//Read image features from a CSV file.
vector<ImageFeature> readFeatureCSV(const string &filename, FeatureType type);

//Read DNN embedding CSV file.
vector<ImageFeature> readDNNCSV(const string &filename);
