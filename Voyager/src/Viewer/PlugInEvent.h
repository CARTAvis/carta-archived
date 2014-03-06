#ifndef PLUGINEVENT_H_
#define PLUGINEVENT_H_

/**
 * Class will be subclasses.  An example would be a TryLoadImageEvent.
 * Example:  PlugInEvent* ev = new TryLoadImageEvent( fileName );
 * core.pluginPropagateEvent( ev );
 */
class PlugInEvent {
	//So that we can later do casts across plug-in boundaries to the right event type.
	Q_OBJECT
public:
	PlugInEvent();
	virtual ~PlugInEvent();

protected:
	void setHandled( bool handled );
private:
	//Was the event handled;
	bool handled;
	//Each kind of event will have an id.
	int eventId;
};


class TryLoadImageEvent : public PlugInEvent {
	Q_OBJECT
public:
	TryLoadImageEvent( String fileName );
	virtual ~TryLoadImageEvent();

};


#endif /* PLUGINEVENT_H_ */
