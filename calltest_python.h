#pragma once
#pragma execution_character_set("utf-8")
#include <QMainWindow>
#include <QProcess>      // 核心：用于调用外部程序
#include <QFileDialog>   // 用于选择文件
#include <QMessageBox>   // 用于弹窗提示
#include <QDebug>
#include <QtWidgets/QMainWindow>
#include "ui_calltest_python.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CallTest_PythonClass; };
QT_END_NAMESPACE

class CallTest_Python : public QMainWindow
{
    Q_OBJECT

public:
    CallTest_Python(QWidget *parent = nullptr);
    ~CallTest_Python();

private slots:
    void on_btnDownload_clicked(); // 绑定到界面按钮的槽函数
    void onProcessOutput();        // 读取 Python 的 print 输出
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus); // 任务结束
    void satelliteInformation(int index);

private:
    QProcess* m_process; // 将 process 定义为成员变量，防止函数结束后被销毁

private:
    Ui::CallTest_PythonClass *ui;
};

