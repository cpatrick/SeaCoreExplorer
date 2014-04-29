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
#include <set>

#include <vtkPolyDataMapper.h>
#include <vtkQtTableView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkConeSource.h>
#include <vtkSmartPointer.h>
#include <vtkGeoView2D.h>
#include <vtkGeoFileTerrainSource.h>
#include <vtkGeoProjection.h>
#include <vtkGeoProjectionSource.h>
#include <vtkGeoTransform.h>
#include <vtkGeoTerrain2D.h>
#include <vtkGeoTerrainNode.h>
#include <vtkGeoAlignedImageSource.h>
#include <vtkGeoAlignedImageRepresentation.h>
#include <vtkGeoFileImageSource.h>
#include <vtkJPEGReader.h>
#include <vtkVariantArray.h>
#include <vtkRenderedGraphRepresentation.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkCamera.h>
#include <vtkDoubleArray.h>
#include <vtkDataSetAttributes.h>
#include <vtkAnnotationLink.h>
#include <vtkPlot.h>
#include <vtkDelimitedTextReader.h>
#include <vtkChartXY.h>
#include <vtkContextView.h>
#include <vtkPen.h>
#include <vtkContextScene.h>
#include <vtkTable.h>
#include <vtkIdTypeArray.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>

#include <QVTKWidget.h>

#include <QFileDialog>
#include <QTableView>
#include <QFile>
#include <QDebug>
#include <QProcess>

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

  this->tableSelecting = false;

  int projNum = 44;
  this->qvtkWidget = new QVTKWidget;
  this->view = vtkGeoView2D::New();
  this->view->SetInteractor(this->qvtkWidget->GetInteractor());
  this->qvtkWidget->SetRenderWindow(this->view->GetRenderWindow());

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
  this->view->SetSurface(terrain);

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
  this->view->AddRepresentation(imageRep);


  this->tableView = new QTableView(this->centralwidget);

  // Add the widgets to the layout.
  QVBoxLayout* curLayout = new QVBoxLayout;
  curLayout->addWidget(this->qvtkWidget);
  this->mapFrame->setLayout(curLayout);

  curLayout = new QVBoxLayout;
  curLayout->addWidget(this->tableView);
  this->tableFrame->setLayout(curLayout);

  // Setup signals
  this->connect(this->actionOpen, SIGNAL(triggered()),
                this, SLOT(showFileDialog()));
  this->connect(this->compareButton, SIGNAL(pressed()),
                this, SLOT(compareCores()));
}

void MainWindow::showFileDialog()
{
  QString fileName =
    QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files (*.csv)"));
  qDebug() << "Selected file: " << fileName;
  QFileInfo fileInfo(fileName);
  QString baseFolder = fileInfo.absolutePath();
  std::ifstream inputFile(fileName.toStdString());
  std::string line;

  typedef boost::tokenizer<
    boost::escaped_list_separator<char> > TokenizerType;

  std::vector<std::string> curCore(NUMBER_OF_TOKENS);

  // Ignore first line
  std::getline(inputFile, line, '\r');

  while(std::getline(inputFile, line, '\r'))
  {
    TokenizerType tok(line);
    curCore.assign(tok.begin(), tok.end());
    SeaCore s;
    s.fromVector(curCore);
    s.coreFile = baseFolder.toStdString() + "/" + s.fileName + ".txt";
    this->cores.push_back(s);
  }

  // Setup Model
  QSeaCoreTableModel* model = new QSeaCoreTableModel();
  model->setCores(cores);
  this->tableView->setModel(model);
  //connect( this->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(onClicked(QModelIndex)) );
  connect(this->tableView->selectionModel(),
    SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
    this,
    SLOT(onTableSelect(const QItemSelection &, const QItemSelection &)));

  this->drawCorePoints();
}

void MainWindow::drawGraph()
{
  // Load a some points and render a graph
  vtkSmartPointer<vtkDelimitedTextReader> reader =
    vtkSmartPointer<vtkDelimitedTextReader>::New();
  reader->SetFileName(cores[2].coreFile.c_str());
  reader->DetectNumericColumnsOn();
  reader->SetFieldDelimiterCharacters("\t");
  reader->Update();
  vtkTable* table = reader->GetOutput();
  table->Dump();

  // Set up the view
  vtkSmartPointer<vtkContextView> plotView =
    vtkSmartPointer<vtkContextView>::New();
  plotView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);

  // Add multiple line plots, setting the colors etc
  vtkSmartPointer<vtkChartXY> chart =
    vtkSmartPointer<vtkChartXY>::New();
  plotView->GetScene()->AddItem(chart);
  vtkPlot *line = chart->AddPlot(vtkChart::LINE);
  line->SetInputData(table, 0, 1);
  line->SetColor(0, 255, 0, 255);
  line->SetWidth(2.0);
  line->GetPen()->SetLineType(4);//For dotted line, can be from 2 to 5 for different dot patterns

  // Start interactor
  plotView->Render();
  //plotView->GetInteractor()->Initialize();
  //plotView->GetInteractor()->Start();
}

void MainWindow::drawCorePoints()
{
  vtkSmartPointer<vtkMutableUndirectedGraph > graph =
  vtkSmartPointer<vtkMutableUndirectedGraph >::New();
  vtkSmartPointer<vtkDoubleArray> latArr =
    vtkSmartPointer<vtkDoubleArray>::New();
  vtkSmartPointer<vtkDoubleArray> lonArr =
    vtkSmartPointer<vtkDoubleArray>::New();

  latArr->SetNumberOfTuples(this->cores.size());
  lonArr->SetNumberOfTuples(this->cores.size());
  latArr->SetName("latitude");
  lonArr->SetName("longitude");

  for(int i = 0; i < this->cores.size(); ++i)
  {
    latArr->SetValue(i, this->cores[i].latitude);
    lonArr->SetValue(i, this->cores[i].longitude);
    graph->AddVertex();
  }

  graph->GetVertexData()->AddArray(latArr);
  graph->GetVertexData()->AddArray(lonArr);

  vtkSmartPointer<vtkRenderedGraphRepresentation> graphRep =
    vtkSmartPointer<vtkRenderedGraphRepresentation>::New();
  graphRep->SetInputData(graph);
  graphRep->SetEdgeVisibility(false);
  graphRep->SetLayoutStrategyToAssignCoordinates("longitude", "latitude");

  vtkSmartPointer<vtkAnnotationLink> link = graphRep->GetAnnotationLink();
  link->AddObserver(vtkCommand::AnnotationChangedEvent, this,
    &MainWindow::selectionCallback);

  this->view->AddRepresentation(graphRep);

  this->view->Render();
}

void MainWindow::onTableSelect(const QItemSelection &, const QItemSelection &)
{
  std::set<int> rows;
  vtkSmartPointer<vtkSelection> sel = vtkSmartPointer<vtkSelection>::New();
  vtkSmartPointer<vtkSelectionNode> selNode = vtkSmartPointer<vtkSelectionNode>::New();
  selNode->SetFieldType(vtkSelectionNode::VERTEX);
  selNode->SetContentType(vtkSelectionNode::INDICES);
  vtkSmartPointer<vtkIdTypeArray> arr = vtkSmartPointer<vtkIdTypeArray>::New();
  arr->SetName("user");
  arr->SetNumberOfComponents(1);
  QModelIndexList indexes = this->tableView->selectionModel()->selection().indexes();
  for (int i = 0; i < indexes.count(); ++i)
  {
    if(rows.count(indexes.at(i).row()) == 0)
    {
      arr->InsertNextValue(indexes.at(i).row());
      rows.insert(indexes.at(i).row());
    }

  }
  selNode->SetSelectionList(arr);
  sel->AddNode(selNode);

  this->tableSelecting = true;
  this->view->GetRepresentation(1)->GetAnnotationLink()->SetCurrentSelection(sel);
}

void MainWindow::compareCores()
{
  QModelIndexList indexes = this->tableView->selectionModel()->selection().indexes();
  std::set<int> rows;
  for (int i = 0; i < indexes.count(); ++i)
  {
    if(rows.count(indexes.at(i).row()) == 0)
    {
      rows.insert(indexes.at(i).row());
    }
  }

  std::set<int>::iterator itr = rows.begin();
  int first = *itr++;
  int second = *itr;

  QString program = QCoreApplication::applicationDirPath() + QString("/SeaCoreComparer");
  QStringList arguments;
  arguments << this->cores[first].coreFile.c_str() << this->cores[second].coreFile.c_str();

  QProcess *myProcess = new QProcess(this);
  myProcess->start(program, arguments);
}

void MainWindow::selectionCallback(vtkObject* caller,
  long unsigned int eventId,
  void* callData)
{
  vtkAnnotationLink* annotationLink = static_cast<vtkAnnotationLink*>(caller);

  vtkSelection* selection = annotationLink->GetCurrentSelection();
  vtkSelectionNode* vertices;
  vtkSelectionNode* edges;
  if(selection->GetNode(0)->GetFieldType() == vtkSelectionNode::VERTEX)
    {
    vertices = selection->GetNode(0);
    }
  else if(selection->GetNode(0)->GetFieldType() == vtkSelectionNode::EDGE)
    {
    edges = selection->GetNode(0);
    }

  vtkIdTypeArray* vertexList = vtkIdTypeArray::SafeDownCast(vertices->GetSelectionList());
  if(this->tableSelecting)
  {
    this->view->Render();
    this->tableSelecting = false;
    return;
  }
  for(vtkIdType i = 0; i < vertexList->GetNumberOfTuples(); i++)
  {
    int curValue = vertexList->GetValue(i);
    if(curValue < 241) // HACK above this are NaNs in Lat-Lon.
    {
      this->tableView->selectRow(curValue);
    }
  }
  this->tableSelecting = false;
}
