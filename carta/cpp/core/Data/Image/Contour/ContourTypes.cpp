#include "ContourTypes.h"
#include "Data/Util.h"
#include "State/UtilState.h"
#include <QDebug>
#include <set>

namespace Carta {
namespace Data {

const QString ContourTypes::CLASS_NAME = "ContourTypes";
const QString ContourTypes::TYPE_MODE = "typeModes";
const QString ContourTypes::MODE_NOLINECOMBOPT = "No line combiner";
const QString ContourTypes::MODE_LINECOMBOPT = "Line combiner";
const QString ContourTypes::MODE_GAUSSBLUR3 = "Gaussian blur 3x3";
const QString ContourTypes::MODE_GAUSSBLUR5 = "Gaussian blur 5x5";
const QString ContourTypes::MODE_BOXBLUR3 = "Box blur 3x3";
const QString ContourTypes::MODE_BOXBLUR5 = "Box blur 5x5";

class ContourTypes::Factory : public Carta::State::CartaObjectFactory {

    public:

        Factory():
            CartaObjectFactory(CLASS_NAME){}

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new ContourTypes (path, id);
        }
    };

bool ContourTypes::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass (CLASS_NAME, new ContourTypes::Factory());

ContourTypes::ContourTypes(const QString& path, const QString& id):
    CartaObject(CLASS_NAME, path, id){
    m_typeModes.resize( 6 );
    m_typeModes[0] = MODE_NOLINECOMBOPT;
    m_typeModes[1] = MODE_LINECOMBOPT;
    m_typeModes[2] = MODE_GAUSSBLUR3;
    m_typeModes[3] = MODE_GAUSSBLUR5;
    m_typeModes[4] = MODE_BOXBLUR3;
    m_typeModes[5] = MODE_BOXBLUR5;
    _initializeDefaultState();
}

QString ContourTypes::getContourType(const QString& type) {
    QString actualType;
    int typeCount = m_typeModes.size();
    for (int i = 0; i < typeCount; i++) {
        if (QString::compare(type, m_typeModes[i], Qt::CaseInsensitive) == 0) {
            actualType = m_typeModes[i];
            break;
        }
    }
    return actualType;
}

QString ContourTypes::getContourTypeDefault() const {
    return MODE_GAUSSBLUR3;
}

void ContourTypes::_initializeDefaultState() {
    int typeCount = m_typeModes.size();
    m_state.insertArray(TYPE_MODE, typeCount);
    for (int i = 0; i < typeCount; i++) {
        QString lookup = Carta::State::UtilState::getLookup(TYPE_MODE, i);
        m_state.setValue<QString>(lookup, m_typeModes[i]);
    }
    m_state.flushState();
}

ContourTypes::~ContourTypes() {

}

} // namespace Data
} // namespace Carta
