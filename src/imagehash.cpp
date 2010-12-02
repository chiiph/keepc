#include "imagehash.h"

ImageHash::ImageHash(QString dbName){
    this->db = QSqlDatabase::addDatabase("QSQLITE");
    this->db.setDatabaseName(dbName);
    if(!this->db.open()){
        qDebug() << "Se produjo un error en la creacion de la base de datos.";
        exit(1);
    }
    this->query = QSqlQuery(this->db);
    this->model = new QSqlQueryModel();
    this->create();        
}

void ImageHash::create(){    
    QString queryStr = "CREATE TABLE IF NOT EXISTS hash ( path VARCHAR(50), ";
    for(int i=0; i<HASH_DEPTH; i++)
        queryStr += "h" + QString::number(i) + " UNSIGNED MEDIUMINT DEFAULT NULL, ";
    queryStr += "PRIMARY KEY (path))";
    this->query.prepare(queryStr);
    bool result = this->query.exec();
    if(!result)
        qDebug() << "Se produjo un error en la creacion de la tabla de hashing.";
}

void ImageHash::insert(QString path, QList<int> hessians){
    QString queryStr = "INSERT INTO hash (path";
    QString values = "VALUES (:path";
    for(int i=0; i<HASH_DEPTH; i++){
        queryStr += ", h" + QString::number(i);
        values += ", :h" + QString::number(i);
    }
    queryStr = queryStr + ") " + values + ")";
    this->query.prepare(queryStr);
    this->query.bindValue(":path", path);
    for(int i=0; i<HASH_DEPTH; i++)
        this->query.bindValue(":h" + QString::number(i), hessians[i]);
    bool result = this->query.exec();
    qDebug() << query.executedQuery();
    qDebug() << query.boundValues();
    if(!result)
        qDebug() << "Error en la insercion:" << query.lastError().driverText() << "," << query.lastError().databaseText();
}

QStringList ImageHash::select(QList<int> hessians){
    QStringList paths = QStringList();
    if(hessians.count() > 0){
        QString queryStr = "SELECT path FROM hash WHERE h0=" + QString::number(hessians[0]);
        for(int i=1; i<SEARCH_DEPTH; i++)
            queryStr += " AND h" + QString::number(i) + "=" + QString::number(hessians[i]);
        this->model->setQuery(queryStr);
        qDebug() << queryStr;
        for(int i=0; i<this->model->rowCount(); i++)
            paths << this->model->record(i).value("path").toString();
    }
    return paths;
}

/*
QStringList ImageHash::getMatchs(int key){
    QStringList paths = QStringList();
    int i=0, currentKey=0;
    this->select();    
    while(i<this->model->rowCount() && currentKey<=key){
        currentKey = this->model->record(i).value("key").toInt();
        if(key == currentKey)
            paths << this->model->record(i).value("path").toString();
        i++;
    }
    return paths;
}
*/
