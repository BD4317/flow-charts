#include <QtTest>
#include "mainwindow.h"
#include "scene.h"
#include "view.h"
#include "textitem.h"
#include "operationstack.h"

class TestMainWindow : public QObject
{
    Q_OBJECT

private:
    MainWindow* mainWindow;

private slots:
    void initTestCase()
    {
        mainWindow = new MainWindow();
        mainWindow->show();
        QTest::qWait(100); // 等待事件循环初始化
    }

    void cleanupTestCase()
    {
        delete mainWindow;
    }

    void testNewTab()
       {
           QTabWidget* tabWidget = mainWindow->findChild<QTabWidget*>("tabWidget");
           QVERIFY(tabWidget);

           int initialTabCount = tabWidget->count();

           QAction* newAction = mainWindow->findChild<QAction*>("newAction");
           QVERIFY(newAction);

           newAction->trigger();
           QCOMPARE(tabWidget->count(), initialTabCount + 1);
       }

    void testAddAndDeleteChart()
    {
        // 获取视图和场景
        View* view = mainWindow->findChild<View*>("graphicsView");
        QVERIFY(view);

        Scene* scene = view->graphicsScene;
        QVERIFY(scene);

        OperationStack* operationStack = view->operationStack;
        QVERIFY(operationStack);

        int initialUndoCount = operationStack->getUndoCount();
        int initialRedoCount = operationStack->getRedoCount();

        // 模拟添加图形
        FlowLayout* flowLayout = mainWindow->findChild<FlowLayout*>("flowLayout");
        QVERIFY(flowLayout);

        ChartButton* chartButton = mainWindow->findChild<ChartButton*>("1", Qt::FindChildrenRecursively);
        QVERIFY(chartButton);

        QTest::mouseClick(chartButton, Qt::LeftButton); // 点击按钮，选择图形

        QGraphicsSceneMouseEvent eventPress(QEvent::GraphicsSceneMousePress);
        eventPress.setScenePos(QPointF(100, 100)); // 设置插入位置
        eventPress.setButton(Qt::LeftButton);
        scene->mousePressEvent(&eventPress);

        // 验证图形是否添加成功
        int chartItemCount = 0;
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == ChartItem::Type)
            {
                ++chartItemCount;
            }
        }
        QCOMPARE(chartItemCount, 1); // 确保只添加了一个图形

        // 检查操作栈状态
        QCOMPARE(operationStack->getUndoCount(), initialUndoCount + 1);
        QCOMPARE(operationStack->getRedoCount(), 0);

        // 撤销添加图形
        QAction* undoAction = mainWindow->findChild<QAction*>("undoAction");
        QVERIFY(undoAction);
        undoAction->trigger();

        // 验证撤销后图形是否被删除
        chartItemCount = 0;
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == ChartItem::Type)
            {
                ++chartItemCount;
            }
        }
        QCOMPARE(chartItemCount, 0); // 确保图形被删除
        QCOMPARE(operationStack->getUndoCount(), initialUndoCount);
        QCOMPARE(operationStack->getRedoCount(), initialRedoCount + 1);

        // 重做添加图形
        QAction* redoAction = mainWindow->findChild<QAction*>("redoAction");
        QVERIFY(redoAction);
        redoAction->trigger();

        // 验证重做后图形是否被恢复
        chartItemCount = 0;
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == ChartItem::Type)
            {
                ++chartItemCount;
            }
        }
        QCOMPARE(chartItemCount, 1); // 确保图形被恢复
        QCOMPARE(operationStack->getUndoCount(), initialUndoCount + 1);
        QCOMPARE(operationStack->getRedoCount(), initialRedoCount);

        // 删除图形
        QGraphicsItem* addedItem = nullptr;
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == ChartItem::Type)
            {
                addedItem = item;
                break;
            }
        }
        QVERIFY(addedItem != nullptr);
        addedItem->setSelected(true);

        QKeyEvent deleteEvent(QEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier);
        QApplication::sendEvent(scene, &deleteEvent);

        // 验证图形是否被删除
        chartItemCount = 0;
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == ChartItem::Type)
            {
                ++chartItemCount;
            }
        }
        QCOMPARE(chartItemCount, 0); // 确保图形被删除

        // 检查删除后的操作栈状态
        QCOMPARE(operationStack->getUndoCount(), initialUndoCount + 2);
        QCOMPARE(operationStack->getRedoCount(), 0);

        // 撤销删除图形
        undoAction->trigger();

        // 验证撤销删除后图形是否恢复
        chartItemCount = 0;
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == ChartItem::Type)
            {
                ++chartItemCount;
            }
        }
        QCOMPARE(chartItemCount, 1); // 确保图形被恢复
        QCOMPARE(operationStack->getUndoCount(), initialUndoCount + 1);
        QCOMPARE(operationStack->getRedoCount(), initialRedoCount + 1);

        // 重做删除图形
        redoAction->trigger();

        // 验证重做删除后图形是否被移除
        chartItemCount = 0;
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == ChartItem::Type)
            {
                ++chartItemCount;
            }
        }
        QCOMPARE(chartItemCount, 0); // 确保图形被移除
        QCOMPARE(operationStack->getUndoCount(), initialUndoCount + 2);
        QCOMPARE(operationStack->getRedoCount(), initialRedoCount);
    }

    int countChartItems(Scene* scene)
    {
        int chartItemCount = 0;
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == ChartItem::Type)
            {
                ++chartItemCount;
            }
        }
        return chartItemCount;
    }

    void testCopyPasteCutChart()
    {
        // 获取视图和场景
        View* view = mainWindow->findChild<View*>("graphicsView");
        QVERIFY(view);
        // 在每次测试之前重新创建一个新的场景
        Scene* scene = new Scene(view);
        view->setScene(scene); // 将新场景设置到视图中
        QVERIFY(scene);
        // 初始检查
        int initialChartCount = countChartItems(scene);
        // 添加图形元素
        FlowLayout* flowLayout = mainWindow->findChild<FlowLayout*>("flowLayout");
        QVERIFY(flowLayout);

        ChartButton* chartButton = mainWindow->findChild<ChartButton*>("1", Qt::FindChildrenRecursively);
        QVERIFY(chartButton);

        QTest::mouseClick(chartButton, Qt::LeftButton); // 点击图形按钮

        // 模拟添加图形的鼠标事件
        QGraphicsSceneMouseEvent eventPress(QEvent::GraphicsSceneMousePress);
        QPointF addPosition(100, 100); // 添加位置
        eventPress.setScenePos(addPosition);
        eventPress.setButton(Qt::LeftButton);
        scene->mousePressEvent(&eventPress);

        // 检查图形项是否添加成功
        int addedChartCount = countChartItems(scene);
        QCOMPARE(addedChartCount, initialChartCount + 1);

        QGraphicsItem* addedItem = nullptr;
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == ChartItem::Type)
            {
                addedItem = item;
                break;
            }
        }
        QVERIFY(addedItem != nullptr);
        addedItem->setSelected(true);

        // 调用复制功能
        mainWindow->copySelect();

        // 设置鼠标位置用于粘贴
        QPointF pastePosition(110, 110); // 预期粘贴位置
        QCursor::setPos(view->mapToGlobal(view->mapFromScene(pastePosition)));

        // 调用粘贴功能
        mainWindow->pasteSelect();

        // 等待事件完成
        QTest::qWait(100);

        // 检查粘贴后的图形项数量
        int finalChartCountAfterPaste = countChartItems(scene);
        QCOMPARE(finalChartCountAfterPaste, initialChartCount + 2);

        // 验证新图形项
        QGraphicsItem* newItem = nullptr;
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == ChartItem::Type && item != addedItem)
            {
                newItem = item;
                break;
            }
        }
        QVERIFY(newItem != nullptr); // 确保找到新粘贴的图形

        // 检查新图形项的位置
        QCOMPARE(newItem->pos(), pastePosition);

        // 调用剪切功能
        addedItem->setSelected(true);
        mainWindow->cutSelect();

        // 等待事件完成
        QTest::qWait(100);

        // 检查剪切后的图形项数量
        int finalChartCountAfterCut = countChartItems(scene);
        QCOMPARE(finalChartCountAfterCut, initialChartCount + 1); // 剪切后图形数量应减少一个

        // 再次粘贴，验证剪切内容的粘贴行为
        QPointF secondPastePosition(120, 120); // 第二次粘贴位置
        QCursor::setPos(view->mapToGlobal(view->mapFromScene(secondPastePosition)));
        mainWindow->pasteSelect();

        // 等待事件完成
        QTest::qWait(100);

        // 检查最终图形项数量
        int finalChartCount = countChartItems(scene);
        QCOMPARE(finalChartCount, initialChartCount + 2); // 最终数量应与初始数量 + 2 相等

        // 验证剪切后粘贴的图形项
        QGraphicsItem* cutPastedItem = nullptr;
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == ChartItem::Type && item != addedItem && item != newItem)
            {
                cutPastedItem = item;
                break;
            }
        }
        QVERIFY(cutPastedItem != nullptr); // 确保找到剪切后粘贴的图形

        // 检查剪切后粘贴图形的位置
        QCOMPARE(cutPastedItem->pos(), secondPastePosition);
    }

    void testDragMoveChart()
    {
        // 获取视图和场景
        View* view = mainWindow->findChild<View*>("graphicsView");
        QVERIFY(view);

        // 创建新场景并设置到视图中
        Scene* scene = new Scene(view);
        view->setScene(scene);
        QVERIFY(scene);

        // 添加图形项
        QPointF startPosition(100, 100);
        view->addChartItem(FlowEnumItem::StartOrEnd, startPosition);

        QGraphicsItem* addedItem = nullptr;
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == ChartItem::Type)
            {
                addedItem = item;
                break;
            }
        }
        QVERIFY(addedItem != nullptr);

        // 验证图形项的初始位置
        QCOMPARE(addedItem->pos(), startPosition);

        // 设置图形项为可移动
        addedItem->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

        // 设置图形项为选中
        addedItem->setSelected(true);
        QVERIFY(addedItem->isSelected()); // 验证图形项是否被选中

        // 模拟鼠标拖动操作
        QPointF newPosition(200, 200);

        // 模拟鼠标按下事件
        QGraphicsSceneMouseEvent pressEvent(QEvent::GraphicsSceneMousePress);
        pressEvent.setScenePos(startPosition);
        pressEvent.setButton(Qt::LeftButton);
        pressEvent.setButtons(Qt::LeftButton);
        pressEvent.setModifiers(Qt::NoModifier);
        QApplication::sendEvent(scene, &pressEvent);

        // 模拟鼠标移动事件
        QGraphicsSceneMouseEvent moveEvent(QEvent::GraphicsSceneMouseMove);
        moveEvent.setScenePos(newPosition);
        moveEvent.setLastScenePos(startPosition);
        moveEvent.setButton(Qt::LeftButton);
        moveEvent.setButtons(Qt::LeftButton);
        moveEvent.setModifiers(Qt::NoModifier);
        QApplication::sendEvent(scene, &moveEvent);

        // 模拟鼠标释放事件
        QGraphicsSceneMouseEvent releaseEvent(QEvent::GraphicsSceneMouseRelease);
        releaseEvent.setScenePos(newPosition);
        releaseEvent.setButton(Qt::LeftButton);
        releaseEvent.setButtons(Qt::LeftButton);
        releaseEvent.setModifiers(Qt::NoModifier);
        QApplication::sendEvent(scene, &releaseEvent);

        // 等待事件完成
        QTest::qWait(100);

        // 验证图形项的新位置
        QCOMPARE(addedItem->pos(), newPosition);

        // 模拟撤销操作
        QAction* undoAction = mainWindow->findChild<QAction*>("undoAction");
        QVERIFY(undoAction != nullptr);
        undoAction->trigger();
        QTest::qWait(100);

        // 验证图形项恢复到初始位置
        QCOMPARE(addedItem->pos(), startPosition);

        // 模拟重做操作
        QAction* redoAction = mainWindow->findChild<QAction*>("redoAction");
        QVERIFY(redoAction != nullptr);
        redoAction->trigger();
        QTest::qWait(100);

        // 验证图形项再次移动到新位置
        QCOMPARE(addedItem->pos(), newPosition);
    }

    /*void testResizeAndRotateChart()
    {
        // 获取视图和场景
        View* view = mainWindow->findChild<View*>("graphicsView");
        QVERIFY(view);

        // 创建新场景并设置到视图中
        Scene* scene = new Scene(view);
        view->setScene(scene);
        QVERIFY(scene);

        // 在场景中添加一个图形项
        QPointF testPosition(0, 0);
        view->addChartItem(FlowEnumItem::Flow1, testPosition);

        // 查找添加的图形项
        ChartItem* chartItem = nullptr;
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == ChartItem::Type)
            {
                chartItem = dynamic_cast<ChartItem*>(item);
                break;
            }
        }
        QVERIFY(chartItem != nullptr);

        // 验证图形项的初始大小
        QRectF initialBoundingRect = chartItem->boundingRect();
        QCOMPARE(initialBoundingRect.size(), QSizeF(16, 16));

        // 选中图形项
        chartItem->setSelected(true);
        QVERIFY(chartItem->isSelected());

        // 计算 RightTop 控制点位置
        QPointF rightTopScenePos = chartItem->scenePos() + QPointF(initialBoundingRect.width() - 1.5, 0);
        qDebug() << "RightTop scene position:" << rightTopScenePos;

        // 模拟鼠标移动到 RightTop 控制点位置
        QTest::mouseMove(view->viewport(), view->mapFromScene(rightTopScenePos));

        // 等待事件循环，确保事件处理完成
        QTest::qWait(100);

        // 验证是否正确移动到 RightTop 控制点
        QRectF rightTopRect = QRectF(rightTopScenePos, QSizeF(1.5, 1.5)); // RightTop 控制点的区域
        QVERIFY(rightTopRect.contains(rightTopScenePos));
        qDebug() << "Moved to RightTop control point successfully.";

        // 验证光标是否更新
        QCOMPARE(view->cursor().shape(), Qt::SizeBDiagCursor);

        // 模拟鼠标拖动 RightTop 控制点来调整大小
        QPointF postop = rightTopScenePos;              // 初始位置
        QPointF posbtn = rightTopScenePos + QPointF(10, -5); // 拖动后的目标位置

        QTest::mousePress(view->viewport(), Qt::LeftButton, Qt::NoModifier, view->mapFromScene(postop));
        QTest::mouseMove(view->viewport(), view->mapFromScene(posbtn));
        QTest::mouseRelease(view->viewport(), Qt::LeftButton, Qt::NoModifier, view->mapFromScene(posbtn));

        // 等待事件循环，确保界面更新生效
        QTest::qWait(100);

        // 验证图形项大小是否更新
        QRectF updatedBoundingRect = chartItem->boundingRect();
        QVERIFY(updatedBoundingRect.width() > initialBoundingRect.width());
        QVERIFY(updatedBoundingRect.height() < initialBoundingRect.height()); // 缩小高度
        qDebug() << "Updated bounding rect:" << updatedBoundingRect;

        // 模拟旋转操作
        chartItem->setRotation(45.0);
        QCOMPARE(chartItem->rotation(), 45.0);
    }


    void testDragMoveChart()
    {
        // 获取视图和场景
        View* view = mainWindow->findChild<View*>("graphicsView");
        QVERIFY(view);

        // 创建新场景并设置到视图中
        Scene* scene = new Scene(view);
        view->setScene(scene);
        QVERIFY(scene);


        // 添加图形项
        QPointF startPosition(100, 100);
        view->addChartItem(FlowEnumItem::StartOrEnd, startPosition);

        QGraphicsItem* addedItem = nullptr;
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == ChartItem::Type)
            {
                addedItem = item;
                break;
            }
        }
        QVERIFY(addedItem != nullptr);

        // 验证图形项的初始位置
        QCOMPARE(addedItem->pos(), startPosition);

        // 设置图形项为可移动
        addedItem->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

        // 设置图形项为选中
        addedItem->setSelected(true);
        QVERIFY(addedItem->isSelected()); // 验证图形项是否被选中

        // 模拟鼠标拖动操作
        QPointF newPosition(200, 200);

        // 模拟鼠标按下事件
        QGraphicsSceneMouseEvent pressEvent(QEvent::GraphicsSceneMousePress);
        pressEvent.setScenePos(startPosition);
        pressEvent.setButton(Qt::LeftButton);
        pressEvent.setButtons(Qt::LeftButton);
        pressEvent.setModifiers(Qt::NoModifier);
        QApplication::sendEvent(scene, &pressEvent);

        // 模拟鼠标移动事件
        QGraphicsSceneMouseEvent moveEvent(QEvent::GraphicsSceneMouseMove);
        moveEvent.setScenePos(newPosition);
        moveEvent.setLastScenePos(startPosition);
        moveEvent.setButton(Qt::LeftButton);
        moveEvent.setButtons(Qt::LeftButton);
        moveEvent.setModifiers(Qt::NoModifier);
        QApplication::sendEvent(scene, &moveEvent);

        // 模拟鼠标释放事件
        QGraphicsSceneMouseEvent releaseEvent(QEvent::GraphicsSceneMouseRelease);
        releaseEvent.setScenePos(newPosition);
        releaseEvent.setButton(Qt::LeftButton);
        releaseEvent.setButtons(Qt::LeftButton);
        releaseEvent.setModifiers(Qt::NoModifier);
        QApplication::sendEvent(scene, &releaseEvent);

        // 等待事件完成
        QTest::qWait(100);

        // 验证图形项的新位置
        QCOMPARE(addedItem->pos(), newPosition);
    }*/

    /*void testSelectMultipleAndBatchOperation()
    {
        // 获取视图和场景
        View* view = mainWindow->findChild<View*>("graphicsView");
        QVERIFY(view);

        // 在每次测试之前重新创建一个新的场景
        Scene* scene = new Scene(view);
        view->setScene(scene); // 将新场景设置到视图中
        scene->clearAllItems();
        QVERIFY(scene);

        // 添加图形元素
        FlowLayout* flowLayout = mainWindow->findChild<FlowLayout*>("flowLayout");
        QVERIFY(flowLayout);

        // 添加第一个图形
        ChartButton* chartButton = mainWindow->findChild<ChartButton*>("1", Qt::FindChildrenRecursively);
        QVERIFY(chartButton);
        QTest::mouseClick(chartButton, Qt::LeftButton); // 点击按钮，选择图形
        QGraphicsSceneMouseEvent eventPress1(QEvent::GraphicsSceneMousePress);
        eventPress1.setScenePos(QPointF(100, 100)); // 设置插入位置
        eventPress1.setButton(Qt::LeftButton);
        scene->mousePressEvent(&eventPress1);

        // 添加第二个图形
        QTest::mouseClick(chartButton, Qt::LeftButton); // 点击按钮，选择图形
        QGraphicsSceneMouseEvent eventPress2(QEvent::GraphicsSceneMousePress);
        eventPress2.setScenePos(QPointF(200, 200)); // 设置插入位置
        eventPress2.setButton(Qt::LeftButton);
        scene->mousePressEvent(&eventPress2);

        // 模拟鼠标拖动框选操作
        QPointF dragStartPos(50, 50);  // 框选起点
        QPointF dragEndPos(250, 250); // 框选终点

        QGraphicsSceneMouseEvent eventPress(QEvent::GraphicsSceneMousePress);
        eventPress.setScenePos(dragStartPos); // 设置鼠标按下位置
        eventPress.setButton(Qt::LeftButton);
        scene->mousePressEvent(&eventPress);

        QGraphicsSceneMouseEvent eventMove(QEvent::GraphicsSceneMouseMove);
        eventMove.setScenePos(dragEndPos); // 设置鼠标拖动到的位置
        scene->mouseMoveEvent(&eventMove);

        QGraphicsSceneMouseEvent eventRelease(QEvent::GraphicsSceneMouseRelease);
        eventRelease.setScenePos(dragEndPos); // 设置鼠标释放位置
        eventRelease.setButton(Qt::LeftButton);
        scene->mouseReleaseEvent(&eventRelease);
        // 等待事件完成
        QTest::qWait(200);
        // 验证框选的图形数量
        int graphicsItemCount = 0;
        QList<QGraphicsItem*> itemsToSelect; // 保存框选的项目

        for (QGraphicsItem* item : scene->items())
        {
            QVERIFY(item != nullptr);
            // 判断是否为目标类型并与框选区域相交
            if (item->type() == ChartItem::Type || item->type() == TextItem::Type)
            {
                if (QRectF(dragStartPos, dragEndPos).intersects(item->sceneBoundingRect()))
                {
                    graphicsItemCount++;
                    itemsToSelect.append(item); // 添加到框选列表
                }
            }
        }

        // 验证框选区域内的图形数量是否正确
        QCOMPARE(graphicsItemCount, 4); // 预期框选两个图形

        // 模拟复制功能
        QAction* copyAction = mainWindow->findChild<QAction*>("copyAction");
        QVERIFY(copyAction);
        copyAction->trigger(); // 触发复制功能

        // 模拟粘贴功能
        QAction* pasteAction = mainWindow->findChild<QAction*>("pasteAction");
        QVERIFY(pasteAction);
        pasteAction->trigger(); // 触发粘贴功能

        // 等待事件完成
        QTest::qWait(200);

        // 验证粘贴的项目是否正确存在
        QList<QGraphicsItem*> pastedItems;
        for (QGraphicsItem* item : scene->items())
        {
            if ((item->type() == ChartItem::Type || item->type() == TextItem::Type) && !itemsToSelect.contains(item))
            {
                pastedItems.append(item);
            }
        }

        // 防御性检查：确保粘贴项目的数量正确
        QCOMPARE(pastedItems.count(), 2);

        // 验证粘贴项目的位置偏移
        QCOMPARE(pastedItems[0]->pos(), itemsToSelect[0]->pos() + QPointF(10, 10)); // 默认粘贴偏移
        QCOMPARE(pastedItems[1]->pos(), itemsToSelect[1]->pos() + QPointF(10, 10));
    }*/

    void testChangeChartColors()
    {
        // 获取视图和场景
        View* view = mainWindow->findChild<View*>("graphicsView");
        QVERIFY(view);
        // 在每次测试之前重新创建一个新的场景
        Scene* scene = new Scene(view);
        view->setScene(scene); // 将新场景设置到视图中
        QVERIFY(scene);

        OperationStack* operationStack = view->operationStack;
        QVERIFY(operationStack);

        int initUndoCount= operationStack->getUndoCount();
        int initRedoCount = operationStack->getRedoCount();

        // 添加图形项
        QPointF startPosition(100, 100);
        view->addChartItem(FlowEnumItem::StartOrEnd, startPosition);

        ChartItem* chartItem = nullptr;
        for (QGraphicsItem* item : scene->items()) {
            if (item->type() == ChartItem::Type) {
                chartItem = dynamic_cast<ChartItem*>(item);
                break;
            }
        }
        QVERIFY(chartItem != nullptr);
        chartItem->setSelected(true);

        // 获取初始路径
        QString initialPath = chartItem->getCurrentPath();
        QVERIFY(!initialPath.isEmpty());

        // 验证添加图形项是否增加了一个操作
        QCOMPARE(operationStack->getUndoCount(), initUndoCount+1);

        // 模拟更改填充颜色
        QToolButton* fillColorToolButton = mainWindow->findChild<QToolButton*>("fillColorToolButton");
        QVERIFY(fillColorToolButton);

        QAction* yellowAction = fillColorToolButton->menu()->actions().at(3); // 黄色
        QVERIFY(yellowAction);

        yellowAction->trigger(); // 触发黄色填充颜色修改
        QTest::qWait(100);

        QCOMPARE(chartItem->getCurrentFillColor(), "y");

        QString updatedPathFill = chartItem->getCurrentPath();
        QVERIFY(initialPath != updatedPathFill);

        // 验证操作栈状态
        QCOMPARE(operationStack->getUndoCount(), initUndoCount+2);

        // 模拟更改边框颜色
        QToolButton* borderColorToolButton = mainWindow->findChild<QToolButton*>("borderColorToolButton");
        QVERIFY(borderColorToolButton);

        QAction* redAction = borderColorToolButton->menu()->actions().at(0); // 红色
        QVERIFY(redAction);

        redAction->trigger(); // 触发红色边框颜色修改
        QTest::qWait(100);

        QCOMPARE(chartItem->getCurrentBorderColor(), "r");

        QString updatedPathBorder = chartItem->getCurrentPath();
        QVERIFY(updatedPathFill != updatedPathBorder);

        // 验证操作栈状态
        QCOMPARE(operationStack->getUndoCount(), initUndoCount+3);

        // 撤销操作
        QAction* undoAction = mainWindow->findChild<QAction*>("undoAction");
        QVERIFY(undoAction);

        undoAction->trigger();
        QTest::qWait(100);
        QCOMPARE(chartItem->getCurrentPath(), updatedPathFill);

        undoAction->trigger();
        QTest::qWait(100);
        QCOMPARE(chartItem->getCurrentPath(), initialPath);

        // 验证操作栈状态
        QCOMPARE(operationStack->getUndoCount(), initUndoCount+1);
        QCOMPARE(operationStack->getRedoCount(), initRedoCount+2);

        // 重做操作
        QAction* redoAction = mainWindow->findChild<QAction*>("redoAction");
        QVERIFY(redoAction);

        redoAction->trigger();
        QTest::qWait(100);
        QCOMPARE(chartItem->getCurrentPath(), updatedPathFill);

        redoAction->trigger();
        QTest::qWait(100);
        QCOMPARE(chartItem->getCurrentPath(), updatedPathBorder);

        // 最终操作栈状态验证
        QCOMPARE(operationStack->getUndoCount(), initUndoCount+3);
        QCOMPARE(operationStack->getRedoCount(), initRedoCount);
    }

    void testInsertEditText()
    {
        // 获取插入文本的 QAction
        QAction* insertTextAction = mainWindow->findChild<QAction*>("insertTextAction");
        QVERIFY(insertTextAction);

        // 获取撤销和重做的 QAction
        QAction* undoAction = mainWindow->findChild<QAction*>("undoAction");
        QAction* redoAction = mainWindow->findChild<QAction*>("redoAction");
        QVERIFY(undoAction);
        QVERIFY(redoAction);

        // 获取视图和场景
        View* view = mainWindow->findChild<View*>("graphicsView");
        QVERIFY(view);

        // 创建新场景并设置到视图中
        Scene* scene = new Scene(view);
        view->setScene(scene);
        QVERIFY(scene);

        // 初始检查文本项数量
        int initialTextCount = scene->allTexts.count();

        // 触发插入文本操作
        insertTextAction->trigger();

        // 模拟鼠标点击场景，插入文本
        QPointF testPosition(100, 100); // 测试文本插入位置
        QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMousePress);
        mouseEvent.setScenePos(testPosition);
        mouseEvent.setButton(Qt::LeftButton);
        mouseEvent.setButtons(Qt::LeftButton);
        scene->mousePressEvent(&mouseEvent);

        // 检查场景是否增加了新的文本项
        QCOMPARE(scene->allTexts.count(), initialTextCount + 1);

        // 获取新插入的文本项
        TextItem* textItem = scene->allTexts.last();
        QVERIFY(textItem != nullptr);
        QCOMPARE(textItem->pos(), testPosition);

        // 默认文本验证
        QCOMPARE(textItem->toPlainText(), QString("文本"));

        // 撤销插入操作
        undoAction->trigger();

        // 检查撤销后文本项数量是否恢复
        QCOMPARE(scene->allTexts.count(), initialTextCount);

        // 检查场景中已无插入的文本项
        QVERIFY(!scene->items().contains(textItem));

        // 重做插入操作
        redoAction->trigger();

        // 检查重做后文本项数量是否恢复
        QCOMPARE(scene->allTexts.count(), initialTextCount + 1);

        // 获取重做后的文本项
        TextItem* redoneTextItem = scene->allTexts.last();
        QVERIFY(redoneTextItem != nullptr);

        // 验证文本和位置是否一致
        QCOMPARE(redoneTextItem->toPlainText(), QString("文本"));
        QCOMPARE(redoneTextItem->pos(), testPosition);

        // 设置文本互动标志，确保可以编辑
        redoneTextItem->setTextInteractionFlags(Qt::TextEditorInteraction);

        // 模拟双击文本框，进入编辑模式
        QTest::mouseDClick(view->viewport(), Qt::LeftButton, Qt::NoModifier, view->mapFromScene(redoneTextItem->pos()));
        QVERIFY(redoneTextItem->textInteractionFlags() == Qt::TextEditorInteraction);

        // 确保焦点在文本框内
        redoneTextItem->setFocus();

        // 模拟输入新文本
        QTest::keyClicks(view->viewport(), "Updated Text");

        // 模拟焦点离开以触发 focusOutEvent
        redoneTextItem->clearFocus();

        // 检查新文本是否正确更新
        QCOMPARE(redoneTextItem->toPlainText(), QString("Updated Text"));

        // 撤销编辑操作
        undoAction->trigger();

        // 检查撤销后是否恢复到默认文本
        QCOMPARE(redoneTextItem->toPlainText(), QString("文本"));

        // 重做编辑操作
        redoAction->trigger();

        // 检查重做后是否恢复到更新的文本
        QCOMPARE(redoneTextItem->toPlainText(), QString("Updated Text"));
    }

    void testTextFormatSettings()
    {
        // 获取视图和场景
        View* view = mainWindow->findChild<View*>("graphicsView");
        QVERIFY(view);

        // 创建新场景并设置到视图中
        Scene* scene = new Scene(view);
        view->setScene(scene);
        QVERIFY(scene);
        // 添加图形元素和关联文本框
        QPointF chartPosition(150, 150);
        view->addChartItem(FlowEnumItem::StartOrEnd, chartPosition);

        // 找到刚刚添加的文本项
        TextItem* textItem = nullptr;
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == TextItem::Type)
            {
                textItem = dynamic_cast<TextItem*>(item);
                break;
            }
        }
        QVERIFY(textItem != nullptr);

        // 设置新的字体、大小和颜色
        QFont newFont("Arial", 16);
        QColor newColor = Qt::blue;

        textItem->setFont(newFont);
        textItem->setDefaultTextColor(newColor);

        // 验证字体和颜色是否正确更新
        QCOMPARE(textItem->font(), newFont);
        QCOMPARE(textItem->defaultTextColor(), newColor);
    }

    void testTextAssociationWithChart()
    {
        // 获取视图和场景
        View* view = mainWindow->findChild<View*>("graphicsView");
        QVERIFY(view);

        // 在每次测试之前重新创建一个新的场景
        Scene* scene = new Scene(view);
        view->setScene(scene); // 将新场景设置到视图中
        QVERIFY(scene);
        // 添加图形项
        QPointF startPosition(100, 100);
        view->addChartItem(FlowEnumItem::StartOrEnd, startPosition);

        // 找到刚刚添加的图形项和文本项
        ChartItem* chartItem = nullptr;
        TextItem* textItem = nullptr;
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == ChartItem::Type)
            {
                chartItem = dynamic_cast<ChartItem*>(item);
            }
            else if (item->type() == TextItem::Type)
            {
                textItem = dynamic_cast<TextItem*>(item);
            }
        }
        QVERIFY(chartItem != nullptr);
        QVERIFY(textItem != nullptr);

        // 确认文本内容是否正确
        QCOMPARE(textItem->toPlainText(), QString("开始或结束"));

        // 检查文本的初始位置是否正确
        QTest::qWait(100); // 等待事件循环以确保所有更新已完成
        QPointF initialTextPosition = textItem->pos();

        // 计算预期位置，遵循 updatePosition 的逻辑
        QRectF flowBoundingRegion = chartItem->boundingRegion(chartItem->transform()).boundingRect();
        QPointF flowCenter = flowBoundingRegion.center() + chartItem->pos();

        QRectF textBoundingRect = textItem->boundingRect();
        QPointF textCenter = textBoundingRect.center() + textItem->pos();

        QPointF expectedTextPosition = textItem->pos() + (flowCenter - textCenter);

        qreal tolerance = 1.0; // 容差范围
        QVERIFY(qAbs(initialTextPosition.x() - expectedTextPosition.x()) <= tolerance);
        QVERIFY(qAbs(initialTextPosition.y() - expectedTextPosition.y()) <= tolerance);

        // 移动图形项
        QPointF newChartPosition(300, 300);
        chartItem->setPos(newChartPosition);

        // 等待事件循环以确保更新
        QTest::qWait(100);

        // 验证图形位置是否已更新
        QCOMPARE(chartItem->pos(), newChartPosition);

        // 验证文本位置是否同步更新
        QPointF updatedTextPosition = textItem->pos();
        flowCenter = chartItem->boundingRegion(chartItem->transform()).boundingRect().center() + chartItem->pos();
        textCenter = textItem->boundingRect().center() + textItem->pos();
        expectedTextPosition = textItem->pos() + (flowCenter - textCenter);

        QVERIFY(qAbs(updatedTextPosition.x() - expectedTextPosition.x()) <= tolerance);
        QVERIFY(qAbs(updatedTextPosition.y() - expectedTextPosition.y()) <= tolerance);
    }

    void testTextOperations()
    {
        // 获取视图和场景
        View* view = mainWindow->findChild<View*>("graphicsView");
        QVERIFY(view);

        Scene* scene = new Scene(view);
        view->setScene(scene);
        QVERIFY(scene);

        OperationStack* operationStack = view->operationStack;
        QVERIFY(operationStack);
        // 获取需要测试的控件
        QTextEdit* searchEdit = mainWindow->findChild<QTextEdit*>("searchEdit");
        QTextEdit* replaceEdit = mainWindow->findChild<QTextEdit*>("replaceEdit");
        QPushButton* searchButton = mainWindow->findChild<QPushButton*>("searchButton");
        QPushButton* nextButton = mainWindow->findChild<QPushButton*>("nextButton");
        QPushButton* lastButton = mainWindow->findChild<QPushButton*>("lastButton");
        QPushButton* allButton = mainWindow->findChild<QPushButton*>("allButton");
        QPushButton* replaceButton = mainWindow->findChild<QPushButton*>("replaceButton");
        QAction* undoAction = mainWindow->findChild<QAction*>("undoAction");
        QAction* redoAction = mainWindow->findChild<QAction*>("redoAction");
        QAction* insertTextAction = mainWindow->findChild<QAction*>("insertTextAction");

        QVERIFY(insertTextAction);
        QVERIFY(searchEdit);
        QVERIFY(replaceEdit);
        QVERIFY(searchButton);
        QVERIFY(nextButton);
        QVERIFY(lastButton);
        QVERIFY(allButton);
        QVERIFY(replaceButton);
        QVERIFY(undoAction);
        QVERIFY(redoAction);

        // 插入文本框的辅助函数
        auto insertTextItem = [&](const QPointF& position, const QString& text) -> TextItem* {
            // 触发插入文本的 QAction
            insertTextAction->trigger();

            // 模拟鼠标点击插入位置
            QTest::mouseClick(view->viewport(), Qt::LeftButton, Qt::NoModifier, view->mapFromScene(position));

            // 查找插入的文本项
            for (QGraphicsItem* item : scene->items())
            {
                if (auto t = dynamic_cast<TextItem*>(item))
                {
                    if (t->toPlainText() == "文本") // 默认文本内容
                    {
                        t->setPlainText(text); // 设置文本内容
                        return t;
                    }
                }
            }
            return nullptr; // 如果没有找到文本项，则返回 nullptr
        };

        // 测试插入文本框
        TextItem* textItem1 = insertTextItem(QPointF(50, 50), "TestText1");
        QVERIFY(textItem1); // 确保成功插入

        TextItem* textItem2 = insertTextItem(QPointF(100, 100), "TestText2");
        QVERIFY(textItem2); // 确保成功插入

        TextItem* textItem3 = insertTextItem(QPointF(150, 150), "TestText1");
        QVERIFY(textItem3); // 确保成功插入

        // 模拟搜索操作
        searchEdit->setPlainText("TestText1");
        QTest::mouseClick(searchButton, Qt::LeftButton);

        // 验证只选中第一个匹配的文本框
        QVERIFY(textItem1->isSelected());
        QVERIFY(!textItem2->isSelected());
        QVERIFY(!textItem3->isSelected());

        // 模拟查找下一个文本框
        QTest::mouseClick(nextButton, Qt::LeftButton);
        QVERIFY(!textItem1->isSelected());
        QVERIFY(!textItem2->isSelected());
        QVERIFY(textItem3->isSelected());

        // 模拟查找上一个文本框
        QTest::mouseClick(lastButton, Qt::LeftButton);
        QVERIFY(textItem1->isSelected());
        QVERIFY(!textItem2->isSelected());
        QVERIFY(!textItem3->isSelected());

        // 模拟选中所有匹配的文本框
        QTest::mouseClick(allButton, Qt::LeftButton);
        QVERIFY(textItem1->isSelected());
        QVERIFY(!textItem2->isSelected());
        QVERIFY(textItem3->isSelected());

        // 模拟替换操作
        replaceEdit->setPlainText("UpdatedText");
        QTest::mouseClick(replaceButton, Qt::LeftButton);

        // 验证替换结果
        QCOMPARE(textItem1->toPlainText(), "UpdatedText");
        QCOMPARE(textItem3->toPlainText(), "UpdatedText");
        QCOMPARE(textItem2->toPlainText(), "TestText2"); // 不匹配的文本框内容不变
        // 验证撤销操作
        undoAction->trigger();
        QCOMPARE(textItem1->toPlainText(), "TestText1");
        QCOMPARE(textItem3->toPlainText(), "TestText1");

        // 验证重做操作
        redoAction->trigger();
        QCOMPARE(textItem1->toPlainText(), "UpdatedText");
        QCOMPARE(textItem3->toPlainText(), "UpdatedText");
    }

    /*void testConnectCharts()
    {
        // 获取视图和场景
        View* view = mainWindow->findChild<View*>("graphicsView");
        QVERIFY(view);

        // 创建新场景
        Scene* scene = new Scene(view);
        view->setScene(scene);

        // 添加两个图形元素
        QPointF position1(50, 50);
        QPointF position2(150, 150);
        view->addChartItem(FlowEnumItem::Flow1, position1);
        view->addChartItem(FlowEnumItem::Flow2, position2);

        ChartItem* item1 = nullptr;
        ChartItem* item2 = nullptr;

        // 查找图形元素
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == ChartItem::Type)
            {
                if (!item1)
                    item1 = dynamic_cast<ChartItem*>(item);
                else
                    item2 = dynamic_cast<ChartItem*>(item);
            }
        }
        QVERIFY(item1);
        QVERIFY(item2);

        // 进入连线模式
        view->graphicsScene->setMode(Mode::InsertLine);

        // 验证是否进入了连线模式
        QCOMPARE(view->graphicsScene->getMode(), Mode::InsertLine);

        // 获取 item1 的左边缘（可以修改为其他边缘）
        QPointF item1EdgePoint = item1->sceneBoundingRect().topLeft() + QPointF(item1->sceneBoundingRect().width() / 2, 0);

        // 获取 item2 的中心点
        QPointF item2CenterPoint = item2->sceneBoundingRect().center();

        // 使用连线工具连接两个元素，从 item1 的边缘开始
        QTest::mousePress(view->viewport(), Qt::LeftButton, Qt::NoModifier, view->mapFromScene(item1EdgePoint));
        QTest::mouseRelease(view->viewport(), Qt::LeftButton, Qt::NoModifier, view->mapFromScene(item2CenterPoint));

        // 验证连线是否正确
        LineItem* lineItem = nullptr;
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == LineItem::Type)
            {
                lineItem = dynamic_cast<LineItem*>(item);
                break;
            }
        }
        QVERIFY(lineItem);
        QCOMPARE(lineItem->startItem, item1);
        QCOMPARE(lineItem->endItem, item2);

        // 确保连线起点为 item1 的边缘位置，终点为 item2 的中心位置
        QCOMPARE(lineItem->line().p1(), item1EdgePoint);
        QCOMPARE(lineItem->line().p2(), item2CenterPoint);

        // 移动第一个元素，验证连线是否随动
        QPointF newPosition1(100, 100);
        item1->setPos(newPosition1);
        QTest::qWait(100);

        // 检查连线是否已经更新
        QCOMPARE(lineItem->line().p1(), item1EdgePoint);
        QCOMPARE(lineItem->line().p2(), item2CenterPoint);
    }

    void testAlignmentAndSnapping()
    {
        // 获取视图和场景
        View* view = mainWindow->findChild<View*>("graphicsView");
        QVERIFY(view);

        // 创建新场景
        Scene* scene = new Scene(view);
        view->setScene(scene);

        // 添加两个图形元素
        QPointF position1(50, 50);
        QPointF position2(150, 150);
        view->addChartItem(FlowEnumItem::Flow1, position1);
        view->addChartItem(FlowEnumItem::Flow2, position2);

        ChartItem* item1 = nullptr;
        ChartItem* item2 = nullptr;

        // 查找图形元素
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == ChartItem::Type)
            {
                if (!item1)
                    item1 = dynamic_cast<ChartItem*>(item);
                else
                    item2 = dynamic_cast<ChartItem*>(item);
            }
        }
        QVERIFY(item1);
        QVERIFY(item2);

        // 移动第一个图形靠近第二个图形
        QPointF movePosition(145, 145);
        QTest::mousePress(view->viewport(), Qt::LeftButton, Qt::NoModifier, view->mapFromScene(item1->sceneBoundingRect().center()));
        QTest::mouseMove(view->viewport(), view->mapFromScene(movePosition));
        QTest::mouseRelease(view->viewport(), Qt::LeftButton, Qt::NoModifier, view->mapFromScene(movePosition));

        // 验证是否显示辅助线
        QVERIFY(scene->isShowingAlignmentLines());

        // 松开鼠标，验证吸附效果
        QPointF expectedPosition(150, 150); // 预期吸附到item2的位置
        QCOMPARE(item1->pos(), expectedPosition);
    }*/

    void testSaveAndLoadXML()
    {
        // 获取视图和场景
        View* view = mainWindow->findChild<View*>("graphicsView");
        QVERIFY(view);

        // 创建新场景并设置到视图中
        Scene* scene = new Scene(view);
        view->setScene(scene);
        QVERIFY(scene);

        // 创建图形项
        QPointF chartPosition(100, 100);
        ChartItem* chartItem = new ChartItem();
        chartItem->setPos(chartPosition);

        // 设置图形项的填充颜色和边框颜色
        QString fillColor = "y";
        QString borderColor = "r";
        chartItem->setCurrentFillColor(fillColor);
        chartItem->setCurrentBorderColor(borderColor);

        scene->addItem(chartItem);

        // 创建文本项
        QPointF textPosition(300, 300);
        TextItem* textItem = new TextItem();
        textItem->setPlainText("Test Text");
        QFont textFont("Arial", 16);
        textItem->setFont(textFont);
        textItem->setDefaultTextColor(Qt::blue);
        textItem->setPos(textPosition);
        scene->addItem(textItem);

        // 验证图形和文本项的初始属性
        QCOMPARE(chartItem->pos(), chartPosition);
        QCOMPARE(chartItem->getCurrentFillColor(), fillColor);
        QCOMPARE(chartItem->getCurrentBorderColor(), borderColor);
        QCOMPARE(textItem->toPlainText(), QString("Test Text"));
        QCOMPARE(textItem->font(), textFont);
        QCOMPARE(textItem->defaultTextColor(), Qt::blue);

        // 保存场景为 XML 文件
        QString testFilePath = "testScene.xml";
        mainWindow->writeXMLFile(testFilePath, "TestTab", "TestGUID", scene);

        // 清空场景
        scene->clear();
        QVERIFY(scene->items().isEmpty());

        // 重新加载 XML 文件
        mainWindow->readXMLFile(testFilePath, 0);

        // 验证加载后的场景内容
        ChartItem* loadedChartItem = nullptr;
        TextItem* loadedTextItem = nullptr;
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == ChartItem::Type)
            {
                loadedChartItem = dynamic_cast<ChartItem*>(item);
            }
            else if (item->type() == TextItem::Type && item->pos() == textPosition)
            {
                loadedTextItem = dynamic_cast<TextItem*>(item);
            }
        }

        // 验证加载后的图形项
        QVERIFY(loadedChartItem != nullptr);
        QCOMPARE(loadedChartItem->pos(), chartPosition);
        QCOMPARE(loadedChartItem->getCurrentFillColor(), fillColor);
        QCOMPARE(loadedChartItem->getCurrentBorderColor(), borderColor);

        // 验证加载后的文本项
        QVERIFY(loadedTextItem != nullptr);
        //QCOMPARE(loadedTextItem->toPlainText(), QString("Test Text"));
        QCOMPARE(loadedTextItem->font(), textFont);
        QCOMPARE(loadedTextItem->defaultTextColor(), Qt::blue);

        // 删除测试文件
        QFile::remove(testFilePath);
    }

    void testZoomView()
    {
        // 获取视图和场景
        View* view = mainWindow->findChild<View*>("graphicsView");
        QVERIFY(view);

        // 创建新场景并设置到视图中
        Scene* scene = new Scene(view);
        view->setScene(scene);
        QVERIFY(scene);
        // 添加图形项
        QPointF startPosition(100, 100);
        view->addChartItem(FlowEnumItem::StartOrEnd, startPosition);

        QGraphicsItem* addedItem = nullptr;
        for (QGraphicsItem* item : scene->items())
        {
            if (item->type() == ChartItem::Type)
            {
                addedItem = item;
                break;
            }
        }
        QVERIFY(addedItem != nullptr);

        // 获取初始缩放比例
        QTransform initialTransform = view->transform();

        // 模拟鼠标滚轮放大
        QPointF zoomCenter = view->mapToScene(view->viewport()->rect().center());
        QWheelEvent zoomInEvent(view->mapFromScene(zoomCenter), 120, Qt::NoButton, Qt::NoModifier, Qt::Vertical);
        QApplication::sendEvent(view->viewport(), &zoomInEvent);

        // 验证视图缩放比例是否增加
        QVERIFY(view->transform().m11() > initialTransform.m11());

        // 模拟鼠标滚轮缩小
        QWheelEvent zoomOutEvent(view->mapFromScene(zoomCenter), -120, Qt::NoButton, Qt::NoModifier, Qt::Vertical);
        QApplication::sendEvent(view->viewport(), &zoomOutEvent);

        // 验证视图缩放比例是否恢复
        QCOMPARE(view->transform().m11(), initialTransform.m11());
    }

    void testCloseTab()
    {
        QTabWidget* tabWidget = mainWindow->findChild<QTabWidget*>("tabWidget");
        QVERIFY(tabWidget);

        int initialTabCount = tabWidget->count();

        if (initialTabCount > 0)
        {
            QMetaObject::invokeMethod(mainWindow, "closeTab", Q_ARG(int, 0));
            QCOMPARE(tabWidget->count(), initialTabCount - 1);
        }
    }
};
//QTEST_MAIN(TestMainWindow)
//#include "testmainwindow.moc"
