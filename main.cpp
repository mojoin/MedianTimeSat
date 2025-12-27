#include "calltest_python.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    CallTest_Python window;
    window.show();
    return app.exec();
}
