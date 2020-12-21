#pragma once

#include "ui_AoRGhostManager.h"

#include <QtWidgets/QMainWindow>

class AorGhostManager : QMainWindow
{
	Q_OBJECT
	
public:
	AorGhostManager(QWidget *parent = Q_NULLPTR);

private:
	Ui::AorGhostManager ui;
};