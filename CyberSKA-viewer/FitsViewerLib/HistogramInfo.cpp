#include <sstream>
#include <cmath>
#include "HistogramInfo.h"


namespace RaiLib {

///////////////////////////////////////////////////////////////////////////////
// HistogramInfo
///////////////////////////////////////////////////////////////////////////////
std::string HistogramInfo::str () const
{
    std::stringstream out;
    out << "HistogramInfo(\n";
    out << "  valid = " << valid << "\n";
    out << "  min     = " << min     << "\n";
    out << "  min95   = " << min95   << "\n";
    out << "  min98   = " << min98   << "\n";
    out << "  min99   = " << min99   << "\n";
    out << "  min995  = " << min995  << "\n";
    out << "  min999  = " << min999  << "\n";
    out << "  min9999 = " << min9999 << "\n";
    out << "  max     = " << max     << "\n";
    out << "  max95   = " << max95   << "\n";
    out << "  max98   = " << max98   << "\n";
    out << "  max99   = " << max99   << "\n";
    out << "  max995  = " << max995  << "\n";
    out << "  max999  = " << max999  << "\n";
    out << "  max9999 = " << max9999 << "\n";
    out << ")\n";
    return out.str ();

//    return QString("HistogramInfo(valid:%1,min:%2,max:%3)")
//            .arg(valid).arg(min).arg(max).toStdString ();
}

static bool cmp( const double & n1, const double & n2)
{
    if( n1 == n2) return true;
    if( isnan (n1) && isnan(n2)) return true;
    return false;
}

bool HistogramInfo::operator == ( const HistogramInfo & h) const
{
    return     cmp(min     , h.min)
            && cmp(min95   , h.min95)
            && cmp(min98   , h.min98)
            && cmp(min99   , h.min99)
            && cmp(min995  , h.min995)
            && cmp(min999  , h.min999)
            && cmp(min9999 , h.min9999)
            && cmp(max     , h.max)
            && cmp(max95   , h.max95)
            && cmp(max98   , h.max98)
            && cmp(max99   , h.max99)
            && cmp(max995  , h.max995)
            && cmp(max999  , h.max999)
            && cmp(max9999 , h.max9999)
            && (valid   == h.valid);

}


}; // namespace RaiLib
