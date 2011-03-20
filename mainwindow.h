/**
  * qtiRecovery - mainwindow.h
  * Copyright (C) 2011 Krystof Celba
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "libirecovery.h"
#include "QKeyEvent"

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
    void loadHistory();
    irecv_client_t client;
    irecv_device *device;

};

#endif // MAINWINDOW_H
