#include "func.h"
#include "QFile"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString folder("C:/Users/User/Desktop/testfolder/");
    //recursDir(folder);//вызываем первичный сбор
    //invertFile("C:/Users/User/Documents/build-main-Desktop_Qt_5_3_MinGW_32bit-Debug/logs.txt");//инвертируем для дальнейшей работы
    mainChecker("C:/Users/User/Documents/build-main-Desktop_Qt_5_3_MinGW_32bit-Debug/logs.txt");//вызывается проверка уже готовых логов и запись новых


    return a.exec();
}
