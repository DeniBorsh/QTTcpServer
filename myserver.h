#pragma once

#include <QtWidgets>
#include <QTcpServer>
#include <QTcpSocket>

class MyServer : public QWidget {
Q_OBJECT
public:
    MyServer(int port, QWidget* parent = nullptr);

public slots:
    virtual void slotNewConnection();
    void slotReadClient();

private:
    QTcpServer* m_tcpServer;
    QTextEdit* m_textEdit;
    quint16 m_nextBlockSize;

    void sendToClient(QTcpSocket* socket, const QString& str);
};
