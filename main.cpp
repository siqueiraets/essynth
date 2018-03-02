#include <QApplication>
#include "ESMainWindowUI.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    ESMainWindowUI w;

    w.show();
    return a.exec();
}
