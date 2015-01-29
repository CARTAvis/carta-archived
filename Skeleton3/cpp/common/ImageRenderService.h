/**
 *
 **/


#ifndef IMAGERENDERSERVICE_H
#define IMAGERENDERSERVICE_H

#include <QObject>

class ImageRenderService : public QObject
{
    Q_OBJECT
public:
    explicit ImageRenderService(QObject *parent = 0);
    ~ImageRenderService();

signals:

public slots:
};

#endif // IMAGERENDERSERVICE_H
