#ifndef NGMODEL_H_
#define NGMODEL_H_

/**
 * Purpose is to hold the internal state of the NGViewer.  This
 * state should be able to be set both from a scripted client and
 * a GUI client.
 */
class NGModel {
public:
	NGModel();
	void setSelectedRegion( const Region& );
	const CellLayout& getLayout() const;
	virtual ~NGModel();
private:


	//Stack layout, gridLayout (possibly make this flexible so that a plug-in could be
	//written for a fancier layout).  Spacial organization of stacks.  Does not represent
	//how many images we are showing.
	CellLayout m_layout;
};

class CellLayout {
private:
	QList<NGGridCell> gridCells;
};

class NGDrawableImage {
private:

	//First axes is the x-axis, second axis is the y-axis, other axes are animation axes.
	QList<int> m_axesPermutation;

	//Whatever is in axesPermutation 0,1 are ignored.  This corresponds to the viewer concept
	//of "channel", but also allows the ability to specify a Stokes plane.  Order is in the
	//original image axes order.
	QList<int> m_frameIndices;

	//Holds a color map and clipping of the image.
	PixelColorizer m_colorizer;

	//Zooming and panning of the image.
	ZoomPan m_zoomPan;

	//alpha transparency
	float m_transparency;

	//Actual data that is shown.
	shared_ptr<NGImageI* > > m_image;
};

class NGModelGridCellI {

};

/**
 * A cell in the layout.
 */
class NGModelGridCellData : public NGModelGridI {
public:
private:

	CoordinateSystem& m_coordinateSystem;

	//Line overlays for the cell.
	Grid m_cellGrid;
	//Holds images that have been loaded into this cell and
	//have been wrapped in a drawable interface.  Allows contour
	//images to be overlayed.
	QList<shared_ptr<NGDrawawable> > m_imageStack;
}

class NGModelGridCellAxis : public NGModelGridI {
public:
private:
	Coordinate& coordinate;
}


class NGModelGridCellColormap:  public NGModelGridI {
public:
private:
	Colormap* m_colorMap;
};

#endif /* NGMODEL_H_ */
