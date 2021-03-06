#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_2_clicked();

    void on_actionAbout_triggered();

    void on_actionOpen_triggered();

    void on_pushButton_clicked();

    void on_actionGenerate_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
