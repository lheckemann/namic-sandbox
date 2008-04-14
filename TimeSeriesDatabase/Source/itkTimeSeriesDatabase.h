
#ifndef __itkTimeSeriesDatabase_h
#define __itkTimeSeriesDatabase_h

#include <itkImage.h>
#include <itkArray.h>
#include <itkImageSource.h>
#include <iostream>
#include <fstream>
#include <LRUCache.h>
#include <counted_ptr.h>

#define TimeSeriesBlockSize 16
#define TimeSeriesBlockSizeP2 TimeSeriesBlockSize*TimeSeriesBlockSize
#define TimeSeriesBlockSizeP3 TimeSeriesBlockSize*TimeSeriesBlockSize*TimeSeriesBlockSize
#define TimeSeriesVolumeBlockSize TimeSeriesBlockSize*TimeSeriesBlockSize*TimeSeriesBlockSize
#define TimeSeriesVolumeBlockSizeP3 TimeSeriesVolumeBlockSize*TimeSeriesVolumeBlockSize*TimeSeriesVolumeBlockSize

namespace itk {

  /*
   * TimeSeriesDatabase transforms a series of images stored on disk into a high performance database
   *
   * The main idea behind TimeSeriesDatabase is to have a representation of a 4 dimensional dataset that
   * is larger than main memory, but may still be accessed in a rapid manner.  Though not strictly
   * ITK conforming, this initial pass is strictly 4 dimensional datasets.
   */
  template <class TPixel> class TimeSeriesDatabase : public ImageSource<Image<TPixel,3> > {
  public:

    typedef TimeSeriesDatabase Self;
    typedef ImageSource<Image<TPixel,3> > Superclass;
    typedef SmartPointer<Self> Pointer;
    typedef SmartPointer<const Self>  ConstPointer;
    typedef WeakPointer<const Self>  ConstWeakPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(TimeSeriesDatabase, ImageSource);

    typedef Image<TPixel, 3> OutputImageType;
    typedef typename OutputImageType::Pointer OutputImageTypePointer;
    typedef Image<TPixel, 2> OutputSliceType;
    typedef typename OutputSliceType::Pointer OutputSliceTypePointer;
    typedef Array<TPixel> ArrayType;

    enum SliceOrientationType { ITK_TSD_XY_PLANE, ITK_TSD_YZ_PLANE, ITK_TSD_XZ_PLANE };
 
    void Connect ( const char* filename );
    void Disconnect();
    void CreateFromFileArchetype ( const char* filename, const char* archetype );
    itkSetMacro ( CurrentImage, unsigned int );
    itkGetMacro ( CurrentImage, unsigned int );

    int GetNumberOfVolumes() { return this->m_Dimensions[3]; };
    virtual void GenerateOutputInformation(void);
    virtual void GenerateData(void);

    void GetVoxelTimeSeries ( typename OutputImageType::IndexType idx, ArrayType& array );
    itkGetMacro ( OutputSpacing, typename OutputImageType::SpacingType );
    itkGetMacro ( OutputRegion, typename OutputImageType::RegionType );
    itkGetMacro ( OutputOrigin, typename OutputImageType::PointType );
    itkGetMacro ( OutputDirection, typename OutputImageType::DirectionType );

  protected:
    TimeSeriesDatabase();
    ~TimeSeriesDatabase();
    virtual void PrintSelf(std::ostream& os, Indent indent) const;
    Array<unsigned int> m_Dimensions;
    Array<unsigned int> m_BlocksPerImage;

    typename OutputImageType::SpacingType m_OutputSpacing;
    typename OutputImageType::RegionType m_OutputRegion;
    typename OutputImageType::PointType m_OutputOrigin;
    typename OutputImageType::DirectionType m_OutputDirection;

    std::streampos CalculatePosition ( Size<3> Position, int ImageCount );
    std::streampos CalculatePosition ( unsigned long index );
    unsigned long CalculateIndex ( Size<3> Position, int ImageCount );
    // Return true if this is a full block, false otherwise.  Assumes there is overlap!
    bool CalculateIntersection ( Size<3> BlockIndex, typename OutputImageType::RegionType RequestedRegion, 
                                 typename OutputImageType::RegionType& BlockRegion,
                                 typename OutputImageType::RegionType& ImageRegion );
    bool IsOpen() const;

    // How many pixels are in the last block?
    Array<unsigned int> m_PixelRemainder;
    std::fstream m_File;
    std::string m_Filename;
    unsigned int m_CurrentImage;

    // our cache
    struct CacheBlock {
      TPixel data[TimeSeriesBlockSize*TimeSeriesBlockSize*TimeSeriesBlockSize];
    };
    LRUCache<unsigned long, CacheBlock> m_Cache;
    CacheBlock* GetCacheBlock ( unsigned long index );
  };

}
# include "itkTimeSeriesDatabase.txx"

#endif
