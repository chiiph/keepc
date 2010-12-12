#include <QApplication>
#include <QString>
#include <highgui.h>
#include <ctype.h>
#include "cv.h"
#include "iostream"
#include "highgui.h"
#include "imagefuncs.h"
#include "features.h"
#include "utils.h"
#include "imagehash.h"

using namespace std;

bool VERBOSE = false;
bool GRAPHIC = false;

/*!
 * Imprime por pantalla el modo de uso de la aplicación.
 */
void printHelp()
{
    cout << endl << "El modo de uso de la aplicacion es el siguiente:" << endl << endl;
    cout << "\tKeepcon imagen1 imagen2 [-v] [-g] [-h]" << endl << endl;
    cout << "\t-v: la aplicacion imprime datos sobre el procesamiento." << endl;
    cout << "\t-g: la aplicacion muestra la imagen de correspondencia hallada para las imagenes (si es posible)." << endl;
    cout << "\t-h: muestra esta ayuda." << endl << endl;
    cout << "\tLos argumentos pueden ser dispuestos en cualquier orden." << endl;
    exit(0);
}

/*!
 * Verifica si la aplicación ha sido invocada de manera correcta.
 * En caso afirmativo, retorna los paths de las imágenes a procesar y setea los flags correspondientes.
 * En caso negativo, se imprime la ayuda por pantalla.
 */
void checkCall(int argc, char *argv[], char* &path1, char* &path2)
{
    string arg;
    QList<char*> qArgs;
    for(int i=1; i<argc; i++)
        qArgs << argv[i];
    for(int i=0; i<qArgs.size(); i++){
        arg = qArgs[i];
        if(arg == "-h")
            printHelp();
        if(arg == "-v"){
            VERBOSE = true;
            qArgs.removeAt(i--);
            continue;
        }
        if(arg == "-g"){
            GRAPHIC = true;
            qArgs.removeAt(i--);
        }
    }
    if(qArgs.size() != 2)
        printHelp();
    path1 = qArgs[0];
    path2 = qArgs[1];
}

/*!
 * Aplicación principal:
 * Carga las dos imágenes recibidas como argumentos y determina el RMS que existe entre ellas.
 * En primera instancia, intenta realizar el cálculo basado en una transformación dada por los features de las mismas.
 * Si este paso no resulta exitoso, se procede a redimensionar y realizar el cálculo básico.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.addLibraryPath(".");
    
    /*
    //TEST
    QString root = "../img/keepcon2-g/";
    QString root2 = "../img/keepcon2/";
    QStringList fileDirs;
    QStringList images;
    QString xml = "";

    fileDirs << "auto" << "converse" << "doki" << "doki3" << "elmonito" << "flores";
    //<< "flores2" << "kitty" << "kitty2" << "lisa" << "lisa2" << "lisa3" << "lisakunfu" << "pelotas" << "perroygato" << "pocobeat" << "pocoyo" << "pocoyotel" << "remera";
    for(int i=0; i<fileDirs.count(); i++){
        QDir dir(root + fileDirs[i] + "/");
        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
        dir.setSorting(QDir::Name | QDir::Reversed);
        images << dir.entryList();
    }

//    QDir dir(root);
//    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
//    dir.setSorting(QDir::Name);
//    images << dir.entryList();
//    qDebug() << images.count();
//    for(int i=0; i<images.count(); i++){
//        qDebug() << images[i];
//    }

    xml = "<Row>\n    <Cell><Data ss:Type=\"String\">Imagen 1/Imagen 2</Data></Cell>\n";
    for(int i=0; i<images.count(); i++){
        xml += "    <Cell><Data ss:Type=\"String\">" + images[i] + "</Data></Cell>\n";
    }
    xml += "</Row>\n";

    IplImage *img1, *img2, *img1C, *img2C;
    QFile file("TEST-11-12-2010.xml");
    QTextStream out(&file);

    for(int i=0; i<images.count(); i++){
        xml += "<Row>\n    <Cell><Data ss:Type=\"String\">" + images[i] + "</Data></Cell>\n";
        for(int j=0; j<images.count(); j++){
            QString resStr = "";
            if(i<j){
                double result = 1.0;
                QString path1 = root2 + images[i];
                QString path2 = root2 + images[j];
                qDebug() << QString(path1) << QString(path2);
                Utils::loadImages(path1.toAscii().data(), path2.toAscii().data(), img1, img2, true);
                Utils::loadImages(path1.toAscii().data(), path2.toAscii().data(), img1C, img2C);
                if(!ImageFuncs::featuresBasedRMS(result, img1, img2, img1C, img2C))
                    result = ImageFuncs::resizeBasedRMS(img1C, img2C);
                cvReleaseImage(&img1);
                cvReleaseImage(&img2);
                cvReleaseImage(&img1C);
                cvReleaseImage(&img2C);
//                delete img1;
//                delete img2;
//                delete img1C;
//                delete img1C;
                for(int k=1; k<4-(resStr.right(resStr.length() - resStr.indexOf('.'))).length(); k++)
                    resStr.append('0');
                resStr = resStr.left(6);
                resStr = QString::number(result);
                qDebug() << result << endl;
            }
            xml += "    <Cell><Data ss:Type=\"String\">" + resStr + "</Data></Cell>\n";
        }
        xml += "</Row>\n";        
        file.open(QIODevice::Append | QIODevice::Text);           
        out << xml;
        file.close();
        xml = "";
    }
    */

    //HASH #FEATURES
    IplImage *img;
    int key=0;    
    QStringList images;
    ImageHash hash = ImageHash();
    QString root = "../img/keepcon2/";

    QDir dir(root);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name | QDir::Reversed);
    QStringList files = dir.entryList();
    
    for(int i=0; i<files.count(); i++){
        img = Utils::loadImage((root + files[i]).toAscii().data(), true);
        key = Features::getHashKey(img);
        images = hash.select(key);
        qDebug() << images;
        hash.insert(root + files[i], key);
        cvReleaseImage(&img);
    }

    img = Utils::loadImage("../img/keepcon2/auto.jpg", true);
    key = Features::getHashKey(img);
    images = hash.select(key);
    qDebug() << "Closer:" << ImageFuncs::closer("../img/keepcon2/auto.jpg", images);
    hash.insert("../img/keepcon2/auto.jpg", key);


    /*
    //RMS
    IplImage *img1=NULL, *img2=NULL, *img1C=NULL, *img2C=NULL;
    char *path1, *path2;

    cout << endl;    
    checkCall(argc, argv, path1, path2);
    Utils::loadImages(path1, path2, img1, img2, true);
    Utils::loadImages(path1, path2, img1C, img2C);    

    if(VERBOSE){
        cout << "Imagenes originales:" << endl;
        cout << "Imagen 1: " << img1->width << " x " << img1->height << " (" << path1 << ")" << endl;
        cout << "Imagen 2: " << img2->width << " x " << img2->height << " (" << path2 << ")" << endl << endl;
        ImageFuncs::setVerbose(true);
        Features::setVerbose(true);
        Utils::setVerbose(true);
    }
    if(GRAPHIC)
        Features::setGraphic(true);

    double result = 1;
    if(!ImageFuncs::featuresBasedRMS(result, img1, img2, img1C, img2C))
        result = ImageFuncs::resizeBasedRMS(img1C, img2C);

    cout << "Resultado: " << result << endl;
    //cout << result;// << endl;

    cvReleaseImage(&img1);
    cvReleaseImage(&img2);
    cvReleaseImage(&img1C);
    cvReleaseImage(&img2C);
    */

    exit(0);
    return app.exec();
}
