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
#include <vtkPolyDataMapper.h>
#include <vtkQtTableView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkConeSource.h>
#include <vtkSmartPointer.h>

#include <QFileDialog>
#include <QFile>
#include <QDebug>

#include "QSeaCoreTableModel.h"

#include "MainWindow.h"

// Constructor
MainWindow::MainWindow()
{
  this->setupUi(this);

  //Create a cone
  vtkSmartPointer<vtkConeSource> coneSource =
    vtkSmartPointer<vtkConeSource>::New();
  coneSource->Update();

  //Create a mapper and actor
  vtkSmartPointer<vtkPolyDataMapper> mapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(coneSource->GetOutputPort());

  vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  //Create a renderer and add the actor
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(actor);

  // VTK/Qt wedded
  this->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);

  // Setup Model
  QSeaCoreTableModel* model = new QSeaCoreTableModel();
  this->tableView->setModel(model);

  // Setup signals
  this->connect(this->actionOpen, SIGNAL(triggered()),
                this, SLOT(showFileDialog()));
}

void MainWindow::showFileDialog()
{
  QString fileName =
    QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files (*.csv)"));
  qDebug() << "Selected file: " << fileName;
  QFile csvFile(fileName);
  QStringList lines;
  if(csvFile.open(QFile::ReadOnly))
  {
    qDebug() << "Loading " << fileName;
    QString data = csvFile.readAll();
    lines = data.split("\n");
    csvFile.close();
  }
  else
  {
    qCritical() << "Unable to open " << fileName;
  }

  for(QString &curLine : lines)
  {
    qDebug() << curLine;
  }

}
