/**
 * @file  QdecGlmDesign.h
 * @brief Contains the data and functions associated with a GLM design run
 *
 * Contains the data and functions associated with the selected GLM input
 * (design) parameters: selected discrete and continuous factors, measure,
 * hemi, smoothness level and design name.  Functions exist to create derived
 * data: .fsgd file, contrast vectors in .mat file format, and the 'y' input
 * data file (concatenated subjects file).
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

#ifndef QDECGLMDESIGN_H
#define QDECGLMDESIGN_H

#include <string>
#include <vector>

#include "QdecContrast.h"
#include "QdecDataTable.h"
#include "ProgressUpdateGUI.h"

#include "vtkQdec.h"

using namespace std;

class VTK_QDEC_EXPORT QdecGlmDesign
{
public:

  // Constructors/Destructors
  //

  QdecGlmDesign ( QdecDataTable* iDataTable );

  virtual ~QdecGlmDesign ( );

  /**
   * Returns true if this design is valid (input parameters have been set and
   * mri_glmfit input data created, ie. Create() has been called successfully)
   * @return bool
   */
  bool IsValid ( );


  /**
   * From the given design parameters, this creates the input data required by
   * mri_glmfit:
   *  - the 'y' data (concatenated subject volumes)
   *  - the FSGD file
   *  - the contrast vectors, as .mat files
   * and writes this data to the specified working directory.
   * @return int
   * @param  iDataTable
   * @param  isName
   * @param  isFirstDiscreteFactor
   * @param  isSecondDiscreteFactor
   * @param  isFirstContinuousFactor
   * @param  isSecondContinuousFactor
   * @param  isMeasure
   * @param  isHemi
   * @param  iSmoothnessLevel
   * @param  iProgressUpdateGUI
   */
  int Create ( QdecDataTable* iDataTable,
               const char* isName,
               const char* isFirstDiscreteFactor,
               const char* isSecondDiscreteFactor,
               const char* isFirstContinuousFactor,
               const char* isSecondContinuousFactor,
               const char* isMeasure,
               const char* isHemi,
               int iSmoothnessLevel,
               ProgressUpdateGUI* iProgressUpdateGUI );

  /**
   * @return string
   */
  string GetName ( );


  /**
   * @return string
   */
  string GetHemi ( );


  /**
   * @return string
   */
  string GetMeasure ( );


  /**
   * @return int
   */
  int GetSmoothness ( );


  /**
   * @return string
   */
  string GetSubjectsDir ( );


  /**
   * @param const char*
   */
  void SetSubjectsDir ( const char* ifnSubjectsDir );


  /**
   * @return string
   */
  string GetAverageSubject ( );


  /**
   * @param const char*
   */
  void SetAverageSubject ( const char* isAverageSubject );


  /**
   * returns the pathname to the fsgd file required by mri_glmfit.
   * @return string
   */
  string GetFsgdFileName ( );


  /**
   * returns the pathname to the input data, 'y', required by mri_glmfit.
   * @return string
   */
  string GetYdataFileName ( );


  /**
   * @return vector< string >
   */
  vector< string > GetContrastNames ( );


  /**
   * @return vector< string >
   */
  vector< string > GetContrastQuestions ( );


  /**
   * @return vector< string >
   */
  vector< string > GetContrastFileNames ( );


  /**
   * @return string
   */
  string GetWorkingDir ( );


  /**
   * @return int
   * @param  isPathName
   */
  int SetWorkingDir ( const char* isPathName );


  /**
   * @return ProgressUpdateGUI*
   */
  ProgressUpdateGUI* GetProgressUpdateGUI ( );

private:

  // private attributes
  //

  bool mbValid;
  QdecDataTable* mDataTable;
  string msName;
  // Stores seleted discrete factors.  Initially empty.
  vector< QdecFactor* > mDiscreteFactors;
  // Stores selected continous factors.  Initially empty.
  vector< QdecFactor* > mContinuousFactors;
  string msMeasure;
  string msHemi;
  int mSmoothness;
  string mfnSubjectsDir;
  string msAverageSubject;
  // Stores contrasts created from an fsgdf file. Can be empty.
  vector< QdecContrast* > mContrasts;
  string mfnFsgdfFile;
  string mfnYdataFile;
  string mfnWorkingDir;
  ProgressUpdateGUI* mProgressUpdateGUI;  

  // private methods
  //

  int GetNumberOfDiscreteFactors ( ) 
  { 
    return this->mDiscreteFactors.size(); 
  }
  int GetNumberOfContinuousFactors ( ) 
  { 
    return this->mContinuousFactors.size(); 
  }

  /**
   * GetNumberOfClasses( ) - returns the number of classes for the design.
   * The number of classes is just all the combinations of all
   * the levels for the discrete factors.
   */
  int GetNumberOfClasses ( );


  /**
   * GetNumberOfRegressors() - returns the number of regressors for the
   * given design.
   */
  int GetNumberOfRegressors( );


  /**
   * GetLevels2ClassName() - returns the class name given that
   * the 1st factor is at nthlevels[0],
   * the 2nd factor is at nthlevels[1], ...
   * The class name is created by appending
   *   Factor1NameLevelName-Factor2NameLevelName...
   */
  string GetLevels2ClassName ( unsigned int* nthlevels );


  /**
   * @return int
   */
  int WriteFsgdFile ( );


  /**
   * Creates Contrast objects based on the selected factors.
   * Stores them in our 'mContrasts' QdecContrast container.
   * @return int
   */
  int GenerateContrasts ( );


  /**
   * Creates the 'y' input data to mri_glmfit, by concatenating the subject
   * volumes, and writes it to the specified filename.
   * @return int
   */
  int WriteYdataFile ( );

};

#endif // QDECGLMDESIGN_H