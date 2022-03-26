#include "MainWindow.h"

#include <QApplication>
#include <QStyleFactory>
#include "Helper.h"

int main(int argc, char *argv[])
{
    // 设置日志格式
    qSetMessagePattern("%{type} | %{time yyyy-MM-dd hh:mm:ss.zzz} | %{threadid} | %{message}");

    QApplication app(argc, argv);

    // 设置基本信息
    QCoreApplication::setOrganizationName("softsec");
    QCoreApplication::setOrganizationDomain("softsec.org");
    QCoreApplication::setApplicationName("QExileBot");

    // 设置基本样式
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    // 设置样式表
    app.setStyleSheet(Helper::File::ReadAll(":/qss/default.qss"));

    // 创建主窗口
    MainWindow w;
    w.show();

    // 进入事件循环
    return app.exec();
}
