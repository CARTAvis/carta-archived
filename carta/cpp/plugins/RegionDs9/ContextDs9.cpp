#include <CartaLib/Regions/IRegion.h>
#include <CartaLib/Regions/Ellipse.h>
#include <CartaLib/Regions/Point.h>
#include <CartaLib/Regions/Rectangle.h>
#include <ContextDs9.h>
#include <measures/Measures/MeasConvert.h>
#include <measures/Measures/MCDirection.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <vector>
#include <QDebug>

ContextDs9::ContextDs9( ){
}
static inline double wrap_angle( double before, double after ) {
    const double UNIT_WRAPAROUND = 2.0 * M_PI;
    if ( after < 0 && before > 0 )
        return after + UNIT_WRAPAROUND;
    else if ( after > 0 && before < 0 )
        return after - UNIT_WRAPAROUND;
    return after;
}

static inline casa::Quantum<casa::Vector<double> > convert_angle( double x, const std::string &xunits, double y, const std::string &yunits,
        casa::MDirection::Types original_coordsys, casa::MDirection::Types new_coordsys, const std::string &new_units="rad" ) {
    casa::Quantum<double> xq(x,casa::String(xunits));
    casa::Quantum<double> yq(y,casa::String(yunits));
    casa::MDirection md = casa::MDirection::Convert(casa::MDirection(xq,yq,original_coordsys), new_coordsys)();
    casa::Quantum<casa::Vector<double> > result = md.getAngle("rad");
    xq.convert("rad");
    yq.convert("rad");
    result.getValue( )(0) = wrap_angle(xq.getValue( ), result.getValue( )(0));
    result.getValue( )(1) = wrap_angle(yq.getValue( ), result.getValue( )(1));
    result.convert(casa::String(new_units));
    return result;
}


void ContextDs9::createBoxCmd( const Vector& center, const Vector& size, double /*angle*/,
        const char* /*color*/, int* /*dash*/, int /*width*/, const char* /*font*/,
        const char* /*text*/, unsigned short /*prop*/, const char* /*comment*/,
        const std::list<Tag>& /*tag*/ ) {
    int centerCount = center.size();
    int sizeCount = size.size();
    Carta::Lib::Regions::Rectangle* info = nullptr;
    if ( sizeCount >= 2 && centerCount >= 2 ){
    	info = new Carta::Lib::Regions::Rectangle();
    	double left = center[0] - size[0] / 2;
    	double top = center[1] - size[1] / 2;
    	QRectF rect( left, top, size[0], size[1]);
    	info->setRectangle( rect );
    }
    if ( info ){
    	m_regions.push_back(  info  );
    }
}


#define DEFINE_POINT_COMMAND(NAME) \
void ContextDs9::create ## NAME ## Cmd( const Vector& center, int size, \
        const char* color, int* dash, int width, const char* font, \
        const char* text, unsigned short prop, const char* comment, const std::list<Tag>& tag ) { \
    PointShape shape = XPT; \
    createPointCmd( center, shape, size, color, dash, width, font, text, prop, comment, tag ); \
}
DEFINE_POINT_COMMAND(BoxPoint)
DEFINE_POINT_COMMAND(CirclePoint)
DEFINE_POINT_COMMAND(DiamondPoint)
DEFINE_POINT_COMMAND(CrossPoint)
DEFINE_POINT_COMMAND(ExPoint)
DEFINE_POINT_COMMAND(ArrowPoint)
DEFINE_POINT_COMMAND(BoxCirclePoint)

void ContextDs9::createPointCmd( const Vector& v, PointShape, int, const char*, int*, int, const char*,
        const char*, unsigned short, const char*, const std::list<Tag>& ){
    if ( v.size() >= 2 ){
        Carta::Lib::Regions::Point* info = new Carta::Lib::Regions::Point();
        info->setPoint( QPointF( v[0], v[1] ) );
        m_regions.push_back( info );
    }
}


void ContextDs9::createEllipseCmd( const Vector& center, const Vector& radius, double angle,
        const char* /*color*/, int* /*dash*/, int /*width*/, const char* /*font*/,
        const char* /*text*/, unsigned short /*prop*/, const char* /*comment*/,
        const std::list<Tag>& /*tag*/ ) {
    // 'width' is the line width... need to thread that through...
    Carta::Lib::Regions::Ellipse* info = new Carta::Lib::Regions::Ellipse();
    if ( radius[0] >= radius[1] ){
    	info->setRadiusMajor( radius[0] );
    	info->setRadiusMinor( radius[1] );
    }
    else {
    	info->setRadiusMajor( radius[1] );
    	info->setRadiusMinor( radius[0] );
    }
    info->setCenter( QPointF( center[0], center[1]) );
    info->setAngle( angle );
    m_regions.push_back( info );
}


void ContextDs9::createCircleCmd( const Vector& center, double radius, const char* color, int* dash,
        int width, const char* font, const char* text, unsigned short prop,
        const char* comment, const std::list<Tag>& tag ) {
    qDebug() << "createCircleCmd";
    std::vector<double> radii(2);
    radii[0] = radius;
    radii[1] = radius;
    double angle = 0;
    createEllipseCmd( center, radii, angle, color, dash, width, font, text, prop, comment, tag );
}

void ContextDs9::createPolygonCmd( const Vector& /*center*/, const Vector& /*bb*/, const char* /*color*/, int* /*dash*/,
        int /*width*/, const char* /*font*/, const char* /*text*/, unsigned short /*prop*/,
        const char* /*comment*/, const std::list<Tag>& /*tag*/ ) {
    qDebug() << "Context create polygon cmd";
}

void ContextDs9::createPolygonCmd( const std::list<Vertex>& verts, const char* /*color*/, int* /*dash*/,
        int /*width*/, const char* /*font*/, const char* /*text*/, unsigned short /*prop*/,
        const char* /*comment*/, const std::list<Tag>& /*tag*/ ) {
    // 'width' is the line width... need to thread that through...
    qDebug() << "createPolygonCmd";
    Carta::Lib::Regions::Polygon* info = new Carta::Lib::Regions::Polygon();
    QPolygonF corners;
    for ( std::list<Vertex>::const_iterator it=verts.begin( ); it != verts.end(); ++it ) {
        if ( (*it).size( ) < 2 ) return;
        corners.push_back( QPointF ((*it)[0], (*it)[1] ) );
    }
    info->setqpolyf( corners );
    m_regions.push_back( info );
}

std::vector<Carta::Lib::Regions::RegionBase* > ContextDs9::getRegions() const {
    return m_regions;
}

Vector ContextDs9::mapToRef(const Vector& v, CoordSystem /*sys*/, SkyFrame /*frame*/) {
    int count = v.size();
    qDebug() << "mapToRef needs to be implemented";
    Vector result( count );
    for ( int i = 0; i < count; i++ ){
        result[i] = v[i];
    }
    return result;
}

double ContextDs9::mapLenToRef(double d, CoordSystem /*sys*/, SkyDist /*frame*/) {
    double lx = d;
    qDebug() << "mapLenToRef needs to be implemented";

    return lx;
}

Vector ContextDs9::mapLenToRef(const Vector &v, CoordSystem /*sys*/, SkyDist /*format*/) {
    Vector result(v);
    qDebug() << "mapLenToRef needs to be implemented";
    return result;
}

double ContextDs9::mapAngleFromRef(double /*angle*/, CoordSystem /*sys*/, SkyFrame /*sky*/) {
    qDebug() << "mapAngleFromRef needs to be implemented";
    return 0;
}
double ContextDs9::mapAngleToRef(double /*angle*/, CoordSystem /*sys*/, SkyFrame /*sky*/) {
    qDebug() << "mapAngleToRef needs to be implemented";
    return 0;
}

double degToRad(double d) {
    double r =  M_PI * d / 180.;

    if (r > 0)
        while (r >= 2*M_PI) r -= 2*M_PI;
    else
        while (r < 0) r += 2*M_PI;

    return r;
}

double radToDeg(double r) {
    double d = 180. * r / M_PI;

    if (d > 0)
        while(d >= 360) d -= 360.;
    else
        while(d < 0) d += 360.;

    return d;
}

double dmsToDegree(int sign, int degree, int min, double sec) {
    // sign is needed because of -00 vs +00
    return double(sign) * (abs(degree) + (min/60.) + (sec/60./60.));
}

double parseSEXStr(const char* d) {
    char* dms = strdup(d); // its going to get clobbered
    char* ptr = dms;

    int sign = 1;
    int degree = atoi(strtok(ptr,":"));
    int minute = atoi(strtok(NULL,":"));
    float sec = atof(strtok(NULL,":"));

    // assumes the minus sign is the first char
    if (degree != 0)
        sign = degree>0 ? 1 : -1;
    else
        sign = d[0] == '-' ? -1 : 1;

    free(dms);

    return dmsToDegree(sign,abs(degree),minute,sec);
}

double parseHMSStr(const char* str) {
    char* hms = strdup(str); // its going to get clobbered
    char* ptr = hms;

    int sign = 1;
    int hour = atoi(strtok(ptr,"h"));
    int minute = atoi(strtok(NULL,"m"));
    float second = atof(strtok(NULL,"s"));

    // assumes the minus sign is the first char
    if (hour != 0)
        sign = hour>0 ? 1 : -1;
    else
        sign = str[0] == '-' ? -1 : 1;

    free(hms);
    return dmsToDegree(sign,abs(hour),minute,second)/24.*360.;
}

double parseDMSStr(const char* str) {
    char* dms = strdup(str); // its going to get clobbered
    char* ptr = dms;

    int sign = 1;
    int degree = atoi(strtok(ptr,"d"));
    int minute = atoi(strtok(NULL,"m"));
    float sec = atof(strtok(NULL,"s"));

    // assumes the minus sign is the first char
    if (degree != 0)
        sign = degree>0 ? 1 : -1;
    else
        sign = str[0] == '-' ? -1 : 1;

    free(dms);
    return dmsToDegree(sign,abs(degree),minute,sec);
}

std::vector<double> coordtovec( double *c ) {
    std::vector<double> result(3);
    result[0] = c[0];
    result[1] = c[1];
    result[2] = c[2];
    return result;
}

std::vector<double> doubletovec( double x, double y, double z ) {
    std::vector<double> result(3);
    result[0] = x;
    result[1] = y;
    result[2] = z;
    return result;
}

ContextDs9::~ContextDs9(){

}

