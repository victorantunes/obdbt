#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtQuick/QQuickView>
#include <QtQuick/qquickitem.h>
#include <QtQuickWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //criar os temporizadores like a Thread
    timer = new QTimer(this);

    //evento do timeout do timer - quando estourar faça o método atualizar
    connect(timer,SIGNAL(timeout()),this,SLOT(atualizarDados()));


    //Dados do QML
    /*QQuickView *view = new QQuickView();
    QWidget *container = QWidget::createWindowContainer(view, this);
    container->setMinimumSize(500, 200);
    container->setMaximumSize(500, 200);
    container->setFocusPolicy(Qt::TabFocus);
    view->setSource(QUrl("qrc:/qml/dashboard.qml"));
    ui->verticalLayout->addWidget(container);
    */

    QQuickWidget *view = new QQuickWidget();
    view->setResizeMode(QQuickWidget::SizeRootObjectToView);
    //connect(quickWidget, &QQuickWidget::statusChanged, this, &MainWindow::onStatusChangedWidget);
    //connect(quickWidget, &QQuickWidget::sceneGraphError, this, &MainWindow::onSceneGraphError);
    //view->setMinimumSize(500, 200);
    //view->setMaximumSize(500, 200);
    view->setSource(QUrl("qrc:/qml/dashboard.qml"));
    ui->verticalLayout->addWidget(view);

    QObject *item = view->rootObject();
    velocidade = item->findChild<QObject*>("velocidade");
    temperatura = item->findChild<QObject*>("temperatura");
    rpm = item->findChild<QObject*>("rpm");
    progBarBorboleta = item->findChild<QObject*>("progBarBorboleta");
    progBarfluxo = item->findChild<QObject*>("progBarfluxo");
    combustivel = item->findChild<QObject*>("combustivel");

    // Criar um agente para descoberta de dispositivos
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

    connect(discoveryAgent, SIGNAL(finished()),
               this, SLOT(finalizarBuscaDispositivos()) );

    //connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(buscarServidor()));

    connect(ui->pushButtonConectar,SIGNAL(clicked(bool)),this,SLOT(conectarServidor()));

    //Configurações para a conexão com o servidor
    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

    qDebug() << "Criou socket!";

    //Sinal que é executado quando existe novas informações disponíveis
    connect( socket, SIGNAL(readyRead()),
             this, SLOT(onReadyRead()) );

    connect( socket, SIGNAL(connected()),
             this, SLOT(onBluetoothConnected()) );

    connect( socket, SIGNAL(disconnected()),
             this, SLOT(onBluetoothDisconnected()) );

    connect( socket, SIGNAL(error(QBluetoothSocket::SocketError)),
             this, SLOT(onBluetoothError(QBluetoothSocket::SocketError)) );

    connect( socket, SIGNAL(stateChanged(QBluetoothSocket::SocketState)),
             this, SLOT(onBluetoothStateChanged(QBluetoothSocket::SocketState)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onReadyRead()
{

    QByteArray data = socket->readAll();

    QString str(data);
    QStringList list;
    QStringList list2;
    QString texto;
    bool ok;

    //qDebug() << "STR: " + str;

    //qualquer sequência de não-caracteres serão usadas para separar o texto
    list2 = str.split(QRegExp("\\W+"), QString::SkipEmptyParts);

    //qDegub() << list2;

    list = QStringList();

    for (int i = 0; i < list2.size(); i++){
        //qDebug() << list2[i];
        texto += list2[i];
    }

    //qDebug() << "Texto: " + texto;

    for(int i=0; i< texto.size(); i++){
        if(i < texto.size() - 1){

            QString substr = texto.midRef(i, 4).toString();

            //Tratamento temperatura motor
            if(substr == "4105"){
                temperaturaMotor = texto.midRef(i + 4, 2).toInt(&ok,16) - 40;
                if (temperatura)
                    temperatura->setProperty("value", (double)temperaturaMotor/215);
                qDebug() << "Temperatura do motor:" << temperaturaMotor;
            }

            //Tratamento AR
            if (substr == "4110"){
                ar = (texto.midRef(i + 4, 2).toInt(&ok,16)*256 + texto.midRef(i + 6, 2).toInt(&ok,16))/100;
                if (progBarfluxo)
                    progBarfluxo->setProperty("value", (double)ar);
                qDebug() << "Fluxo de ar:" << ar;
            }

            //Tratamento RPM
            if (substr == "410C"){
                rpm_int = (texto.midRef(i + 4, 2).toInt(&ok,16)*256 + texto.midRef(i + 6, 2).toInt(&ok,16))/4;
                if (rpm)
                    rpm->setProperty("value", (double)rpm_int/1000);
                qDebug() << "RPM:" << rpm_int;
            }

            //Tratamento Borboleta
            if (substr == "4111"){
                borboleta = (texto.midRef(i + 4, 2).toInt(&ok,16)*100)/255;
                if (progBarBorboleta)
                    progBarBorboleta->setProperty("value", borboleta);

                qDebug() << "Borboleta:" << borboleta;
            }

            //Tratamento Combustivel
            if (substr == "4106"){
                combustivelValor = (texto.midRef(i + 4, 2).toInt(&ok,16)-128)*100/128;
                if (combustivel)
                    combustivel->setProperty("value", combustivelValor);

                qDebug() << "Combustível:" << combustivelValor;
            }

            if (substr == "410D"){
                velocidadeInstantanea = texto.midRef(i + 4, 2).toInt(&ok,16);
                //ui->lcdNumberVelocidade->display(velocidadeInstantanea);
                if (velocidade)
                    velocidade->setProperty("value", (double)velocidadeInstantanea);
                qDebug() << "Velocidade do veiculo:" << velocidadeInstantanea;
            }

        }
    }

}

void MainWindow::onBluetoothDisconnected()
{
    qDebug() << "Bluetooth Disconnected!";
}

void MainWindow::onBluetoothError(QBluetoothSocket::SocketError erro)
{
    qDebug() << erro;
}

void MainWindow::onBluetoothStateChanged(QBluetoothSocket::SocketState state)
{
    qDebug() << state;
}

void MainWindow::onBluetoothConnected()
{

    //Garantir estado confiável
    socket->write( QByteArray("AT Z\r") );

    //inicializar o ELM327
    socket->write( QByteArray("AT E0\r") );
    socket->write( QByteArray("AT L0\r") );
    socket->write( QByteArray("AT ST 00\r") );
    socket->write( QByteArray("AT SP 0\r") );

    qDebug() << "ELM327 Inicializado";

    timer->start(100);  //2000ms = 2s
}

void MainWindow::conectarServidor()
{
    qDebug() << "Solicita conexão com servidor";

//    QString address(ui->comboBox->currentText());
//    QStringList lista = address.split(" ");
//    if (lista.size() > 1){
//        QBluetoothAddress addr(lista.at(1));
//        qDebug() << lista.at(1);
//        qDebug() << "ADDR";
//        qDebug() << addr;
//        socket->connectToService(addr,
//                QBluetoothUuid(QString("00001101-0000-1000-8000-00805F9B34FB")),
//                QIODevice::ReadWrite);
//    }

   QBluetoothAddress addr("00:0D:18:3A:67:89");
   socket->connectToService(addr, QBluetoothUuid(QString("00001101-0000-1000-8000-00805F9B34FB")),QIODevice::ReadWrite);


}

void MainWindow::buscarServidor()
{
    qDebug() << "Inicia busca de servidores!";
    discoveryAgent->start();
}

void MainWindow::finalizarBuscaDispositivos()
{
    qDebug() << "Conclui busca de servidores!";
//    QList<QBluetoothDeviceInfo> devices = discoveryAgent->discoveredDevices();

//    foreach(QBluetoothDeviceInfo d, devices)
//        ui->comboBox->addItem(d.name() + " " + d.address().toString());
}

void MainWindow::atualizarDados()
{

//    qDebug() << "Solicita temperatura";

    //Temperatura
    //socket->write(QByteArray("01 05\r"));

    //qDebug() << "Solicita abertura da borboleta";

    //Borboleta
    //socket->write(QByteArray("01 11\r"));

    //qDebug() << "Solicita velocidade";
    //Velocidade
    //socket->write(QByteArray("01 0D\r"));

    //qDebug() << "Solicita RPM";
    //RPM
    //socket->write(QByteArray("01 0C\r"));

    //qDebug() << "Solicita fluxo de ar";
    //AR
    socket->write(QByteArray("01 10\r"));

    //qDebug() << "Solicita Combustível";
    //Combustivel
    //socket->write(QByteArray("01 06\r"));

    //socket->write(QByteArray("01 00\r"));

}
