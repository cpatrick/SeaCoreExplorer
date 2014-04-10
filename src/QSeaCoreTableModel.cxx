/******************************************************************************
 * Copyright 2014 Matthew Adams, Elizabeth Reischmann, Patrick Reynolds
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#include "QSeaCoreTableModel.h"

QSeaCoreTableModel::QSeaCoreTableModel( QObject* parent)
: QAbstractTableModel(parent)
{
}

QSeaCoreTableModel::~QSeaCoreTableModel()
{
}

int
QSeaCoreTableModel
::rowCount(const QModelIndex& parent) const
{
    return 5;
}

int
QSeaCoreTableModel
::columnCount(const QModelIndex& parent) const
{
    return 3;
}

QVariant
QSeaCoreTableModel
::data(const QModelIndex& index, int role) const
{
    if(role == Qt::DisplayRole)
    {
        return QVariant(index.row() * index.column() + role);
    }
    return QVariant();
}

QVariant
QSeaCoreTableModel
::headerData(int section, Qt::Orientation orientation, int role) const
{
    return section;
}
