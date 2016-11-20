/**
 * Functions for serializing and deserializing keys and values to be used with the disk cache
 **/

#pragma once

/**
 * Int --> byte array
 **/ 
QByteArray i2qb( const int & d) {
    QByteArray ba;
    ba.append( (const char *)( & d), sizeof( int));
    return ba;
}

/**
 * Byte array --> int
 **/ 
int qb2i( const QByteArray & ba) {
    if( ba.size() != sizeof(int)) {
        throw std::runtime_error("Could not unpack QByteArray into int: size is incorrect.");
    }
    return * ((const int *) (ba.constData()));
}

/**
 * Double --> byte array
 **/ 
QByteArray d2qb( const double & d) {
    QByteArray ba;
    ba.append( (const char *)( & d), sizeof( double));
    return ba;
}

/**
 * Byte array --> double
 **/ 
double qb2d( const QByteArray & ba) {
    if( ba.size() != sizeof(double)) {
        throw std::runtime_error("Could not unpack QByteArray into double: size is incorrect.");
    }
    return * ((const double *) (ba.constData()));
}

/**
 * Vector of doubles --> byte array
 **/ 
QByteArray vd2qb( const std::vector<double> & vd) {
    QByteArray ba;
    for( const double & d : vd) {
        ba.append( (const char *)( & d), sizeof( double));
    }
    return ba;
}

/**
 * Byte array --> vector of doubles
 **/ 
std::vector<double> qb2vd( const QByteArray & ba) {
    std::vector<double> vd;
    if( ba.size() % sizeof(double) != 0) {
        throw std::runtime_error("Could not unpack QByteArray into std::vector<double>: size is incorrect.");
    }
    const char * cptr = ba.constData();
    for( int i = 0 ; i < ba.size() ; i += sizeof(double)) {
        vd.push_back( * ((const double *) (cptr + i)));
    }
    return vd;
}

/**
 * Vector of integers --> byte array
 **/ 
QByteArray vi2qb( const std::vector<int> & vi) {
    QByteArray ba;
    for( const int & i : vi) {
        ba.append( (const char *)( & i), sizeof( int));
    }
    return ba;
}

/**
 * Byte array --> vector of integers
 **/ 
std::vector<int> qb2vi( const QByteArray & ba) {
    std::vector<int> vi;
    if( ba.size() % sizeof(int) != 0) {
        throw std::runtime_error("Could not unpack QByteArray into std::vector<int>: size is incorrect.");
    }
    const char * cptr = ba.constData();
    for( int i = 0 ; i < ba.size() ; i += sizeof(int)) {
        vi.push_back( * ((const int *) (cptr + i)));
    }
    return vi;
}

/**
 * Pair of int, double --> byte array
 **/ 
QByteArray id2qb( const std::pair<int, double> & id) {
    QByteArray ba;
    ba.append( (const char *)( & id.first), sizeof( int));
    ba.append( (const char *)( & id.second), sizeof( double));
    return ba;
}


/**
 * Byte array --> pair of int, double
 **/ 
std::pair<int, double> qb2id( const QByteArray & ba) {
    if( ba.size() != (sizeof(double) + sizeof(int))) {
        throw std::runtime_error("Could not unpack QByteArray into std::pair<int, double>: size is incorrect.");
    }
    const char * cptr = ba.constData();
    int int_val( * ((const int *) (cptr)));
    double double_val( * ((const double *) (cptr + sizeof(int))));
    return std::make_pair(int_val, double_val);
}
