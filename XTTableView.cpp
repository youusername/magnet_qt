#include "XTTableView.h"
#include <QDebug>
#include <QtDebug>
#include "mainwindow.h"

XTTableView::~XTTableView()
{

}
XTTableView::XTTableView(QWidget *parent)
    : QTableView(parent)
{

    this->installEventFilter(this);
}

void XTTableView::mousePressEvent(QMouseEvent * event)
{
     setCurrentIndex(QModelIndex());//对当前索引的初始化，否则点击无效区域时会记录上一次的有效值
     QTableView::mousePressEvent(event);
     QModelIndex index = currentIndex();
     if(index.row() < 0 && index.column() < 0)//如果点击表格空白处直接返回
         return;

     if (event->button() == Qt::LeftButton)//鼠标左键
     {
//         qDebug() << "leftClicked";

         emit leftClicked(index);

     }else{//鼠标右键

//         qDebug() << "rightClicked";
         emit rightClicked(index);
     }
}
