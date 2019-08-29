#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "XTTableView.h"
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void clickDownloadButton();
    void testSlot();
    void clicked_rightMenu(const QPoint &pos);  //右键信号槽函数
    void onShowOrHideColumn(QAction *action);
private:
    Ui::MainWindow *ui;

    QMenu *rightMenu;  //右键菜单
    QAction *cutAction;  //剪切
    QAction *copyAction;  //复制
    QAction *pasteAction;  //粘贴
    QAction *deleteAction;  //删除
};

#endif // MAINWINDOW_H
