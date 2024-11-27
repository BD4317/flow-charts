#include "operation.h"
#include "view.h"

Operation::Operation(QObject *parent) : QObject(parent)
{}

// 视图移动操作的构造函数
ViewMoveOperation:: ViewMoveOperation(QPointF oldPosition, QPointF newPosition, QObject* parent)
   : Operation(parent), oldPosition(oldPosition), newPosition(newPosition)
{}

// 撤销视图移动操作
void  ViewMoveOperation::undo() const
{
    if (this->parent() != nullptr)
    {
        View* view = qobject_cast<View*>(this->parent());
        if (view != nullptr)
        {
            QPointF disPointF = oldPosition - newPosition;  // 计算移动距离
            view->scene()->setSceneRect(view->scene()->sceneRect().x() - disPointF.x(),
                                        view->scene()->sceneRect().y() - disPointF.y(),
                                        view->scene()->sceneRect().width(),
                                        view->scene()->sceneRect().height());
            view->scene()->update();  // 更新场景
        }
    }
}

// 执行视图移动操作
void  ViewMoveOperation::redo() const
{
    if (this->parent() != nullptr)
    {
        View* view = qobject_cast<View*>(this->parent());
        if (view != nullptr)
        {
            QPointF disPointF = newPosition - oldPosition;  // 计算移动距离
            view->scene()->setSceneRect(view->scene()->sceneRect().x() - disPointF.x(),
                                        view->scene()->sceneRect().y() - disPointF.y(),
                                        view->scene()->sceneRect().width(),
                                        view->scene()->sceneRect().height());
            view->scene()->update();  // 更新场景
        }
    }
}

// 缩放操作的构造方法
ScaleOperation::ScaleOperation(int delta, double multiple, QPointF position, QObject* parent)
    : Operation(parent), delta(delta), multiple(multiple), position(position)
{}

// 撤销缩放操作
void ScaleOperation::undo() const
{
    if (this->parent() != nullptr)
    {
        View* view = qobject_cast<View*>(this->parent());
        if (!(delta > 0))
        {
            view->scaleMultiple = view->scaleMultiple * multiple;
            view->scaleByWheel(multiple, multiple, position);
        }
        else
        {
            view->scaleMultiple = view->scaleMultiple / multiple;
            view->scaleByWheel(1.0 / multiple, 1.0 / multiple, position);
        }
        emit view->scaleMultipleChanged(view->scaleMultiple);
    }
}

// 执行缩放操作
void ScaleOperation::redo() const
{
    if (this->parent() != nullptr)
    {
        View* view = qobject_cast<View*>(this->parent());
        if (delta > 0)
        {
            view->scaleMultiple = view->scaleMultiple * multiple;
            view->scaleByWheel(multiple, multiple, position);
        }
        else
        {
            view->scaleMultiple = view->scaleMultiple / multiple;
            view->scaleByWheel(1.0 / multiple, 1.0 / multiple, position);  
        }
        emit view->scaleMultipleChanged(view->scaleMultiple);
    }
}

// 添加操作的构造
AppendOperation::AppendOperation(QList<QGraphicsItem*> addToItems, QObject* parent)
    : Operation(parent), addToItems(addToItems)
{}

// 撤销添加操作
void AppendOperation::undo() const
{
    if (this->parent() != nullptr)
    {
        View* view = qobject_cast<View*>(this->parent());
        for (QGraphicsItem* item : qAsConst(addToItems))
        {
            if (item->type() == TextItem::Type)
            {
                view->graphicsScene->allTexts.removeAll(qgraphicsitem_cast<TextItem*>(item));
            }
            if (item->type() == LineItem::Type)
            {
                view->graphicsScene->allLines.removeAll(qgraphicsitem_cast<LineItem*>(item));
            }
            view->graphicsScene->removeItem(item);
        }
    }
}

// 执行添加操作
void AppendOperation::redo() const
{
    if (this->parent() != nullptr)
    {
        View* view = qobject_cast<View*>(this->parent());
        for (QGraphicsItem* item : qAsConst(addToItems))
        {
            if (item->type() == TextItem::Type)
            {
                view->graphicsScene->allTexts.append(qgraphicsitem_cast<TextItem*>(item));
            }
            if (item->type() == LineItem::Type)
            {
                view->graphicsScene->allLines.append(qgraphicsitem_cast<LineItem*>(item));
            }
            view->graphicsScene->addItem(item);
        }
    }
}

// 删除操作的构造函数
DeleteOperation::DeleteOperation(QList<QGraphicsItem*> addToItems, QObject* parent)
    : AppendOperation(addToItems, parent), addToItems(addToItems)
{}

// 撤销删除操作
void DeleteOperation::undo() const
{
    AppendOperation::redo();  // 调用基类的执行方法
}

// 执行删除操作
void DeleteOperation::redo() const
{
    AppendOperation::undo();  // 调用基类的撤销方法
}


// 移动操作的构造函数
MoveOperation::MoveOperation(QPointF startPosition, QPointF endPosition, QList<QGraphicsItem*> items, QObject* parent)
    : Operation(parent), moveItems(items), startPosition(startPosition), endPosition(endPosition)
{}

// 撤销移动操作
void MoveOperation::undo() const
{
    if (this->parent() != nullptr)
    {
        QPointF disPointF = endPosition - startPosition;
        for (QGraphicsItem* item : qAsConst(moveItems))
        {
            if (item->type() == TextItem::Type && (qgraphicsitem_cast<TextItem*>(item))->connectItem == nullptr)
            {
                item->setPos(item->pos() - disPointF);
            }
            else if (item->type() == ChartItem::Type)
            {
                item->setPos(item->pos() - disPointF);
            }
            else if (item->type() == PixmapItem::Type)
            {
                item->setPos(item->pos() - disPointF);
            }
        }
    }
}

// 执行移动操作
void MoveOperation::redo() const
{
    if (this->parent() != nullptr)
    {
        QPointF disPointF = endPosition - startPosition;
        for (QGraphicsItem* item : qAsConst(moveItems))
        {
            if (item->type() == TextItem::Type && (qgraphicsitem_cast<TextItem*>(item))->connectItem == nullptr)
            {
                item->setPos(item->pos() + disPointF);
            }
            else if (item->type() == ChartItem::Type)
            {
                item->setPos(item->pos() + disPointF);
            }
            else if (item->type() == PixmapItem::Type)
            {
                item->setPos(item->pos() + disPointF);
            }
        }
    }
}

// 大小变换操作的构造函数
ChangeOperation::ChangeOperation(QTransform oldTransform, QTransform newTransform, QGraphicsItem* item, QObject* parent)
    : Operation(parent), item(item), oldTransform(oldTransform), newTransform(newTransform)
{}

// 撤销变换操作
void ChangeOperation::undo() const
{
    if (item != nullptr)
    {
        item->setTransform(oldTransform);   // 恢复旧的变换
        item->update();                     // 更新图形项
        if (item->scene() != nullptr && item->scene() != nullptr)
        {
            item->scene()->update();        // 更新场景
        }

    }
}

// 执行变换操作
void ChangeOperation::redo() const
{
    if (item != nullptr)
    {
        item->setTransform(newTransform);   // 设置新的变换
        item->update();                     // 更新图形项
        if (item->scene() != nullptr && item->scene() != nullptr)
        {
            item->scene()->update();        // 更新场景
        }

    }
}

// 涂色操作的构造函数
ColorOperation::ColorOperation(QList<QPair<QString, QString>> fillColors, QList<QPair<QString, QString>> borderColors, QList<QGraphicsItem*> items, QObject* parent)
    : Operation(parent), fillColors(fillColors), borderColors(borderColors), items(items)
{}

// 撤销涂色操作
void ColorOperation::undo() const
{
    for (int i = 0; i < items.count(); ++i)
    {
        ChartItem* chartItem = qgraphicsitem_cast<ChartItem*>(items[i]);
        chartItem->setCurrentFillColor(fillColors[i].first);
        chartItem->setCurrentBorderColor(borderColors[i].first);
        chartItem->setSharedRenderer(chartItem->svgRender);
    }
}

// 执行涂色操纵
void ColorOperation::redo() const
{
    for (int i = 0; i < items.count(); ++i)
    {
        ChartItem* chartItem = qgraphicsitem_cast<ChartItem*>(items[i]);
        chartItem->setCurrentFillColor(fillColors[i].second);
        chartItem->setCurrentBorderColor(borderColors[i].second);
        chartItem->setSharedRenderer(chartItem->svgRender);
    }
}

// 文本替换操作的构造函数
ReplacceTextOperation::ReplacceTextOperation(QString oldText, QString newText, QList<QGraphicsItem*> changedItems, QObject* parent)
    : Operation(parent),  oldText(oldText), newText(newText), changedItems(changedItems)
{}

// 撤销替换操作
void ReplacceTextOperation::undo() const
{
    for (QGraphicsItem* item : changedItems)
    {
        TextItem* textItem = qgraphicsitem_cast<TextItem*>(item);
        textItem->setPlainText(oldText);
    }
}

// 执行替换操作
void ReplacceTextOperation::redo() const
{
    for (QGraphicsItem* item : changedItems)
    {
        TextItem* textItem = qgraphicsitem_cast<TextItem*>(item);
        textItem->setPlainText(newText);
    }
}

// 文本样式更改操作的构造函数
ChangeFontOperation::ChangeFontOperation(QFont oldFont, QColor oldColor, QFont newFont, QColor newColor, QList<QGraphicsItem*> changedItems, QObject* parent)
    : Operation(parent), oldFont(oldFont), oldColor(oldColor), newFont(newFont), newColor(newColor), changedItems(changedItems)
{}

// 撤销更改操作
void ChangeFontOperation::undo() const
{
    for (QGraphicsItem* item : changedItems)
    {
        TextItem* textItem = qgraphicsitem_cast<TextItem*>(item);
        textItem->setFont(oldFont);
        textItem->setDefaultTextColor(oldColor);
    }
}

// 执行更改操作
void ChangeFontOperation::redo() const
{
    for (QGraphicsItem* item : changedItems)
    {
        TextItem* textItem = qgraphicsitem_cast<TextItem*>(item);
        textItem->setFont(newFont);
        textItem->setDefaultTextColor(newColor);
    }
}


AppendBackgroundOperation::AppendBackgroundOperation(QBrush oldBackground, QBrush newBackground, QObject* parent)
    : Operation(parent), oldBackground(oldBackground), newBackground(newBackground)
{}

void AppendBackgroundOperation::undo() const
{
    View* view = qobject_cast<View*>(this->parent());
    view->graphicsScene->setBackgroundBrush(oldBackground);
    view->graphicsScene->update();
    view->update();
}

void AppendBackgroundOperation::redo() const
{
    View* view = qobject_cast<View*>(this->parent());
    view->graphicsScene->setBackgroundBrush(newBackground);
    view->graphicsScene->update();
    view->update();
}

ChangeRectOperation::ChangeRectOperation(QRectF oldRect, QRectF newRect, QGraphicsItem* item, QObject* parent)
    : Operation(parent), oldRect(oldRect), newRect(newRect), item(item)
{}

void ChangeRectOperation::undo() const
{
    PixmapItem* pixmapItem = qgraphicsitem_cast<PixmapItem*>(item);
    pixmapItem->changeRect(oldRect);
}

void ChangeRectOperation::redo() const
{
    PixmapItem* pixmapItem = qgraphicsitem_cast<PixmapItem*>(item);
    pixmapItem->changeRect(newRect);
}
