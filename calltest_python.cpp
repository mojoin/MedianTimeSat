#pragma execution_character_set("utf-8")
#include "calltest_python.h"
#include <QStyledItemDelegate>
#include <QListView>
#include <QDebug>
#include <QCoreApplication>

CallTest_Python::CallTest_Python(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CallTest_PythonClass())
{
    ui->setupUi(this);

    m_process = new QProcess(this);

    // 启动 Python 进程
    connect(ui->pushButton, &QPushButton::clicked, this, &CallTest_Python::on_btnDownload_clicked);
    // 连接信号：实时读取 Python 的 print() 输出
    connect(m_process, &QProcess::readyReadStandardOutput, this, &CallTest_Python::onProcessOutput);
    // 连接信号：任务结束 接受Python代码返回值(注意是 0 1 值代表成功或失败)
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, &CallTest_Python::onProcessFinished);

    connect(ui->SatelliteSelection, QOverload<int>::of(&QComboBox::currentIndexChanged),
    this, &CallTest_Python::satelliteInformation);

    // 替换 delegate，让样式表支持 item 属性
    ui->SatelliteSelection->setItemDelegate(new QStyledItemDelegate(ui->SatelliteSelection));

    // 获取应用程序目录
    QString appdir = QCoreApplication::applicationDirPath();
    // 设置默认云量阈值
    ui->CloudThreshold->setValue(10);
}

CallTest_Python::~CallTest_Python()
{
    delete ui;
}

void CallTest_Python::on_btnDownload_clicked()
{
    // 1. 让用户选择 SHP 文件 (输入)
    QString shpPath = QFileDialog::getOpenFileName(this,
        "选择矿区矢量范围",
        "E:/data", // 默认打开路径
        "Shapefile (*.shp);;GeoJSON (*.geojson)");

    if (shpPath.isEmpty()) return; // 用户取消了

    // 2. 让用户选择保存路径 (输出)
    QString savePath = QFileDialog::getSaveFileName(this,
        "保存遥感影像结果",
        "E:/Test/result_geemap.tif", // 默认文件名
        "GeoTIFF (*.tif)");

    if (savePath.isEmpty()) return; // 用户取消了

    QString appdir = QCoreApplication::applicationDirPath();
    // 3. 准备调用环境
    // 【关键】必须使用你 venv 里的 python.exe，因为只有它安装了 geemap
    QString pythonExe = appdir + "/gee/venv/Scripts/python.exe";
    QString scriptPath = appdir + "/gee/大文件数据下载.py";
    qint32 index = ui->SatelliteSelection->currentIndex();
    QString id = ui->projectId->text();
    qint32 scale = ui->scale->value();
    QString start = ui->startDateEdit->text();
    QString end = ui->endDateEdit->text();
    qint32 cloud = ui->CloudThreshold->value();
    // 4. 构造参数列表
    // 对应 Python 里的 parser.add_argument
    QStringList params;
    params << scriptPath;
    params << "--shp" << shpPath;
    params << "--out" << savePath;
    params << "--index" << QString::number(index);
    params << "--id" << id;
    params << "--scale" << QString::number(scale);
    params << "--start" << start;
    params << "--end" << end;
    params << "--cloud" << QString::number(cloud);

    // 5. 启动进程
    ui->textEdit->append("正在启动 Python 下载引擎...");
    // 禁用按钮防止重复点击
    // ui->btnDownload->setEnabled(false); 

    m_process->start(pythonExe, params);
    ui->textEdit->append("\n");
}

// 实时显示 Python 日志
void CallTest_Python::onProcessOutput()
{
    // 读取 Python 的 print 输出
    QByteArray data = m_process->readAllStandardOutput();
    QString log = QString::fromLocal8Bit(data); // 注意编码，Windows通常是 Local8Bit (GBK)

    // 你可以把它显示在 UI 的文本框里，例如 ui->textEdit->append(log);
    qDebug() << "[Python]:" << log.trimmed();
    ui->textEdit->append(log.trimmed());
}

// 任务结束处理
void CallTest_Python::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    // 恢复按钮
    // ui->btnDownload->setEnabled(true);

    if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
        QMessageBox::information(this, "成功", "遥感影像下载完成\n请在QGIS中查看结果");

        // 可选：下载完成后自动加载到地图 (调用你之前的 loadTifToCanvas 函数)
        // loadTifToCanvas(m_currentSavePath); 
    }
    else {
        // 读取错误信息
        QString error = QString::fromLocal8Bit(m_process->readAllStandardError());
        QMessageBox::critical(this, "失败", "下载过程中发生错误：\n" + error);
    }
}

// 实时显示卫星信息
void CallTest_Python::satelliteInformation(int index)
{
    
    QString Inf = "";
    int scale = 0;
    switch (index)
    {
    case 0:
        Inf = "请选择数据";
        break;
    case 1:
        Inf = "目前的矿山监测主力，分辨率高，重访周期短。\n\
分辨率: 10米 (可见光/近红外), 20米 (红边/短波红外)\n\
特点 : 经过大气校正（表面反射率），数据质量最好。";
        scale = 10;
        break;
    case 2:
        Inf = "如果你需要 2013 年以后的数据，且 Sentinel-2 云量太大时，用它替补\n分辨率: 30米\n\
特点 : 波段与 Sentinel - 2 类似，但分辨率稍低。\n\
注意：波段名称不同（Sentinel - 2 的近红外是 B8，Landsat 8 与 9是 B5）。";
        scale = 30;
        break;
    case 3:
        Inf = "如果你需要 2013 年以后的数据，且 Sentinel-2 云量太大时，用它替补\n分辨率: 30米\n\
特点 : 波段与 Sentinel - 2 类似，但分辨率稍低。\n\
注意：波段名称不同（Sentinel - 2 的近红外是 B8，Landsat 8 与 9是 B5）。";
        scale = 30;
        break;
    case 4:
        Inf = "时间范围1984-2012\n分辨率: 30米";
        scale = 30;
        break;
    case 5:
        Inf = "1999-至今, 但2003年后有条带丢失\n分辨率: 30米";
        scale = 30;
        break;
    case 6:
        Inf = "分辨率: 250米\n特点 : 每天都有图，几乎没有云的影响（合成产品），但太糊了，不适合小矿山";
        scale = 250;
        break;
    case 7:
        Inf = "分辨率: 30米\n地形与高程数据 (DEM)";
        scale = 30;
        break;
    case 8:
        Inf = "分辨率: 30米\n特点 : 精度比 SRTM 略高，填充了空洞\n地形与高程数据 (DEM)";
        scale = 30;
        break;
    case 9:
        Inf = "分辨率: 30米\n特点 : 日本卫星数据，在某些山区精度优于 SRTM";
        scale = 30;
        break;
    case 10:
        Inf = "分辨率: 10米\n特点 : 主动微波遥感，不管白天黑夜、不管下雨阴天都能拍到图\n\
雷达影像用途：全天候监测、地表沉降监测（InSAR）、水体识别";
        scale = 10;
        break;
    case 11:
        Inf = "分辨率: 10米\n特点 : 直接告诉你这个像素是树、是草、还是房子\n\
ESA WorldCover 是年度产品（annual），时间分辨率为1年";
        scale = 10;
    case 12:
        Inf = "(属于社区数据集)\n分辨率 : 10米";
        scale = 10;
        break;
    }
    ui->characteristic->setText(Inf);
    ui->scale->setValue(scale);
}

