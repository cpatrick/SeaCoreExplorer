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
#include <vector>
#include <iostream>
#include <fstream>

#include <vtkPolyDataMapper.h>
#include <vtkQtTableView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkConeSource.h>
#include <vtkSmartPointer.h>
#include <vtkGeoFileTerrainSource.h>
#include <vtkGeoProjection.h>
#include <vtkGeoProjectionSource.h>
#include <vtkGeoTransform.h>
#include <vtkGeoTerrain2D.h>
#include <vtkGeoTerrainNode.h>
#include <vtkGeoView2D.h>
#include <vtkGeoAlignedImageSource.h>
#include <vtkGeoAlignedImageRepresentation.h>
#include <vtkGeoFileImageSource.h>
#include <vtkJPEGReader.h>
#include <vtkCamera.h>

#include <QFileDialog>
#include <QTableView>
#include <QFile>
#include <QDebug>

#include <boost/tokenizer.hpp>

#include "csv_v3.h"
#include "QSeaCoreTableModel.h"
#include "SeaCore.h"

#include "MainWindow.h"

static const int NUMBER_OF_TOKENS = 10;

// Constructor
MainWindow::MainWindow()
{
  this->setupUi(this);

  int projNum = 44;
  this->qvtkWidget = new QVTKWidget;
  vtkGeoView2D *view = vtkGeoView2D::New();
  view->SetInteractor(this->qvtkWidget->GetInteractor());
  this->qvtkWidget->SetRenderWindow(view->GetRenderWindow());

  // Create the terrain
  vtkSmartPointer<vtkGeoTerrain2D> terrain =
    vtkSmartPointer<vtkGeoTerrain2D>::New();
  vtkSmartPointer<vtkGeoSource> terrainSource;
  vtkGeoProjectionSource* projSource = vtkGeoProjectionSource::New();
  projSource->SetProjection(projNum);
  projSource->Initialize();
  vtkSmartPointer<vtkGeoTransform> transform =
    vtkSmartPointer<vtkGeoTransform>::New();
  vtkSmartPointer<vtkGeoProjection> proj =
    vtkSmartPointer<vtkGeoProjection>::New();
  proj->SetName(vtkGeoProjection::GetProjectionName(projNum));
  transform->SetDestinationProjection(proj);
  terrainSource.TakeReference(projSource);
  terrain->SetSource(terrainSource);
  view->SetSurface(terrain);

  // Copy BG Image
  QFile mapResource(":/map.jpg");
  QString mapPath = QDir::temp().absolutePath() + QString("/map.jpg");
  mapResource.copy(mapPath.toStdString().c_str());

  // Create background image
  vtkSmartPointer<vtkGeoAlignedImageRepresentation> imageRep =
    vtkSmartPointer<vtkGeoAlignedImageRepresentation>::New();
  vtkSmartPointer<vtkGeoSource> imageSource;
  vtkGeoAlignedImageSource* alignedSource = vtkGeoAlignedImageSource::New();
  vtkSmartPointer<vtkJPEGReader> reader =
    vtkSmartPointer<vtkJPEGReader>::New();
  reader->SetFileName(mapPath.toStdString().c_str());
  reader->Update();
  alignedSource->SetImage(reader->GetOutput());
  imageSource.TakeReference(alignedSource);
  imageSource->Initialize();
  imageRep->SetSource(imageSource);
  view->AddRepresentation(imageRep);
  view->GetRenderer()->GetActiveCamera()->Zoom(0.5);

  this->tableView = new QTableView(this->centralwidget);

  this->verticalLayout->addWidget(this->qvtkWidget);
  this->verticalLayout->addWidget(this->tableView);

  double vec[3] = {20, 20, 0};
  double out[3];
  transform->TransformPoint(vec, out);
  std::cout << vec[0] << "," << vec[1] << "," << vec[2] << std::endl;
  std::cout << out[0] << "," << out[1] << "," << out[2] << std::endl;

  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetCenter(out);
  sphere->SetRadius(5.0);
  sphere->SetThetaResolution(18);
  sphere->SetPhiResolution(18);

  // map to graphics library
  vtkPolyDataMapper *map = vtkPolyDataMapper::New();
  map->SetInputData(sphere->GetOutput());

  // actor coordinates geometry, properties, transformation
  vtkActor *aSphere = vtkActor::New();
  aSphere->SetMapper(map);
  view->GetRenderer()->AddActor(aSphere);

  // Setup signals
  this->connect(this->actionOpen, SIGNAL(triggered()),
                this, SLOT(showFileDialog()));
}

void MainWindow::showFileDialog()
{
  QString fileName =
    QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files (*.csv)"));
  qDebug() << "Selected file: " << fileName;
  std::ifstream inputFile(fileName.toStdString());
  std::string line;

  typedef boost::tokenizer<
    boost::escaped_list_separator<char> > TokenizerType;

  std::vector<std::string> curCore(NUMBER_OF_TOKENS);

  // Ignore first line
  std::getline(inputFile, line, '\r');

  std::vector<SeaCore> cores;
  while(std::getline(inputFile, line, '\r'))
  {
    TokenizerType tok(line);
    curCore.assign(tok.begin(), tok.end());
    SeaCore s;
    s.fromVector(curCore);
    cores.push_back(s);
  }

  // Setup Model
  QSeaCoreTableModel* model = new QSeaCoreTableModel();
  model->setCores(cores);
  this->tableView->setModel(model);
}
