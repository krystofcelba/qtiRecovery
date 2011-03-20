#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "libirecovery.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int received_cb_g(irecv_client_t client, const irecv_event_t* event);
    int progress_cb_g(irecv_client_t client, const irecv_event_t* event);
    int precommand_cb_g(irecv_client_t client, const irecv_event_t* event);
    int postcommand_cb_g(irecv_client_t client, const irecv_event_t* event);

private slots:
    void showDeviceInfo();
    void sendCommand();

private:
    Ui::MainWindow *ui;
    irecv_client_t client;
};

#endif // MAINWINDOW_H
