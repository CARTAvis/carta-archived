/**
 * Reaing global settings file at startup.
 *
 **/

#pragma once

#include <QJsonObject>
#include <QStringList>
class QString;

namespace MainConfig {

///
/// \brief The ParsedInfo contains parsed data from the main configuration file
///
class ParsedInfo {

public:

    ///
    /// \brief get a list of plugin directories
    ///
    const QStringList & pluginDirectories() const;

    /**
     * Returns any valid user set histogram maximum bin count or -1 if no
     * valid user supplied value has been provided.
     * @return the maximum allowed value for the histogram bin count or -1 is
     *   no valid value has been specified.
     */
    int getHistogramBinCountMax() const;


    /**
     * Returns any valid user set contour level count maximum or -1 if no
     * valid user supplied value has been provided.
     * @return the maximum allowed value for the contour level count or -1 is
     *   no valid value has been specified.
     */
    int getContourLevelCountMax() const;

    /// whether hacks are enabled or not
    bool hacksEnabled() const;

    /**
     * Returns whether CARTA should come up with areas under
     * active development shown.
     */
    bool isDeveloperLayout() const;

    /**
     * Returns whether CARTA should use approximation method
     * for percentile calculation, if the percentile is not equal to 0 or 1.
     */
    bool isPercentileApproximation() const;

    /**
     * Returns the value used to divide the range of pixel value: (max - min),
     * so the pixel bin size used for percentile approximation is (max - min) / getPercentApproxDividedNum.
     */
    unsigned int getPercentApproxDividedNum() const;

    /// the whole config file as json
    const QJsonObject & json() const;

    /**
     * Converts a jsonValue to a boolean.
     * @param jsonValue - the value to convert.
     * @param errorMsg - a place holder for setting an error message if the
     *      passed in value cannot be converted.
     * @return - the corresponding boolean; default is false if there is an
     *      error doing the conversion.
     */
    static bool toBool( const QJsonValue& jsonValue, QString& errorMsg );

    /**
     * Converts a jsonValue to an integer.
     * @param jsonValue - the value to convert.
     * @param errorMsg - a place holder for setting an error message if the
     *      passed in value cannot be converted.
     * @return - the corresponding integer; default is -1 if there is an
     *      error doing the conversion.
     */
    static int toInt( const QJsonValue& jsonValue, QString& errorMsg );

    /**
     * Converts a jsonValue to an unsigned integer.
     * @param jsonValue - the value to convert.
     * @param errorMsg - a place holder for setting an error message if the
     *      passed in value cannot be converted.
     * @return - the corresponding unsigned integer; default is 1000000 if there is an
     *      error doing the conversion.
     */
    static unsigned int toUnsignedInt( const QJsonValue& jsonValue, QString& errorMsg );
    
    /**
     * Allows modification of the JSON object by tests.
     */
    QJsonObject::iterator insert(const QString &key, const QJsonValue &value);

protected:

    QStringList m_pluginDirectories;
    bool m_hacksEnabled = false;
    bool m_percentileApproximation = false;
    bool m_developerLayout = false;
    int m_histogramBinCountMax = -1;
    int m_contourLevelCountMax = -1;
    unsigned int m_percentApproxDividedNum = 1000000;

    QJsonObject m_json;

    friend ParsedInfo parse( const QString & filePath);
};

///
/// \brief parse the given config file
/// \param path to the JSON file containing the configuration
/// \return parsed config file
///
ParsedInfo parse( const QString & filePath);



}
