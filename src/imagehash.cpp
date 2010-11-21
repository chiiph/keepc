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
    this->query.prepare("CREATE TABLE IF NOT EXISTS hash ("
                            "path VARCHAR(50), "
                            "key INTEGER NOT NULL, "
                            "PRIMARY KEY(path, key))");
    bool result = this->query.exec();
    if(!result)
        qDebug() << "Se produjo un error en la creacion de la base de datos.";    
}

void ImageHash::insert(QString path, int key){
    this->query.prepare("INSERT INTO hash (path, key) VALUES (:path, :key)");
    this->query.bindValue(":path", path);
    this->query.bindValue(":key", key);
    bool result = this->query.exec();
    if(!result)
        qDebug() << "Error en la insercion:" << query.lastError().driverText() << "," << query.lastError().databaseText();
}

void ImageHash::select(){
    this->model->setQuery("SELECT * FROM hash ORDER BY key");
}

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
