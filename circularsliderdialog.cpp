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
	  standardOffset(90), scaleCount(8), degreeChar(0x00B0)
{
	setMinimumSize(300, 300);
	setDefaultSettings();

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	createGraphics();
	mainLayout->addWidget(view);
	/// TODO: debug information
	addSector(qMakePair(45.0, 135.0));
	addSector(qMakePair(225.0, 315.0));
	createControlWidgets();
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
	angleSpinBox = new QDoubleSpinBox;
	angleSpinBox->setRange(0, 359.9999);
	angleSpinBox->setDecimals(3);
	connect(this, SIGNAL(angleChanged(double)), angleSpinBox, SLOT(setValue(double)));
	connect(angleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
	QPushButton *acceptButton = new QPushButton(tr("Accept"));
	QPushButton *cancelButton = new QPushButton(tr("Cancel"));
	connect(acceptButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	QHBoxLayout *widgetsLayout = new QHBoxLayout;
	QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(layout());
	mainLayout->addLayout(widgetsLayout);

	widgetsLayout->addWidget(angleSpinBox, 1);
	widgetsLayout->addWidget(acceptButton);
	widgetsLayout->addWidget(cancelButton);
}

void CircularSliderDialog::setDefaultSettings()
{
	arrowPen = QPen(Qt::green, 2);
	arrowItem = 0;
	arrowTextItem = 0;

	valueTextItem = 0;

	sectorsBrush = QBrush(Qt::red);
	sectorsPen = QPen();

	backgroundBrush = QBrush(Qt::white);

	circleBrush = QBrush(Qt::white);
	scalePen = QPen(Qt::black, 2);

	mCCW = 1;
	mOffset = 0;
	mAngle = 0;
}

void CircularSliderDialog::setOffset(qreal offset)
{
	mOffset = offset;
	redraw();
}

void CircularSliderDialog::redraw()
{
	clearScene();
	calculateDrawRect();
	drawScale();
	drawSectors();

}

void CircularSliderDialog::clearScene()
{
	scene->clear();
	scene->setSceneRect(view->rect());
	scene->setBackgroundBrush(backgroundBrush);
	arrowItem = 0;
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

	scaleLine.setAngle(standardOffset + mOffset);
	textLine.setAngle(standardOffset + mOffset);
	qreal scaleStep = 360 / scaleCount * mCCW;
	for (int i = 0; i < scaleCount; i++)
	{
		scene->addLine(scaleLine, scalePen);
		QString scaleText = QString::number(qAbs(scaleStep * i)) + degreeChar;
		QGraphicsTextItem *textItem = scene->addText(scaleText);

		QRectF boundRect = textItem->boundingRect();
		boundRect.moveCenter(textLine.p2());
		textItem->setPos(boundRect.topLeft());

		scaleLine.setAngle(scaleLine.angle() + scaleStep);
		textLine.setAngle(textLine.angle() + scaleStep);
	}

	scene->addEllipse(drawRect, QPen(), circleBrush);
}

void CircularSliderDialog::drawSectors()
{
	QListIterator<Sector> i(sectors);
	while(i.hasNext())
	{
		Sector sector = i.next();
		QGraphicsEllipseItem *ellipse = scene->addEllipse(drawRect, sectorsPen, sectorsBrush);

		int startAngle = (sector.first * mCCW + mOffset + standardOffset) * 16;
		int spanAngle = fmod(sector.second - sector.first + 360, 360) * 16 * mCCW;
		ellipse->setStartAngle(startAngle);
		ellipse->setSpanAngle(spanAngle);
	}
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
	if (sector.first == sector.second)
	{
		return;
	}
	sectors.append(sector);
	redraw();
}

void CircularSliderDialog::setAngle(const qreal &angle)
{
	mAngle = angle;
	drawAngle();
	emit angleChanged(mAngle);
}

void CircularSliderDialog::drawAngle()
{
	drawArrow(mAngle);
	drawAngleValue();
}

qreal CircularSliderDialog::angle()
{
	return mAngle;
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

void CircularSliderDialog::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);
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
	qreal angle = calculateAngle(mouseEvent->scenePos());
	switch (mouseEvent->type())
	{
		case QEvent::GraphicsSceneMouseMove:
		{
			if (!leftButtonWasPressed())
			{
				return;
			}
			drawArrow(nearestPermittedAngle(angle));
			return;
		}
		case QEvent::GraphicsSceneMousePress:
		{
			buttonPressed = (anglePermitted(angle)) ? mouseEvent->button() : Qt::NoButton;
			return;
		}
		case QEvent::GraphicsSceneMouseRelease:
		{
			if (!leftButtonWasPressed())
			{
				return;
			}

			setAngle(nearestPermittedAngle(angle));
			buttonPressed = Qt::NoButton;
			return;
		}
		default: return;
	}
}

qreal CircularSliderDialog::calculateAngle(const QPointF &point)
{
	QLineF line(drawRect.center(), point);
	return fmod(((line.angle() - mOffset - standardOffset) * mCCW + 720), 360);
}

bool CircularSliderDialog::leftButtonWasPressed()
{
	return buttonPressed == Qt::LeftButton;
}

void CircularSliderDialog::drawArrow(const qreal &angle)
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
	arrow.setAngle(mCCW * angle + mOffset + standardOffset);
	arrow.setLength(drawRect.width() / 2);
	arrowItem->setLine(arrow);

	arrowTextItem->setPlainText(QString::number(angle) + degreeChar);
	QRectF boundRect = arrowTextItem->boundingRect();
	boundRect.moveCenter(arrow.p2());
	arrowTextItem->setPos(boundRect.topLeft());
}

bool CircularSliderDialog::anglePermitted(const qreal &angle)
{
	QListIterator<Sector> i(sectors);
	while(i.hasNext())
	{
		Sector sector = i.next();
		if (angleInSector(sector, angle))
		{
			return false;
		}
	}
	return true;
}

bool CircularSliderDialog::angleInSector(const Sector &sector, const qreal &angle)
{
	return  !(((sector.first < sector.second) && (sector.second <= angle || angle <= sector.first)) ||
			  ((sector.first > sector.second) && (sector.second <= angle && angle <= sector.first)));
}

qreal CircularSliderDialog::nearestPermittedAngle(const qreal &angle)
{
	QListIterator<Sector> i(sectors);
	while(i.hasNext())
	{
		Sector sector = i.next();

		if (angleInSector(sector, angle))
		{
			return calculateNearestAngle(sector, angle);
		}
	}
	return angle;
}

qreal CircularSliderDialog::calculateNearestAngle(const Sector &sector, const qreal &angle)
{
	return (qAbs(sector.first - angle) < qAbs(sector.second - angle)) ? sector.first : sector.second;
}

void CircularSliderDialog::drawAngleValue()
{
	if (!valueTextItem)
	{
		valueTextItem = scene->addText("");
		valueTextItem->setPos(drawRect.bottomRight());
	}
	valueTextItem->setPlainText(QString::number(mAngle) + degreeChar);
}

void CircularSliderDialog::onValueChanged(double angle)
{
	if (anglePermitted(angle))
	{
		setAngle(angle);
	}
}
