/**
 *
 **/


#ifndef PIXELPIPELINECONTROLLER_H
#define PIXELPIPELINECONTROLLER_H

#include <QObject>

class PixelPipelineController : public QObject
{
    Q_OBJECT
public:
    explicit PixelPipelineController(QObject *parent = 0);
    ~PixelPipelineController();

signals:

public slots:
};

#endif // PIXELPIPELINECONTROLLER_H
