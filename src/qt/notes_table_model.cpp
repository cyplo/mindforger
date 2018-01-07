/*
 notes_table_model.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "notes_table_model.h"

namespace m8r {

NotesTableModel::NotesTableModel(QObject *parent)
    : QStandardItemModel(parent)
{
    setColumnCount(2);
    setRowCount(0);
}

void NotesTableModel::removeAllRows(void)
{
    QStandardItemModel::clear();

    QStringList tableHeader;
    tableHeader
            << tr("Note")
            << tr("Outline");
    setHorizontalHeaderLabels(tableHeader);
}

void NotesTableModel::addRow(Note* note)
{
    QList<QStandardItem*> items;

    QStandardItem* noteItem = new QStandardItem(QString(note->getTitle().c_str()));
    noteItem->setData(QVariant::fromValue(note));
    items.append(noteItem);
    items.append(new QStandardItem(note->getOutline()->getTitle().c_str()));

    appendRow(items);
}

}
