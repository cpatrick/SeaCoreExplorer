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

#include <sstream>
#include <iostream>
#include <limits>

#include <boost/lexical_cast.hpp>

#include <QDebug>

#include "SeaCore.h"

void SeaCore::toString(std::string& output)
{
  std::stringstream ss;
  ss << "File Name: " << fileName << "\n"
     << "Publication: " << publication << "\n"
     << "latitude: " << latitude << "\n"
     << "longitude: " << longitude << "\n"
     << "proxy: " << proxy << "\n"
     << "length: " << length << "\n"
     << "Average Sampling: " << averageSampling << "\n"
     << "Max Sampling: " << maxSampling << "\n"
     << "Dating Method: " << datingMethod << "\n"
     << "Notes: " << notes << "\n";

  output = ss.str();
}

void SeaCore::fromVector(const std::vector<std::string>& vec)
{
  this->fileName = std::string(vec[0]);
  this->publication = std::string(vec[1]);

  try
  {
    this->latitude = boost::lexical_cast<double>(vec[2]);
  }
  catch( boost::bad_lexical_cast )
  {
    qDebug() << "Invalid latitude for core " << vec[0].c_str() << ": \""
             << vec[2].c_str() << "\"";
    this->latitude = std::numeric_limits<double>::signaling_NaN();
  }

  try
  {
    this->longitude = boost::lexical_cast<double>(vec[3]);
  }
  catch( boost::bad_lexical_cast )
  {
    qDebug() << "Invalid longitude for core " << vec[0].c_str() << ": \""
             << vec[3].c_str() << "\"";
    this->longitude = std::numeric_limits<double>::signaling_NaN();
  }

  this->proxy = std::string(vec[4]);

  try
  {
    this->length = boost::lexical_cast<double>(vec[5]);
  }
  catch( boost::bad_lexical_cast )
  {
    qDebug() << "Invalid length for core " << vec[0].c_str() << ": \""
             << vec[5].c_str() << "\"";
    this->length = std::numeric_limits<double>::signaling_NaN();
  }

  try
  {
    this->averageSampling = boost::lexical_cast<double>(vec[6]);
  }
  catch( boost::bad_lexical_cast )
  {
    qDebug() << "Invalid Average Sampling for core " << vec[0].c_str()
             << ": \"" << vec[6].c_str() << "\"";
    this->averageSampling = std::numeric_limits<double>::signaling_NaN();
  }

  try
  {
    this->maxSampling = boost::lexical_cast<double>(vec[7]);
  }
  catch( boost::bad_lexical_cast )
  {
    qDebug() << "Invalid Max Sampling for core " << vec[0].c_str() << ": \""
             << vec[7].c_str() << "\"";
    this->maxSampling = std::numeric_limits<double>::signaling_NaN();
  }
  this->datingMethod = std::string(vec[8]);
  this->notes = std::string(vec[9]);
}

QVariant
SeaCore
::getFieldByIndex(int index) const
{
  switch(index)
  {
    case 0:
      return QVariant(QString(this->fileName.c_str()));
    case 1:
      return QVariant(QString(this->publication.c_str()));
    case 2:
      return QVariant(this->latitude);
    case 3:
      return QVariant(this->longitude);
    case 4:
      return QVariant(QString(this->proxy.c_str()));
    case 5:
      return QVariant(this->length);
    case 6:
      return QVariant(this->averageSampling);
    case 7:
      return QVariant(this->maxSampling);
    case 8:
      return QVariant(QString(this->datingMethod.c_str()));
    case 9:
      return QVariant(QString(this->notes.c_str()));
    default:
      qDebug() << "Invalid field index.";
      return QVariant(QString(""));
  }
}
