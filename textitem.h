#ifndef TEXTITEM_H
#define TEXTITEM_H

#include <QGraphicsTextItem>
#include <QTextDocument>
#include <QUuid>

#include "lineitem.h"

class TextItem: public QGraphicsTextItem
{
    Q_OBJECT
public:
    TextItem(QGraphicsItem* parent = nullptr);

    QString text;                                                                   // 文本内容
    QGraphicsItem* connectItem;                                                     // 关联对象
    QString Uid;                                                                    // 唯一标识符

    enum { Type = UserType + 4 };                                                   // 类型标识

    int type() const override;
    void setTextEditFlags(Qt::TextInteractionFlags flags);                          // 设置文本交互标志
    void setConnectItem(QGraphicsItem* item);                                       // 设置关联图形
    void updatePosition();                                                          // 位置更新改变

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override; // 图形更改事件
    void focusOutEvent(QFocusEvent* event) override;                                // 失去光标事件
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;           // 鼠标双击事件

public slots:
    void parentPositionHasChanged();                                                // 更新位置

};

#endif // TEXTITEM_H
