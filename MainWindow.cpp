#include "MainWindow.hpp"
#include "GraphicsView.hpp"
#include "View.hpp"
#include "ui_MainWindow.h"
#include <QFile>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QHash>
#include <QMessageBox>
#include <QPainter>
#include <QPair>
#include <QPointF>
#include <QTextStream>
#include <cmath>

#include <QDebug>

using std::move;

static QHash<QPair<int64_t, int64_t>, QPointF> parse_r_sps(QString file_name, bool rev);
static QHash<QPair<int64_t, int64_t>, QPointF> parse_s_sps(QString file_name, bool rev);

MainWindow::MainWindow(QString rec_name, QString src_name, QString x_name, bool rev)
    : QMainWindow(nullptr)
    , scene(new QGraphicsScene(this))
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QHash<QPair<int64_t, int64_t>, QPointF> rec = parse_r_sps(rec_name, rev);
    bool first = true;
    for (auto& v : rec) {
        if (first) {
            first = false;
            min_x = max_x = v.x();
            min_y = max_y = v.y();
        }
        if (min_x > v.x())
            min_x = v.x();
        if (min_y > v.y())
            min_y = v.y();
        if (max_x < v.x())
            max_x = v.x();
        if (max_y < v.y())
            max_y = v.y();
    }
    QHash<QPair<int64_t, int64_t>, QPointF> src = parse_s_sps(src_name, rev);
    for (auto& v : src) {
        if (min_x > v.x())
            min_x = v.x();
        if (min_y > v.y())
            min_y = v.y();
        if (max_x < v.x())
            max_x = v.x();
        if (max_y < v.y())
            max_y = v.y();
    }
    x_origin = min_x;
    y_origin = min_y;
    QFile x_sps_file(x_name);
    if (!x_sps_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "Error", "Can not read X SPS file");
        QApplication::quit();
    }
    QImage result(max_x - min_x, max_y - min_y, QImage::Format_ARGB32);
    QPainter painter(&result);
    QPen pen;
    pen.setWidth(5);
    pen.setColor(QColor(Qt::GlobalColor::white));
    painter.setPen(pen);
    result.fill(0xffbbbbbb);
    QTextStream in(&x_sps_file);
    while (!in.atEnd()) {
        QString str = in.readLine();
        if (str.startsWith('X')) {
            int64_t s_line, s_number, r_line, r_number_from, r_number_to;
            bool ok = true;
            if (rev) {
                s_line = str.mid(17, 10).toLongLong(&ok);
                if (!ok) {
                    QMessageBox::critical(nullptr, "Error", "Can not read source line number (18, 27) from X SPS file: " + str);
                    exit(1);
                }
                s_number = str.mid(27, 10).toLongLong(&ok);
                if (!ok) {
                    QMessageBox::critical(nullptr, "Error", "Can not read source point number (28, 37) from X SPS file: " + str);
                    exit(1);
                }
                r_line = str.mid(49, 10).toLongLong(&ok);
                if (!ok) {
                    QMessageBox::critical(nullptr, "Error", "Can not read receiver line number (50, 59) from X SPS file: " + str);
                    exit(1);
                }
                r_number_from = str.mid(59, 10).toLongLong(&ok);
                if (!ok) {
                    QMessageBox::critical(nullptr, "Error", "Can not read receiver point number (60, 69) from X SPS file: " + str);
                    exit(1);
                }
                r_number_to = str.mid(69, 10).toLongLong(&ok);
                if (!ok) {
                    QMessageBox::critical(nullptr, "Error", "Can not read receiver point number (70, 79) from X SPS file: " + str);
                    exit(1);
                }
            } else {
                s_line = str.mid(13, 16).toLongLong(&ok);
                if (!ok) {
                    QMessageBox::critical(nullptr, "Error", "Can not read source line number (14, 29) from X SPS file: " + str);
                    exit(1);
                }
                s_number = str.mid(29, 8).toLongLong(&ok);
                if (!ok) {
                    QMessageBox::critical(nullptr, "Error", "Can not read source point number (30, 37) from X SPS file: " + str);
                    exit(1);
                }
                r_line = str.mid(47, 16).toLongLong(&ok);
                if (!ok) {
                    QMessageBox::critical(nullptr, "Error", "Can not read receiver line number (48, 63) from X SPS file: " + str);
                    exit(1);
                }
                r_number_from = str.mid(63, 8).toLongLong(&ok);
                if (!ok) {
                    QMessageBox::critical(nullptr, "Error", "Can not read receiver point number (64, 71) from X SPS file: " + str);
                    exit(1);
                }
                r_number_to = str.mid(71, 8).toLongLong(&ok);
                if (!ok) {
                    QMessageBox::critical(nullptr, "Error", "Can not read receiver point number (72, 79) from X SPS file: " + str);
                    exit(1);
                }
            }
            QHash<QPair<int64_t, int64_t>, QPointF>::iterator sit = src.find(QPair<int64_t, int64_t>(s_line, s_number));
            if (sit == src.end()) {
                QMessageBox::critical(nullptr, "Error", "There is source in X SPS file which missing in S SPS file: " + QString::number(s_line) + QString::number(s_number));
                exit(1);
            }
            QPointF src = *sit;
            for (int64_t i = r_number_from; i <= r_number_to; ++i) {
                QHash<QPair<int64_t, int64_t>, QPointF>::iterator rit = rec.find(QPair<int64_t, int64_t>(r_line, i));
                if (rit == rec.end()) {
                    QMessageBox::critical(nullptr, "Error", "There is receiver in X SPS file which missing in R SPS file: " + QString::number(r_line) + QString::number(i));
                    exit(1);
                }
                QPointF rec = *rit;
                //result.setPixel((rec.x() + src.x()) / 2 - min_x, max_y - (rec.y() + src.y()) / 2, 0xffffffff);
                painter.drawPoint((rec.x() + src.x()) / 2 - min_x, max_y - (rec.y() + src.y()) / 2);
            }
        }
    }
    scene->addPixmap(QPixmap::fromImage(result));
    View* view = new View(min_x, min_y, this);
    draw_grid();
    setCentralWidget(view);
}

void MainWindow::draw_grid()
{
    QImage grid(max_x - min_x, max_y - min_y, QImage::Format_ARGB32);
    grid.fill(Qt::transparent);
    QPainter painter(&grid);
    QPen pen;
    pen.setWidth(5);
    pen.setColor(QColor(Qt::GlobalColor::red));
    painter.setPen(pen);
    for (int i = 0; i < x_cells_number + 1; ++i) {
        int y = y_cells_number * y_cell_size;
        int x1 = x_origin - min_x + i * x_cell_size * cos(azimuth * M_PI / 180);
        int x2 = x1 + sin(azimuth * M_PI / 180) * y;
        int y1 = grid.height() + i * x_cell_size * sin(azimuth * M_PI / 180) - y_origin + min_y;
        int y2 = y1 - cos(azimuth * M_PI / 180) * y;
        painter.drawLine(x1, y1, x2, y2);
    }
    for (int i = 0; i < y_cells_number + 1; ++i) {
        int x = x_cells_number * x_cell_size;
        int x1 = x_origin - min_x + i * y_cell_size * sin(azimuth * M_PI / 180);
        int x2 = x1 + cos(azimuth * M_PI / 180) * x;
        int y1 = grid.height() - i * y_cell_size * cos(azimuth * M_PI / 180) - y_origin + min_y;
        int y2 = y1 + sin(azimuth * M_PI / 180) * x;
        painter.drawLine(x1, y1, x2, y2);
    }
    this->grid = std::move(grid);
}

void MainWindow::redraw_grid()
{
    QGraphicsItem* ptr = scene->items()[0];
    scene->removeItem(ptr);
    delete ptr;
    draw_grid();
    scene->addPixmap(QPixmap::fromImage(grid));
}

void MainWindow::show_grid(bool flag)
{
    if (flag) {
        scene->addPixmap(QPixmap::fromImage(grid));
    } else {
        QGraphicsItem* ptr = scene->items()[0];
        scene->removeItem(ptr);
        delete ptr;
    }
}

void MainWindow::x_cell_size_changed(double value)
{
    x_cell_size = value;
    redraw_grid();
}

void MainWindow::y_cell_size_changed(double value)
{
    y_cell_size = value;
    redraw_grid();
}

void MainWindow::x_cells_number_changed(int value)
{
    x_cells_number = value;
    redraw_grid();
}

void MainWindow::y_cells_number_changed(int value)
{
    y_cells_number = value;
    redraw_grid();
}

void MainWindow::azimuth_changed(double value)
{
    azimuth = value;
    redraw_grid();
}

void MainWindow::x_origin_changed(double value)
{
    x_origin = value;
    redraw_grid();
}

void MainWindow::y_origin_changed(double value)
{
    y_origin = value;
    redraw_grid();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QHash<QPair<int64_t, int64_t>, QPointF> parse_r_sps(QString file_name, bool rev)
{
    QFile r_sps_file(file_name);
    if (!r_sps_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "Error", "Can not read R SPS file");
        QApplication::quit();
    }
    QHash<QPair<int64_t, int64_t>, QPointF> rec;
    QTextStream in(&r_sps_file);
    while (!in.atEnd()) {
        QString str = in.readLine();
        if (str.startsWith('R')) {
            int64_t line, number;
            bool ok = true;
            if (rev) {
                line = str.mid(1, 10).toLongLong();
                if (!ok) {
                    QMessageBox::critical(nullptr, "Error", "Can not read line number (2, 11) from R SPS file: " + str);
                    exit(1);
                }
                number = str.mid(11, 10).toLongLong();
                if (!ok) {
                    QMessageBox::critical(nullptr, "Error", "Can not read point number (12, 21) from R SPS file: " + str);
                    exit(1);
                }
            } else {
                line = str.mid(1, 16).toLongLong();
                if (!ok) {
                    QMessageBox::critical(nullptr, "Error", "Can not read line number (2, 17) from R SPS file: " + str);
                    exit(1);
                }
                number = str.mid(17, 8).toLongLong();
                if (!ok) {
                    QMessageBox::critical(nullptr, "Error", "Can not read point number (18, 25) from R SPS file: " + str);
                    exit(1);
                }
            }
            double x = str.mid(46, 9).toDouble(&ok);
            if (!ok) {
                QMessageBox::critical(nullptr, "Error", "Can not read X coordinate (47, 55) from R SPS file: " + str);
                exit(1);
            }
            double y = str.mid(55, 10).toDouble(&ok);
            if (!ok) {
                QMessageBox::critical(nullptr, "Error", "Can not read Y coordinate (56, 65) from R SPS file: " + str);
                exit(1);
            }
            rec[QPair<int64_t, int64_t>(line, number)] = { x, y };
        }
    }
    return rec;
}

QHash<QPair<int64_t, int64_t>, QPointF> parse_s_sps(QString file_name, bool rev)
{
    QFile s_sps_file(file_name);
    if (!s_sps_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "Error", "Can not read S SPS file");
        QApplication::quit();
    }
    QHash<QPair<int64_t, int64_t>, QPointF> src;
    QTextStream in(&s_sps_file);
    while (!in.atEnd()) {
        QString str = in.readLine();
        if (str.startsWith('S')) {
            int64_t line, number;
            bool ok = true;
            if (rev) {
                line = str.mid(1, 10).toLongLong();
                if (!ok) {
                    QMessageBox::critical(nullptr, "Error", "Can not read line number (2, 11) from S SPS file: " + str);
                    exit(1);
                }
                number = str.mid(11, 10).toLongLong();
                if (!ok) {
                    QMessageBox::critical(nullptr, "Error", "Can not read point number (12, 21) from S SPS file: " + str);
                    exit(1);
                }
            } else {
                line = str.mid(1, 16).toLongLong();
                if (!ok) {
                    QMessageBox::critical(nullptr, "Error", "Can not read line number (2, 17) from S SPS file: " + str);
                    exit(1);
                }
                number = str.mid(17, 8).toLongLong();
                if (!ok) {
                    QMessageBox::critical(nullptr, "Error", "Can not read point number (18, 25) from S SPS file: " + str);
                    exit(1);
                }
            }
            double x = str.mid(46, 9).toDouble(&ok);
            if (!ok) {
                QMessageBox::critical(nullptr, "Error", "Can not read X coordinate (47, 55) from S SPS file: " + str);
                exit(1);
            }
            double y = str.mid(55, 10).toDouble(&ok);
            if (!ok) {
                QMessageBox::critical(nullptr, "Error", "Can not read Y coordinate (56, 65) from S SPS file: " + str);
                exit(1);
            }
            src[QPair<int64_t, int64_t>(line, number)] = { x, y };
        }
    }
    return src;
}

void MainWindow::save_grid()
{
    QString save_file_name = QFileDialog::getSaveFileName(this, tr("Choose file to save grid"));
    QFile save_file(save_file_name);
    if (!save_file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "Error", "Can not write to " + save_file_name);
        return;
    }
    QTextStream out(&save_file);
    out << "[grid]\n";
    out << Qt::fixed << "x_orig=" << x_origin << '\n';
    out << Qt::fixed << "y_orig=" << y_origin << '\n';
    out << Qt::fixed << "azimuth=" << azimuth << '\n';
    out << Qt::fixed << "x_cell_size=" << x_cell_size << '\n';
    out << Qt::fixed << "y_cell_size=" << y_cell_size << '\n';
    out << "x_cells_number=" << x_cells_number << '\n';
    out << "y_cells_number=" << y_cells_number << '\n';
}
