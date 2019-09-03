#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QMainWindow>
#include "XTTableView.h"
#include <QMenu>
#include <QMenuBar>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QDir>
#include <QUrl>
#include <QApplication>
#include <QtWebView/QtWebView>
#include <QtCore>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <iostream>
#include <QtXmlPatterns/QXmlQuery>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/HTMLparser.h>
#include <iconv.h>
#include <QtXmlPatterns/QtXmlPatterns>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QStringList queryHTML(const QString &html, const QString &query);
    static QString getHtml(QString url)
    {
        QNetworkAccessManager *manager = new QNetworkAccessManager();
        QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url)));
        QByteArray responseData;
        QEventLoop eventLoop;
        connect(manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
        eventLoop.exec();
//block until finish
        responseData = reply->readAll();
        return QString(responseData);
    }

private slots:


    xmlXPathObjectPtr getXPathObjectPtr(xmlDocPtr doc, xmlChar* xpath_exp);

    void on_searchButton_clicked();
    void initTableView();
    void initListTableView();

    void clickListTableView();
    void clickDownloadButton();
    void testSlot();
    void clicked_rightMenu(const QPoint &pos);  //右键信号槽函数
    void onShowOrHideColumn(QAction *action);


private:
    Ui::MainWindow *ui;

    //记录当前选择网站序号
    int currentListSelect;
    int currentPage;
    QJsonArray jsonArray;

    QMenu *rightMenu;  //右键菜单
    QAction *cutAction;  //剪切
    QAction *copyAction;  //复制
    QAction *pasteAction;  //粘贴
    QAction *deleteAction;  //删除
};

#endif // MAINWINDOW_H












