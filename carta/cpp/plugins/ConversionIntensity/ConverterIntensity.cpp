#include "ConverterIntensity.h"
#include <math.h>
#include <QDebug>

const double ConverterIntensity::BOLTZMANN = 1.38e-23;
const double ConverterIntensity::LIGHT_SPEED_FACTOR = 9e-10; // includes a unit conversion; find out exactly what this is
const double ConverterIntensity::ARCSECONDS_SQUARED_PER_STERADIAN = pow(180 * 3600 / M_PI, 2);
const double ConverterIntensity::JY_IN_MJY = 1e6;

//const QString ConverterIntensity::FRACTION_OF_PEAK = "Fraction of Peak";
//const QString ConverterIntensity::JY_BEAM = "Jy/beam";
//const QString ConverterIntensity::JY_SR = "MJy/sr";
//const QString ConverterIntensity::JY_ARCSEC = "Jy/arcsec^2";
//const QString ConverterIntensity::JY = "Jy";
//const QString ConverterIntensity::KELVIN = "Kelvin";
//const QString ConverterIntensity::TIMES_PIXELS = "*pixels";
//const double ConverterIntensity::SPEED_LIGHT_FACTOR = 0.0000000009;
//const double ConverterIntensity::FREQUENCY_FACTOR = 2 * 0.0000000000000000000000138;
//const double ConverterIntensity::ARCSECONDS_PER_STERADIAN = 206.265 * 206.265;

//const QList<QString> ConverterIntensity::BEAM_UNITS =
        //QList<QString>() << "pJy/beam" <<"10pJy/beam"<<"100pJy/beam"<<
        //"nJy/beam"<<"10nJy/beam"<<"100nJy/beam"<<
        //"uJy/beam"<<"10uJy/beam"<<"100uJy/beam"<<
        //"mJy/beam"<<"10mJy/beam"<<"100mJy/beam"<<
        //"Jy/beam"<<"10Jy/beam"<<"100Jy/beam"<<
        //"kJy/beam"<<"10kJy/beam"<<"100kJy/beam"<<
        //"MJy/beam"<<"10MJy/beam"<<"100MJy/beam"<<
        //"GJy/beam";

//const QList<QString> ConverterIntensity::JY_UNITS =
        //QList<QString>() << "pJy/arcsec^2" <<"10pJy/arcsec^2"<<"100pJy/arcsec^2"<<
        //"nJy/arcsec^2"<<"10nJy/arcsec^2"<<"100nJy/arcsec^2"<<
        //"uJy/arcsec^2"<<"10uJy/arcsec^2"<<"100uJy/arcsec^2"<<
        //"mJy/arcsec^2"<<"10mJy/arcsec^2"<<"100mJy/arcsec^2"<<
        //"Jy/arcsec^2"<<"10Jy/arcsec^2"<<"100Jy/arcsec^2"<<
        //"kJy/arcsec^2"<<"10kJy/arcsec^2"<<"100kJy/arcsec^2"<<
        //"MJy/arcsec^2"<<"10MJy/arcsec^2"<<"100MJy/arcsec^2"<<
        //"GJy/arcsec^2";

//const QList<QString> ConverterIntensity::JY_SR_UNITS =
        //QList<QString>() << "pMJy/sr" <<"10pMJy/sr"<<"100pMJy/sr"<<
        //"nMJy/sr"<<"10nMJy/sr"<<"100nMJy/sr"<<
        //"uMJy/sr"<<"10uMJy/sr"<<"100uMJy/sr"<<
        //"mMJy/sr"<<"10mMJy/sr"<<"100mMJy/sr"<<
        //"MJy/sr"<<"10MJy/sr"<<"100MJy/sr"<<
        //"kMJy/sr"<<"10kMJy/sr"<<"100kMJy/sr"<<
        //"MMJy/sr"<<"10MMJy/sr"<<"100MMJy/sr"<<
        //"GMJy/sr";

//const QList<QString> ConverterIntensity::KELVIN_UNITS =
        //QList<QString>() << "pKelvin" <<"10pKelvin"<<"100pKelvin"<<
        //"nKelvin"<<"10nKelvin"<<"100nKelvin"<<
        //"uKelvin"<<"10uKelvin"<<"100uKelvin"<<
        //"mKelvin"<<"10mKelvin"<<"100mKelvin"<<
        //"Kelvin"<<"10Kelvin"<<"100Kelvin"<<
        //"kKelvin"<<"10kKelvin"<<"100kKelvin"<<
        //"MKelvin"<<"10MKelvin"<<"100MKelvin"<<
        //"GKelvin";


ConverterIntensity::ConverterIntensity() {
}

void ConverterIntensity::convert(
        std::vector<double>& values, const std::vector<double>& hertzValues,
        const QString& oldUnits, const QString& newUnits,
        double maxValue, const QString& maxUnits,
        double beamAngle, double beamArea ) {

    std::function<double(double, double)> lambda;
    double multiplier;

    std::tie(lambda, multiplier) = converters(values, hertzValues, oldUnits, newUnits, maxValue, maxUnits, beamAngle, beamArea);

    for (size_t i = 0; i < values.size(); i++) {
        values[i] = lambda(values[i], hertzValues[i]) * multiplier;
    }
}

std::tuple<std::function<double(double, double)>, double> ConverterIntensity::converters(
        std::vector<double>& values, const std::vector<double>& hertzValues,
        const QString& oldUnits, const QString& newUnits,
        double maxValue, const QString& maxUnits,
        double beamAngle, double beamArea ) {

    double multiplier(1);
    std::function<double(double, double)> lambda = [](double y, double x){ std::ignore = x; return y; };

    int fromExponent, toExponent;
    QString fromBase, toBase;

    if (newUnits == "Fraction of Peak") {
        // Currently the only allowed option for converting *to* FOP is *from* the same units as the maximum value
        // No other conversions are possible
        if (oldUnits != "Fraction of Peak") { // which would be a no-op
            if (oldUnits != maxUnits) {
                // TODO: we need better error handling in the plugin
                qWarning("In conversion to Fraction of Peak, maximum value units don't match intensity units. Result may be incorrect.");
            }
            
            // do the conversion anyway for now; that's what the plugin did before
            multiplier /= maxValue;
        }
    } else {
        if (oldUnits == "Fraction of Peak") {
            multiplier *= maxValue;
            std::tie(fromExponent, fromBase) = splitUnits(maxUnits);
        } else {
            std::tie(fromExponent, fromBase) = splitUnits(oldUnits);
        }
    
        std::tie(toExponent, toBase) = splitUnits(newUnits);

        multiplier *= pow(10, fromExponent - toExponent);

        if (fromBase == "Kelvin" && toBase != "Kelvin") {
            lambda = [](double y, double x){ return y * pow(x, 2); };
        } else if (toBase == "Kelvin" && fromBase != "Kelvin") {
            lambda = [](double y, double x){return y / pow(x, 2); };
        } // else keep the default lambda
        
        // TODO: precision -- optimise the order of terms in the multipliers

        if (fromBase == "Kelvin") {
            if (toBase == "Jy/beam") {
                multiplier *= (2 * BOLTZMANN * beamAngle) / LIGHT_SPEED_FACTOR;
            } else if (toBase == "Jy/arcsec^2") {
                multiplier *= (2 * BOLTZMANN * beamAngle) / (beamArea * LIGHT_SPEED_FACTOR);
            } else if (toBase == "MJy/sr") {
                multiplier *= (ARCSECONDS_SQUARED_PER_STERADIAN * 2 * BOLTZMANN * beamAngle) / (beamArea * JY_IN_MJY * LIGHT_SPEED_FACTOR);
            }
        } else if (fromBase == "Jy/beam") {
            if (toBase == "Kelvin") {
                multiplier *= LIGHT_SPEED_FACTOR / (2 * BOLTZMANN * beamAngle);
            } else if (toBase == "Jy/arcsec^2") {
                multiplier *= 1 / beamArea;
            } else if (toBase == "MJy/sr") {
                multiplier *= ARCSECONDS_SQUARED_PER_STERADIAN / (beamArea * JY_IN_MJY);
            }
        } else if (fromBase == "Jy/arcsec^2") {
            if (toBase == "Kelvin") {
                multiplier *= (beamArea * LIGHT_SPEED_FACTOR) / (2 * BOLTZMANN * beamAngle);
            } else if (toBase == "Jy/beam") {
                multiplier *= beamArea;
            } else if (toBase == "MJy/sr") {
                multiplier *= ARCSECONDS_SQUARED_PER_STERADIAN / JY_IN_MJY;
            }
        } else if (fromBase == "MJy/sr") {
            if (toBase == "Kelvin") {
                multiplier *= (beamArea * JY_IN_MJY * LIGHT_SPEED_FACTOR) / (ARCSECONDS_SQUARED_PER_STERADIAN * 2 * BOLTZMANN * beamAngle);
            } else if (toBase == "Jy/beam") {
                multiplier *= (beamArea * JY_IN_MJY) / ARCSECONDS_SQUARED_PER_STERADIAN;
            } else if (toBase == "Jy/arcsec^2") {
                multiplier *= JY_IN_MJY / ARCSECONDS_SQUARED_PER_STERADIAN;
            }
        }
    }

    return std::make_tuple(lambda, multiplier);
}

std::tuple<int, QString> ConverterIntensity::splitUnits(const QString& units) {
    int exponent(1);
    QString baseUnits("");

    const QStringList BASE_UNITS = {"Jy/beam", "Jy/arcsec^2", "MJy/sr", "Kelvin"};
    const QMap<QString, int> SI_PREFIX = {{"p", -12}, {"n", -9}, {"u", -6}, {"m", -3}, {"", 0}, {"k", 3}, {"M", 6}, {"G", 9}};

    QString baseUnitList = BASE_UNITS.join("|");
    QString siPrefixList = QStringList(SI_PREFIX.keys()).join("|");

    QRegExp rx("^(10+)?(" + siPrefixList + ")?(" + baseUnitList + ")(\\*pixels)?$");
    int pos = rx.indexIn(units);
    if (pos > 1) {
        if (rx.cap(1) != "") {
            exponent += std::round(log10(rx.cap(1).toInt()));
        }

        if (rx.cap(2) != "") {
            exponent += SI_PREFIX[rx.cap(2)];
        }

        baseUnits = rx.cap(3);
    }

    return std::make_tuple(exponent, baseUnits);
}

//bool ConverterIntensity::isSupportedUnits( const QString& yUnit ) {
    //bool acceptable = false;
    //if ( yUnit.contains( JY ) || yUnit.contains( KELVIN ) ||
            //yUnit.contains( FRACTION_OF_PEAK)) {
        //acceptable = true;
    //}
    //return acceptable;
//}

//QString ConverterIntensity::stripPixels( const QString& units ) {
    //int pixelIndex = units.indexOf( TIMES_PIXELS );
    //QString strippedUnits = units;
    //if ( pixelIndex > 0 ) {
        //strippedUnits = units.left(pixelIndex );
    //}
    //return strippedUnits;
//}

//void ConverterIntensity::convert( std::vector<double>& values,
        //const std::vector<double>& hertzValues,
        //const QString& oldUnits, const QString& newUnits,
        //double maxValue, const QString& maxUnits,
        //double beamAngle, double beamArea ) {

    //bool supportedUnits = isSupportedUnits( oldUnits );
    //if ( supportedUnits ) {
        //supportedUnits = isSupportedUnits( newUnits );
    //}
    //if ( !supportedUnits ) {
        //return;
    //}

    //QString newUnitsBase = stripPixels( newUnits );
    //QString oldUnitsBase = stripPixels( oldUnits );
    //QString maxUnitsBase = stripPixels( maxUnits );

    ////Change fraction of peak back to the original units before converting.  We don't
    ////want the current values in fraction of peak going forward.
    //QString baseConvertUnits = oldUnitsBase;
    //int maxPoints = values.size();
    //if ( oldUnitsBase == FRACTION_OF_PEAK && newUnitsBase != FRACTION_OF_PEAK) {
        //for ( int i = 0; i < maxPoints; i++ ) {
            //values[i] = percentToValue( values[i], maxValue);
        //}
        //baseConvertUnits = maxUnitsBase;
    //}

    ////Exit if we don't have anything to do.
    //if ( baseConvertUnits == newUnitsBase ) {
        //return;
    //}

    //if ( newUnitsBase == FRACTION_OF_PEAK ) {
        ////Scale the vector
        //for ( int i = 0; i < maxPoints; i++ ) {
            //values[ i ] = valueToPercent( values[i], maxValue );
        //}
    //}

    //else {
        ////If the original units are in JY or JY_BEAM, strip off a prefix such as
        ////m, k, etc and adjust the data.
        //QString strippedBase = baseConvertUnits;
        //if ( isJansky( baseConvertUnits ) ) {
            //strippedBase = getJanskyBaseUnits( baseConvertUnits );
            //convertJansky( values, baseConvertUnits, strippedBase/*, coord*/ );
        //} else if ( isKelvin( baseConvertUnits )) {
            //strippedBase = getKelvinBaseUnits( baseConvertUnits );
            //convertKelvin( values, baseConvertUnits, strippedBase/*, coord*/ );
        //}

        //QString strippedNew = newUnitsBase;
        //if ( isJansky( newUnitsBase)) {
            //strippedNew = getJanskyBaseUnits( newUnitsBase );
        //}
        //else if ( isKelvin( newUnitsBase)) {
            //strippedNew = getKelvinBaseUnits( newUnitsBase );
        //}

        ////Do the conversion using the base units without the prefix.
        //int hertzCount = hertzValues.size();
        //for ( int i = 0; i < maxPoints; i++ ) {
        	//double hertzValue = 0;
        	//if ( i < hertzCount ){
        		//hertzValue = hertzValues[i];
        	//}
            //values[ i ] = convertQuantity( values[i], hertzValue,
                    //strippedBase, strippedNew,
                    //beamAngle, beamArea );
        //}

        ////Add any additional prefix back in.
        //if ( isJansky( newUnitsBase ) ) {
            //convertJansky( values, strippedNew, newUnitsBase/*, coord*/ );
        //} else if ( isKelvin( newUnitsBase )) {
            //convertKelvin( values, strippedNew, newUnitsBase/*, coord*/ );
        //}
    //}
//}

//QString ConverterIntensity::getJanskyBaseUnits( const QString& units ) {
    //QString baseUnits = units;
    //int jyIndex = units.indexOf( JY );
    //if ( jyIndex >= 0 ) {
        //int mJyIndex = units.indexOf( JY_SR );
        //if (mJyIndex >= 0 ) {
            //baseUnits = units.mid( mJyIndex, units.length() - mJyIndex );
        //} else {
            //baseUnits = units.mid( jyIndex, units.length() - jyIndex );
        //}
    //}
    //return baseUnits;
//}

//QString ConverterIntensity::getKelvinBaseUnits( const QString& units ) {
    //QString baseUnits = units;
    //int kelvinIndex = units.indexOf( KELVIN );
    //if ( kelvinIndex > 0 ) {
        //baseUnits = units.mid( kelvinIndex, units.length() - kelvinIndex );
    //}
    //return baseUnits;
//}

//bool ConverterIntensity::isKelvin( const QString& units ) {
    //bool kelvinUnits = false;
    //if ( units.indexOf( KELVIN) >= 0 ) {
        //kelvinUnits = true;
    //}
    //return kelvinUnits;
//}

//bool ConverterIntensity::isJansky( const QString& units ) {
    //bool janskyUnits = false;
    //if ( units.indexOf( JY ) >=0  ) {
        //janskyUnits = true;
    //}
    //return janskyUnits;
//}

//void ConverterIntensity::convertJansky( std::vector<double>& values, const QString& oldUnits,
        //const QString& newUnits ) {
    //if ( oldUnits.indexOf( JY_BEAM) >= 0 && newUnits.indexOf( JY_BEAM) >= 0 ) {
        //for ( int i = 0; i < static_cast<int>(values.size()); i++ ) {
            //values[i] = convertJyBeams( oldUnits, newUnits, values[i]);
        //}
    //}

    //else if ( oldUnits.indexOf( JY_SR) >= 0 && newUnits.indexOf( JY_SR) >= 0) {
        //for ( int i = 0; i < static_cast<int>(values.size()); i++ ) {
            //values[i] = convertJYSR( oldUnits, newUnits, values[i]);
        //}
    //} else {
        //for ( int i = 0; i < static_cast<int>(values.size()); i++ ) {
            //values[i] = convertJY( oldUnits, newUnits, values[i]);
        //}
    //}
//}

//void ConverterIntensity::convertKelvin( std::vector<double>& values,
        //const QString& oldUnits, const QString& newUnits) {
    //for ( int i = 0; i < static_cast<int>(values.size()); i++ ) {
        //if ( oldUnits.indexOf( KELVIN) >= 0 && newUnits.indexOf( KELVIN) >= 0 ) {
            //values[i] = convertKelvin( oldUnits, newUnits, values[i]);
        //}
    //}
//}

//double ConverterIntensity::valueToPercent( double yValue, double maxValue ) {
    //double convertedYValue = yValue / maxValue;
    //return convertedYValue;
//}


//double ConverterIntensity::percentToValue( double yValue, double maxValue ) {
    //double convertedYValue = yValue * maxValue;
    //return convertedYValue;
//}

//double ConverterIntensity::beamToArcseconds( double yValue, double beamArea ) {
    //double convertedValue = yValue;
    //if ( beamArea != 0 ) {
        //convertedValue = yValue / beamArea;
    //}
    //return convertedValue;
//}

//double ConverterIntensity::arcsecondsToBeam( double yValue, double beamArea ) {
    //double convertedValue = yValue;
    //if ( beamArea != 0 ) {
        //convertedValue = yValue * beamArea;
    //}
    //return convertedValue;
//}

//double ConverterIntensity::srToArcseconds( double yValue ) {
    //double convertedValue = yValue / ARCSECONDS_PER_STERADIAN;
    //return convertedValue;
//}

//double ConverterIntensity::arcsecondsToSr( double yValue ) {
    //double convertedValue = yValue * ARCSECONDS_PER_STERADIAN;
    //return convertedValue;
//}

//void ConverterIntensity::convert( std::vector<double> &resultValues, int sourceIndex,
        //int destIndex) {

    //if ( sourceIndex >= 0 && destIndex >= 0 ) {
        //int diff = qAbs( destIndex - sourceIndex );
        //float power = pow( 10, diff );
        //if ( destIndex > sourceIndex ) {
            //power = 1 / power;
        //}
        //for ( int i = 0; i < static_cast<int>(resultValues.size()); i++ ) {
            //resultValues[i] = resultValues[i] * power;
        //}
    //} else {
       //// qDebug() <<  "Converter: could not convert sourceIndex=" <<
        ////                sourceIndex << " destIndex=" << destIndex;
    //}
//}

//double ConverterIntensity::convertNonKelvinUnits( double value,
        //const QString& oldUnits, const QString& newUnits, double beamArea ) {
    //double convertedValue = value;
    //if ( oldUnits != newUnits ){
        //if ( oldUnits == JY_BEAM ) {
            //if ( newUnits != JY_BEAM ) {
                //convertedValue = beamToArcseconds( value, beamArea );
                //if ( newUnits == JY_SR ) {
                    //convertedValue = arcsecondsToSr( convertedValue );
                //}
            //}
        //}
        //else if ( oldUnits == JY_SR ) {
            //if ( newUnits != JY_SR ) {
                //convertedValue = srToArcseconds( value );
                //if ( newUnits == JY_BEAM ) {
                    //convertedValue = arcsecondsToBeam( convertedValue, beamArea );
                //}
            //}
        //} else if ( oldUnits == JY_ARCSEC ) {
            //if ( newUnits != JY_ARCSEC ) {
                //if ( newUnits == JY_SR ) {
                    //convertedValue = arcsecondsToSr( value );
                //} else if ( newUnits == JY_BEAM ) {
                    //convertedValue = arcsecondsToBeam( value, beamArea );
                //} else {
                    //qDebug()<<"Unsupported units: "<<newUnits;
                //}

            //}
        //} else {
            ////qDebug() << "Unrecognized units:"<<oldUnits;
        //}
    //}
    //return convertedValue;
//}


//double ConverterIntensity::convertQuantity( double yValue, double frequencyValue,
        //const QString& oldUnits, const QString& newUnits, double beamSolidAngle,
        //double beamArea ) {

    //double convertedYValue = yValue;
    //if ( oldUnits != KELVIN && newUnits != KELVIN ) {
        //convertedYValue = convertNonKelvinUnits( yValue, oldUnits, newUnits, beamArea );
    //}
    //else if ( oldUnits == KELVIN && newUnits != KELVIN ) {
        //if ( beamSolidAngle > 0 ) {
            ////kelvin * solidAngle * 2 * 1.38 x 10^-23 * freq^2 / (10^-32 x (3 x 10^8)^2)
            //double num = yValue * beamSolidAngle * FREQUENCY_FACTOR * pow( frequencyValue, 2);
            //double den = SPEED_LIGHT_FACTOR;
            //double jyBeamValue = num / den;

            ////New units are now JY/BEAM convert them to what we need
            //convertedYValue = convertNonKelvinUnits( jyBeamValue, JY_BEAM, newUnits, beamArea );
        //} else {
            //qDebug() << "Could not convert from Kelvin because the beam solid angle was 0";
        //}
    //} else if ( oldUnits != KELVIN && newUnits == KELVIN ){
        ////oldUnits != KELVIN && newUnits == KELVIN
        //if ( beamSolidAngle > 0 ) {
            ////First convert the value to Jy/Beam
            //double valueBeam = convertNonKelvinUnits( yValue, oldUnits, JY_BEAM, beamArea);

            ////Temperature in Kelvin is now:
            ////Jy/beam x 10^(-32) x (3 x 10^8)^2 / ( solidAngle x 2 x 1.38 x 10^-23 x (xvalueinHz)^2 ).
            //double num = valueBeam * SPEED_LIGHT_FACTOR;
            //double den = beamSolidAngle * FREQUENCY_FACTOR * pow(frequencyValue,2);
            //convertedYValue = num / den;
        //} else {
            //qDebug() << "Could not convert to Kelvin because the beamSolidAngle was 0";
        //}
    //}
    //return convertedYValue;
//}

//double ConverterIntensity::convertJY( const QString& oldUnits, const QString& newUnits,
        //double value ) {
    //int sourceIndex = JY_UNITS.indexOf( oldUnits );
    //int destIndex = JY_UNITS.indexOf( newUnits );
    //std::vector<double> resultValues(1);
    //resultValues[0] = value;
    //ConverterIntensity::convert( resultValues, sourceIndex, destIndex );
    //return resultValues[0];
//}

//double ConverterIntensity::convertJYSR( const QString& oldUnits,
        //const QString& newUnits, double value ) {
    //int sourceIndex = JY_SR_UNITS.indexOf( oldUnits );
    //int destIndex = JY_SR_UNITS.indexOf( newUnits );
    //std::vector<double> resultValues(1);
    //resultValues[0] = value;
    //ConverterIntensity::convert( resultValues, sourceIndex, destIndex );
    //return resultValues[0];
//}

//double ConverterIntensity::convertJyBeams( const QString& oldUnits,
        //const QString& newUnits, double value ) {
    //int sourceIndex = BEAM_UNITS.indexOf( oldUnits );
    //int destIndex = BEAM_UNITS.indexOf( newUnits );
    //std::vector<double> resultValues(1);
    //resultValues[0] = value;
    //ConverterIntensity::convert( resultValues, sourceIndex, destIndex );
    //return resultValues[0];
//}

//double ConverterIntensity::convertKelvin( const QString& oldUnits,
        //const QString& newUnits, double value) {
    //int sourceIndex = KELVIN_UNITS.indexOf( oldUnits );
    //int destIndex = KELVIN_UNITS.indexOf( newUnits );
    //std::vector<double> resultValues(1);
    //resultValues[0] = value;
    //ConverterIntensity::convert( resultValues, sourceIndex, destIndex );
    //return resultValues[0];
//}

ConverterIntensity::~ConverterIntensity() {
}
