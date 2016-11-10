/*
 * Stores settings for rendering an image.
 */

#pragma once

#include "CartaLib/CartaLib.h"
#include <QPointF>
#include <QString>
#include <QSize>
#include <QList>

namespace Carta {

namespace Data {

class Layer;

class RenderRequest {

public:

    /**
     * Constructor.
     */
    RenderRequest( const std::vector<int>& frames,
            const Carta::Lib::KnownSkyCS& cs );

    /**
     * Return the minimum clip percent.
     * @return - the minimum clip percent.
     */
    double getClipPercentMin() const;

    /**
     * Return the maximum clip percent.
     * @return - the maximum clip percent.
     */
    double getClipPercentMax() const;

    /**
     * Return the layers that will be rendered.
     * @return - a list of layers to be rendered.
     */
    QList<std::shared_ptr<Layer> > getData() const;

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
     * Returns the zoom to use for the render.
     * @return - the zoom factor to use in rendering.
     */
    double getZoom() const;

    /**
     * Return the pan to use for rendering.
     * @return - the pan to use in rendering.
     */
    QPointF getPan() const;

    /**
     * Returns the index of the image that should be the top image in the stack.
     * @return - the index of the image that should be at the top of the stack.
     */
    int getTopIndex() const;

    /**
     * Returns whether or not clips should be recomputed on a new frame.
     * @return - true if clips should be recomputed for a new frame; false, otherwise.
     */
    bool isRecomputeClips() const;

    /**
     * Returns whether the request includes a context rendering.
     * @return - true, if a context image will be rendered; false otherwise.
     */
    bool isRequestContext() const;

    /**
     * Returns whether the main image will be rendered.
     * @return - true if the main image will be rendered; false otherwise.
     */
    bool isRequestMain() const;

    /**
     * Returns whether a zoom view of the image will be rendered.
     * @return - true if a zoom view of the image will be rendered; false otherwise.
     */
    bool isRequestZoom() const;

    /**
     * Returns true if the request is for index in the stack; false otherwise.
     * @return - true, if the request is for the top index in the stack; false, otherwise.
     */
    bool isStackTop() const;

    /**
     * Returns whether the request calls for a specific pan value or whether the pan from the
     * image should be used.
     * @return - if the request specifies a pan value; false, otherwise.
     */
    bool isPanSet() const;

    /**
     * Returns whether the request calls for a specific zoom value.
     * @return - true if a specific zoom value has been set; false, otherwise.
     */
    bool isZoomSet() const;

    /**
     * Return the requested size of the rendered image.
     * @return - the requested size of the rendered image.
     */
    QSize getOutputSize() const;

    /**
     * Returns whether or not the other request is equal to this one.
     * @param other - the other request.
     * @return - whether or not the other request is equal to this one.
     */
	bool operator==( const RenderRequest& other ) const;

	/**
	 * Set the min and max clip percentiles.
	 * @param minClipPercent - the minimum clip percentile.
	 * @param maxClipPercent - the maximum clip percentile.
	 */
	void setClipPercents( double minClipPercent, double maxClipPercent );

    /**
     * Set the list of layers to be rendered.
     * @param datas - the list of layers to be rendered.
     */
    void setData(QList<std::shared_ptr<Layer> > datas);

    /**
     * Set the pan value that should be used for rendering.
     * @param pan - the pan value that should be used for rendering.
     */
    void setPan( QPointF pan );

    /**
     * Set whether or not the request should include a render of the main
     * image.
     * @param mainRequest - true to render the main image; false, otherwise.
     */
    void setRequestMain( bool mainRequest );

    /**
     * Set whether or not the request should include a render of the context
     * image.
     * @param contextRequest - true to render the context image; false, otherwise.
     */
    void setRequestContext( bool contextRequest );

    /**
     * Set whether or not the request should include a render of the zoom image.
     * @param zoomRequest - true to render the zoom image; false, otherwise.
     */
    void setRequestZoom( bool zoomRequest );

    /**
     * Set the size of the output image.
     * @param size - the size of the output image.
     */
    void setOutputSize( const QSize& size );

    /**
     * Set whether or not clips should be recomputed on a new frame.
     * @param recomputeClipsOnNewFrame - true if clips should be recomputed; false, otherwise.
     */
    void setRecomputeClips( bool recomputeClipsOnNewFrame );

    /**
     * Set whether or not to include the grid with the rendering.
     * @param stackTop - true if the grid should be included; false, otherwise.
     */
    void setStackTop( bool stackTop );

    /**
     * Set the index of the top image in the stack.
     * @param topIndex - the index of the top image in the stack.
     */
    void setTopIndex( int topIndex );

    /**
     * Set the zoom value that should be used for rendering.
     * @param zoomValue - the zoom value that should be used for rendering.
     */
    void setZoom( double zoomValue );

    virtual ~RenderRequest();

private:

    std::vector<int> m_frames;
    Carta::Lib::KnownSkyCS m_cs;
    int m_topIndex;
    bool m_stackTop;
    double m_zoom;
    bool m_requestMain;
    bool m_requestContext;
    bool m_requestZoom;
    QPointF m_pan;
    QSize m_outputSize;
    QList<std::shared_ptr<Layer> > m_datas;
    bool m_recomputeClips;
    double m_minClipPercent;
    double m_maxClipPercent;

    RenderRequest& operator=( const RenderRequest& other );
};

}
}

