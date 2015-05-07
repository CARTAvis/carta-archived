#include "Layout.h"
#include "Colormap/Colormap.h"
#include "Animator/Animator.h"
#include "Colormap/Colormap.h"
#include "State/UtilState.h"
#include "Controller.h"
#include "Histogram.h"
#include "Statistics.h"
#include "ViewPlugins.h"
#include "Util.h"

#include <QDir>
#include <QDebug>

namespace Carta {

namespace Data {

class Layout::Factory : public Carta::State::CartaObjectFactory {

public:

    Factory():
        CartaObjectFactory( LAYOUT ){};

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new Layout (path, id);
    }
};

const QString Layout::COLUMN = "col";
const QString Layout::EMPTY = "Empty";
const QString Layout::LAYOUT = "Layout";
const QString Layout::LAYOUT_ROWS = "rows";
const QString Layout::HIDDEN = "Hidden";
const QString Layout::LAYOUT_COLS = "cols";
const QString Layout::LAYOUT_PLUGINS = "plugins";
const QString Layout::ROW = "row";
const QString Layout::CLASS_NAME = "Layout";
const QString Layout::TYPE_IMAGE = "Image";
const QString Layout::TYPE_ANALYSIS = "Analysis";
const QString Layout::TYPE_CUSTOM = "Custom";
const QString Layout::TYPE_SELECTED = "layoutType";


bool Layout::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME,
                                                   new Layout::Factory());

typedef Carta::State::UtilState UtilState;

Layout::Layout( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
    _initializeCommands();
}

QString Layout::addWindow( int rowIndex, int colIndex ){
    int oldRowCount = m_state.getValue<int>( LAYOUT_ROWS );
    int oldColCount = m_state.getValue<int>( LAYOUT_COLS );
    //Adding a row to an existing column.
    QString result;
    if ( 0 <= colIndex && colIndex < oldColCount ){
        m_state.setValue<QString>(TYPE_SELECTED,TYPE_CUSTOM);
        //Adding a window to an existing row.
        if ( 0 <= rowIndex && rowIndex < oldRowCount ){
            //See if there is an unused row somewhere in the column; find the closest one.
            int hiddenRow = _findEmptyRow( colIndex, rowIndex );
            //Add in a new window at the hidden row
            if ( hiddenRow >= rowIndex ){
                bool pluginSet =_setPlugin( hiddenRow, colIndex, EMPTY );
                if ( pluginSet ){
                    m_state.flushState();
                }

            }
            //Move existing plugins up to make space at the given index.
            else if ( 0 <= hiddenRow && hiddenRow < rowIndex ){
                //Move existing plugins down until we get to row index.
                for ( int i = hiddenRow; i < rowIndex - 1; i++ ){
                    _moveCell( i+1, colIndex, i, colIndex );
                }
                //Insert the new value at row index
                bool pluginSet = _setPlugin( rowIndex, colIndex, EMPTY );
                if ( pluginSet ){
                    m_state.flushState();
                }
            }
            //There were no empty rows. Insert an additional row, and then move existing
            //plugins up to make space for it.
            else {
                int newRowCount = oldRowCount + 1;
                int newSize = newRowCount * oldColCount;
                m_state.setValue<int>( LAYOUT_ROWS, newRowCount );
                m_state.resizeArray ( LAYOUT_PLUGINS, newSize, Carta::State::StateInterface::PreserveAll );

                //Adjust cells in the affected column by moving ones below the
                //new cell down one and setting the new cell empty.
                for ( int i = newRowCount - 2; i>= 0; i-- ){
                    //Move cells in the specific column down one.
                    if ( i >= rowIndex ){
                        _moveCell( i, colIndex, i+1, colIndex);
                    }
                    //Fill in the new cell as empty.
                    if ( i == rowIndex ){
                        _setPlugin( rowIndex, colIndex, EMPTY );
                    }
                    //We are not changing the beginning rows.
                    else if ( i < rowIndex ){
                       break;
                    }
                }

                //Fill in the new row with empty windows for all columns
                //but the affected one.
                for ( int i = oldRowCount; i < newRowCount; i++ ){
                    for ( int j = 0; j < oldColCount; j++ ){
                        if ( j != colIndex ){
                            _setPlugin( i, j, EMPTY, true );
                         }
                     }
                }
                m_state.flushState();
            }
        }
    }
    else {
        result = "It is not possible to add a window at column:"+colIndex;
    }
    return result;
}

void Layout::clear(){
    int oldRows = m_state.getValue<int>( LAYOUT_ROWS );
    int oldCols = m_state.getValue<int>( LAYOUT_COLS );
    if ( oldRows != 0 || oldCols != 0 ){
        m_state.setValue<int>( LAYOUT_ROWS, 0 );
        m_state.setValue<int>( LAYOUT_COLS, 0 );
        m_state.resizeArray( LAYOUT_PLUGINS, 0 );
        m_state.flushState();
    }
}

QString Layout::getStateString() const {
    QString result = m_state.toString();
    return result;
}

int Layout::_findEmptyRow( int colIndex, int targetRowIndex ) const {
    int hiddenRow = -1;
    int diff = std::numeric_limits<int>::max();
    int rowCount = m_state.getValue<int>(LAYOUT_ROWS );
    for ( int i = 0; i < rowCount; i++ ){
        int index = _getArrayIndex(i, colIndex);
        QString lookup( UtilState::getLookup( LAYOUT_PLUGINS,index) );
        QString plugin = m_state.getValue<QString>(lookup);
        if ( plugin == HIDDEN ){
            int rowDistance = qAbs( i - targetRowIndex );
            if ( rowDistance < diff ){
                diff = rowDistance;
                hiddenRow = i;
            }
        }
    }
    return hiddenRow;
}


int Layout::_getArrayIndex( int rowIndex, int colIndex ) const {
    int colCount = m_state.getValue<int>( LAYOUT_COLS );
    int arrayIndex = rowIndex * colCount + colIndex;
    return arrayIndex;
}

QString Layout::_getPlugin( int rowIndex, int colIndex ) const{
    int arrayIndex = _getArrayIndex( rowIndex, colIndex );
    int rows = m_state.getValue<int>( LAYOUT_ROWS );
    int cols = m_state.getValue<int>( LAYOUT_COLS );
    QString plugin;
    if ( arrayIndex < rows*cols && arrayIndex >= 0 ){
        QString lookup( UtilState::getLookup( LAYOUT_PLUGINS, arrayIndex) );
        plugin = m_state.getValue<QString>( lookup );
    }
    return plugin;
}

int Layout::_getColumnCount( int colIndex ) const {
    int colCount = 0;
    if ( 0 <= colIndex ){
        int rowCount = m_state.getValue<int>( LAYOUT_ROWS );
        for ( int j = 0; j < rowCount; j++ ){
            QString pluginName = _getPlugin( j, colIndex );
            if ( pluginName != HIDDEN ){
                colCount++;
            }
        }
    }
    return colCount;
}

int Layout::_getMaxRowColumn() const {

    int colCount = m_state.getValue<int>( LAYOUT_COLS );
    int maxCount = 0;
    int maxColIndex = -1;
    for ( int i = 0; i < colCount; i++ ){
        int colCount = _getColumnCount( i );
        if ( colCount > maxCount ){
            maxCount = colCount;
            maxColIndex = i;
        }
    }
    return maxColIndex;
}

QStringList Layout::getPluginList() const {
    int rows = m_state.getValue<int>( LAYOUT_ROWS);
    int cols = m_state.getValue<int>( LAYOUT_COLS );
    int pluginCount = rows * cols;
    QStringList plugins;
    for ( int i = 0; i < pluginCount; i++ ){
        QString lookup( UtilState::getLookup( LAYOUT_PLUGINS, i) );
        plugins.push_back(m_state.getValue<QString>(lookup));
    }
    return plugins;
}

void Layout::_initializeCommands(){

    addCommandCallback( "setLayoutSize", [=] (const QString & /*cmd*/,
                   const QString & params, const QString & /*sessionId*/) -> QString {
           std::set<QString> keys = {LAYOUT_ROWS, LAYOUT_COLS};
           std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
           QString rowStr = dataValues[LAYOUT_ROWS];
           QString colStr = dataValues[LAYOUT_COLS];
           bool valid = false;
           int rows = rowStr.toInt( &valid );
           QString result;
           if ( valid ){
               int cols = colStr.toInt( &valid );
               if ( valid ){
                   //If we are going to remove plugins with this new layout we will need
                   //to remove them from the model.
                   result = setLayoutSize( rows, cols);
               }
               else {
                   result = "Invalid layout cols: "+params;
               }
           }
           else {
               result =  "Invalid layout rows: " +params;
           }
           return result;
       });

    addCommandCallback( "addWindow", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = { ROW, COLUMN };
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            bool validRow = false;
            QString result;
            int rowIndex = dataValues[ROW].toInt(&validRow);
            bool validCol = false;
            int colIndex = dataValues[COLUMN].toInt( &validCol );
            if ( validRow && validCol ){
                result = addWindow( rowIndex, colIndex );
            }
            return result;
        });

    addCommandCallback( "removeWindow", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
                std::set<QString> keys = { ROW, COLUMN };
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                bool validRow = false;
                QString result;
                int rowIndex = dataValues[ROW].toInt(&validRow);
                bool validCol = false;
                int colIndex = dataValues[COLUMN].toInt( &validCol );
                if ( validRow && validCol ){
                    result = removeWindow( rowIndex, colIndex );
                }
                return result;
            });
}



void Layout::_initializeDefaultState(){
    m_state.insertArray( LAYOUT_PLUGINS, 0 );
    m_state.insertValue<int>( LAYOUT_ROWS, 0 );
    m_state.insertValue<int>( LAYOUT_COLS, 0 );
    m_state.insertValue<QString>(TYPE_SELECTED, TYPE_CUSTOM );
}

bool Layout::isLayoutAnalysis() const {
    bool layoutAnalysis = false;
    if ( m_state.getValue<QString>(TYPE_SELECTED) == TYPE_ANALYSIS ){
        layoutAnalysis = true;
    }
    return layoutAnalysis;
}



bool Layout::isLayoutImage() const {
    bool layoutImage = false;
    if ( m_state.getValue<QString>(TYPE_SELECTED) == TYPE_IMAGE ){
        layoutImage = true;
    }
    return layoutImage;
}

void Layout::_moveCell( int sourceRow, int sourceCol, int destRow, int destCol ){
    int emptyIndex = _getArrayIndex( destRow, destCol );
    int existIndex = _getArrayIndex( sourceRow, sourceCol );
    QString emptyLookup( UtilState::getLookup(LAYOUT_PLUGINS,emptyIndex) );
    QString existLookup( UtilState::getLookup(LAYOUT_PLUGINS, existIndex) );
    QString existPlugin = m_state.getValue<QString>( existLookup );
    m_state.setValue<QString>(emptyLookup, existPlugin );
}

void Layout::resetState( const Carta::State::StateInterface& savedState ){
    QString layoutLookup = CLASS_NAME;
    int rows = savedState.getValue<int>(UtilState::getLookup( layoutLookup, LAYOUT_ROWS));
    int cols = savedState.getValue<int>(UtilState::getLookup( layoutLookup, LAYOUT_COLS ));
    QString typeStr = savedState.getValue<QString>(UtilState::getLookup( layoutLookup, TYPE_SELECTED));
    setLayoutSize(rows,cols, typeStr);
    //The view manager needs to update the plugins it supports.
    QString pluginArrayLookup = UtilState::getLookup( layoutLookup, LAYOUT_PLUGINS);
    int pluginCount = savedState.getArraySize( pluginArrayLookup );
    QStringList pluginList;
    for ( int i = 0; i < pluginCount; i++ ){
        QString arrayIndexStr = UtilState::getLookup(pluginArrayLookup, i);
        pluginList.append( savedState.getValue<QString>(arrayIndexStr) );
     }
    bool customLayout = false;
    if ( typeStr == TYPE_CUSTOM ){
        customLayout = true;
    }
    QStringList oldNames = getPluginList();
    _setPlugin( pluginList, customLayout );
    emit pluginListChanged( pluginList, oldNames );
}

QString Layout::removeWindow( int rowIndex, int colIndex ){
    QString result;
    int rowCount = m_state.getValue<int>(LAYOUT_ROWS);
    int colCount = m_state.getValue<int>(LAYOUT_COLS);
    if ( 0 <= rowIndex && rowIndex < rowCount &&
         0 <= colIndex && colIndex < colCount ){
        m_state.setValue<QString>(TYPE_SELECTED, TYPE_CUSTOM );
        int maxRowColumn = _getMaxRowColumn();
        //Decide if this is the column with the maximum number of rows.
        if ( maxRowColumn == colIndex ){
            //Move everything up one row so the last row is empty.
            for ( int i = rowIndex+1; i < rowCount; i++ ){
                for ( int j = 0; j < colCount; j++ ){
                    //Check to see if i-1 is excluded.
                    QString plugin = _getPlugin( i -1, j );
                    if ( plugin == HIDDEN || j == colIndex){
                        //Move row i -> i-1
                        _moveCell( i, j, i-1, j );
                        //Set row i empty for next pass.
                        _setPlugin( i, j, HIDDEN );
                    }
                }
            }
            //Resize the array with one fewer row.
            int newRowCount = rowCount - 1;
            m_state.setValue<int>(LAYOUT_ROWS, newRowCount);
            m_state.resizeArray( LAYOUT_PLUGINS, (newRowCount) * colCount, Carta::State::StateInterface::PreserveAll );
            m_state.flushState();
        }
        //Some other column has the maximum number of rows.
        else {
            int colCountIndex = _getColumnCount( colIndex );
            //Just exclude the widget as long as there is at least one other visible
            //widget in the column.
            if ( colCountIndex > 1 ){
                bool removed = _setPlugin( rowIndex, colIndex, HIDDEN );
                if ( removed ){
                    m_state.flushState();
                }
                else {
                    result = "Could not remove window, one or more invalid indices row="+QString::number(rowIndex)+" column="+QString::number(colIndex);
                }
            }
            //Remove the entire column after making sure the last column is empty after shifting
            //everything to the right of colIndex left.
            else {
                for ( int i = 0; i < rowCount; i++ ){
                    for ( int j = colIndex+1; j < colCount; j++ ){
                        //Move column j -> j-1
                        _moveCell( i, j, i, j-1 );
                    }
                }
                int newColCount = colCount - 1;
                m_state.setValue<int>(LAYOUT_COLS, newColCount);
                m_state.resizeArray( LAYOUT_PLUGINS, rowCount * newColCount, Carta::State::StateInterface::PreserveAll );
                m_state.flushState();
            }
        }
    }
    else {
        result = "Could not remove window, one or more invalid indices row="+ QString::number(rowIndex)+" column="+QString::number(colIndex);
    }
    Util::commandPostProcess( result);
    return result;
}

void Layout::setLayoutAnalysis(){
    setLayoutSize( 4, 2, TYPE_ANALYSIS );
    QStringList oldNames = getPluginList();
    QStringList names = {Controller::PLUGIN_NAME, Statistics::CLASS_NAME,
            HIDDEN, Animator::CLASS_NAME,
            HIDDEN, Colormap::CLASS_NAME,
            HIDDEN, Histogram::CLASS_NAME};
    _setPlugin( names );
    emit pluginListChanged( names, oldNames );
}

void Layout::setLayoutDeveloper(){
    setLayoutSize( 3, 2 );
    QStringList oldNames = getPluginList();
    QStringList names = {Controller::PLUGIN_NAME, Animator::CLASS_NAME,
            HIDDEN, Colormap::CLASS_NAME,
            HIDDEN, Histogram::CLASS_NAME};
    _setPlugin( names );
    emit pluginListChanged( names, oldNames );
}

void Layout::setLayoutImage(){
    setLayoutSize( 2,1, TYPE_IMAGE);
    QStringList oldNames = getPluginList();
    QStringList name = {Controller::PLUGIN_NAME, HIDDEN};
    _setPlugin( name );
    emit pluginListChanged( name, oldNames );
}


QString Layout::setLayoutSize( int rows, int cols, const QString& layoutType ){
    QString errorMsg;
    if ( rows >= 0 && cols >= 0 ){
        int oldRows = m_state.getValue<int>( LAYOUT_ROWS );
        int oldCols = m_state.getValue<int>( LAYOUT_COLS );
        if ( rows != oldRows || cols != oldCols ){
            QStringList oldNames = getPluginList();
            //If any of the windows are hidden, show them as empty.
            for ( int i = 0; i < oldRows; i++ ){
                for ( int j = 0; j < oldCols; j++ ){
                    QString pluginName = _getPlugin( i, j );
                    if ( pluginName == HIDDEN ){
                        _setPlugin( i, j, EMPTY );
                    }
                }
            }
            m_state.setValue<int>( LAYOUT_ROWS, rows );
            m_state.setValue<int>( LAYOUT_COLS, cols );
            m_state.setValue<QString>(TYPE_SELECTED, layoutType );

            m_state.resizeArray( LAYOUT_PLUGINS, rows * cols, Carta::State::StateInterface::PreserveAll );

            //Resize always puts things at the end so we set extra cells empty.
            int startIndex = oldRows * oldCols;
            int endIndex = rows * cols;
            for ( int i = startIndex; i < endIndex; i++ ){
                 QString lookup( UtilState::getLookup(LAYOUT_PLUGINS, i) );
                 m_state.setValue( lookup, EMPTY );
            }
            QStringList newNames = getPluginList();
            emit pluginListChanged( newNames, oldNames );
            m_state.flushState();
        }
    }
    else {
        errorMsg = "Invalid layout rows ="+QString::number(rows)+" and/or cols="+QString::number(cols);
    }
    Util::commandPostProcess( errorMsg);
    return errorMsg;
}

bool Layout::_setPlugin( int rowIndex, int colIndex, const QString& name, bool insert ){
    int arrayIndex = _getArrayIndex( rowIndex, colIndex );
    int rows = m_state.getValue<int>( LAYOUT_ROWS );
    int cols = m_state.getValue<int>( LAYOUT_COLS );
    bool pluginSet = false;
    if ( arrayIndex < rows*cols && arrayIndex >= 0 ){
        QString lookup( UtilState::getLookup(LAYOUT_PLUGINS,arrayIndex) );
        if ( !insert ){
            QString oldValue = m_state.getValue<QString>( lookup );
            if ( name != oldValue ){

                pluginSet = true;
            }
        }
        else {
            pluginSet = true;
        }
        if ( pluginSet ){
            m_state.setValue<QString>(lookup, name );
        }
    }
    return pluginSet;
}

bool Layout::_setPlugin( const QStringList& names, bool custom ){
    int rows = m_state.getValue<int>( LAYOUT_ROWS );
    int cols = m_state.getValue<int>( LAYOUT_COLS );
    bool valid = true;
    int existingSize = rows * cols;
    if ( names.size() == existingSize ){
        int nameCount = names.size();
        bool changed = false;
        for ( int i = 0; i < nameCount; i++ ){
            QString lookup( UtilState::getLookup(LAYOUT_PLUGINS,i) );
            QString oldValue = m_state.getValue<QString>(lookup);
            if ( oldValue != names[i]){
                m_state.setValue<QString>(lookup, names[i]);
                changed = true;
            }
        }
        if ( changed ){
            if ( custom ){
                m_state.setValue<QString>( TYPE_SELECTED, TYPE_CUSTOM );
            }
            m_state.flushState();
        }
    }
    else {
        valid = false;
    }
    return valid;
}

Layout::~Layout(){

}
}
}
