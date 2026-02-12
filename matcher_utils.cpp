#include "matcher_utils.h"
#include <algorithm>
#include <cmath>

using namespace std;

double computeSSD(const vector<int> &a, const vector<int> &b) {
    double ssd = 0.0;
    for (int i = 0; i < a.size(); i++) {
        double diff = a[i] - b[i];
        ssd += diff * diff;
    }
    return ssd;
}

double histIntersection(const vector<double> &a, const vector<double> &b) {
    double sum = 0;
    for (int i = 0; i < a.size(); i++) {
        sum += min(a[i], b[i]);
    }
    return 1.0 - sum;
}

double cosineDistance(const vector<double> &a, const vector<double> &b) {
    double dot = 0, na = 0, nb = 0;
    for (int i = 0; i < a.size(); i++) {
        dot += a[i] * b[i];
        na += a[i] * a[i];
        nb += b[i] * b[i];
    }
    na = sqrt(na);
    nb = sqrt(nb);
    if (na == 0 || nb == 0) return 1.0;
    return 1.0 - dot / (na * nb);
}

vector<Match> matchFeatures(const ImageFeature &target,
                           const vector<ImageFeature> &db,
                           FeatureType type,
                           int N) {

    vector<Match> matches;

    for (auto &f : db) {
        double dist = 1e9;

        if (type == BASELINE) {
            dist = computeSSD(target.intFeat, f.intFeat);
        }
        else if (type == DNN_EMB) {
            dist = cosineDistance(target.dblFeat, f.dblFeat);
        }
        else {
            dist = histIntersection(target.dblFeat, f.dblFeat);
        }

        matches.push_back({f.name, dist});
    }

    sort(matches.begin(), matches.end(),
         [](const Match &a, const Match &b) {
             return a.dist < b.dist;
         });

    if (N > matches.size()) N = matches.size();
    matches.resize(N);

    return matches;
}
