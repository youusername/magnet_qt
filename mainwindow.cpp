#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QTableView"
#include "QStandardItemModel"
#include <qdesktopservices.h>
#include <QLabel>
#include <QPixmap>
#include <QClipboard>
#include "httpclient.h"
#include <QMessageBox>

static const QString RULE_URL = "https://gitee.com/zvj88888888/magnet_qt/raw/master/rule.json";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);


    progressCircle = new ProgressCircle();

    progressCircle->setFixedSize(30, 30);
    progressCircle->setMaximumHeight(30);
    progressCircle->setHidden(true);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignCenter);
    layout->addWidget(progressCircle);
    ui->loading->setLayout(layout);


    connect(webView,&QWebEngineView::loadFinished,[this](int){
        qDebug()<<"loadFinished";
        webView->page()->runJavaScript("document.documentElement.outerHTML",QWebEngineScript::ApplicationWorld,[this](const QVariant& data) {
            qDebug()<<"123123:"<<data.toString();
            sideModel* object = jsonModelList[currentListSelect];
            resultList = queryHTML(data.toString(),object);

            reloadTableData(resultList);
            //加载完毕,隐藏loading
            progressCircle->setHidden(true);
        });

    });


    currentPage = 1;

    initTableView();
    initListTableView();

    QPixmap pix(":/img/icon512.png");
    ui->label_pix->setPixmap(pix.scaled(100,100,Qt::KeepAspectRatio));


    QFileInfo file("rule.json");


    HttpClient(RULE_URL).debug(true).success([=](const QString &response) {

    }).fail([](const QString &error, int errorCode) {
        qDebug() << error << errorCode;
    }).download(file.absoluteFilePath());
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
    model->setColumnCount(3);    //列

    //添加数据
//    for(int j=0;j<15;j++)
//    {
//         //写id
//        QStandardItem *itemID = new QStandardItem("hello"+QString::number(j));//QString::number(j)));
//        model->setItem(j,0,itemID);

//        QStandardItem *item2 = new QStandardItem("22222"+QString::number(j));
//        model->setItem(j,1,item2);

//        QPushButton * m_button = new QPushButton("打开");

//                        //触发下载按钮的槽函数
//        connect(m_button, SIGNAL(clicked(bool)), this, SLOT(clickDownloadButton())); //
//        m_button->setProperty("row", j);  //为按钮设置row属性
//        m_button->setProperty("fileName", QString::number(j));  //为按钮设置fileName属性
//        //m_button->setProperty("column", col)

//       ui->tableView->setIndexWidget(model->index(model->rowCount() - 1,3),m_button);

//    }
    tableview->setColumnWidth(0,tableview->size().width()-160);
    tableview->setColumnWidth(1,80);
    tableview->setColumnWidth(2,80);
    tableview->showMaximized();


    QHeaderView * header = tableview->verticalHeader();//ui->fileTree->header();
//    header->setSectionResizeMode(QHeaderView::ResizeToContents);
//    header->setStretchLastSection(false);
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::Fixed);
    header->setSectionResizeMode(2, QHeaderView::Fixed);

    tableview->setContextMenuPolicy(Qt::CustomContextMenu);  //少这句，右键没有任何反应的。

    rightMenu = new QMenu;

    copyAction = new QAction("复制",this);
    downloadAction = new QAction("下载",this);

    rightMenu->addAction(copyAction);
    rightMenu->addAction(downloadAction);

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

    QFileInfo file("rule.json");
    if(!file.isFile()){
        qDebug() <<"no File!";
        if(QFile::copy(":/rule","rule.json")){
            qDebug() <<"copy File!";
        }
    }
    qDebug() <<"File path:"<<file.absoluteFilePath();
    QFile openFile(file.absoluteFilePath());

    if(!openFile.open(QIODevice::ReadOnly)) {
        qDebug() << "File open error";
    } else {
        qDebug() <<"File open!";
    }

    QString allData = openFile.readAll();
    openFile.close();


    QJsonParseError json_error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(allData.toUtf8(), &json_error);
    qDebug() << "json:"<<jsonDoc;
    if(jsonDoc.isNull() || (json_error.error != QJsonParseError::NoError))
    {
            qDebug() << "json error!"<<json_error.errorString();
            return;
     }

    QJsonArray array = jsonDoc.array();


    for(int i = 0; i < array.count(); i++)
    {

        QJsonObject object = array[i].toObject();
        QStandardItem *item = new QStandardItem(object.value("site").toString());
        model->setItem(i,0,item);

        sideModel *jsonmodel = new sideModel();
        jsonmodel->site = object.value("site").toString();
        jsonmodel->magnet = object.value("magnet").toString();
        jsonmodel->group = object.value("group").toString();
        jsonmodel->name   = object.value("name").toString();
        jsonmodel->size   = object.value("size").toString();
        jsonmodel->count  = object.value("count").toString();
        jsonmodel->source = object.value("source").toString();
        jsonModelList << jsonmodel;
    }
//    qDebug()<<"jsonmodel:"<<jsonmodel->magnet;
//    delete(jsonmodel);

    listTableView->setColumnWidth(0,150);
    listTableView->selectRow(0);
    currentListSelect = 0;

}
//源网站列表点击
void MainWindow::clickListTableView(){
//    qDebug() << "list_row:" << ui->listTableView->currentIndex().row();
    currentListSelect = ui->listTableView->currentIndex().row();

    //搜索框不为空就直接切换源站点搜索
    if(!ui->searchText->text().isEmpty()){
        on_searchButton_clicked();
    }
}

void MainWindow::clicked_rightMenu(const QPoint &pos)  //右键信号槽函数
{
    rightMenu->exec(QCursor::pos());

    qDebug() << pos;
}

void MainWindow::onShowOrHideColumn(QAction *action)
{
//    qDebug() << action;
//    qDebug() << "menu_row:"<<ui->tableView->currentIndex().row();
    sideModel * model = resultList[ui->tableView->currentIndex().row()];
    if(action->text().compare("复制")==0){

        QClipboard *clipboard = QApplication::clipboard();  //获取系统剪贴板指针
        QString originalText = clipboard->text();           //获取剪贴板上文本信息

        clipboard->setText(model->magnet);                  //设置剪贴板内容

    }else {
        QDesktopServices::openUrl(QUrl(model->magnet));
    }

}

//搜索按钮点击事件
void MainWindow::on_searchButton_clicked(){

    resultList.clear();

    if(ui->tableView->model() != nullptr){
        delete(ui->tableView->model());
        ui->tableView->setModel(nullptr);
    }
//    qDebug() << "search_row:" << currentListSelect;
    sideModel* object = jsonModelList[currentListSelect];

    if(jsonModelList.length()<=0){
        qDebug() << "源网站对象为空";

        return;
    }

    if(ui->searchText->text().isEmpty()){
        qDebug() << "搜索关键字为空";
        return;
    }
    //显示Loading
    progressCircle->setHidden(false);

    QString string = object->source;//object.value("source").toString();

    QString str_page = string.replace(QRegExp("PPP"), QString::number(currentPage));

    QByteArray keyword_utf8 = ui->searchText->text().toUtf8();
    QByteArray byteArrayPercentEncoded = keyword_utf8.toPercentEncoding();
    QString encode_keyword =  QString(byteArrayPercentEncoded);
    QString url_str = string.replace(QRegExp("XXX"),encode_keyword);
    qDebug()<<"search URL:"<<url_str;
    qDebug()<<"-------";

    webView->setUrl(QUrl(url_str));

//    HttpClient(QUrl(url_str).fromPercentEncoding(url_str.toUtf8())).debug(true).success([this,object](const QString &response) {
//        qDebug() << response;
//        resultList = queryHTML(response,object);

//        reloadTableData(resultList);
//    }).fail([](const QString &error, int errorCode) {
//        qDebug() << error << errorCode;
//    }).get();

}

void MainWindow::htmlFinished(QNetworkReply *reply){

    sideModel * object = jsonModelList[currentListSelect];

    //取出域名判断是不是最后点击的网站
    QString reply_url = reply->url().toString().split(QRegExp("://"))[1].split(QRegExp("/"))[0];
    QString current_url = object->source.split(QRegExp("://"))[1].split(QRegExp("/"))[0];

//    qDebug()<<"reply_url:"<<reply_url;
//    qDebug()<<"current_url:"<<current_url;

    if(reply_url.compare(current_url) == 0){

        QString responseData = reply->readAll();
        qDebug()<<"-------";
//        qDebug()<<"html_URL:"<< reply->url();
        qDebug()<<"htmlFinished:"<< responseData.length();
        qDebug()<<"-------";
        resultList = queryHTML(responseData,object);

        reloadTableData(resultList);
    }else {
        qDebug()<<"不是当前选中,路过结果!";
    }



}

QList<sideModel*> MainWindow::queryHTML(const QString &html, sideModel * model) {

    QList<sideModel*> list;

    QString group = model->group;

    QString query_magnet = group + clearDot(model->magnet);
    QString query_name = group + clearDot(model->name);
    QString query_size = group + clearDot(model->size);
    QString query_count = group + clearDot(model->count);

//    qDebug()<<"magnet:"<<query_magnet;
//    qDebug()<<"name:"<<query_name;
//    qDebug()<<"size:"<<query_size;
//    qDebug()<<"count:"<<query_count;
//    qDebug()<<"-------";

    htmlParserCtxtPtr ctxt = htmlNewParserCtxt();
    htmlDocPtr htmlDoc = htmlCtxtReadMemory(ctxt, html.toUtf8().constData(), (int)strlen(html.toUtf8().constData()), "", NULL, HTML_PARSE_NOBLANKS | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET | HTML_PARSE_NODEFDTD);
    xmlXPathContextPtr context = xmlXPathNewContext ( htmlDoc );

    xmlXPathObjectPtr result_magnet = xmlXPathEvalExpression ((const xmlChar*)query_magnet.toUtf8().constData(), context);
    xmlXPathObjectPtr result_name = xmlXPathEvalExpression ((const xmlChar*)query_name.toUtf8().constData(), context);
    xmlXPathObjectPtr result_size = xmlXPathEvalExpression ((const xmlChar*)query_size.toUtf8().constData(), context);
    xmlXPathObjectPtr result_count = xmlXPathEvalExpression ((const xmlChar*)query_count.toUtf8().constData(), context);

    xmlXPathFreeContext (context);


    if (result_magnet == nullptr) {
        qDebug()<<"Invalid XQuery ?";
    }
    else {
        xmlNodeSetPtr node_magnet = result_magnet->nodesetval;
        xmlNodeSetPtr node_name = result_name->nodesetval;
        xmlNodeSetPtr node_size = result_size->nodesetval;
        xmlNodeSetPtr node_count = result_count->nodesetval;

        if ( !xmlXPathNodeSetIsEmpty ( node_magnet ) ) {
            for (int i = 0; i < node_magnet->nodeNr; i++ ) {
                xmlNodePtr  nodePtr_magnet = node_magnet->nodeTab[i];
                xmlNodePtr  nodePtr_name = node_name->nodeTab[i];
                xmlNodePtr  nodePtr_size = node_size->nodeTab[i];
                xmlNodePtr  nodePtr_count = node_count->nodeTab[i];

                sideModel * result_model = new sideModel();

                QString magnet = QString::fromUtf8((char*)nodePtr_magnet->children->content);
                QString size = QString::fromUtf8((char*)nodePtr_size->children->content);
                QString count = QString::fromUtf8((char*)nodePtr_count->children->content);

                QString name = "";
                if(!QString::fromUtf8((char*)nodePtr_name->children->content).isEmpty()){
                    name = name + QString::fromUtf8((char*)nodePtr_name->children->content);
                }

                if(nodePtr_name->children->next != nullptr){
                    name = name + QString::fromUtf8((char*)nodePtr_name->children->next->content);
                }

                if(!QString::fromUtf8((char*)nodePtr_name->content).isEmpty()){
                    name = name + QString::fromUtf8((char*)nodePtr_name->content);
                }

                if(!QString::fromUtf8((char*)nodePtr_name->last->content).isEmpty()){
                    name = name + QString::fromUtf8((char*)nodePtr_name->last->content);
                }
                if(name.isEmpty()){
                    name = ui->searchText->text();
                }

                result_model->magnet    = clearMagnet(magnet);
                result_model->name      = clearSpace(name);
                result_model->site      = size;
                result_model->count     = count;

//                qDebug()<<"magnet:"<<magnet;
//                qDebug()<<"name:"<<name;
//                qDebug()<<"size:"<<size;
//                qDebug()<<"count:"<<count;
//                qDebug()<<"-------";

                list << result_model;

            }
        }

        xmlXPathFreeObject (result_magnet);
        xmlXPathFreeObject (result_name);
        xmlXPathFreeObject (result_size);
        xmlXPathFreeObject (result_count);
    }

    qDebug()<<"xPath结果:"<<list.length();
    return list;
}
QString MainWindow::clearMagnet(QString str){
    QString clearStr ;

    clearStr = clearSpace(str);

    QString pattern("magnet:?[^\"]+");
    QRegExp rx(pattern);

//    bool match = rx.exactMatch(str);
    if(rx.exactMatch(clearStr)){
        return clearStr;
    }

//    qDebug()<< "magnet:" << clearStr;
    if(clearStr.endsWith(".html")){
        clearStr = clearStr.replace(".html","");
//        qDebug()<< "magnet1:" << clearStr;
    }

    if(rx.exactMatch(clearStr)){
        return clearStr;
    }

    QStringList sections = clearStr.split(QRegExp("&"));
    if(sections.length()>1){
        clearStr = sections[0];
//        qDebug()<< "magnet2:" << clearStr;
    }

    if(rx.exactMatch(clearStr)){
        return clearStr;
    }

    if(clearStr.length()>=40){
        clearStr = "magnet:?xt=urn:btih:" + clearStr.mid(clearStr.length()-40,40);
//        qDebug()<< "magnet3:" << clearStr;
    }

    if(rx.exactMatch(clearStr)){
        return clearStr;
    }

//    qDebug()<< str << ":----:" << clearStr;
    return clearStr;
}
QString MainWindow::clearSpace(QString str){
    QString clearStr ;

    clearStr = str.replace(" ","");

//    qDebug()<< str << ":----:" << clearStr;
    return clearStr;
}

QString MainWindow::clearDot(QString str){
    QString clearStr ;
    if(str.startsWith(".")){
        clearStr = str.mid(1,str.length()-1);
    }else {
        clearStr = str;
    }
//    qDebug()<< str << ":----:" << clearStr;
    return clearStr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reloadTableData(QList<sideModel*>list){

    QStandardItemModel *model = new QStandardItemModel();
    ui->tableView->setModel(model);//来使用model

//    model->setHeaderData(0,Qt::Horizontal,"ID");//设置表的column名称；
    model->setColumnCount(3);    //列

    //添加数据
    for(int j=0;j<list.length();j++)
    {
        sideModel * side = list[j];
         //写id
        QStandardItem *item_magnet = new QStandardItem(side->name);//QString::number(j)));
        model->setItem(j,0,item_magnet);

        QStandardItem *item_size = new QStandardItem(side->site);
        model->setItem(j,1,item_size);

        QStandardItem *item_count = new QStandardItem(side->count);
        model->setItem(j,2,item_count);



    }
    ui->tableView->setColumnWidth(0,ui->tableView->size().width()-162);
    ui->tableView->setColumnWidth(1,80);
    ui->tableView->setColumnWidth(2,80);
    ui->tableView->showMaximized();


    QHeaderView * header = ui->tableView->verticalHeader();//ui->fileTree->header();
//    header->setSectionResizeMode(QHeaderView::ResizeToContents);
//    header->setStretchLastSection(false);
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::Fixed);
    header->setSectionResizeMode(2, QHeaderView::Fixed);
}

void MainWindow::testSlot(){

    qDebug() << "row:" << ui->tableView->currentIndex().row() << "column:" << ui->tableView->currentIndex().column();
//    int row= ui->tableView->currentIndex().row();

//    QAbstractItemModel *model = ui->tableView->model ();

//    QModelIndex index = model->index(row,0);//选中行第一列的内容

//    QVariant data = model->data(index);

}

void MainWindow::clickDownloadButton(){

//    QPushButton *btn = (QPushButton *)sender();   //产生事件的对象
//    int row = btn->property("row").toInt();  //取得按钮的行号属性
//    QString openFileName = btn->property("fileName").toString();  //获取按钮的fileName属性，其他的可以自行添加。

//    sideModel * obj_model = resultList[row];

//    qDebug() << "click_magnet:" << model->magnet;
//    qDebug() << "button_row:"<<row << "row:" << ui->tableView->currentIndex().row() << "column:" << ui->tableView->currentIndex().column();

//    QString URL = "" + obj_model->magnet;
//    qDebug() << "url:" << URL;
//    QDesktopServices::openUrl(QUrl(URL));

}

void MainWindow::on_pushButton_clicked()
{
    QMessageBox message(QMessageBox::NoIcon, "Donate", "");
    QImage image(":/img/donate.png");
    message.setIconPixmap(QPixmap::fromImage(image));
    message.exec();
}


QString MainWindow::getTrackerslist(){
    return "&tr=http://104.238.198.186:8000/announce&tr=http://107.150.14.110:6969/announce&tr=http://109.121.134.121:1337/announce&tr=http://114.55.113.60:6969/announce&tr=http://125.227.35.196:6969/announce&tr=http://128.199.70.66:5944/announce&tr=http://1337.abcvg.info:80/announce&tr=http://157.7.202.64:8080/announce&tr=http://158.69.146.212:7777/announce&tr=http://173.254.204.71:1096/announce&tr=http://178.175.143.27:80/announce&tr=http://178.33.73.26:2710/announce&tr=http://182.176.139.129:6969/announce&tr=http://185.5.97.139:8089/announce&tr=http://185.83.215.123:6969/announce&tr=http://188.165.253.109:1337/announce&tr=http://194.106.216.222:80/announce&tr=http://195.123.209.37:1337/announce&tr=http://210.244.71.25:6969/announce&tr=http://210.244.71.26:6969/announce&tr=http://213.163.67.56:1337/announce&tr=http://37.19.5.139:6969/announce&tr=http://37.19.5.155:6881/announce&tr=http://46.4.109.148:6969/announce&tr=http://5.79.249.77:6969/announce&tr=http://5.79.83.193:2710/announce&tr=http://51.254.244.161:6969/announce&tr=http://51.68.122.172:80/announce&tr=http://59.36.96.77:6969/announce&tr=http://62.210.202.61:80/announce&tr=http://74.82.52.209:6969/announce&tr=http://78.30.254.12:2710/announce&tr=http://80.246.243.18:6969/announce&tr=http://81.200.2.231:80/announce&tr=http://85.17.19.180:80/announce&tr=http://87.110.238.140:6969/announce&tr=http://87.248.186.252:8080/announce&tr=http://87.253.152.137:80/announce&tr=http://91.216.110.47:80/announce&tr=http://91.217.91.21:3218/announce&tr=http://91.218.230.81:6969/announce&tr=http://93.92.64.5:80/announce&tr=http://95.211.168.204:2710/announce&tr=http://[2001:1b10:1000:8101:0:242:ac11:2]:6969/announce&tr=http://[2001:470:1:189:0:1:2:3]:6969/announce&tr=http://[2a04:ac00:1:3dd8::1:2710]:2710/announce&tr=http://aaa.army:8866/announce&tr=http://agusiq-torrents.pl:6969/announce&tr=http://asnet.pw:2710/announce&tr=http://atrack.pow7.com:80/announce&tr=http://bittracker.ru:80/announce&tr=http://bt.henbt.com:2710/announce&tr=http://bt.pusacg.org:8080/announce&tr=http://bt2.careland.com.cn:6969/announce&tr=http://explodie.org:6969/announce&tr=http://fxtt.ru:80/announce&tr=http://grifon.info:80/announce&tr=http://h4.trakx.nibba.trade:80/announce&tr=http://highteahop.top:6960/announce&tr=http://mail2.zelenaya.net:80/announce&tr=http://npupt.com:80/announce.php&tr=http://ns349743.ip-91-121-106.eu:80/announce&tr=http://open.acgnxtracker.com:80/announce&tr=http://open.acgtracker.com:1096/announce&tr=http://open.lolicon.eu:7777/announce&tr=http://open.touki.ru:80/announce.php&tr=http://opentracker.i2p.rocks:6969/announce&tr=http://p4p.arenabg.ch:1337/announce&tr=http://p4p.arenabg.com:1337/announce&tr=http://pow7.com:80/announce&tr=http://pt.lax.mx:80/announce&tr=http://retracker.bashtel.ru:80/announce&tr=http://retracker.gorcomnet.ru:80/announce&tr=http://retracker.hotplug.ru:2710/announce&tr=http://retracker.krs-ix.ru:80/announce&tr=http://retracker.mgts.by:80/announce&tr=http://retracker.ohys.net:80/announce&tr=http://retracker.sevstar.net:2710/announce&tr=http://retracker.spark-rostov.ru:80/announce&tr=http://retracker.telecom.by:80/announce&tr=http://secure.pow7.com:80/announce&tr=http://share.camoe.cn:8080/announce&tr=http://siambit.org:80/announce.php&tr=http://sugoi.pomf.se:80/announce&tr=http://t.acg.rip:6699/announce&tr=http://t.nyaatracker.com:80/announce&tr=http://t.overflow.biz:6969/announce&tr=http://t1.leech.ie:80/announce&tr=http://t1.pow7.com:80/announce&tr=http://t2.leech.ie:80/announce&tr=http://t2.pow7.com:80/announce&tr=http://t3.leech.ie:80/announce&tr=http://theheartofchronic.com:80/announce.php&tr=http://thetracker.org:80/announce&tr=http://torrent.gresille.org:80/announce&tr=http://torrentsmd.com:8080/announce&tr=http://torrentsmd.eu:8080/announce&tr=http://torrentsmd.me:8080/announce&tr=http://tr.cili001.com:8070/announce&tr=http://tr.kxmp.cf:80/announce&tr=http://tracker.acgnx.se:80/announce&tr=http://tracker.aletorrenty.pl:2710/announce&tr=http://tracker.baravik.org:6970/announce&tr=http://tracker.birkenwald.de:6969/announce&tr=http://tracker.bittor.pw:1337/announce&tr=http://tracker.bittorrent.am:80/announce&tr=http://tracker.bittorrent.am:80/announce.php&tr=http://tracker.bt4g.com:2095/announce&tr=http://tracker.bz:80/announce&tr=http://tracker.calculate.ru:6969/announce&tr=http://tracker.city9x.com:2710/announce&tr=http://tracker.corpscorp.online:80/announce&tr=http://tracker.devil-torrents.pl:80/announce&tr=http://tracker.dler.org:6969/announce&tr=http://tracker.dutchtracking.com:80/announce&tr=http://tracker.dutchtracking.nl:80/announce&tr=http://tracker.edoardocolombo.eu:6969/announce&tr=http://tracker.electro-torrent.pl:80/announce&tr=http://tracker.ex.ua:80/announce&tr=http://tracker.files.fm:6969/announce&tr=http://tracker.filetracker.pl:8089/announce&tr=http://tracker.flashtorrents.org:6969/announce&tr=http://tracker.gbitt.info:80/announce&tr=http://tracker.gcvchp.com:2710/announce&tr=http://tracker.grepler.com:6969/announce&tr=http://tracker.internetwarriors.net:1337/announce&tr=http://tracker.ipv6tracker.ru:80/announce&tr=http://tracker.kamigami.org:2710/announce&tr=http://tracker.kicks-ass.net:80/announce&tr=http://tracker.kisssub.org:80/announce&tr=http://tracker.kuroy.me:5944/announce&tr=http://tracker.lelux.fi:80/announce&tr=http://tracker.mg64.net:6881/announce&tr=http://tracker.moeking.me:6969/announce&tr=http://tracker.nyaa.uk:6969/announce&tr=http://tracker.nyap2p.com:8080/announce&tr=http://tracker.opentrackr.org:1337/announce&tr=http://tracker.skyts.net:6969/announce&tr=http://tracker.tfile.co:80/announce&tr=http://tracker.tfile.me:80/announce&tr=http://tracker.tiny-vps.com:6969/announce&tr=http://tracker.torrentbay.to:6969/announce&tr=http://tracker.torrentyorg.pl:80/announce&tr=http://tracker.trackerfix.com:80/announce&tr=http://tracker.tvunderground.org.ru:3218/announce&tr=http://tracker.uw0.xyz:6969/announce&tr=http://tracker.ygsub.com:6969/announce&tr=http://tracker.yoshi210.com:6969/announce&tr=http://tracker.zerobytes.xyz:1337/announce&tr=http://tracker01.loveapp.com:6789/announce&tr=http://tracker1.itzmx.com:8080/announce&tr=http://tracker1.torrentino.com:80/announce&tr=http://tracker1.wasabii.com.tw:6969/announce&tr=http://tracker2.dler.org:80/announce&tr=http://tracker2.itzmx.com:6961/announce&tr=http://tracker2.wasabii.com.tw:6969/announce&tr=http://tracker3.itzmx.com:6961/announce&tr=http://tracker4.itzmx.com:2710/announce&tr=http://tracker9.bol.bg:80/announce.php&tr=http://trun.tom.ru:80/announce&tr=http://vps02.net.orel.ru:80/announce&tr=http://www.loushao.net:8080/announce&tr=http://www.proxmox.com:6969/announce&tr=http://www.wareztorrent.com:80/announce&tr=https://1337.abcvg.info:443/announce&tr=https://2.tracker.eu.org:443/announce&tr=https://3.tracker.eu.org:443/announce&tr=https://aaa.army:8866/announce&tr=https://open.kickasstracker.com:443/announce&tr=https://opentracker.acgnx.se:443/announce&tr=https://t.quic.ws:443/announce&tr=https://t1.leech.ie:443/announce&tr=https://t2.leech.ie:443/announce&tr=https://t3.leech.ie:443/announce&tr=https://tk.mabo.ltd:443/announce&tr=https://tracker.bt-hash.com:443/announce&tr=https://tracker.fastdownload.xyz:443/announce&tr=https://tracker.gbitt.info:443/announce&tr=https://tracker.hama3.net:443/announce&tr=https://tracker.lelux.fi:443/announce&tr=https://tracker.nanoha.org:443/announce&tr=https://tracker.nitrix.me:443/announce&tr=https://tracker.opentracker.se:443/announce&tr=https://tracker.parrotlinux.org:443/announce&tr=https://tracker.sloppyta.co:443/announce&tr=https://tracker.tamersunion.org:443/announce&tr=https://tracker.vectahosting.eu:2053/announce&tr=https://www.wareztorrent.com:443/announce&tr=udp://104.238.198.186:8000/announce&tr=udp://107.150.14.110:6969/announce&tr=udp://109.121.134.121:1337/announce&tr=udp://114.55.113.60:6969/announce&tr=udp://128.199.70.66:5944/announce&tr=udp://151.80.120.114:2710/announce&tr=udp://168.235.67.63:6969/announce&tr=udp://178.33.73.26:2710/announce&tr=udp://182.176.139.129:6969/announce&tr=udp://185.5.97.139:8089/announce&tr=udp://185.83.215.123:6969/announce&tr=udp://185.86.149.205:1337/announce&tr=udp://188.165.253.109:1337/announce&tr=udp://191.101.229.236:1337/announce&tr=udp://194.106.216.222:80/announce&tr=udp://195.123.209.37:1337/announce&tr=udp://195.123.209.40:80/announce&tr=udp://208.67.16.113:8000/announce&tr=udp://212.1.226.176:2710/announce&tr=udp://212.47.227.58:6969/announce&tr=udp://213.163.67.56:1337/announce&tr=udp://37.19.5.155:2710/announce&tr=udp://46.4.109.148:6969/announce&tr=udp://5.79.249.77:6969/announce&tr=udp://5.79.83.193:6969/announce&tr=udp://51.254.244.161:6969/announce&tr=udp://52.58.128.163:6969/announce&tr=udp://62.138.0.158:6969/announce&tr=udp://62.212.85.66:2710/announce&tr=udp://74.82.52.209:6969/announce&tr=udp://78.30.254.12:2710/announce&tr=udp://85.17.19.180:80/announce&tr=udp://89.234.156.205:80/announce&tr=udp://9.rarbg.com:2710/announce&tr=udp://9.rarbg.me:2710/announce&tr=udp://9.rarbg.me:2780/announce&tr=udp://9.rarbg.to:2710/announce&tr=udp://9.rarbg.to:2730/announce&tr=udp://91.216.110.52:451/announce&tr=udp://91.218.230.81:6969/announce&tr=udp://94.23.183.33:6969/announce&tr=udp://95.211.168.204:2710/announce&tr=udp://[2001:1b10:1000:8101:0:242:ac11:2]:6969/announce&tr=udp://[2001:470:1:189:0:1:2:3]:6969/announce&tr=udp://[2a03:7220:8083:cd00::1]:451/announce&tr=udp://[2a04:ac00:1:3dd8::1:2710]:2710/announce&tr=udp://[2a04:c44:e00:32e0:4cf:6aff:fe00:aa1]:6969/announce&tr=udp://aaa.army:8866/announce&tr=udp://bt.aoeex.com:8000/announce&tr=udp://bt.delbertbeta.cc:19943/announce&tr=udp://bt.okmp3.ru:2710/announce&tr=udp://bt.xxx-tracker.com:2710/announce&tr=udp://bt1.archive.org:6969/announce&tr=udp://bt2.54new.com:8080/announce&tr=udp://bt2.archive.org:6969/announce&tr=udp://chihaya.de:6969/announce&tr=udp://chihaya.toss.li:9696/announce&tr=udp://denis.stalker.upeer.me:6969/announce&tr=udp://eddie4.nl:6969/announce&tr=udp://exodus.desync.com:6969/announce&tr=udp://inferno.demonoid.pw:3418/announce&tr=udp://ipv4.tracker.harry.lu:80/announce&tr=udp://ipv6.tracker.harry.lu:80/announce&tr=udp://ipv6.tracker.zerobytes.xyz:16661/announce&tr=udp://mgtracker.org:2710/announce&tr=udp://open.demonii.com:1337/announce&tr=udp://open.demonii.si:1337/announce&tr=udp://open.nyap2p.com:6969/announce&tr=udp://open.stealth.si:80/announce&tr=udp://opentor.org:2710/announce&tr=udp://opentracker.i2p.rocks:6969/announce&tr=udp://peerfect.org:6969/announce&tr=udp://public.popcorn-tracker.org:6969/announce&tr=udp://qg.lorzl.gq:2710/announce&tr=udp://retracker.akado-ural.ru:80/announce&tr=udp://retracker.baikal-telecom.net:2710/announce&tr=udp://retracker.hotplug.ru:2710/announce&tr=udp://retracker.lanta-net.ru:2710/announce&tr=udp://retracker.netbynet.ru:2710/announce&tr=udp://retracker.nts.su:2710/announce&tr=udp://retracker.sevstar.net:2710/announce&tr=udp://santost12.xyz:6969/announce&tr=udp://shadowshq.eddie4.nl:6969/announce&tr=udp://shadowshq.yi.org:6969/announce&tr=udp://tc.animereactor.ru:8082/announce&tr=udp://thetracker.org:80/announce&tr=udp://torrent.gresille.org:80/announce&tr=udp://torrentclub.tech:6969/announce&tr=udp://tr.bangumi.moe:6969/announce&tr=udp://tr.cili001.com:8070/announce&tr=udp://tracker-udp.gbitt.info:80/announce&tr=udp://tracker.0o.is:6969/announce&tr=udp://tracker.aletorrenty.pl:2710/announce&tr=udp://tracker.army:6969/announce&tr=udp://tracker.beeimg.com:6969/announce&tr=udp://tracker.birkenwald.de:6969/announce&tr=udp://tracker.bittor.pw:1337/announce&tr=udp://tracker.coppersurfer.tk:6969/announce&tr=udp://tracker.cyberia.is:6969/announce&tr=udp://tracker.cypherpunks.ru:6969/announce&tr=udp://tracker.dler.org:6969/announce&tr=udp://tracker.doko.moe:6969/announce&tr=udp://tracker.ds.is:6969/announce&tr=udp://tracker.dutchtracking.com:6969/announce&tr=udp://tracker.eddie4.nl:6969/announce&tr=udp://tracker.ex.ua:80/announce&tr=udp://tracker.filemail.com:6969/announce&tr=udp://tracker.filepit.to:6969/announce&tr=udp://tracker.filetracker.pl:8089/announce&tr=udp://tracker.flashtorrents.org:6969/announce&tr=udp://tracker.grepler.com:6969/announce&tr=udp://tracker.halfchub.club:6969/announce&tr=udp://tracker.iamhansen.xyz:2000/announce&tr=udp://tracker.ilibr.org:80/announce&tr=udp://tracker.internetwarriors.net:1337/announce&tr=udp://tracker.justseed.it:1337/announce&tr=udp://tracker.kamigami.org:2710/announce&tr=udp://tracker.kicks-ass.net:80/announce&tr=udp://tracker.kuroy.me:5944/announce&tr=udp://tracker.leechers-paradise.org:6969/announce&tr=udp://tracker.lelux.fi:6969/announce&tr=udp://tracker.moeking.me:6969/announce&tr=udp://tracker.msm8916.com:6969/announce&tr=udp://tracker.nyaa.uk:6969/announce&tr=udp://tracker.open-internet.nl:6969/announce&tr=udp://tracker.openbittorrent.com:80/announce&tr=udp://tracker.opentrackr.org:1337/announce&tr=udp://tracker.piratepublic.com:1337/announce&tr=udp://tracker.sbsub.com:2710/announce&tr=udp://tracker.skyts.net:6969/announce&tr=udp://tracker.swateam.org.uk:2710/announce&tr=udp://tracker.teambelgium.net:6969/announce&tr=udp://tracker.tiny-vps.com:6969/announce&tr=udp://tracker.torrent.eu.org:451/announce&tr=udp://tracker.torrentbay.to:6969/announce&tr=udp://tracker.tvunderground.org.ru:3218/announce&tr=udp://tracker.uw0.xyz:6969/announce&tr=udp://tracker.vanitycore.co:6969/announce&tr=udp://tracker.yoshi210.com:6969/announce&tr=udp://tracker.zerobytes.xyz:1337/announce&tr=udp://tracker.zum.bi:6969/announce&tr=udp://tracker1.itzmx.com:8080/announce&tr=udp://tracker2.christianbro.pw:6969/announce&tr=udp://tracker2.indowebster.com:6969/announce&tr=udp://tracker2.itzmx.com:6961/announce&tr=udp://tracker3.itzmx.com:6961/announce&tr=udp://tracker4.itzmx.com:2710/announce&tr=udp://tracker4.piratux.com:6969/announce&tr=udp://valakas.rollo.dnsabr.com:2710/announce&tr=udp://wambo.club:1337/announce&tr=udp://www.loushao.net:8080/announce&tr=udp://xxxtor.com:2710/announce&tr=udp://z.mercax.com:53/announce&tr=udp://zephir.monocul.us:6969/announce&tr=udp://zer0day.ch:1337/announce&tr=udp://zer0day.to:1337/announce";
}
