#include "MainWindow.hpp"
#include "SPSDialog.hpp"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
#ifndef QT_DEBUG
    SPSDialog dialog;
    int code = dialog.exec();
    if (!code)
        exit(1);
    QString r = dialog.r_sps_file_name();
    QString s = dialog.s_sps_file_name();
    QString x = dialog.x_sps_file_name();
    bool rev = dialog.sps_revision_2();
#else
    QString r = "/home/andalevor/Documents/KGF/SPS 220620/BLOCK1r220620.rps";
    QString s = "/home/andalevor/Documents/KGF/SPS 220620/BLOCK1s220620.sps";
    QString x = "/home/andalevor/Documents/KGF/SPS 220620/BLOCK1relat220620.xps";
    bool rev = 0;
#endif
    MainWindow w(r, s, x, rev);
    w.show();
    return app.exec();
}
