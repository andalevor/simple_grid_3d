#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QString rec_name, QString src_name, QString x_name, bool rev);
    ~MainWindow();
    QGraphicsScene* scene;
    double min_x, min_y, max_x, max_y, x_cell_size, y_cell_size, azimuth, x_origin, y_origin;
    int x_cells_number, y_cells_number;

public slots:
    void x_cell_size_changed(double);
    void y_cell_size_changed(double);
    void x_origin_changed(double);
    void y_origin_changed(double);
    void x_cells_number_changed(int);
    void y_cells_number_changed(int);
    void azimuth_changed(double);
    void draw_grid();
    void redraw_grid();
    void show_grid(bool);
    void save_grid();

private:
    void populate_scene();
    Ui::MainWindow* ui;
    QImage grid;
};
#endif // MAINWINDOW_HPP
