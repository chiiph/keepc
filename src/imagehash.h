#ifndef IMAGEHASH_H
#define IMAGEHASH_H
#include <QString>
#include <QtSql>
#include <QDebug>
#define HASH_DEPTH 10
#define SEARCH_DEPTH 5

class ImageHash
{
public:    

    ImageHash(QString dbName = "db\\hash.db");
    void create();
    void insert(QString path, QList<int> hessians);
    QStringList select(QList<int> hessians);
    QStringList getMatchs(int key);

private:

    QSqlDatabase db;
    QSqlQuery query;
    QSqlQueryModel *model;

};

#endif // IMAGEHASH_H
