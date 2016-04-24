/*
 * Stores settings for rendering an image.
 */

#pragma once

#include "CartaLib/CartaLib.h"
#include <QString>
#include <QSize>


namespace Carta {

namespace Data {

class RenderRequest {

public:

    /**
     * Constructor.
     */
    RenderRequest( const std::vector<int>& frames,
            const Carta::Lib::KnownSkyCS& cs, bool topOfStack,/* bool save,*/
            const QSize& outputSize );

    /**
     * Returns the frame indices of the request.
     * @return - a list of frames to be rendered.
     */
    std::vector<int> getFrames() const;

    /**
     * Returns the coordinate system to use for rendering.
     * @return - the coordinate system to use for rendering.
     */
    Carta::Lib::KnownSkyCS getCoordinateSystem() const;

    /**
     * Returns the index of the image that should be the top image in the stack.
     * @return - the index of the image that should be at the top of the stack.
     */
    int getTopIndex() const;

    /**
     * Returns true if the request is for index in the stack; false otherwise.
     * @return - true, if the request is for the top index in the stack; false, otherwise.
     */
    bool isStackTop() const;

    /**
     * Return the requested size of the rendered image.
     * @return - the requested size of the rendered image.
     */
    QSize getOutputSize() const;

    /**
     * Set the index of the top image in the stack.
     * @param topIndex - the index of the top image in the stack.
     */
    void setTopIndex( int topIndex );

    virtual ~RenderRequest();

private:

    std::vector<int> m_frames;
    Carta::Lib::KnownSkyCS m_cs;
    int m_topIndex;
    bool m_stackTop;
    QSize m_outputSize;

    RenderRequest( const RenderRequest& other);
    RenderRequest& operator=( const RenderRequest& other );
};

}
}

