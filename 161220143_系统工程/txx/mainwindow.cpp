#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(this->width(),this->height());
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_color_choose_clicked()
{
    QColor color = QColorDialog::getColor(Qt::white);
    if(color.isValid())
    {
        int r, g, b;
        color.getRgb(&r, &g, &b);
        ui->openGLWidget->setColor(r,g,b);
    }
    update();
}

void MainWindow::on_penSize_valueChanged(int arg1)
{
    ui->openGLWidget->setpenSize(arg1);
}

void MainWindow::on_point_clicked()
{
    //QMessageBox::about(NULL, "About", QString::number(ui->openGLWidget->currentColor[0])+" "+QString::number(ui->openGLWidget->currentColor[1])+" "+QString::number(ui->openGLWidget->currentColor[2]));
    ui->openGLWidget->setMode(1);
    ui->openGLWidget->clean_drawingPoints();
    ui->statusBar->showMessage(tr("draw point"));
}

void MainWindow::on_line_clicked()
{
    ui->openGLWidget->setMode(2);
    ui->openGLWidget->clean_drawingPoints();
    ui->statusBar->showMessage(tr("draw line"));
}

void MainWindow::on_curve_clicked()
{
    ui->openGLWidget->setMode(3);
    ui->openGLWidget->clean_drawingPoints();
    ui->statusBar->showMessage(tr("draw curve"));
}

void MainWindow::on_rect_clicked()
{
    ui->openGLWidget->setMode(4);
    ui->openGLWidget->clean_drawingPoints();
    ui->statusBar->showMessage(tr("draw rectangle"));
}

void MainWindow::on_filled_rect_clicked()
{
    ui->openGLWidget->setMode(5);
    ui->openGLWidget->clean_drawingPoints();
    ui->statusBar->showMessage(tr("draw filled rectangle"));
}

void MainWindow::on_poly_clicked()
{
    ui->openGLWidget->setMode(6);
    ui->openGLWidget->clean_drawingPoints();
    ui->statusBar->showMessage(tr("draw polygon"));
}

void MainWindow::on_filled_poly_clicked()
{
    ui->openGLWidget->setMode(7);
    ui->openGLWidget->clean_drawingPoints();
    ui->statusBar->showMessage(tr("draw filled polygon"));
}

void MainWindow::on_circle_clicked()
{
    ui->openGLWidget->setMode(8);
    ui->openGLWidget->clean_drawingPoints();
    ui->statusBar->showMessage(tr("draw circle"));
}

void MainWindow::on_filled_circle_clicked()
{
    ui->openGLWidget->setMode(9);
    ui->openGLWidget->clean_drawingPoints();
    ui->statusBar->showMessage(tr("draw filled circle"));
}

void MainWindow::on_oval_clicked()
{
    ui->openGLWidget->setMode(10);
    ui->openGLWidget->clean_drawingPoints();
    ui->statusBar->showMessage(tr("draw oval"));
}

void MainWindow::on_filled_oval_clicked()
{
    ui->openGLWidget->setMode(11);
    ui->openGLWidget->clean_drawingPoints();
    ui->statusBar->showMessage(tr("draw filled oval"));
}

void MainWindow::on_action_undo_triggered()
{
    ui->openGLWidget->Undo();
}

void MainWindow::on_action_redo_triggered()
{
    ui->openGLWidget->Redo();
}

void MainWindow::on_move_clicked()
{
    ui->openGLWidget->setMode(0);
    ui->openGLWidget->clean_drawingPoints();
    ui->statusBar->showMessage(tr("move"));
}

void MainWindow::on_choose_clicked()
{
    ui->openGLWidget->setMode(-1);
    ui->openGLWidget->clean_drawingPoints();
    ui->statusBar->showMessage(tr("choose"));
}

void MainWindow::on_choose_rect_clicked()
{
    ui->openGLWidget->setMode(-2);
    ui->openGLWidget->clean_drawingPoints();
    ui->statusBar->showMessage(tr("choose Rect"));
}

void MainWindow::on_choose_cancel_clicked()
{
    ui->openGLWidget->setMode(0);
    ui->openGLWidget->clean_drawingPoints();
    ui->openGLWidget->choose_cancel();
    ui->statusBar->showMessage(tr("choose cancel"));
}

void MainWindow::on_rotate_clicked()
{
    QString text = QInputDialog::getText(NULL,tr("rotate"),tr("input angle[-180,180]"));
    if(text == NULL)
        return;
    double angle = text.toDouble();
    if(angle < -180 || angle > 180)
        QMessageBox::about(NULL, "invalid data", "must between -180 and 180");
    else ui->openGLWidget->rotate(angle);
    ui->statusBar->showMessage(tr("rotate"));
}

void MainWindow::on_zoom_clicked()
{
    QString text = QInputDialog::getText(NULL,tr("zoom"),tr("input times(0,5]"));
    if(text == NULL)
        return;
    double zoom = text.toDouble();
    if(zoom<=0||zoom>5)
        QMessageBox::about(NULL, "invalid data", "must between 0 and 5");
    else ui->openGLWidget->zoom(zoom);
    ui->statusBar->showMessage(tr("zoom"));
}

void MainWindow::on_change_color_clicked()
{
    ui->openGLWidget->changeColor();
    ui->statusBar->showMessage(tr("change color"));
}

void MainWindow::on_size_choose_clicked()
{
    ui->openGLWidget->changeSize();
    ui->statusBar->showMessage(tr("change size"));
}

void MainWindow::on_action_open_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,"打开文件",".",QStringLiteral("pic(*.png;*.jpg;*.jpeg;*.bmp);;All (*.*)"));
    if(!filename.isEmpty())
        ui->openGLWidget->openFile(filename);
}

void MainWindow::on_action_save_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this,"打开文件",".",QStringLiteral("pic(*.png;*.jpg;*.jpeg;*.bmp);;All (*.*)"));
    ui->openGLWidget->saveFile(filename);
}

void MainWindow::on_action_new_triggered()
{
    ui->openGLWidget->newFile();
}
