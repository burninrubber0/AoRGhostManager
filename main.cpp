#include "Ui/AoRGhostManager.h"

#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	AoRGhostManager* w = new AoRGhostManager();
	w->show();
	return a.exec();
}