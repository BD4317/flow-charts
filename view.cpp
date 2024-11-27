#include "view.h"

#include <QDebug>

View::View(QWidget *parent)
    : QGraphicsView(parent), scaleMultiple(1.0), isMoveView(false)
{
    setRenderHint(QPainter::Antialiasing);              // 启用反锯齿渲染
    setCacheMode(QGraphicsView::CacheBackground);       // 设置缓存模式为背景缓存
    graphicsScene = new Scene(this);                    // 创建自定义场景对象
    graphicsScene->setSceneRect(this->rect());          // 设置场景的矩形区域
    setScene(graphicsScene);                            // 将场景设置为当前视图的场景
    setStyleSheet("padding: 0px; border: 0px;");        // 设置视图的样式表
    setAlignment(Qt::AlignVCenter | Qt::AlignTop);      // 设置视图对齐方式为垂直居中和顶部对齐
    this->setAcceptDrops(true);                         // 启用拖放操作

    operationStack = new OperationStack(parent);        // 创建操作栈，用于撤销和重做操作

    // 当操作栈的操作数量发生改变时，判断撤销和重做按钮是否依然可用
    connect(operationStack, &OperationStack::countChange, this, &View::buttonChange);

    // 设置快捷键Ctrl+Z用于撤销
    /*QShortcut* undoCut = new QShortcut(QKeySequence(tr("Ctrl+Z")), this, nullptr, nullptr, Qt::ApplicationShortcut);
    connect(undoCut, &QShortcut::activated, this, [&]() { operationStack->undo(); });

    // 设置快捷键Ctrl+Y用于重做
    QShortcut* redoCut = new QShortcut(QKeySequence(tr("Ctrl+Y")), this, nullptr, nullptr, Qt::ApplicationShortcut);
    connect(redoCut, &QShortcut::activated, this, [&]() { operationStack-> redo(); });*/

    // 设置快捷键Esc用于退出文本编辑模式
    QShortcut* outTextCut = new QShortcut(QKeySequence("Esc"), this, nullptr, nullptr, Qt::ApplicationShortcut);
    connect(outTextCut, &QShortcut::activated, this, &View::outTextEdit);
}


void View::scaleByWheel(double scaleX, double scaleY, QPointF position)
{
    QPoint nowPosition = QPoint(int(position.x()), int(position.y()));      // 获取当前的鼠标所在的view坐标
    QPointF nowScenePosition = this->mapToScene(nowPosition);               // 转化为当前鼠标相对于scene的位置
    scale(scaleX, scaleY);                                                  // 缩放视图
    scene()->setSceneRect(this->mapToScene(this->rect()).boundingRect());   // 调整scene，使得scene和view一致
    QPointF scenePosition = this->mapToScene(nowPosition);                  // 获取缩放后的scene坐标
    QPointF positionChanged = scenePosition - nowScenePosition;             // 获取缩放前后的坐标差值，即为需要进行move的位移
    // 调整位置
    double newX = scene()->sceneRect().x() - positionChanged.x();
    double newY = scene()->sceneRect().y() - positionChanged.y();
    scene()->setSceneRect(newX, newY, scene()->sceneRect().width(), scene()->sceneRect().height());
    scene()->update();                                                      // 更新场景
}

void View::addChartItem(FlowEnumItem type, QPointF position)
{
    ChartItem* item = new ChartItem(type);                                  // 创建流程图形
    graphicsScene->addItem(item);                                           // 将图形添加到场景中
    TextItem* textItem = new TextItem();                                    // 创建文本项
    textItem->setPlainText(item->text);
    textItem->text = item->text;
    textItem->setConnectItem(item);                                         // 设置关联文本
    // 设置文本项的属性
    textItem->setFlag(QGraphicsItem::ItemIsMovable, false);                 // 禁用移动
    textItem->setFlag(QGraphicsItem::ItemIsSelectable, true);               // 启用选择
    textItem->setFlag(QGraphicsRectItem::ItemSendsGeometryChanges, false);  // 禁用几何变更信号
    graphicsScene->addItem(textItem);                                        // 将文本项添加到场景中
    // 连接图形项位置变化信号
    connect(item, &ChartItem::itemPositionHasChanged, textItem, &TextItem::parentPositionHasChanged);
    graphicsScene->allTexts.append(textItem);                                // 添加到文本列表
    item->setPos(position);                                                  // 设置流程图项的位置
    // 将添加操作记录到命令栈中，用于撤销操作
    this->operationStack->addOperation(new AppendOperation(QList<QGraphicsItem*>() << item << textItem, this));
}


void View::setScene(Scene *scene)
{
    graphicsScene = scene;
    QGraphicsView::setScene(scene);

    // 创建撤销和重做按钮
    undoButton = new QPushButton();                                                             // 创建按钮
    undoButton->setMaximumWidth(5000);                                                          // 设置最大宽度
    undoButton->setObjectName("undoOperation");                                                 // 设置名称
    undoButton->setStyleSheet("background-color:transparent;font-size:14pt;");                  // 设置格式
    undoButton->setFlat(true);                                                                  // 淡化按钮边框
    connect(undoButton, &QPushButton::clicked, this, [&]() {operationStack->undo(); });         // 点击按钮的操作
    undoButton->setToolTip(tr("撤销"));                                                          // 设置提示
    QPixmap pixmap(":/image/QATUndoSmall.png");                                                 // 导入图片
    pixmap = pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);            // 缩放导入的图片等比例到指定大小
    QIcon icon(pixmap);                                                                         // 图片导入图标
    undoButton->setIcon(icon);                                                                  // 设置图标
    undoButton->setEnabled(false);                                                              // 按钮初始不可用

    redoButton = new QPushButton();
    redoButton->setMaximumWidth(5000);
    redoButton->setObjectName("redoOperation");
    redoButton->setStyleSheet("background-color:transparent;font-size:14pt;");
    redoButton->setFlat(true);
    connect(redoButton, &QPushButton::clicked, this, [&]() {operationStack-> redo(); });
    redoButton->setToolTip(tr("还原"));
    QPixmap pixmap2(":/image/QATRedoSmall.png");
    pixmap2 = pixmap2.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QIcon icon2(pixmap2);
    redoButton->setIcon(icon2);
    redoButton->setEnabled(false);

    // 将撤销和重做按钮添加到场景中
    undoWidget = scene->addWidget(undoButton);
    undoWidget->setZValue(9999);                                                                // 置于顶部
    undoWidget->setMinimumHeight(50);                                                           // 设置最小高度
    undoWidget->setFlag(QGraphicsItem::ItemIsMovable, false);                                   // 禁用移动
    undoWidget->setFlag(QGraphicsItem::ItemIsSelectable, false);                                // 禁用选中
    undoWidget->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);                       // 不受父类影响
    undoWidget->setOpacity(0.75);                                                               // 设置透明度

    redoWidget = scene->addWidget(redoButton);
    redoWidget->setZValue(9999);
    redoWidget->setMinimumHeight(50);
    redoWidget->setFlag(QGraphicsItem::ItemIsMovable, false);
    redoWidget->setFlag(QGraphicsItem::ItemIsSelectable, false);
    redoWidget->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    redoWidget->setOpacity(0.75);
}

void View::updateButtonPosition()
{
    undoWidget->setPos(this->mapToScene(QPoint(0, 0)));                             // 设置撤销按钮的位置在视图最左侧
    redoWidget->setPos(this->mapToScene(int(undoWidget->geometry().width()), 0));   // 设置重做按钮的位置在撤销按钮旁
}

void View::wheelEvent(QWheelEvent *event)
{
    // 只有滚轮事件发生时
    if (event->buttons() == Qt::NoButton && event->modifiers() == Qt::NoModifier)
    {
        QPointF wheelPosition = event->pos();                               // 获取滚轮位置
        if (event->delta() > 0)
        {
            scaleMultiple = scaleMultiple * multiple;
            scaleByWheel(multiple, multiple, wheelPosition);                // 放大
        }
        else
        {
            scaleMultiple = scaleMultiple / multiple;
            scaleByWheel(1.0 / multiple, 1.0 / multiple, wheelPosition);    // 缩小
        }
        emit scaleMultipleChanged(scaleMultiple);
        operationStack->addOperation(new ScaleOperation(event->delta(), multiple, wheelPosition, this));
    }
    this->setFocus();
    QGraphicsView::wheelEvent(event);
}

void View::paintEvent(QPaintEvent *event)
{
    updateButtonPosition(); // 更新按钮位置
    QGraphicsView::paintEvent(event);
}

void View::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasText() && event->mimeData()->text().toInt() > 0 && event->mimeData()->text().toInt() <= 10)
    {
        event->acceptProposedAction();  // 接受拖动
    }
    else
    {
        event->ignore();                // 忽略拖动
    }
}

void View::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasText() && event->mimeData()->text().toInt() > 0 && event->mimeData()->text().toInt() <= 10)
    {
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}

void View::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasText() && event->mimeData()->text().toInt() > 0 && event->mimeData()->text().toInt() <= 10)
    {

        addChartItem(FlowEnumItem(event->mimeData()->text().toInt()), this->mapToScene(event->pos()));  // 添加图形
        this->setFocus();
        graphicsScene->setMode(Mode::NoMode);                                                           // 防止点击再次添加图形
    }
}

void View::mousePressEvent(QMouseEvent *event)
{
    movePosition = event->pos();                        // 记录当前鼠标位置
    pressPosition = event->pos();                       // 记录按下时鼠标位置
    if (event->buttons() == Qt::MiddleButton)           // 按下鼠标中键
    {
        isMoveView = true;                              // 启用视图移动
        viewport()->setCursor(Qt::ClosedHandCursor);    // 设置光标为手形
    }
    QGraphicsView::mousePressEvent(event);
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    if (isMoveView)
    {
        QPointF positionChanged = this->mapToScene(event->pos()) - this->mapToScene(movePosition);  // 计算位移
        scene()->setSceneRect(scene()->sceneRect().x() - positionChanged.x(),
                              scene()->sceneRect().y() - positionChanged.y(),
                              scene()->sceneRect().width(),
                              scene()->sceneRect().height());
        movePosition = event->pos();                                                                // 更新当前鼠标位置
    }
    QGraphicsView::mouseMoveEvent(event);
}

void View::mouseReleaseEvent(QMouseEvent *event)
{
    if (isMoveView)
    {
        QPointF startpos = this->mapToScene(movePosition);
        QPointF endpos = this->mapToScene(event->pos());
        QPointF positionChanged = endpos - startpos;
        // 调整位置
        scene()->setSceneRect(scene()->sceneRect().x() - positionChanged.x(),
                              scene()->sceneRect().y() - positionChanged.y(),
                              scene()->sceneRect().width(),
                              scene()->sceneRect().height());
        viewport()->setCursor(Qt::ArrowCursor);  // 还原光标
        this->operationStack->addOperation(new  ViewMoveOperation(this->mapToScene(pressPosition), endpos, this));
        isMoveView = false;                     // 禁用视图移动
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void View::buttonChange(int undoCount, int redoCount)
{
    undoButton->setEnabled(undoCount != 0); // 撤销栈为空则按钮不可用
    redoButton->setEnabled(redoCount != 0); // 重做栈为空则按钮不可用
}

void View::outTextEdit()
{
    QList<QGraphicsItem*> lists = this->scene()->selectedItems();                               // 获取选中的图形项
    for (QGraphicsItem* item : qAsConst(lists))
    {
        if (item->type() == TextItem::Type)                                                     // 如果是文本框类型
        {
            TextItem* textItem = qgraphicsitem_cast<TextItem*>(item);                           // 强制类型转换
            if (textItem->textInteractionFlags() == Qt::TextEditorInteraction)
            {
                textItem->setTextEditFlags(Qt::NoTextInteraction);                              // 禁用文本编辑
                textItem->setSelected(false);                                                   // 取消选中
            }
        }
    }
}


