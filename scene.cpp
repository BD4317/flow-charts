#include "scene.h"
#include "view.h"

Scene::Scene(QObject* parent)
    : QGraphicsScene(parent),
      mode(NoMode),
      lineItem(nullptr),
      chartItem(nullptr),
      textItem(nullptr),
      pixmapItem(nullptr),
      isMove(false),
      shiftIsClicked(false)
{}

Scene::~Scene()
{
    items().clear();
}

void Scene::saveSceneToImage(QString filename)
{
    this->clearSelection();                                             // 取消选中
    QPixmap pixSaveImage(int(this->width()), int(this->height()));      // 根据当前画布大小创建一个等大的Pixmap
    pixSaveImage.fill(QColor("#ffffff"));                               // 用白色填充图像背景
    QPainter painterTanns(&pixSaveImage);                               // 创建一个QPainter对象，用于绘制场景内容
    painterTanns.setRenderHint(QPainter::Antialiasing, true);           // 设置反锯齿
    painterTanns.setRenderHint(QPainter::TextAntialiasing, true);       // 设置文本反锯齿
    painterTanns.setRenderHint(QPainter::SmoothPixmapTransform, true);  // 设置平滑像素变换
    this->render(&painterTanns);                                        // 将场景内容渲染到QPixmap对象上
    pixSaveImage.save(filename);                                        // 保存图像
}

void Scene::saveSceneToSVG(QString filename)
{
    this->clearSelection();                                             // 取消选中
    QSvgGenerator generator;                                            // 创建一个QSvgGenerator对象
    generator.setFileName(filename);                                    // 设置SVG文件的名称
    generator.setSize(QSize(int(this->width()), int(this->height())));  // 设置SVG图像的尺寸
    generator.setTitle("Scene SVG");                                    // 设置SVG文件的标题（可选）
    generator.setDescription("A scene exported to SVG");                // 设置SVG文件的描述（可选）
    QPainter painter;                                                   // 创建一个QPainter对象
    painter.begin(&generator);                                          // 与QSvgGenerator关联
    painter.setRenderHint(QPainter::Antialiasing, true);                // 设置反锯齿
    painter.setRenderHint(QPainter::TextAntialiasing, true);            // 设置文本反锯齿
    this->render(&painter);                                             // 将场景内容渲染到SVG生成器中
    painter.end();                                                      // 结束绘制
}

// 移除场景中选中的项的方法
void Scene::removeAllSelect(QList<QGraphicsItem *> items)
{
    // 定义要移除的图形项列表
    QList<ChartItem*> removeCharts;
    QList<TextItem*> removeTexts;
    QList<LineItem*> removeLines;

    for (QGraphicsItem* item : qAsConst(items))
    {
        switch (item->type())
        {
            case QGraphicsProxyWidget::Type :                                                   // 如果是代理控件，不能移除
                break;
            case ChartItem::Type :                                                 // 如果是图形，还需要删除相关的线和文本
            {
                QList<TextItem*> connectText = getConnectText(item);               // 获取关联的文本
                for (TextItem* text : qAsConst(connectText))
                {
                    allTexts.removeAll(text);                                                   // 从关联文本列表中移除
                }
                QList<LineItem*> connectLine = getConnectLine(item);         // 获取关联的线
                for (LineItem* line : qAsConst(connectLine))
                {
                    allLines.removeAll(line);                                                   // 从关联连接线列表中移除
                }
                removeTexts.append(connectText);                                                // 添加到要移除的文本列表
                removeLines.append(connectLine);                                                // 添加到要移除的线列表
                removeCharts.append(qgraphicsitem_cast<ChartItem*>(item));         // 添加到移除列表
                break;
            }
            case LineItem::Type :                                            // 如果是线，还需要删除相关的文本
            {
                QList<TextItem*> connectText = getConnectText(item);
                for (TextItem* text : qAsConst(connectText))
                {
                    allTexts.removeAll(text);                                                   // 从关联文本列表中移除
                }
                removeTexts.append(connectText);                                                // 添加到要移除的文本项列表
                if (!removeLines.contains(qgraphicsitem_cast<LineItem*>(item)))
                {
                    removeLines.append(qgraphicsitem_cast<LineItem*>(item)); // 添加到移除列表
                }
                break;
            }
            case TextItem::Type :
            {
                allTexts.removeAll(qgraphicsitem_cast<TextItem *>(item));          // 从关联文本列表中移除
                if (!removeTexts.contains(qgraphicsitem_cast<TextItem *>(item)))
                {
                    removeTexts.append(qgraphicsitem_cast<TextItem *>(item));      // 添加到移除列表
                }
                break;
            }
            default:
                break;
        }
    }

    // 移除
    QList<QGraphicsItem *> allMovedItems;
    for (ChartItem* chart : qAsConst(removeCharts))
    {
        allMovedItems.append(chart);
        this->removeItem(chart);
    }
    for (TextItem* text : qAsConst(removeTexts))
    {
        allMovedItems.append(text);
        this->removeItem(text);
    }
    for (LineItem* line : qAsConst(removeLines))
    {
        allMovedItems.append(line);
        this->removeItem(line);
    }

    // 将删除操作记录到命令栈中，用于撤销操作
    (qobject_cast<View*>(this->parent()))->operationStack->addOperation(new DeleteOperation(allMovedItems, this->parent()));
}

QList<TextItem*> Scene::getConnectText(QGraphicsItem* item)
{
    QList<TextItem*> connectText;                  // 保存关联的文本
    if (item->type() == ChartItem::Type)           // 如果是图形
    {
        ChartItem* chart = qgraphicsitem_cast<ChartItem*>(item);
        if (chart != nullptr)
        {
            for (TextItem* text : qAsConst(allTexts))
            {
                if (qgraphicsitem_cast<ChartItem*>(text->connectItem) == chart)
                {
                    connectText.append(text);                   // 添加到返回列表
                }
            }
        }
    }
    else if (item->type() == LineItem::Type) // 如果是线
    {
        LineItem* line = qgraphicsitem_cast<LineItem*>(item);
        if (line != nullptr)
        {
            for (TextItem* text : qAsConst(allTexts))
            {
                if (qgraphicsitem_cast<LineItem*>(text->connectItem) == line)
                    connectText.append(text);
            }
        }
    }
    return connectText;
}

QList<LineItem*> Scene::getConnectLine(QGraphicsItem* item)
{
    QList<LineItem*> connectLine;    // 保存关联的线

    if (item->type() == ChartItem::Type)   // 如果是图形
    {
        for (LineItem* line : qAsConst(allLines))
        {
            if (line->startItem == item || line->endItem == item)
            {
                connectLine.append(line);              // 添加到返回列表
            }
        }
    }
    return connectLine;
}

void Scene::setMode(Mode mode)
{
    this->mode = mode;                                                  // 设置模式
    QGraphicsView* view = dynamic_cast<QGraphicsView*>(this->parent()); // 获取视图

    switch (mode)
    {
        case InsertLine:                                                // 插入线模式
        {
            view->viewport()->setCursor(Qt::CrossCursor);               // 设置光标为十字形
            view->setDragMode(QGraphicsView::NoDrag);                   // 禁用拖动模式
            break;
        }
        case InsertText:                                                // 插入文本模式
        {
            view->viewport()->setCursor(Qt::IBeamCursor);               // 设置光标为I形光标
            view->setDragMode(QGraphicsView::NoDrag);                   // 禁用拖动模式
            break;
        }
        default:  // 默认模式
        {
            view->viewport()->setCursor(Qt::ArrowCursor);               // 设置光标为箭头
            view->setDragMode(QGraphicsView::RubberBandDrag);           // 设置拖动模式为橡皮筋拖动
            break;
        }
    }
}

void Scene::doubleClickItem()
{
    LineItem* line = qobject_cast<LineItem*>(sender());
    if (line != nullptr)
    {
        for (TextItem* text : qAsConst(allTexts))
        {
            if (qgraphicsitem_cast<LineItem*>(text->connectItem) == line)
            {
                text->setTextEditFlags(Qt::TextEditorInteraction);          // 设置文本编辑标志
                text->setFocus();                                           // 设置焦点到文本项
                return;
            }
        }
        // 如果没有找到关联的文本项，则添加一个新的文本项
        TextItem* text = new TextItem();
        text->setConnectItem(line);                                         // 设置关联文本
        text->setFlag(QGraphicsItem::ItemIsMovable, false);                 // 禁用移动
        text->setFlag(QGraphicsItem::ItemIsSelectable, true);               // 启用选择
        text->setFlag(QGraphicsRectItem::ItemSendsGeometryChanges, false);  // 禁用几何变更信号
        addItem(text);                                                      // 添加到场景中
        connect(line, &LineItem::itemPositionHasChanged, text, &TextItem::parentPositionHasChanged);
        allTexts.append(text);                                              // 添加到关联文本列表
        text->setTextEditFlags(Qt::TextEditorInteraction);                  // 设置文本编辑标志
        text->setFocus();                                                   // 设置焦点

        // 将添加操作记录到命令栈中，用于撤销操作
        (qobject_cast<View*>(this->parent()))->operationStack->addOperation(new AppendOperation(QList<QGraphicsItem*>() << text, this->parent()));
    }
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mousePressEvent(event);                                                 // 调用父类的鼠标按下事件处理
    if (event->button() != Qt::LeftButton)                                                  // 只处理左键事件
    {
        return;
    }
    switch (mode)
    {
        case InsertChart :                                                                  // 插入图形模式
        {
            View* view = qobject_cast<View*>(this->parent());
            view->addChartItem(flowType, event->scenePos());
            break;
        }
        case InsertLine :                                                                   // 插入线模式
        {
            lineItem = new QGraphicsLineItem(QLineF(event->scenePos(), event->scenePos())); // 创建新的连接线项
            lineItem->setPen(QPen(QColor("#7596D7"), 2));                                   // 设置画笔颜色和宽度
            addItem(lineItem);                                                              // 添加到场景中
            break;
        }
        case InsertText:  // 插入文本模式
        {
            TextItem* textItem = new TextItem();  // 创建新的文本项
            textItem->setPos(event->scenePos());  // 设置文本项位置

            addItem(textItem);  // 添加到场景中
            allTexts.append(textItem);  // 添加到文本列表

            // 将添加操作记录到命令栈中，用于撤销操作
            (qobject_cast<View*>(this->parent()))->operationStack->addOperation(new AppendOperation(QList<QGraphicsItem*>() << textItem, this->parent()));
            break;
        }

        case NoMode:                                                                        // 无模式
        {
            QList<QGraphicsItem*> startItems = items(event->scenePos());                    // 获取鼠标按下位置的所有图形项
            for (QGraphicsItem* item : qAsConst(startItems))
            {
                if (item->type() == ChartItem::Type && selectedItems().contains(item))
                {
                    chartItem = qgraphicsitem_cast<ChartItem*>(item);
                    startPosition = chartItem->pos();                                       // 记录起始位置
                    isMove = true;                                                     // 标记为单个移动项
                    QRectF viewRect = qobject_cast<View*>(this->parent())->rect();          // 获取视图的矩形范围
                    QPointF leftTop = qobject_cast<View*>(this->parent())->mapToScene(int(viewRect.x()), int(viewRect.y()));
                    QPointF rightBottom = qobject_cast<View*>(this->parent())->mapToScene(int(viewRect.width()), int(viewRect.height()));
                    QList<QGraphicsItem*> viewItems = items(QRectF(leftTop.x(), leftTop.y(), rightBottom.x() - leftTop.x(), rightBottom.y() - leftTop.y()));
                    for (QGraphicsItem* rectItem : qAsConst(viewItems))
                    {
                        if (rectItem->type() == ChartItem::Type && !selectedItems().contains(rectItem))
                        {
                            QRectF rects = rectItem->boundingRegion(rectItem->transform()).boundingRect();
                            QPen pen = QPen(QColor("#000000"), 0.5, Qt::DashLine);
                            // 选中框的左线
                            QGraphicsLineItem* leftLine = new QGraphicsLineItem(rectItem->pos().x(), leftTop.y(), rectItem->pos().x(), rightBottom.y());
                            leftLine->setPen(pen);
                            leftLine->setData(Qt::UserRole + 1, QPointF(rectItem->pos().x(), rectItem->pos().y() + rects.height() / 2));
                            leftLine->hide();
                            addItem(leftLine);
                            verticalLine.append(leftLine);
                            // 选中框的右线
                            QGraphicsLineItem* rightLine = new QGraphicsLineItem(rectItem->pos().x() + rects.width(), leftTop.y(), rectItem->pos().x() + rects.width(), rightBottom.y());
                            rightLine->setPen(pen);
                            rightLine->setData(Qt::UserRole + 1, QPointF(rectItem->pos().x() + rects.width(), rectItem->pos().y() + rects.height() / 2));
                            rightLine->hide();
                            addItem(rightLine);
                            verticalLine.append(rightLine);
                            // 选中框的上线
                            QGraphicsLineItem* topLine = new QGraphicsLineItem(leftTop.x(), rectItem->pos().y(), rightBottom.x(), rectItem->pos().y());
                            topLine->setPen(pen);
                            topLine->setData(Qt::UserRole + 1, QPointF(rectItem->pos().x() + rects.width() / 2, rectItem->pos().y()));
                            topLine->hide();
                            addItem(topLine);
                            horizontalLine.append(topLine);
                            // 选中框的下线
                            QGraphicsLineItem* bottomLine = new QGraphicsLineItem(leftTop.x(), rectItem->pos().y() + rects.height(), rightBottom.x(), rectItem->pos().y() + rects.height());
                            bottomLine->setPen(pen);
                            bottomLine->setData(Qt::UserRole + 1, QPointF(rectItem->pos().x() + rects.width() / 2, rectItem->pos().y() + rects.height()));
                            bottomLine->hide();
                            addItem(bottomLine);
                            horizontalLine.append(bottomLine);
                        }
                    }
                    break;
                }
                else if (item->type() == TextItem::Type && selectedItems().contains(item))
                {
                    textItem = qgraphicsitem_cast<TextItem*>(item);
                    startPosition = textItem->pos();
                    isMove = true;
                }
                else if (item->type() == PixmapItem::Type && selectedItems().contains(item))
                {
                    pixmapItem = qgraphicsitem_cast<PixmapItem*>(item);
                    startPosition = pixmapItem->pos();
                    isMove = true;
                }
            }
            break;
        }
        default:
            break;
    }
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (mode == InsertLine && lineItem != nullptr)
    {
        QLineF newLine(lineItem->line().p1(), event->scenePos());   // 更新连接线的位置
        lineItem->setLine(newLine);
    }
    else
    {
        QGraphicsScene::mouseMoveEvent(event);                      // 调用父类的鼠标移动事件处理
        if (mode == NoMode && isMove && chartItem != nullptr)
        {
            // 隐藏所有的辅助对齐线
            for (QGraphicsLineItem* item : qAsConst(horizontalLine))
            {
                item->hide();
            }
            for (QGraphicsLineItem* item : qAsConst(verticalLine))
            {
                item->hide();
            }
            // 获取临近的对齐线并显示
            QRectF chartRect = chartItem->boundingRegion(chartItem->transform()).boundingRect();
            QPointF center = chartItem->pos() + QPoint(int(chartRect.width() / 2), int(chartRect.height() / 2));
            double horizontalDistance = 11;  // 左右对齐线的距离差
            for (QGraphicsLineItem* item : qAsConst(verticalLine))
            {
                double dis = qAbs(center.x() - item->line().p1().x()) - chartRect.width() / 2;
                if (dis >= 0 && dis <= 10 && distance(item->data(Qt::UserRole + 1).toPointF(), center) < chartRect.width() / 2 + 200)
                {
                    item->show();
                    if (dis < qAbs(horizontalDistance))
                    {
                        horizontalDistance = (center.x() >= item->line().p1().x() ? -1 : 1) * dis;
                    }
                }
            }

            double verticallDistance = 11;  // 上下对齐线的距离差
            for (QGraphicsLineItem* item : qAsConst(horizontalLine))
            {
                double dis = qAbs(center.y() - item->line().p1().y()) - chartRect.height() / 2;
                if (dis >= 0 && dis <= 10 && distance(item->data(Qt::UserRole + 1).toPointF(), center) < chartRect.width() / 2 + 200)
                {
                    item->show();
                    if (dis < qAbs(verticallDistance))
                    {
                        verticallDistance = (center.y() >= item->line().p1().y() ? -1 : 1) * dis;
                    }
                }
            }

            // 更新图形项的位置
            endPosition = chartItem->pos();
            QList<QGraphicsItem*> selectLists = selectedItems();
            for (QGraphicsItem* item : qAsConst(selectLists))
            {
                if (item->type() == LineItem::Type)
                {
                    continue;
                }
                item->setPos(item->pos().x() + (horizontalDistance <= 10 ? horizontalDistance : 0), item->pos().y() + (verticallDistance <= 10 ? verticallDistance : 0));
            }
        }
    }
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (lineItem != nullptr && mode == InsertLine)
    {
        QList<QGraphicsItem *> startItems = items(lineItem->line().p1());
        QList<QGraphicsItem *> endItems = items(lineItem->line().p2());
        // 在起始的图形标记线的起始点
        ChartItem* startItem = nullptr;
        for (QGraphicsItem *topItem : startItems)
        {
            if (topItem->type() == ChartItem::Type)
            {
                startItem = qgraphicsitem_cast<ChartItem*>(topItem);
                break;
            }
        }
        // 在结束的图形标记线的结束点
        ChartItem* endItem = nullptr;
        for (QGraphicsItem* topItem : endItems)
        {
            if (topItem->type() == ChartItem::Type)
            {
                endItem = qgraphicsitem_cast<ChartItem*>(topItem);
                break;
            }
        }
        // 起始点，终点均不为空，且起始点和终点不是同一图形
        if (startItem != nullptr && endItem != nullptr && startItem != endItem)
        {
            LineItem* line = new LineItem(startItem, endItem);
            allLines.append(line);
            addItem(line);
            connect(line, &LineItem::doubleClickItem, this, &Scene::doubleClickItem);

            // 将添加操作记录到命令栈中，用于撤销操作
            (qobject_cast<View*>(this->parent()))->operationStack->addOperation(new AppendOperation(QList<QGraphicsItem*>() << line, this->parent()));
        }
        // // 清空线条
        removeItem(lineItem);
        delete lineItem;
        lineItem = nullptr;
        if (!shiftIsClicked)
        {
            setMode(NoMode);
        }
    }
    else if (mode == InsertText || mode == InsertChart)
    {
        setMode(NoMode);
    }
    else if (mode == NoMode && isMove)
    {
        if (chartItem)
        {
            endPosition = chartItem->pos();
            isMove = false;
            chartItem = nullptr;
            // 清空边框
            for (QGraphicsItem* item : qAsConst(horizontalLine))
            {
                removeItem(item);
            }
            for (QGraphicsItem* item : qAsConst(verticalLine))
            {
                removeItem(item);
            }
            verticalLine.clear();
            horizontalLine.clear();
        }
        else if (textItem)
        {
            endPosition = textItem->pos();
            isMove = false;
            textItem = nullptr;
        }
        else if (pixmapItem)
        {
            endPosition = pixmapItem->pos();
            isMove = false;
            pixmapItem = nullptr;
        }
        // 起始位置不同则记录
        if (!(startPosition == event->scenePos() || startPosition == endPosition))
        {
            (qobject_cast<View*>(this->parent()))->operationStack->addOperation(new MoveOperation(startPosition, endPosition, selectedItems(), this->parent()));
        }
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

void Scene::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == (Qt::ShiftModifier))  // 按下Shift键
    {
        shiftIsClicked = true;
        setMode(InsertLine);
    }
    else if (event->key() == (Qt::Key_Delete))      // 按下Delete键
    {
        if (selectedItems().count() != 0)
        {
            removeAllSelect(selectedItems());
        }
    }
    QGraphicsScene::keyPressEvent(event);
}

void Scene::keyReleaseEvent(QKeyEvent *event)
{
    if (shiftIsClicked)
    {   // 移除创建的临时线
        if (lineItem != nullptr)
        {
            removeItem(lineItem);
            delete lineItem;
            lineItem = nullptr;
        }
        shiftIsClicked = false;
        setMode(NoMode);
    }
    QGraphicsScene::keyReleaseEvent(event);
}

double Scene::distance(QPointF pos1, QPointF pos2)
{
    double distance = sqrt(pow((pos1.x() - pos2.x()), 2) + pow(pos1.y() - pos2.y(), 2));
    return distance;
}
Mode Scene::getMode() const
{
    return mode;
}

void Scene::clearAllItems()
{
    // 清空 `allTexts` 和 `allLines` 列表
    allTexts.clear();
    allLines.clear();

    // 获取场景中的所有图形项
    QList<QGraphicsItem*> allItems = items();

    // 从场景中移除并销毁所有图形项
    for (QGraphicsItem* item : allItems)
    {
        removeItem(item);
        delete item;  // 删除图形项以释放内存
    }
}
