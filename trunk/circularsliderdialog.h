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
	void setAngle(const qreal &angle);
	qreal angle();

	void setSectorsBrush(const QBrush &brush);
	void setSectorsPen(const QPen &pen);
	void setBackgroundBrush(const QBrush &brush);
	void setCircleBrush(const QBrush &brush);
	void setScalePen(const QPen &pen);
	void setArrowPen(const QPen &pen);

protected:
	void resizeEvent(QResizeEvent *event);
	bool eventFilter(QObject *obj, QEvent *event);

signals:
	void angleChanged(double);

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

	qreal calculateAngle(const QPointF &point);
	void drawArrow(const qreal &angle);
	bool anglePermitted(const qreal &angle);
	bool angleInSector(const Sector &sector, const qreal &angle);
	qreal nearestPermittedAngle(const qreal &angle);
	qreal calculateNearestAngle(const Sector &sector, const qreal &angle);

	QGraphicsView *view;
	QBrush backgroundBrush;
	QGraphicsScene *scene;

	QBrush circleBrush;
	QPen scalePen;
	QRectF drawRect;

	QPen arrowPen;
	QGraphicsLineItem *arrowItem;
	QGraphicsTextItem *arrowTextItem;

	QDoubleSpinBox *angleSpinBox;

	/**
	 * @brief mCCW - направление шкалы: -1 по часовой, иначе 1
	 */
	int mCCW;
	/**
	 * @brief mOffset - смещение шкалы
	 */
	qreal mOffset;

	QBrush sectorsBrush;
	QPen sectorsPen;
	QList<Sector> sectors;

	qreal mAngle;

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
	/**
	 * @brief degreeChar - знак градуса
	 */
	const QChar degreeChar;

private slots:
	void onValueChanged(double angle);
};

#endif // CIRCULARSLIDERDIALOG_H
