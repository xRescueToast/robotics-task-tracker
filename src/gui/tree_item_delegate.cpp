#include "tree_item_delegate.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include "tree_roles.h"

//icon size and spacing in pixels
static constexpr int kIconSize = 13;
static constexpr int kIconGap = 5;
//extra room at the right edge so the trash hover box is not clipped
static constexpr int kEdgePad = 3;
//hover box is inset this far from the row's top and bottom
static constexpr int kBoxInset = 1;
//corner radius of the hover box
static constexpr int kBoxRadius = 3;

//faint highlight behind a hovered icon
static const QColor kIconHoverColor(255, 255, 255, 40);

TreeItemDelegate::TreeItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent),
      m_pencilIcon(":/icons/pencil.svg"),
      m_trashIcon(":/icons/trash.svg") {
    //watch the tree's viewport so we know when the mouse leaves it
    if (auto* view = qobject_cast<QAbstractItemView*>(parent)) {
        view->viewport()->installEventFilter(this);
    }
}

void TreeItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    //only hovered mission and task rows get icons
    if (!(option.state & QStyle::State_MouseOver) || !hasIcons(index)) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    //fill in the row's text, colors and state, then pick the style to draw with
    QStyleOptionViewItem rowOption(option);
    initStyleOption(&rowOption, index);
    QStyle* style = rowOption.widget ? rowOption.widget->style() : QApplication::style();

    //draw the full-width row background with no text
    QString text = rowOption.text;
    rowOption.text.clear();
    style->drawControl(QStyle::CE_ItemViewItem, &rowOption, painter, rowOption.widget);

    //draw the text ourselves, ending before the pencil and shortened with ... if needed
    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &rowOption, rowOption.widget);
    textRect.setRight(pencilRect(option.rect).left() - kIconGap);
    QString shown = rowOption.fontMetrics.elidedText(text, Qt::ElideRight, textRect.width());
    bool enabled = rowOption.state & QStyle::State_Enabled;
    //normal text color, the windows 11 style keeps it on selected rows too
    style->drawItemText(painter, textRect, rowOption.displayAlignment, rowOption.palette, enabled, shown, QPalette::Text);

    //highlight the icon under the mouse
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(kIconHoverColor);
    for (const QRect& iconRect : {pencilRect(option.rect), trashRect(option.rect)}) {
        if (iconRect.contains(m_mousePos)) {
            //square box as tall as the row minus the inset, centered on the icon
            int side = option.rect.height() - kBoxInset * 2;
            QRect box(0, 0, side, side);
            box.moveCenter(iconRect.center());
            painter->drawRoundedRect(QRectF(box), kBoxRadius, kBoxRadius);
        }
    }
    painter->restore();

    //draw the icons in the space we kept free
    m_pencilIcon.paint(painter, pencilRect(option.rect));
    m_trashIcon.paint(painter, trashRect(option.rect));
}

//watches mouse movement so the icon highlight and cursor follow the mouse
bool TreeItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) {
    if (event->type() == QEvent::MouseMove) {
        //remember where the mouse is
        m_mousePos = static_cast<QMouseEvent*>(event)->position().toPoint();
        auto* view = qobject_cast<QAbstractItemView*>(parent());
        if (view) {
            //pointing hand over an icon, default cursor everywhere else
            bool overIcon = pencilRect(option.rect).contains(m_mousePos) || trashRect(option.rect).contains(m_mousePos);
            if (overIcon && hasIcons(index)) {
                view->viewport()->setCursor(Qt::PointingHandCursor);
            } else {
                view->viewport()->unsetCursor();
            }
            //repaint just this row so the highlight follows the mouse
            view->viewport()->update(option.rect);
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

//clears the hover state when the mouse leaves the tree
bool TreeItemDelegate::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::Leave) {
        m_mousePos = QPoint(-1, -1);
        if (auto* viewport = qobject_cast<QWidget*>(watched)) {
            viewport->unsetCursor();
            viewport->update();
        }
    }
    //never swallow the event, the tree still needs it
    return QStyledItemDelegate::eventFilter(watched, event);
}

//true for mission and task rows, false for action rows like + Add task
bool TreeItemDelegate::hasIcons(const QModelIndex& index) const {
    QVariant kind = index.data(kKindRole);
    if (!kind.isValid()) {
        return false;
    }
    auto rowKind = static_cast<TreeRowKind>(kind.toInt());
    return rowKind == TreeRowKind::Mission || rowKind == TreeRowKind::Task;
}

//trash sits at the far right, centered vertically
QRect TreeItemDelegate::trashRect(const QRect& row) const {
    return QRect(row.right() - kIconSize - kIconGap - kEdgePad, row.center().y() - kIconSize / 2, kIconSize, kIconSize);
}

//pencil sits just left of the trash
QRect TreeItemDelegate::pencilRect(const QRect& row) const {
    return trashRect(row).translated(-(kIconSize + kIconGap), 0);
}