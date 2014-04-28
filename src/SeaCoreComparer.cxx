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
#include <vtkBlockItem.h>
#include <vtkIdTypeArray.h>
#include <vtkRect.h>
#include <vtkVector.h>

vtkSmartPointer<vtkAnnotationLink> annotationLink;
vtkPlot *line;
vtkPlot *line2;

static void selectionCallback(vtkObject* obj, unsigned long, void*, void*)
{
  vtkArrayIteratorTemplate<vtkIdType>* iter
    = static_cast<vtkArrayIteratorTemplate<vtkIdType>*>(line->GetSelection()->NewIterator());
  for(vtkIdType i = 0; i < iter->GetNumberOfValues(); ++i)
  {
    std::cout << iter->GetValue(i) << std::endl;
  }
  iter->Delete();
  iter = NULL;
  std::cout << "***" << std::endl;
  iter
    = static_cast<vtkArrayIteratorTemplate<vtkIdType>*>(line2->GetSelection()->NewIterator());
  for(vtkIdType i = 0; i < iter->GetNumberOfValues(); ++i)
  {
    vtkIdType indx = iter->GetValue(i);
    std::cout << line2->GetInput()->GetValue(indx, 0) << ","
              << line2->GetInput()->GetValue(indx, 1) << std::endl;
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
  plotView->GetRenderWindow()->SetSize(800,800);
  chart->GetAxis(vtkAxis::LEFT)->SetTitle("Delta 18O (Climate Proxy)");
  chart->GetAxis(vtkAxis::BOTTOM)->SetTitle("Depth in Seabed (Time Proxy)");
  plotView->GetScene()->AddItem(chart);
  line = chart->AddPlot(vtkChart::STACKED);
  line->SetInputData(table, 0, 1);
  line->SetColor(21, 72, 144, 200);
  line->SetWidth(2.0);

  line2 = chart->AddPlot(vtkChart::STACKED);
  line2->SetInputData(table2, 0, 1);
  line2->SetColor(255, 102, 0, 200);
  line2->SetWidth(2.0);

  // Setup annotation link and requisite callbacks
  annotationLink = vtkSmartPointer<vtkAnnotationLink>::New();
  chart->SetAnnotationLink(annotationLink);
  vtkCallbackCommand* command = vtkCallbackCommand::New();
  command->SetCallback(selectionCallback);
  annotationLink->AddObserver(vtkCommand::AnnotationChangedEvent, command, 1.0);

  vtkSmartPointer<vtkBlockItem> blockTest =
    vtkSmartPointer<vtkBlockItem>::New();
  blockTest->SetDimensions(0, 0, 100, 100);

  vtkSmartPointer<vtkBlockItem> blockTest2 =
    vtkSmartPointer<vtkBlockItem>::New();
  blockTest2->SetDimensions(200, 200, 100, 100);

  plotView->GetScene()->AddItem(blockTest);
  plotView->GetScene()->AddItem(blockTest2);

  // Start interactor
  plotView->GetInteractor()->Initialize();
  plotView->GetInteractor()->Start();
}