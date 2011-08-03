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
#include "cbir.h"

using namespace std;

bool VERBOSE = false;
bool GRAPHIC = false;
bool BUILD = false;
bool ADD = false;
bool SEARCH = false;

/*!
 * Imprime por pantalla el modo de uso de la aplicación.
 */
void printHelp()
{
    cout << endl << "El modo de uso de la aplicacion es el siguiente:" << endl << endl;
    cout << "\t- Para realizar la comparacion de dos imagenes:" << endl << endl;
    cout << "\t\tKeepcon imagen1 imagen2 [-v] [-g]" << endl << endl;
    cout << "\t\t-v: la aplicacion imprime datos sobre el procesamiento." << endl;
    cout << "\t\t-g: la aplicacion muestra la imagen de correspondencia hallada para las imagenes (en caso de ser posible)." << endl << endl;
    cout << "\t\tLos argumentos pueden ser provistos en cualquier orden." << endl << endl;
    cout << "\t- Para crear el indice a partir de un conjunto de imagenes:" << endl << endl;
    cout << "\t\tKeepcon --build directorio/raiz/de/las/imagenes" << endl << endl;
    cout << "\t- Para agregar una imagen en el indice:" << endl << endl;
    cout << "\t\tKeepcon --add imagen" << endl << endl;
    cout << "\t- Para buscar las imagenes similares en el indice:" << endl << endl;
    cout << "\t\tKeepcon --search imagen" << endl << endl;
    cout << "\t- Para mostrar esta ayuda:" << endl << endl;
    cout << "\t\tKeepcon -h." << endl << endl;
    exit(0);
}

/*!
 * Verifica si la aplicación ha sido invocada de manera correcta.
 * En caso afirmativo, retorna los paths de las imágenes a procesar y setea los flags correspondientes.
 * En caso negativo, se imprime la ayuda por pantalla.
 */
void checkCall(int argc, char *argv[], QString &path1, QString &path2)
{
    QString arg;
    QStringList qArgs;   
    for(int i=1; i<argc; i++)
        qArgs << QString(argv[i]).toLower();
    if(qArgs.contains("-h"))
        printHelp();
    if(qArgs.contains("--build")){
        BUILD = true;
        qArgs.removeOne("--build");
        if(qArgs.size() != 1)
            printHelp();
        path1 = qArgs[0];
    }else{
        if(qArgs.contains("--add")){
            ADD = true;
            qArgs.removeOne("--add");
            if(qArgs.size() != 1)
                printHelp();
            path1 = qArgs[0];
        }else{
            if(qArgs.contains("--search")){
                SEARCH = true;
                qArgs.removeOne("--search");
                if(qArgs.size() != 1)
                    printHelp();
                path1 = qArgs[0];
            }else{
                for(int i=0; i<qArgs.size(); i++){
                    arg = qArgs[i];
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
        }
    }
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

    fileDirs << "auto" << "converse" << "doki" << "doki3" << "elmonito" << "flores" << "flores2" << "kitty" << "kitty2" << "lisa" << "lisa2" << "lisa3" << "lisakunfu" << "pelotas" << "perroygato" << "pocobeat" << "pocoyo" << "pocoyotel" << "remera";
    for(int i=0; i<fileDirs.count(); i++){
        QDir dir(root + fileDirs[i] + "/");
        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
        dir.setSorting(QDir::Name | QDir::Reversed);
        images << dir.entryList();
    }
    QDir dir(root);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    images << dir.entryList();
    qDebug() << images.count();

    for(int i=0; i<images.count(); i++){
        qDebug() << images[i];
    }

    xml = "<Row>\n    <Cell><Data ss:Type=\"String\">Imagen 1/Imagen 2</Data></Cell>\n";
    for(int i=0; i<images.count(); i++){
        xml += "    <Cell><Data ss:Type=\"String\">" + images[i] + "</Data></Cell>\n";
    }
    xml += "</Row>\n";

    IplImage *img1, *img2, *img1C, *img2C;
    QFile file("TEST-12-12-2010.xml");
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
                resStr = QString::number(result);
                for(int k=1; k<4-(resStr.right(resStr.length() - resStr.indexOf('.'))).length(); k++)
                    resStr.append('0');
                resStr = resStr.left(6);                
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

    /*
    //HASH #FEATURES
    IplImage *img;
    int key=0;    
    QStringList images;
    ImageHash hash = ImageHash();
    QString root = "img/";

    QDir dir(root);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name | QDir::Reversed);
    QStringList files = dir.entryList();    

    for(int i=0; i<files.count(); i++)
        hash.featuresCountInsert((root + files[i]).toAscii().data());

    hash.featuresCoundSearch("img/auto-t.jpg");
    */


    //GENERAL

    IplImage *img1=NULL, *img2=NULL, *img1C=NULL, *img2C=NULL;
    QString path1, path2;
    //ImageHash hash = ImageHash();

    cout << endl;    
    checkCall(argc, argv, path1, path2);

    if(BUILD){
        CBIR cbir = CBIR();
        cbir.buildIndex(path1);
        cbir.query("../img/keepcon2/flores-d.jpg");
    }else{
        if(ADD){
            CBIR cbir = CBIR();
            cbir.addImage(path1);
        }else{
            if(SEARCH){
                CBIR cbir = CBIR();
                cbir.query(path1);
            }else{
                Utils::loadImages(path1.toAscii().data(), path2.toAscii().data(), img1, img2, true);
                Utils::loadImages(path1.toAscii().data(), path2.toAscii().data(), img1C, img2C);
                if(VERBOSE){
                    cout << "Imagenes originales:" << endl;
                    cout << "Imagen 1: " << img1->width << " x " << img1->height << " (" << path1.toAscii().data() << ")" << endl;
                    cout << "Imagen 2: " << img2->width << " x " << img2->height << " (" << path2.toAscii().data() << ")" << endl << endl;
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
            }
        }
    }

    //CBIR

    exit(0);
    return app.exec();
}
