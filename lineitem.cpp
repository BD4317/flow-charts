#include "lineitem.h"

#include <QDebug>

LineItem::LineItem(ChartItem* startItem, ChartItem* endItem, QGraphicsItem* parent)
    : QGraphicsLineItem(parent), startItem(startItem), endItem(endItem)
{
    this->setAcceptHoverEvents(true);                                                       // 接受悬停事件
    setFlag(QGraphicsLineItem::ItemIsSelectable, true);                                     // 设置图形项可选中
    setFlag(QGraphicsLineItem::ItemSendsGeometryChanges, true);                             // 设置几何变更事件
    Uid = QUuid::createUuid().toString(QUuid::WithoutBraces);                               // 生成唯一ID
    color = Qt::black;                                                                      // 设置颜色
    setPen(QPen(color, 2));                                                                 // 设置画笔
    connect(startItem, &ChartItem::itemPositionHasChanged, this, [&](){ this->update(); }); // 更新图形项
    connect(endItem, &ChartItem::itemPositionHasChanged, this, [&](){ this->update(); });   // 更新图形项
    setZValue(99);                                                                          // 设置Z值
    QLineF line(mapFromItem(startItem, 0, 0), mapFromItem(endItem, 0, 0));                  // 计算初始线段
    setLine(line);                                                                          // 设置线段
}

int LineItem::type() const
{
    return Type;
}

QPointF LineItem::getBoundedIntersection(ChartItem* chartItem, QLineF centerline)
{
    QPolygonF framePoints;
    framePoints << QPoint(0, 0) << QPoint(0, 160) << QPoint(160, 160) << QPoint(160, 0) << QPoint(0, 0);
    QPointF p1 = framePoints.first() + chartItem->pos();                                        // 第一个点
    QPointF p2;
    QPointF intersectPoint;
    QLineF polyLine;
    bool ishave = false;
    for (int i = 1; i < framePoints.count(); ++i)
    {
        p2 = framePoints.at(i) + chartItem->pos();                                              // 下一个点
        polyLine = QLineF(p1, p2);  // 构建边线
        QLineF::IntersectType intersectType = polyLine.intersect(centerline, &intersectPoint);  // 计算交点
        if (intersectType == QLineF::BoundedIntersection)
        {
            ishave = true;
            break;
        }
        p1 = p2;
    }
    if (ishave)
    {
        return intersectPoint;
    }
    else
    {
        return  chartItem->pos();
    }
}

QRectF LineItem::boundingRect() const
{
    return shape().boundingRect();
}

QPainterPath LineItem::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape(); // 获取基类形状
    path.addPolygon(arrowHead);                     // 添加箭头形状
    return path;
}

void LineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (startItem->collidesWithItem(endItem))
    {
        return;
    }
    QPen myPen = pen();
    myPen.setColor(color);
    qreal arrowSize = 10;
    painter->setPen(myPen);
    painter->setBrush(color);

    QLineF centerLine(QPoint(80, 80) + startItem->pos(), QPoint(80, 80) + endItem->pos());

    QPointF startpoint = getBoundedIntersection(startItem, centerLine);
    QPointF endpoint = getBoundedIntersection(endItem, centerLine);

    if (lines != QLineF(startpoint, endpoint))
    {
        lines = QLineF(startpoint, endpoint);
        emit itemPositionHasChanged();
    }
    setLine(lines);

    double angle = std::atan2(-line().dy(), line().dx());

    QPointF arrowP1 = line().p2() + QPointF(sin(angle - M_PI / 3) * arrowSize,
                                            cos(angle - M_PI / 3) * arrowSize);
    QPointF arrowP2 = line().p2() + QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize,
                                            cos(angle - M_PI + M_PI / 3) * arrowSize);

    arrowHead.clear();
    arrowHead << line().p2() << arrowP1 << arrowP2;

    painter->drawPolygon(arrowHead);
    if (isSelected())
    {
        painter->setPen(QPen(color, 3, Qt::DotLine));
    }
    painter->drawLine(line());
}

void LineItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsLineItem::mouseDoubleClickEvent(event);    // 调用基类的鼠标双击事件处理
    emit doubleClickItem();                             // 发射双击信号
}
