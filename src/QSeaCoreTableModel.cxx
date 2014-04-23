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
#include "SeaCore.h"

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
    return this->cores.size();
}

int
QSeaCoreTableModel
::columnCount(const QModelIndex& parent) const
{
    return 10;
}

QVariant
QSeaCoreTableModel
::data(const QModelIndex& index, int role) const
{
    if(role == Qt::DisplayRole)
    {
        return this->cores[index.row()].getFieldByIndex(index.column());
    }
    return QVariant();
}

QVariant
QSeaCoreTableModel
::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Core Name");
            case 1:
                return QString("Publication");
            case 2:
                return QString("Latitude");
            case 3:
                return QString("Longitude");
            case 4:
                return QString("Proxy");
            case 5:
                return QString("Length");
            case 6:
                return QString("Average Sampling");
            case 7:
                return QString("Maximum Sampling");
            case 8:
                return QString("Dating Method");
            case 9:
                return QString("Notes");
            }
        }
    }
    return QVariant();
}

void
QSeaCoreTableModel
::setCores( const std::vector<SeaCore>& cores )
{
    this->cores = cores;
}

void
QSeaCoreTableModel
::getCores( std::vector<SeaCore>& cores ) const
{
    cores = this->cores;
}