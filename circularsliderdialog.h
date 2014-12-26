#ifndef CIRCULARSLIDERDIALOG_H
#define CIRCULARSLIDERDIALOG_H

#include <QDialog>

#include <QPen>

class QGraphicsView;
class QGraphicsScene;
class QGraphicsLineItem;
class QGraphicsTextItem;
class QGraphicsSceneMouseEvent;
class QDoubleSpinBox;

class CircularSliderDialog : public QDialog
{
	Q_OBJECT
public:
	CircularSliderDialog(QWidget *parent = 0);

	void setOffset(qreal offset);
	void setDirection(bool directionCW);
	void setScaleParameters(qreal offset, bool directionCW = true);

	void clearSectors();
	void addSector(const QPair<qreal, qreal> &sector);
	void addSector(const QPair<qint32, qint32> &sector);

	void setValue(const qreal &value);
	qreal value();

	void setSectorsBrush(const QBrush &brush);
	void setSectorsPen(const QPen &pen);
	void setBackgroundBrush(const QBrush &brush);
	void setCircleBrush(const QBrush &brush);
	void setScalePen(const QPen &pen);
	void setArrowPen(const QPen &pen);

	void setMinimum(const qreal &min);
	void setMaximum(const qreal &max);
	void setRange(const qreal &min, const qreal &max);
	void setPrefix(const QString &prefix);
	void setSuffix(const QString &suffix);

protected:
	void resizeEvent(QResizeEvent *event);
	bool eventFilter(QObject *obj, QEvent *event);

signals:
	void valueChanged(double);

private:
	void createGraphics();
	void createGraphicsScene();
	void createGraphicsView();
	void createControlWidgets();

	typedef QPair<qreal, qreal> Sector;
	void setDefaultSettings();

	void redraw();
	void clearScene();
	void calculateDrawRect();
	void drawScale();
	void drawSectors();

	void handleEvent(QEvent *event);
	bool leftButtonWasPressed();
	Qt::MouseButton buttonPressed;

	qreal calculateValue(const QPointF &point);
	void drawArrow(const qreal &value);
	bool valuePermitted(const qreal &value);
	bool valueInSector(const Sector &sector, const qreal &value);
	qreal nearestPermittedValue(const qreal &value);
	qreal calculateNearestValue(const Sector &sector, const qreal &value);

	QGraphicsView *view;
	QBrush backgroundBrush;
	QGraphicsScene *scene;

	QBrush circleBrush;
	QPen scalePen;
	QRectF drawRect;

	QPen arrowPen;
	QGraphicsLineItem *arrowItem;
	QGraphicsTextItem *arrowTextItem;

	QDoubleSpinBox *valueSpinBox;

	qreal range();
	qreal minimum;
	qreal maximum;

	QString mSuffix;
	QString mPrefix;

	/**
	 * @brief mCCW - направление шкалы: -1 по часовой, иначе 1
	 */
	int mCCW;

	qreal generalOffset();

	/**
	 * @brief mOffset - смещение шкалы
	 */
	qreal mOffset;

	QBrush sectorsBrush;
	QPen sectorsPen;
	QList<Sector> sectors;
	bool sectorIsValid(const Sector &sector);

	qreal angleFromValue(const qreal &value);
	qreal valueFromAngle(const qreal &angle);
	qreal mValue;

	/**
	 * @brief padding - учитывает отступы и длину, высоту текста надписи
	 */
	const qreal padding;
	/**
	 * @brief scalePadding - высота риски координатной шкалы
	 */
	const qreal scalePadding;
	/**
	 * @brief standartOffset - стандартный поворот координатной шкалы (0° вверху)
	 */
	const qreal standardOffset;
	/**
	 * @brief scaleStep - шаг координатной шкалы
	 */
	const qreal scaleCount;
private slots:
	void onValueChanged(double value);
};

#endif // CIRCULARSLIDERDIALOG_H
