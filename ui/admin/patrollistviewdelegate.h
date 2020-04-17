#ifndef PATROLLISTVIEWDELEGATE_H
#define PATROLLISTVIEWDELEGATE_H

#include <QAbstractItemDelegate>

class QPainter;

class PatrolListViewDelegate
        : public QAbstractItemDelegate
{
public:
    PatrolListViewDelegate(QObject *parent = 0);
    virtual ~PatrolListViewDelegate();
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};


#endif // PATROLLISTVIEWDELEGATE_H
