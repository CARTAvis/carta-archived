#include "ScriptTester.h"
#include "ScriptFacade.h"

#include <QDebug>

ScriptTester::ScriptTester(){
    m_scriptFacade = ScriptFacade::getInstance();
}

void ScriptTester::runTest(){
    //Please comment out these calls as appropriate.
    //_runSingleImage();
    _runCustomImage();
    //_runAnalysisImage();
}

void ScriptTester::_runAnalysisImage() {
    //Set the layout
    m_scriptFacade->setAnalysisLayout();

    //Set the image
    //Please note that the path to the image is munged where the real path is /scratch/Images/Orion....
    //The path will need to be changed appropriately to refer to an actual image on the machine where
    //this test is being run.
    QString controlId = m_scriptFacade->getImageViewId( 0 );
    m_scriptFacade->loadFile( controlId, "/RootDirectory/Orion.methanol.cbc.contsub.image.fits" );

    //Don't have to do any linking of the color map to the image because it is set up by default.
    //However we need the color map object if we are going to change the color.  We use an index
    //of 0 to obtain an existing color map.
    QString colormapId = m_scriptFacade->getColorMapId( 0 );

    //Set the color map
    QStringList colormaps = m_scriptFacade->getColorMaps();
    int mapCount = colormaps.size();
    qDebug() << "Script tester found "<< mapCount << " colormaps";
    if ( mapCount > 1 ){
        QString mapName = colormaps[1];
        qDebug() << "Script tester setting map to "<<mapName;
        m_scriptFacade->setColorMap( colormapId, mapName);
    }

    m_scriptFacade->setChannel( 5 );
    m_scriptFacade->showImageAnimator();

    //Save the image
    qDebug() << "Save image not implemented";
}

void ScriptTester::_runCustomImage() {
    //Set the layout
    m_scriptFacade->setCustomLayout( 2, 2 );

    QString controlId = m_scriptFacade->getImageViewId( 0 );
    m_scriptFacade->loadFile( controlId, "/RootDirectory/m31_cropped.fits" );

    //Save the image
    qDebug() << "Save image not implemented";
}

void ScriptTester::_runSingleImage(){
    //Set the layout
    m_scriptFacade->setImageLayout();

    //Set the image
    //Please note that the path to the image is munged where the real path is /scratch/Images/Orion....
    //The path will need to be changed appropriately to refer to an actual image on the machine where
    //this test is being run.
    QString controlId = m_scriptFacade->getImageViewId( 0 );
    m_scriptFacade->loadFile( controlId, "/RootDirectory/m31_cropped.fits" );

    //Create a colormap, even though there is not one being displayed.  Note that here we are using a
    //default color map index of -1 to indicate that the server should create a color map for us since
    //the layout does not automatically create one.
    QString colormapId = m_scriptFacade->getColorMapId();

    //We also have to do more work here to link the color map to the object displaying the image.
    m_scriptFacade->linkAdd( colormapId, controlId );

    //Get a list of the color maps available on the server and set one.
    QStringList colormaps = m_scriptFacade->getColorMaps();
    int mapCount = colormaps.size();
    if ( mapCount > 0 ){
        QString mapName = colormaps[mapCount-1];
        m_scriptFacade->setColorMap( colormapId, mapName);
    }

    qDebug() << "Save image not implemented";
}





