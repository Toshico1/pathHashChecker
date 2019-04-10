#include "func.h"
#include "QFile"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString folder("C:/Users/User/Desktop/testfolder/");
    recursDir(folder);
    mainChecker("C:/Users/User/Documents/build-main-Desktop_Qt_5_3_MinGW_32bit-Debug/logs.txt");
    return a.exec();
}
