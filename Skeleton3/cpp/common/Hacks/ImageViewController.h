/**
 *
 **/


#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include <QObject>

class ViewController : public QObject
{
    Q_OBJECT
public:
    explicit ViewController(QObject *parent = 0);
    ~ViewController();

signals:

public slots:
};

#endif // VIEWCONTROLLER_H
