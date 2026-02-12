#pragma once
#include <string>
#include <vector>

struct Match {
    std::string name;
    double dist;
};

#include "feature_utils.h"

std::vector<Match> matchFeatures(const ImageFeature &target,
                                 const std::vector<ImageFeature> &db,
                                 FeatureType type,
                                 int N);
