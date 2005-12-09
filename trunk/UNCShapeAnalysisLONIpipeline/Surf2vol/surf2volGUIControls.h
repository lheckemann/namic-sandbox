#ifndef surf2volGUIControls_h
#define surf2volGUIControls_h

#include "surf2volGUI.h"
#include "convertBYU.h"
#include "Image3D.h"
#include <iostream>
#include <fstream>
#include <vector>


class surf2volGUIControls : public surf2volGUI
{ 
public:



struct byuinfostruct
{
  QString byuname;
  bool imageproperties;
  bool byuproperties;
  float byupixdims[3];
  float imagepixdims[3];
  int imagesize[3];
  bool outputchangeresolution;
  float outputpixdims[3];
  int saveas;
  int outputtype;
  QString outputname;
  int outputformat;
  QString outputsuffix;
  QString outputdirectory;
  // chooseoutputdirectory
  float outputvolumevalue;
  int subdivAxis;
  int subdivNum;
  double addFrameSize;
  QString subdivFile;
  QString subdivVarFile;
};

  
  surf2volGUIControls( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~surf2volGUIControls();
  
  typedef unsigned short PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  typedef itk::Image<unsigned char,3> ImageTypeChar;
  typedef ImageType::Pointer ImagePointer;
  typedef itk::ImageRegionIterator<ImageType> IteratorType;
  typedef itk::ImageFileReader<ImageType>   ReaderType;
  typedef itk::ImageFileWriter<ImageType>   WriterType;
  typedef itk::ImageFileWriter<ImageTypeChar>   WriterTypeChar;
  
  
  void AddBYU();
  void DeleteBYU(QListViewItem* item);
  void SelectBYU(QListViewItem* item);

  int GetIndex(QListViewItem* item);
  void Update();

  void SetBYUProperties();
  void SetImageProperties();
  void SetBYUPixDimX(const QString&);
  void SetBYUPixDimY(const QString&);
  void SetBYUPixDimZ(const QString&);
  void SetImageDimX(const QString&);
  void SetImageDimY(const QString&);
  void SetImageDimZ(const QString&);
  void SetImagePixDimX(const QString&);
  void SetImagePixDimY(const QString&);
  void SetImagePixDimZ(const QString&);
  void SetChangeResolution();
  void SetOutputPixdimX(const QString&);
  void SetOutputPixdimY(const QString&);
  void SetOutputPixdimZ(const QString&);
  void SetSaveAs(int);
  void SetOutputType(int);
  void SetOutputName(const QString &);
  void SetOutputFormat(int);
  void SetOutputSuffix(const QString &);
  void ChooseOutputDirectory();
  void SetOutputDirectory(const QString &);
  
  void Convert(int argc);

  std::vector<byuinfostruct> m_byulist;
  int m_index;
  QListViewItem* m_lastitem;

 protected:
  
  void subdivide_BBox (convertBYU * m_convert, byuinfostruct m_info );
  void subdivide_Planes (convertBYU * m_convert, byuinfostruct m_info,
                                                                   QString m_basename, QString m_outputformat);
  
};

#endif
