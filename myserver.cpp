#include "myserver.h"

MyServer::MyServer(int port, QWidget *parent) : QWidget(parent)
                                    , m_nextBlockSize{0}
{
    m_tcpServer = new QTcpServer(this);
    if (!m_tcpServer->listen(QHostAddress::Any, port)) {
        QMessageBox::critical(0, "Server Error",
                              "Unable to start the server:" + m_tcpServer->errorString());
        m_tcpServer->close();
        return;
    }
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    m_textEdit = new QTextEdit;
    m_textEdit->setReadOnly(true);

    auto vLayout = new QVBoxLayout;
    vLayout->addWidget(new QLabel{"<H1>Server</H1>"});
    vLayout->addWidget(m_textEdit);
    setLayout(vLayout);
}

void MyServer::slotNewConnection() {
    QTcpSocket* clientSocket = m_tcpServer->nextPendingConnection();
    connect(clientSocket, SIGNAL(disconnected()),
            clientSocket, SLOT(deleteLater()));
    connect(clientSocket, SIGNAL(readyRead()),
            this, SLOT(slotReadClient()));
    sendToClient(clientSocket, "Server Response: Connected!");
}

void MyServer::slotReadClient() {
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_5_15);
    while(true) {
        if (!m_nextBlockSize) {
            if (clientSocket->bytesAvailable() < sizeof(quint16))
                break;
            in >> m_nextBlockSize;
        }
        if (clientSocket->bytesAvailable() < m_nextBlockSize)
            break;

        QTime time;
        QString str;
        in >> time >> str;
        QString message = time.toString() + " " + "Client has sent - " + str;
        m_textEdit->append(message);
        m_nextBlockSize = 0;
        sendToClient(clientSocket, "Server Response: Received \"" + str + "\"");
    }
}

void MyServer::sendToClient(QTcpSocket *socket, const QString &str) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint16(0) << QTime::currentTime() << str;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));

    socket->write(block);
}

