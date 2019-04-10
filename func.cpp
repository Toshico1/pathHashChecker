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
            stream << "---" << list[i] << " NEW " <<QTime::currentTime().toString("HH:mm:ss") << "\r\n";
        }
    }else{
        qDebug() << "OSHIBKA 12";
    }
    logfile.close();
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


//проводит проверку и создает новые логи для дальнейшей работы этой же функции
void mainChecker(QString logs){
    //logs = "C:/Users/User/Documents/build-main-Desktop_Qt_5_3_MinGW_32bit-Debug/tmpfile.txt";
    QFile logfile(logs);
    logfile.open(QIODevice::ReadOnly);
    //QTextStream *stream = new QTextStream(&logfile);
    QString tmp;
    QFile tmpfile("tmpfile.txt");//новый лог файл
    tmpfile.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream *stream = new QTextStream(&tmpfile);
    //*stream << tmp;
    while(!logfile.atEnd()){
        //qDebug() << tmp;
        tmp = logfile.readLine();
        if(tmp.left(3) != "---"){
            *stream << tmp;
        }else{
            int i = 0;
            for(; tmp.at(i) != ' ';i++){}
            QString vrempath = tmp.left(i); //обрезать в пути ---
            vrempath.remove(0,3);
            *stream << "---" << vrempath << " ";
            QByteArray realHash = fileChecksum(vrempath);//берем хэш нашего файла (по пути)
            bool triger = tmp.contains(realHash.toHex());
            *stream << realHash.toHex() << " ";
            if(!triger){
                *stream << "EDITED ";
            }else{
                *stream << "OLD ";
            }
            *stream << QTime::currentTime().toString("HH:mm:ss") << "\r\n";
        }
    }
    logfile.close();
    tmpfile.close();

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
            absDirList += list.at(i).absoluteFilePath(); //сохраняем abs папки
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
