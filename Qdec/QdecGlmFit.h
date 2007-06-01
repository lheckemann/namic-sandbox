/**
 * @file  QdecGlmFit.h
 * @brief Wrapper for mri_glmfit.
 *
 * Run mri_glmfit, given its input data, and puts the output in the specified
 * working directory.
 */
/*
 * Original Author: Nick Schmansky
 * CVS Revision Info:
 *    $Author: nicks $
 *    $Date: 2007/05/20 23:15:23 $
 *    $Revision: 1.2 $
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

#ifndef QDECGLMFIT_H
#define QDECGLMFIT_H

#include <string>
#include <vector>

#include "QdecGlmDesign.h"
#include "QdecGlmFitResults.h"

using namespace std;

class QdecGlmFit
{
public:

  // Constructors/Destructors
  //

  QdecGlmFit ( );

  virtual ~QdecGlmFit ( );

  // public attribute accessor methods
  //


  /**
   * @return int
   * @param  iGlmDesign
   */
  int Run (QdecGlmDesign* iGlmDesign );


  /**
   * @return QdecGlmFitResults
   */
  QdecGlmFitResults* GetResults ( );

private:

  // private attributes
  //

  QdecGlmFitResults* mGlmFitResults;

};

#endif // QDECGLMFIT_H
