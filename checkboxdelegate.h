#ifndef CHECKDELEGATE_H
#define CHECKDELEGATE_H

#include <QStyledItemDelegate>
#include <QtGui>

class CheckBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    CheckBoxDelegate(QObject *parent = 0);
protected:
    void paint(QPainter* painter,const QStyleOptionViewItem& option,const QModelIndex& index) const;
    bool editorEvent(QEvent *event,QAbstractItemModel *model,const QStyleOptionViewItem &option,const QModelIndex &index);
};


#endif // CHECKDELEGATE_H

