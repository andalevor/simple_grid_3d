#ifndef VIEW_HPP
#define VIEW_HPP

#include <QGraphicsScene>
#include <QWidget>

namespace Ui {
class View;
}

class View : public QWidget {
    Q_OBJECT

public:
    explicit View(double min_x, double min_y, QWidget* parent = nullptr);
    ~View();

private:
    Ui::View* ui;
};

#endif // VIEW_HPP
