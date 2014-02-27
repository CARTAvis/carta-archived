/**
 * This is the license for the CyberSKA image viewer.
 *
 **/

#include "PureWeb.h"

void pwdeltree(const QString &path) {
    GetStateManager().XmlStateManager().DeleteTree( path.toStdString());
}
