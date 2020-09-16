#include "View.hpp"
#include "MainWindow.hpp"
#include "ui_View.h"

View::View(double min_x, double min_y, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::View)
{
    ui->setupUi(this);
    MainWindow* window = dynamic_cast<MainWindow*>(parent);
    ui->graphicsView->setScene(window->scene);
    window->x_cell_size = ui->xCellSizeSpin->value();
    window->y_cell_size = ui->yCellSizeSpin->value();
    window->x_cells_number = ui->xCellsNumberSpin->value();
    window->y_cells_number = ui->yCellsNumberSpin->value();
    window->azimuth = ui->azimuthSpin->value();
    ui->xOriginSpin->setValue(min_x);
    ui->yOriginSpin->setValue(min_y);
    connect(ui->azimuthSpin, SIGNAL(valueChanged(double)), window, SLOT(azimuth_changed(double)));
    connect(ui->xCellSizeSpin, SIGNAL(valueChanged(double)), window, SLOT(x_cell_size_changed(double)));
    connect(ui->yCellSizeSpin, SIGNAL(valueChanged(double)), window, SLOT(y_cell_size_changed(double)));
    connect(ui->xCellsNumberSpin, SIGNAL(valueChanged(int)), window, SLOT(x_cells_number_changed(int)));
    connect(ui->yCellsNumberSpin, SIGNAL(valueChanged(int)), window, SLOT(y_cells_number_changed(int)));
    connect(ui->xOriginSpin, SIGNAL(valueChanged(double)), window, SLOT(x_origin_changed(double)));
    connect(ui->yOriginSpin, SIGNAL(valueChanged(double)), window, SLOT(y_origin_changed(double)));
    connect(ui->inlineParX, SIGNAL(valueChanged(bool)), window, SLOT(inline_par_x_changed(bool)));
    connect(ui->drawGrid, SIGNAL(toggled(bool)), window, SLOT(show_grid(bool)));
    connect(ui->drawGrid, SIGNAL(toggled(bool)), ui->graphicsView, SLOT(toggleDragMode(bool)));
    connect(ui->saveButt, SIGNAL(clicked()), window, SLOT(save_grid()));
}

View::~View()
{
    delete ui;
}
