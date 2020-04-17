#ifndef DIALOGABOUT_H
#define DIALOGABOUT_H

#include <QDialog>
#include <QGraphicsDropShadowEffect>

namespace Ui {
    class DialogAbout;
}

class DialogAbout : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAbout(QWidget *parent = 0);
    ~DialogAbout();

private slots:
    void on_pushButto_ok_clicked();

private:
    Ui::DialogAbout *ui;
};

#endif // DIALOGABOUT_H
