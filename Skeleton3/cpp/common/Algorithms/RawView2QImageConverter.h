/***
 * Coordinates selections on one or more data controllers.
 *
 */

#pragma once

#include <QImage>
#include <QCache>
#include "IImage.h"


/// compute clip values from the values in a view
template <typename Scalar>
static
typename std::tuple<Scalar,Scalar> computeClips(
        NdArray::TypedView<Scalar> & view,
        double perc
        )
{
    // read in all values from the view into an array
    // we need our own copy because we'll do quickselect on it...
    std::vector<Scalar> allValues;
    view.forEach( [& allValues](const Scalar & val) {
        if( std::isfinite( val)) {
            allValues.push_back( val);
        }
    });

    // indicate bad clip if no finite numbers were found
    if( allValues.size() == 0) {
        return std::make_tuple(
                    std::numeric_limits<Scalar>::quiet_NaN(),
                    std::numeric_limits<Scalar>::quiet_NaN());
    }

    Scalar clip1, clip2;
    Scalar hist = (1.0 - perc) / 2.0;
    int x1 = allValues.size() * hist;
    std::nth_element( allValues.begin(), allValues.begin() + x1, allValues.end());
    clip1 = allValues[x1];
    x1 = allValues.size() - 1 - x1;
    std::nth_element( allValues.begin(), allValues.begin() + x1, allValues.end());
    clip2 = allValues[x1];

    if(0) { // debug
        int64_t count = 0;
        for( auto & x : allValues) {
            if( x >= clip1 && x <= clip2) { count++ ; }
        }
        qDebug() << "autoClip(" << perc << ")=" << count * 100.0 /allValues.size();
    }

    return std::make_tuple( clip1, clip2);
}


/// algorithm for converting an instance of image interface to qimage
class RawView2QImageConverter {
public:
    typedef RawView2QImageConverter & Me;
    typedef float Scalar;

    struct CachedImage {
        QImage img;
        double clip1, clip2, perc;
        bool specificToFrame;

        CachedImage( QImage pImg, double c1, double c2, bool spec, double p) {
            img = pImg; clip1 = c1; clip2 = c2;
            specificToFrame = spec;
            perc = p;
        }
    };

    RawView2QImageConverter() {
        // set cache size (in MB)
        m_cache.setMaxCost( 6 * 1024);
    };

    Me & setView( NdArray::RawViewInterface * rawView) {
        if( ! rawView) {
            throw std::runtime_error( "iimage is null");
        }
        if( rawView->dims().size() < 2) {
            throw std::runtime_error( "need at least 2 dimensions");
        }
        m_rawView = rawView;
        return * this;
    };

    Me & setFrame( int frame) {
        if( frame > 0) {
            int nFrames = 0;
            if( m_rawView->dims().size() > 2) {
                nFrames = m_rawView->dims()[2];
            }
            if( frame >= nFrames) {
                throw std::runtime_error( "not enough frames in this image");
            }
        }
        return * this;
    };

    Me & setAutoClip( double val) {
        m_autoClip = val;
        return * this;
    };

    const QImage & go( int frame, bool recomputeClip = true) {

        // let's see if we have the result in cache
        CachedImage * img = m_cache[ frame];
//        img = nullptr;
//        qDebug() << "Cache check #" << m_frame;

        if( img) {
            bool match = false;
            // we have something in cache, let's see if it matches what we want
            if( recomputeClip) {
                if( img->specificToFrame && img->perc == m_autoClip) {
                    match = true;
                }
            }
            else {
                if( img->clip1 == m_clip1 && img->clip2 == m_clip2) {
                    match = true;
                }
            }
            if( match) {
                qDebug() << "Cache hit #" << frame;
                return img->img;
            }
        }
        qDebug() << "Cache miss #" << frame;

        NdArray::TypedView<Scalar> view( m_rawView, false);

        if( ! std::isfinite(m_clip1) || recomputeClip) {
            std::tie( m_clip1, m_clip2) = computeClips<Scalar>( view, m_autoClip);
        }

        int width = m_rawView->dims()[0];
        int height = m_rawView->dims()[1];

        // make sure there is at least 1 finite value, if not, return a black image
        if( ! std::isfinite(m_clip1)) {
            // there were no values.... return a black image
            m_qImage = QImage( width, height, QImage::Format_ARGB32);
            m_qImage.fill( 0);
            return m_qImage;
        }

        // construct image using clips (clipd, clipinv)
        m_qImage = QImage( width, height, QImage::Format_ARGB32);
        if( m_qImage.bytesPerLine() != width * 4) {
            throw std::runtime_error( "qimage does not have consecutive memory...");
        }
        QRgb * outPtr = reinterpret_cast<QRgb *>( m_qImage.bits());

        // precalculate linear transformation coeff.
        float clipdinv = 1.0 / (m_clip2 - m_clip1);

        // apply the clips
        view.forEach( [& outPtr, & clipdinv, this](const float & ival) {
            if( std::isfinite( ival)) {
                float val = (ival - m_clip1) * clipdinv;
                int gray = val * 255;
                if( gray > 255) gray = 255;
                if( gray < 0) gray = 0;
                * outPtr = qRgb( gray, gray, gray);
            }
            else {
                * outPtr = qRgb( 255, 0, 0);
            }
            outPtr ++;
        });

//        // construct image using clips (clipd, clipinv)
//        static std::vector < QRgb > outBuffer;
//        outBuffer.resize( width * height);
//        QRgb * outPtr = & outBuffer[0];

//        // precalculate linear transformation coeff.
//        float clipdinv = 1.0 / (m_clip2 - m_clip1);

//        // apply the clips
//        view.forEach( [& outPtr, & clipdinv, this](const float & ival) {
//            if( std::isfinite( ival)) {
//                float val = (ival - m_clip1) * clipdinv;
//                int gray = val * 255;
//                if( gray > 255) gray = 255;
//                if( gray < 0) gray = 0;
//                * outPtr = qRgb( gray, gray, gray);
//            }
//            else {
//                * outPtr = qRgb( 255, 0, 0);
//            }
//            outPtr ++;
//        });

//        // construct the qimage from the temporary buffer
//        m_qImage = QImage( reinterpret_cast<uchar *>( & outBuffer[0]),
//                width, height, QImage::Format_ARGB32);

        // stick this into the cache so we don't need to recompute it
        m_cache.insert( frame,
                        new CachedImage( m_qImage, m_clip1, m_clip2, recomputeClip, m_autoClip),
                        width * height * 4 / 1024 / 1024);

        qDebug() << "cache total=" << m_cache.totalCost() << "MB";

        return m_qImage;
    };
protected:
    QImage m_qImage;
    NdArray::RawViewInterface * m_rawView = nullptr;
    double m_autoClip = 0.95;
    QCache<int,CachedImage> m_cache;
    float m_clip1 = 1.0/0.0, m_clip2 = 0.0;
};


