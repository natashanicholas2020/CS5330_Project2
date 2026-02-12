#include <QApplication>
#include "Project2Window.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Project2Window w;
    w.show();
    return app.exec();
}
