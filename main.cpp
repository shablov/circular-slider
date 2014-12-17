#include "circularsliderdialog.h"
#include <QApplication>

#include <QDebug>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CircularSliderDialog w;
	w.setRange(-20, 300);
	w.setSuffix(QChar(0x00B0));
	w.addSector(qMakePair(20.0, 30.0));
	if (w.exec() == QDialog::Accepted)
	{
		qDebug() << w.value();
	}

	return 0;
}
