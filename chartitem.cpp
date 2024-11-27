#include "chartitem.h"

ChartItem::ChartItem(FlowEnumItem type, QGraphicsItem* parent, QString Uid)
    : QGraphicsSvgItem(parent), Uid(Uid), chartType(type),  currentFillColor("w"), currentBorderColor("b")
{
    this->setAcceptHoverEvents(true);                                           // 设置接受悬停事件
    setFlag(QGraphicsSvgItem::ItemIsMovable, true);                             // 设置图形项可移动
    setFlag(QGraphicsSvgItem::ItemIsSelectable, true);                          // 设置图形项可选中
    setFlag(QGraphicsSvgItem::ItemSendsGeometryChanges, true);                  // 设置几何变更事件
    this->Uid = QUuid::createUuid().toString(QUuid::WithoutBraces);             // 生成新的唯一ID
    svgRender = new QSvgRenderer;                                               // 创建SVG渲染器
    svgRender->load(QString(":/image/flowchart/icon/fc-%1-bw.svg").arg(type));  // 加载对应类型的SVG文件
    setSharedRenderer(svgRender);                                               // 设置共享渲染器
    currentSvgPath = QString(":/image/flowchart/icon/fc-%1-bw.svg").arg(type);  // 初始化currentSvgPath为默认路径
    text = QString(FlowTypeStrings[type - 1]).replace("流程图：", "");           // 设置文本，去掉“流程图：”前缀
    setTransform(transform().scale(10, 10));                                    // 设置标题，去掉“流程图：”前缀

    // 设置控制点
    new ControlPoint(RectDirection::LeftTop, this);
    new ControlPoint(RectDirection::LeftBtn, this);
    new ControlPoint(RectDirection::RightTop, this);
    new ControlPoint(RectDirection::RightBtn, this);
    new ControlPoint(RectDirection::BottomCenter, this);
}

FlowEnumItem ChartItem::getChartType()
{
    return chartType;
}

void ChartItem::setChartType(FlowEnumItem type)
{
    if (type > 0 && type <= 11)
    {
        chartType = type;
    }
    currentSvgPath = QString(":/image/flowchart/icon/fc-%1-%2%3.svg").arg(static_cast<int>(chartType)).arg(currentBorderColor).arg(currentFillColor);
    svgRender->load(currentSvgPath);
    setSharedRenderer(svgRender);   // 更新渲染器
}

QString ChartItem::getCurrentFillColor() const
{
    return currentFillColor;
}

void ChartItem::setCurrentFillColor(const QString& color)
{
    if (color == "r" || color == "g" || color == "y" || color == "w")
    {
        currentFillColor = color;
    }
    currentSvgPath = QString(":/image/flowchart/icon/fc-%1-%2%3.svg").arg(static_cast<int>(chartType)).arg(currentBorderColor).arg(currentFillColor);
    svgRender->load(currentSvgPath);
    setSharedRenderer(svgRender);   // 更新渲染器
}

QString ChartItem::getCurrentBorderColor() const
{
    return currentBorderColor;
}

void ChartItem::setCurrentBorderColor(const QString& color)
{
    if (color == "r" || color == "l" || color == "b")
    {
        currentBorderColor = color;
    }
    currentSvgPath = QString(":/image/flowchart/icon/fc-%1-%2%3.svg").arg(static_cast<int>(chartType)).arg(currentBorderColor).arg(currentFillColor);
    svgRender->load(currentSvgPath);
    setSharedRenderer(svgRender);   // 更新渲染器
}

QString ChartItem::getCurrentPath(){
    return currentSvgPath;
}

void ChartItem::setCurrentPath(QString currentPath)
{
    currentSvgPath = currentPath;
    svgRender->load(currentSvgPath);
    setSharedRenderer(svgRender);   // 更新渲染器
}

int ChartItem::type() const
{
    return Type;
}

extern QPainterPath qt_regionToPath(const QRegion& region);

void ChartItem::getPolys()
{
    QPixmap* pixmap = new QPixmap(svgRender->defaultSize());            // 创建一个与SVG大小相同的QPixmap
    pixmap->fill(Qt::transparent);                                      // 设置背景透明
    QPainter painter(pixmap);                                           // 创建QPainter对象
    painter.setRenderHints(QPainter::Antialiasing);                     // 开启抗锯齿
    svgRender->render(&painter);                                        // 渲染SVG内容到pixmap
    pixmap = new QPixmap(pixmap->transformed(this->transform()));       // 应用当前变换
    QBitmap mask = pixmap->mask();                                      // 获取像素掩码
    QPainterPath mappath = qt_regionToPath(QRegion(mask));              // 将掩码转换为路径
    mappath.translate(this->transform().dx(), this->transform().dy());  // 将路径平移到当前变换的dx和dy位置
    delete pixmap;                                                      // 删除pixmap，释放内存
}

QPainterPath ChartItem::shape() const
{
    QPainterPath path = QGraphicsSvgItem::shape();  // 获取SVG的形状
    return path;                                    // 返回形状
}

void ChartItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QGraphicsSvgItem::paint(painter, option, widget);  // 调用基类的绘制方法
}

QVariant ChartItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    // 处理选择状态变更事件
    if (change == QGraphicsSvgItem::ItemSelectedChange)
    {
        // 如果被选择，显示所有子项
        if (value.toUInt() == 1)
        {
            foreach (QGraphicsItem *item, this->childItems())
            {
                if (item->type() != QGraphicsTextItem::Type)
                {
                    item->show();
                }
            }
        }
        // 如果取消选择，隐藏所有子项
        else
        {
            foreach (QGraphicsItem* item, this->childItems())
            {
                if (item->type() != QGraphicsTextItem::Type)
                {
                    item->hide();
                }
            }
        }
    }
    // 处理几何变更事件
    else if (change == QGraphicsSvgItem::ItemTransformHasChanged)
    {
        getPolys();                     // 更新多边形路径
        emit itemPositionHasChanged();  // 发射位置变更信号
    }
    // 处理位置变更事件
    else if (change == QGraphicsSvgItem::ItemPositionHasChanged)
    {
        emit itemPositionHasChanged(); // 发射位置变更信号
    }
    // 返回基类的处理结果
    return QGraphicsSvgItem::itemChange(change, value);
}
