/*************************************************************************
Copyright (c) 2007, Regents of the University of Iowa

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of The University of Iowa nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*************************************************************************/
#ifndef __UIIG_OBJECTUTILS_H__
#define __UIIG_OBJECTUTILS_H__

#include "objectentry.h"
#include "objectmap.h"
#include <uiigcolormap/ucrgba.h>
#include <uiigcolormap/frgba.h>
#include <uiigcore/image.h>

namespace uiig
{

  /**
   * \author Hans J. Johnson
   * \brief This function is used to make an Object map by tagging the
   * pixels from the input SourceImage into categories based on a
   * uniform partitioning of the SourceImage intensities between
   * the specified MinRange and MaxRange specifications.
   * \param CurrentMap The map to fill
   * \param SourceImage The image to be tagged
   * \param MinRange The value for the begining of the first partition
   * \param MaxRange The value for the end of the last partition
   * \param NumberOfPartitions The number of partitions to make, (Must be <= 250)
   * \return returns true if successful
   */
  bool MakeUniformObjectMap( uiig::CObjectMap & CurrentMap,
    const uiig::CImage<float>  & SourceImage,
    const float MinRange, const float MaxRange,
    const unsigned char NumberOfPartitions);

  /**
   * \author John Dill
   * \brief This function is used to make a Color Object map by tagging the
   * pixels from the input SourceImage into color categories based on
   * the order of the colors in the color vector.  First come first served.
   * \param CurrentMap The map to fill
   * \param SourceImage The image to be tagged
   * \param MinRange The value for the begining of the first color
   * \param MaxRange The value for the end of the last color
   * \param NumberOfPartitions The number of partitions to make, (Must be <= 250)
   * \param std::vector<unsigned char> rgbVector, the RGB values for the region to be colored
   * \return returns true if successful
   */
  bool MakeUniformColorObjectMap( uiig::CObjectMap & CurrentMap,
    const uiig::CImage<float>  & SourceImage,
    const float MinRange, const float MaxRange,
    const unsigned char NumberOfPartitions,
    const std::vector<unsigned char> rgbVector);

  /**
   * \author John Dill
   * \brief Makes a objectmap using the colors in a colormap with equally spaced bins
   * \par MakeUniformObjectmapFromColormap Description
   * This function is used to make a Color Object map by tagging the
   * pixels from the input SourceImage into color categories based on
   * the order of the colors in the color vector.  First come first served.
   * If the number of partitions and number of elements of the color vector are
   * different, the colormap is resampled to equal the number of partitions
   * to produce the color coded correspondence.  It doesn't interpolate between the
   * colors, similar to a nearest neighbor interpolation.  The colormap will
   * be converted to unsigned char automatically irrespective of what colormap
   * type you put into the function.
   * If the type is an unsigned char image, each graylevel is converted to a separate
   * objectmap.  If you wish to use data stored in an unsigned char image with regions
   * defined by bands of graylevels, convert the unsigned char image data to a floating
   * point image to get that effect.  It is best if the binary masks in the volume are
   * consecutive graylevels.  It essentially performs the same function as
   * ConvertImageToObjectMap.
   *
   * \param CurrentMap The map to fill
   * \param SourceImage<T> The image to be tagged of type T (unsigned char or float)
   * \param MinRange The value for the begining of the first color
   * \param MaxRange The value for the end of the last color
   * \param NumberOfPartitions The number of partitions to make, (Must be <= 250)
   * \param Colormap rgb colormap cmap, the RGB values for the region to be colored
   * \return returns true if successful
   */
  bool MakeUniformObjectmapFromColormap( uiig::CObjectMap & CurrentMap,
    const uiig::CImage<float> & SourceImage,
    const float MinRange,
    const float MaxRange,
    const unsigned char NumberOfPartitions,
    const std::vector<uiig::ucRGBA> & cmap );

  /**
   * \author John Dill
   * \brief Makes a objectmap using the colors in a colormap with logarithmically spaced bins
   * \par MakeLogscaleObjectmapFromColormap Description
   * This function is used to colorize a floating point image
   * with a logarithmic based scale for determining the ranges for which the colors are
   * applied. Ranges that start or end for numbers < 0 are errors, since the log of
   * negative numbers does not exist.
   * This essentially will map a range from min to max to
   * 1 and ( max + 1 - min ).  Eg 0 to 400 gets mapped to 1 and 401 before
   * applying the log scaling.  5 to 20 gets mapped to 1 to 16.  -30 to 30 gets
   * mapped to 1 and 61.  It's the range that is translated, not the values of
   * the floating point image.  The effect is that values closer to min get
   * smaller bins for a particular color while values closer to max get larger
   * bins for a color.
   * \param CurrentMap The map to fill
   * \param SourceImage The image to be tagged
   * \param MinRange The value for the begining of the first color
   * \param MaxRange The value for the end of the last color
   * \param NumberOfPartitions The number of partitions to make, (Must be <= 250)
   * \param Colormap<T> rgb colormap cmap, the RGB values for the region to be colored
   * \param StartLogScaleLow A flag to specify which end to have more color resolution
   * \return returns true if successful
   * \sa MakeUniformObjectmapFromColormap
   */
  bool MakeLogscaleObjectmapFromColormap( uiig::CObjectMap & CurrentMap,
    const uiig::CImage<float> & SourceImage,
    const float MinRange,
    const float MaxRange,
    const unsigned char NumberOfPartitions,
    const std::vector<uiig::ucRGBA> & cmap,
    bool StartLogScaleLow = true );

  /**
   * \author John Dill
   * \brief This function is used to convert an unsigned char image to an object map where
   * the voxel graylevel is representative of the object identifier from 0 to max object identifier,
   * assumes background is 0 and is not used to represent an object
   * \param CurrentMap The map to fill
   * \param SourceImage The image to be converted to an object map
   * \param MinimumObjectIdentifier The tag value for the beginning of the first object
   * \param MaximumObjectIdentifier The tag value for the end of the last object
   * \param std::vector<unsigned char> rgbVector, the RGB values for the region to be colored
   * \return returns true if successful
   */
  bool ConvertImageToObjectMap( uiig::CObjectMap & CurrentMap,
    const uiig::CImage<unsigned char> & SourceImage,
    const unsigned char MinimumObjectIdentifier,
    const unsigned char MaximumObjectIdentifier,
    const std::vector<unsigned char> rgbVector);

  /**
   * \author John Dill
   * \brief This function will set the StartColor relative to the EndColor of all
   * objects in an objectmap.  If value is invalid, ie not between 0 and 1, then 0.1
   * is the default percentage used in setting the start color values.
   * \param startColorPercent The percentage of end color to use from the start color
   * \return none
   */
  void SetStartColorPercent( uiig::CObjectMap & CurrentMap, float startColorPercent );

  /**
   * \author John Dill
   * \brief This function will calculate the color that Analyze renders in multisection 
   * tool (image slicer).  You can render any image using an objectmap as long as the 
   * image values are converted to an unsigned char scale.  Set the minimum and maximum
   * pixel values for the objects in the objectmap before rendering if applying a window.
   * This is extremely slow rendering if used for multiple pixels with the same objectmap.
   * Create a colormapper and then render using the colormapper for more speed and efficiency.
   * \param red the rendered red value
   * \param green the rendered green value
   * \param blue the rendered blue value
   * \param objmap the objectmap
   * \param grayvalue the grayvalue of the image element to render
   * \param xloc the x location of the image element to render
   * \param yloc the y location of the image element to render
   * \param zloc the z location of the image element to render
   * \param minimum_pixel_value the minimum graylevel used to render the color.  All grayvalues lower
   * than minimum_pixel_value are set to minimum_pixel_value
   * \param maximum_pixel_value the maximum graylevel used to render the color.  All grayvalues higher
   * than maximum_pixel_value are set to maximum_pixel_value
   */
  void GetAnalyzeMultiSectionRenderedColor( unsigned char& red, unsigned char& green, unsigned char& blue, const uiig::CObjectMap& objmap, const unsigned char gray, const int xloc, const int yloc, const int zloc, const unsigned char minimum_pixel_value = 0, const unsigned char maximum_pixel_value = 255 );

  /**
   * \brief This function will calculate the color that Analyze renders in multisection 
   * tool (image slicer).  You can render any image using an objectmap as long as the 
   * image values are converted to an unsigned char scale.  Set the minimum and maximum
   * pixel values for the objects in the objectmap when creating the colormapper before
   * rendering with the colormapper object.  This render operation is significantly faster
   * than the one which takes an objectmap because all colors for each grayvalue have been
   * precalculated and is a simple memory access operation.
   * \param red the rendered red value
   * \param green the rendered green value
   * \param blue the rendered blue value
   * \param colormapper the vector of colormaps that map a grayvalue to a color
   * \param grayvalue the grayvalue of the image element to render
   * \param object_index the object corresponding to the gray value
   */
  void GetAnalyzeMultiSectionRenderedColor( unsigned char& red, unsigned char& green, unsigned char& blue, const std::vector< std::vector<uiig::RGBTRIPLE> >& colormapper, const unsigned char gray, const unsigned char object_index );

  /**
   * \brief This function will calculate the color that Analyze renders in multisection 
   * tool (image slicer).  You can render any image using an objectmap as long as the 
   * image values are converted to an unsigned char scale.  Set the minimum and maximum
   * pixel values for the objects in the objectmap when creating the colormapper before
   * rendering with the colormapper object.  This render operation is significantly faster
   * than the one which takes an objectmap because all colors for each grayvalue have been
   * precalculated and is a simple memory access operation.
   * \param rgb the rgb color of the rendered pixel
   * \param colormapper the vector of colormaps that map a grayvalue to a color
   * \param grayvalue the grayvalue of the image element to render
   * \param object_index the object corresponding to the gray value
   */
  void GetAnalyzeMultiSectionRenderedColor( uiig::RGBTRIPLE& rgb, const std::vector< std::vector<uiig::RGBTRIPLE> >& colormapper, const unsigned char gray, const unsigned char object_index );

  /**
   * \author John Dill
   * \brief This function will build a colormapper for an objectmap.  For each object, there is
   * a vector of 256 RGBTRIPLE colors where the graylevel will be the index to the color.  This
   * is created for each object in the objectmap.
   * \param colormapper the vector of colormap vectors
   * \param objmap the objectmap
   * \param minimum_pixel_value the minimum graylevel used to render the color.  All grayvalues lower
   * than minimum_pixel_value are set to minimum_pixel_value
   * \param maximum_pixel_value the maximum graylevel used to render the color.  All grayvalues higher
   * than maximum_pixel_value are set to maximum_pixel_value
   */
  void CreateObjectMapColorMapper( std::vector< std::vector<uiig::RGBTRIPLE> >& colormapper, const uiig::CObjectMap& objmap, const unsigned char minimum_pixel_value = 0, const unsigned char maximum_pixel_value = 255 );

  /**
   * \author Hans J. Johnson
   * \brief This function is used to print all the values of an ObjectEntry to the screen
   * \param rhs The ObjectEntry to print
   */
  void PrintObjectEntry( const uiig::CObjectEntry & rhs );

}                                // end namespace uiig
#endif                           // __UIIG_OBJECTUTILS_H_
