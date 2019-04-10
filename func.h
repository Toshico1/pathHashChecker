#ifndef FUNC_H
#define FUNC_H
//-------------------------------------------
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QTextStream>
#include <QCryptographicHash>
//-------------------------------------------

void mainChecker(QString logs);
void writeLogs();
void recursDir(QString dirS);
QByteArray fileChecksum(const QString &fileName);

#endif // FUNC_H
