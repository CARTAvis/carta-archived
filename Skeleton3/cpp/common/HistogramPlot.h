#pragma once
#include <qwt_plot.h>
#include <qwt_samples.h>
#include <qwt_plot_histogram.h>
#include <QWidget>
#include <QRect>
#include <QRectF>
#include <QPainter>
#include <qwt_plot_renderer.h>
#include <QImage>
#include <QPaintDevice>
#include <QString>
#include <qwt_scale_engine.h>
#include <qwt_scale_map.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <QDebug>

class QPainter;

class HistogramPlot : public QwtPlotHistogram {
	public:
	HistogramPlot();
	virtual void drawSeries ( QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap,
		const QRectF& canvasRect, int from, int to ) const;
	void setSampleCount(int sampleNumber);
	virtual ~HistogramPlot();

	private:
	int m_sampleCount;
	// QString m_style;
};