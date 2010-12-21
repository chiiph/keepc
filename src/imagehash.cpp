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
}

//HASH CON BUCKET = SQRT(#FEATURES)
void ImageHash::create(int bucket){
    QString queryStr = "CREATE TABLE IF NOT EXISTS bucket" + QString::number(bucket) + " ( path VARCHAR(200), PRIMARY KEY (path))";
    //qDebug() << queryStr;
    this->query.prepare(queryStr);    
    if(!this->query.exec()){
        qDebug() << "Error en la creacion de la tabla \"bucket" + QString::number(bucket) + "\"." << endl;
        exit(1);
    }
}

void ImageHash::insert(QString path, int key){
    QString queryStr;
    if(key > LAST_BUCKET)
        key = LAST_BUCKET;
    this->create(key);
    queryStr = "SELECT path FROM bucket" + QString::number(key) + " WHERE path='" + path + "'";
    //qDebug() << queryStr;
    this->model->setQuery(queryStr);
    if(this->model->rowCount() > 0)
        return;
    queryStr = "INSERT INTO bucket" + QString::number(key) + " VALUES ('" + path + "')";
    //qDebug() << queryStr;
    this->query.prepare(queryStr);
    if(!this->query.exec()){
        qDebug() << "Error en la insercion de la imagen" + path + "\"." << endl;
        exit(1);
    }
}

QStringList ImageHash::select(int key){
    QStringList paths = QStringList();    
    QString queryStr = "SELECT path FROM bucket" + QString::number(key);
    //qDebug() << queryStr;
    this->model->setQuery(queryStr);
    for(int i=0; i<this->model->rowCount(); i++)
        paths << this->model->record(i).value("path").toString();    
    return paths;
}

void ImageHash::featuresCountInsert(QString path){
    IplImage *img = Utils::loadImage(path.toAscii().data(), true);
    int key = Features::getHashKey(img);
    this->insert(path, key);
    cvReleaseImage(&img);
    qDebug() << "Imagen insertada con exito.";
}

void ImageHash::featuresCountSearch(QString path){
    double closerRMS = 1;
    QString closerPath = "";
    bool contains = false;
    IplImage *img = Utils::loadImage(path.toAscii().data(), true);
    int key = Features::getHashKey(img);
    QStringList images = this->select(key);
    qDebug() << "Buscando imagenes similares..." << endl;
    ImageFuncs::closer(path, images, closerPath, closerRMS, contains);
    if(!contains)
        this->insert(path, key);
    if(closerPath == "")
        qDebug() << "No hay registradas imagenes similares (que difieran de la ingresada).";
    else
        cout << "La imagen registrada de mayor similitud es \"" << closerPath.toAscii().data() << "\", con un valor de " << closerRMS << "." << endl;
    cvReleaseImage(&img);   
}


/*HASH QUE ALMACENA EL HESSIANO DE "HASH_DEPTH" FEATURES

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
*/

