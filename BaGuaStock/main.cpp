#include "BaGuaStock.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BaGuaStock w;
    w.showMaximized();
    return a.exec();
}
