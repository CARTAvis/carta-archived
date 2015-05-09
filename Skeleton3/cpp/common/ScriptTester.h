/***
 * Provides an example of how the Python scripting interface might work.
 *
 */

#pragma once

class ScriptFacade;
class ScriptTester {

public:
    ScriptTester();
    virtual ~ScriptTester(){}

    /**
     * Runs one or more scripting samples.
     */
    void runTest();

private:
    /**
     * Sets a predefined analysis layout, loads an image, changes the colormap
     * and saves it.
     */
    void _runAnalysisImage();

    /**
     * Sets a predefined custom layout
     */
    void _runCustomImage();

    /**
     * Sets a predefined layout consisting of a single image, loads an image, changes
     * the colormap, and saves it.
     */
    void _runSingleImage();

    ScriptFacade* m_scriptFacade;
    ScriptTester( const ScriptTester& other);
    ScriptTester operator=( const ScriptTester& other );
};


