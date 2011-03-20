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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"

MainWindow *thisPointer;

int received_cb(irecv_client_t client, const irecv_event_t* event);
int progress_cb(irecv_client_t client, const irecv_event_t* event);
int precommand_cb(irecv_client_t client, const irecv_event_t* event);
int postcommand_cb(irecv_client_t client, const irecv_event_t* event);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    thisPointer = this;

    connect(ui->actionDevice_info , SIGNAL(triggered()), this, SLOT(showDeviceInfo()));
    connect(ui->pushButton , SIGNAL(clicked()), this, SLOT(sendCommand()));

    irecv_init();
    client = NULL;
    for (int i = 0; i <= 5; i++)
    {
            printf("Attempting to connect... \n");

            if (irecv_open(&client) != IRECV_E_SUCCESS)
                    sleep(1);
            else
                    break;

            if (i == 5) {
                    qApp->quit();
            }
    }

    if(client)
    {
        irecv_event_subscribe(client, IRECV_PROGRESS, &progress_cb, NULL);
        irecv_event_subscribe(client, IRECV_RECEIVED, &received_cb, NULL);
        irecv_event_subscribe(client, IRECV_PRECOMMAND, &precommand_cb, NULL);
        irecv_event_subscribe(client, IRECV_POSTCOMMAND, &postcommand_cb, NULL);

        irecv_get_device(client, &device);

        QString statusString = QString("");
        unsigned char srnm[12], imei[15];

        int ret = irecv_get_srnm(client, srnm);
        if(ret == IRECV_E_SUCCESS)
        {
            statusString.sprintf("%s connected. Serial number: %s", device->product, srnm);
            ui->deviceInfoLabel->setText(statusString);
        }
    }
}

void MainWindow::showDeviceInfo()
{
    char info[1000];
    int ret;
    unsigned int cpid, bdid;
    unsigned long long ecid;
    unsigned char srnm[12], imei[15];

    //sprintf(info, "%sProduct: %s\nModel: %s\nBoard id: %s\nChip id: %s\n", info, device->product, device->model, device->board_id, device->chip_id);

    ret = irecv_get_cpid(client, &cpid);
    if(ret == IRECV_E_SUCCESS)
    {
        sprintf(info, "%sCPID: %d\n", info, cpid);
    }

    ret = irecv_get_bdid(client, &bdid);
    if(ret == IRECV_E_SUCCESS)
    {
        sprintf(info, "%sBDID: %d\n", info, bdid);
    }

    ret = irecv_get_ecid(client, &ecid);
    if(ret == IRECV_E_SUCCESS)
    {
        sprintf(info, "%sECID: %lld\n", info, ecid);
    }

    ret = irecv_get_srnm(client, srnm);
    if(ret == IRECV_E_SUCCESS)
    {
        sprintf(info, "%sSRNM: %s\n", info, srnm);
    }

    ret = irecv_get_imei(client, imei);
    if(ret == IRECV_E_SUCCESS)
    {
        sprintf(info, "%sIMEI: %s\n", info, imei);
    }


    QMessageBox *dialog = new QMessageBox(this);
    dialog->setText(info);
    dialog->exec();
}

void MainWindow::sendCommand()
{
    QString tmp = QString("> ");
    tmp.append(ui->lineEdit->text());
    ui->textEdit->append(tmp);

    char *cmd = ui->lineEdit->text().toAscii().data();
    printf("%s",cmd);
    int error = irecv_send_command(client, cmd);
    if (error != IRECV_E_SUCCESS)
    {
        qApp->quit();
    }
}



int received_cb(irecv_client_t client, const irecv_event_t* event)
{
    return thisPointer->received_cb_g(client, event);
}

int precommand_cb(irecv_client_t client, const irecv_event_t* event)
{
    return thisPointer->precommand_cb_g(client, event);
}

int postcommand_cb(irecv_client_t client, const irecv_event_t* event)
{
    return thisPointer->postcommand_cb_g(client, event);
}

int progress_cb(irecv_client_t client, const irecv_event_t* event)
{
    return thisPointer->progress_cb_g(client, event);
}


int MainWindow::received_cb_g(irecv_client_t client, const irecv_event_t* event)
{
    if (event->type == IRECV_RECEIVED) {
            int i = 0;
            int size = event->size;
            char* data = event->data;
            for (i = 0; i < size; i++)
            {
                QString tmp = QString("");
                tmp.sprintf("%c", data[i]);
                ui->textEdit->append(tmp);
            }
    }
    return 0;
}
int MainWindow::precommand_cb_g(irecv_client_t client, const irecv_event_t* event)
{
    /*if (event->type == IRECV_PRECOMMAND) {
            irecv_error_t error = 0;
            if (event->data[0] == '/') {
                    parse_command(client, event->data, event->size);
                    return -1;
            }
    }*/
    return 0;
}

int MainWindow::postcommand_cb_g(irecv_client_t client, const irecv_event_t* event)
{
    char* value = NULL;
    char* action = NULL;
    char* command = NULL;
    char* argument = NULL;
    irecv_error_t error = IRECV_E_SUCCESS;

    if (event->type == IRECV_POSTCOMMAND) {
            command = strdup(event->data);
            action = strtok(command, " ");
            if (!strcmp(action, "getenv")) {
                    argument = strtok(NULL, " ");
                    error = irecv_getenv(client, argument, &value);
                    if (error != IRECV_E_SUCCESS) {
                            //debug("%s\n", irecv_strerror(error));
                            free(command);
                            return error;
                    }
                    QString tmp = QString("");
                    tmp.sprintf("%s\n", value);
                    ui->textEdit->append(tmp);
                    free(value);
            }

            if (!strcmp(action, "reboot")) {
                qApp->quit();
            }
    }

    if (command) free(command);
    return 0;
}

int MainWindow::progress_cb_g(irecv_client_t client, const irecv_event_t* event)
{
    /*if (event->type == IRECV_PROGRESS) {
            print_progress_bar(event->progress);
    }
    */
    return 0;

}


MainWindow::~MainWindow()
{
    irecv_close(client);
    irecv_exit();
    delete ui;
}
