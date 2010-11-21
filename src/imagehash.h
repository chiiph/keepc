#ifndef IMAGEHASH_H
#define IMAGEHASH_H
#include <QString>
#include <QtSql>
#include <QDebug>

class ImageHash
{
public:    

    ImageHash(QString dbName = "db\\hash.db");
    void create();
    void insert(QString path, int key);
    void select();
    QStringList getMatchs(int key);

private:

    QSqlDatabase db;
    QSqlQuery query;
    QSqlQueryModel *model;

};

#endif // IMAGEHASH_H
