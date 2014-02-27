#include <cstdlib>
#include <time.h>
#include <cmath>

#include <QBuffer>
#include <QPainter>

#include "common.h"

int Debug::verbocity = 1000;
bool Debug::displayLevel = true;
bool Debug::displaySource = true;
bool Debug::displayFunctionName = false;
bool Debug::displayThreadId = true;
QTime Debug::m_time;

//std::ostream & operator << ( std::ostream & out, const QString & s)
//{
//    out << s.toStdString();
//    return out;
//}
std::basic_ostream<char> & operator << ( std::basic_ostream<char> & out, const QString & s)
{
    out << s.toStdString();
    return out;
}

std::basic_istream<char> & operator >> ( std::basic_istream<char> & inp, QString & s)
{
    std::string ss;
    inp >> ss;
    s = ss.c_str ();
    return inp;
}

QTextStream & operator << ( QTextStream & out, const std::string & s)
{
    out << s.c_str ();
    return out;
}

QTextStream &operator <<(QTextStream &out, const QPoint &pt)
{
    out << pt.x() << " " << pt.y();
    return out;
}

QTextStream &operator <<(QTextStream &out, const QPointF &pt)
{
    out << pt.x() << " " << pt.y();
    return out;
}




void fatalError( const QString & s)
{
    std::cerr << ConsoleColors::bright() << ConsoleColors::redBg() << ConsoleColors::yellow()
              << "Exception occured: " << s.toStdString()
              << ConsoleColors::reset() << "\n";
    std::cerr << "FATAL ERROR\n";
    exit(-1);
}
void fatalError( const char * s) { fatalError( QString(s)); }
void fatalError( const std::string & s) { fatalError( QString(s.c_str())); }


namespace RaiLib {
void
print_trace (std::ostream &out)
{
  void *array[10];
  size_t size;
  char **strings;
  size_t i;

  size = backtrace (array, 1000);
  strings = backtrace_symbols (array, size);

  out << "Stack frames: " << size << "\n";

  for (i = 0; i < size; i++)
      out << "  - " << strings[i] << "\n";

  free (strings);
}
};


QString image2base64( const QImage & pimg, int quality, int frame)
{
    QBuffer buff;

    QImage img = pimg;
    {
        QPainter p( & img);
        p.setFont( QFont("Times", 24, QFont::Bold));
        p.setPen( "red");
        p.drawText(QRectF(img.rect()),Qt::AlignTop | Qt::AlignLeft,
                   QString("Quality %1 frame %2").arg(quality).arg(frame));
    }

    if( quality < 100) {
        if( ! img.save(&buff, "JPG", quality))
            throw "JPEG compression failed";
        return "data:image/jpeg;base64," + buff.data().toBase64();
    } else {
        if( ! img.save(&buff, "PNG", 100))
            throw "PNG compression failed";
        return "data:image/png;base64," + buff.data().toBase64();
    }
}

Debug::DebugReceiver * Debug::debugReceiver_ = 0;
QMutex Debug::m_mutex;

void qsleep (double seconds)
{
    timespec sp;
    sp.tv_sec = std::floor ( seconds);
    sp.tv_nsec = std::floor( 1000000000L * (seconds - sp.tv_sec));
    // paranoia
    sp.tv_nsec = std::min( 999999999L, sp.tv_nsec);
    nanosleep( & sp, 0);
}

