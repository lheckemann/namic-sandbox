/**
 * @file  QdecProject.h
 * @brief API class containing all qdec subject data and methods
 *
 * Top-level interface class containing all data associated with a users
 * subject group, and potentially mri_glmfit processed data associated with
 * that group.
 */
/*
 * Original Author: Nick Schmansky
 * CVS Revision Info:
 *    $Author: nicks $
 *    $Date: 2007/05/23 21:20:58 $
 *    $Revision: 1.5 $
 *
 * Copyright (C) 2007,
 * The General Hospital Corporation (Boston, MA).
 * All rights reserved.
 *
 * Distribution, usage and copying of this software is covered under the
 * terms found in the License Agreement file named 'COPYING' found in the
 * FreeSurfer source code root directory, and duplicated here:
 * https://surfer.nmr.mgh.harvard.edu/fswiki/FreeSurferOpenSourceLicense
 *
 * General inquiries: freesurfer@nmr.mgh.harvard.edu
 * Bug reports: analysis-bugs@nmr.mgh.harvard.edu
 *
 */

#ifndef QDECPROJECT_H
#define QDECPROJECT_H

#include <string>
#include <vector>

#include "QdecDataTable.h"
#include "QdecGlmDesign.h"
#include "QdecGlmFit.h"
#include "QdecGlmFitResults.h"
#include "ProgressUpdateGUI.h"

#include "vtkQdec.h"

using namespace std;

class VTK_QDEC_EXPORT QdecProject
{
public:

  // Constructors/Destructors
  //

  QdecProject ( );

  virtual ~QdecProject ( );

  /**
   * Load a project file (containing all necessary info to begin working
   * either on a new project, or to continue working using results from a
   * prior saved work session).
   * @return int
   * @param  isFileName
   */
  int LoadProjectFile ( const char* isFileName );


  /**
   * Save all necessary information pertaining to this project
   * (all subject data, any results, any user preferences).
   * @return int
   * @param  isFileName
   */
  int SaveProjectFile ( const char* isFileName );


  /**
   * @return int
   * @param  isFileName
   */
  int LoadDataTable ( const char* isFileName );

  /**
   * @return void
   * @param  iFilePointer
   */
  void DumpDataTable ( FILE* iFilePointer );

  /**
   * @return int
   * @param  isFileName
   */
  int SaveDataTable ( const char* isFileName );


  /**
   * @return QdecDataTable*
   */
  QdecDataTable* GetDataTable ( );


  /**
   * @return string
   */
  string GetSubjectsDir ( );


  /**
   * @param  ifnSubjectsDir
   */
  void SetSubjectsDir ( const char* ifnSubjectsDir );


  /**
   * @return string
   */
  string GetAverageSubject ( );


  /**
   * @param  isSubjectName
   */
  void SetAverageSubject ( const char* isSubjectName );


  /**
   * @return string
   */
  string GetWorkingDir ( );


  /**
   * @return 0 if ok, 1 on error
   * @param  isWorkingDir
   */
  int SetWorkingDir ( const char* isWorkingDir );


  /**
   * @return vector< string >
   */
  vector< string > GetSubjectIDs ( );


  /**
   * @return vector< string >
   */
  vector< string > GetDiscreteFactors ( );


  /**
   * @return vector< string >
   */
  vector< string > GetContinousFactors ( );


  /**
   * @return string
   */
  string GetHemi ( );


  /**
   * From the given design parameters, this creates the input data required by
   * mri_glmfit:
   *  - the 'y' data (concatenated subject volumes)
   *  - the FSGD file
   *  - the contrast vectors, as .mat files
   * and writes this data to the specified working directory.
   * @return int
   * @param  isName
   * @param  isFirstDiscreteFactor
   * @param  isSecondDiscreteFactor
   * @param  isFirstContinuousFactor
   * @param  isSecondContinuousFactor
   * @param  isMeasure
   * @param  isHemi
   * @param  iSmoothnessLevel
   * @param  iProgressUpdateGUI (optional)
   */
  int CreateGlmDesign ( const char* isName,
                        const char* isFirstDiscreteFactor,
                        const char* isSecondDiscreteFactor,
                        const char* isFirstContinuousFactor,
                        const char* isSecondContinuousFactor,
                        const char* isMeasure,
                        const char* isHemi,
                        int iSmoothnessLevel,
                        ProgressUpdateGUI* iProgressUpdateGUI=NULL );


  /**
   * @return int
   */
  int RunGlmFit ( );


  /**
   * @return QdecGlmFitResults
   */
  QdecGlmFitResults* GetGlmFitResults ( );

  /**
   * Run mri_label2label on each subject, mapping the label that was drawn on 
   * the average surface to each subject. Optionally supply a window manager
   * to allow posting progress info
   * @return int
   * @param  ifnLabel
   * @param  iProgressUpdateGUI (optional)
   */
  int GenerateMappedLabelForAllSubjects 
    ( const char* ifnLabel,
      ProgressUpdateGUI* iProgressUpdateGUI=NULL );


private:

  // private attributes
  //

  string mfnProjectFile;
  QdecDataTable* mDataTable;
  QdecGlmDesign* mGlmDesign;
  QdecGlmFit* mGlmFitter;

};

#endif // QDECPROJECT_H
