#ifndef OPERATIONSTACK_H
#define OPERATIONSTACK_H

#include <QStack>
#include <QMessageBox>

#include "operation.h"

class OperationStack:public QObject
{
    Q_OBJECT
public:
    OperationStack(QObject *parent = nullptr);

    QStack<Operation*>* undoStack = new QStack<Operation*>();   // 撤销操作的栈
    QStack<Operation*>* redoStack = new QStack<Operation*>();   // 重做操作的栈

    void addOperation(Operation* operation);                    // 将操作压入撤销栈
    void undo();                                                // 撤销栈操作出栈
    void redo();                                                // 重做栈操作出栈
    void clearall();                                            // 清空栈
    int getUndoCount() const { return undoStack->count(); }
    int getRedoCount() const { return redoStack->count(); }
signals:
    void countChange(int undoCount,int redoCount);              // 栈中数量发生改变时发送信号
};

#endif // OPERATIONSTACK_H
