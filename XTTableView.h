#ifndef XTTABLEVIEW_H
#define XTTABLEVIEW_H

#include <QObject>
#include <QWidget>
#include "QTableView"
#include <QMainWindow>
#include <QMouseEvent>

class XTTableView : public QTableView
{
         Q_OBJECT
public:

    XTTableView(QWidget *parent = 0);
    ~XTTableView();

signals:
     void leftClicked(const QModelIndex &);//点击左键信号
     void rightClicked(const QModelIndex &);//点击右键信号

protected:
    void mousePressEvent(QMouseEvent *event);//函数重写

};

#endif // XTTABLEVIEW_H
