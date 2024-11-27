#include "global.h"

// Global 类的构造函数
Global::Global()
{
    // 构造函数体为空，因为没有需要初始化的成员变量
}

// 静态成员变量的初始化
QFont Global::globalFont = QFont();    // 初始化全局字体为默认字体
QColor Global::globalColor = QColor(); // 初始化全局颜色为默认颜色
