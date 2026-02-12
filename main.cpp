//Name: Natasha Nicholas
//Date: Feb. 11, 2026
//File: main.cpp
//
//This program initializes the QApplication and launches the main window for Project 2

#include <QApplication>
#include "Project2Window.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Project2Window w;
    w.show();
    return app.exec();
}
