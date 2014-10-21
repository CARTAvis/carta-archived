#include "StateLibrary.h"
#include "Globals.h"
#include "IConnector.h"
#include <QDebug>

const QString StateLibrary::SEPARATOR = "/";
const QString StateLibrary::APP_ROOT = "carta";
const QString StateLibrary::ANIMATOR = "animator";
const QString StateLibrary::AUTO_CLIP = "autoClip";
const QString StateLibrary::CHANNEL = "channel";
const QString StateLibrary::CLIP_VALUE = "clipValue";
const QString StateLibrary::CONTROLLER = "controller";
const QString StateLibrary::DATA = "data";
const QString StateLibrary::DATA_COUNT = "dataCount";
const QString StateLibrary::ID = "id";
const QString StateLibrary::IMAGE = "image";
const QString StateLibrary::LAYOUT = "layout";
const QString StateLibrary::LAYOUT_ROWS = "rows";
const QString StateLibrary::LAYOUT_COLS = "cols";
const QString StateLibrary::LAYOUT_PLUGIN = "plugins";
const QString StateLibrary::LINK = "link";
const QString StateLibrary::LINK_COUNT = "linkCount";
const QString StateLibrary::MOUSE = "mouse";
const QString StateLibrary::PLUGIN_LIST = "pluginList";

StateLibrary* StateLibrary::library = nullptr;

StateLibrary* StateLibrary::instance() {
    if (library == nullptr) {
        library = new StateLibrary();
    }
    return library;
}

StateLibrary::StateLibrary() {
    _initializeStates();
}

void StateLibrary::_initializeStates() {
    states.insert(StateKey::ANIMATOR_CHANNEL_STEP,
            State(SEPARATOR + APP_ROOT + SEPARATOR + ANIMATOR + SEPARATOR + CHANNEL + SEPARATOR + "frameStep",
                    "Channel increment", true, false));
    states.insert(StateKey::ANIMATOR_CHANNEL_RATE,
            State(SEPARATOR + APP_ROOT + SEPARATOR + ANIMATOR + SEPARATOR + CHANNEL + SEPARATOR + "frameRate",
                    "Speed indicator for incrementing/decrementing channels in play or reverse play mode.", true, false));
    states.insert(StateKey::ANIMATOR_CHANNEL_END_BEHAVIOR,
            State(SEPARATOR + APP_ROOT + SEPARATOR + ANIMATOR + SEPARATOR + CHANNEL + SEPARATOR + "endBehavior",
                    "Defines the next index when at the beginning or end of the channel range.  Supported types are wrap, reverse, and jump.",true, false));
    states.insert(StateKey::ANIMATOR_IMAGE_STEP,
            State(SEPARATOR + APP_ROOT + SEPARATOR + ANIMATOR + SEPARATOR + IMAGE + SEPARATOR + "frameStep",
                    "Image increment", true, false));
    states.insert(StateKey::ANIMATOR_IMAGE_RATE,
            State(SEPARATOR + APP_ROOT + SEPARATOR + ANIMATOR + SEPARATOR + IMAGE + SEPARATOR + "frameRate",
                    "Speed indicator for incrementing/decrementing images in play or reverse play mode.", true, false));
    states.insert(StateKey::ANIMATOR_IMAGE_END_BEHAVIOR,
            State(SEPARATOR + APP_ROOT + SEPARATOR + ANIMATOR + SEPARATOR + IMAGE + SEPARATOR + "endBehavior",
                    "Defines the next index when at the beginning or end of the image range.  Supported types are wrap, reverse, and jump.", true, false));
    states.insert(StateKey::ANIMATOR_LINK,
            State(SEPARATOR + APP_ROOT + SEPARATOR + ANIMATOR + SEPARATOR + LINK, "Link to another window listening to this animator", true, false ));
    states.insert(StateKey::ANIMATOR_LINK_COUNT,
            State(SEPARATOR + APP_ROOT + SEPARATOR + ANIMATOR + SEPARATOR + LINK_COUNT, "Count of windows listening to this animator", true, false ));
    states.insert(StateKey::AVAILABLE_DATA,
                State(SEPARATOR + APP_ROOT + SEPARATOR + DATA,
                        "Available data files.", false, false));
    states.insert(StateKey::DATA_PATH,
            State(SEPARATOR + APP_ROOT + SEPARATOR + CONTROLLER + SEPARATOR + DATA,
                    "Path to data source.", true, false));
    states.insert(StateKey::AUTO_CLIP,
            State(SEPARATOR + APP_ROOT + SEPARATOR + AUTO_CLIP,
                    "Auto clip images", true, false));
    states.insert(StateKey::CLIP_VALUE,
            State(SEPARATOR + APP_ROOT + SEPARATOR + CLIP_VALUE,
                    "Auto clip value", true, false));
    states.insert(StateKey::DATA_COUNT,
                State(SEPARATOR + APP_ROOT + SEPARATOR + CONTROLLER + SEPARATOR + DATA_COUNT,
                        "Path to data source.", true, false));
    states.insert(StateKey::FRAME_CHANNEL,
            State(SEPARATOR + APP_ROOT + SEPARATOR + ANIMATOR + SEPARATOR + CHANNEL + SEPARATOR + "frame", "Channel index", false, false));
    states.insert(StateKey::FRAME_CHANNEL_END,
            State(SEPARATOR + APP_ROOT + SEPARATOR + ANIMATOR + SEPARATOR + CHANNEL + SEPARATOR + "frameEnd",
                    "Ending channel index", false, false));
    states.insert(StateKey::FRAME_CHANNEL_START,
            State(SEPARATOR + APP_ROOT + SEPARATOR + ANIMATOR + SEPARATOR + CHANNEL + SEPARATOR + "frameStart",
                    "Beginning channel index", false, false));
    states.insert(StateKey::FRAME_IMAGE,
            State(SEPARATOR + APP_ROOT + SEPARATOR + ANIMATOR + SEPARATOR + IMAGE + SEPARATOR + "frame", "Image index", false, false));
    states.insert(StateKey::FRAME_IMAGE_END,
            State(SEPARATOR + APP_ROOT + SEPARATOR + ANIMATOR + SEPARATOR + IMAGE + SEPARATOR + "frameEnd",
                    "Ending image index", false, false));
    states.insert(StateKey::FRAME_IMAGE_START,
            State(SEPARATOR + APP_ROOT + SEPARATOR + ANIMATOR + SEPARATOR + IMAGE + SEPARATOR + "frameStart",
                    "Beginning image index", false, false));
    states.insert(StateKey::LAYOUT_ROWS,
            State(SEPARATOR + APP_ROOT + SEPARATOR + LAYOUT + SEPARATOR +LAYOUT_ROWS,
                    "Grid row count", true, false));
    states.insert(StateKey::LAYOUT_COLS,
            State(SEPARATOR + APP_ROOT + SEPARATOR + LAYOUT + SEPARATOR +LAYOUT_COLS,
                    "Grid column count", true, false));
    states.insert(StateKey::LAYOUT_PLUGIN,
            State(SEPARATOR + APP_ROOT + SEPARATOR + LAYOUT + SEPARATOR + LAYOUT_PLUGIN,
                    "Plugin Identifier", true, false));
    states.insert(StateKey::MOUSE_X,
            State(SEPARATOR + APP_ROOT + SEPARATOR + MOUSE + SEPARATOR + "x",
                    "X-coordinate of the mouse cursor.", false, false));
    states.insert(StateKey::MOUSE_Y,
            State(SEPARATOR + APP_ROOT + SEPARATOR + MOUSE + SEPARATOR + "y",
                    "Y-coordinate of the mouse cursor.", false, false));
    states.insert(StateKey::PLUGIN_NAME,
            State(SEPARATOR + APP_ROOT + SEPARATOR + PLUGIN_LIST + SEPARATOR + "name", "Plugin name.", false, false));
    states.insert(StateKey::PLUGIN_DESCRIPTION,
            State(SEPARATOR + APP_ROOT + SEPARATOR + PLUGIN_LIST + SEPARATOR + "description", "Plugin description", false, false));
    states.insert(StateKey::PLUGIN_TYPE,
            State(SEPARATOR + APP_ROOT + SEPARATOR + PLUGIN_LIST + SEPARATOR + "type", "Type of plugin", false, false));
    states.insert(StateKey::PLUGIN_VERSION,
            State(SEPARATOR + APP_ROOT + SEPARATOR + PLUGIN_LIST + SEPARATOR + "version", "Plugin version", false, false));
    states.insert(StateKey::PLUGIN_ERRORS,
            State(SEPARATOR + APP_ROOT + SEPARATOR + PLUGIN_LIST + SEPARATOR + "loadErrors", "Plugin load errors", false, false));
    states.insert(StateKey::PLUGIN_STAMP,
            State(SEPARATOR + APP_ROOT + SEPARATOR + PLUGIN_LIST + SEPARATOR + "stamp", "Number of plugins loaded", false, false));

    states.insert(StateKey::CURSOR, State("/cursor", "formatted mouse cursor", false, false));

    states.insert(StateKey::HACKS, State("/hacks", "various hacks for which i'm too laze to edit this file", false, false));

}

QString StateLibrary::getPath(StateKey stateKey, const QString& id) const {
    State stateValue = states[stateKey];
    QString path(stateValue.getPath());
    if (id.trimmed().length() > 0) {
        path.append(SEPARATOR + id.trimmed());
    }
    return path;
}

StateKey StateLibrary::findKey(const QString& path) const {
    QString basePath = _getLookupPath( path );
    StateKey pathKey = StateKey::END_KEY;
    for (StateKey key : states.keys()) {
        if (states[key].getPath() == basePath ) {
            pathKey = key;
            break;
        }
    }
    return pathKey;
}

QString StateLibrary::_getLookupPath( const QString& path ) const {
    QString basePath = path;
    //Remove index identifiers from the path.
    if ( path.at( path.length() - 1).isDigit()){
        int lastSeparationIndex = path.lastIndexOf(SEPARATOR);
        if (lastSeparationIndex >= 0) {
            basePath = path.left(lastSeparationIndex);
        }
    }
    return basePath;
}

bool StateLibrary::isPersistent(const QString& path) const {
    bool persistentState = false;
    QString basePath = _getLookupPath( path );
    QList<State> stateValues = states.values();
    State lookup(basePath, "", false, true);
    int lookupIndex = stateValues.indexOf(lookup);
    if (lookupIndex >= 0) {
        persistentState = stateValues[lookupIndex].isPersistent();
    }
    return persistentState;
}



