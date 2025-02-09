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

#include <hdf5.h>

#include <QtCore/QSet>
#include <QtCore/QVector>

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdSetGetMacros.h"

#include "MicPhase.h"
#include "MicReader.h"

/**
 * @class H5MicReader H5MicReader.h EbsdLib/HEDM/H5MicReader.h
 * @brief
 *
 * @date Mar 24, 2011
 * @version 1.0
 *
 * HEDM OIM Scans are stored in the HDF5 file into a heirarchy of folders where
 * each folder stores an individual .Mic file. The folder has a simple index as
 * its name, for example if the index of the .Mic file was 11 then the HDF folder
 * that contains that .Mic file's data has an HDF Group name of "11". Within this
 * folder there are 2 more folders: <b>Data</b> and <b>Header</b>.\n
 *   The <b>Data</b> folder contains all the columner data from the .Mic file where
 * each column is its own HDF data set. The <b>Header</b> folder contains the complete
 * header data from the .Mic file as either data sets or as subfolders. The dataset
 * with the name <b>OriginalFile</b> contains the unchMiced complete header from
 * the .Mic file in a single string HDF data set. Each of the header keys has been
 * also saved to its own HDF data set which can be seen with any HDF5 data viewer
 * application. For the Phase data inside the header a new HDF group is created
 * called <b>Phases</b> which contains each of the phase data. Again each phase
 * has an HDF5 group where its data is collected with a simple numerical index
 * as its name. Within each Phase's HDF Group folder are the various data sets
 * that make up the Phase data. They are:
 * @li Phase
 * @li MaterialName
 * @li Formula
 * @li Info
 * @li Symmetry
 * @li LatticeConstants
 * @li NumberHKLFamilies
 *
 * The HKLFamilies form another sub hdf group within the Phase group. This folder
 * contains datasets where each data set holds a single HKLFamily from the Phase
 * data. The dataset is an HDF5 Compound Data Type where you have the following
 * layout
 * @code
 * typedef struct
{
    int h;
    int k;
    int l;
    int s1;
    float diffractionIntensity;
    int s2;
} HKLFamily_t;
 * @endcode
 *
 * After reading the HDF5 file one can use the various methods to extract the phases
 * or other header information or all the columnar data.
 */
class H5MicReader : public MicReader
{
public:
  EBSD_SHARED_POINTERS(H5MicReader)
  EBSD_STATIC_NEW_MACRO(H5MicReader)
  EBSD_TYPE_MACRO(H5MicReader)
  ~H5MicReader() override;

  /**
   * @brief The HDF5 path to find the EBSD data
   */
  EBSD_INSTANCE_STRING_PROPERTY(HDF5Path)

  /**
   * @brief Reads the file
   * @return error condition
   */
  int readFile() override;

  /**
   * @brief Reads the header section of the file
   * @param Valid HDF5 Group ID
   * @return error condition
   */
  int readHeader(hid_t parId);

  /**
   * @brief Reads ONLY the header portion of the HEDM .Mic file
   * @return 1 on success
   */
  int readHeaderOnly() override;

  /**
   * @brief Returns a vector of MicPhase objects corresponding to the phases
   * present in the file
   */
  QVector<MicPhase::Pointer> getPhases()
  {
    return m_Phases;
  }

  /**
   * @brief Sets the names of the arrays to read out of the file
   * @param names
   */
  void setArraysToRead(QSet<QString> names);

  /**
   * @brief Over rides the setArraysToReads to tell the reader to load ALL the data from the HDF5 file. If the
   * ArrayNames to read is empty and this is true then all arrays will be read.
   * @param b
   */
  void readAllArrays(bool b);

protected:
  H5MicReader();

  /**
   * @brief Reads the data section of the file
   * @param Valid HDF5 Group ID
   * @return error condition
   */
  int readData(hid_t parId);

private:
  QVector<MicPhase::Pointer> m_Phases;
  QSet<QString> m_ArrayNames;
  bool m_ReadAllArrays;

public:
  H5MicReader(const H5MicReader&) = delete;            // Copy Constructor Not Implemented
  H5MicReader(H5MicReader&&) = delete;                 // Move Constructor Not Implemented
  H5MicReader& operator=(const H5MicReader&) = delete; // Copy Assignment Not Implemented
  H5MicReader& operator=(H5MicReader&&) = delete;      // Move Assignment Not Implemented
};
