#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qthread.h>
#include <QNetworkRequest>
#include <qregexp.h>
#include <QTableWidgetItem>
#include <qtablewidget.h>
#include <qeventloop.h>
#include <qtimer.h>
#include <QtSql/QSqlDatabase>
#include <qdir.h>
#include <qfile.h>
#include <qsqlquery.h>
#include <qmessagebox.h>
//#include <qrandom.h>
#include <qfiledialog.h>
#include <qsqlerror.h>
#include <qnetworkcookiejar.h>
#include <qabstractnetworkcache.h>

int failed;
int threadsInProccess;
int done;
int delay;
int chosenSite = 0;
QNetworkCookieJar *scJar;
    QAbstractNetworkCache *anc;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(14);
    ui->tableWidget->setColumnWidth(0, 200);
    failed = false;
    done = 0;
    delay = 1000;

    scJar = new QNetworkCookieJar;
    threadsInProccess = 0;
   // anc = new QAbstractNetworkCache;
    ui->groupBox_3->clicked(this);
}


MainWindow::~MainWindow()
{
    delete ui;

}

void CParser::replyFinished(QNetworkReply * rep)
{
    if (mode == 1)
        threadsInProccess--;
    // qDebug() << rep->errorString();
               connect(rep, SIGNAL(sslErrors(QList<QSslError>)), rep, SLOT(ignoreSslErrors()));
    if (rep->error() == QNetworkReply::NoError)
    {
        //qDebug() << "noError";
        QByteArray arr = QString(rep->readAll()).toLatin1();

        int lastPos = 0;
       // while( ( lastPos = exp.indexIn( arr, lastPos ) ) != -1 )

        if (mode == 0)
        {
            QStringList sList;
            qDebug() << rep->url() << arr.length();
           // qDebug() << arr;
            QRegExp exp("<a class=['\"][block ]*brand-link[^\\/]+['\"] href=['\"]([^>]+)['\"]");
            if (chosenSite == 0)
                exp.setPattern("col_mb10'[^rel]+href=['|\"]([^>]+)['\"]>");
            int parsFirm = -1;
            while ( (lastPos = exp.indexIn( arr, lastPos )) != -1)
            {
                //qDebug() << exp.cap(1);
                // lastPos = exp.indexIn( arr, lastPos );
                lastPos += exp.matchedLength();
                if (exp.matchedLength() == -1)
                    break;
                QString url = QString("https://www.laptopscreen.com") + exp.cap(1);
                if (chosenSite == 0)
                    url = QString("https://www.screencountry.com") + exp.cap(1);
                if (parsFirm == -1)
                {
                    QString fcont = "brand=hp";
                    if (chosenSite == 1)
                        fcont = "/HP-";
                    if (url.contains(fcont, Qt::CaseInsensitive))
                    {
                        parsFirm = HP;
                       // qDebug() << "HP";
                    }
                }
                url.replace("amp;", "");
                if (parsFirm == HP)
                {
                    QRegExp reg("([0-9][\\D]?[\\D]([\\D]))&brand");
                    if (chosenSite == 1)
                        reg.setPattern("[^POW](UR|ER|SR|SERIES)");
                    int pos = reg.indexIn(url);
                    if (chosenSite == 0)
                        if (reg.cap(1).trimmed() != "")
                            if (reg.cap(2) != "R")
                            {
                                continue;
                            }
                    if (chosenSite == 1)
                        if (pos == -1)
                            continue;
                }
                url.replace(" ", "%20");
                sList << url;
                //arr = arr.remove(0, exp.matchedLength());
                //emit addRow(sList);
               // sList.clear();
                //emit createThread(url);
            }
            qDebug() << "endpars";
            emit addRow(sList);
        }
        if (mode == 1)
        {
            if (arr.length() < 2000)
            {
                emit repliedWithError(row);
                errorType = ipBan;
                failed = ipBan;
                emit endThread();
                return;
            }
            //qDebug() << arr.length();
            QStringList sList;
            QStringList resList, typeList, dispList, conList, jpg1List, jpg2List;
           // sList << url;
            for (int i = 0; i < 13; i++)
                sList.append("");
            int jpgPos = 0;
            QString sStart = "sc_col sc_span_12 with_pad desktop_tablet_specs";
            if (chosenSite == 1)
                sStart = "row bg-white item-box";
            while ((lastPos = arr.indexOf(sStart, lastPos)) != -1)
            {
                jpgPos = lastPos;
               // qDebug() << lastPos;
                QRegExp reg(">\n([^<]+)<\\/span>\n[ ]+<span class='col span_15'>\n([0-9A-z -./]+)<");
                if (chosenSite == 1)
                    reg.setPattern("col dsk-span-3 tbt-span-4 no-mob spec-name text-right['\"]>([^:]+):<\\/div>\\s*<[^>]+>\\s*<[^>]+>\\s*<?b?>?([^<]+)");
                for (int i = 0; i < 9; i++)
                {
                    if ((lastPos = reg.indexIn(arr, lastPos )) == -1)
                        break;
                    lastPos += reg.matchedLength();
                    QString sType = reg.cap(1).trimmed();
                 //  qDebug() << sType << reg.cap(2).trimmed();
                    if (reg.cap(1).trimmed() == "Condition" || reg.cap(1).trimmed() == "Product Warranty" || reg.cap(1).trimmed() == "Warranty")
                        break;

                        //  if (reg.cap(2).trimmed() == "Touch Screen Only")
                      /*if (touch && rep == 1)
                        {
                            //qDebug() << "fill";
                            if (i == 1 || i == 2 || i == 4 || i == 6)
                                sList << reg.cap(2).trimmed();
                        }*/
                    if (sType == "Finish Type" || sType == "Surface Type")
                        if (sList.at(4) == "")
                            sList.replace(4, reg.cap(2).trimmed());
                    if (sType.toLower() == "display type" || sType == "Part Type")
                    {

                        bool repeat = false;
                        QString cap = reg.cap(2).trimmed();
                        if (cap.indexOf("b>") != -1)
                            cap = cap.replace("b>", "");
                        for (int k = 0; k < dispList.count(); k++)
                            if (dispList.at(k) == cap)
                                repeat = true;
                        if (!repeat)
                        {
                            if (sList.at(1) != "")
                                sList.replace(1, sList.at(1) + ";" + cap);
                            else
                                sList.replace(1, cap);
                            dispList << cap;
                        }
                    }
                    if (sType == "Compatibility")
                        if (sList.at(0) == "")
                            sList.replace(0, reg.cap(2).trimmed());
                    if (sType == "Screen Size" || sType == "size")
                        if (sList.at(2) == "")
                            sList.replace(2, reg.cap(2).trimmed());
                    if (sType == "Video Connector")
                    {
                        bool repeat = false;
                        for (int k = 0; k < conList.count(); k++)
                            if (conList.at(k) == reg.cap(2).trimmed())
                                repeat = true;
                        if (!repeat)
                        {
                            if (sList.at(6) != "")
                                sList.replace(6, sList.at(6) + ";" + reg.cap(2).trimmed());
                            else
                                sList.replace(6, reg.cap(2).trimmed());
                            conList << reg.cap(2).trimmed();
                        }
                    }
                    if (sType == "Display Technology")
                        if (sList.at(7) == "")
                            sList.replace(7, reg.cap(2).trimmed());
                    if ((sType == "Screen Resolution" && reg.cap(2).trimmed() != "") || sType == "resolution")
                    {
                            bool repeat = false;
                            for (int k = 0; k < resList.count(); k++)
                                if (resList.at(k) == reg.cap(2).trimmed())
                                    repeat = true;
                            if (!repeat)
                            {
                                if (sList.at(3) != "")
                                    sList.replace(3, sList.at(3) + ";" + reg.cap(2).trimmed());
                                else
                                    sList.replace(3, reg.cap(2).trimmed());
                                resList << reg.cap(2).trimmed();
                            }
                        }
                    if ((sType == "Backlight type" && reg.cap(2).trimmed() != "") || sType == "Optical Technology")
                    {
                        bool repeat = false;
                        for (int k = 0; k < typeList.count(); k++)
                            if (typeList.at(k) == reg.cap(2).trimmed())
                                repeat = true;
                        if (!repeat)
                        {
                            if (sList.at(5) != "")
                                sList.replace(5, sList.at(5) + ";" + reg.cap(2).trimmed());
                            else
                                sList.replace(5, reg.cap(2).trimmed());
                            typeList << reg.cap(2).trimmed();
                        }
                    }
                    if (sType == "Comments")
                    {
                        QString cap = reg.cap(2).trimmed();
                        if (cap.indexOf("b>") != -1)
                            cap = cap.replace("b>", "");
                        sList.replace(10, cap);
                    }
                    if (sType == "Refresh Rate")
                        sList.replace(11, reg.cap(2).trimmed());
                    if (sType == "Mountings")
                        sList.replace(12, reg.cap(2).trimmed());
                }
                // searching images
                QRegExp imgReg("href='([^>]+)'>");
                if (chosenSite == 1)
                    imgReg.setPattern("img\\s+src=['\"]([^\"']+)");
                if ((jpgPos = imgReg.indexIn(arr, jpgPos)) != -1)
                {
                   // qDebug() << jpgPos << lastPos;
                    if (jpgPos < lastPos)
                    {
                        jpgPos += imgReg.matchedLength();
                        QString str = imgReg.cap(1);
                       // qDebug() << str;
                        while (str.contains("/"))
                        {
                            str = str.remove(0, str.indexOf("/") + 1);

                        }
                        bool repeat = false;
                        for (int k = 0; k < jpg1List.count(); k++)
                            if (jpg1List.at(k) == str)
                                repeat = true;
                        if (!repeat)
                        {
                            if (sList.at(8) != "")
                                sList.replace(8, sList.at(8) + ";" + str);
                            else
                                sList.replace(8, str);
                            jpg1List << str;
                        }
                    }
                }
                if ((jpgPos = imgReg.indexIn(arr, jpgPos)) != -1)
                {
                    if (jpgPos < lastPos)
                    {
                        QString str = imgReg.cap(1);

                        while (str.contains("/"))
                            str = str.remove(0, str.indexOf("/") + 1);
                        bool repeat = false;
                        for (int k = 0; k < jpg2List.count(); k++)
                            if (jpg2List.at(k) == str)
                                repeat = true;
                        if (!repeat)
                        {
                            if (sList.at(9) != "")
                                sList.replace(9, sList.at(9) + ";" + str);
                            else
                                sList.replace(9, str);
                            jpg2List << str;
                        }
                    }
                }
            }
            //qDebug() << "rep";
                emit updateRow(row, sList);
            //emit this->thread()->terminate();
           //emit this->thread()->deleteLater();
        }
    }
    else
    {
        emit repliedWithError(row);
        if ((rep->error() != 401) && (rep->error() != QNetworkReply::UnknownNetworkError) && (rep->error() != QNetworkReply::RemoteHostClosedError)
                && (rep->errorString() != "Proxy denied connection"))
        {
            errorType = replyError;
            failed = replyError;
        }
        qDebug() << rep->errorString() << rep->error();
    }
    emit endThread();
}

void MainWindow::createThread(QString url, int row)
{
    QThread *thread = new QThread;
    if (thread == NULL)
    {
        failed = reThreadError;
        return;
    }
    CParser *par = new CParser(1);
    par->mode = 1;
    par->row = row;
    par->url = url;
    par->ip = ui->edit_IP->text();
    par->port = ui->edit_Port->text().toInt();
  // par->manager->setCookieJar(scJar);
   // par->manager->setCache(anc);
   // qDebug() << url;
    //par->getInfo();
    connect(thread, SIGNAL(started()), par, SLOT(getInfo()));
   // connect(par, SIGNAL(done()), this, SLOT(saveToExcel()));
   // connect(par, SIGNAL(done()), this, SLOT(setRetryEnabled()));
   // connect(par, SIGNAL(increase()), this, SLOT(increaseProgressBar()));
   // connect(par, SIGNAL(updateItem(int,int, QString)), this, SLOT(updateTableItem(int,int,QString)));
    connect(par, SIGNAL(updateRow(int, QStringList)), this, SLOT(updateRow(int, QStringList)));
    connect(par, SIGNAL(repliedWithError(int)), this, SLOT(updateStartRow(int)));
    connect(par, SIGNAL(endThread()), this, SLOT(updateCounter()));
    connect(par, SIGNAL(endThread()), thread, SLOT(quit()));
    connect(par, SIGNAL(endThread()), par, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    par->moveToThread(thread);
    thread->start();
   // thread->sleep(1);

}

void CParser::getInfo()
{
    QNetworkProxy proxy(QNetworkProxy::HttpProxy, ip, port);
    if (ip != "")
        manager->setProxy(proxy);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.106 Safari/537.36");
   req.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    req.setRawHeader("Accept-Language","en-US,en;q=0.5");
    req.setRawHeader("Connection", "keep-alive");
    /*QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setProtocol(QSsl::TlsV1_0);
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);
    req.setSslConfiguration(sslConfig);*/
    manager->get(req);
}

void MainWindow::on_pushButton_clicked()
{
    qDebug() << ui->sb_From->value() << ui->sb_To->value();
    if ((ui->sb_From->value() != 0) && (ui->sb_To->value() != 0))
        for (int i = ui->sb_From->value(); (i <= ui->sb_To->value()) && (failed == 0); i++)
        {
            ui->sb_From->setValue(i);
            parsLinks(ui->lineEdit->text() + QString::number(i));
            int delay = 200;
            QEventLoop loop;
            QTimer timer;
            timer.setInterval(delay + (double(qrand())/RAND_MAX)*100); //5 sec
            connect (&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
            timer.start();
            loop.exec();
        }
   else
        parsLinks(ui->lineEdit->text());
    if (failed == 0)
    {
        ui->sb_To->setValue(0);
        ui->sb_From->setValue(0);
    }

}

void MainWindow::parsLinks(QString url)
{
   // QThread *thread = new QThread;
    CParser *par = new CParser(0);
    par->mode = 0;
   // par->moveToThread(thread);
    par->url = url;
    par->ip = ui->edit_IP->text();
    par->port = ui->edit_Port->text().toInt();

   // par->sender = this;
    //connect(thread, SIGNAL(started()), par, SLOT(getInfo()));
   // connect(par, SIGNAL(done()), this, SLOT(saveToExcel()));
   // connect(par, SIGNAL(done()), this, SLOT(setRetryEnabled()));
   // connect(par, SIGNAL(increase()), this, SLOT(increaseProgressBar()));
   // connect(par, SIGNAL(updateItem(int,int, QString)), this, SLOT(updateTableItem(int,int,QString)));
    //connect(par, SIGNAL(createThread(QString)), this, SLOT(createThread(QString)));
    connect(par, SIGNAL(addRow(QStringList)), this, SLOT(addRow(QStringList)));

   // connect(thread, SIGNAL(finished()), par, SLOT(deleteLater()));
    par->getInfo();
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    done = 0;
}

void MainWindow::addRow(QStringList text)
{
    //text.count() < 10 ? ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1) :
    qDebug() << text.count() << ui->tableWidget->rowCount();
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(text.count() + ui->tableWidget->rowCount());
    int added = 0;
    for (int i = 0; i < text.count(); i++)
    {
        QString url = text.at(i);
        QList<QTableWidgetItem *> find = ui->tableWidget->findItems(url, Qt::MatchFixedString);
        if (find.count() == 0)
        {
            QTableWidgetItem *item = new QTableWidgetItem(url);
            ui->tableWidget->setItem(row++, 0, item);
            added++;
        }
       // qDebug() << i;
    }
    qDebug() << "added:" << added;
    /*for (int i = 0; i < text.count(); i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem(text.at(i));
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, i, item);
    }*/
    for (int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        if (ui->tableWidget->item(i, 0) == NULL)
        {
            ui->tableWidget->removeRow(i--);
            continue;
        }
        if (ui->tableWidget->item(i,0)->text() == "")
        {
            ui->tableWidget->removeRow(i--);
            continue;
        }
    }
    ui->tableWidget->repaint();
}

void MainWindow::on_btn_SaveToExcel_clicked()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC3","dbExcel");
    QString sPath = QFileDialog::getSaveFileName(this, tr("Save file as..."), "screens", "*.xlsx");
    if (!sPath.contains(".xlsx"))
        sPath += ".xlsx";

    if (!QFile().copy("D:\\Projects\\qt\\NB_parser_new\\template.xlsx", sPath))
    {
        QMessageBox msgBox;
        msgBox.setText("Cann't overwrite existing file!");
        msgBox.exec();
        return;
    }

   // qDebug() << sPath;
    db.setDatabaseName(QString("DRIVER={Microsoft Excel Driver (*.xls, *.xlsx, *.xlsm, *.xlsb)};FIL=Excel 12.0;DriverID=1046;DBQ=%1;Readonly=False").arg(sPath));
    if (!db.open())
    {
        QMessageBox msgBox;
        msgBox.setText("Error opening excel!");
        msgBox.exec();
        return;
    }
    QSqlQuery query(QSqlDatabase::database("dbExcel"));
    for (int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        QString sql = QString("INSERT INTO [Лист1$] (URL, Наименование, [Тип экрана], Диагональ, Разрешение, матгл,  [Тип подсветки], Разъём, [Display Technology], foto1, foto2, Комментарий, Частота, Монтаж)");

        sql += " VALUES (:p0, :p1, :p2, :p3, :p4, :p5, :p6, :p7, :p8, :p9, :p10, :p11, :p12, :p13)";

        query.prepare(sql);

        for (int j = 0; j < ui->tableWidget->columnCount(); j++)
            if (ui->tableWidget->item(i, j) != nullptr)
                query.bindValue(j, ui->tableWidget->item(i, j)->text());
        query.exec();
    }
    QMessageBox msgBox;
    msgBox.setText("The document has been saved.");
    msgBox.exec();
    db.close();
}

void MainWindow::on_pushButton_3_clicked()
{
    //threadsInProccess = 0;
    failed = 0;
    int startRow = ui->sb_Row->value() == - 1? 0 : ui->sb_Row->value();
    ui->sb_Row->setValue(-1);
    for (int i = startRow; i < ui->tableWidget->rowCount() && failed == 0/* && (done < 500)*/; i++)
    {
        updateCounter();
        if (ui->tableWidget->item(i,1) != NULL)
            if (ui->tableWidget->item(i,1)->text() != "")
                continue;
        if (threadsInProccess++ > 80)
            delay += 50;
        if (threadsInProccess < 50 && delay > 0)
            delay >= 50 ? delay -= 50 : delay = 0;
        ui->edit_Delay->setText(QString::number(delay));
        ui->lcd_Delay->display(QString::number(delay));
        createThread(ui->tableWidget->item(i, 0)->text(), i);
        QEventLoop loop;
        QTimer timer;
        timer.setInterval(delay + (double(qrand())/RAND_MAX)*100); //5 sec
        connect (&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
        timer.start();
        loop.exec();
        if (threadsInProccess > 150)
            failed = reTooManyThreads;
    }
    if (failed > 0)
    {
        QMessageBox msgBox;
        if (failed == replyError)
            msgBox.setText("Error in request from server");
        if (failed == ipBan)
            msgBox.setText("IP banned");
        if (failed == reThreadError)
            msgBox.setText("Error creating thread");
        if (failed == reStopedByUser)
            msgBox.setText("Stoped by the user");
        if (failed == reTooManyThreads)
            msgBox.setText("Stoped! Too many threads!");
        msgBox.exec();
    }
}

void MainWindow::updateRow(int row, QStringList list)
{
    for (int i = 0; i < list.count(); i++)
    {
        QTableWidgetItem *item = ui->tableWidget->item(row, i + 1);
        if (item == nullptr)
        {
            item = new QTableWidgetItem(list.at(i));
            ui->tableWidget->setItem(row, i + 1, item);
        }
        else
            item->setText(list.at(i));
    }
    done++;
    ui->lcd_Done->display(done);
}

void MainWindow::on_pushButton_4_clicked()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC3","dbExcel");
    QString sPath = QFileDialog::getOpenFileName(this, tr("Open file"), "D:\\projects\\ascreens\\", "*.xlsx");
    db.setDatabaseName(QString("DRIVER={Microsoft Excel Driver (*.xls, *.xlsx, *.xlsm, *.xlsb)};FIL=Excel 12.0;DriverID=1046;DBQ=%1;Readonly=False").arg(sPath));
   // db.setDatabaseName("DRIVER={Microsoft Excel Driver (*.xlsx)};DBQ=" + sPath);
    db.open();
    QSqlQuery query(QSqlDatabase::database("dbExcel"));
   // QSqlQuery query("select * from [Sheet1$]");
   query.exec("select * from ["+ QString("Лист1") + "$]");
   // query.next();
    qDebug() << query.lastError() <<  query.size() << query.isActive();
    ui->tableWidget->setRowCount(query.size());
    int row = 0;
    while (query.next())
    {
        ui->tableWidget->setRowCount(row + 1);
        for (int i = 0; i < 14; i++)
        {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(i).toString());
            ui->tableWidget->setItem(row, i, item);
        }
        row++;
    }
    db.close();
    ui->tableWidget->repaint();
}

void MainWindow::on_pushButton_5_clicked()
{
    failed = reStopedByUser;
}

void MainWindow::on_edit_IP_textChanged(const QString &arg1)
{
    if (arg1.contains(":"))
    {
        ui->edit_IP->setText(arg1.mid(0, arg1.indexOf(":")));
        ui->edit_Port->setText(arg1.mid(arg1.indexOf(":") + 1, 4));
        ui->edit_Delay->setText("1000");
        delay = 1000;
        ui->lcd_Delay->display(delay);
    }
    if (arg1.contains(" "))
    {
        ui->edit_IP->setText(arg1.mid(0, arg1.indexOf(" ")));
        ui->edit_Port->setText(arg1.mid(arg1.indexOf(" ") + 1, 4));
        ui->edit_Delay->setText("1000");
        delay = 1000;
        ui->lcd_Delay->display(delay);
    }
    if (arg1.contains("	"))
    {
        ui->edit_IP->setText(arg1.mid(0, arg1.indexOf("	")));
        ui->edit_Port->setText(arg1.mid(arg1.indexOf("	") + 1, 4));
        ui->edit_Delay->setText("1000");
        delay = 1000;
        ui->lcd_Delay->display(delay);
    }
}

void MainWindow::updateCounter()
{
    ui->lcdNumber->display(threadsInProccess);
    ui->lcdNumber->repaint();
}

void MainWindow::on_edit_Delay_editingFinished()
{
    try
    {
        delay = ui->edit_Delay->text().toInt();
        ui->lcd_Delay->display(delay);
    }
    catch(...)
    {
        delay = 1000;
    }
}

void MainWindow::updateStartRow(int index)
{
    if (ui->sb_Row->value() == -1)
        ui->sb_Row->setValue(index);
    else
        if (ui->sb_Row->value() > index)
            ui->sb_Row->setValue(index);
}

void MainWindow::on_pushButton_6_clicked()
{
  /*  // Типа открытие сайта и сохранение кэша
     QWebView *view = new QWebView;
    QNetworkAccessManager *man = new QNetworkAccessManager;
    //man->setCookieJar(vnJar);
    man->setCookieJar(scJar);
   // man->setCache(anc);
    view->page()->setNetworkAccessManager(man);
    view->load(QUrl(ui->lineEdit->text()));
    view->show();

 //  deleteExtraNB();*/
}

void MainWindow::deleteExtraNB()
{
    int deleted = 0;
    for (int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        QString text = ui->tableWidget->item(i, 0)->text().toLower();
        QRegExp reg("([0-9][\\D]?[\\D]([\\D]))&brand");
        reg.indexIn(text);
        if (reg.cap(1) != "")
        {
            if (reg.cap(2) != "r")
            {
                ui->tableWidget->removeRow(i--);
                deleted++;
            }
        }
    }
    qDebug() << deleted;
}

void MainWindow::on_pushButton_7_clicked()
{
    deleteExtraNB();
}

void MainWindow::on_groupBox_3_clicked()
{
    if (ui->radioButton->isChecked())
        chosenSite = 0;
    if (ui->radioButton_2->isChecked())
        chosenSite = 1;
}
