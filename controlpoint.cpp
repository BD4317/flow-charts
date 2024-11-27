#include "controlpoint.h"
#include "view.h"

ControlPoint::ControlPoint(RectDirection rectdirection, QGraphicsItem* parent)
    : QGraphicsRectItem(parent), rectdirection(rectdirection), isCheck(false)
{
    this->setAcceptHoverEvents(true);                                                       // 接受悬停事件
    setFlag(QGraphicsRectItem::ItemIsMovable, true);                                        // 设置图形项可移动
    setFlag(QGraphicsRectItem::ItemIsSelectable, false);                                    // 设置图形项不可选中
    setFlag(QGraphicsRectItem::ItemSendsGeometryChanges, false);                            // 不发送几何变更事件

    QRectF parentrect = parent->boundingRect();
    // 获取父项的边界矩形

    // 根据控制点的位置类型设置矩形的初始位置和大小
    switch (rectdirection)
    {
        case RectDirection::LeftTop:
            setRect(0, 0, 1.5, 1.5);                                                        // 左上角
            break;
        case RectDirection::LeftBtn:
            setRect(0, parentrect.height() - 1.5, 1.5, 1.5);                                // 左下角
            break;
        case RectDirection::RightTop:
            setRect(parentrect.width() - 1.5, 0, 1.5, 1.5);                                 // 右上角
            break;
        case RectDirection::RightBtn:
            setRect(parentrect.width() - 1.5, parentrect.height() - 1.5, 1.5, 1.5);         // 右下角
            break;
        case RectDirection::BottomCenter:
            setRect(parentrect.width() / 2 - 0.75, parentrect.height() - 1.5, 1.5, 1.5);    // 下边框中间
            break;
    }

    hide();                                                                                 // 初始状态下隐藏控制点
}

int ControlPoint::type() const
{
    return Type;
}

void ControlPoint::setParentItemTransform(QPointF postop, QPointF posbtn)
{
    this->prepareGeometryChange();
    QPointF posdiff = posbtn - postop;
    QRectF rect = this->parentItem()->boundingRect();
    QTransform oldtran = this->parentItem()->transform();
    QTransform tran = this->parentItem()->transform();

    if (rectdirection == BottomCenter)
    {
        // 旋转逻辑
        QPointF center = rect.center();
        qreal angle = QLineF(center, posbtn).angleTo(QLineF(center, postop));
        tran.translate(center.x(), center.y());
        tran.rotate(angle);
        tran.translate(-center.x(), -center.y());
    } else
    {
        // 缩放逻辑
        switch (rectdirection)
        {
            case RectDirection::LeftTop:
                if (posbtn.x() > (rect.x() + rect.width() - 5) || posbtn.y() > (rect.y() + rect.height() - 5))
                {
                    return;
                }
                tran.translate(posdiff.x(), posdiff.y());
                posdiff = QPointF(-1 * posdiff.x(), -1 * posdiff.y());
                break;
            case RectDirection::LeftBtn:
                if (posbtn.x() > (rect.x() + rect.width() - 5) || posbtn.y() < (rect.y() + 5))
                {
                    return;
                }
                tran.translate(posdiff.x(), 0);
                posdiff = QPointF(-1 * posdiff.x(), posdiff.y());
                break;
            case RectDirection::RightTop:
                if (posbtn.x() < (rect.x() + 5) || posbtn.y() > (rect.y() + rect.height() - 5))
                {
                    return;
                }
                tran.translate(0, posdiff.y());
                posdiff = QPointF(posdiff.x(), -1 * posdiff.y());
                break;
            case RectDirection::RightBtn:
                if (posbtn.x() < (rect.x() + 5) || posbtn.y() < (rect.y() + 5))
                {
                    return;
                }
                break;
            default:
                break;
        }

        double dx = (rect.width() + posdiff.x()) / rect.width();
        double dy = (rect.height() + posdiff.y()) / rect.height();

        if (dx == 1.0 && dy == 1.0)
        {
            return;
        }
        tran.scale(dx, dy);
    }

    this->parentItem()->setTransform(tran);
    this->update();

    (qobject_cast<View*>(this->parentItem()->scene()->parent()))->operationStack->addOperation(new ChangeOperation(oldtran, tran, this->parentItem()));
}

QRectF ControlPoint::boundingRect() const
{
    return shape().boundingRect();  // 返回形状的边界矩形
}

QPainterPath ControlPoint::shape() const
{
    QPainterPath path;
    QRectF currentRect = rect();  // 获取当前矩形的大小和位置
    QPolygonF poly;

    // 根据控制点的位置类型创建多边形形状
    switch (rectdirection)
    {
        case RectDirection::LeftTop:
        {
            QRectF ellipseRect(currentRect.x(), currentRect.y(), currentRect.width(), currentRect.height());
            path.addEllipse(ellipseRect);
            break;
        }
        case RectDirection::LeftBtn:
        {
            QRectF ellipseRect(currentRect.x(), currentRect.y(), currentRect.width(), currentRect.height());
            path.addEllipse(ellipseRect);
            break;
        }
        case RectDirection::RightTop:
        {
            QRectF ellipseRect(currentRect.x(), currentRect.y(), currentRect.width(), currentRect.height());
            path.addEllipse(ellipseRect);
            break;
        }
        case RectDirection::RightBtn:
        {
            QRectF ellipseRect(currentRect.x(), currentRect.y(), currentRect.width(), currentRect.height());
            path.addEllipse(ellipseRect);
            break;
        }
        case RectDirection::BottomCenter:
        {
            QRectF ellipseRect(currentRect.x(), currentRect.y(), currentRect.width(), currentRect.height());
            path.addEllipse(ellipseRect);
            break;
        }
    }
    return path;  // 返回多边形形状
}

void ControlPoint::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->save();
    painter->setOpacity(0.8);       // 设置透明度
    painter->setPen(Qt::NoPen);     // 无边框
    QRectF _rect = rect();          // 获取当前矩形的大小和位置

    // 根据控制点的位置类型绘制多边形形状
    switch (rectdirection)
    {
        case RectDirection::LeftTop:
        {
            QPainterPath path;
            QRectF ellipseRect(_rect.x(), _rect.y(), _rect.width(), _rect.height());

            // 绘制圆形
            path.addEllipse(ellipseRect);
            painter->fillPath(path, QBrush(QColor("#517bcc")));  // 填充多边形
            break;
        }
        case RectDirection::LeftBtn:
        {
            QPainterPath path;
            QRectF ellipseRect(_rect.x(), _rect.y(), _rect.width(), _rect.height());

            // 绘制圆形
            path.addEllipse(ellipseRect);
            painter->fillPath(path, QBrush(QColor("#517bcc")));
            break;
        }
        case RectDirection::RightTop:
        {
            QPainterPath path;
            QRectF ellipseRect(_rect.x(), _rect.y(), _rect.width(), _rect.height());

            // 绘制圆形
            path.addEllipse(ellipseRect);
            painter->fillPath(path, QBrush(QColor("#517bcc")));
            break;
        }
        case RectDirection::RightBtn:
        {
            QPainterPath path;
            QRectF ellipseRect(_rect.x(), _rect.y(), _rect.width(), _rect.height());

            // 绘制圆形
            path.addEllipse(ellipseRect);
            painter->fillPath(path, QBrush(QColor("#517bcc")));
            break;
        }
        //绘制控制点
        case RectDirection::BottomCenter:
        {
            // 绘制同心圆
            QPainterPath path;
            QRectF outerEllipseRect(_rect.x(), _rect.y(), _rect.width(), _rect.height());
            QRectF innerEllipseRect(_rect.x() + 0.3, _rect.y() + 0.3, _rect.width() - 0.6, _rect.height() - 0.6);

            path.addEllipse(outerEllipseRect);
            path.addEllipse(innerEllipseRect);

            painter->fillPath(path, QBrush(QColor("#517bcc")));  // 填充多边形
            break;
        }
    }
    painter->restore();                                         // 恢复绘图状态
}

void ControlPoint::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
    this->setFocus();                           // 设置焦点
    // 根据控制点位置设置鼠标光标形状
    if (rectdirection == LeftTop || rectdirection == RightBtn)
    {
        this->setCursor(Qt::SizeFDiagCursor);   // 设置为对角调整光标
    }
    else if (rectdirection == LeftBtn || rectdirection == RightTop)
    {
        this->setCursor(Qt::SizeBDiagCursor);   // 设置为反对角调整光标
    }
    else
    {
        this->setCursor(Qt::SizeVerCursor);     //设置为垂直调整光标
    }

    QGraphicsRectItem::hoverEnterEvent(e);      // 调用基类处理
}

void ControlPoint::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
    this->setCursor(Qt::ArrowCursor);           // 恢复鼠标光标为箭头
    QGraphicsRectItem::hoverLeaveEvent(e);      // 调用基类处理
}

void ControlPoint::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        isCheck = true;                 // 标记鼠标已按下
        clickPosition = event->pos();   // 记录鼠标按下的位置
    }
}

void ControlPoint::mouseMoveEvent(QGraphicsSceneMouseEvent*)
{
    // 空防止图片在缩放过程中移动
}

void ControlPoint::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (isCheck)
    {
        setParentItemTransform(clickPosition, event->pos());    // 调整父项的变换
        clickPosition = event->pos();                           // 更新按下的位置
    }
}
