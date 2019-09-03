#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QTableView"
#include "QStandardItemModel"
#include <string>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    currentPage = 1;

    initTableView();
    initListTableView();

}
void MainWindow::initTableView(){
    XTTableView *tableview = ui->tableView;
    tableview->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableview->setSelectionMode(QAbstractItemView::SingleSelection);
    tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableview->setAlternatingRowColors(true);


    connect(tableview,SIGNAL(leftClicked(const QModelIndex &)),this,SLOT(testSlot()));

    QStandardItemModel *model = new QStandardItemModel();
    tableview->setModel(model);//来使用model
//    model->setHeaderData(0,Qt::Horizontal,"ID");//设置表的column名称；
    model->setColumnCount(4);    //列

    //添加数据
    for(int j=0;j<15;j++)
    {
         //写id
        QStandardItem *itemID = new QStandardItem("hello"+QString::number(j));//QString::number(j)));
        model->setItem(j,0,itemID);

        QStandardItem *item2 = new QStandardItem("22222"+QString::number(j));
        model->setItem(j,1,item2);

        QPushButton * m_button = new QPushButton("打开");

                        //触发下载按钮的槽函数
        connect(m_button, SIGNAL(clicked(bool)), this, SLOT(clickDownloadButton())); //
        m_button->setProperty("row", j);  //为按钮设置row属性
        m_button->setProperty("fileName", QString::number(j));  //为按钮设置fileName属性
        //m_button->setProperty("column", col)

       ui->tableView->setIndexWidget(model->index(model->rowCount() - 1,3),m_button);

    }
    tableview->setColumnWidth(0,500);
    tableview->setColumnWidth(1,80);
    tableview->setColumnWidth(2,80);
    tableview->setColumnWidth(3,80);
    tableview->showMaximized();

    tableview->setContextMenuPolicy(Qt::CustomContextMenu);  //少这句，右键没有任何反应的。

    rightMenu = new QMenu;
    cutAction = new QAction("剪切",this);
    copyAction = new QAction("复制",this);
    pasteAction = new QAction("粘贴",this);
    deleteAction = new QAction("删除",this);

    rightMenu->addAction(cutAction);
    rightMenu->addAction(copyAction);
    rightMenu->addAction(pasteAction);
    rightMenu->addAction(deleteAction);

    connect(tableview,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(clicked_rightMenu(QPoint)));
    connect(rightMenu, SIGNAL(triggered(QAction *)),
            this, SLOT(onShowOrHideColumn(QAction *)));

}

void MainWindow::initListTableView(){
    XTTableView *listTableView = ui->listTableView;
    listTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    listTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    listTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(listTableView,SIGNAL(leftClicked(const QModelIndex &)),this,SLOT(clickListTableView()));

    QStandardItemModel *model = new QStandardItemModel();
    listTableView->setModel(model);//来使用model
    model->setColumnCount(1);    //列

    QFile file(QDir::homePath() + "/magnet_qt/rule.json");
        if(!file.open(QIODevice::ReadWrite)) {
            qDebug() << "File open error";
        } else {
            qDebug() <<"File open!";
        }

    QByteArray allData = file.readAll();
    file.close();


    QJsonParseError json_error;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));

    if(jsonDoc.isNull() || (json_error.error != QJsonParseError::NoError))
    {
            qDebug() << "json error!";
            return;
     }

    QJsonArray array = jsonDoc.array();
    jsonArray = array;

    for(int i = 0; i < array.count(); i++)
    {

        QJsonObject object = array[i].toObject();
        QStandardItem *item = new QStandardItem(object.value("site").toString());
        model->setItem(i,0,item);
    }

    listTableView->setColumnWidth(0,150);
    listTableView->selectRow(0);
    currentListSelect = 0;

}
//网站列表点击
void MainWindow::clickListTableView(){
//    qDebug() << "row:" << ui->listTableView->currentIndex().row() << "column:" << ui->listTableView->currentIndex().column();
    currentListSelect = ui->listTableView->currentIndex().row();
}

void MainWindow::clicked_rightMenu(const QPoint &pos)  //右键信号槽函数
{
    rightMenu->exec(QCursor::pos());

    qDebug() << pos;
}
void MainWindow::onShowOrHideColumn(QAction *action)
{
    qDebug() << action;
    // 稍后会添加该函数的实现代码。
}

//搜索按钮点击事件
void MainWindow::on_searchButton_clicked(){

    qDebug() << "currentListSelect:" << currentListSelect;
    QJsonObject object = jsonArray[currentListSelect].toObject();

//    if(object.isEmpty()){
//        qDebug() << "源网站对象为空";
//        return;
//    }

//    if(ui->searchText->text().isEmpty()){
//        qDebug() << "搜索关键字为空";
//        return;
//    }

    ui->searchText->setText("钢铁侠");

    QString string = object.value("source").toString();

    QString str_page = string.replace(QRegExp("PPP"), QString::number(currentPage));

    QByteArray keyword_utf8 = ui->searchText->text().toUtf8();
    QByteArray byteArrayPercentEncoded = keyword_utf8.toPercentEncoding();
    QString encode_keyword =  QString(byteArrayPercentEncoded);
    QString url_str = string.replace(QRegExp("XXX"),encode_keyword);

    QString data_str = MainWindow::getHtml(url_str);

//     qDebug() << data_str;

     queryHTML(data_str,object.value("group").toString());

//    xmlDoc *d = htmlParseDoc((const xmlChar*)data_str, "utf-8");
//    qDebug()<< "doc:"<< d;



//        xmlDocPtr doc = NULL;
//        xmlXPathObjectPtr xpath_obj = NULL;
//        xmlNodeSetPtr nodeset = NULL;
//        xmlChar* xpath_exp = (xmlChar*)"//a";
//        xmlChar* uri;

//        doc = xmlParseMemory(data_str.toUtf8().constData(),strlen(data_str.toUtf8().constData()));

//        xpath_obj = getXPathObjectPtr(doc, xpath_exp);

//        if(NULL != xpath_obj) {
//            nodeset = xpath_obj->nodesetval;
//            int i = 0;
//            for(i = 0; i < nodeset->nodeNr; i ++) {
//                uri = xmlGetProp(nodeset->nodeTab[i],(xmlChar*)"href");
//                printf("link address:%s\n",uri);
//                xmlFree(uri);
//            }
//            xmlXPathFreeObject(xpath_obj);
//        }
//        xmlFreeDoc(doc);
//        xmlCleanupParser();
}

//static xmlDocPtr getDocPtr(char* docname) {
//   xmlDocPtr doc = NULL;
//   xmlKeepBlanksDefault(0);

//   doc = xmlParseFile(docname);
//   if(NULL == doc) {
//       fprintf(stderr, "document cannot be parsed!\n");
//       exit(1);
//   }
//   return doc;
//}

xmlXPathObjectPtr  MainWindow::getXPathObjectPtr(xmlDocPtr doc, xmlChar* xpath_exp) {
   xmlXPathObjectPtr result;
   xmlXPathContextPtr context;

   context = xmlXPathNewContext(doc);
   result = xmlXPathEvalExpression((const xmlChar*)xpath_exp, context);
   xmlXPathFreeContext(context);

   if(NULL == result) {
       fprintf(stderr, "eval expression error!\n");
       return NULL;
   }

   if(xmlXPathNodeSetIsEmpty(result->nodesetval)) {
       fprintf(stderr, "empty node set!\n");
       xmlXPathFreeObject(result);
       return NULL;
   }
   return result;
}


QStringList MainWindow::queryHTML(const QString &html, const QString &query) {
    QStringList list;

    htmlParserCtxtPtr ctxt = htmlNewParserCtxt();

    htmlDocPtr htmlDoc = htmlCtxtReadMemory(ctxt, html.toUtf8().constData(), strlen(html.toUtf8().constData()), "", "utf-8", 0);


    xmlXPathContextPtr context = xmlXPathNewContext ( htmlDoc );
//    xmlXPathRegister
    xmlXPathObjectPtr result = xmlXPathEvalExpression ((xmlChar*)query.toUtf8().constData(), context);
    xmlXPathFreeContext (context);
    if (result == NULL) {
        qDebug()<<"Invalid XQuery ?";
    }
    else {
        xmlNodeSetPtr nodeSet = result->nodesetval;
        if ( !xmlXPathNodeSetIsEmpty ( nodeSet ) ) {
            for (int i = 0; i < nodeSet->nodeNr; i++ ) {
                xmlNodePtr  nodePtr;
                nodePtr = nodeSet->nodeTab[i];
                QString xml = QString::fromUtf8((char*)nodePtr->children->content);
//                list.append(decodeXml(xml));
                qDebug()<<"xml:"<<xml;
            }
        }

        xmlXPathFreeObject (result);
    }
    qDebug()<<list;
    return list;
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::testSlot(){

    qDebug() << "row:" << ui->tableView->currentIndex().row() << "column:" << ui->tableView->currentIndex().column();
//    int row= ui->tableView->currentIndex().row();

//    QAbstractItemModel *model = ui->tableView->model ();

//    QModelIndex index = model->index(row,0);//选中行第一列的内容

//    QVariant data = model->data(index);

}

void MainWindow::clickDownloadButton(){
    QPushButton *btn = (QPushButton *)sender();   //产生事件的对象
    QString row = btn->property("row").toString();  //取得按钮的行号属性
    QString openFileName = btn->property("fileName").toString();  //获取按钮的fileName属性，其他的可以自行添加。

    qDebug() << "click_Button:" << row;
    qDebug() << "row:" << ui->tableView->currentIndex().row() << "column:" << ui->tableView->currentIndex().column();

}








