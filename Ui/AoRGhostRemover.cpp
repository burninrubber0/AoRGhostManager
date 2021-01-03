#include "AoRGhostManager.h"

#include <fstream>

#include <QDialog>
#include <QFileDialog>
#include <QTableWidget>

// ***** Slots *****
void AoRGhostRemover::removeGhost()
{
	foreach(QModelIndex index, rm.tableGhosts->selectionModel()->selectedRows())
	{
		rm.tableGhosts->item(index.row(), 0)->setBackground(Qt::red);
		rm.tableGhosts->item(index.row(), 1)->setBackground(Qt::red);
		rm.tableGhosts->item(index.row(), 2)->setBackground(Qt::red);
		rm.tableGhosts->item(index.row(), 3)->setBackground(Qt::red);
	}
}

void AoRGhostRemover::addGhost()
{
	foreach(QModelIndex index, rm.tableGhosts->selectionModel()->selectedRows())
	{
		if (rm.tableGhosts->item(index.row(), 0)->background() == Qt::red)
		{
			rm.tableGhosts->item(index.row(), 0)->setBackground(Qt::white);
			rm.tableGhosts->item(index.row(), 1)->setBackground(Qt::white);
			rm.tableGhosts->item(index.row(), 2)->setBackground(Qt::white);
			rm.tableGhosts->item(index.row(), 3)->setBackground(Qt::white);
		}
	}
}

void AoRGhostRemover::saveGhosts()
{
	std::ofstream ghosts;
	std::ofstream backup;

	QFileInfo backupInfo(ghostsPath);
	QString backupPath = backupInfo.dir().path() + "/Ghosts.original.txt";
	backup.open(backupPath.toStdString(), std::ios::out);
	backup << backupData;
	backup.close();

	for (int i = items.size() - 1; i >= 0; --i)
	{
		if (rm.tableGhosts->item(i, 0)->background() == Qt::red)
			items.erase(items.begin() + i);
	}

	ghosts.open(ghostsPath.toStdString(), std::ios::out);
	ghosts << "{\"Items\":[";
	for (int i = 0; i < items.size(); ++i)
	{
		ghosts << items.at(i) << '}';
		if (i != items.size() - 1)
			ghosts << ',';
	}
	ghosts << "]}";
	ghosts.close();

	QDialog::close();
}

void AoRGhostRemover::exitGr()
{
	QDialog::close();
}

// ***** UI *****
// Open ghosts file
void AoRGhostRemover::openGhostsFile()
{
	std::ifstream ghosts;

	ghostsPath = QFileDialog::getOpenFileName(this, tr("Select your Ghosts.txt"), defaultGhostsPath, tr("Ghosts file (*.txt)"));

	ghosts.open(ghostsPath.toStdString(), std::ios::in);
	std::getline(ghosts, ghostsData);
	ghosts.close();

	// Store original data
	backupData = ghostsData;
}

void AoRGhostRemover::loadRmTable()
{
	if (!parseGhostData())
	{
		rm.tableGhosts->setRowCount(items.size());

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
			rm.tableGhosts->setItem(i, 0, itm0);
			rm.tableGhosts->setItem(i, 1, itm1);
			rm.tableGhosts->setItem(i, 2, itm2);
			rm.tableGhosts->setItem(i, 3, itm3);
		}
	}
	else
		QDialog::close();
}

bool AoRGhostRemover::parseGhostData()
{
	std::string start;
	std::string tmpItem;
	int numOpenBraces = 0;
	int numOpenBrackets = 0;

	// Save each item
	for (int i = 0; i < ghostsData.length(); ++i)
	{
		// Check that start of file is correct
		if (i < 11)
			start += ghostsData[i];
		if (i == 10)
		{
			if (start != "{\"Items\":[{")
			{
				QMessageBox::critical(this, "Error", "Invalid ghosts file");
				return true;
			}
		}

		// Track braces to determine item start/end
		if (ghostsData[i] == '{')
			numOpenBraces += 1;
		else if (ghostsData[i] == '}')
			numOpenBraces -= 1;
		else if (ghostsData[i] == '[')
			numOpenBrackets += 1;
		else if (ghostsData[i] == ']')
			numOpenBrackets -= 1;

		// Store the current item
		if (numOpenBraces > 1)
			tmpItem += ghostsData[i];

		// Save the current item when it ends
		if (numOpenBraces == 1 && numOpenBrackets == 1 && ghostsData[i] == ',')
		{
			items.push_back(tmpItem);
			tmpItem.clear();
		}

		// Save the last item
		if (i != 0 && numOpenBraces == 0 && numOpenBrackets == 0)
			items.push_back(tmpItem);
	}

	// Check if all brackets and braces are closed at end
	if (numOpenBraces != 0 || numOpenBrackets != 0)
	{
		QString text = "Unclosed braces: " + QString::number(numOpenBraces)
			+ ", unclosed brackets: " + QString::number(numOpenBrackets);
		QMessageBox::critical(this, "Error parsing ghosts file", text);
		return true;
	}

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
					else if (recordingValue)
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

	return false;
}

void AoRGhostRemover::connectActions()
{
	connect(rm.btnRemove, &QPushButton::clicked, this, &AoRGhostRemover::removeGhost);
	connect(rm.btnAdd, &QPushButton::clicked, this, &AoRGhostRemover::addGhost);
	connect(rm.btnSave, &QPushButton::clicked, this, &AoRGhostRemover::saveGhosts);
	connect(rm.btnCancel, &QPushButton::clicked, this, &AoRGhostRemover::exitGr);
}