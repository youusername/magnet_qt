#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QTableView"
#include "QStandardItemModel"
#include <QDebug>
#include <QtDebug>
#include "XTTableView.h"
#include <QMenu>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    QPushButton *okButton = new QPushButton("text");
//    QPushButton *cancelButton = new QPushButton("Cancel");
//    ui->siteListLayout->addWidget(okButton);
//    ui->siteListLayout->addWidget(cancelButton);





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
//    model->setRowCount(10);    //行



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
void MainWindow::clicked_rightMenu(const QPoint &pos)  //右键信号槽函数
{
    rightMenu->exec(QCursor::pos());

    qDebug() << "";
}
void MainWindow::onShowOrHideColumn(QAction *action)
{
    qDebug() << action;
    // 稍后会添加该函数的实现代码。
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








