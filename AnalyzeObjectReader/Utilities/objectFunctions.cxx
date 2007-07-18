bool AnalyzeObjectMap::WriteObjectByName(const std::string & ObjectName, const std::string & filename)
{
  // Finding the object index with the name indicated
  if (ObjectName == std::string("Background"))
  {
    // Can not write a background object
    ::fprintf(stderr, "Error: Can not write out a background object.\n");
    return false;
  }

  int objectTag = 0;
  {
    for (int i = 1; i <= this->GetNumberOfObjects(); i++)
    {
      if (ObjectName == this->getObjectEntry(i)->GetName())
      {
        objectTag = i;
        break;
      }
    }
  }

  if (objectTag == 0)
  {
    ::fprintf(stderr, "Error.  Object name not found in current objectmap.\n");
    return false;
  }

  FILE *fptr;
  std::string tempfilename=filename;

  // This assumes that the final '.' in a file name is the delimiter
  // for the file's extension type
  const std::string::size_type it = filename.find_last_of( "." );

  //Note: if there is no extension, then it=basic_string::npos;
  if(it > filename.length())
  {
    tempfilename+=".obj";
  }
  else
  {
    // Now we have the index of the extension, make a new string
    // that extends from the first letter of the extension to the end of filename
    std::string fileExt( filename, it, filename.length() );
    if(fileExt != ".obj")
    {
      tempfilename+=".obj";
    }
  }

  // Opening the file
  if ( ( fptr = ::fopen( tempfilename.c_str(), "wb" ) ) == NULL)
  {
    ::fprintf( stderr, "Error: Could not open %s\n", filename.c_str() );
    exit(-1);
  }

  int header[5];

  // Reading the Header into the class
  header[0]=VERSION7;
#if 0
  header[1]=this->getXDim();
  header[2]=this->getYDim();
  header[3]=this->getZDim();
  header[4]=2;                   // Including the background object when writing the .obj file

  bool NeedByteSwap = false;

  FileIOUtility util;
  // Byte swapping needed, Analyze object files are always written in BIG ENDIAN format
  if(util.getMachineEndianess() == util.DT_LITTLE_ENDIAN)
  {
    NeedByteSwap=true;
    util.FourByteSwap(&(header[0]));
    util.FourByteSwap(&(header[1]));
    util.FourByteSwap(&(header[2]));
    util.FourByteSwap(&(header[3]));
    util.FourByteSwap(&(header[4]));
  }

  // Creating the binary mask
  CImageFlat<unsigned char> binaryMask(this->getXDim(), this->getYDim(), this->getZDim());

  #endif
//TODO  binaryMask.ImageClear();

  // Creating the binary mask
  {
#if 0
    for (int i = 0; i < this->getXDim()*this->getYDim()*this->getZDim(); i++)
    {
      if (this->ConstPixel(i) == objectTag)
      {
        binaryMask.Pixel(i) = 1;
      }
    }
#endif
  }

  // Writing the header, which contains the version number, the size, and the
  // number of objects
  if ( ::fwrite( header, sizeof(int), 5, fptr) != 5 )
  {
    ::fprintf( stderr, "Error: Could not write header of %s\n", filename.c_str());
    exit(-1);
  }

  // Since the NumberOfObjects does not reflect the background, the background will be written out

  // Writing the background header
  // Using a temporary so that the object file is always written in BIG_ENDIAN mode but does
  // not affect the current object itself
#if 0
  AnalyzeObjectEntry ObjectWrite = this->getObjectEntry(0);

  if (NeedByteSwap == true)
  {
    SwapObjectEndedness(ObjectWrite.getObjectPointer());
  }


  if (::fwrite((ObjectWrite.getObjectPointer()), sizeof(Object), 1, fptr) != 1)
  {
    ::fprintf(stderr, "13: Unable to write object background of %s\n", filename.c_str());
    exit(-1);
  }

  ObjectWrite = this->getObjectEntry(objectTag);

  if (NeedByteSwap == true)
  {
    SwapObjectEndedness(ObjectWrite.getObjectPointer());
  }

  // Writing the object header
  if (::fwrite((ObjectWrite.getObjectPointer()), sizeof(Object), 1, fptr) != 1)
  {
    ::fprintf(stderr, "13: Unable to write in object #%d description of %s\n", objectTag, filename.c_str());
    exit(-1);
  }

  RunLengthEncodeImage(binaryMask, fptr);

  ::fclose(fptr);
  #endif
  return true;
}


bool AnalyzeObjectMap::WriteDisplayedObjects(const std::string & filenamebase)
{
  FILE *fptr;
  std::string tempfilename=filenamebase;

  for (int objectTag = 1; objectTag <= this->GetNumberOfObjects(); objectTag++)
  {

    // DEBUG
    // Look at the case when the user puts in a file with an .obj extension.  Append the
    // number in the middle of the filename, not at the end

    if (getObjectEntry(objectTag)->GetDisplayFlag() != 0)
    {
      // Append the object number to the filename
      std::string tempfilenumbername = tempfilename + std::string("_");

      std::string filenumber("#");
      filenumber[0] = (char)((int)'0' + objectTag);
      tempfilenumbername += filenumber;

      tempfilenumbername+=".obj";

      // Opening the file
      if ( ( fptr = ::fopen( tempfilenumbername.c_str(), "wb" ) ) == NULL)
      {
        ::fprintf( stderr, "Error: Could not open %s\n", tempfilenumbername.c_str());
        exit(-1);
      }

      int header[5];

      // Reading the Header into the class
      header[0]=VERSION7;
#if 0
      header[1]=this->getXDim();
      header[2]=this->getYDim();
      header[3]=this->getZDim();
      header[4]=2;               // Including the background object when writing the .obj file

      bool NeedByteSwap = false;

      FileIOUtility util;
      // Byte swapping needed, Analyze files are always written in BIG ENDIAN format
      if(util.getMachineEndianess() == util.DT_LITTLE_ENDIAN)
      {
        NeedByteSwap=true;
        util.FourByteSwap(&(header[0]));
        util.FourByteSwap(&(header[1]));
        util.FourByteSwap(&(header[2]));
        util.FourByteSwap(&(header[3]));
        util.FourByteSwap(&(header[4]));
      }

      // Creating the binary mask
      CImageFlat<unsigned char> binaryMask(this->getXDim(), this->getYDim(), this->getZDim());
//TODO      binaryMask.ImageClear();

      // Creating the binary mask
      for (int i = 0; i < this->getXDim()*this->getYDim()*this->getZDim(); i++)
      {
        if (this->ConstPixel(i) == objectTag)
        {
          binaryMask.Pixel(i) = 1;
        }
      }

      // Writing the header, which contains the version number, the size, and the
      // number of objects
      if ( ::fwrite( header, sizeof(int), 5, fptr) != 5 )
      {
        ::fprintf( stderr, "Error: Could not write header of %s\n", tempfilenumbername.c_str());
        exit(-1);
      }

      // Since the NumberOfObjects does not reflect the background, the background will be written out

      // Writing the background header
      // Using a temporary so that the object file is always written in BIG_ENDIAN mode but does
      // not affect the current object itself
      AnalyzeObjectEntry ObjectWrite = this->getObjectEntry(0);

      if (NeedByteSwap == true)
      {
        SwapObjectEndedness(ObjectWrite.getObjectPointer());
      }

      if (::fwrite((ObjectWrite.getObjectPointer()), sizeof(Object), 1, fptr) != 1)
      {
        ::fprintf(stderr, "13: Unable to write object background of %s\n", tempfilenumbername.c_str());
        exit(-1);
      }

      ObjectWrite = this->getObjectEntry(objectTag);

      if (NeedByteSwap == true)
      {
        SwapObjectEndedness(ObjectWrite.getObjectPointer());
      }

      // Writing the object header
      if (::fwrite((ObjectWrite.getObjectPointer()), sizeof(Object), 1, fptr) != 1)
      {
        ::fprintf(stderr, "13: Unable to write in object #%d description of %s\n",
          objectTag, tempfilenumbername.c_str());
        exit(-1);
      }

      RunLengthEncodeImage(binaryMask, fptr);

      fclose(fptr);
    }
  }
  

#endif
  }
  }
  return true;
}

#if 0
bool AnalyzeObjectMap::CalculateBoundingRegionAndCenter( void )
{

  for (int i = 0; i < NumberOfObjects; i++)
  {
    AnaylzeObjectEntryArray[i]->SetMinimumXValue(this->GetXDim());
    AnaylzeObjectEntryArray[i]->SetMaximumXValue(0);
    AnaylzeObjectEntryArray[i]->SetMinimumYValue(this->GetYDim());
    AnaylzeObjectEntryArray[i]->SetMaximumYValue(0);
    AnaylzeObjectEntryArray[i]->SetMinimumZValue(this->GetZDim());
    AnaylzeObjectEntryArray[i]->SetMaximumZValue(0);
  }

  const int PlaneSize = this->GetXDim()*this->GetYDim();

  std::vector<long int> xsum(this->GetNumberOfObjects());
  std::vector<long int> ysum(this->GetNumberOfObjects());
  std::vector<long int> zsum(this->GetNumberOfObjects());
  std::vector<long int> xnum(this->GetNumberOfObjects());
  std::vector<long int> ynum(this->GetNumberOfObjects());
  std::vector<long int> znum(this->GetNumberOfObjects());

  {
    for (int i = 0; i < this->GetNumberOfObjects(); i++)
    {
      xsum[i] = 0;
      ysum[i] = 0;
      zsum[i] = 0;
      xnum[i] = 0;
      ynum[i] = 0;
      znum[i] = 0;
    }
  }

  int xIndex, yIndex, zIndex, r, s, t;

  for (zIndex=0, t=0; zIndex < this->GetZDim(); zIndex++, t+=PlaneSize)
  {
    for (yIndex=0, s=t; yIndex < this->GetYDim(); yIndex++, s+=this->GetXDim())
    {
      for (xIndex=0, r=s; xIndex < this->GetXDim(); xIndex++, r++)
      {
        int object = this->ConstPixel(r);
        if (object > this->GetNumberOfObjects())
        {
          ::fprintf(stderr, "Error: There are greater index than number of objects.\n");
          exit(-1);
        }
        if (xIndex < getObjectEntry(object)->GetMinimumXValue())
          getObjectEntry(object)->SetMinimumXValue(xIndex);
        if (xIndex > getObjectEntry(object)->GetMaximumXValue())
          getObjectEntry(object)->SetMaximumXValue(xIndex);

        if (yIndex < getObjectEntry(object)->GetMinimumYValue())
          getObjectEntry(object)->SetMinimumYValue(yIndex);
        if (yIndex > getObjectEntry(object)->GetMaximumYValue()
          getObjectEntry(object)->SetMaximumYValue(yIndex);

        if (zIndex < getObjectEntry(object)->GetMinimumZValue())
          getObjectEntry(object)->SetMinimumZValue(zIndex);
        if (zIndex > getObjectEntry(object)->GetMaximumZValue())
          getObjectEntry(object)->SetMaximumZValue(zIndex);

        xsum[object]+=xIndex;
        ysum[object]+=yIndex;
        zsum[object]+=zIndex;
        xnum[object]++;
        ynum[object]++;
        znum[object]++;
      }
    }
  }

  {
    for (int i = 0; i < this->GetNumberOfObjects(); i++)
    {
      if (xnum[i] == 0)
        getObjectEntry(i)->SetXCenter(0);
      else
        getObjectEntry(i)->SetXCenter((short int)(xsum[i]/xnum[i] - this->GetXDim()/2));

      if (ynum[i] == 0)
        getObjectEntry(i)->SetYCenter(0);
      else
        getObjectEntry(i)->SetYCenter((short int)(ysum[i]/ynum[i] - this->GetYDim()/2));

      if (znum[i] == 0)
        getObjectEntry(i)->SetZCenter(0);
      else
        getObjectEntry(i)->SetZCenter((short int)(zsum[i]/znum[i] - this->GetZDim()/2));
    }
  }

  return true;
}


int AnalyzeObjectMap::EvenlyShade( void )
{
  int NumberOfShades = 255 / this->GetNumberOfObjects();

  // Not allocating shades to the background object
  for (int i = 0; i < this->GetNumberOfObjects(); i++)
    getObjectEntry(i+1)->SetShades(NumberOfShades);

  return NumberOfShades;
}


void AnalyzeObjectMap::ConstShade( void )
{
  // Not allocating shades to the background object, and setting each shade to 1
  for (int i = 0; i < this->GetNumberOfObjects(); i++)
    getObjectEntry(i+1)->SetShades( 1 );
}


int AnalyzeObjectMap::getVersion( void ) const
{
  return Version;
}

int AnalyzeObjectMap::GetNumberOfObjects( void ) const
{
  return this->NumberOfObjects;
}


unsigned char AnalyzeObjectMap::isObjectShown( const unsigned char index ) const
{
  return ShowObject[index];
}


unsigned char AnalyzeObjectMap::getMinimumPixelValue( const unsigned char index ) const
{
  return MinimumPixelValue[index];
}


unsigned char AnalyzeObjectMap::getMaximumPixelValue( const unsigned char index ) const
{
  return MaximumPixelValue[index];
}

void AnalyzeObjectMap::setMinimumPixelValue( const unsigned char index, const unsigned char value )
{
  MinimumPixelValue[index] = value;
}

void AnalyzeObjectMap::setMaximumPixelValue( const unsigned char index, const unsigned char value )
{
  MaximumPixelValue[index] = value;
}
#endif
