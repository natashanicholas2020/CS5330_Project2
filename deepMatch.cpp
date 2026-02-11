#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

using namespace std;

const int FEAT_DIM = 512;

struct ImageFeat {
    string name;
    vector<double> feat;
};

struct Match {
    string name;
    double dist;
};

/* -------- L2 normalize -------- */
void normalize(vector<double> &v) {
    double norm = 0.0;
    for (double x : v)
        norm += x * x;
    norm = sqrt(norm);

    if (norm > 0) {
        for (double &x : v)
            x /= norm;
    }
}

/* -------- Cosine distance -------- */
double cosineDistance(const vector<double> &a, const vector<double> &b) {
    double dot = 0.0;
    for (int i = 0; i < FEAT_DIM; i++)
        dot += a[i] * b[i];
    return 1.0 - dot;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("usage: %s <target_filename> <embedding_csv> <N>\n", argv[0]);
        return -1;
    }

    string targetName = argv[1];
    char *csvName = argv[2];
    int N = atoi(argv[3]);

    FILE *fp = fopen(csvName, "r");
    if (!fp) {
        printf("Cannot open embedding file\n");
        return -1;
    }

    vector<ImageFeat> database;
    ImageFeat target;
    bool foundTarget = false;

    char line[16384];

    while (fgets(line, sizeof(line), fp)) {
        char *tok = strtok(line, ",");
        string fname = tok;

        vector<double> feat;
        for (int i = 0; i < FEAT_DIM; i++) {
            tok = strtok(NULL, ",");
            feat.push_back(atof(tok));
        }

        normalize(feat);

        if (fname == targetName) {
            target.name = fname;
            target.feat = feat;
            foundTarget = true;
        }

        database.push_back({fname, feat});
    }

    fclose(fp);

    if (!foundTarget) {
        printf("Target image not found in embedding file\n");
        return -1;
    }

    vector<Match> matches;

    for (const auto &img : database) {
        double d = cosineDistance(target.feat, img.feat);
        matches.push_back({img.name, d});
    }

    sort(matches.begin(), matches.end(),
         [](const Match &a, const Match &b) {
             return a.dist < b.dist;
         });

    for (int i = 0; i < N && i < matches.size(); i++) {
        printf("%s %.6f\n", matches[i].name.c_str(), matches[i].dist);
    }

    return 0;
}
