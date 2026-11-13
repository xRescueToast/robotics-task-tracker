#pragma once

#include <QStyledItemDelegate>
#include <QIcon>
#include <QPoint>

class TreeItemDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit TreeItemDelegate(QObject* parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    bool hasIcons(const QModelIndex& index) const;
    QRect trashRect(const QRect& row) const;
    QRect pencilRect(const QRect& row) const;

    QIcon m_pencilIcon;
    QIcon m_trashIcon;
    //last mouse position inside the tree, -1,-1 means nowhere
    QPoint m_mousePos{-1, -1};
};