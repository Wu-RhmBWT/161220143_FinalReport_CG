#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPainter>
#include <qcolordialog.h>
#include <QMessageBox>
#include <QString>
#include <QInputDialog>
#include <QFileDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void DrawMap();

private slots:
    void on_color_choose_clicked();

    void on_point_clicked();

    void on_penSize_valueChanged(int arg1);

    void on_line_clicked();

    void on_curve_clicked();

    void on_rect_clicked();

    void on_filled_rect_clicked();

    void on_poly_clicked();

    void on_circle_clicked();

    void on_filled_circle_clicked();

    void on_oval_clicked();

    void on_filled_oval_clicked();

    void on_filled_poly_clicked();

    void on_action_undo_triggered();

    void on_action_redo_triggered();

    void on_move_clicked();

    void on_choose_clicked();

    void on_choose_cancel_clicked();

    void on_rotate_clicked();

    void on_zoom_clicked();

    void on_choose_rect_clicked();

    void on_change_color_clicked();

    void on_action_open_triggered();
    void on_action_save_triggered();
    void on_action_new_triggered();
    void on_size_choose_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
