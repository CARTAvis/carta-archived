/**
 *
 **/

#include "DesktopStateWriter.h"
#include "common/State/StateLibrary.h"
#include <QDebug>
#include <QFile>


DesktopStateWriter::DesktopStateWriter(const QString& filePath)
	:fileLocation( filePath ){
}

void DesktopStateWriter::addPathData( const QString& path, const QString& value ){
	qDebug() << "Writer::addPathData "<<path<<" value="<<value;
	QStringList pathParts = path.split( StateLibrary::SEPARATOR );
	if ( pathParts.size() > 1 ){
		if ( document.isNull()){
			QDomNode rootNode = document.createElement( pathParts[1] );
			document.appendChild( rootNode );
		}
		//The first path is blank because they start with a slash.
		pathParts.removeFirst();
		pathParts.removeFirst();
		QDomNode childOne = document.firstChild();
		addChildren( childOne, pathParts, value );
	}
}

bool DesktopStateWriter::saveState(){
	QFile file( fileLocation );
	bool fileSaved = true;
	if ( !file.open( QIODevice::WriteOnly) ){
		qDebug() << "DesktopStateWriter could not open the file for writing: "<<fileLocation;
		fileSaved = false;
	}
	else {
		QTextStream stream( &file );
		qDebug() << "DesktopStateWriter saved file at "<<fileLocation;
		document.save( stream, 4 );
	}
	return fileSaved;
}

void DesktopStateWriter::addChildren( QDomNode& node, QStringList& parts, const QString& value ){
	bool existingChild = false;
	if ( parts.size() > 0 ){
		//Check to see if the node already has a child with the given name.
		QDomNodeList children = node.childNodes();

		for ( int i = 0; i < children.count(); i++ ){
			QDomNode child = children.at( i );
			if ( child.nodeType() == QDomNode::ElementNode ){
				QDomElement childElement = child.toElement();
				if ( childElement.tagName() == parts[0] ){
					existingChild = true;
					parts.removeFirst();
					addChildren( child, parts, value );
					break;
				}
			}
		}
	}

	if ( !existingChild ){
		if ( parts.size() == 0 ){
			//Add a text node with the given value.
			QDomText characterNode = document.createTextNode( value );
			node.appendChild( characterNode );
		}
		else {
			//Add an element node
			QDomElement elementNode = document.createElement( parts[0]);
			node.appendChild( elementNode );
			parts.removeFirst();
			addChildren( elementNode, parts, value );
		}
	}
}
