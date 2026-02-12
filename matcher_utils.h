//Name: Natasha Nicholas
//Date: Feb. 11, 2026
//File: matcher_utils.h
//
// Header file for matcher_utils.cpp

#pragma once
#include <string>
#include <vector>

struct Match {
    std::string name;
    double dist;
};

#include "feature_utils.h"

//Match a target feature against a database of features.
std::vector<Match> matchFeatures(const ImageFeature &target,
                                 const std::vector<ImageFeature> &db,
                                 FeatureType type,
                                 int N);
