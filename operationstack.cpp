#include "operationstack.h"

OperationStack::OperationStack(QObject* parent)
    : QObject(parent)
{}

// 将操作压入撤销栈
void OperationStack::addOperation(Operation* _com)
{
    redoStack->clear();  // 清空重做栈
    undoStack->push(_com);  // 将命令压入撤销栈
    emit countChange(undoStack->count(), redoStack->count());  // 发射信号通知栈数量更新
}

// 执行撤销操作
void OperationStack::undo()
{
    if (undoStack->count() == 0)                // 检查撤销栈是否为空
    {
        QMessageBox::information(dynamic_cast<QWidget*>(this->parent()), tr("提示"), tr("无可撤销项!"));  // 提示用户无可撤销项
        return;
    }
    Operation* operation = undoStack->pop();    // 从撤销栈中弹出操作
    operation->undo();                          // 执行撤销操作
    redoStack->push(operation);                 // 将对应的重做操作压入重做栈
    emit countChange(undoStack->count(), redoStack->count());
}

// 执行重做操作
void OperationStack:: redo()
{
    if (redoStack->count() == 0)                // 检查重做栈是否为空
    {
        QMessageBox::information(dynamic_cast<QWidget*>(this->parent()), tr("提示"), tr("无可执行项!"));  // 提示用户无可执行项
        return;
    }
    Operation* com = redoStack->pop();          // 从重做栈中弹出操作
    com->redo();                                // 执行重做操作
    undoStack->push(com);                       // 将对应的撤销操作压入撤销栈
    emit countChange(undoStack->count(), redoStack->count());
}

// 清空撤销栈和重做栈的所有操作
void OperationStack::clearall()
{
    redoStack->clear();                                         // 清空重做栈
    undoStack->clear();                                         // 清空撤销栈
    emit countChange(undoStack->count(), redoStack->count());   // 发射信号通知栈数量更新
}
