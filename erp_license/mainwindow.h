#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include "dbman.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_2_clicked();
    void on_register_2_clicked();

    void on_MainWindow_destroyed();

private:

private:
    Ui::MainWindow *ui;
    QLineEdit *m_editIP;
    QLineEdit *m_editDBName;
    QLineEdit *m_editUser;
    QLineEdit *m_editPasswd;
    QLineEdit *m_editKey;
    dbman m_db;
};

#endif // MAINWINDOW_H
