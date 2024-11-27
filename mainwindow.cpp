#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);                  // 设置UI
    setWindowTitle("flyhigh v1.1.1");   // 设置窗口标题为"flyhigh"和版本
    setAcceptDrops(true);               // 启用拖放操作
    loadChartView();                    // 加载流程图视图
    createActions();                    // 创建动作（菜单项和工具栏按钮）
    createMenus();                      // 创建菜单
    createToolBar();                    // 创建工具栏

    connect(ui->insertLineAction, &QAction::triggered, this, &MainWindow::insertLine);
    connect(ui->insertTextAction, &QAction::triggered, this, &MainWindow::insertText);
    connect(ui->setColorAction, &QAction::triggered, this, &MainWindow::setFontColor);
    connect(ui->setFontAction, &QAction::triggered, this, &MainWindow::setFont);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow:: loadChartView()
{
    // 创建存放按钮的控件
    FlowLayout* flowLayout = new FlowLayout();
    flowLayout->setObjectName("flowLayout");
    flowLayout->setMargin(3);
    flowLayout->setSpacing(3);

    // 遍历每个字符串
    for (int i = 0; i < FlowTypeStrings.count(); i++)
    {
        ChartButton* button = new ChartButton();                                // 创建按钮
        button->setObjectName(QString::number(i + 1));                          // 命名控件
        button->setText(QString(FlowTypeStrings[i]).replace("流程图：", ""));    // 设置按钮文本，去除“流程图”
        button->setCheckable(false);                                            // 设置按钮为不可选中状态
        button->setToolTip(FlowTypeStrings[i]);                                 // 设置按钮提示
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);                // 设置按钮格式为图标在文本下方
        button->setIcon(QIcon(QString(":/image/flowchart/icon/fc-%1-bw.svg").arg(i + 1)));
        button->setIconSize(QSize(234, 132));
        connect(button, &ChartButton::hasSelectChart, this, &MainWindow::insertChart);
        flowLayout->addWidget(button);
    }
    ui->chartFrame->setLayout(flowLayout);                              // 放置按钮
    ui->chartFrame->setAcceptDrops(true);                               // 设置可拖动
}

void MainWindow::insertChart(FlowEnumItem type)
{
    View* view = ui->tabWidget->currentWidget()->findChild<View*>("graphicsView");
    if (view)
    {
        view->graphicsScene->setMode(Mode::InsertChart);
        view->graphicsScene->flowType = type;
    }
}

void MainWindow::createActions()
{
    // 打开XML文件
    openAction = new QAction(QIcon(":/image/XML.png"),tr("打开XML文件"), this);
    openAction->setShortcut(tr("Ctrl+O"));
    openAction->setStatusTip(tr("Open an existing file"));
    openAction->setObjectName("openAction");  // 设置 objectName
    connect(openAction, &QAction::triggered, this, &MainWindow::openXMLFile);

    // 保存XML文件
    saveAction = new QAction(QIcon(":/image/save.png"),tr("保存"), this);
    saveAction->setShortcut(tr("Ctrl+S"));
    saveAction->setStatusTip(tr("Save file"));
    saveAction->setObjectName("saveAction");  // 设置 objectName
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveXMLFile);

    // 另存为XML文件
    saveAsAction = new QAction(QIcon(":/image/saveas.png"),tr("另存为"),this);
    saveAsAction->setShortcut(tr("Ctrl+A"));
    saveAsAction->setStatusTip(tr("Save file as"));
    saveAsAction->setObjectName("saveAsAction");  // 设置 objectName
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAsXMLFile);

    // 新建页
    newAction = new QAction(QIcon(":/image/addpng.png"), tr("新建"), this);
    newAction->setShortcut(tr("Ctrl+N"));
    newAction->setStatusTip(tr("New File"));
    newAction->setObjectName("newAction");  // 设置 objectName
    connect(newAction, &QAction::triggered, this, &MainWindow::createNewPage);

    // 删除
    deleteAction = new QAction(QIcon(":/image/abondon.png"), tr("删除"), this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(tr("Delete item from charts"));
    deleteAction->setObjectName("deleteAction");  // 设置 objectName
    connect(deleteAction, &QAction::triggered, this, &MainWindow::deleteSelect);

    // 复制
    copyAction = new QAction(QIcon(":/image/copy.png"),tr("复制"), this);
    copyAction->setShortcut(tr("Ctrl+C"));
    copyAction->setStatusTip(tr("Copy Item(s)"));
    copyAction->setObjectName("copyAction");  // 设置 objectName
    connect(copyAction, &QAction::triggered, this, &MainWindow::copySelect);

    // 粘贴
    pasteAction = new QAction(QIcon(":/image/paste.png"),tr("粘贴"), this);
    pasteAction->setShortcut(tr("Ctrl+V"));
    pasteAction->setStatusTip(tr("Paste Item(s)"));
    pasteAction->setObjectName("pasteAction");  // 设置 objectName
    connect(pasteAction, &QAction::triggered, this, &MainWindow::pasteSelect);

    // 剪切
    cutAction = new QAction(QIcon(":/image/cut.png"),tr("剪切"), this);
    cutAction->setShortcut(tr("Ctrl+X"));
    cutAction->setStatusTip(tr("Cut Item(s)"));
    cutAction->setObjectName("cutAction");  // 设置 objectName
    connect(cutAction, &QAction::triggered, this, &MainWindow::cutSelect);

    // 插入html文本操作
    insertTextAction = new QAction(QIcon(":/image/T.png"),tr("插入文本"), this);
    connect(insertTextAction, &QAction::triggered, this, &MainWindow::insertText);

    // 添加连接线操作
    insertLineAction = new QAction(tr("连接线"), this);
    insertLineAction->setIcon(QIcon(QString::fromUtf8(":/image/connecting.png")));
    connect(insertLineAction, &QAction::triggered, this, &MainWindow::insertLine);

    // 撤销操作
    undoAction = new QAction(QIcon(":/image/QATUndoSmall.png"), tr("撤销"), this);
    undoAction->setShortcut(tr("Ctrl+Z"));
    undoAction->setObjectName("undoAction");  // 设置 objectName
    undoAction->setStatusTip(tr("撤销之前的操作"));

    connect(undoAction, &QAction::triggered, this, &MainWindow::undo);

    // 重做操作
    redoAction = new QAction(QIcon(":/image/QATRedoSmall.png"), tr("重做"), this);
    redoAction->setShortcut(tr("Ctrl+Y"));
    redoAction->setStatusTip(tr("重做之前的操作"));
    redoAction->setObjectName("redoAction");
    connect(redoAction, &QAction::triggered, this, &MainWindow::redo);

    // 退出
    exitAction = new QAction(QIcon(":/image/turnout.png"),tr("退出"), this);
    exitAction->setShortcut(tr("Ctrl+P"));
    exitAction->setStatusTip(tr("Quit SceneChart example"));
    exitAction->setObjectName("exitAction");
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    //关于
    aboutAction = new QAction(QIcon(":/image/about.png"), tr("关于"), this);
    aboutAction->setShortcut(tr("Ctrl+B"));
    aboutAction->setObjectName("aboutAction");
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    //图片作为背景
    bgAction = new QAction(QIcon(":/image/image background.png"), tr("导入图片作为背景"), this);
    bgAction->setShortcut(tr("Ctrl+G"));
    bgAction->setObjectName("bgAction");
    connect(bgAction, &QAction::triggered, this, &MainWindow::bg);

    //图片作为元素
    addAction = new QAction(QIcon(":/image/image save.png"), tr("导入图片作为元素"), this);
    addAction->setShortcut(tr("Ctrl+I"));
    addAction->setObjectName("addAction");
    connect(addAction, &QAction::triggered, this,&MainWindow::addPixmapItem);

}

void MainWindow::addPixmapItem() {
    // 打开文件对话框以选择图片文件
    QString fileName = QFileDialog::getOpenFileName(this, tr("导入图片作为元素"), ".", tr("Image Files (*.png *.jpg *.bmp *.jpeg)"));
    if (!fileName.isEmpty())                                                                    // 如果文件路径不为空，则继续处理
    {
        View* view = ui->tabWidget->currentWidget()->findChild<View*>("graphicsView");          // 获取当前的 QGraphicsView 对象
        if (view != nullptr)
        {
            QPixmap pixmap(fileName);                                                           // 加载图片
            if (!pixmap.isNull())                                                               // 检查图片是否成功加载
            {
                PixmapItem* pixmapItem = new PixmapItem(pixmap);                                // 创建一个新的 CustomPixmapItem 对象
                view->graphicsScene->addItem(pixmapItem);                                       // 添加图片项到场景中
                view->operationStack->addOperation(new AppendOperation(QList<QGraphicsItem*>() << pixmapItem, view));
            }
            else
            {

                QMessageBox::warning(this, tr("错误"), tr("无法加载图片文件，请选择有效的图片格式。")); // 提示用户图片加载失败
            }
        }
        else
        {
            QMessageBox::warning(this, tr("错误"), tr("未找到视图。请确保正确打开标签页。"));          // 提示用户视图不存在
        }
    }
}

void MainWindow::bg()
{
    // 打开文件对话框选择图片文件
    QString fileName = QFileDialog::getOpenFileName(this, tr("导入图片作为背景"), ".", tr("Image Files (*.png *.jpg *.bmp *.jpeg)"));
    if (!fileName.isEmpty())                                                                                        // 如果文件路径不为空，则设置背景
    {
        View* view = ui->tabWidget->currentWidget()->findChild<View*>("graphicsView");                              // 获取当前的 QGraphicsView 对象
        if (view != nullptr)
        {
            QPixmap pixmap(fileName);                                                                               // 加载图片
            if (!pixmap.isNull())                                                                                   // 检查图片是否成功加载
            {
                QBrush oldBackground = view->graphicsScene->backgroundBrush();
                QPixmap scaledPixmap = pixmap.scaled(view->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);  // 将图片调整为与视图大小相同
                view->graphicsScene->setBackgroundBrush(QBrush(scaledPixmap));                                      // 设置背景为缩放后的图片
                view->graphicsScene->update();
                view->update();                                                                                     // 更新视图以适应新背景
                view->operationStack->addOperation(new AppendBackgroundOperation(oldBackground, view->graphicsScene->backgroundBrush(), view));
            }
            else
            {
                QMessageBox::warning(this, tr("错误"), tr("无法加载图片文件，请选择有效的图片格式。"));                    // 提示用户图片加载失败
            }
        }
        else
        {
            QMessageBox::warning(this, tr("错误"), tr("未找到视图。请确保正确打开标签页。"));                             // 提示用户视图不存在
        }
    }
}

void MainWindow::openXMLFile()
{
    QFileDialog* fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(QStringLiteral("选中文件"));
    fileDialog->setDirectory(".");
    fileDialog->setNameFilter(tr("xml(*.xml)"));
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    fileDialog->setViewMode(QFileDialog::Detail);

    if (fileDialog->exec())
    {
        QStringList fileNames = fileDialog->selectedFiles();
        if (!fileNames.isEmpty())
        {
            QFileInfo info(fileNames[0]);
            int index = addTabWidgetPage(info.baseName());
            QWidget* widget = ui->tabWidget->widget(index);
            widget->setObjectName(info.baseName());
            readXMLFile(info.absoluteFilePath(), index);
        }
    }
}

void MainWindow::readXMLFile(QString filePath, int tabIndex)
{
    QWidget* widget = ui->tabWidget->widget(tabIndex);
    QMap<QString, ChartItem*> chartMap;
    QMap<QString, LineItem*> lineMap;
    QMap<QString, TextItem*> textMap;

    if (widget != nullptr)
    {
        View* view = widget->findChild<View*>("graphicsView");
        if (view != nullptr)
        {
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QXmlStreamReader reader(&file);
                while (!reader.atEnd())
                {
                    if (reader.isStartElement())
                    {
                        if (reader.name() == "ChartItem")
                        {
                            QXmlStreamAttributes attributes = reader.attributes();
                            FlowEnumItem _flowtype = static_cast<FlowEnumItem>(attributes.value("FlowType").toInt());
                            QStringList tranlist = attributes.value("QTransform").toString().split(" ");
                            QTransform tran = QTransform(tranlist[0].toDouble(), tranlist[1].toDouble(), tranlist[2].toDouble(),
                                                         tranlist[3].toDouble(), tranlist[4].toDouble(), tranlist[5].toDouble(),
                                                         tranlist[6].toDouble(), tranlist[7].toDouble(), tranlist[8].toDouble());
                            ChartItem* chartitem = new ChartItem(_flowtype);
                            chartitem->Uid = attributes.value("Uid").toString();
                            chartitem->setTransform(tran);
                            chartitem->setPos(QPointF(attributes.value("X").toDouble(), attributes.value("Y").toDouble()));

                            // 读取并设置颜色属性
                            chartitem->setCurrentFillColor(attributes.value("FillColor").toString());
                            chartitem->setCurrentBorderColor(attributes.value("BorderColor").toString());

                            // 读取并设置图元路径
                            QString svgPath = attributes.value("SvgPath").toString();
                            chartitem->setCurrentPath(svgPath);
                            chartitem->svgRender->load(svgPath);
                            chartitem->setSharedRenderer(chartitem->svgRender);

                            view->graphicsScene->addItem(chartitem);
                            chartMap.insert(chartitem->Uid, chartitem);
                        }
                        else if (reader.name() == "LineItem")
                        {
                            QXmlStreamAttributes attributes = reader.attributes();
                            QString myStartItemUid = attributes.value("myStartItem").toString();
                            QString myEndItemUid = attributes.value("myEndItem").toString();
                            if (chartMap.contains(myStartItemUid) && chartMap.contains(myEndItemUid))
                            {
                                LineItem* lineItem = new LineItem(chartMap[myStartItemUid], chartMap[myEndItemUid]);
                                lineItem->setLine(QLineF(attributes.value("x1").toDouble(), attributes.value("y1").toDouble(),
                                                         attributes.value("x2").toDouble(), attributes.value("y2").toDouble()));
                                lineItem->color = QColor(attributes.value("myColor").toString());
                                lineItem->Uid = attributes.value("Uid").toString();
                                view->graphicsScene->addItem(lineItem);
                                lineMap.insert(lineItem->Uid, lineItem);
                            }
                        }
                        else if (reader.name() == "TextItem")
                        {
                            QXmlStreamAttributes attributes = reader.attributes();
                            TextItem* textItem = new TextItem();
                            view->graphicsScene->addItem(textItem);
                            textItem->Uid = attributes.value("Uid").toString();
                            textItem->setPlainText(attributes.value("HtmlText").toString());
                            textItem->text = attributes.value("HtmlText").toString();
                            textItem->setDefaultTextColor(QColor(attributes.value("defaultTextColor").toString()));
                            QFont font = QFont();
                            font.fromString(attributes.value("Font").toString());
                            textItem->setFont(font);
                            textItem->setPos(QPointF(attributes.value("X").toDouble(), attributes.value("Y").toDouble()));
                            textItem->update();
                            textMap.insert(textItem->Uid, textItem);
                        }
                        else if (reader.name() == "ConnectItem")
                        {
                            QXmlStreamAttributes attributes = reader.attributes();
                            TextItem* textItem = nullptr;
                            QGraphicsItem* connectItem = nullptr;
                            // 找到关联文本项
                            QString textUid = attributes.value("TextItemUid").toString();
                            if (textMap.contains(textUid))
                            {
                                textItem = textMap[textUid];
                            }
                            // 找到关联图形项
                            QString connectUid = attributes.value("ConnectItemUid").toString();
                            if (chartMap.contains(connectUid))
                            {
                                connectItem = chartMap[connectUid];
                            }
                            else if (lineMap.contains(connectUid))
                            {
                                connectItem = lineMap[connectUid];
                            }
                            if (textItem && connectItem)
                            {
                                textItem->setConnectItem(connectItem);
                                // 设置文本项的属性
                                textItem->setFlag(QGraphicsItem::ItemIsMovable, false);                 // 禁用移动
                                textItem->setFlag(QGraphicsItem::ItemIsSelectable, true);               // 启用选择
                                textItem->setFlag(QGraphicsRectItem::ItemSendsGeometryChanges, false);  // 禁用几何变更信号
                                // 连接图形项位置变化信号
                                if (connectItem->type() == ChartItem::Type)
                                {
                                    connect(qgraphicsitem_cast<ChartItem*>(connectItem), &ChartItem::itemPositionHasChanged, textItem, &TextItem::parentPositionHasChanged);
                                }
                                else
                                {
                                    connect(qgraphicsitem_cast<LineItem*>(connectItem), &LineItem::itemPositionHasChanged, textItem, &TextItem::parentPositionHasChanged);
                                }
                            }
                        }
                    }
                    reader.readNext();
                }
                file.close();
            }
            view->graphicsScene->update();
        }
    }
}

void MainWindow::saveXMLFile()
{
    ui->tabWidget->currentWidget()->findChild<View*>("graphicsView")->operationStack->clearall();   // 清空操纵栈
}

void MainWindow::writeXMLFile(QString filePath, QString tabName, QString guid, Scene* scene)
{
    QList<QGraphicsItem*> allItems = scene->items();
    QList<ChartItem*> chartItems;
    QList<TextItem*> textItems;
    QList<LineItem*> lineItems;
    QList<QPair<TextItem*, QGraphicsItem*>> connectItems;

    for (QGraphicsItem* item : qAsConst(allItems))
    {
        switch (item->type())
        {
            case ChartItem::Type :
                chartItems.append(qgraphicsitem_cast<ChartItem*>(item));
                break;
            case LineItem::Type :
                lineItems.append(qgraphicsitem_cast<LineItem*>(item));
                break;
            case TextItem::Type :
                textItems.append(qgraphicsitem_cast<TextItem*>(item));
                break;
        }
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QXmlStreamWriter xml(&file);
        xml.setAutoFormatting(true);
        xml.writeStartDocument();
        xml.writeStartElement("FlowCharts");
        xml.writeAttribute("guid", guid);
        xml.writeAttribute("Tabname", tabName);

        for (ChartItem* chartItem : qAsConst(chartItems))
        {
            xml.writeStartElement("ChartItem");
            xml.writeAttribute("Uid", chartItem->Uid);
            xml.writeAttribute("FlowType", QString::number(static_cast<int>(chartItem->getChartType())));
            xml.writeAttribute("FillColor", chartItem->getCurrentFillColor());
            xml.writeAttribute("BorderColor", chartItem->getCurrentBorderColor());
            xml.writeAttribute("SvgPath", chartItem->getCurrentPath());  // 保存图元路径

            QStringList transform;
            transform << QString::number(chartItem->transform().m11()) << QString::number(chartItem->transform().m12()) << QString::number(chartItem->transform().m13());
            transform << QString::number(chartItem->transform().m21()) << QString::number(chartItem->transform().m22()) << QString::number(chartItem->transform().m23());
            transform << QString::number(chartItem->transform().m31()) << QString::number(chartItem->transform().m32()) << QString::number(chartItem->transform().m33());
            xml.writeAttribute("QTransform", transform.join(" "));
            xml.writeAttribute("X", QString::number(chartItem->pos().x()));
            xml.writeAttribute("Y", QString::number(chartItem->pos().y()));
            xml.writeEndElement();
        }

        for (LineItem* lineItem : qAsConst(lineItems))
        {
            xml.writeStartElement("LineItem");
            xml.writeAttribute("Uid", lineItem->Uid);
            xml.writeAttribute("myStartItem", lineItem->startItem->Uid);
            xml.writeAttribute("myEndItem", lineItem->endItem->Uid);
            xml.writeAttribute("myColor", lineItem->color.name());
            xml.writeAttribute("x1", QString::number(lineItem->line().x1()));
            xml.writeAttribute("y1", QString::number(lineItem->line().y1()));
            xml.writeAttribute("x2", QString::number(lineItem->line().x2()));
            xml.writeAttribute("y2", QString::number(lineItem->line().y2()));
            xml.writeEndElement();
        }

        for (TextItem* textItem : qAsConst(textItems))
        {
            xml.writeStartElement("TextItem");
            xml.writeAttribute("Uid", textItem->Uid);
            xml.writeAttribute("HtmlText", textItem->text);
            xml.writeAttribute("defaultTextColor", textItem->defaultTextColor().name());
            xml.writeAttribute("Font", textItem->font().toString());
            xml.writeAttribute("X", QString::number(textItem->pos().x()));
            xml.writeAttribute("Y", QString::number(textItem->pos().y()));
            xml.writeEndElement();
            // 检查关联图形
            QGraphicsItem* connectItem = qgraphicsitem_cast<TextItem*>(textItem)->connectItem;
            if (connectItem)
            {
                if ((connectItem->type() == ChartItem::Type && chartItems.contains(qgraphicsitem_cast<ChartItem*>(connectItem))) ||
                        (connectItem->type() == LineItem::Type && lineItems.contains(qgraphicsitem_cast<LineItem*>(connectItem))))
                {
                    connectItems.append(qMakePair(qgraphicsitem_cast<TextItem*>(textItem), connectItem));
                }
            }
        }

        for (const auto& connectPair : connectItems)
        {
            xml.writeStartElement("ConnectItem");
            xml.writeAttribute("TextItemUid", connectPair.first->Uid);
            if (connectPair.second->type() == ChartItem::Type) {
                xml.writeAttribute("ConnectItemUid", qgraphicsitem_cast<ChartItem*>(connectPair.second)->Uid);
            }
            else
            {
                xml.writeAttribute("ConnectItemUid", qgraphicsitem_cast<LineItem*>(connectPair.second)->Uid);
            }
            xml.writeEndElement();
        }

        xml.writeEndElement();
        xml.writeEndDocument();
        file.close();
        QMessageBox::information(this, "提示", "已保存到：" + filePath + "!");
    }
}

void MainWindow::saveAsXMLFile()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("保存XML文件"), ".", tr("XML Files (*.xml)"));

    if (!filePath.isEmpty())
    {
        if (!filePath.endsWith(".xml", Qt::CaseInsensitive))
        {
            filePath += ".xml";
        }

        QWidget* tabwidget = ui->tabWidget->currentWidget();
        View* view = tabwidget->findChild<View*>("graphicsView");

        QString tabName = ui->tabWidget->tabText(ui->tabWidget->indexOf(tabwidget));

        writeXMLFile(filePath, tabName, tabwidget->objectName(), view->graphicsScene);
        view->operationStack->clearall();
    }
    else
    {
        QMessageBox::warning(this, tr("警告"), tr("文件名不能为空。"));
    }
}

void MainWindow::createNewPage()
{
    bool ok = false;
    QString title = QInputDialog::getText(this, "新增单页", "请输入新增单页的名称", QLineEdit::Normal, "", &ok, Qt::WindowFlags(Qt::WindowCloseButtonHint | Qt::Dialog));
    if (ok)
    {
        int page = addTabWidgetPage(title);
        ui->tabWidget->setCurrentIndex(page);
    }
}

int MainWindow::addTabWidgetPage(QString pageName)
{
    // 新增页的图标
    QIcon pageIcon;
    pageIcon.addFile(QString::fromUtf8(":/image/addpng.png"), QSize(), QIcon::Normal, QIcon::Off);

    // 添加新页
    QWidget* newPage = new QWidget();
    newPage->setObjectName(QUuid::createUuid().toString(QUuid::WithoutBraces));

    // 设置垂直布局的布局管理器
    QVBoxLayout* verticalLayout = new QVBoxLayout(newPage);                                     // 布局到新页上
    verticalLayout->setSpacing(0);                                                              // 间距设置为0
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));                         // 设置名称
    verticalLayout->setContentsMargins(0, 0, 0, 0);                                             // 内边距为0

    // 设置视图
    View* graphicsView = new View(newPage);
    graphicsView->setObjectName("graphicsView");
    graphicsView->setStyleSheet("border:0px;");                                                 // 无边框
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);                           // 禁用垂直滚动条
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);                         // 禁用水平滚动条
    graphicsView->setDragMode(QGraphicsView::RubberBandDrag);                                   // 拖动模式设置为橡皮带拖动
    verticalLayout->addWidget(graphicsView);                                                    // 视图垂直排列在布局管理器上

    // 设置水平线
    QFrame* horizontalLine = new QFrame(newPage);
    horizontalLine->setObjectName(QString::fromUtf8("horizontalLine"));
    horizontalLine->setFrameShape(QFrame::HLine);                                               // 设置为水平线，用于分割
    horizontalLine->setFrameShadow(QFrame::Sunken);                                             // 设置框架阴影为凹陷，突出显示控件
    verticalLayout->addWidget(horizontalLine);                                                  // 水平线垂直排列在布局管理器上

    // 设置底部的工具栏
    QFrame* toolBox = new QFrame(newPage);
    toolBox->setObjectName(QString::fromUtf8("toolBox"));
    toolBox->setMinimumSize(QSize(0, 30));                                                      // 设置最小宽度和高度
    toolBox->setMaximumSize(QSize(16777215, 30));                                               // 设置最大宽度和高度
    toolBox->setStyleSheet(QString::fromUtf8("#toolBox{border:0px;}"));                         // 无边框
    toolBox->setFrameShape(QFrame::StyledPanel);                                                // 设置面板格式
    toolBox->setFrameShadow(QFrame::Raised);                                                    // 设置阴影提升，使框架高于背景

    // 设置水平布局的布局管理器
    QHBoxLayout* horizontalLayout = new QHBoxLayout(toolBox);
    horizontalLayout->setSpacing(3);                                                            // 间距为3像素
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    horizontalLayout->setContentsMargins(0, 0, 0, 0);
    // 内边距为0
    //设置字体
    QFont Font("SimSun-ExtB", 14, QFont::Normal);
    // 设置缩放标签
    QLabel* scaleLabel = new QLabel(toolBox);
    scaleLabel->setFont(Font);
    scaleLabel->setText("缩放:");
    scaleLabel->setObjectName(QString::fromUtf8("scaleLabel"));
    horizontalLayout->addWidget(scaleLabel);

    // 设置显示缩放程度和调整其程度的控件
    QDoubleSpinBox* scaleDoubleSpinBox = new QDoubleSpinBox(toolBox);
    scaleDoubleSpinBox->setFont(Font); // 这里我们直接重用labelFont
    scaleDoubleSpinBox->setObjectName(QString::fromUtf8("scaleDoubleSpinBox"));
    scaleDoubleSpinBox->setSuffix(QCoreApplication::translate("MainWindow", "%", nullptr));     // 设置显示格式（%结尾）
    scaleDoubleSpinBox->setMinimumSize(QSize(0, 30));                                           // 设置空间最小大小
    scaleDoubleSpinBox->setReadOnly(true);                                                      // 设置为只读模式（无法通过直接输入改变缩放程度）
    scaleDoubleSpinBox->setDecimals(2);                                                         // 显示两位小数
    scaleDoubleSpinBox->setMinimum(-9999.000000000000000);                                      // 设置最小值
    scaleDoubleSpinBox->setMaximum(9999.989999999999782);                                       // 设置最大值
    scaleDoubleSpinBox->setValue(100.000000000000000);                                          // 设置初始值
    connect(graphicsView, &View::scaleMultipleChanged, this, &MainWindow::scaleValueChanged);   // 缩放程度改变时发出信号调用槽函数修改控件显示的值
    horizontalLayout->addWidget(scaleDoubleSpinBox);

    // 设置垂直的分割线1
    QFrame* dividingLine_1 = new QFrame(toolBox);
    dividingLine_1->setObjectName(QString::fromUtf8("dividingLine_1"));
    dividingLine_1->setFrameShape(QFrame::VLine);                                               // 设置为垂直线
    dividingLine_1->setFrameShadow(QFrame::Sunken);                                             // 使框架凹陷在背景中
    horizontalLayout->addWidget(dividingLine_1);

    // 设置搜索文本框
    QTextEdit* searchEdit = new QTextEdit(toolBox);
    searchEdit->setFont(Font);
    searchEdit->setObjectName(QString::fromUtf8("searchEdit"));
    searchEdit->setMaximumSize(QSize(150, 30));
    searchEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);                             // 隐藏垂直滚动条
    horizontalLayout->addWidget(searchEdit);

    // 设置搜索按钮
    QPushButton* searchButton = new QPushButton(toolBox);
    searchButton->setFont(Font);
    searchButton->setObjectName(QString::fromUtf8("searchButton"));
    searchButton->setText("搜索");
    searchButton->setFlat(true);
    // 添加图标到按钮
    QIcon searchIcon(":/image/search.png");  // 假设图标文件路径是":/image/search.png"
    searchButton->setIcon(searchIcon);  // 设置图标
    horizontalLayout->addWidget(searchButton);
    connect(searchButton, &QPushButton::pressed, this, &MainWindow::searchText);

    // 设置替换文本框
    QTextEdit* replaceEdit = new QTextEdit(toolBox);
    replaceEdit->setFont(Font);
    replaceEdit->setObjectName(QString::fromUtf8("replaceEdit"));
    replaceEdit->setMaximumSize(QSize(150, 30));
    replaceEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    horizontalLayout->addWidget(replaceEdit);

    // 设置替换按钮
    QPushButton* replaceButton = new QPushButton(toolBox);
    replaceButton->setFont(Font);
    replaceButton->setObjectName(QString::fromUtf8("replaceButton"));
    replaceButton->setText("替换");
    replaceButton->setFlat(true);
    horizontalLayout->addWidget(replaceButton);
    connect(replaceButton, &QPushButton::pressed, this, &MainWindow::replaceText);

    // 设置上一个按钮
    QPushButton* lastButton = new QPushButton(toolBox);
    lastButton->setFont(Font);
    lastButton->setObjectName(QString::fromUtf8("lastButton"));
    lastButton->setFlat(true);
    // 设置图标
    QIcon lastIcon(":/image/previous.png");  // 假设图标文件路径是":/image/last.png"
    lastButton->setIcon(lastIcon);  // 为按钮设置图标
    horizontalLayout->addWidget(lastButton);
    connect(lastButton, &QPushButton::pressed, this, &MainWindow::lastText);

    // 设置下一个按钮
    QPushButton* nextButton = new QPushButton(toolBox);
    nextButton->setFont(Font);
    nextButton->setObjectName(QString::fromUtf8("nextButton"));
    nextButton->setFlat(true);
    // 设置图标
    QIcon nextIcon(":/image/next.png");  // 假设图标文件路径是":/image/next.png"
    nextButton->setIcon(nextIcon);  // 为按钮设置图标
    horizontalLayout->addWidget(nextButton);
    connect(nextButton, &QPushButton::pressed, this, &MainWindow::nextText);

    // 设置全部按钮
    QPushButton* allButton = new QPushButton(toolBox);
    allButton->setFont(Font);
    allButton->setObjectName(QString::fromUtf8("allButton"));
    allButton->setText("选中全部");
    allButton->setFlat(true);
    horizontalLayout->addWidget(allButton);
    connect(allButton, &QPushButton::pressed, this, &MainWindow::allText);

    // 设置垂直的分割线2
    QFrame* dividingLine_2 = new QFrame(toolBox);
    dividingLine_2->setObjectName(QString::fromUtf8("dividingLine_2"));
    dividingLine_2->setFrameShape(QFrame::VLine);
    dividingLine_2->setFrameShadow(QFrame::Sunken);
    horizontalLayout->addWidget(dividingLine_2);

    // 插入一个空白区域
    QSpacerItem* horizontalSpacer = new QSpacerItem(100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);

    // 设置保存文件为XML格式的按钮
    QPushButton* saveXMLButton = new QPushButton(toolBox);
    saveXMLButton->setFont(Font);
    saveXMLButton->setMinimumSize(QSize(0, 30));
    QIcon saveXMLIcon;
    saveXMLIcon.addFile(QString::fromUtf8(":/image/Saveas.png"), QSize(), QIcon::Normal, QIcon::On);
    saveXMLButton->setIcon(saveXMLIcon);
    saveXMLButton->setText("保存为XML格式");                                                         // 设置按钮文本
    saveXMLButton->setFlat(true);                                                               // 设置按钮风格，使其扁平
    //saveXMLButton->setShortcut(QKeySequence(tr("Ctrl+S")));                                     // 设置快捷键为Ctrl+S
    connect(saveXMLButton, &QPushButton::pressed, this, &MainWindow::saveAsXMLFile);            // 点击保存键后触发保存文件的槽函数
    horizontalLayout->addWidget(saveXMLButton);

    // 设置垂直的分割线3
    QFrame* dividingLine_3 = new QFrame(toolBox);
    dividingLine_3->setObjectName(QString::fromUtf8("dividingLine_3"));
    dividingLine_3->setFrameShape(QFrame::VLine);
    dividingLine_3->setFrameShadow(QFrame::Sunken);
    horizontalLayout->addWidget(dividingLine_3);

    // 设置保存文件为SVG格式的按钮
    QPushButton* saveSVGButton = new QPushButton(toolBox);
    saveSVGButton->setFont(Font);
    saveSVGButton->setObjectName(QString::fromUtf8("saveSVGButton"));
    saveSVGButton->setMinimumSize(QSize(0, 30));
    QIcon saveSVGIcon;
    saveSVGIcon.addFile(QString::fromUtf8(":/image/saveas2.png"), QSize(), QIcon::Normal, QIcon::On);
    saveSVGButton->setIcon(saveSVGIcon);
    saveSVGButton->setText("保存为SVG格式");
    saveSVGButton->setFlat(true);
    connect(saveSVGButton, &QPushButton::pressed, this, &MainWindow::saveSVG);
    horizontalLayout->addWidget(saveSVGButton);
    verticalLayout->addWidget(toolBox);

    // 添加右键菜单
    graphicsView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(graphicsView, &QGraphicsView::customContextMenuRequested, this, &MainWindow::showMenu);

    return ui->tabWidget->addTab(newPage, pageIcon, pageName);
}

void MainWindow::scaleValueChanged(double scaleMultiple)
{   // 查找对应控件
    QDoubleSpinBox* scaleDoubleSpinBox = sender()->parent()->findChild<QDoubleSpinBox*>("scaleDoubleSpinBox");
    scaleDoubleSpinBox->setValue(scaleMultiple * 100);
}

void MainWindow::searchText()
{
    View* view = sender()->parent()->parent()->findChild<View*>("graphicsView");
    // 重置
    view->graphicsScene->containTexts.clear();
    view->graphicsScene->currentIndex = 0;
    QTextEdit* searchEdit = sender()->parent()->findChild<QTextEdit*>("searchEdit");
    QString text = searchEdit->toPlainText();
    view->graphicsScene->currentText = text;
    for (TextItem* textItem : view->graphicsScene->allTexts)
    {
        if (textItem->toPlainText() == text && text != "") // 包含文本且不为空
        {
            view->graphicsScene->containTexts.append(textItem);
        }
        textItem->setSelected(false);
    }
    // 不为空
    if (!view->graphicsScene->containTexts.isEmpty())
    {
       view->graphicsScene->containTexts[0]->setSelected(true);
    }
}

void MainWindow::lastText()
{
    View* view = sender()->parent()->parent()->findChild<View*>("graphicsView");
    // 移除已经更改的文本
    for (TextItem* textItem : view->graphicsScene->containTexts)
    {
        if (textItem->toPlainText() != view->graphicsScene->currentText)
        {
            view->graphicsScene->containTexts.removeOne(textItem);
        }
    }
    if (!view->graphicsScene->containTexts.isEmpty())
    {
        view->graphicsScene->containTexts[view->graphicsScene->currentIndex]->setSelected(false);
        if (view->graphicsScene->currentIndex > 0)
        {
            view->graphicsScene->currentIndex--;
        }
        else
        {
            view->graphicsScene->currentIndex = view->graphicsScene->containTexts.count() - 1;
        }
        view->graphicsScene->containTexts[view->graphicsScene->currentIndex]->setSelected(true);
    }
}

void MainWindow::nextText()
{
    View* view = sender()->parent()->parent()->findChild<View*>("graphicsView");
    for (TextItem* textItem : view->graphicsScene->containTexts)
    {
        if (textItem->toPlainText() != view->graphicsScene->currentText)
        {
            view->graphicsScene->containTexts.removeOne(textItem);
        }
    }
    if (!view->graphicsScene->containTexts.isEmpty())
    {
        view->graphicsScene->containTexts[view->graphicsScene->currentIndex]->setSelected(false);
        if (view->graphicsScene->currentIndex < view->graphicsScene->containTexts.count() - 1)
        {
            view->graphicsScene->currentIndex++;
        }
        else
        {
            view->graphicsScene->currentIndex = 0;
        }
        view->graphicsScene->containTexts[view->graphicsScene->currentIndex]->setSelected(true);
    }
}

void MainWindow::allText()
{
    // 获取当前视图
    View* view = sender()->parent()->parent()->findChild<View*>("graphicsView");
    if (!view) return;

    // 清除与当前查找文本不匹配的文本项
    QList<TextItem*> validTexts;
    for (TextItem* textItem : view->graphicsScene->containTexts)
    {
        if (textItem->toPlainText() == view->graphicsScene->currentText)
        {
            validTexts.append(textItem);
        }
    }
    view->graphicsScene->containTexts = validTexts; // 更新 `containTexts` 列表

    // 设置所有匹配的文本项为选中状态
    for (TextItem* textItem : view->graphicsScene->containTexts)
    {
        textItem->setSelected(true);
    }
}

void MainWindow::replaceText()
{
    View* view = sender()->parent()->parent()->findChild<View*>("graphicsView");
    QTextEdit* replaceEdit = sender()->parent()->findChild<QTextEdit*>("replaceEdit");

    QString oldText;
    QString newText = replaceEdit->toPlainText();

    QList<QGraphicsItem*> textItems;
    for (TextItem* textItem : view->graphicsScene->allTexts)
    {

        if (textItem->isSelected()) // 被选中的文本框
        {
            oldText = textItem->toPlainText();
            textItem->setHtml(newText);
            textItems << textItem;
        }
    }
    view->operationStack->addOperation(new ReplacceTextOperation(oldText, newText, textItems, view));
}

void MainWindow::saveSVG()
{
    View* view = sender()->parent()->parent()->findChild<View*>("graphicsView");
    QString defaultPath = "C:/";    // 默认路径
    QString filename = QFileDialog::getSaveFileName(this, tr("Save SVG File"), defaultPath, tr("SVG Files (*.svg)"));
    if (!filename.isEmpty())
    {
        view->graphicsScene->saveSceneToSVG(filename);
    }
}

void MainWindow::showMenu(const QPoint& position)
{
    View* graphicsView = ui->tabWidget->currentWidget()->findChild<View*>("graphicsView");
    QMenu contextMenu(tr("Context menu"), graphicsView);    // 创建菜单，并关联到视图中

    // 将动作插入到菜单中
    contextMenu.addAction(deleteAction);
    contextMenu.addAction(copyAction);
    contextMenu.addAction(pasteAction);
    contextMenu.addAction(cutAction);
    contextMenu.addSeparator();                             // 分界线
    contextMenu.addAction(insertTextAction);
    contextMenu.addAction(insertLineAction);
    contextMenu.addSeparator();
    contextMenu.addAction(undoAction);
    contextMenu.addAction(redoAction);
    // 设置菜单出现的位置
    contextMenu.exec(graphicsView->mapToGlobal(position));
}

void MainWindow::deleteSelect()
{
    View* graphicsView = ui->tabWidget->currentWidget()->findChild<View*>("graphicsView");
    QList<QGraphicsItem *> items = graphicsView->graphicsScene->selectedItems();

    if (items.count() != 0)
    {
        for (QGraphicsItem* item : items)
        {
            if (item->type() == PixmapItem::Type) // 检查是否是自定义图片项
            {
                graphicsView->graphicsScene->removeItem(item);
                delete item; // 删除项
            }
            else if (item->type() == ChartItem::Type)
            {
                // 对其他类型的项进行删除操作
                graphicsView->graphicsScene->removeAllSelect(items);
            }
            // 可以添加更多的条件来支持其他类型的项目
        }
    }
}

void MainWindow::copySelect()
{
    if (ui->tabWidget->currentWidget() == nullptr)
    {
       return;
    }
    View* view = ui->tabWidget->currentWidget()->findChild<View*>("graphicsView");
    if (view != nullptr)
    {
        QList<QGraphicsItem*> selectedItems = view->graphicsScene->selectedItems();
        QMap<ChartItem*, QString> copiedChartItems;
        QMap<LineItem*, QString> copiedLineItems;
        QMap<TextItem*, QString> copiedTextItems;
        QList<QPair<TextItem*, QGraphicsItem*>> copiedConnectItems;
        // 分类
        for (QGraphicsItem* item : qAsConst(selectedItems))
        {
            switch(item->type())
            {
                case ChartItem::Type :
                    copiedChartItems.insert(qgraphicsitem_cast<ChartItem*>(item), QUuid::createUuid().toString(QUuid::WithoutBraces));
                    break;
                case LineItem::Type :
                    copiedLineItems.insert(qgraphicsitem_cast<LineItem*>(item), QUuid::createUuid().toString(QUuid::WithoutBraces));
                    break;
                case TextItem::Type :
                    copiedTextItems.insert(qgraphicsitem_cast<TextItem*>(item), QUuid::createUuid().toString(QUuid::WithoutBraces));
                    break;
            }
        }

        QClipboard* clipboard = QApplication::clipboard();  // 剪切板
        QString clipboardData;
        QXmlStreamWriter xml(&clipboardData);               // 创建XML流读取数据
        xml.setAutoFormatting(true);
        xml.writeStartDocument();
        xml.writeStartElement("CopiedItems");

        // 复制图形信息
        for (ChartItem* chartItem : copiedChartItems.keys())
        {
            xml.writeStartElement("ChartItem");
            xml.writeAttribute("Uid", copiedChartItems[chartItem]);
            xml.writeAttribute("FlowType", QString::number(static_cast<int>(chartItem->getChartType())));
            xml.writeAttribute("X", QString::number(chartItem->pos().x()));
            xml.writeAttribute("Y", QString::number(chartItem->pos().y()));
            xml.writeAttribute("FillColor", chartItem->getCurrentFillColor());
            xml.writeAttribute("BorderColor", chartItem->getCurrentBorderColor());
            // 添加缩放矩阵信息
            QTransform transform = chartItem->transform();
            QStringList trans;
            trans << QString::number(transform.m11()) << QString::number(transform.m12())
                  << QString::number(transform.m13());
            trans << QString::number(transform.m21()) << QString::number(transform.m22())
                  << QString::number(transform.m23());
            trans << QString::number(transform.m31()) << QString::number(transform.m32())
                  << QString::number(transform.m33());
            xml.writeAttribute("QTransform", trans.join(" "));
            xml.writeEndElement();
        }


        // 复制线信息
        for (LineItem* lineItem : copiedLineItems.keys())
        {
            xml.writeStartElement("LineItem");
            xml.writeAttribute("Uid", copiedLineItems[lineItem]);
            xml.writeAttribute("myStartItem", copiedChartItems[lineItem->startItem]);
            xml.writeAttribute("myEndItem", copiedChartItems[lineItem->endItem]);
            xml.writeAttribute("myColor", lineItem->color.name());
            xml.writeAttribute("x1", QString::number(lineItem->line().x1()));
            xml.writeAttribute("y1", QString::number(lineItem->line().y1()));
            xml.writeAttribute("x2", QString::number(lineItem->line().x2()));
            xml.writeAttribute("y2", QString::number(lineItem->line().y2()));
            xml.writeEndElement();
        }

        // 复制文本信息
        for (TextItem* textItem : copiedTextItems.keys())
        {
            xml.writeStartElement("TextItem");
            xml.writeAttribute("Uid", copiedTextItems[textItem]);
            xml.writeAttribute("HtmlText", textItem->text);
            xml.writeAttribute("defaultTextColor", textItem->defaultTextColor().name());
            xml.writeAttribute("Font", textItem->font().toString());
            xml.writeAttribute("X", QString::number(textItem->pos().x()));
            xml.writeAttribute("Y", QString::number(textItem->pos().y()));

            // 添加缩放矩阵信息
            QTransform transform = textItem->transform();
            QStringList trans;
            trans << QString::number(transform.m11()) << QString::number(transform.m12())
                  << QString::number(transform.m13());
            trans << QString::number(transform.m21()) << QString::number(transform.m22())
                  << QString::number(transform.m23());
            trans << QString::number(transform.m31()) << QString::number(transform.m32())
                  << QString::number(transform.m33());
            xml.writeAttribute("QTransform", trans.join(" "));
            xml.writeEndElement();
            // 检查关联图形
            QGraphicsItem* connectItem = qgraphicsitem_cast<TextItem*>(textItem)->connectItem;
            if (connectItem)
            {
                if ((connectItem->type() == ChartItem::Type && copiedChartItems.keys().contains(qgraphicsitem_cast<ChartItem*>(connectItem))) ||
                        (connectItem->type() == LineItem::Type && copiedLineItems.keys().contains(qgraphicsitem_cast<LineItem*>(connectItem))))
                {
                    copiedConnectItems.append(qMakePair(qgraphicsitem_cast<TextItem*>(textItem), connectItem));
                }
            }
        }

        // 复制关联信息
        for (const auto& connectPair : copiedConnectItems)
        {
            xml.writeStartElement("ConnectItem");
            xml.writeAttribute("TextItemUid", copiedTextItems[connectPair.first]);
            if (connectPair.second->type() == ChartItem::Type)
            {
                xml.writeAttribute("ConnectItemUid", copiedChartItems[qgraphicsitem_cast<ChartItem*>(connectPair.second)]);
            }
            else
            {
                xml.writeAttribute("ConnectItemUid", copiedLineItems[qgraphicsitem_cast<LineItem*>(connectPair.second)]);
            }
            xml.writeEndElement();
        }

        xml.writeEndElement();                              // 读取结束
        xml.writeEndDocument();
        clipboard->setText(clipboardData);
    }
}

void MainWindow::pasteSelect()
{
    if (ui->tabWidget->currentWidget() == nullptr)
    {
        return;
    }

    View* view = ui->tabWidget->currentWidget()->findChild<View*>("graphicsView");
    if (view != nullptr)
    {
        QClipboard *clipboard = QApplication::clipboard();
        QString clipboardData = clipboard->text();

        if (clipboardData.isEmpty())
        {
            QMessageBox::information(this, "提示", "剪贴板为空，无法粘贴!");
            return;
        }

        // 获取鼠标当前位置并转换为场景坐标
        QPointF mousePosition = view->mapToScene(view->mapFromGlobal(QCursor::pos()));

        QXmlStreamReader reader(clipboardData);
        QMap<QString, ChartItem*> mapCharts;
        QMap<QString, LineItem*> mapLines;
        QMap<QString, TextItem*> mapTexts;

        // 用于计算复制项的中心点
        QList<QPointF> originalPositions;
        // 首先遍历一次，收集所有图形项的位置
        while (!reader.atEnd())
        {
            reader.readNext();
            if (reader.isStartElement() && reader.name() == "ChartItem")
            {
                QXmlStreamAttributes attributes = reader.attributes();
                QPointF originalPos(attributes.value("X").toDouble(), attributes.value("Y").toDouble());
                originalPositions.append(originalPos);
            }

            if (reader.isStartElement() && reader.name() == "TextItem")
            {
                QXmlStreamAttributes attributes = reader.attributes();
                QPointF originalPos(attributes.value("X").toDouble(), attributes.value("Y").toDouble());
                originalPositions.append(originalPos);
            }
        }

        // 计算原始图形项的中心点
        QPointF centerPosition;
        for (const QPointF& pos : originalPositions)
        {
           centerPosition += pos;
        }
        if (!originalPositions.empty())
        {
           centerPosition /= originalPositions.size();
        }

        // 计算鼠标位置和中心点之间的偏移
        QPointF offset = mousePosition - centerPosition;

        // 重新开始读取剪贴板数据，实际创建图形项
        reader.clear();
        reader.addData(clipboardData);

        QList<QGraphicsItem*> appendItems;
        while (!reader.atEnd())
        {
            reader.readNext();
            // 粘贴图形
            if (reader.isStartElement() && reader.name() == "ChartItem")
            {
                QXmlStreamAttributes attributes = reader.attributes();
                FlowEnumItem _flowtype = static_cast<FlowEnumItem>(attributes.value("FlowType").toInt());
                ChartItem* chartItem = new ChartItem(_flowtype);
                chartItem->Uid = attributes.value("Uid").toString();

                // 计算粘贴时的新位置
                QPointF originalPos(attributes.value("X").toDouble(), attributes.value("Y").toDouble());
                chartItem->setPos(originalPos + offset);

                // 应用缩放矩阵
                QStringList tranlist = attributes.value("QTransform").toString().split(" ");
                if (tranlist.size() == 9)
                {
                    QTransform transform = QTransform(
                                tranlist[0].toDouble(), tranlist[1].toDouble(), tranlist[2].toDouble(),
                                tranlist[3].toDouble(), tranlist[4].toDouble(), tranlist[5].toDouble(),
                                tranlist[6].toDouble(), tranlist[7].toDouble(), tranlist[8].toDouble());
                    chartItem->setTransform(transform);
                }

                // 恢复填充颜色和边框颜色
                QString fillColor = attributes.value("FillColor").toString();
                QString borderColor = attributes.value("BorderColor").toString();
                QString newPath = QString(":/image/flowchart/icon/fc-%1-%2%3.svg")
                                  .arg(static_cast<int>(_flowtype))
                                  .arg(borderColor)
                                  .arg(fillColor);
                chartItem->svgRender->load(newPath);
                chartItem->setSharedRenderer(chartItem->svgRender);
                chartItem->setCurrentPath(newPath);

                view->graphicsScene->addItem(chartItem);
                appendItems << chartItem;
                mapCharts.insert(chartItem->Uid, chartItem);
            }
            // 粘贴连接线
            else if (reader.isStartElement() && reader.name() == "LineItem")
            {
               QXmlStreamAttributes attributes = reader.attributes();
               QString myStartItem_uid = attributes.value("myStartItem").toString();
               QString myEndItem_uid = attributes.value("myEndItem").toString();

               if (mapCharts.contains(myStartItem_uid) && mapCharts.contains(myEndItem_uid))
               {
                   LineItem* lineItem = new LineItem(mapCharts[myStartItem_uid], mapCharts[myEndItem_uid]);
                   lineItem->setLine(QLineF(attributes.value("x1").toDouble() + offset.x(), attributes.value("y1").toDouble() + offset.y(),
                                            attributes.value("x2").toDouble() + offset.x(), attributes.value("y2").toDouble() + offset.y()));
                   lineItem->color = QColor(attributes.value("myColor").toString());
                   lineItem->Uid = attributes.value("Uid").toString();
                   view->graphicsScene->addItem(lineItem);
                   appendItems << lineItem;
                   view->graphicsScene->allLines.append(lineItem);
                   connect(lineItem, &LineItem::doubleClickItem, view->graphicsScene, &Scene::doubleClickItem);
                   mapLines.insert(lineItem->Uid, lineItem);
                }
            }
            // 粘贴文本
            else if (reader.isStartElement() && reader.name() == "TextItem")
            {
                QXmlStreamAttributes attributes = reader.attributes();
                TextItem* textItem = new TextItem;
                view->graphicsScene->addItem(textItem);
                textItem->Uid = attributes.value("Uid").toString();
                textItem->setHtml(attributes.value("HtmlText").toString());
                textItem->text = attributes.value("HtmlText").toString();
                QFont font = QFont();
                font.fromString(attributes.value("Font").toString());
                textItem->setFont(font);
                textItem->setDefaultTextColor(QColor(attributes.value("defaultTextColor").toString()));
                textItem->update();

                // 计算粘贴时的新位置
                QPointF originalPos(attributes.value("X").toDouble(), attributes.value("Y").toDouble());
                textItem->setPos(originalPos + offset);

                // 应用缩放矩阵
                QStringList tranlist = attributes.value("QTransform").toString().split(" ");
                if (tranlist.size() == 9)
                {
                    QTransform tran = QTransform(
                                tranlist[0].toDouble(), tranlist[1].toDouble(), tranlist[2].toDouble(),
                                tranlist[3].toDouble(), tranlist[4].toDouble(), tranlist[5].toDouble(),
                                tranlist[6].toDouble(), tranlist[7].toDouble(), tranlist[8].toDouble());
                    textItem->setTransform(tran);
                }

                appendItems << textItem;
                view->graphicsScene->allTexts.append(textItem);
                mapTexts.insert(textItem->Uid, textItem);
            }
            // 粘贴关联信息
            else if (reader.isStartElement() && reader.name() == "ConnectItem")
            {
                QXmlStreamAttributes attributes = reader.attributes();
                TextItem* textItem = nullptr;
                QGraphicsItem* connectItem = nullptr;
                // 找到关联文本项
                QString textUid = attributes.value("TextItemUid").toString();
                if (mapTexts.contains(textUid))
                {
                    textItem = mapTexts[textUid];
                }
                // 找到关联图形项
                QString connectUid = attributes.value("ConnectItemUid").toString();
                if (mapCharts.contains(connectUid))
                {
                    connectItem = mapCharts[connectUid];
                }
                else if (mapLines.contains(connectUid))
                {
                    connectItem = mapLines[connectUid];
                }
                if (textItem && connectItem)
                {
                    textItem->setConnectItem(connectItem);
                    // 设置文本项的属性
                    textItem->setFlag(QGraphicsItem::ItemIsMovable, false);                 // 禁用移动
                    textItem->setFlag(QGraphicsItem::ItemIsSelectable, true);               // 启用选择
                    textItem->setFlag(QGraphicsRectItem::ItemSendsGeometryChanges, false);  // 禁用几何变更信号
                    // 连接图形项位置变化信号
                    if (connectItem->type() == ChartItem::Type)
                    {
                        connect(qgraphicsitem_cast<ChartItem*>(connectItem), &ChartItem::itemPositionHasChanged, textItem, &TextItem::parentPositionHasChanged);
                    }
                    else
                    {
                        connect(qgraphicsitem_cast<LineItem*>(connectItem), &LineItem::itemPositionHasChanged, textItem, &TextItem::parentPositionHasChanged);
                    }
                }
            }

        }

        if (reader.hasError())
        {
            QMessageBox::warning(this, "错误", "解析剪贴板数据时出错: " + reader.errorString());
        }
        view->operationStack->addOperation(new AppendOperation(appendItems, view)); // 添加元素操纵入栈
        view->graphicsScene->update();
    }
}

void MainWindow::cutSelect()
{
    if (ui->tabWidget->currentWidget() == nullptr)
    {
          return;
    }
    // 获取当前的 QGraphicsViewRefactor 对象
    View* refator = ui->tabWidget->currentWidget()->findChild<View*>("graphicsView");
    if (refator != nullptr)
    {
        QList<QGraphicsItem*> selectedItems = refator->graphicsScene->selectedItems();
        copySelect();
        // 删除选中的项
        refator->graphicsScene->removeAllSelect(selectedItems);
    }
}

void MainWindow::insertLine()
{
    View* view = ui->tabWidget->currentWidget()->findChild<View*>("graphicsView");
    if (view)
    {
        view->graphicsScene->setMode(Mode::InsertLine);
    }
}

void MainWindow::insertText()
{
    View* view = ui->tabWidget->currentWidget()->findChild<View*>("graphicsView");
    if (view)
    {
        view->graphicsScene->setMode(Mode::InsertText); // 切换为插入文本模式
    }
}

void MainWindow::undo()
{
    View* view = sender()->parent()->findChild<View*>("graphicsView");
    if (view)
    {
        view->operationStack->undo();
    }
}

void MainWindow::redo()
{
    View* view = sender()->parent()->findChild<View*>("graphicsView");
    if (view)
    {
        view->operationStack->redo();
    }
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("文件"));
    fileMenu->setObjectName("fileMenu");
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(exitAction);
    fileMenu->addAction(bgAction);
    fileMenu->addAction(addAction);

    editMenu = menuBar()->addMenu(tr("编辑"));
    editMenu->setObjectName("editMenu");
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(cutAction);
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);

    aboutMenu = menuBar()->addMenu(tr("帮助"));
    aboutMenu->setObjectName("aboutMenu");
    aboutMenu->addAction(aboutAction);
    // 设置字体样式和大小
    QString styleSheet = R"(
        QMenuBar {
            font-family: 'SimSun-ExtB';
            font-size: 14pt;
            background-color:white;
        }

        QMenu {
            font-family: 'SimSun-ExtB';
            font-size: 14pt;
            background-color:white;
            /* 可以为整个菜单设置背景色，但通常不需要 */
        }

        QMenu::item {
            /* 这里可以设置未悬停时的菜单项样式 */
            background-color: white; /* 或者其他颜色 */
        }

        QMenu::item:hover {
            /* 鼠标悬停时的菜单项样式 */
            background-color:rgba(155, 155, 155,0.5);
            color: black; /* 白色字体 */
        }

        QMenu::item:selected {
            /* 菜单项被选中（如通过键盘导航）时的样式 */
            background-color:rgba(155, 155, 155,0.5);
            color: black; /* 白色字体 */
        }
        /* QAction 样式通常通过 QMenu::item 来控制 */
    )";

    // 应用样式表到整个窗口（这将影响窗口中的所有菜单和菜单项）
    this->setStyleSheet(styleSheet);
}

void MainWindow::createToolBar()
{
    QToolBar* colorToolBar = addToolBar(tr("颜色"));
    colorToolBar->setStyleSheet("QToolBar { font-family: 'SimSun-ExtB'; font-size: 14pt; }"
                                "QToolButton { font-family: 'SimSun-ExtB'; font-size: 14pt; }"
                                "QMenu { font-family: 'SimSun-ExtB'; font-size: 14pt; }");
    colorToolBar->setObjectName("colorToolButton");


    QToolButton* fillColorToolButton = new QToolButton();                       // 创建填充颜色按钮
    fillColorToolButton->setText("填充颜色");
    fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    QIcon fillColorIcon(":/image/paint.png");                                   // 设置图标
    fillColorToolButton->setIcon(fillColorIcon);
    fillColorToolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);      // 设置图标在左侧，文本在右侧
    fillColorToolButton->setObjectName("fillColorToolButton");

    QMenu* colorMenu = new QMenu(fillColorToolButton);                          // 创建填充颜色菜单
    colorMenu->setObjectName("colorMenu");
    QAction* whiteAction = colorMenu->addAction(QIcon(":/path/to/white.png"), "白色");
    whiteAction->setObjectName("whiteAction");
    QAction* redAction = colorMenu->addAction(QIcon(":/image/red.png"), "红色");
    redAction->setObjectName("redAction");
    QAction* greenAction = colorMenu->addAction(QIcon(":/image/green.png"), "绿色");
    greenAction->setObjectName("greenAction");
    QAction* yellowAction = colorMenu->addAction(QIcon(":/image/yellow.png"), "黄色");
    yellowAction->setObjectName("yellowAction");

    // 连接填充颜色菜单的actions到槽函数
    connect(whiteAction, &QAction::triggered, this, &MainWindow::selectFillColor);
    connect(redAction, &QAction::triggered, this, &MainWindow::selectFillColor);
    connect(greenAction, &QAction::triggered, this, &MainWindow::selectFillColor);
    connect(yellowAction, &QAction::triggered, this, &MainWindow::selectFillColor);


    QToolButton *borderColorToolButton = new QToolButton;// 创建边框颜色按钮
    borderColorToolButton->setText("边框颜色");
    borderColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    QIcon borderColorIcon(":/image/paint line.png");                            // 设置图标
    borderColorToolButton->setIcon(borderColorIcon);
    borderColorToolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);    // 设置图标在左侧，文本在右侧
    borderColorToolButton->setObjectName("borderColorToolButton");
    QMenu* colorMenu2 = new QMenu(borderColorToolButton);                       // 创建边框颜色菜单
    colorMenu2->setObjectName("colorMenu2");
    QAction* redAction2 = colorMenu2->addAction(QIcon(":/image/red.png"), "红色");
    redAction2->setObjectName("redAction2");
    QAction* blueAction2 = colorMenu2->addAction(QIcon(":/image/blue.png"), "蓝色");
    blueAction2->setObjectName("blueActon2");
    QAction* blackAction2 = colorMenu2->addAction(QIcon(":/image/black.png"), "黑色");
    blackAction2->setObjectName("blackActon2");

    // 连接边框颜色菜单的actions到槽函数
    connect(redAction2, &QAction::triggered, this, &MainWindow::selectBorderColor);
    connect(blueAction2, &QAction::triggered, this, &MainWindow::selectBorderColor);
    connect(blackAction2, &QAction::triggered, this, &MainWindow::selectBorderColor);

    QToolButton *templateToolButton = new QToolButton;// 创建模板按钮
    templateToolButton->setText("模版");
    templateToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    QIcon templateIcon(":/image/template.png");
    templateToolButton->setIcon(templateIcon);
    templateToolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);    // 设置图标在左侧，文本在右侧
    templateToolButton->setObjectName("templateToolButton");
    QMenu* templateMenu = new QMenu(templateToolButton);
    templateMenu->setObjectName("templateMenu");
    QAction *book = templateMenu->addAction("还书流程图");
    book->setObjectName("book");
    QAction *decision =templateMenu->addAction("决策流程图");
    decision->setObjectName("decision");
    QAction *pay=templateMenu->addAction("支付流程图");
    pay->setObjectName("pay");
    connect(book, &QAction::triggered, this, &MainWindow::selectBook);
    connect(decision, &QAction::triggered, this, &MainWindow::selectDecision);
    connect(pay, &QAction::triggered, this, &MainWindow::selectPay);
    // 设置菜单到按钮上，按钮到工具栏上
    borderColorToolButton->setMenu(colorMenu2);
    fillColorToolButton->setMenu(colorMenu);
    // 设置菜单到按钮上
    templateToolButton->setMenu(templateMenu);
    colorToolBar->addWidget(fillColorToolButton);
    colorToolBar->addWidget(borderColorToolButton);
    colorToolBar->addWidget(templateToolButton);
}

void MainWindow::selectBook()
{
    QString filePath = ":/template/book.xml";  // 指定图书馆还书流程图的XML文件路径

    // 打开并读取XML文件
    if (!filePath.isEmpty())
    {
        QFileInfo info(filePath);
        int index = addTabWidgetPage(info.baseName());
        QWidget* widget = ui->tabWidget->widget(index);
        widget->setObjectName(info.baseName());
        readXMLFile(info.absoluteFilePath(), index);
    }
}

void MainWindow::selectDecision()
{
    QString filePath = ":/template/require.xml";  // 指定决策流程图的XML文件路径

    // 打开并读取XML文件
    if (!filePath.isEmpty())
    {
        QFileInfo info(filePath);
        int index = addTabWidgetPage(info.baseName());
        QWidget* widget = ui->tabWidget->widget(index);
        widget->setObjectName(info.baseName());
        readXMLFile(info.absoluteFilePath(), index);
    }
}

void MainWindow::selectPay()
{
    QString filePath = ":/template/pay.xml";  // 指定决策流程图的XML文件路径

    // 打开并读取XML文件
    if (!filePath.isEmpty())
    {
        QFileInfo info(filePath);
        int index = addTabWidgetPage(info.baseName());
        QWidget* widget = ui->tabWidget->widget(index);
        widget->setObjectName(info.baseName());
        readXMLFile(info.absoluteFilePath(), index);
    }
}

void MainWindow::selectFillColor()
{
    // 获取发送信号的action
    QAction* action = qobject_cast<QAction*>(sender());

    // 获取图形视图
    View* view = ui->tabWidget->currentWidget()->findChild<View*>("graphicsView");
    if (view == nullptr)
    {
        return;
    }

    // 获取选中的图形项
    QList<QGraphicsItem*> selectedItems = view->graphicsScene->selectedItems();
    if (selectedItems.isEmpty())
    {
        return;
    }

    QList<QGraphicsItem*> changedItems;                             // 修改的图形
    QList<QPair<QString, QString>> fillColors;                      // 填充颜色
    QList<QPair<QString, QString>> borderColors;                    // 边框颜色

    // 解析新颜色路径
    QString selectedColor = action->text();                         // 从action的文本获取选择的颜色
    QString newFillColor = "";                                      // 填充颜色代码
    if (selectedColor == "红色")
    {
        newFillColor = "r";                                         // 红色
    }
    else if (selectedColor == "绿色")
    {
        newFillColor = "g";                                         // 绿色
    }
    else if (selectedColor == "黄色")
    {
        newFillColor = "y";                                         // 黄色
    }
    else
    {
        newFillColor = "w";                                         // 默认白色
    }

    for (QGraphicsItem* item : selectedItems)
    {
        if (item->type() == ChartItem::Type)                        // 只考虑图形
        {
            ChartItem* chartItem = qgraphicsitem_cast<ChartItem*>(item);

            changedItems.append(chartItem);
            QString oldFillColor = chartItem->getCurrentFillColor();

            chartItem->setCurrentFillColor(newFillColor);           // 设置颜色

            // 颜色更改信息添加
            fillColors.append({oldFillColor, newFillColor});
            borderColors.append({chartItem->getCurrentBorderColor(), chartItem->getCurrentBorderColor()});
        }
    }
    view->operationStack->addOperation(new ColorOperation(fillColors, borderColors, changedItems));
}

void MainWindow::selectBorderColor()
{
    // 获取发送信号的action
    QAction* action = qobject_cast<QAction*>(sender());

    // 获取图形视图
    View* view = ui->tabWidget->currentWidget()->findChild<View*>("graphicsView");
    if (view == nullptr)
    {
        return;
    }

    // 获取选中的图形项
    QList<QGraphicsItem*> selectedItems = view->graphicsScene->selectedItems();
    if (selectedItems.isEmpty())
    {
        return;
    }

    QList<QGraphicsItem*> changedItems;                             // 修改的图形
    QList<QPair<QString, QString>> fillColors;                      // 填充颜色
    QList<QPair<QString, QString>> borderColors;                    // 边框颜色

    // 解析新颜色路径
    QString selectedBorderColor = action->text();                   // 从action的文本获取选择的颜色
    QString newBorderColor = "";                                    // 边框颜色代码
    if (selectedBorderColor == "红色")
    {
        newBorderColor = "r";                                       // 红色
    }
    else if (selectedBorderColor == "蓝色")
    {
        newBorderColor = "l";                                       // 蓝色
    }
    else
    {
        newBorderColor = "b";                                       // 黑色
    }

    for (QGraphicsItem* item : selectedItems)
    {
        if (item->type() == ChartItem::Type)                        // 只考虑图形
        {
            ChartItem* chartItem = qgraphicsitem_cast<ChartItem*>(item);

            changedItems.append(chartItem);
            QString oldBorderColor = chartItem->getCurrentBorderColor();

            chartItem->setCurrentBorderColor(newBorderColor);       // 设置颜色

            // 颜色更改信息添加
            fillColors.append({chartItem->getCurrentFillColor(), chartItem->getCurrentFillColor()});
            borderColors.append({oldBorderColor, newBorderColor});
        }
    }
    view->operationStack->addOperation(new ColorOperation(fillColors, borderColors, changedItems));
}

void MainWindow::setFont()
{
    if (ui->tabWidget->currentWidget() == nullptr)
    {
        return;
    }

    View* view = ui->tabWidget->currentWidget()->findChild<View*>("graphicsView");
    if (view != nullptr)
    {
        QList<QGraphicsItem*> items;
        QList<QGraphicsItem*> selectitems = view->graphicsScene->selectedItems();

        for (QGraphicsItem* textitem : qAsConst(selectitems))
        {
            if (textitem->type() == TextItem::Type)
            {
                items.append(qgraphicsitem_cast<TextItem*>(textitem));
            }
        }

        // 无文本则退出
        if (items.count() == 0)
        {
            return;
        }

        bool isOk = false;
        QFont oldFont = qgraphicsitem_cast<TextItem*>(items[0])->font();
        QColor color = qgraphicsitem_cast<TextItem*>(items[0])->defaultTextColor();

        // 创建一个默认字体并设置其大小为12
        QFont defaultFont = (items.count() > 0 ? qgraphicsitem_cast<TextItem*>(items[0])->font() : QFont("Arial", 12));

        // 显示字体选择对话框，使用默认字体
        QFont newFont = QFontDialog::getFont(&isOk, defaultFont, this);
        if (isOk)
        {
            for (QGraphicsItem* item : qAsConst(items))
            {
                TextItem* textItem = qgraphicsitem_cast<TextItem*>(item);
                textItem->setFont(newFont);
                textItem->update();
            }
        }

        view->operationStack->addOperation(new ChangeFontOperation(oldFont, color, newFont, color, items, view));
    }
}

void MainWindow::setFontColor()
{
    if (ui->tabWidget->currentWidget() == nullptr)
    {
        return;
    }

    View* view = ui->tabWidget->currentWidget()->findChild<View*>("graphicsView");
    if (view != nullptr)
    {
        QList<QGraphicsItem*> textItems;
        QList<QGraphicsItem* > selectitems = view->graphicsScene->selectedItems();
        for (QGraphicsItem* textitem : qAsConst(selectitems))
        {
            if (textitem->type() == TextItem::Type)
            {
                textItems.append(qgraphicsitem_cast<TextItem*>(textitem));
            }
        }
        if (textItems.count() == 0)
        {
            return;
        }
        QFont font = qgraphicsitem_cast<TextItem*>(textItems[0])->font();
        QColor oldColor = qgraphicsitem_cast<TextItem*>(textItems[0])->defaultTextColor();
        QColor newColor = QColorDialog::getColor(textItems.count() > 0 ? qgraphicsitem_cast<TextItem*>(textItems[0])->defaultTextColor() : Qt::black, this);
        if (newColor.isValid())
        {
            for (QGraphicsItem* item : qAsConst(textItems))
            {
                TextItem* textItem = qgraphicsitem_cast<TextItem*>(item);
                textItem->setDefaultTextColor(newColor);
                textItem->update();
            }
        }
        view->operationStack->addOperation(new ChangeFontOperation(font, oldColor, font, newColor, textItems, view));
    }
}

void MainWindow::closeTab(int index)
{
    ui->tabWidget->removeTab(index);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About FLYHIGH"),
                       tr("一、新建流程图\n"
                          "点击菜单栏的“文件”>“新建”，或使用快捷键（Ctrl+N）创建一个新的流程图文件，或者使用模板。\n"
                          "二、添加元素\n"
                          "从左侧工具栏选择所需的形状，然后在画布上点击并拖动以绘制。\n"
                          "三、编辑与调整\n"
                          "可以移动、缩放、旋转元素，或调整其颜色、边框等属性。\n"
                          "四、保存与导出\n"
                          "完成流程图后，点击菜单栏的“文件”>“保存”或“另存为”来保存您的文件。支持导出SVG格式。"));
}
