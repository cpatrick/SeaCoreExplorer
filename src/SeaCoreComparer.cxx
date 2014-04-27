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

#include <vtkDelimitedTextReader.h>
#include <vtkTable.h>
#include <vtkContextView.h>
#include <vtkChartXY.h>
#include <vtkPlot.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTableToArray.h>
#include <vtkArrayToTable.h>
#include <vtkArraySort.h>
#include <vtkPen.h>
#include <vtkContextScene.h>
#include <vtkRenderer.h>
#include <vtkAxis.h>
#include <vtkCommand.h>
#include <vtkCallbackCommand.h>
#include <vtkAnnotationLink.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkAbstractArray.h>
#include <vtkDataArray.h>
#include <vtkArrayIterator.h>
#include <vtkArrayIteratorTemplate.h>
#include <vtkDataSetAttributes.h>

vtkSmartPointer<vtkAnnotationLink> annotationLink;

static void selectionCallback(vtkObject* obj, unsigned long, void*, void*)
{
  vtkSmartPointer<vtkSelection> annSel = annotationLink->GetCurrentSelection();
  vtkSmartPointer<vtkAbstractArray> idxArr = annSel->GetNode(0)->GetSelectionList();
  vtkSmartPointer<vtkDataSetAttributes> selData =
    annSel->GetNode(0)->GetSelectionData();
  std::cout << "Number of Nodes: " << annSel->GetNumberOfNodes() << std::endl;
  vtkSmartPointer<vtkDataArray> data = vtkDataArray::SafeDownCast(selData->GetArray(0));
  vtkArrayIteratorTemplate<vtkIdType>* iter
    = static_cast<vtkArrayIteratorTemplate<vtkIdType>*>(data->NewIterator());
  for(vtkIdType i = 0; i < iter->GetNumberOfValues(); ++i)
  {
    std::cout << iter->GetValue(i) << std::endl;
  }
  iter->Delete();
}

int main(int argc, char const *argv[])
{
  // Load first core
  vtkSmartPointer<vtkDelimitedTextReader> reader =
    vtkSmartPointer<vtkDelimitedTextReader>::New();
  reader->SetFileName(argv[1]);
  reader->DetectNumericColumnsOn();
  reader->SetFieldDelimiterCharacters("\t");
  reader->Update();
  vtkTable* table = reader->GetOutput();

  // Load a second core
  vtkSmartPointer<vtkDelimitedTextReader> reader2 =
    vtkSmartPointer<vtkDelimitedTextReader>::New();
  reader2->SetFileName(argv[2]);
  reader2->DetectNumericColumnsOn();
  reader2->SetFieldDelimiterCharacters("\t");
  reader2->Update();
  vtkTable* table2 = reader2->GetOutput();

  // Set up the view
  vtkSmartPointer<vtkContextView> plotView =
    vtkSmartPointer<vtkContextView>::New();
  plotView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);

  // Add multiple line plots, setting the colors etc
  vtkSmartPointer<vtkChartXY> chart =
    vtkSmartPointer<vtkChartXY>::New();
  chart->GetAxis(vtkAxis::LEFT)->SetTitle("Delta 18O");
  chart->GetAxis(vtkAxis::BOTTOM)->SetTitle("Depth in Seabed");
  plotView->GetScene()->AddItem(chart);
  vtkPlot *line = chart->AddPlot(vtkChart::STACKED);
  line->SetInputData(table, 0, 1);
  line->SetColor(21, 72, 144, 200);
  line->SetWidth(2.0);

  vtkPlot* line2 = chart->AddPlot(vtkChart::STACKED);
  line2->SetInputData(table2, 0, 1);
  line2->SetColor(255, 102, 0, 200);
  line2->SetWidth(2.0);

  // Setup annotation link and requisite callbacks
  annotationLink = vtkSmartPointer<vtkAnnotationLink>::New();
  chart->SetAnnotationLink(annotationLink);
  vtkCallbackCommand* command = vtkCallbackCommand::New();
  command->SetCallback(selectionCallback);
  annotationLink->AddObserver(vtkCommand::AnnotationChangedEvent, command, 1.0);

  // Start interactor
  plotView->GetInteractor()->Initialize();
  plotView->GetInteractor()->Start();
}