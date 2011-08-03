#ifndef IMAGEHASH_H
#define IMAGEHASH_H
#include <QString>
#include <QtSql>
#include <QDebug>
#include "utils.h"
#include "imagefuncs.h"
#include "features.h"
#define LAST_BUCKET 100000

class ImageHash
{
public:    

    ImageHash(QString dbName = "db/hash.db");
    void create();
    void create(int bucket);
    void insert(QString path, QList<int> hessians);
    void insert(QString path, int key);
    QStringList select(QList<int> hessians);
    QStringList select(int key);
    QStringList getMatchs(int key);
    void featuresCountInsert(QString path);
    void featuresCountSearch(QString path);

private:

    QSqlDatabase db;
    QSqlQuery query;
    QSqlQueryModel *model;

};

#endif // IMAGEHASH_H
