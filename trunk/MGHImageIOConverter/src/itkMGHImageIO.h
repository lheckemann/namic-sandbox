

#ifndef H_ITK_IMAGE_IO_H
#define H_ITK_IMAGE_IO_H

// STL includes
#include <string>
#include <vector>

// ITK includes
#include "itkImageIOBase.h"
#include "itkIOCommon.h"
#include "itkExceptionObject.h"
#include "itkByteSwapper.h"
#include "itkMetaDataObject.h"
#include "itkMatrix.h"

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_cross.h>

#include <zlib.h>

// variables used in the IO
//
// because of the templated functions, need to declare them here
// try to avoid name scoping
namespace fs
{
const int MRI_UCHAR = 0;
const int MRI_INT   = 1;
const int MRI_FLOAT = 3;
const int MRI_SHORT = 4;
const int MRI_TENSOR= 6;

const int FS_DIMENSION_HEADER_SIZE = sizeof(int) * 7;
const int FS_RAS_HEADER_SIZE = (sizeof(float) * 15) + sizeof(short);
const int FS_UNUSED_HEADER_SIZE = 256 - FS_RAS_HEADER_SIZE;
const int FS_WHOLE_HEADER_SIZE =
    FS_RAS_HEADER_SIZE + FS_DIMENSION_HEADER_SIZE + FS_UNUSED_HEADER_SIZE;

const int TAG_OLD_COLORTABLE = 1;
const int TAG_OLD_USEREALRAS = 2;
const int TAG_CMDLINE = 3;
const int TAG_USEREALRAS = 4;
const int TAG_COLORTABLE = 5;

const int TAG_OLD_SURF_GEOM = 20;
const int TAG_SURF_GEOM = 21;

const int TAG_OLD_MGH_XFORM = 30;
const int TAG_MGH_XFORM = 31;
const int TAG_AUTO_ALIGN = 33;

std::string CreateTag(int tag, std::string contents);

};

namespace itk
{

class OutputStreamWrapper;

class ITK_EXPORT MGHImageIO : public ImageIOBase
{
public:
    typedef MGHImageIO              Self;
    typedef ImageIOBase             Superclass;
    typedef SmartPointer<Self>      Pointer;

    /** Method for creation through the object factory **/
    itkNewMacro(Self);
    /** RTTI (and related methods) **/
    itkTypeMacro(MGHImageIO, Superclass);


    /**--------------- Read the data----------------- **/
    virtual bool CanReadFile(const char* FileNameToRead);
    /* Set the spacing and dimension information for the set file name */
    virtual void ReadImageInformation();
    /* Read the data from the disk into provided memory buffer */
    virtual void Read(void* buffer);

    /**---------------Write the data------------------**/

    virtual bool CanWriteFile(const char* FileNameToWrite);
    /* Set the spacing and dimension information for the set file name */
    virtual void WriteImageInformation();
    /* Write the data to the disk from the provided memory buffer */
    virtual void Write(const void* buffer);

protected:
    MGHImageIO();
    ~MGHImageIO();
    void PrintSelf(std::ostream& os, Indent indent) const;

    void ReadVolumeHeader(gzFile fp);

private:
    // processes the actual data buffer
    void SwapBytesIfNecessary(void* buffer,
                              unsigned long numberOfPixels);

    template<class Writer>
    void WriteHeader(Writer& writer);

    template<class Writer>
    void WriteData(Writer& writer, const void* buffer);

    void PermuteFrameValues(const void* buffer, char* tempmemory);

    unsigned int GetComponentSize() const;

}; // end class declaration

//-----------------------------------
//
// WriteHeader
//
//-----------------------------------
template<class Writer>
void
MGHImageIO::WriteHeader(Writer& writer)
{
    const int mghVersion = 1;

    // version
    writer.Write( mghVersion );

    // dimensions
    for (unsigned int ui=0; ui<3; ++ui)
        writer.Write( (int)m_Dimensions[ui] );

    // nframes
    writer.Write( (int)m_NumberOfComponents );

    // type
    switch ( m_ComponentType )
    {
    case UCHAR:
        writer.Write( fs::MRI_UCHAR ); break;
    case INT:
        writer.Write( fs::MRI_INT ); break;
    case FLOAT:
        writer.Write( fs::MRI_FLOAT ); break;
    case SHORT:
        writer.Write( fs::MRI_SHORT ); break;

        // DJ -- added these cases to make the compiler shut up
    case CHAR:
    case DOUBLE:
    case USHORT:
    case UNKNOWNCOMPONENTTYPE:
    case UINT:
    case ULONG:
    case LONG:
        itkExceptionMacro( << " Unhandled component type ");
        break;

    }

    // dof !?! -> default value = 1
    writer.Write( (int)1 );

    // write RAS and voxel size info
    writer.Write( (short)1 );

    // spacing
    for (unsigned int ui=0; ui<3; ++ui)
        writer.Write( (float)m_Spacing[ui] );

    //=================================================
    // get directions matrix
    std::vector<std::vector<double> > vvRas;
    for (unsigned int ui=0; ui<3; ++ui) vvRas.push_back( GetDirection(ui) );
    // transpose data before writing it
    std::vector<float> vBufRas;
    // transpose matrix
    for (unsigned int ui=0; ui<3; ++ui)
        for (unsigned int uj=0; uj<3; ++uj)
            vBufRas.push_back( (float)vvRas[ui][uj] );
    //==================================================

    for (std::vector<float>::const_iterator cit = vBufRas.begin();
            cit != vBufRas.end(); ++cit )
        writer.Write( *cit );

    // write c_r, c_a, c_s
    // defined as origin + DC x resolution x ( dim0/2 , dim1/2, dim2/2 )
    //
    float crasBuf;
    for (unsigned int ui=0; ui<3; ++ui)
    {
        crasBuf = m_Origin[ui];
        for (unsigned int uj=0; uj<3; ++uj)
            crasBuf += vvRas[uj][ui]*m_Spacing[uj]*(float)m_Dimensions[uj]/2.0f;
        writer.Write( crasBuf );
    } // next ui

    // fill the rest of the buffer with zeros
    char* buffer = new char[ fs::FS_UNUSED_HEADER_SIZE *sizeof(char) ];
    memset( buffer, 0, fs::FS_UNUSED_HEADER_SIZE *sizeof(char) );
    writer.WriteBuffer( buffer, fs::FS_UNUSED_HEADER_SIZE );
    delete[] buffer;
}

//--------------------------------------------
//
// WriteData
//
//--------------------------------------------
template<class Writer>
void
MGHImageIO::WriteData(Writer& writer,
                      const void* buffer)
{
    // swap bytes if necessary
    const unsigned long int numvalues = m_Dimensions[0]
                                        * m_Dimensions[1] * m_Dimensions[2] * m_NumberOfComponents;
    const unsigned long int numbytes = this->GetComponentSize() * numvalues;

    char* tempmemory = new char[numbytes];

    // re-arrange data in frames
    if ( m_NumberOfComponents > 1 )
    {
        PermuteFrameValues(buffer, tempmemory);
    }
    else
        memcpy(tempmemory, buffer, numbytes);


    this->SwapBytesIfNecessary(tempmemory,numvalues);

    writer.WriteBuffer( tempmemory, numbytes );

    delete[] tempmemory;

    itk::MetaDataDictionary &thisDict=this->GetMetaDataDictionary();

    float fScanBuffer;

    if ( ExposeMetaData<float>(thisDict, "TR", fScanBuffer) )
    {
        writer.Write(fScanBuffer);
        if ( ExposeMetaData<float>(thisDict, "FlipAngle", fScanBuffer) )
        {
            writer.Write(fScanBuffer);
            if ( ExposeMetaData<float>(thisDict, "TE", fScanBuffer) )
            {
                writer.Write(fScanBuffer);
                if ( ExposeMetaData<float>(thisDict, "TI", fScanBuffer) )
                {
                    writer.Write(fScanBuffer);
                    if ( ExposeMetaData<float>(thisDict, "FoV", fScanBuffer) )
                        writer.Write(fScanBuffer);
                } // end TI
            } // end TE
        } // end FlipAngle
    } // end TR

    //========================
    // write tags at the end of file

    // xform
    if ( thisDict.HasKey( "XForm" ) )
    {
        std::string strFileName;
        ExposeMetaData( thisDict, "XForm",
                        strFileName );
        std::string strTagData = fs::CreateTag( fs::TAG_MGH_XFORM,
                                                strFileName );
        writer.WriteBuffer( strTagData.c_str(),
                            strTagData.size() );
    } // end xform

    // command lines
    if ( thisDict.HasKey( "CmdLines" ) )
    {
        typedef std::vector<std::string> StringVector;
        StringVector vstr;
        ExposeMetaData( thisDict, "CmdLines",
                        vstr );
        for ( typename StringVector::const_iterator cit = vstr.begin();
                cit != vstr.end(); ++cit )
        {
            std::string strTagData = fs::CreateTag( fs::TAG_CMDLINE,
                                                    *cit );
            writer.WriteBuffer( strTagData.c_str(),
                                strTagData.size() );
        } // next cit
    } // end command lines

    // autoalign
    if ( thisDict.HasKey( "AutoAlign" ) )
    {
        typedef std::vector<double> DoubleVector;
        DoubleVector dvec;
        ExposeMetaData( thisDict, "AutoAlign", dvec );
        if ( dvec.size() != 16 )
        {
            itkWarningMacro(<< " MGHImageIO::Write - AutoAlign dictionary tag should have length 16");
        }
        else
        {
            std::ostringstream oss;
            oss << "AutoAlign";
            for ( typename DoubleVector::const_iterator cit = dvec.begin();
                    cit != dvec.end(); ++cit )
            {
                oss << " " << *cit;
            } // next cit
            std::string strTagData = fs::CreateTag( fs::TAG_AUTO_ALIGN,
                                                    oss.str() );
            writer.WriteBuffer( strTagData.c_str(),
                                strTagData.size() );
        }
    } // end auto align


}   // end WriteData

} // end namespace itk


#endif // H_ITK_IMAGE_IO_H



