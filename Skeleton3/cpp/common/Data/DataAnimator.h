/***
 * Coordinates selections on one or more data controllers.
 *
 */

#pragma once

#include <memory>
#include <QObject>
#include <State/StateKey.h>

class DataController;
class DataSelection;

class DataAnimator : public QObject {

	Q_OBJECT

public:

    /**
     * Constructor.
     * @param winId an identifier for the animator.
     */
    DataAnimator( const QString& winId );

    /**
     * Adds a DataController to this animator.
     * @param controller the DataController that will be managed.
     */
    void addController( std::shared_ptr<DataController> controller );

private slots:
	//Adjusts internal state based on the state in the child controllers.
	void _adjustStates();

private:
	//Initialize state specific to an animator.
	void _initializeStates();

	//Add callbacks for commands.
	void _initializeCommands();

	//Readjusts the lower and upper bound for the selection based the corresponding selections
	//of the child DataControllers.
	void _adjustState(StateKey lowKey, StateKey highKey, std::shared_ptr<DataSelection>& selection );

	//Identifier for this animator.
	QString m_id;

	//List of controllers managed by this animator.
	QList<std::shared_ptr<DataController> > m_controllers;

	//Animator's channel selection.
	std::shared_ptr<DataSelection> m_selectChannel;
	//Animator's image selection.
	std::shared_ptr<DataSelection> m_selectImage;

	DataAnimator( const DataAnimator& other);
	DataAnimator operator=( const DataAnimator& other );
};
