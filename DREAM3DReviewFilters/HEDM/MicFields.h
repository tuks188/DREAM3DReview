/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
 * All rights reserved.
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <QtCore/QString>
#include <QtCore/QVector>

#include "EbsdLib/AbstractEbsdFields.h"
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdSetGetMacros.h"
#include "DREAM3DReview/DREAM3DReviewFilters/HEDM/MicConstants.h"

/**
 * @class MicFields MicFields.h EbsdLib/HEDM/MicFields.h
 * @brief This class simply holds the names of the columns that are present in the
 * HEDM .Mic file.
 *
 * @date Aug 18, 2011
 * @version 1.0
 */
class MicFields : public AbstractEbsdFields
{
public:
  MicFields();
  ~MicFields() override;

  QVector<QString> getFieldNames() override;

  template <typename T>
  T getFilterFeatures()
  {
    T features;

    features.push_back(Mic::Confidence);
    features.push_back(Mic::X);
    features.push_back(Mic::Y);

    return features;
  }

public:
  MicFields(const MicFields&) = delete;            // Copy Constructor Not Implemented
  MicFields(MicFields&&) = delete;                 // Move Constructor Not Implemented
  MicFields& operator=(const MicFields&) = delete; // Copy Assignment Not Implemented
  MicFields& operator=(MicFields&&) = delete;      // Move Assignment Not Implemented
};
