#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <iostream>

using namespace std;

const int H_BINS = 16;
const int S_BINS = 8;
const int ORI_BINS = 16;
const int COLOR_SIZE = H_BINS * S_BINS;

struct Feature {
    string name;
    vector<double> color;
    vector<double> ori;
};

double histIntersection(const vector<double> &a, const vector<double> &b) {
    double sum = 0;
    for (int i = 0; i < a.size(); i++) sum += min(a[i], b[i]);
    return 1.0 - sum;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("usage: %s <target> <csv> <N>\n", argv[0]);
        return -1;
    }

    string targetName = argv[1];
    char *csvName = argv[2];
    int N = atoi(argv[3]);

    FILE *fp = fopen(csvName, "r");
    if (!fp) return -1;

    Feature target;
    vector<Feature> db;

    char line[8192];
    while (fgets(line, sizeof(line), fp)) {
        char *tok = strtok(line, ",");
        string name = tok;

        vector<double> color;
        for (int i = 0; i < COLOR_SIZE; i++) {
            tok = strtok(NULL, ",");
            color.push_back(atof(tok));
        }

        vector<double> ori;
        for (int i = 0; i < ORI_BINS; i++) {
            tok = strtok(NULL, ",");
            ori.push_back(atof(tok));
        }

        Feature f{ name, color, ori };
        if (name == targetName) target = f;
        db.push_back(f);
    }
    fclose(fp);

    struct Match { string name; double dist; };
    vector<Match> matches;

    for (auto &img : db) {
        double dColor = histIntersection(target.color, img.color);
        double dOri   = histIntersection(target.ori, img.ori);

        double d = 0.5 * dColor + 0.5 * dOri;  // no DNN used here (add DNN if you want)
        matches.push_back({ img.name, d });
    }

    sort(matches.begin(), matches.end(),
         [](const Match &a, const Match &b){ return a.dist < b.dist; });

    for (int i = 0; i < N; i++)
        printf("%s %.6f\n", matches[i].name.c_str(), matches[i].dist);

    return 0;
}
