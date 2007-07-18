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

#if 0
void CompactString( std::string & output, const std::string input )
{
  char *buffer = new char(input.length() + 1);
  int count = 0;

  for (unsigned int i = 0; i < input.length(); i++)
  {
    if (input[i] != ' ')
    {
      buffer[count] = input[i];
    }
  }

  buffer[input.length()] = '\0';

  output = std::string(buffer);

  delete[] buffer;
}
#endif

#if 0
bool AnalyzeObjectMap::AddObjectInRange(const CImage<float> & InputImage,
const float MinRange, const float MaxRange,
std::string ObjectName, const int EndRed,
const int EndGreen, const int EndBlue,
const int Shades, bool OverWriteObjectFlag)
{
  const int nx=InputImage.getXDim();
  const int ny=InputImage.getYDim();
  const int nz=InputImage.getZDim();
  if(nx != this->getXDim() ||
    ny != this->getYDim() ||
    nz != this->getZDim())
  {
    return false;
  }
  this->NumberOfObjects++;
  this->AnaylzeObjectEntryArray[NumberOfObjects]= AnalyzeObjectEntry::New();
  AnaylzeObjectEntryArray[NumberOfObjects]->SetName(ObjectName);
  AnaylzeObjectEntryArray[NumberOfObjects]->SetEndColor(EndRed,EndGreen,EndBlue);
  AnaylzeObjectEntryArray[NumberOfObjects]->SetStartColor(static_cast<int>(0.1F*EndRed),
    static_cast<int>(0.1F*EndGreen),
    static_cast<int>(0.1F*EndBlue));
  AnaylzeObjectEntryArray[NumberOfObjects]->SetShades(Shades);

  for(int k=0; k<nz; k++)
  {
    for( int j=0; j< ny ; j++)
    {
      for( int i=0; i<nx; i++)
      {
        const float CurrentValue=InputImage(i,j,k);

        if(CurrentValue>=MinRange && CurrentValue<=MaxRange)
        {
          // Check to see if there is another object here (not background)
          if (this->Pixel(i,j,k) > 0)
          {
            // This pixel has already been assigned an object
            if (OverWriteObjectFlag == true)
              this->Pixel(i,j,k)=NumberOfObjects;
            // else do nothing
          }
          else
          {
            // Must be background
            this->Pixel(i,j,k)=NumberOfObjects;
          }
        }
      }
    }
  }
  return true;
}

bool AnalyzeObjectMap::AddObjectInRange(const CImage<unsigned char> & InputImage,
const int MinRange, const int MaxRange,
std::string ObjectName, const int EndRed, const int EndGreen,
const int EndBlue, const int Shades, bool OverWriteObjectFlag)
{
  const int nx=InputImage.getXDim();
  const int ny=InputImage.getYDim();
  const int nz=InputImage.getZDim();
  if(nx != this->getXDim() ||
    ny != this->getYDim() ||
    nz != this->getZDim())
  {
    return false;
  }
  this->NumberOfObjects++;
  this->AnaylzeObjectEntryArray[NumberOfObjects]= AnalyzeObjectEntry::New();
  AnaylzeObjectEntryArray[NumberOfObjects]->SetName(ObjectName);
  AnaylzeObjectEntryArray[NumberOfObjects]->SetEndColor(EndRed,EndGreen,EndBlue);
  AnaylzeObjectEntryArray[NumberOfObjects]->SetStartColor(static_cast<int>(0.1F*EndRed),
    static_cast<int>(0.1F*EndGreen),
    static_cast<int>(0.1F*EndBlue));
  AnaylzeObjectEntryArray[NumberOfObjects]->SetShades(Shades);

  for(int k=0; k<nz; k++)
  {
    for( int j=0; j<ny ; j++)
    {
      for( int i=0; i<nx; i++)
      {
        const int CurrentValue=InputImage(i,j,k);
        if(CurrentValue>=MinRange && CurrentValue<=MaxRange)
        {
          if (this->Pixel(i,j,k) > 0)
          {
            // This pixel has already been assigned an object
            if (OverWriteObjectFlag == true)
              this->Pixel(i,j,k)=NumberOfObjects;
            // else do nothing
          }
          else
          {
            // Must be background
            this->Pixel(i,j,k)=NumberOfObjects;
          }
        }
      }
    }
  }
  return true;
}
#endif


bool AnalyzeObjectMap::RemoveObjectByName(const std::string & ObjectName)
{
  if (ObjectName == std::string("Background"))
  {
    // Can not delete background object
    ::fprintf(stderr, "Can not remove the background object.\n");
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
    // Object name not found
    printf("Object name %s was not found.\n", ObjectName.c_str());
    return false;
  }

  // Go through all the object headers and shift them down
  {
    for (int j = objectTag; j < this->GetNumberOfObjects(); j++)
    {
      this->getObjectEntry(j) = this->getObjectEntry(j+1);
    }
  }

  // Deleting the last extra data structure
  delete AnaylzeObjectEntryArray[this->GetNumberOfObjects()];
  AnaylzeObjectEntryArray[this->GetNumberOfObjects()] = NULL;

  #if 0
  // Changing the image object identifiers
  {

    for (int i = 0; i < this->getXDim()*this->getYDim()*this->getZDim(); i++)
    {
      if (this->ConstPixel(i) == objectTag)
      {
        // Assign to the background
        this->Pixel(i) = 0;
      }
      else if (this->ConstPixel(i) > objectTag)
      {
        this->Pixel(i) = this->Pixel(i) - 1;
      }

    }
  }

  NumberOfObjects = NumberOfObjects - 1;
  #endif

  return true;
}


bool AnalyzeObjectMap::RemoveObjectByRange(const unsigned char MinRange, const unsigned char MaxRange)
{
  int startObjectTag = MinRange;
  int endObjectTag = MaxRange;

  // Validating ranges
  if (MinRange == 0)
  {
    // Do not delete background object, but continue with the rest of the objects in range
    printf("Can not delete the background object.\n");
    startObjectTag = 1;
  }

  if (MaxRange > this->GetNumberOfObjects())
  {
    // Set the end to the number of objects
    printf("Max Range too large.  Setting it too maximum number of objects in the objectmap.\n");
    endObjectTag = this->GetNumberOfObjects();
  }

  if (MinRange > MaxRange)
  {
    // Parameters are backwards
    ::fprintf(stderr, "Minimum Range %d and Maximum Range %d for removing objects are invalid.\n",
      MinRange, MaxRange);
    return false;
  }

  int NumberToDelete = endObjectTag - startObjectTag + 1;

  // Go through all the object headers and shift them down
  {
    for (int j = endObjectTag + 1; j <= this->GetNumberOfObjects(); j++)
    {
      this->getObjectEntry(j-NumberToDelete) = this->getObjectEntry(j);
    }
  }

  // Deleting the last extra data structure
  {
    for (int i = this->GetNumberOfObjects(); i > this->GetNumberOfObjects() - NumberToDelete; i--)
    {
      delete AnaylzeObjectEntryArray[i];
      AnaylzeObjectEntryArray[i] = NULL;
    }
  }

#if 0
  // Changing the image object identifiers
  {
    for (int i = 0; i < this->getXDim()*this->getYDim()*this->getZDim(); i++)
    {
      if ((this->ConstPixel(i) >= startObjectTag) && (this->ConstPixel(i) <= endObjectTag))
      {
        // Assign to the background
        this->Pixel(i) = 0;
      }
      else if (this->ConstPixel(i) > endObjectTag)
      {
        this->Pixel(i) = this->Pixel(i) - NumberToDelete;
      }
    }
  }

  NumberOfObjects = NumberOfObjects - NumberToDelete;
#endif
  return true;
}

#if 0
bool AnalyzeObjectMap::CheckObjectOverlap( const CImage<float> & InputImage, const float MinRange, const float MaxRange, const int MinObjectRange, const int MaxObjectRange )
{
  const int nx=InputImage.getXDim();
  const int ny=InputImage.getYDim();
  const int nz=InputImage.getZDim();
  if(nx != this->getXDim() ||
    ny != this->getYDim() ||
    nz != this->getZDim())
  {
    return false;
  }

  // Error checking
  if ( MinObjectRange < 0 )
  {
    ::fprintf( stderr, "Error. Can not have a MinObjectRange < 0!\n" );
    exit(-1);
  }
  if ( MaxObjectRange < 0 )
  {
    ::fprintf( stderr, "Error. Can not have a MaxObjectRange < 0!\n" );
    exit(-1);
  }
  if ( MaxObjectRange < MinObjectRange )
  {
    fprintf( stderr, "Error. MaxObjectRange can not be less than MinObjectRange!\n" );
    exit(-1);
  }

  for(int k=0; k<nz; k++)
  {
    for( int j=0; j<ny ; j++)
    {
      for( int i=0; i<nx; i++)
      {
        const float CurrentValue=InputImage(i,j,k);
        if(CurrentValue>=MinRange && CurrentValue<=MaxRange)
        {
          if (this->Pixel(i,j,k) >= MinObjectRange && this->Pixel(i,j,k) <= MaxObjectRange)
          {
            // This pixel has already been assigned an object in the defined range, overlap occuring
            return true;
          }
        }
      }
    }
  }
  return false;
}


bool AnalyzeObjectMap::CheckObjectOverlap( const CImage<unsigned char> & InputImage, const int MinRange, const int MaxRange, const int MinObjectRange, const int MaxObjectRange )
{
  const int nx=InputImage.getXDim();
  const int ny=InputImage.getYDim();
  const int nz=InputImage.getZDim();
  if(nx != this->getXDim() ||
    ny != this->getYDim() ||
    nz != this->getZDim())
  {
    return false;
  }

  // Error checking
  if ( MinObjectRange < 0 )
  {
    ::fprintf( stderr, "Error. Can not have a MinObjectRange < 0!\n" );
    exit(-1);
  }
  if ( MaxObjectRange < 0 )
  {
    ::fprintf( stderr, "Error. Can not have a MaxObjectRange < 0!\n" );
    exit(-1);
  }
  if ( MaxObjectRange < MinObjectRange )
  {
    fprintf( stderr, "Error. MaxObjectRange can not be less than MinObjectRange!\n" );
    exit(-1);
  }

  for(int k=0; k<nz; k++)
  {
    for( int j=0; j<ny ; j++)
    {
      for( int i=0; i<nx; i++)
      {
        const int CurrentValue=InputImage(i,j,k);
        if(CurrentValue>=MinRange && CurrentValue<=MaxRange)
        {
          if (this->Pixel(i,j,k) >= MinObjectRange && this->Pixel(i,j,k) <= MaxObjectRange)
          {
            // This pixel has already been assigned an object in the defined range, overlap occuring
            return true;
          }
        }
      }
    }
  }
  return false;
}
#endif
int AnalyzeObjectMap::getObjectIndex( const std::string &ObjectName  )
{
  for(int index=0; index<=this->GetNumberOfObjects(); index++)
  {
    if(ObjectName == this->getObjectEntry(index)->GetName() )
    {
      return index;
    }
  }
  //Failure if not found.
  return -1;
}

    /**
   * Constant defining the maximum number of shades possible for an object as of Version 6
   */
  const int MAXANALYZESHADES = 250;
