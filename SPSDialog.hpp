#ifndef SPSDIALOG_HPP
#define SPSDIALOG_HPP

#include <QDialog>

namespace Ui {
class SPSDialog;
}

class SPSDialog : public QDialog {
    Q_OBJECT

public:
    explicit SPSDialog(QWidget* parent = nullptr);
    ~SPSDialog();
    QString const& r_sps_file_name() { return r_sps_name; }
    QString const& s_sps_file_name() { return s_sps_name; }
    QString const& x_sps_file_name() { return x_sps_name; }
    bool sps_revision_2() { return sps_rev_2; }

public slots:
    void chooseRSPS();
    void chooseSSPS();
    void chooseXSPS();
    void setRev0();
    void setRev21();

private:
    void check_names();
    Ui::SPSDialog* ui;
    QString r_sps_name;
    QString s_sps_name;
    QString x_sps_name;
    bool sps_rev_2 = false;
};

#endif // SPSDIALOG_HPP
