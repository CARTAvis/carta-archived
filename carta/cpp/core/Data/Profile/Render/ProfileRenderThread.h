/**
 * A thread that blocks until the Profile data has been computed and is
 * available for reading.
 **/

#pragma once

#include "CartaLib/Hooks/ProfileResult.h"
#include <QThread>


namespace Carta{
namespace Data{

class ProfileRenderThread : public QThread {

    Q_OBJECT;

public:

    /**
     * Constructor.
     * @param pipeFileDescriptor - the file descriptor for the pipe where the
     *      Profile data should be read.
     * @param parent - the parent object.
     */
    ProfileRenderThread( QObject* parent = nullptr);

    /**
     * Returns the Profile data.
     * @return - the computed data for a Profile plot.
     */
    Carta::Lib::Hooks::ProfileResult getResult() const;


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
    virtual ~ProfileRenderThread();


private:
    int m_fileDescriptor;
    Carta::Lib::Hooks::ProfileResult m_result;

    ProfileRenderThread( const ProfileRenderThread& other);
    ProfileRenderThread& operator=( const ProfileRenderThread& other );
};
}
}



