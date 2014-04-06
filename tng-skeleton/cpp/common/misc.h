/* Miscelanous utilities, for which we have not found a good home.
 *
 *
 */


#ifndef COMMON_MISC_H
#define COMMON_MISC_H

template< typename STREAM>
STREAM& operator<< ( STREAM & stream, const QString & str )
{
    return stream << str.toStdString();
}

template< typename STREAM>
STREAM& operator>> ( STREAM & stream, QString & str )
{
    std::string tmpstr;
    stream >> tmpstr;
    str = tmpstr.c_str();
    return stream;
}


#endif // COMMON_MISC_H
