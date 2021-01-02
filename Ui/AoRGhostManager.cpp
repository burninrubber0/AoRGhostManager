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
// Open both ghost files
void AoRGhostManager::openGhosts()
{
	openPlayerGhosts();
	openNewGhosts();
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

// Exit program
void AoRGhostManager::exitGm()
{
	if (tableLoaded.at(0))
	{
		if (QMessageBox::warning(this, "Warning", "Are you sure? Any unsaved changes will be lost.",
			QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No) == QMessageBox::StandardButton::Yes)
		{
			QApplication::quit();
		}
	}
	else
	{
		QApplication::quit();
	}
}

// Bring up the about window
void AoRGhostManager::about()
{
	QMessageBox::about(this, "About", "<p align='center'>Art of Rally Ghost Manager<br>by burninrubber0");
}

// Add items to the player ghosts vector/table
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
			for (int i = 0; i < items.size(); ++i)
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
					&& !hasPushed && !hasReplaced)
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
					break;
				}
				// Else add new entry (green)
				else if ((itmMap->text() != ui.ghostsLeft->item(i, 0)->text()
					|| itmClass->text() != ui.ghostsLeft->item(i, 1)->text())
					&& !hasPushed && !hasReplaced && i == items.size() - 1)
				{
					items.push_back(newItems.at(index.row()));
					hasPushed = true;
					ui.ghostsLeft->setRowCount(items.size());
					if (ui.ghostsRight->item(index.row(), 0)->background() != Qt::lightGray)
					{
						itmMap->setBackground(Qt::green);
						itmClass->setBackground(Qt::green);
						itmCar->setBackground(Qt::green);
						itmTime->setBackground(Qt::green);
					}
					ui.ghostsLeft->setItem(items.size() - 1, 0, itmMap);
					ui.ghostsLeft->setItem(items.size() - 1, 1, itmClass);
					ui.ghostsLeft->setItem(items.size() - 1, 2, itmCar);
					ui.ghostsLeft->setItem(items.size() - 1, 3, itmTime);
				}
			}
		}

		// Remove transferred rows from new vector and new table
		for (int i = newItems.size() - 1; i >= 0; --i)
		{
			if (ui.ghostsRight->item(i, 0)->isSelected())
			{
				newItems.erase(newItems.begin() + i);
				ui.ghostsRight->removeRow(i);
			}
		}
	}
}

// Remove items from the player ghosts vector/table
void AoRGhostManager::transferRight()
{
	// If a ghost is selected in the left table
	if (ui.ghostsLeft->selectionModel()->hasSelection())
	{
		// Then for each selected row
		foreach(const QModelIndex index, ui.ghostsLeft->selectionModel()->selectedRows())
		{
			bool hasBeenTransferred = false;

			// Move both the vector items and table items to the right table
			for (int i = 0; i < items.size(); ++i)
			{
				QTableWidgetItem* itmMap = new QTableWidgetItem();
				QTableWidgetItem* itmClass = new QTableWidgetItem();
				QTableWidgetItem* itmCar = new QTableWidgetItem();
				QTableWidgetItem* itmTime = new QTableWidgetItem();
				itmMap->setText(ui.ghostsLeft->item(index.row(), 0)->data(Qt::DisplayRole).toString());
				itmClass->setText(ui.ghostsLeft->item(index.row(), 1)->data(Qt::DisplayRole).toString());
				itmCar->setText(ui.ghostsLeft->item(index.row(), 2)->data(Qt::DisplayRole).toString());
				itmTime->setText(ui.ghostsLeft->item(index.row(), 3)->data(Qt::DisplayRole).toString());

				// If ghost was replacing another, revert to the original
				if (ui.ghostsLeft->item(index.row(), 0)->background() == Qt::red
					&& !hasBeenTransferred)
				{
					// Set new table entry
					newItems.push_back(items.at(index.row()));
					ui.ghostsRight->setRowCount(newItems.size());
					ui.ghostsRight->setItem(newItems.size() - 1, 0, itmMap);
					ui.ghostsRight->setItem(newItems.size() - 1, 1, itmClass);
					ui.ghostsRight->setItem(newItems.size() - 1, 2, itmCar);
					ui.ghostsRight->setItem(newItems.size() - 1, 3, itmTime);
					hasBeenTransferred = true;

					// Revert player table entry
					// This will break when player items get deleted! Remove the delete button altogether and prevent the transfer of player ghosts?
					ui.ghostsLeft->item(index.row(), 0)->setBackground(Qt::white);
					ui.ghostsLeft->item(index.row(), 1)->setBackground(Qt::white);
					ui.ghostsLeft->item(index.row(), 2)->setBackground(Qt::white);
					ui.ghostsLeft->item(index.row(), 3)->setBackground(Qt::white);
					ui.ghostsLeft->item(index.row(), 0)->setText(QString::fromStdString(bckMaps.at(index.row())));
					ui.ghostsLeft->item(index.row(), 1)->setText(QString::fromStdString(bckClasses.at(index.row())));
					ui.ghostsLeft->item(index.row(), 2)->setText(QString::fromStdString(bckCars.at(index.row())));
					ui.ghostsLeft->item(index.row(), 3)->setText(QString::fromStdString(bckTimes.at(index.row())));
				}
				// If it wasn't replacing, move back normally
				else if (ui.ghostsLeft->item(index.row(), 0)->background() == Qt::green
					&& !hasBeenTransferred)
				{
					// Set new table entry
					newItems.push_back(items.at(index.row()));
					ui.ghostsRight->setRowCount(newItems.size());
					ui.ghostsRight->setItem(newItems.size() - 1, 0, itmMap);
					ui.ghostsRight->setItem(newItems.size() - 1, 1, itmClass);
					ui.ghostsRight->setItem(newItems.size() - 1, 2, itmCar);
					ui.ghostsRight->setItem(newItems.size() - 1, 3, itmTime);
					hasBeenTransferred = true;
				}
				// If it was originally a player ghost, TODO: make it light grey or leave disabled?
				else
				{
					// TODO
				}
			}
		}

		// Remove previously transferred new rows from player vector/table
		for (int i = items.size() - 1; i >= 0; --i)
		{
			if (ui.ghostsLeft->item(i, 0)->isSelected()
				&& ui.ghostsLeft->item(i, 0)->background() == Qt::green)
			{
				items.erase(items.begin() + i);
				ui.ghostsLeft->removeRow(i);
			}
		}
	}
}

void AoRGhostManager::search(QLineEdit* line, QComboBox* comboBox, QTableWidget* table)
{
	// TODO: Fix filters

	isNew.resize(table->rowCount(), false);
	isReplaced.resize(table->rowCount(), false);

	// If not blank
	if (line->text().length() != 0)
	{
		// Then for every row in the table
		for (int i = 0; i < table->rowCount(); ++i)
		{
			if (line == ui.lnPlayerGhostSearch)
			{
				if (table->item(i, 0)->background() == Qt::green)
					isNew.at(i) = true;
				else if (table->item(i, 0)->background() == Qt::red)
					isReplaced.at(i) = true;
				else if (table->item(i, 0)->background() == Qt::white)
				{
					isNew.at(i) = false;
					isReplaced.at(i) = false;
				}
			}

			// Check for a match
			// Search maps
			if (table->item(i, 0)->text().contains(line->text(), Qt::CaseInsensitive))
			{
				if (comboBox->currentIndex() == 0 || comboBox->currentIndex() == 1)
				{
					// Set background to yellow 
					table->item(i, 0)->setBackground(Qt::yellow);
					table->item(i, 1)->setBackground(Qt::yellow);
					table->item(i, 2)->setBackground(Qt::yellow);
					table->item(i, 3)->setBackground(Qt::yellow);
				}
			}
			// Search classes
			else if (table->item(i, 1)->text().contains(line->text(), Qt::CaseInsensitive))
			{
				if (comboBox->currentIndex() == 0 || comboBox->currentIndex() == 2)
				{
					// Set background to yellow 
					table->item(i, 0)->setBackground(Qt::yellow);
					table->item(i, 1)->setBackground(Qt::yellow);
					table->item(i, 2)->setBackground(Qt::yellow);
					table->item(i, 3)->setBackground(Qt::yellow);
				}
			}
			// Search cars
			else if (table->item(i, 2)->text().contains(line->text(), Qt::CaseInsensitive))
			{
				if (comboBox->currentIndex() == 0 || comboBox->currentIndex() == 3)
				{
					// Set background to yellow 
					table->item(i, 0)->setBackground(Qt::yellow);
					table->item(i, 1)->setBackground(Qt::yellow);
					table->item(i, 2)->setBackground(Qt::yellow);
					table->item(i, 3)->setBackground(Qt::yellow);
				}
			}
			// If previously highlighted item no longer matches
			else if (!table->item(i, 0)->text().contains(line->text(), Qt::CaseInsensitive)
				&& !table->item(i, 1)->text().contains(line->text(), Qt::CaseInsensitive)
				&& !table->item(i, 2)->text().contains(line->text(), Qt::CaseInsensitive)
				&& table->item(i, 0)->background() == Qt::yellow)
			{
				// Revert to original color
				if (line == ui.lnPlayerGhostSearch)
				{
					if (!isNew.at(i) && !isReplaced.at(i))
					{
						table->item(i, 0)->setBackground(Qt::white);
						table->item(i, 1)->setBackground(Qt::white);
						table->item(i, 2)->setBackground(Qt::white);
						table->item(i, 3)->setBackground(Qt::white);
					}
					else if (isNew.at(i))
					{
						table->item(i, 0)->setBackground(Qt::green);
						table->item(i, 1)->setBackground(Qt::green);
						table->item(i, 2)->setBackground(Qt::green);
						table->item(i, 3)->setBackground(Qt::green);
					}
					else if (isReplaced.at(i))
					{
						table->item(i, 0)->setBackground(Qt::red);
						table->item(i, 1)->setBackground(Qt::red);
						table->item(i, 2)->setBackground(Qt::red);
						table->item(i, 3)->setBackground(Qt::red);
					}
				}
				else
				{
					table->item(i, 0)->setBackground(Qt::white);
					table->item(i, 1)->setBackground(Qt::white);
					table->item(i, 2)->setBackground(Qt::white);
					table->item(i, 3)->setBackground(Qt::white);
				}
			}
		}
	}
	// If line becomes empty again
	else if (line->text().length() == 0)
	{
		for (int i = 0; i < table->rowCount(); ++i)
		{
			// Revert all items to their original color
			if (line == ui.lnPlayerGhostSearch)
			{
				if (table->item(i, 0)->background() == Qt::yellow
					&& !isNew.at(i) && !isReplaced.at(i))
				{
					table->item(i, 0)->setBackground(Qt::white);
					table->item(i, 1)->setBackground(Qt::white);
					table->item(i, 2)->setBackground(Qt::white);
					table->item(i, 3)->setBackground(Qt::white);
				}
				else if (isNew.at(i))
				{
					table->item(i, 0)->setBackground(Qt::green);
					table->item(i, 1)->setBackground(Qt::green);
					table->item(i, 2)->setBackground(Qt::green);
					table->item(i, 3)->setBackground(Qt::green);
				}
				else if (isReplaced.at(i))
				{
					table->item(i, 0)->setBackground(Qt::red);
					table->item(i, 1)->setBackground(Qt::red);
					table->item(i, 2)->setBackground(Qt::red);
					table->item(i, 3)->setBackground(Qt::red);
				}
			}
			else
			{
				if (table->item(i, 0)->background() == Qt::yellow)
				{
					table->item(i, 0)->setBackground(Qt::white);
					table->item(i, 1)->setBackground(Qt::white);
					table->item(i, 2)->setBackground(Qt::white);
					table->item(i, 3)->setBackground(Qt::white);
				}
			}
		}
	}
}

void AoRGhostManager::openPlayerGhosts()
{
	if (!tableLoaded.at(0))
	{
		ghostsPath = QFileDialog::getOpenFileName(this, tr("Select your Ghosts.txt"), defaultGhostsPath, tr("Ghosts file (*.txt)"));
		ui.lnPlayerGhostsFile->setText(ghostsPath);
		if (!readGhosts(false))
			loadTable(false);
	}
	else
	{
		if (QMessageBox::warning(this, "Warning", "Changing the current player file will cause any unsaved changes to be lost. Continue?",
			QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No) == QMessageBox::StandardButton::Yes)
		{
			items.clear();
			maps.clear();
			classes.clear();
			cars.clear();
			times.clear();
			bckItems.clear();
			bckMaps.clear();
			bckClasses.clear();
			bckCars.clear();
			bckTimes.clear();
			isNew.clear();
			isReplaced.clear();

			ghostsPath = QFileDialog::getOpenFileName(this, tr("Select your Ghosts.txt"), defaultGhostsPath, tr("Ghosts file (*.txt)"));
			ui.lnPlayerGhostsFile->setText(ghostsPath);
			if (!readGhosts(false))
				loadTable(false);
		}
	}
}

void AoRGhostManager::openNewGhosts()
{
	if (!tableLoaded.at(1))
	{
		newGhostsPath = QFileDialog::getOpenFileName(this, tr("Select new Ghosts.txt"), defaultGhostsPath, tr("Ghosts file (*.txt)"));
		ui.lnNewGhostsFile->setText(newGhostsPath);
		if (!readGhosts(true))
			loadTable(true);
	}
	else
	{
		newItems.clear();
		newMaps.clear();
		newClasses.clear();
		newCars.clear();
		newTimes.clear();

		newGhostsPath = QFileDialog::getOpenFileName(this, tr("Select new Ghosts.txt"), defaultGhostsPath, tr("Ghosts file (*.txt)"));
		ui.lnNewGhostsFile->setText(newGhostsPath);
		if (!readGhosts(true))
			loadTable(true);
	}
}

// ***** UI *****
// Connect signals to slots
void AoRGhostManager::connectActions()
{
	connect(ui.actionOpen, &QAction::triggered, this, &AoRGhostManager::openGhosts);
	connect(ui.actionSave, &QAction::triggered, this, [this] { AoRGhostManager::saveGhosts(ghostsPath); });
	connect(ui.actionSaveAs, &QAction::triggered, this, &AoRGhostManager::saveGhostsAs);
	connect(ui.actionExit, &QAction::triggered, this, &AoRGhostManager::exitGm);
	connect(ui.actionAbout, &QAction::triggered, this, &AoRGhostManager::about);
	connect(ui.btnTransferLeft, &QPushButton::clicked, this, &AoRGhostManager::transferLeft);
	connect(ui.btnTransferRight, &QPushButton::clicked, this, &AoRGhostManager::transferRight);
	connect(ui.lnPlayerGhostSearch, &QLineEdit::textEdited, this,
		[this] { AoRGhostManager::search(ui.lnPlayerGhostSearch, ui.cboPlayerSearch, ui.ghostsLeft); });
	connect(ui.lnNewGhostSearch, &QLineEdit::textEdited, this,
		[this] { AoRGhostManager::search(ui.lnNewGhostSearch, ui.cboNewSearch, ui.ghostsRight); });
	//connect(ui.cboPlayerSearch, &QComboBox::currentIndexChanged, this,
	//	[this] { AoRGhostManager::search(ui.lnPlayerGhostSearch, ui.cboPlayerSearch, ui.ghostsLeft); });
	//connect(ui.cboNewSearch, &QComboBox::currentIndexChanged, this,
	//	[this] { AoRGhostManager::search(ui.lnNewGhostSearch, ui.cboNewSearch, ui.ghostsRight); });
	connect(ui.btnBrowsePlayerFile, &QToolButton::clicked, this, &AoRGhostManager::openPlayerGhosts);
	connect(ui.btnBrowseNewFile, &QToolButton::clicked, this, &AoRGhostManager::openNewGhosts);
}

// Load ghost data into tables
void AoRGhostManager::loadTable(bool loadNew)
{
	// If no error was encountered, load parsed ghost data
	if (!loadNew)
	{
		if (!parseGhostData(playerGhostsData, false))
		{
			setUpTable(false);

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

				tableLoaded.at(0) = true;
			}
		}
	}
	else
	{
		if (!parseGhostData(newGhostsData, true))
		{
			setUpTable(true);

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

				tableLoaded.at(1) = true;
			}
		}
	}
	// If both tables are loaded and have not previously been loaded
	if (tableLoaded.at(0) && tableLoaded.at(1) && !ui.actionSave->isEnabled())
	{
		// Enable actions after the loops have completed
		ui.actionSave->setEnabled(true);
		ui.actionSaveAs->setEnabled(true);
		ui.btnTransferLeft->setEnabled(true);
		ui.btnTransferRight->setEnabled(true);
		ui.lnPlayerGhostSearch->setEnabled(true);
		ui.lnNewGhostSearch->setEnabled(true);
		ui.lnPlayerGhostSearch->setPlaceholderText("Search...");
		ui.lnNewGhostSearch->setPlaceholderText("Search...");
		//ui.cboPlayerSearch->setEnabled(true);
		//ui.cboNewSearch->setEnabled(true);
		ui.cboPlayerSearch->setCurrentIndex(0);
		ui.cboNewSearch->setCurrentIndex(0);
	}
}

bool AoRGhostManager::readGhosts(bool isNew)
{
	std::ifstream ghosts;

	if (!isNew)
	{
		ghosts.open(ghostsPath.QString::toStdString(), std::ios::in);
		if (ghosts.fail())
		{
			QMessageBox::critical(this, "Error", "Failed to open ghosts file");
			return true;
		}
		std::getline(ghosts, playerGhostsData);
		ghosts.close();

		// Back up data
		playerGhostsDataBck = playerGhostsData;
	}
	else
	{
		ghosts.open(newGhostsPath.QString::toStdString(), std::ios::in);
		if (ghosts.fail())
		{
			QMessageBox::critical(this, "Error", "Failed to open ghosts file");
			return true;
		}
		std::getline(ghosts, newGhostsData);
		ghosts.close();
	}

	return false;
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
				QMessageBox::critical(this, "Error", "Invalid ghosts file");
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

	if (!isNew)
	{
		// Store original items
		bckItems = items;

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
				if (numQuotes == 0 && recordingValue)
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

		// Store original data
		bckMaps = maps;
		bckClasses = classes;
		bckCars = cars;
		bckTimes = times;
	}
	else if (isNew)
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
						else if (recordingValue)
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
				if (numQuotes == 0 && recordingValue)
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
void AoRGhostManager::setUpTable(bool setUpNew)
{
	QStringList hHeaders = { "Map", "Class", "Car", "Time" };
	if (!setUpNew)
	{
		ui.ghostsLeft->setRowCount(items.size());
		ui.ghostsLeft->setColumnCount(4);
		ui.ghostsLeft->setHorizontalHeaderLabels(hHeaders);
	}
	else
	{
		ui.ghostsRight->setRowCount(newItems.size());
		ui.ghostsRight->setColumnCount(4);
		ui.ghostsRight->setHorizontalHeaderLabels(hHeaders);
	}
}

// ***** Constructors *****
AoRGhostManager::AoRGhostManager(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connectActions();
}