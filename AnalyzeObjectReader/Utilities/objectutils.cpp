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
//
// Name: objectutils.cpp
// Author: John Dill
// Date: 5/18/00
//

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <uiigcolormap/frgba.h>
#include <uiigcolormap/ucrgba.h>
#include <uiigcolormap/colorutils.h>
#include "objectutils.h"

bool uiig::MakeUniformObjectMap( uiig::CObjectMap & CurrentMap, const uiig::CImage<float> & SourceImage,
const float MinRange, const float MaxRange,
const unsigned char NumberOfPartitions)
{
  assert(NumberOfPartitions != 0);
  assert(NumberOfPartitions <= 250);
  assert(MinRange <= MaxRange);

  std::vector<float> RangeSettings(NumberOfPartitions+1);

  const float PartitionSize=(MaxRange-MinRange)/static_cast<float>(NumberOfPartitions);
  RangeSettings[0]=MinRange;
  {
    for(unsigned char i=1; i <RangeSettings.size(); i++)
    {
      RangeSettings[i]=MinRange+static_cast<float>(i)*PartitionSize;
    }
  }
  const unsigned char MinGrey=63;
  const unsigned char MaxGrey=255;
  unsigned char GreyPartitionSize=(MaxGrey-MinGrey)/NumberOfPartitions;

  unsigned int NumberOfShades = 255 / NumberOfPartitions;
  if (NumberOfShades > 250)
  {
    NumberOfShades = 250;
  }

  GreyPartitionSize=(GreyPartitionSize>0)?GreyPartitionSize:1;
  {
    for(unsigned char i=1; i <RangeSettings.size(); i++)
    {
      char objectname[32];
      sprintf(objectname,"Range%5.5g_%5.5g",RangeSettings[i-1],RangeSettings[i]);
      const unsigned char CurrentGreyColor=MinGrey+i*GreyPartitionSize;
      CurrentMap.AddObjectInRange(SourceImage, RangeSettings[i-1],
        RangeSettings[i],objectname,
        CurrentGreyColor, CurrentGreyColor,
        CurrentGreyColor, NumberOfShades, true);
    }
  }
  return true;
}


bool uiig::MakeUniformColorObjectMap( uiig::CObjectMap & CurrentMap, const uiig::CImage<float> & SourceImage,
const float MinRange, const float MaxRange,
const unsigned char NumberOfPartitions,
const std::vector<unsigned char> rgbVector)
{
  assert(NumberOfPartitions != 0);
  assert(NumberOfPartitions <= 250);
  assert( 3 * static_cast<unsigned int>(NumberOfPartitions) <= rgbVector.size() );
  assert(MinRange <= MaxRange);

  std::vector<float> RangeSettings(NumberOfPartitions+1);

  const float PartitionSize=(MaxRange-MinRange)/static_cast<float>(NumberOfPartitions);
  RangeSettings[0]=MinRange;
  {
    for(unsigned char i=1; i <RangeSettings.size(); i++)
    {
      RangeSettings[i]=MinRange+static_cast<float>(i)*PartitionSize;
    }
  }

  unsigned int NumberOfShades = 255 / NumberOfPartitions;
  if (NumberOfShades > 250)
  {
    NumberOfShades = 250;
  }

  {
    for(unsigned char i=1; i < RangeSettings.size(); i++)
    {
      char objectname[32];
      sprintf(objectname,"Range%5.5g_%5.5g",RangeSettings[i-1],RangeSettings[i]);
      CurrentMap.AddObjectInRange(SourceImage, RangeSettings[i-1],
        RangeSettings[i],objectname,
        rgbVector[(i-1)*3],rgbVector[(i-1)*3+1],
        rgbVector[(i-1)*3+2], NumberOfShades, true);
    }
  }
  return true;
}


bool uiig::MakeUniformObjectmapFromColormap( uiig::CObjectMap & CurrentMap,
const uiig::CImage<float> & SourceImage,
const float MinRange,
const float MaxRange,
const unsigned char NumberOfPartitions,
const std::vector<uiig::ucRGBA> & cmap )
{
  assert(NumberOfPartitions != 0);
  assert(NumberOfPartitions <= 250);
  assert(MinRange <= MaxRange);

  std::vector<uiig::ucRGBA> resample_cmap;

  // Resampling the colormap to the number of partitions
  if ( cmap.size() != NumberOfPartitions )
  {
    uiig::CreateCustomColormap( resample_cmap, cmap, NumberOfPartitions );
  }
  else
  {
    resample_cmap = cmap;
  }

  std::vector<float> RangeSettings(NumberOfPartitions+1);

  const float PartitionSize=(static_cast<float>(MaxRange-MinRange))/static_cast<float>(NumberOfPartitions);
  RangeSettings[0]=MinRange;
  {
    for(unsigned char i=1; i <RangeSettings.size(); i++)
    {
      RangeSettings[i]=static_cast<float>(MinRange) + (static_cast<float>(i)*PartitionSize);
    }
  }

  unsigned int NumberOfShades = 255 / NumberOfPartitions;
  if (NumberOfShades > 250)
  {
    NumberOfShades = 250;
  }

  {
    for(unsigned char i=1; i < RangeSettings.size(); i++)
    {
      char objectname[32];
      sprintf(objectname,"Range%5.5g_%5.5g",RangeSettings[i-1],RangeSettings[i]);
      CurrentMap.AddObjectInRange(SourceImage, RangeSettings[i-1],
        RangeSettings[i],objectname,
        resample_cmap[i-1].getR(), resample_cmap[i-1].getG(),
        resample_cmap[i-1].getB(), NumberOfShades, true);
    }
  }
  return true;
}


bool uiig::MakeLogscaleObjectmapFromColormap( uiig::CObjectMap & CurrentMap,
const uiig::CImage<float> & SourceImage, const float MinRange, const float MaxRange,
const unsigned char NumberOfPartitions, const std::vector<uiig::ucRGBA> & cmap, bool StartLogScaleLow )
{
  assert(NumberOfPartitions != 0);
  assert(NumberOfPartitions <= 250);
  assert(MinRange <= MaxRange);

  std::vector<uiig::ucRGBA> resample_cmap;

  // Resampling the colormap to the number of partitions
  if ( cmap.size() != NumberOfPartitions )
  {
    uiig::CreateCustomColormap( resample_cmap, cmap, NumberOfPartitions );
  }
  else
  {
    resample_cmap = cmap;
  }

  std::vector<float> RangeSettings(NumberOfPartitions+1);
  std::vector<float> LogRangeSettings(NumberOfPartitions+1);

  float NewMinRange = 1.0F;
  float NewMaxRange = static_cast<float>(MaxRange - MinRange) + 1.0F;
  //std::cout << "NewMinRange = " << NewMinRange << std::endl;
  //std::cout << "NewMaxRange = " << NewMaxRange << std::endl;

  float LogMinRange = log( NewMinRange );
  float LogMaxRange = log( NewMaxRange );
  //std::cout << "LogMinRange = " << LogMinRange << std::endl;
  //std::cout << "LogMaxRange = " << LogMaxRange << std::endl;

  if ( StartLogScaleLow )
  {
    const float LogPartitionSize=(static_cast<float>(LogMaxRange-LogMinRange))/static_cast<float>(NumberOfPartitions);
    LogRangeSettings[0]=LogMinRange;
    RangeSettings[0]=MinRange;

    {
      for(unsigned int i=1; i < LogRangeSettings.size(); i++)
      {
        LogRangeSettings[i]=static_cast<float>(LogMinRange) + (static_cast<float>(i)*LogPartitionSize);
        RangeSettings[i]=RangeSettings[i-1] + ( exp(LogRangeSettings[i]) - exp(LogRangeSettings[i-1]) );
        //std::cout << "LogRangeSettings[" << i << "] = " << LogRangeSettings[i] << "..." << std::endl;
        //std::cout << "RangeSettings[" << i << "] = " << RangeSettings[i] << "..." << std::endl;
      }
    }
  }
  else
  {
    const float LogPartitionSize=(static_cast<float>(LogMaxRange-LogMinRange))/static_cast<float>(NumberOfPartitions);
    LogRangeSettings[0]=LogMinRange;
    RangeSettings[NumberOfPartitions]=MaxRange;

    {
      for(unsigned int i=1; i < LogRangeSettings.size(); i++)
      {
        LogRangeSettings[i]=static_cast<float>(LogMinRange) + (static_cast<float>(i)*LogPartitionSize);
        //std::cout << "LogRangeSettings[" << i << "] = " << LogRangeSettings[i] << "..." << std::endl;
      }

      for(unsigned int i=1; i < LogRangeSettings.size(); i++)
      {
        RangeSettings[NumberOfPartitions-i]=RangeSettings[NumberOfPartitions-i+1] - ( exp(LogRangeSettings[i]) - exp(LogRangeSettings[i-1]) );
        //std::cout << "RangeSettings[" << NumberOfPartitions-i << "] = " << RangeSettings[NumberOfPartitions-i] << "..." << std::endl;
      }
    }
  }

  unsigned int NumberOfShades = 255 / NumberOfPartitions;
  if (NumberOfShades > 250)
  {
    NumberOfShades = 250;
  }

  {
    for(unsigned char i=1; i < RangeSettings.size(); i++)
    {
      char objectname[32];
      sprintf(objectname,"Range%5.5g_%5.5g",RangeSettings[i-1],RangeSettings[i]);
      CurrentMap.AddObjectInRange(SourceImage, RangeSettings[i-1],
        RangeSettings[i],objectname,
        resample_cmap[i-1].getR(), resample_cmap[i-1].getG(),
        resample_cmap[i-1].getB(), NumberOfShades, true);
    }
  }
  return true;
}


bool uiig::ConvertImageToObjectMap( uiig::CObjectMap & CurrentMap,
const uiig::CImage<unsigned char> & SourceImage,
const unsigned char MinimumObjectIdentifier,
const unsigned char MaximumObjectIdentifier,
const std::vector<unsigned char> rgbVector)
{
                                 // 0 is background
  assert(MinimumObjectIdentifier != 0);
  assert(MinimumObjectIdentifier <= MaximumObjectIdentifier );
  assert( 3 * static_cast<unsigned int>(MaximumObjectIdentifier - MinimumObjectIdentifier + 1) <= rgbVector.size() );
  assert((MaximumObjectIdentifier - MinimumObjectIdentifier + 1) <= 250 );
  std::string s("Object ");

  int j = 0;

  // This algorithm will split each voxel graylevel value or tag and convert it to an object
  for (int i = MinimumObjectIdentifier; i <= MaximumObjectIdentifier; i++)
  {
    std::string objectName = s + (char)(i + (int)'0');
    CurrentMap.AddObjectInRange(SourceImage, i, i, objectName, rgbVector[3*j], rgbVector[3*j+1],
      rgbVector[3*j+2], 1, true);
    j += 1;
  }

  CurrentMap.EvenlyShade();

  return true;
}


void uiig::SetStartColorPercent( uiig::CObjectMap & CurrentMap, float startColorPercent )
{
  float startPercent;
  if ( startColorPercent < 0.0F || startColorPercent > 1.0F )
  {
    startPercent = 0.1F;
  }
  else
  {
    startPercent = startColorPercent;
  }

  for ( int i = 0; i < CurrentMap.getNumberOfObjects(); i++ )
  {
    CurrentMap.getObjectEntry( i ).setStartRed( static_cast<int>( startPercent * CurrentMap.getObjectEntry( i ).getEndRed() ) );
    CurrentMap.getObjectEntry( i ).setStartGreen( static_cast<int>( startPercent * CurrentMap.getObjectEntry( i ).getEndGreen() ) );
    CurrentMap.getObjectEntry( i ).setStartBlue( static_cast<int>( startPercent * CurrentMap.getObjectEntry( i ).getEndBlue() ) );
  }
}

void uiig::GetAnalyzeMultiSectionRenderedColor( unsigned char& red, unsigned char& green, unsigned char& blue, const uiig::CObjectMap& objmap, const unsigned char gray, const int xloc, const int yloc, const int zloc, const unsigned char minimum_pixel_value, const unsigned char maximum_pixel_value )
{

  // Analyze ObjectMap Rendering Algorithm (Proof of Principal, EXTREMELY slow)
  //
  // start color         <-------------- Number of Shades = 4 --------------->                  end color
  //      -------------------------------------------------------------------------------------------
  //      |                          |                             |                               ||
  //      |                          |                             |                               ||
  //      |                          |                            1|1                              22
  //      0       (25,25,0)         8|8      (101,101,0)          6|7         (178,178,0)          55
  //      |                         4|5                           9|0                              45
  //      |                          |                             |                               ||
  //      |                          |                             |                               ||
  //      -------------------------------------------------------------------------------------------
  // minimum pixel value                                                                  maximum pixel value
  //
  // This is a general diagram for how Analyze maps a grayvalue to a color.  This is example is for
  // the color yellow (255, 255, 0) and minimum pixel value equal to 0 and maximum
  // pixel value equal to 255, number of shades equal to 4.  The end color is represented by the 
  // slot 255.  The start color is a percentage of the end color (typically 10% rounding down).  
  // If the number of shades is equal to one, then the end color dominates the color allocation, 
  // producing a rendering which is the end color.  If the number of shades is equal to 2, then the 
  // maximum pixel value is assigned the end color and the rest is assigned the start color.  For 
  // number of shades > 3, maximum pixel value is always the end color.  Then each of the subdivisions
  // are linearly interpolated without rounding to produce the colors and also the gray value cutoffs.
  // These values shown above have been verified from Analyze and show how the values are separated.
  // For images that are of a different type than unsigned char, the value must be converted to a grayscale
  // value before rendering.  All divisions in the interpolation are integer divisions, so the floating
  // point calculations aren't rounded when they are converted to unsigned char.
  //
  // Here we go!

  // Check that the index is inside the image
  assert( xloc >= 0 );
  assert( yloc >= 0 );
  assert( zloc >= 0 );
  assert( xloc < objmap.getXDim() );
  assert( yloc < objmap.getYDim() );
  assert( zloc < objmap.getZDim() );

  unsigned char object_index = objmap.ConstPixel( xloc, yloc, zloc );
  //std::cout << "object_index = " << static_cast<int>( object_index ) << std::endl;

  //uiig::PrintObjectEntry( objmap.getObjectEntry( object_index ) );

  const int start_color_red = objmap.getObjectEntry( object_index ).getStartRed();
  const int start_color_green = objmap.getObjectEntry( object_index ).getStartGreen();
  const int start_color_blue = objmap.getObjectEntry( object_index ).getStartBlue();

  const int end_color_red = objmap.getObjectEntry( object_index ).getEndRed();
  const int end_color_green = objmap.getObjectEntry( object_index ).getEndGreen();
  const int end_color_blue = objmap.getObjectEntry( object_index ).getEndBlue();
  
  int number_of_shades = objmap.getObjectEntry( object_index ).getShades(); 

  //std::cout << "Minimum pixel value = " << static_cast<int>( minimum_pixel_value ) << std::endl;
  //std::cout << "Maximum pixel value = " << static_cast<int>( maximum_pixel_value ) << std::endl;

  if ( number_of_shades == 1 )
  {
  red = end_color_red;
  green = end_color_green;
  blue = end_color_blue;
  }
  else
  {
  std::vector<int> gray_cutoff_index( number_of_shades );
  std::vector<int> red_cutoff_colors( number_of_shades );
  std::vector<int> green_cutoff_colors( number_of_shades );
  std::vector<int> blue_cutoff_colors( number_of_shades );

  const int number_of_shades_1 = number_of_shades - 1;

  // Building the cutoff table
  gray_cutoff_index[number_of_shades_1] = maximum_pixel_value;
  red_cutoff_colors[number_of_shades_1] = end_color_red;
  green_cutoff_colors[number_of_shades_1] = end_color_green;
  blue_cutoff_colors[number_of_shades_1] = end_color_blue;
  
  for ( int i = 0; i < number_of_shades - 1; i++ )
  {
    gray_cutoff_index[i] = minimum_pixel_value + static_cast<int>( ceil ( ( static_cast<float>( i )* static_cast<float>( maximum_pixel_value - minimum_pixel_value ) / static_cast<float>( number_of_shades - 1 ) ) ) );
    red_cutoff_colors[i] = start_color_red + static_cast<int>( ( static_cast<float>( i )* static_cast<float>( end_color_red  - start_color_red ) / static_cast<float>( number_of_shades - 1 ) ) );
    green_cutoff_colors[i] = start_color_green + static_cast<int>( ( static_cast<float>( i )* static_cast<float>( end_color_green  - start_color_green ) / static_cast<float>( number_of_shades - 1 ) ) );
    blue_cutoff_colors[i] = start_color_blue + static_cast<int>( ( static_cast<float>( i )* static_cast<float>( end_color_blue  - start_color_blue ) / static_cast<float>( number_of_shades - 1 ) ) );
  }  

  //for ( int i = 0; i < number_of_shades; i++ )
  //{
  //  std::cout << "i = " << i << ", cutoff( gray, red, green, blue ) = (" << gray_cutoff_index[i] << ", " << red_cutoff_colors[i] << ", " << green_cutoff_colors[i] << ", " << blue_cutoff_colors[i] << ")" << std::endl;
  //}
  
  for ( int i = number_of_shades - 1; i >= 0; i-- )
  {
    if ( gray >= gray_cutoff_index[i] )
    {
    red = static_cast<unsigned char>( red_cutoff_colors[i] );
    green = static_cast<unsigned char>( green_cutoff_colors[i] );
    blue = static_cast<unsigned char>( blue_cutoff_colors[i] );
    break;
    }
  }
  }
}

void uiig::GetAnalyzeMultiSectionRenderedColor( unsigned char& red, unsigned char& green, unsigned char& blue, const std::vector< std::vector<uiig::RGBTRIPLE> >& colormapper, const unsigned char gray, const unsigned char object_index )
{
  assert( object_index < colormapper.size() );
  red = (colormapper[object_index])[gray].getConstRedComponent();
  green = (colormapper[object_index])[gray].getConstGreenComponent();
  blue = (colormapper[object_index])[gray].getConstBlueComponent();
}

void uiig::GetAnalyzeMultiSectionRenderedColor( uiig::RGBTRIPLE& rgb, const std::vector< std::vector<uiig::RGBTRIPLE> >& colormapper, const unsigned char gray, const unsigned char object_index )
{
  assert( object_index < colormapper.size() );
  rgb.setRGBTRIPLEComponents( (colormapper[object_index])[gray].getConstRedComponent(), (colormapper[object_index])[gray].getConstGreenComponent(), (colormapper[object_index])[gray].getConstBlueComponent() );
}

void uiig::CreateObjectMapColorMapper( std::vector< std::vector<uiig::RGBTRIPLE> >& colormapper, const uiig::CObjectMap& objmap, const unsigned char minimum_pixel_value, const unsigned char maximum_pixel_value )
{
  colormapper.clear();

  std::vector<uiig::RGBTRIPLE> colormap( 256 );

  for ( int object_index = 0; object_index <= objmap.getNumberOfObjects(); object_index++ )
  {
  const int start_color_red = objmap.getObjectEntry( object_index ).getStartRed();
  const int start_color_green = objmap.getObjectEntry( object_index ).getStartGreen();
  const int start_color_blue = objmap.getObjectEntry( object_index ).getStartBlue();
  
  const int end_color_red = objmap.getObjectEntry( object_index ).getEndRed();
  const int end_color_green = objmap.getObjectEntry( object_index ).getEndGreen();
  const int end_color_blue = objmap.getObjectEntry( object_index ).getEndBlue();

  const int number_of_shades = objmap.getObjectEntry( object_index ).getShades();
  std::vector<int> gray_cutoff_index( number_of_shades );
  std::vector<int> red_cutoff_colors( number_of_shades );
  std::vector<int> green_cutoff_colors( number_of_shades );
  std::vector<int> blue_cutoff_colors( number_of_shades );
  
  const int number_of_shades_1 = number_of_shades - 1;
  
  // Building the cutoff table
  gray_cutoff_index[number_of_shades_1] = maximum_pixel_value;
  red_cutoff_colors[number_of_shades_1] = end_color_red;
  green_cutoff_colors[number_of_shades_1] = end_color_green;
  blue_cutoff_colors[number_of_shades_1] = end_color_blue;
  
  for ( int i = 0; i < number_of_shades_1; i++ )
  {
    gray_cutoff_index[i] = minimum_pixel_value + static_cast<int>( ceil ( ( static_cast<float>( i )* static_cast<float>( maximum_pixel_value - minimum_pixel_value ) / static_cast<float>( number_of_shades - 1 ) ) ) );
    red_cutoff_colors[i] = start_color_red + static_cast<int>( ( static_cast<float>( i )* static_cast<float>( end_color_red  - start_color_red ) / static_cast<float>( number_of_shades - 1 ) ) );
    green_cutoff_colors[i] = start_color_green + static_cast<int>( ( static_cast<float>( i )* static_cast<float>( end_color_green  - start_color_green ) / static_cast<float>( number_of_shades - 1 ) ) );
    blue_cutoff_colors[i] = start_color_blue + static_cast<int>( ( static_cast<float>( i )* static_cast<float>( end_color_blue  - start_color_blue ) / static_cast<float>( number_of_shades - 1 ) ) );
  }

  // Assigning colors to the colormap vector
  int startgrayvalue = 0;
  for ( int i = 0; i < number_of_shades - 1; i++ )
  {
    for ( int gray = startgrayvalue; gray < gray_cutoff_index[i+1]; gray++ )
    {
    colormap[gray].setRGBTRIPLEComponents( static_cast<unsigned char>( red_cutoff_colors[i] ), static_cast<unsigned char>( green_cutoff_colors[i] ), static_cast<unsigned char>( blue_cutoff_colors[i] ) );
    }
    startgrayvalue = gray_cutoff_index[i+1];
  }
  colormap[255].setRGBTRIPLEComponents( static_cast<unsigned char>( end_color_red ), static_cast<unsigned char>( end_color_green ), static_cast<unsigned char>( end_color_blue ) );

  //for ( int i = 0; i < number_of_shades; i++ )
  //{
  //  std::cout << "i = " << i << ", cutoff( gray, red, green, blue ) = (" << gray_cutoff_index[i] << ", " << red_cutoff_colors[i] << ", " << green_cutoff_colors[i] << ", " << blue_cutoff_colors[i] << ")" << std::endl;
  //}

  //for ( int i = 0; i < 256; i++ )
  //{
  //  std::cout << "color[" << i << "] = (" << static_cast<int>( colormap[i].getRedComponent() ) << "," << static_cast<int>( colormap[i].getGreenComponent() ) << "," << static_cast<int>( colormap[i].getBlueComponent() ) << ")" << std::endl;
  //}

  colormapper.push_back( colormap );
  }
}

void uiig::PrintObjectEntry( const uiig::CObjectEntry & rhs )
{
  std::cout <<  "Name " << rhs.getName() << std::endl;
  std::cout <<  "DisplayFlag " << rhs.getDisplayFlag() << std::endl;
  std::cout <<  "CopyFlag " << static_cast<int>( rhs.getCopyFlag() ) << std::endl;
  std::cout <<  "MirrorFlag " << static_cast<int>( rhs.getMirrorFlag() ) << std::endl;
  std::cout <<  "StatusFlag " << static_cast<int>( rhs.getStatusFlag() ) << std::endl;
  std::cout <<  "NeighborsUsedFlag " << static_cast<int>( rhs.getNeighborsUsedFlag() ) << std::endl;
  std::cout <<  "Shades " << rhs.getShades() << std::endl;
  std::cout <<  "StartRed " << rhs.getStartRed() << std::endl;
  std::cout <<  "StartGreen " << rhs.getStartGreen() << std::endl;
  std::cout <<  "StartBlue " << rhs.getStartBlue() << std::endl;
  std::cout <<  "EndRed " << rhs.getEndRed() << std::endl;
  std::cout <<  "EndGreen " << rhs.getEndGreen() << std::endl;
  std::cout <<  "EndBlue " << rhs.getEndBlue() << std::endl;
  std::cout <<  "XRotation " << rhs.getXRotation() << std::endl;
  std::cout <<  "YRotation " << rhs.getYRotation() << std::endl;
  std::cout <<  "ZRotation " << rhs.getZRotation() << std::endl;
  std::cout <<  "XTranslation " << rhs.getXTranslation() << std::endl;
  std::cout <<  "YTranslation " << rhs.getYTranslation() << std::endl;
  std::cout <<  "ZTranslation " << rhs.getZTranslation() << std::endl;
  std::cout <<  "XCenter " << rhs.getXCenter() << std::endl;
  std::cout <<  "YCenter " << rhs.getYCenter() << std::endl;
  std::cout <<  "ZCenter " << rhs.getZCenter() << std::endl;
  std::cout <<  "XRotationIncrement " << rhs.getXRotationIncrement() << std::endl;
  std::cout <<  "YRotationIncrement " << rhs.getYRotationIncrement() << std::endl;
  std::cout <<  "ZRotationIncrement " << rhs.getZRotationIncrement() << std::endl;
  std::cout <<  "XTranslationIncrement " << rhs.getXTranslationIncrement() << std::endl;
  std::cout <<  "YTranslationIncrement " << rhs.getYTranslationIncrement() << std::endl;
  std::cout <<  "ZTranslationIncrement " << rhs.getZTranslationIncrement() << std::endl;
  std::cout <<  "MinimumXValue " << rhs.getMinimumXValue() << std::endl;
  std::cout <<  "MinimumYValue " << rhs.getMinimumYValue() << std::endl;
  std::cout <<  "MinimumZValue " << rhs.getMinimumZValue() << std::endl;
  std::cout <<  "MaximumXValue " << rhs.getMaximumXValue() << std::endl;
  std::cout <<  "MaximumYValue " << rhs.getMaximumYValue() << std::endl;
  std::cout <<  "MaximumZValue " << rhs.getMaximumZValue() << std::endl;
  std::cout <<  "Opacity " << rhs.getOpacity() << std::endl;
  std::cout <<  "OpacityThickness " << rhs.getOpacityThickness() << std::endl;
  std::cout <<  "Dummy " << rhs.getDummy() << std::endl;
}


#ifndef USE_AUTOMATIC_INSTANTIATION
#if 0
namespace uiig
{

  template bool uiig::MakeUniformObjectmapFromColormap<float, unsigned char>
    ( uiig::CObjectMap & CurrentMap, const uiig::CImage<float> & SourceImage,
    const float MinRange, const float MaxRange,
    const unsigned char NumberOfPartitions, const uiig::Colormap<unsigned char> & cmap );
  template bool uiig::MakeUniformObjectmapFromColormap<float, float>
    ( uiig::CObjectMap & CurrentMap, const uiig::CImage<float> & SourceImage,
    const float MinRange, const float MaxRange,
    const unsigned char NumberOfPartitions, const uiig::Colormap<float> & cmap );

  template bool uiig::MakeLogscaleObjectmapFromColormap<unsigned char>
    ( uiig::CObjectMap & CurrentMap, const uiig::CImage<float> & SourceImage,
    const float MinRange, const float MaxRange,
    const unsigned char NumberOfPartitions, const uiig::Colormap<unsigned char> & cmap,
    bool StartLogScaleLow );
  template bool uiig::MakeLogscaleObjectmapFromColormap<float>
    ( uiig::CObjectMap & CurrentMap, const uiig::CImage<float> & SourceImage,
    const float MinRange, const float MaxRange,
    const unsigned char NumberOfPartitions, const uiig::Colormap<float> & cmap,
    bool StartLogScaleLow );

}                                // end of namespace uiig
#endif
#endif
