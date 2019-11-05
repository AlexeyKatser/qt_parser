#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <qstringlist.h>
#include <qtnetwork/QNetworkProxy>

enum nbFirms
{
    HP = 1,
    ASUS = 2,
    ACER = 3,
    DELL = 4,
    LENOVO = 5,
    PACKARD_BELL = 6
};

enum repErrors
{
    noError,
    replyError = 1,
    ipBan = 2,
    reThreadError = 3,
    reStopedByUser = 4,
    reTooManyThreads = 5
};

namespace Ui {
class MainWindow;
}
class CParser : public QObject
{
    Q_OBJECT
public:
    CParser(int d){mode = d; manager = new QNetworkAccessManager(this);
                   connect(manager, SIGNAL(finished(QNetworkReply*)),
                           this, SLOT(replyFinished(QNetworkReply*)));

                  }
    QNetworkAccessManager *manager;
    int mode;
    int row;
    QString ip;
    quint16 port;
    QString url;
    int errorType;
    ~CParser(){manager->disconnect();
              // delete this->thread();
              // qDebug() << "manager deleted";
        delete manager;}
public slots:
    void replyFinished(QNetworkReply * rep);
    void getInfo();

signals:
    void done();
    void deleteit();
    void updateRow(int, QStringList);
    void addRow(QStringList);
    void updateItem(int row, int column, QString text);
    void createThread(QString);
    void endThread();
    void repliedWithError(int);
};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = NULL);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void createThread(QString, int);
    void on_pushButton_2_clicked();
    void addRow(QStringList);
    void on_btn_SaveToExcel_clicked();
    void updateRow(int, QStringList);
    void on_pushButton_3_clicked();
    void updateCounter();
    void on_pushButton_4_clicked();
    void updateStartRow(int);
    void on_pushButton_5_clicked();

    void on_edit_IP_textChanged(const QString &arg1);

    void on_edit_Delay_editingFinished();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_groupBox_3_clicked();

private:
    Ui::MainWindow *ui;
    void deleteExtraNB();
    void parsLinks(QString);
};



#endif // MAINWINDOW_H
