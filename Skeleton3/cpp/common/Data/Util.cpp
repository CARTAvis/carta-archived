#include "Util.h"

#include <QStringList>
#include <QDebug>

Util::Util( ) {

}

QVector<QString> Util::parseParamMap( const QString& params, const QList<QString>& keys ){
    QVector<QString> values;
    QStringList paramList = params.split( ",");
    if ( paramList.size() == keys.size() ){
        values.resize( keys.size());
        for ( QString param : paramList ){
            QStringList pair = param.split( ":");
            if ( pair.size() == 2 ){
                int keyIndex = keys.indexOf( pair[0] );
                if ( keyIndex >= 0 ){
                    values[keyIndex] = pair[1];
                }
                else {
                    qDebug() << "Unrecognized key="<<pair[0];
                }
            }
            else {
                qDebug() <<"Badly formatted param map="<<param;
            }
        }
    }
    else {
        qDebug() << "Discrepancy between parameter count="<<paramList.size()
                <<" and key count="<<keys.size()<<" params="<<params;
    }
    return values;
}

Util::~Util(){

}
