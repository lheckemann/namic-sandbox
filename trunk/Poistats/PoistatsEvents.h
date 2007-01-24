#ifndef POISTATSEVENTS_H_
#define POISTATSEVENTS_H_

#include <itkEventObject.h>

namespace itk
{

/** Store all poistats events here, so that the CommandUpdate can catch them */

itkEventMacro( PoistatsOdfCalculationStartEvent, ProgressEvent );
itkEventMacro( PoistatsOdfCalculationProgressEvent, ProgressEvent );
itkEventMacro( PoistatsOdfCalculationEndEvent, ProgressEvent );

}

#endif /*POISTATSEVENTS_H_*/
