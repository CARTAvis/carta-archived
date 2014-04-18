/**
 *
 **/


#ifndef MYQAPP_H
#define MYQAPP_H

#include <QApplication>

class MyQApp : public QApplication
{
    Q_OBJECT
public:
    explicit MyQApp(QObject *parent = 0);

signals:

public slots:

};

#endif // MYQAPP_H
