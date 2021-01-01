#pragma once

#include "ui_AoRGhostManager.h"

#include <string>
#include <vector>

#include <QMainWindow>
#include <QStandardPaths>
#include <QTableWidget>

class AoRGhostManager : public QMainWindow
{
	Q_OBJECT
	
public:
	AoRGhostManager(QWidget *parent = Q_NULLPTR);

private:
	Ui::GhostManager ui;

	QString defaultGhostsPath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0)
		+ "/AppData/LocalLow/Funselektor Labs/art of rally/cloud";
	QString ghostsPath, newGhostsPath;
	std::string playerGhostsData;
	std::string newGhostsData;
	std::string playerGhostsDataBck;
	int numOpenBraces = 0;
	int numOpenBrackets = 0;

	// Ghost data
	std::vector<std::string> items;
	std::vector<std::string> maps;
	std::vector<std::string> classes;
	std::vector<std::string> cars;
	std::vector<std::string> times;

	// New ghost data
	std::vector<std::string> newItems;
	std::vector<std::string> newMaps;
	std::vector<std::string> newClasses;
	std::vector<std::string> newCars;
	std::vector<std::string> newTimes;

	// Backup data (original ghosts file)
	std::vector<std::string> bckItems;
	std::vector<std::string> bckMaps;
	std::vector<std::string> bckClasses;
	std::vector<std::string> bckCars;
	std::vector<std::string> bckTimes;

	// Search
	std::vector<bool> isNew; // Green
	std::vector<bool> isReplaced; // Red

	void connectActions();
	void loadTables(std::string g0, std::string g1);
	bool parseGhostData(std::string ghosts, bool isNew);
	void setUpTables();

private slots:
	bool openGhosts();
	void saveGhosts(QString path);
	void saveGhostsAs();
	void exitGm();
	void about();
	void transferLeft();
	void transferRight();
	void search(QLineEdit* line, QTableWidget* table);
};