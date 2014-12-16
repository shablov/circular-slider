#include "circularsliderdialog.h"
#include <QApplication>

#include <QDebug>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CircularSliderDialog w;
	if (w.exec() == QDialog::Accepted)
	{
		qDebug() << w.angle();
	}

	return 0;
}
