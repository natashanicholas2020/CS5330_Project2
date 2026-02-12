//Name: Natasha Nicholas
//Date: Feb. 11, 2026
//File: Project2Window.h
//
// Header file for Project2Window.cpp

#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QListWidget>
#include <QLineEdit>
#include <QTabWidget>
#include <opencv2/opencv.hpp>
#include "feature_utils.h"
#include "matcher_utils.h"

class Project2Window : public QMainWindow {
    Q_OBJECT

public:
    Project2Window(QWidget *parent = nullptr);

private slots:
    void onLoadImages();
    void onExtractFeatures();
    void onRunMatch();

private:
    QTabWidget *tabs;
    QWidget *extractTab;
    QWidget *matchTab;

    QPushButton *btnLoadImages;
    QPushButton *btnExtract;
    QLineEdit *editDir;
    QLineEdit *editCSV;

    QPushButton *btnMatch;
    QLineEdit *editTarget;
    QLineEdit *editCSVMatch;
    QLineEdit *editN;
    QListWidget *listResults;

    std::string imageDir;
};
