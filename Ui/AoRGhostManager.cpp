#include "AoRGhostManager.h"

#include "ui_AoRGhostManager.h"

#include <fstream>
#include <string>

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTableWidget>

// ***** Slots *****
// Open ghost files
bool AoRGhostManager::openGhosts()
{
	std::ifstream playerGhosts, newGhosts;
	QString newGhostsPath;

	ghostsPath = QFileDialog::getOpenFileName(this, tr("Select your Ghosts.txt"), defaultGhostsPath, tr("Ghosts file (*.txt)"));
	newGhostsPath = QFileDialog::getOpenFileName(this, tr("Select new Ghosts.txt"), defaultGhostsPath, tr("Ghosts file (*.txt)"));

	playerGhosts.open(ghostsPath.QString::toStdString(), std::ios::in);
	newGhosts.open(newGhostsPath.QString::toStdString(), std::ios::in);
	if (playerGhosts.fail() || newGhosts.fail())
	{
		QMessageBox::critical(this, "Error", "Failed to open ghosts file");
		return true;
	}
	std::getline(playerGhosts, playerGhostsData);
	std::getline(newGhosts, newGhostsData);
	playerGhosts.close();
	newGhosts.close();

	loadTables(playerGhostsData, newGhostsData);

	return false;
}

// Exit program
void AoRGhostManager::exitGm()
{
	QApplication::quit();
}

// Transfer item from right to left
void AoRGhostManager::transferLeft()
{
	// If a ghost is selected in the right table
	if (ui.ghostsRight->selectionModel()->hasSelection())
	{
		// Then for each selected row
		foreach(const QModelIndex &index, ui.ghostsRight->selectionModel()->selectedRows())
		{
			bool hasPushed = false;
			bool hasReplaced = false;

			// Move both the vector items and table items to the left table
			for (int i = 0; i < items.size() - 1; ++i)
			{
				QTableWidgetItem* itmMap = new QTableWidgetItem();
				QTableWidgetItem* itmClass = new QTableWidgetItem();
				QTableWidgetItem* itmCar = new QTableWidgetItem();
				QTableWidgetItem* itmTime = new QTableWidgetItem();
				itmMap->setText(ui.ghostsRight->item(index.row(), 0)->data(Qt::DisplayRole).toString());
				itmClass->setText(ui.ghostsRight->item(index.row(), 1)->data(Qt::DisplayRole).toString());
				itmCar->setText(ui.ghostsRight->item(index.row(), 2)->data(Qt::DisplayRole).toString());
				itmTime->setText(ui.ghostsRight->item(index.row(), 3)->data(Qt::DisplayRole).toString());

				// If ghost map/class matches existing, replace existing (red)
				if (itmMap->text() == ui.ghostsLeft->item(i, 0)->text()
					&& itmClass->text() == ui.ghostsLeft->item(i, 1)->text()
					&& hasPushed == false && hasReplaced == false)
				{
					items.at(i) = newItems.at(index.row());
					hasReplaced = true;
					itmMap->setBackground(Qt::red);
					itmClass->setBackground(Qt::red);
					itmCar->setBackground(Qt::red);
					itmTime->setBackground(Qt::red);
					ui.ghostsLeft->setItem(i, 0, itmMap);
					ui.ghostsLeft->setItem(i, 1, itmClass);
					ui.ghostsLeft->setItem(i, 2, itmCar);
					ui.ghostsLeft->setItem(i, 3, itmTime);
				}
				// Else add new entry (green)
				else if (itmMap->text() != ui.ghostsLeft->item(i, 0)->text()
					&& itmClass->text() != ui.ghostsLeft->item(i, 1)->text()
					&& hasPushed == false && hasReplaced == false)
				{
					items.push_back(newItems.at(index.row()));
					hasPushed = true;
					ui.ghostsLeft->setRowCount(items.size());
					itmMap->setBackground(Qt::green);
					itmClass->setBackground(Qt::green);
					itmCar->setBackground(Qt::green);
					itmTime->setBackground(Qt::green);
					ui.ghostsLeft->setItem(items.size() - 1, 0, itmMap);
					ui.ghostsLeft->setItem(items.size() - 1, 1, itmClass);
					ui.ghostsLeft->setItem(items.size() - 1, 2, itmCar);
					ui.ghostsLeft->setItem(items.size() - 1, 3, itmTime);
				}
			}
		}
	}
}

// This will be deleted soon
void AoRGhostManager::removeGhosts()
{
	if (ui.ghostsLeft->selectionModel()->hasSelection())
	{
		bool indexNeedsFixing = false;

		for (int i = items.size() - 1; i > 0; --i)
		{
			if (ui.ghostsLeft->item(i, 0)->isSelected())
			{
				items.erase(items.begin() + i);
				ui.ghostsLeft->removeRow(i);
			}
		}
	}
}

// Save ghosts to file
void AoRGhostManager::saveGhosts(QString path)
{
	// Save backup ghosts file
	std::ofstream backupFile;
	QString backupPath = defaultGhostsPath + "/Ghosts.original.txt";
	backupFile.open(QString(backupPath).toStdString(), std::ios::out);
	backupFile << playerGhostsDataBck;
	backupFile.close();

	// Overwrite original ghosts file with altered data
	std::ofstream ghostsFile;
	ghostsFile.open(QString(path).toStdString(), std::ios::out);
	ghostsFile << "{\"Items\":[";
	for (int i = 0; i < items.size(); ++i)
	{
		ghostsFile << items.at(i) << '}';
		if (i != items.size() - 1)
			ghostsFile << ',';
	}
	ghostsFile << "]}";
	ghostsFile.close();
}

// Save ghosts to a different location than previously specified
void AoRGhostManager::saveGhostsAs()
{
	QString customGhostsPath = QFileDialog::getSaveFileName(this, tr("Save File"), ghostsPath, "");
	saveGhosts(customGhostsPath);
}

// Bring up the about window
void AoRGhostManager::about()
{
	QMessageBox::about(this, "About", "<p align='center'>Art of Rally Ghost Manager<br>by burninrubber0");
}

// ***** UI *****
// Connect signals to slots
void AoRGhostManager::connectActions()
{
	connect(ui.actionOpen, &QAction::triggered, this, &AoRGhostManager::openGhosts);
	connect(ui.actionSave, &QAction::triggered, this, [this] { AoRGhostManager::saveGhosts(ghostsPath); });
	connect(ui.actionSaveAs, &QAction::triggered, this, &AoRGhostManager::saveGhostsAs);
	connect(ui.actionExit, &QAction::triggered, this, &AoRGhostManager::exitGm);
	connect(ui.btnTransferLeft, &QPushButton::clicked, this, &AoRGhostManager::transferLeft);
	connect(ui.btnRemoveGhosts, &QPushButton::clicked, this, &AoRGhostManager::removeGhosts);
	connect(ui.actionAbout, &QAction::triggered, this, &AoRGhostManager::about);
}

// Load ghost data into tables
void AoRGhostManager::loadTables(std::string g0, std::string g1)
{
	// Back up original data before anything else
	playerGhostsDataBck = playerGhostsData;
	newGhostsDataBck = newGhostsData;

	// If no error was encountered
	if (parseGhostData(g0, false) == false && parseGhostData(g1, true) == false)
	{
		setUpTables();

		// Add data from vectors to player table
		for (int i = 0; i < items.size(); ++i)
		{
			QTableWidgetItem* itm0 = new QTableWidgetItem;
			QTableWidgetItem* itm1 = new QTableWidgetItem;
			QTableWidgetItem* itm2 = new QTableWidgetItem;
			QTableWidgetItem* itm3 = new QTableWidgetItem;
			itm0->setText(QString::fromStdString(maps.at(i)));
			itm1->setText(QString::fromStdString(classes.at(i)));
			itm2->setText(QString::fromStdString(cars.at(i)));
			itm3->setText(QString::fromStdString(times.at(i)));
			ui.ghostsLeft->setItem(i, 0, itm0);
			ui.ghostsLeft->setItem(i, 1, itm1);
			ui.ghostsLeft->setItem(i, 2, itm2);
			ui.ghostsLeft->setItem(i, 3, itm3);
		}

		// Add data from new vectors to new table
		for (int i = 0; i < newItems.size(); ++i)
		{
			QTableWidgetItem* itm0 = new QTableWidgetItem;
			QTableWidgetItem* itm1 = new QTableWidgetItem;
			QTableWidgetItem* itm2 = new QTableWidgetItem;
			QTableWidgetItem* itm3 = new QTableWidgetItem;
			itm0->setText(QString::fromStdString(newMaps.at(i)));
			itm1->setText(QString::fromStdString(newClasses.at(i)));
			itm2->setText(QString::fromStdString(newCars.at(i)));
			itm3->setText(QString::fromStdString(newTimes.at(i)));
			ui.ghostsRight->setItem(i, 0, itm0);
			ui.ghostsRight->setItem(i, 1, itm1);
			ui.ghostsRight->setItem(i, 2, itm2);
			ui.ghostsRight->setItem(i, 3, itm3);
		}

		// Enable save actions after the loops have completed
		ui.actionSave->setEnabled(true);
		ui.actionSaveAs->setEnabled(true);
	}
}

// Read ghost data into vectors
bool AoRGhostManager::parseGhostData(std::string ghosts, bool isNew)
{
	std::string start;
	std::string tmpItem;

	// Save each item
	for (int i = 0; i < ghosts.length(); ++i)
	{
		// Check that start of file is correct
		if (i < 11)
			start += ghosts[i];
		if (i == 10)
		{
			if (start != "{\"Items\":[{")
			{
				QMessageBox::critical(this, "Error parsing ghosts file", "Invalid ghosts data");
				return true;
			}
		}

		// Track braces to determine item start/end
		if (ghosts[i] == '{')
			numOpenBraces += 1;
		else if (ghosts[i] == '}')
			numOpenBraces -= 1;
		else if (ghosts[i] == '[')
			numOpenBrackets += 1;
		else if (ghosts[i] == ']')
			numOpenBrackets -= 1;

		// Store the current item
		if (numOpenBraces > 1)
			tmpItem += ghosts[i];

		// Save the current item when it ends
		if (numOpenBraces == 1 && numOpenBrackets == 1 && ghosts[i] == ',')
		{
			if (isNew == false)
				items.push_back(tmpItem);
			else
				newItems.push_back(tmpItem);
			tmpItem.clear();
		}

		// Save the last item
		if (i != 0 && numOpenBraces == 0 && numOpenBrackets == 0)
		{
			if (isNew == false)
				items.push_back(tmpItem);
			else
				newItems.push_back(tmpItem);
		}
	}

	// Check if all brackets and braces are closed at end
	if (numOpenBraces != 0 || numOpenBrackets != 0)
	{
		QString text = "Unclosed braces: " + QString::number(numOpenBraces)
			+ ", unclosed brackets: " + QString::number(numOpenBrackets);
		QMessageBox::critical(this, "Error parsing ghosts file", text);
		return true;
	}

	if (isNew == false)
	{
		// Pull the necessary info from each item
		for (int i = 0; i < items.size(); ++i)
		{
			bool recordingValue = false;
			int numQuotes = 0;
			std::string tmpString;
			std::string tmpFieldName;

			for (int j = 0; j < items.at(i).length(); ++j)
			{
				// If current char is string data
				if (numQuotes == 1 && items.at(i)[j] != '\"') // working
				{
					tmpString += items.at(i)[j];
					continue;
				}

				// If current char is start/end of string
				else if (items.at(i)[j] == '\"')
				{
					// Start of string
					if (numQuotes == 0)
						numQuotes += 1;
					// End of string
					else if (numQuotes == 1)
					{
						numQuotes = 0;
						// Save the names of fields that'll be used later
						if (tmpString == "_map" || tmpString == "_class" || tmpString == "_car" || tmpString == "_finishTime")
							tmpFieldName = tmpString;
						// Save field values to vectors
						else if (recordingValue == true)
						{
							if (tmpFieldName == "_map")
								maps.push_back(tmpString);
							else if (tmpFieldName == "_class")
								classes.push_back(tmpString);
							else if (tmpFieldName == "_car")
								cars.push_back(tmpString);

							tmpFieldName.clear();
							recordingValue = false;
						}
						tmpString.clear();
					}
				}

				// If a value to be recorded is here
				else if (items.at(i)[j] == ':' && (tmpFieldName == "_map" || tmpFieldName == "_class" || tmpFieldName == "_car"
					|| tmpFieldName == "_finishTime"))
					recordingValue = true;

				// Counting braces
				else if (items.at(i)[j] == '{')
					numOpenBraces += 1;
				else if (items.at(i)[j] == '}')
					numOpenBraces -= 1;

				// If item complete
				else if (items.at(i)[j] == ',' && numOpenBraces == 0)
					break;

				// If current char is being recorded but isn't string data
				if (numQuotes == 0 && recordingValue == true)
				{
					if (items.at(i)[j] == ',')
					{
						if (tmpFieldName == "_finishTime")
							times.push_back(tmpString);

						tmpFieldName.clear();
						tmpString.clear();
						recordingValue = false;
					}
					else if (items.at(i)[j] == ':')
						continue;
					else
						tmpString += items.at(i)[j];
				}
			}
		}
	}
	else if (isNew == true)
	{
		// Pull the necessary info from each item
		for (int i = 0; i < newItems.size(); ++i)
		{
			bool recordingValue = false;
			int numQuotes = 0;
			std::string tmpString;
			std::string tmpFieldName;

			for (int j = 0; j < newItems.at(i).length(); ++j)
			{
				// If current char is string data
				if (numQuotes == 1 && newItems.at(i)[j] != '\"') // working
				{
					tmpString += newItems.at(i)[j];
					continue;
				}

				// If current char is start/end of string
				else if (newItems.at(i)[j] == '\"')
				{
					// Start of string
					if (numQuotes == 0)
						numQuotes += 1;
					// End of string
					else if (numQuotes == 1)
					{
						numQuotes = 0;
						// Save the names of fields that'll be used later
						if (tmpString == "_map" || tmpString == "_class" || tmpString == "_car" || tmpString == "_finishTime")
							tmpFieldName = tmpString;
						// Save field values to vectors
						else if (recordingValue == true)
						{
							if (tmpFieldName == "_map")
								newMaps.push_back(tmpString);
							else if (tmpFieldName == "_class")
								newClasses.push_back(tmpString);
							else if (tmpFieldName == "_car")
								newCars.push_back(tmpString);

							tmpFieldName.clear();
							recordingValue = false;
						}
						tmpString.clear();
					}
				}

				// If a value to be recorded is here
				else if (newItems.at(i)[j] == ':' && (tmpFieldName == "_map" || tmpFieldName == "_class" || tmpFieldName == "_car"
					|| tmpFieldName == "_finishTime"))
					recordingValue = true;

				// Counting braces
				else if (newItems.at(i)[j] == '{')
					numOpenBraces += 1;
				else if (newItems.at(i)[j] == '}')
					numOpenBraces -= 1;

				// If item complete
				else if (newItems.at(i)[j] == ',' && numOpenBraces == 0)
					break;

				// If current char is being recorded but isn't string data
				if (numQuotes == 0 && recordingValue == true)
				{
					if (newItems.at(i)[j] == ',')
					{
						if (tmpFieldName == "_finishTime")
							newTimes.push_back(tmpString);

						tmpFieldName.clear();
						tmpString.clear();
						recordingValue = false;
					}
					else if (newItems.at(i)[j] == ':')
						continue;
					else
						tmpString += newItems.at(i)[j];
				}
			}
		}
	}
	numOpenBraces = 0;
	numOpenBrackets = 0;

	return false;
}

// Set up ghost tables with headers and row/column count
void AoRGhostManager::setUpTables()
{
	QStringList hHeaders = { "Map", "Class", "Car", "Time" };
	ui.ghostsLeft->setRowCount(items.size());
	ui.ghostsLeft->setColumnCount(4);
	ui.ghostsLeft->setHorizontalHeaderLabels(hHeaders);
	ui.ghostsRight->setRowCount(newItems.size());
	ui.ghostsRight->setColumnCount(4);
	ui.ghostsRight->setHorizontalHeaderLabels(hHeaders);
}

// ***** Constructors *****
AoRGhostManager::AoRGhostManager(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connectActions();
}