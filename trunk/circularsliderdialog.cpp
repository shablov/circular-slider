#include "circularsliderdialog.h"

#include "math.h"

#include <QDoubleSpinBox>
#include <QPushButton>

#include <QHBoxLayout>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>

#include <QGraphicsSceneMouseEvent>

CircularSliderDialog::CircularSliderDialog(QWidget *parent)
	: QDialog(parent), padding(40), scalePadding(5),
	  standardOffset(90), scaleCount(8)
{
	setMinimumSize(300, 300);
	setWindowTitle(tr("Bearing choice"));

	setDefaultSettings();

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	createGraphics();
	mainLayout->addWidget(view);
	createControlWidgets();
}


void CircularSliderDialog::setDefaultSettings()
{
	arrowPen = QPen(Qt::green, 2);
	arrowItem = 0;
	arrowTextItem = 0;

	sectorsBrush = QBrush(Qt::red);
	sectorsPen = QPen();

	backgroundBrush = QBrush(Qt::white);

	circleBrush = QBrush(Qt::white);
	scalePen = QPen(Qt::black, 2);

	mCCW = -1;
	mOffset = 0;
	mValue = 0;
	minimum = 0;
	maximum = 360;
}

void CircularSliderDialog::createGraphics()
{
	createGraphicsScene();
	createGraphicsView();
}

void CircularSliderDialog::createGraphicsScene()
{
	scene = new QGraphicsScene;
	scene->installEventFilter(this);
}

void CircularSliderDialog::createGraphicsView()
{
	view = new QGraphicsView;
	view->setRenderHint(QPainter::Antialiasing);
	view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view->setScene(scene);
}


void CircularSliderDialog::createControlWidgets()
{
	QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(layout());
	if (!mainLayout)
	{
		return;
	}
	valueSpinBox = new QDoubleSpinBox;
	valueSpinBox->setRange(minimum, maximum);
	valueSpinBox->setDecimals(1);
	valueSpinBox->setPrefix(mPrefix);
	valueSpinBox->setSuffix(mSuffix);
	connect(this, SIGNAL(valueChanged(double)), valueSpinBox, SLOT(setValue(double)));
	connect(valueSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
	QPushButton *acceptButton = new QPushButton(tr("Accept"));
	QPushButton *cancelButton = new QPushButton(tr("Cancel"));
	connect(acceptButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	QHBoxLayout *widgetsLayout = new QHBoxLayout;
	mainLayout->addLayout(widgetsLayout);

	widgetsLayout->addWidget(valueSpinBox, 1);
	widgetsLayout->addWidget(acceptButton);
	widgetsLayout->addWidget(cancelButton);
}

void CircularSliderDialog::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);
	redraw();
}


void CircularSliderDialog::redraw()
{
	clearScene();
	calculateDrawRect();
	drawScale();
	drawSectors();
	drawArrow(mValue);
}

void CircularSliderDialog::clearScene()
{
	scene->clear();
	scene->setSceneRect(view->rect());
	scene->setBackgroundBrush(backgroundBrush);
	arrowItem = 0;
	arrowTextItem = 0;
}

void CircularSliderDialog::calculateDrawRect()
{
	QRectF sceneRect = scene->sceneRect();
	qreal side = qMin(sceneRect.width(), sceneRect.height());
	side -= (padding + scalePadding)* 2;
	QPointF centerPoint = sceneRect.center();

	drawRect = QRectF();
	drawRect.setWidth(side);
	drawRect.setHeight(side);
	drawRect.moveCenter(centerPoint);
}

void CircularSliderDialog::drawScale()
{
	QPointF centerPoint = drawRect.center();
	QLineF scaleLine(centerPoint, QPointF(drawRect.right() + scalePadding, centerPoint.y()));
	QLineF textLine(scaleLine);
	textLine.setLength(textLine.length() + 20);

	scaleLine.setAngle(generatOffset());
	textLine.setAngle(generatOffset());
	qreal scaleStep = 360 / scaleCount * mCCW;
	qreal scaleTextStep = range() / scaleCount;
	for (int i = 0; i < scaleCount; i++)
	{
		scene->addLine(scaleLine, scalePen);
		QString scaleText = mPrefix + QString::number(minimum + scaleTextStep * i) + mSuffix;
		QGraphicsTextItem *textItem = scene->addText(scaleText);

		QRectF boundRect = textItem->boundingRect();
		boundRect.moveCenter(textLine.p2());
		textItem->setPos(boundRect.topLeft());

		scaleLine.setAngle(scaleLine.angle() + scaleStep);
		textLine.setAngle(textLine.angle() + scaleStep);
	}

	scene->addEllipse(drawRect, QPen(), circleBrush);
}

qreal CircularSliderDialog::generatOffset()
{
	return mOffset + standardOffset;
}

qreal CircularSliderDialog::range()
{
	return maximum - minimum;
}

void CircularSliderDialog::drawSectors()
{
	QListIterator<Sector> i(sectors);
	while(i.hasNext())
	{
		Sector sector = i.next();
		if (!sectorIsValid(sector))
		{
			sectors.removeAll(sector);
			continue;
		}
		QGraphicsEllipseItem *ellipse = scene->addEllipse(drawRect, sectorsPen, sectorsBrush);

		qreal firstAngle = angleFromValue(sector.first);
		qreal secondAngle = angleFromValue(sector.second);
		int startAngle = (firstAngle * mCCW + generatOffset()) * 16;
		int spanAngle = fmod(secondAngle - firstAngle + 360, 360) * 16 * mCCW;
		ellipse->setStartAngle(startAngle);
		ellipse->setSpanAngle(spanAngle);
	}
}

bool CircularSliderDialog::sectorIsValid(const Sector &sector)
{
	return (sector.first != sector.second) &&
			(minimum <= sector.first && sector.first <= maximum) &&
			(minimum <= sector.second && sector.second <= maximum);
}

qreal CircularSliderDialog::angleFromValue(const qreal &value)
{
	return ((value - minimum) * 360 / (range()));
}

void CircularSliderDialog::drawArrow(const qreal &value)
{
	if (!arrowItem)
	{
		arrowItem = scene->addLine(QLineF(), arrowPen);
	}
	if (!arrowTextItem)
	{
		arrowTextItem = scene->addText("");
	}

	QLineF arrow(drawRect.center(), QPointF(mCCW, 0));
	qreal angle  = angleFromValue(value);
	arrow.setAngle(mCCW * angle + generatOffset());
	arrow.setLength(drawRect.width() / 2);
	arrowItem->setLine(arrow);

	arrowTextItem->setPlainText(mPrefix + QString::number(value) + mSuffix);
	QRectF boundRect = arrowTextItem->boundingRect();
	boundRect.moveCenter(arrow.p2());
	arrowTextItem->setPos(boundRect.topLeft());
}

void CircularSliderDialog::setOffset(qreal offset)
{
	mOffset = offset;
	redraw();
}

void CircularSliderDialog::setDirection(bool directionCW)
{
	mCCW = directionCW ? -1 : 1;
	mOffset *= mCCW;
	redraw();
}

void CircularSliderDialog::setScaleParameters(qreal offset, bool directionCW)
{
	mCCW = directionCW ? -1 : 1;
	mOffset = offset;
	mOffset *= mCCW;
	redraw();
}

void CircularSliderDialog::clearSectors()
{
	sectors.clear();
	redraw();
}

void CircularSliderDialog::addSector(const QPair<qreal, qreal> &sector)
{
	if (!sectorIsValid(sector))
	{
		return;
	}
	sectors.append(sector);
	redraw();
}

void CircularSliderDialog::addSector(const QPair<qint32, qint32> &sector)
{
	Sector newSector = qMakePair<qreal, qreal>(sector.first, sector.second);
	if (!sectorIsValid(newSector))
	{
		return;
	}
	sectors.append(newSector);
	redraw();
}

qreal CircularSliderDialog::value()
{
	return mValue;
}

void CircularSliderDialog::setSectorsBrush(const QBrush &brush)
{
	sectorsBrush = brush;
	redraw();
}

void CircularSliderDialog::setSectorsPen(const QPen &pen)
{
	sectorsPen = pen;
	redraw();
}

void CircularSliderDialog::setBackgroundBrush(const QBrush &brush)
{
	backgroundBrush = brush;
	redraw();
}

void CircularSliderDialog::setCircleBrush(const QBrush &brush)
{
	circleBrush = brush;
	redraw();
}

void CircularSliderDialog::setScalePen(const QPen &pen)
{
	scalePen = pen;
	redraw();
}

void CircularSliderDialog::setArrowPen(const QPen &pen)
{
	arrowPen = pen;
	redraw();
}

void CircularSliderDialog::setMinimum(const qreal &min)
{
	minimum = min;
	valueSpinBox->setMinimum(min);
	setValue(mValue);
	redraw();
}

void CircularSliderDialog::setMaximum(const qreal &max)
{
	maximum = max;
	valueSpinBox->setMaximum(max);
	setValue(mValue);
	redraw();
}

void CircularSliderDialog::setRange(const qreal &min, const qreal &max)
{
	minimum = min;
	maximum = max;
	valueSpinBox->setRange(min, max);
	setValue(mValue);
	redraw();
}

void CircularSliderDialog::setPrefix(const QString &prefix)
{
	valueSpinBox->setPrefix(prefix);
	mPrefix = prefix;
	redraw();
}

void CircularSliderDialog::setSuffix(const QString &suffix)
{
	valueSpinBox->setSuffix(suffix);
	mSuffix = suffix;
	redraw();
}

bool CircularSliderDialog::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::GraphicsSceneMouseMove ||
			event->type() == QEvent::GraphicsSceneMousePress ||
			event->type() == QEvent::GraphicsSceneMouseRelease)
	{
		handleEvent(event);
	}
	return QWidget::eventFilter(obj, event);
}

void CircularSliderDialog::handleEvent(QEvent *event)
{
	QGraphicsSceneMouseEvent *mouseEvent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
	if (!mouseEvent)
	{
		return;
	}
	qreal value = calculateValue(mouseEvent->scenePos());
	switch (mouseEvent->type())
	{
		case QEvent::GraphicsSceneMouseMove:
		{
			if (!leftButtonWasPressed())
			{
				return;
			}
			drawArrow(nearestPermittedValue(value));
			return;
		}
		case QEvent::GraphicsSceneMousePress:
		{
			buttonPressed = (valuePermitted(value)) ? mouseEvent->button() : Qt::NoButton;
			return;
		}
		case QEvent::GraphicsSceneMouseRelease:
		{
			if (!leftButtonWasPressed())
			{
				return;
			}

			setValue(value);
			buttonPressed = Qt::NoButton;
			return;
		}
		default: return;
	}
}

qreal CircularSliderDialog::calculateValue(const QPointF &point)
{
	QLineF line(drawRect.center(), point);
	qreal angle = fmod(((line.angle() - generatOffset()) * mCCW + 720), 360);
	return valueFromAngle(angle);
}

qreal CircularSliderDialog::valueFromAngle(const qreal &angle)
{
	return angle / 360 * range() + minimum;
}

bool CircularSliderDialog::leftButtonWasPressed()
{
	return buttonPressed == Qt::LeftButton;
}

void CircularSliderDialog::setValue(const qreal &angle)
{
	mValue = ((angle < minimum) || (angle > maximum)) ? minimum : angle;
	mValue = nearestPermittedValue(mValue);
	drawArrow(mValue);
	emit valueChanged(mValue);
}

qreal CircularSliderDialog::nearestPermittedValue(const qreal &angle)
{
	QListIterator<Sector> i(sectors);
	while(i.hasNext())
	{
		Sector sector = i.next();

		if (valueInSector(sector, angle))
		{
			return calculateNearestValue(sector, angle);
		}
	}
	return angle;
}

bool CircularSliderDialog::valueInSector(const Sector &sector, const qreal &angle)
{
	return  !(((sector.first < sector.second) && (sector.second <= angle || angle <= sector.first)) ||
			  ((sector.first > sector.second) && (sector.second <= angle && angle <= sector.first)));
}

qreal CircularSliderDialog::calculateNearestValue(const Sector &sector, const qreal &value)
{
	if (sector.first < sector.second)
	{
		return (qAbs(sector.first - value) < qAbs(sector.second - value))
			? sector.first : sector.second;
	}
	return (qAbs(sector.first - value) < qAbs(fmod(sector.second + range() - value, range())))
		? sector.first : sector.second;
}

bool CircularSliderDialog::valuePermitted(const qreal &angle)
{
	QListIterator<Sector> i(sectors);
	while(i.hasNext())
	{
		Sector sector = i.next();
		if (valueInSector(sector, angle))
		{
			return false;
		}
	}
	return true;
}

void CircularSliderDialog::onValueChanged(double angle)
{
	if (valuePermitted(angle))
	{
		setValue(angle);
	}
}
