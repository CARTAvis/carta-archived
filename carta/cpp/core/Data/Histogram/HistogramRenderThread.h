/**
 * A thread that blocks until the histogram data has been computed and is
 * available for reading.
 **/

#pragma once

#include "CartaLib/Hooks/HistogramResult.h"
#include <QThread>


namespace Carta{
namespace Data{

class HistogramRenderThread : public QThread {

    Q_OBJECT;

public:

    /**
     * Constructor.
     * @param pipeFileDescriptor - the file descriptor for the pipe where the
     *      histogram data should be read.
     * @param parent - the parent object.
     */
    HistogramRenderThread( int pipeFileDescriptor, QObject* parent = nullptr);

    /**
     * Returns the histogram data.
     * @return - the computed data for a histogram plot.
     */
    Carta::Lib::Hooks::HistogramResult getResult() const;


    /**
     * Run the thread.
     */
    void run();

    /**
     * Set a new read file descriptor.
     * @param fileDescriptor - a new read file descriptor.
     */
    void setFileDescriptor( int fileDescriptor );

    /**
     * Destructor.
     */
    virtual ~HistogramRenderThread();

private:
    int m_fileDescriptor;
    Carta::Lib::Hooks::HistogramResult m_result;

    HistogramRenderThread( const HistogramRenderThread& other);
    HistogramRenderThread& operator=( const HistogramRenderThread& other );
};
}
}



