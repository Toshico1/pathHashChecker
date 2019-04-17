#include "func.h"
#include <QTime>
#include <QList>

//Считаем хэши
QByteArray fileChecksum(const QString &fileName){
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(QCryptographicHash::Md5);//создаем ключь
        if (hash.addData(&f)) {                          //уникализируем
            qDebug() << hash.result().toHex();
            return hash.result(); //HEX
        }
    }
    return QByteArray();
}

//записываем логи работы для первого запуска
void writeLogs(QString absPathDir, QStringList list){
    QFile logfile("logs.txt"); //Создаем лог файл и поток к нему, записывая главную дир. и файлы+хэши в ней
    logfile.open(QIODevice::Append | QIODevice::Text);
    QTextStream stream(&logfile);
    logfile.open(QIODevice::WriteOnly);
    if (QFile::exists("logs.txt")){
        if(logfile.size() != 0) //отступ
            stream << "\r\n";
        stream << absPathDir << "\r\n";
        for(int i = 0; i < list.size(); i++){
            stream << "---" << list[i] << " OLD " <<QTime::currentTime().toString("HH:mm:ss") << "\r\n";
        }
    }else{
        qDebug() << "OSHIBKA 12";
    }
    logfile.close();
}

void invertFile(QString path){
    QFile file(path);
    QStringList strlist;
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    while(!file.atEnd()){
        strlist += file.readLine();
    }
    file.close();
    file.open(QIODevice::Truncate);
    file.close();
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream stream(&file);
    for(int i = strlist.size()-1; i != -1; i--){
        stream << strlist[i];
    }
    file.close();
}

//Считаем кол-во файлов в папке не рекурсивно

int valOfFiles(QString path){
    QDir dir(path);
    int k = 0;
    QFileInfoList list = dir.entryInfoList();
    for(int i = 0; i < list.size(); i++){
        if(list.at(i).isFile())
            k++;
    }
    return k;
}
/* Система флагов
 * DELETE - файл был при прошлой проверке, но в данный момент не найден
 * NEW - файл не обнаружен при прошлой проверке, найден сейчас и сохранен в логи
 * EDITED - файл изменил свое содержимое
 * OLD - файл не был изменен
 *
 * Формат записи данных о файлах
 * PATH HASH FLAG CHECK_TIME
 *
 * Формат записи данных о директориях
 * PATH
 */


QFileInfoList filesInDir(QString path){
    QDir dir(path);
    dir.setFilter(QDir::Files);
    QFileInfoList infoList = dir.entryInfoList();
    return infoList;
}

QString handlerHashSTR(QString tmp){
    int i = 0;
    for(; tmp[i] != ' '; i++){}
    tmp = tmp.remove(0,i+1);
    i = 0;
    for(; tmp[i] != ' '; i++){}
    tmp = tmp.left(i);
    return tmp;
}
QString handlerPathSTR(QString tmp){
int i = 0;
tmp = tmp.remove(0,3);
for(; tmp[i] != ' '; i++){}
tmp = tmp.left(i);
return tmp;
}

//проводит проверку и создает новые логи для дальнейшей работы этой же функции
void mainChecker(QString logs){
    //logs = "C:/Users/User/Documents/build-main-Desktop_Qt_5_3_MinGW_32bit-Debug/tmpfile.txt";
    QFile oldLogFile(logs);
    oldLogFile.open(QIODevice::ReadOnly);
    QFile newLogFile("tmpfile.txt");//новый лог файл
    newLogFile.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream newStream(&newLogFile);
    QTextStream oldStream(&oldLogFile);
    QString str;
    QStringList listOfFiles; //как то подчищать его
    int i_p = 0;
    while(!oldLogFile.atEnd()){
        str = oldLogFile.readLine();
        if(str != "\r\r\n" && str != "\r\n" && str != "\n"){
            if(str.contains("---")){
                QString tmp = str;
                listOfFiles += tmp;
            }else{//попали в папку
                int i = 0;
                for(; str[i]!='\r'; i++){}
                str = str.left(i);
                QFileInfoList list = filesInDir(str); //функция принимающая path dir и возвращающая QList!!!ДОПИСАТЬ РЕКУРСИВНЫЙ ЗАНОС НОВЫХ ПАПОК!!!
                for(int i = 0; i < listOfFiles.size(); i++){ //перебираем из лог.txt в dirInfo
                    i_p = i;
                    bool triger = true;
                    if(triger){
                        triger = false;
                        for(int j = 0; j < list.size(); j++){
                            if(handlerPathSTR(listOfFiles[i]) == list[j].absoluteFilePath()){//сравниваем пути
                                if(handlerHashSTR(listOfFiles[i]) == fileChecksum(list[j].absoluteFilePath()).toHex()){//сравниваем хэши
                                    newStream << "---" << handlerPathSTR(listOfFiles[i]) << " " << handlerHashSTR(listOfFiles[i]) <<
                                                 " OLD " << QTime::currentTime().toString("HH:mm:ss") << "\r\n";
                                    triger = true;
                                    break;
                                }else{
                                    newStream << "---" << handlerPathSTR(listOfFiles[i]) << " " << fileChecksum(handlerPathSTR(listOfFiles[i])).toHex() <<
                                                 " EDITED " << QTime::currentTime().toString("HH:mm:ss") << "\r\n";
                                    triger = true;
                                    break;
                                }
                            }else{triger = false;}
                        }
                        if(triger == false){
                            newStream << "---" << handlerPathSTR(listOfFiles[i]) << " " << "0000000000000000" <<
                                         " DELETED " << QTime::currentTime().toString("HH:mm:ss") << "\r\n";
                        }
                    }
                }
                for(int i = 0; i <= i_p; i++)
                    listOfFiles.removeAt(0);
                newStream << str << "\r\r\n";
            }

        }else{newStream << str;}
    }

}


//Рекурсивный перебор папок и обращаемся к записи в файл
void recursDir(QString dirS){
    QDir dir(dirS);
    QString fileList; //сохраняем список файлов и директорий
    QString absDirList; // abs путь к dir
    QStringList absFileList; // abs список файлов
    QFileInfoList list = dir.entryInfoList(); //получаем все файлы
    int i = 2;
    for(; i < list.size(); i++){        //до тех пор пока в папке есть файлы
        QFileInfo fileInfo = list.at(i);
        fileList += fileInfo.fileName(); //сохраняем список файлов
        if(i % 2 == 0){
            fileList += " ";
        }
        if(list.at(i).isDir()){ //вызываем рекурсию для перебора dir
            absDirList += list.at(i).absoluteFilePath(); //сохраняем abs папки(не нужно)
            dir.cd(list.at(i).absoluteFilePath());       //двигаемся глубже и вызываем рекурсию
            recursDir(list.at(i).absoluteFilePath());
        }else{
            QByteArray hash = fileChecksum(list.at(i).absoluteFilePath()); //считаем хэш
            QString tmp = list.at(i).absoluteFilePath(); //Форматируем вид (можно ли это сделать удобнее ?)
            tmp += " ";
            tmp += hash.toHex();
            absFileList += tmp; //добавляем к списку наш файл
            if(valOfFiles(list.at(i).absolutePath()) == absFileList.size()){ //проверка на ложное заскакивание в запись
                writeLogs(list[i].absolutePath(), absFileList); //вызываем функцию записи

            }
        }
    }
}
