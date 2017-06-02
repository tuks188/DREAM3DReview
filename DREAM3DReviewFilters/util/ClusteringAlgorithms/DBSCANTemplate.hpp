/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _dbscantemplate_h_
#define _dbscantemplate_h_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "DREAM3DReview/DREAM3DReviewFilters/util/DistanceTemplate.hpp"

template<typename T>
class DBSCANTemplate
{
public:
  SIMPL_SHARED_POINTERS(DBSCANTemplate)
  SIMPL_TYPE_MACRO(DBSCANTemplate)

  DBSCANTemplate() {}
  virtual ~DBSCANTemplate() {}

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  bool operator()(IDataArray::Pointer p)
  {
    return (std::dynamic_pointer_cast<DataArray<T>>(p) != nullptr);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Execute(AbstractFilter* filter, IDataArray::Pointer inputIDataArray, BoolArrayType::Pointer maskDataArray, 
               Int32ArrayType::Pointer fIds, float epsilon, int32_t minPnts, int32_t distMetric)
  {
    typename DataArray<T>::Pointer inputDataPtr = std::dynamic_pointer_cast<DataArray<T>>(inputIDataArray);
    T* inputData = inputDataPtr->getPointer(0);
    int32_t* fPtr = fIds->getPointer(0);

    size_t numTuples = inputDataPtr->getNumberOfTuples();
    size_t numCompDims = inputDataPtr->getNumberOfComponents();
    std::vector<bool> visited(numTuples, false);
    std::vector<bool> clustered(numTuples, false);

    //double dist = 0.0;
    double minDist = static_cast<double>(epsilon);
    int32_t cluster = 0;
    bool* mask = maskDataArray->getPointer(0);

    int64_t progIncrement = static_cast<int64_t>(numTuples / 100);
    int64_t prog = 1;
    int64_t progressInt = 0;
    int64_t counter = 0;

    for(size_t i = 0; i < numTuples; i++)
    {
      if(filter->getCancel()) { return; }
      if(mask[i] && !visited[i])
      {
        visited[i] = true;

        if(counter > prog)
        {
          progressInt = static_cast<int64_t>((static_cast<float>(counter) / numTuples) * 100.0f);
          QString ss = QObject::tr("Scanning Data || Visited Point %1 of %2 || %3% Completed").arg(counter).arg(numTuples).arg(progressInt);
          filter->notifyStatusMessage(filter->getMessagePrefix(), filter->getHumanLabel(), ss);
          prog = prog + progIncrement;
        }
        counter++;

        std::list<size_t> neighbors = epsilon_neighbors(filter, minDist, i, inputData, mask, 
                                                        numCompDims, numTuples, distMetric);

        if(static_cast<int32_t>(neighbors.size()) < minPnts)
        {
          fPtr[i] = 0;
          clustered[i] = true;
        }
        else
        {
          cluster++;
          expand_cluster(filter, neighbors, fPtr, cluster, minPnts, visited, clustered,
                         minDist, i, inputData, mask,
                         numCompDims, numTuples, distMetric,
                         progIncrement, prog, progressInt, counter);
        }
      }
    }
  }

private:
  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  std::list<size_t> epsilon_neighbors(AbstractFilter* filter, double eps, size_t index, T* data, bool* mask, 
                                      size_t dims, size_t numTuples, int32_t metric)
  {
    std::list<size_t> neighbors;

    for(size_t i = 0; i < numTuples; i++)
    {
      if(filter->getCancel()) { return std::list<size_t>(); }
      if(mask[i])
      {
        double dist = DistanceTemplate::GetDistance<T, T, double>(data + (dims * index), 
                                                                  data + (dims * i),
                                                                  dims,
                                                                  metric);
        if(dist < eps) { neighbors.push_back(i); }
      }
    }

    return neighbors;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void expand_cluster(AbstractFilter* filter, std::list<size_t>& neighbors, int32_t* features, int32_t cluster,
                      int32_t minPnts, std::vector<bool>& visited, std::vector<bool>& clustered,
                      double eps, size_t index, T* data, bool* mask,
                      size_t dims, size_t numTuples, int32_t metric,
                      int64_t& progIncrement, int64_t& prog, int64_t& progressInt, int64_t& counter)
  {
    features[index] = cluster;
    clustered[index] = true;

    for(auto&& idx : neighbors)
    {
      if(filter->getCancel()) { return; }
      if(mask[idx])
      {
        if(!visited[idx])
        {
          visited[idx] = true;

          if(counter > prog)
          {
            progressInt = static_cast<int64_t>((static_cast<float>(counter) / numTuples) * 100.0f);
            QString ss = QObject::tr("Scanning Data || Visited Point %1 of %2 || %3% Completed").arg(counter).arg(numTuples).arg(progressInt);
            filter->notifyStatusMessage(filter->getMessagePrefix(), filter->getHumanLabel(), ss);
            prog = prog + progIncrement;
          }
          counter++;

          std::list<size_t> neighbors_prime = epsilon_neighbors(filter, eps, idx, data, mask,
                                                                dims, numTuples, metric);
          if(static_cast<int32_t>(neighbors_prime.size()) >= minPnts)
          {
            neighbors.splice(std::end(neighbors), neighbors_prime);
          }
        }
        if(!clustered[idx])
        {
          features[idx] = cluster;
          clustered[idx] = true;
        }
      }
    }
  }
  
  DBSCANTemplate(const DBSCANTemplate&); // Copy Constructor Not Implemented
  void operator=(const DBSCANTemplate&); // Operator '=' Not Implemented
};

#endif /* _dbscantemplate_h_ */
