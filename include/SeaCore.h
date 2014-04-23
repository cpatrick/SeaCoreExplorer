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
#ifndef __SeaCore_H_
#define __SeaCore_H_

#include <string>
#include <vector>

class QVariant;

/**
 * Container class for sea core data.
 */
class SeaCore
{
public:

  SeaCore() {};
  virtual ~SeaCore() {};

  std::string fileName;
  std::string publication;
  double latitude;
  double longitude;
  std::string proxy;
  double length;
  double averageSampling;
  double maxSampling;
  std::string datingMethod;
  std::string notes;

  void toString(std::string& output);
  void fromVector(const std::vector<std::string>& vec);
  QVariant getFieldByIndex(int index) const;

};

#endif
