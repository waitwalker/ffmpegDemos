#include "mainwindow.h"

#include <QApplication>

void test() {

}

void test1() {

}

// p是要调用的函数指针，*p后面的是要传递的函数参数
void test2(void(*P)()){

}

int main(int argc, char *argv[])
{

    test2(test1);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
