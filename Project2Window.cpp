#include "Project2Window.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <opencv2/opencv.hpp>

Project2Window::Project2Window(QWidget *parent) : QMainWindow(parent) {

    tabs = new QTabWidget(this);

    extractTab = new QWidget();
    auto *extLayout = new QVBoxLayout();

    editDir = new QLineEdit();
    btnLoadImages = new QPushButton("Choose Image Directory");
    btnExtract = new QPushButton("Extract Features (All)");

    extLayout->addWidget(btnLoadImages);
    extLayout->addWidget(editDir);
    extLayout->addWidget(btnExtract);
    extractTab->setLayout(extLayout);

    matchTab = new QWidget();
    auto *matLayout = new QVBoxLayout();

    editTarget = new QLineEdit();
    editTarget->setPlaceholderText("Target image filename (e.g., pic.0535.jpg)");

    editCSVMatch = new QLineEdit();
    editCSVMatch->setPlaceholderText("Feature CSV file");

    editN = new QLineEdit();
    editN->setPlaceholderText("N (top matches)");

    btnMatch = new QPushButton("Run Match");

    listResults = new QListWidget();

    matLayout->addWidget(editTarget);
    matLayout->addWidget(editCSVMatch);
    matLayout->addWidget(editN);
    matLayout->addWidget(btnMatch);
    matLayout->addWidget(listResults);
    matchTab->setLayout(matLayout);

    tabs->addTab(extractTab, "Extract");
    tabs->addTab(matchTab, "Match");

    setCentralWidget(tabs);
    setWindowTitle("Project 2 GUI");

    connect(btnLoadImages, &QPushButton::clicked, this, &Project2Window::onLoadImages);
    connect(btnExtract, &QPushButton::clicked, this, &Project2Window::onExtractFeatures);
    connect(btnMatch, &QPushButton::clicked, this, &Project2Window::onRunMatch);
}

void Project2Window::onLoadImages() {
    QString dir = QFileDialog::getExistingDirectory(this, "Choose Image Directory");
    if (!dir.isEmpty()) {
        imageDir = dir.toStdString();
        editDir->setText(dir);
    }
}

void Project2Window::onExtractFeatures() {

    if (imageDir.empty() ) {
        QMessageBox::warning(this, "Error", "Choose directory and output CSV!");
        return;
    }

    auto baseline = extractDirFeatures(imageDir, BASELINE);
    writeFeatureCSV("baseline.csv", baseline);

    auto hist = extractDirFeatures(imageDir, COLOR);
    writeFeatureCSV("hist.csv", hist);

    auto multihist = extractDirFeatures(imageDir, MULTIHIST);
    writeFeatureCSV("multihist.csv", multihist);

    auto ct = extractDirFeatures(imageDir, COLOR_TEXTURE);
    writeFeatureCSV("ct.csv", ct);

    auto custom = extractDirFeatures(imageDir, CUSTOM);
    writeFeatureCSV("custom.csv", custom);

    QMessageBox::information(this, "Done", "Features extracted!");
}

void Project2Window::onRunMatch() {
    std::string target = editTarget->text().trimmed().toStdString();
    std::string csv = editCSVMatch->text().trimmed().toStdString();
    int N = std::stoi(editN->text().trimmed().toStdString());

    listResults->clear();

    FeatureType type = BASELINE;

    if (csv.find("baseline") != std::string::npos) type = BASELINE;
    else if (csv.find("hist") != std::string::npos) type = COLOR;
    else if (csv.find("multihist") != std::string::npos) type = MULTIHIST;
    else if (csv.find("ct") != std::string::npos) type = COLOR_TEXTURE;
    else if (csv.find("custom") != std::string::npos) type = CUSTOM;
    else if (csv.find("dnn") != std::string::npos) type = DNN_EMB;

    vector<ImageFeature> db;
    ImageFeature targetFeat;

    if (type == DNN_EMB) {
        db = readDNNCSV(csv);

        bool found = false;
        for (auto &f : db) {
            if (f.name == target) {
                targetFeat = f;
                found = true;
                break;
            }
        }
        if (!found) {
            QMessageBox::warning(this, "Error", "Target not found in DNN CSV.");
            return;
        }
    }
    else {
        cv::Mat targetImg = cv::imread(imageDir + "/" + target);
        if (targetImg.empty()) {
            QMessageBox::warning(this, "Error", "Target image not found in image folder.");
            return;
        }
        targetFeat = computeFeatures(targetImg, type, target);
        db = readFeatureCSV(csv, type);
    }

    db.erase(std::remove_if(db.begin(), db.end(),
                        [&](const ImageFeature &f) {
                            return f.name == target;
                        }),
         db.end());

    auto matches = matchFeatures(targetFeat, db, type, N);

    for (auto &m : matches) {
        listResults->addItem(QString::fromStdString(m.name + "  dist=" + std::to_string(m.dist)));
    }
}
