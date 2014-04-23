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
#ifndef __QSeaCoreTableModel_H_
#define __QSeaCoreTableModel_H_

#include <vector>

#include <QAbstractTableModel>

class SeaCore;

class QSeaCoreTableModel : public QAbstractTableModel
{
  Q_OBJECT
public:

  // Constructor/Destructor
  QSeaCoreTableModel( QObject* parent = 0);
  virtual ~QSeaCoreTableModel();

  // Required functions
  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  int columnCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section,
                      Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const;

  // Adding cores
  void setCores( const std::vector<SeaCore>& cores );
  void getCores( std::vector<SeaCore>& cores ) const;

protected:

  std::vector<SeaCore> cores;

};

#endif
