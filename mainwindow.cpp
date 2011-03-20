#include "mainwindow.h"
#include "ui_mainwindow.h"

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

        QString statusString = QString("");
        unsigned long long ecid;
        unsigned char srnm[12];

        int ret = irecv_get_ecid(client, &ecid);
        if(ret == IRECV_E_SUCCESS)
        {
                statusString.sprintf("Device connected ECID: %lld", ecid);
                ui->deviceInfoLabel->setText(statusString);
        }
    }
}

void MainWindow::showDeviceInfo()
{
    printf("ahoj\n");
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
    delete ui;
}
