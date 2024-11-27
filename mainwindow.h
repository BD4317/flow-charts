#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QInputDialog>
#include <QClipboard>
#include <QFontDialog>
#include <QMenuBar>
#include <QDoubleSpinBox>
#include <QColorDialog>

#include "flowlayout.h"
#include "chartbutton.h"
#include "view.h"
#include "textitem.h"
#include "pixmapitem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow;
             class QAction;
             class QMenuBar;}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    void loadChartView();                                                               // 加载流程图图形视图
    void createActions();                                                               // 定义Action
    void createMenus();                                                                 // 创建菜单
    void createToolBar();                                                               // 创建工具栏
    int addTabWidgetPage(QString pageName);                                             // 添加单页

    QAction* openAction;                                                                // 打开XML文件
    QAction* saveAction;                                                                // 保存XML文件
    QAction* saveAsAction;                                                              // 另存为XML文件
    QAction* newAction;                                                                 // 添加新页
    QAction* copyAction;                                                                // 复制
    QAction* pasteAction;                                                               // 粘贴
    QAction* cutAction;                                                                 // 剪切
    QAction* deleteAction;                                                              // 删除
    QAction* insertLineAction;                                                          // 插入连接线
    QAction* insertTextAction;                                                          // 插入文本
    QAction* undoAction;                                                                // 撤销
    QAction* redoAction;                                                                // 重做
    QAction* exitAction;
    QAction* aboutAction;
    QAction* bgAction;
    QAction* addAction;

    QMenu* fileMenu;                                                                    // 文件菜单
    QMenu* editMenu;                                                                    // 编辑菜单
    QMenu* toolMenu;                                                                    // 工具菜单
    QMenu* aboutMenu;

public slots:
    void insertChart(FlowEnumItem type);                                                // 插入图形
    void readXMLFile(QString filePath, int tabIndex);                                   // 读XML文件
    void writeXMLFile(QString filePath, QString tabName, QString guid, Scene* scene);   // 写XML文件
    void openXMLFile();                                                                 // 打开XML文件
    void saveXMLFile();                                                                 // 保存文件为XML格式
    void saveAsXMLFile();                                                               // 文件另存为XML格式
    void createNewPage();                                                               // 创建新页
    void scaleValueChanged(double scaleMultiple);                                       // 缩放程度改变引发控件显示的值发生改变
    void searchText();                                                                  // 搜索指定的文本
    void replaceText();                                                                 // 替换指定的文本
    void lastText();                                                                    // 上一个文本
    void nextText();                                                                    // 下一个文本
    void allText();                                                                     // 选中全部文本
    void saveSVG();                                                                     // 保存文件为SVG格式
    void showMenu(const QPoint& position);                                              // 右键展示菜单
    void deleteSelect();                                                                // 删除选中的元素
    void copySelect();                                                                  // 复制选中的元素
    void pasteSelect();                                                                 // 粘贴选中的元素
    void cutSelect();                                                                   // 剪切选中的元素
    void insertLine();                                                                  // 插入连接线
    void insertText();                                                                  // 插入文本
    void undo();                                                                        // 撤销
    void redo();                                                                        // 重做
    void selectFillColor();                                                             // 填充颜色选择
    void selectBorderColor();                                                           // 边框颜色选择
    void setFont();                                                                     // 设置字体
    void closeTab(int index);                                                           // 删除页面
    void setFontColor();                                                                // 设置字体颜色
    void about();
    void bg();
    void addPixmapItem();
    void selectBook();
    void selectDecision();
    void selectPay();
};

#endif // MAINWINDOW_H
