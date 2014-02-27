#ifndef HISTOGRAMINFO_H
#define HISTOGRAMINFO_H

#include <QtGlobal>
#include <string>

namespace RaiLib {

/// HistogramInfo holds histogram related information about a frame or file
/// i.e. min/max for 100%, 99.99%, etc...
struct HistogramInfo {
    double min, min95, min98, min99, min995, min999, min9999,
    max, max95, max98, max99, max995, max999, max9999;

    /// has this been retrieved from the db
    bool valid;
    /// default constructor - initialize invalid info
    HistogramInfo() { valid = false; }
    /// debugging
    std::string str() const;

    static const quint64 BinSize = sizeof(min) * 14 + sizeof(valid);

    bool operator == ( const HistogramInfo & h) const;
};

namespace Cache {


};
};


#endif // HISTOGRAMINFO_H
