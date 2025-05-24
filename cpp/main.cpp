#include "FaceSlider.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    FaceSlider w;
    w.show();
    return a.exec();
}
