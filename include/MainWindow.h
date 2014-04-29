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
#ifndef __MainWindow_H_
#define __MainWindow_H_

#include <vector>

#include <QMainWindow>

class vtkGeoView2D;
class vtkMutableUndirectedGraph;
class vtkRenderedGraphRepresentation;
class vtkAnnotationLink;

#include "SeaCore.h"

#include "ui_MainWindow.h"

class QVTKWidget;
class QTableView;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
  Q_OBJECT
public:

  // Constructor/Destructor
  MainWindow();
  virtual ~MainWindow() {};

public slots:

  void showFileDialog();
  void drawCorePoints();
  void drawGraph();

protected:

  void selectionCallback(vtkObject * obj, unsigned long id, void *ptr);

  QVTKWidget* qvtkWidget;
  QTableView* tableView;
  std::vector<SeaCore> cores;
  vtkGeoView2D* view;

};

#endif
