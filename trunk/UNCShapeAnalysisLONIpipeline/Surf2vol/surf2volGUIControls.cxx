#include "surf2volGUIControls.h"
#include <qfiledialog.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlistview.h>
#include <iostream>
#include "convertBYU.h"
#include <limits.h>
#include <cmath>

// The probability computation library of John Burkardt, GREAT library btw
#include "prob.h"

#include "itkCastImageFilter.h"

surf2volGUIControls::surf2volGUIControls( QWidget* parent,  const char* name, WFlags fl )
    : surf2volGUI( parent, name,  fl )
{
  g_listbyu->setSorting(-1);
  m_index = -1;
  m_lastitem = NULL;
}


surf2volGUIControls::~surf2volGUIControls()
{
}

void surf2volGUIControls::AddBYU()
{
  QListViewItem* item;
  QStringList filelist( QFileDialog::getOpenFileNames( "Surface BYU (*.byu)",QString::null, this ) );
  for ( QStringList::Iterator it = filelist.begin(); it != filelist.end(); ++it ) 
    {
      QString path = (*it).latin1();
      QString filename = path.mid(path.findRev("/")+1);
      path = path.mid(0,path.findRev("/")+1);
      if (m_lastitem == NULL)
        item = new QListViewItem(g_listbyu);
      else
        item = new QListViewItem(g_listbyu,m_lastitem);
      
      item->setText( 0, filename);
      item->setText( 1, path);

      byuinfostruct m_byuproperties;

      // INPUT parameters
      m_byuproperties.byuproperties = g_byuproperties->isChecked();
      if (g_byux->text().isEmpty()) m_byuproperties.byupixdims[0] = 0;
      else m_byuproperties.byupixdims[0] = atof(g_byux->text());
      if (g_byuy->text().isEmpty()) m_byuproperties.byupixdims[1] = 0;
      else m_byuproperties.byupixdims[1] = atof(g_byuy->text());  
      if (g_byuz->text().isEmpty()) m_byuproperties.byupixdims[2] = 0;
      else m_byuproperties.byupixdims[2] = atof(g_byuz->text());
      
      m_byuproperties.imageproperties = g_imageproperties->isChecked();
      if (g_imagepixdimx->text().isEmpty()) m_byuproperties.imagepixdims[0] = 0;
      else m_byuproperties.imagepixdims[0] = atof(g_imagepixdimx->text());
      if (g_imagepixdimy->text().isEmpty()) m_byuproperties.imagepixdims[1] = 0;
      else m_byuproperties.imagepixdims[1] = atof(g_imagepixdimy->text());
      if (g_imagepixdimz->text().isEmpty()) m_byuproperties.imagepixdims[2] = 0;
      else m_byuproperties.imagepixdims[2] = atof(g_imagepixdimz->text());
      if (g_imagesizex->text().isEmpty()) m_byuproperties.imagesize[0] = 0;
      else m_byuproperties.imagesize[0] = atoi(g_imagesizex->text());
      if (g_imagesizey->text().isEmpty()) m_byuproperties.imagesize[1] = 0;
      else m_byuproperties.imagesize[1] = atoi(g_imagesizey->text());
      if (g_imagesizez->text().isEmpty()) m_byuproperties.imagesize[2] = 0;
      else m_byuproperties.imagesize[2] = atoi(g_imagesizez->text());
      

      // OUTPUT parameters
      m_byuproperties.outputchangeresolution = g_outputchangeresolution->isChecked();
      if (g_outputpixdimx->text().isEmpty()) m_byuproperties.outputpixdims[0] = 0;
      else m_byuproperties.outputpixdims[0] = atof(g_outputpixdimx->text());
      if (g_outputpixdimy->text().isEmpty()) m_byuproperties.outputpixdims[1] = 0;
      else m_byuproperties.outputpixdims[1] = atof(g_outputpixdimy->text());
      if (g_outputpixdimz->text().isEmpty()) m_byuproperties.outputpixdims[2] = 0;
      else m_byuproperties.outputpixdims[2] = atof(g_outputpixdimz->text());
    
      m_byuproperties.saveas = g_outputsaveas->currentItem();
      m_byuproperties.outputtype = g_outputtype->currentItem();
      m_byuproperties.outputname = g_outputname->text();
      m_byuproperties.outputformat = g_outputformat->currentItem();
      m_byuproperties.outputsuffix = g_outputsuffix->text();
      m_byuproperties.outputdirectory = g_outputdirectory->text();
      m_byuproperties.outputvolumevalue = 0.0;
      m_byuproperties.subdivAxis = 0;
      m_byuproperties.subdivNum = 0;
      m_byuproperties.addFrameSize = 0.1;
      m_byuproperties.subdivFile = "";
      m_byuproperties.subdivVarFile = "";

      m_byuproperties.byuname = (*it).latin1();
      m_byulist.push_back(m_byuproperties);
      m_lastitem = item;
      g_listbyu->setSelected(item,true);
    }
  g_addbyu->setDown(false);
  m_index = 0;
}

void surf2volGUIControls::DeleteBYU(QListViewItem* item)
{
  g_listbyu->takeItem(item);
  std::vector<byuinfostruct>::iterator m_iter;
  int offset = 0;
  for (m_iter = m_byulist.begin();m_iter != m_byulist.end();m_iter++){
    if (offset == m_index){
      m_byulist.erase(m_iter);
      return;
    }
    offset++;
  }
}

void surf2volGUIControls::SelectBYU(QListViewItem* item)
{  
  m_index = GetIndex(item);
  if (m_index != -1)
    {
      g_byuproperties->setChecked(m_byulist[m_index].byuproperties);
      g_imageproperties->setChecked(m_byulist[m_index].imageproperties);
      g_byux->setText(QString("%1").arg(m_byulist[m_index].byupixdims[0]));
      g_byuy->setText(QString("%1").arg(m_byulist[m_index].byupixdims[1]));
      g_byuz->setText(QString("%1").arg(m_byulist[m_index].byupixdims[2]));
      g_imagesizex->setText(QString("%1").arg(m_byulist[m_index].imagesize[0]));
      g_imagesizey->setText(QString("%1").arg(m_byulist[m_index].imagesize[1]));
      g_imagesizez->setText(QString("%1").arg(m_byulist[m_index].imagesize[2]));
      g_imagepixdimx->setText(QString("%1").arg(m_byulist[m_index].imagepixdims[0]));
      g_imagepixdimy->setText(QString("%1").arg(m_byulist[m_index].imagepixdims[1]));
      g_imagepixdimz->setText(QString("%1").arg(m_byulist[m_index].imagepixdims[2]));
      
      g_outputchangeresolution->setChecked(m_byulist[m_index].outputchangeresolution);
      g_outputpixdimx->setText(QString("%1").arg(m_byulist[m_index].outputpixdims[0]));
      g_outputpixdimy->setText(QString("%1").arg(m_byulist[m_index].outputpixdims[1]));
      g_outputpixdimz->setText(QString("%1").arg(m_byulist[m_index].outputpixdims[2]));
      g_outputsaveas -> setCurrentItem(m_byulist[m_index].saveas);
      g_outputtype->setCurrentItem(m_byulist[m_index].outputtype);
      g_outputname->setText(m_byulist[m_index].outputname);
      g_outputformat->setCurrentItem(m_byulist[m_index].outputformat);
      g_outputsuffix->setText(m_byulist[m_index].outputsuffix);
      g_outputdirectory->setText(m_byulist[m_index].outputdirectory);

      g_outputvolumevalue->setText(QString("%1").arg(m_byulist[m_index].outputvolumevalue));
  
      //m_byuproperties.subdivAxis is not updated on GUI
      //m_byuproperties.subdivNum  is not updated on GUI
      //m_byuproperties.addFrameSize  is not updated on GUI
      //m_byuproperties.subdivFile  is not updated on GUI
      //m_byuproperties.subdivVarFile  is not updated on GUI

      Update();
    }
}  

int surf2volGUIControls::GetIndex(QListViewItem* item)
{
  int offset = 0;
  if (item != NULL){
    QListViewItem* m_item = g_listbyu->firstChild();
    while (m_item)
      {
  if (m_item == item)
    return offset;
  
  offset++;
  m_item = m_item->nextSibling();
      }
  }
  return -1;
}

void surf2volGUIControls::Update()  
{
  if (g_sameproperties->isChecked()){
    for (unsigned int i=0;i<m_byulist.size();i++){

      // input parameters
      m_byulist[i].imageproperties = g_imageproperties->isChecked();
      m_byulist[i].byuproperties = g_byuproperties->isChecked();
      m_byulist[i].byupixdims[0] = atof(g_byux->text());
      m_byulist[i].byupixdims[1] = atof(g_byuy->text());
      m_byulist[i].byupixdims[2] = atof(g_byuz->text());
      m_byulist[i].imagepixdims[0] = atof(g_imagepixdimx->text());
      m_byulist[i].imagepixdims[1] = atof(g_imagepixdimy->text());
      m_byulist[i].imagepixdims[2] = atof(g_imagepixdimz->text());
      m_byulist[i].imagesize[0] = atoi(g_imagesizex->text());
      m_byulist[i].imagesize[1] = atoi(g_imagesizey->text());
      m_byulist[i].imagesize[2] = atoi(g_imagesizez->text());

      // output parameters
      m_byulist[i].outputchangeresolution = g_outputchangeresolution->isChecked();
      m_byulist[i].outputpixdims[0] = atof(g_outputpixdimx->text());
      m_byulist[i].outputpixdims[1] = atof(g_outputpixdimy->text());
      m_byulist[i].outputpixdims[2] = atof(g_outputpixdimz->text());
      m_byulist[i].saveas = g_outputsaveas->currentItem();
      m_byulist[i].outputtype = g_outputtype->currentItem();
      m_byulist[i].outputname = g_outputname->text();
      m_byulist[i].outputformat = g_outputformat->currentItem();
      m_byulist[i].outputsuffix = g_outputsuffix->text();
      m_byulist[i].outputdirectory = g_outputdirectory->text();
      m_byulist[i].outputvolumevalue = 0.0;
      m_byulist[i].subdivAxis = 0;
      m_byulist[i].subdivNum = 0;
      m_byulist[i].addFrameSize = 0.1;
      m_byulist[i].subdivFile = "";
      m_byulist[i].subdivVarFile = "";
   
   } // for
  } // if
}

void surf2volGUIControls::SetBYUProperties()
{
  g_imageproperties->setChecked(!g_byuproperties->isChecked());
  if (m_index != -1){
    m_byulist[m_index].byuproperties = g_byuproperties->isChecked();
    SetImageProperties();
    Update();
  }
}

void surf2volGUIControls::SetImageProperties()
{
  if (g_imageproperties->isChecked()){
    //    g_outputimage->setChecked(true);
    //    SetOutputImageSave();
    //    g_outputimage->setDisabled(false);
    //    g_outputmask_onebyone->setDisabled(false);
    //    g_outputmask_all->setDisabled(false);
  }
  else{
    //  g_outputimage->setChecked(false);      
    //    SetOutputImageSave();
    //    g_outputimage->setDisabled(true);
    //    g_outputmask_onebyone->setChecked(false);
    //    SetSaveMaskOnebyOne();
    //    g_outputmask_onebyone->setDisabled(true);
    //    g_outputmask_all->setChecked(false);
    //    SetSaveMaskAll();
    //    g_outputmask_all->setDisabled(true);
  }  

  g_byuproperties->setChecked(!g_imageproperties->isChecked());

  if (m_index != -1){
    m_byulist[m_index].imageproperties = g_imageproperties->isChecked();
    Update();
  }
}

void surf2volGUIControls::SetBYUPixDimX(const QString& pixdim)
{
  if (m_index != -1){
    m_byulist[m_index].byupixdims[0] = atof(pixdim);
    Update();
  }
}

void surf2volGUIControls::SetBYUPixDimY(const QString& pixdim)
{
  if (m_index != -1){
    m_byulist[m_index].byupixdims[1] = atof(pixdim);
    Update();
  }
}

void surf2volGUIControls::SetBYUPixDimZ(const QString& pixdim)
{
  if (m_index != -1){
    m_byulist[m_index].byupixdims[2] = atof(pixdim);
    Update();
  }
}

void surf2volGUIControls::SetImageDimX(const QString& pixdim)
{
  if (m_index != -1){
    m_byulist[m_index].imagesize[0] = atoi(pixdim);
    Update();
  }
}

void surf2volGUIControls::SetImageDimY(const QString& pixdim)
{
  if (m_index != -1){
    m_byulist[m_index].imagesize[1] = atoi(pixdim);
    Update();
  }
}

void surf2volGUIControls::SetImageDimZ(const QString& pixdim)
{
  if (m_index != -1){
    m_byulist[m_index].imagesize[2] = atoi(pixdim);
    Update();
  }
}

void surf2volGUIControls::SetImagePixDimX(const QString& pixdim)
{ 
  if (m_index != -1){
    m_byulist[m_index].imagepixdims[0] = atof(pixdim);
    Update();
  }
}

void surf2volGUIControls::SetImagePixDimY(const QString& pixdim)
{
  if (m_index != -1){
    m_byulist[m_index].imagepixdims[1] = atof(pixdim);
    Update();
  }
}

void surf2volGUIControls::SetImagePixDimZ(const QString& pixdim)
{
  if (m_index != -1){
    m_byulist[m_index].imagepixdims[2] = atof(pixdim);
    Update();
  }
}

void surf2volGUIControls::SetChangeResolution()
{
  if (m_index != -1){
    m_byulist[m_index].outputchangeresolution = g_outputchangeresolution->isChecked();
    Update();
  }
}
  
void surf2volGUIControls::SetOutputPixdimX(const QString& x)
{  
  if (m_index != -1){
    m_byulist[m_index].outputpixdims[0] = atof(x);
    Update();
  }
}

void surf2volGUIControls::SetOutputPixdimY(const QString& y)
{
  if (m_index != -1){
    m_byulist[m_index].outputpixdims[1] = atof(y);
    Update();
  }
}
  
void surf2volGUIControls::SetOutputPixdimZ(const QString& z)
{
  if (m_index != -1){
    m_byulist[m_index].outputpixdims[2] = atof(z);
    Update();
  }  
}

void surf2volGUIControls::SetSaveAs(int offset)
{
  if (m_index != -1){
    m_byulist[m_index].saveas = offset;
    Update();
  }
}

void surf2volGUIControls::SetOutputType(int offset)
{
  if (m_index != -1){
    m_byulist[m_index].outputtype = offset;
    Update();
  }
}

void surf2volGUIControls::SetOutputName(const QString& name)
{
  if (m_index != -1){
    m_byulist[m_index].outputname = name;
    Update();
  }
}

void surf2volGUIControls::SetOutputFormat(int offset)
{  
  if (m_index != -1){
    m_byulist[m_index].outputformat = offset;
    Update();
  }
}

void surf2volGUIControls::SetOutputSuffix(const QString& suffix)
{
  if (m_index != -1){
    m_byulist[m_index].outputsuffix = suffix;
    Update();
  }
}

void surf2volGUIControls::ChooseOutputDirectory()
{
  QString s = QFileDialog::getExistingDirectory ();
  if (!s.isEmpty())
    g_outputdirectory->setText(s);

  g_chooseoutputdirectory->setDown(false);
}

void surf2volGUIControls::SetOutputDirectory(const QString& name)
{
  if (m_index != -1){
    m_byulist[m_index].outputdirectory = name;
    Update();
  }
}
    
void surf2volGUIControls::Convert(int argc)
{
  // pour gerer quelles sont les byu a ecrire sur le meme masque de segmentation
  std::vector<int> m_allmaskindex;
  std::vector<convertBYU*> m_convertlist;

  // declaration de la variable volume
  double volume = 0.0;

  // variables utiles
  QString m_outputpath = "";
  QString m_outputname = "";
  QString m_outputformat = "";
  QString m_name = "";
  QString m_basename = "";

  // on sauve ceux qu'il faut sauver individuellement
  for (unsigned int count=0;count<m_byulist.size();count++) {
    byuinfostruct m_info =  m_byulist[count];
    convertBYU *  m_convert = new convertBYU;
    
    // il faut affecter les bonnes dimensions
    int _imDimX, _imDimY, _imDimZ;
    double _deltaxi, _deltayi, _deltazi;
    double _deltaxf, _deltayf, _deltazf;
    
    if(m_info.byuproperties){
      _imDimX = 0; _imDimY = 0; _imDimZ = 0;
      _deltaxi=m_info.byupixdims[0]; 
      _deltayi=m_info.byupixdims[1];
      _deltazi=m_info.byupixdims[2];
      _deltaxf=m_info.byupixdims[0]; 
      _deltayf=m_info.byupixdims[1];
      _deltazf=m_info.byupixdims[2];
    } else{
      _imDimX=m_info.imagesize[0];
      _imDimY=m_info.imagesize[1];
      _imDimZ=m_info.imagesize[2]; 
      _deltaxi=m_info.imagepixdims[0]; 
      _deltayi=m_info.imagepixdims[1];
      _deltazi=m_info.imagepixdims[2];
      _deltaxf=m_info.imagepixdims[0]; 
      _deltayf=m_info.imagepixdims[1];
      _deltazf=m_info.imagepixdims[2];
    }
    
    if(m_info.outputchangeresolution){
      _deltaxf=m_info.outputpixdims[0];
      _deltayf=m_info.outputpixdims[1];
      _deltazf=m_info.outputpixdims[2];
    }
     
    m_convert->SetDimensions(_imDimX, _imDimY, _imDimZ,
                                 _deltaxi, _deltayi, _deltazi,
                                 _deltaxf, _deltayf, _deltazf, 
                                 m_info.addFrameSize);
    
    // lecture BYU
    m_convert->ReadInput(m_info.byuname.latin1());
    
    // Writing output
    int index = m_info.byuname.findRev("/");
    int extindex = m_info.byuname.findRev(".");
    
    // output name 
    if (m_info.outputdirectory.isEmpty())
      m_outputpath =   m_info.byuname.mid(0,index);
    else
      m_outputpath = m_info.outputdirectory;
    
    if (m_info.outputname.isEmpty())
      m_outputname = m_info.byuname.mid(index+1,extindex-index-1);  
    else
      m_outputname =   m_info.outputname;
    if(m_info.outputformat==0)         { m_outputformat = ".gipl";
    } else if (m_info.outputformat==1) { m_outputformat = ".hdr";
    } else if (m_info.outputformat==2) { m_outputformat = ".mha";
    } else if (m_info.outputformat==3) { m_outputformat = ".raw3";
    } else                             { m_outputformat = ".gipl";
    }

    m_basename = m_outputpath +   "/" + m_outputname + m_info.outputsuffix;
    if (argc == 1){
      m_name = m_outputpath +   "/" + m_outputname + m_info.outputsuffix + 
         g_outputformat->text(m_info.outputformat);
      m_outputformat=g_outputformat->text(m_info.outputformat);
      g_outputvolumevalue->setText(QString("%1").arg(m_byulist[m_index].outputvolumevalue));
    }
    else{ 
      m_name = m_outputpath +    "/" + m_outputname +  m_info.outputsuffix + m_outputformat;
      std::cout<<"Volume "<<m_name.latin1()<<" : "<<volume<<std::endl;
    }
    
    // Convert 
    m_convert->FillP(volume);
    m_byulist[count].outputvolumevalue = volume;

    // save transform from input coordinate system to output coordinate system
    double origin[3];
    origin[0] = origin[1] =origin[2] = 0.0;
    m_convert->TransformWorldToImage(origin);
    double scale[3];
    scale[0] = scale[1] = scale[2] = 1.0;
    m_convert->TransformWorldToImage(scale);
    for (int i = 0; i < 3; i++) scale[i] = scale[i] - origin[i]; 

    QString m_name_transfile = m_outputpath +    "/" + m_outputname +  ".trans.txt";
    std::ofstream efile(m_name_transfile.latin1(), std::ios::out);  
    if (!efile) {
      std::cerr << "Error: open of file \"" << m_name_transfile.latin1()  << "\" failed." << std::endl;
    } else {
      efile.precision(10);
      efile << "ByuOriginInImage = { " << origin[0] << " , " 
             << origin[1] << " , " << origin[2] << "};" << std::endl;
      efile << "ByuScaleInImage = { " << scale[0] << " , " 
             << scale[1] << " , " << scale[2] << "};" << std::endl;
    }
    efile.close();

    //Subdivide output if necessary
    if (m_info.subdivNum) {
      subdivide_BBox(m_convert, m_info);
    } else if ( ! m_info.subdivFile.isEmpty() ) {
      subdivide_Planes(m_convert, m_info, m_basename, m_outputformat);
    }
    
    if (m_info.saveas == 0){
      m_convert->WriteVolume(m_name.latin1(),m_info.outputtype);
    }
    if ((m_info.saveas == 1) || (m_info.saveas == 2)){
      if ((m_info.imagesize[0]==0) || (m_info.imagesize[1]==0) || (m_info.imagesize[2]==0) || 
        (m_info.imagepixdims[0]==0) || (m_info.imagepixdims[1]==0) || (m_info.imagepixdims[2]==0))
      std::cout << "Can't convert into segmentation mask ... specify correct input dimensions!" 
         <<std::endl;
      else{      
        //  A sauver un par un ou sur le meme masque de segmentation :
        if (m_info.saveas == 2) {
          m_allmaskindex.push_back(count);
          m_convertlist.push_back(m_convert);
        }
        else{
          m_convert->WriteImage(m_name.latin1(),m_info.outputtype);  
          delete m_convert;
        } // else m_info.allatonce
      }
    } // m_info.outputimagesave
  } // for
  
  
  // ecriture des masks dans une image commune
  
  // on recupere les dimensions de la premiere image demandee et on construit l'image
  if (m_allmaskindex.size() != 0){
    byuinfostruct m_info = m_byulist[m_allmaskindex[0]];
    if ((m_info.imagesize[0]==0) || (m_info.imagesize[1]==0) || (m_info.imagesize[2]==0) || 
  (m_info.imagepixdims[0]==0) || (m_info.imagepixdims[1]==0) || (m_info.imagepixdims[2]==0))
      std::cout << "Can't convert into segmentation mask ... Please specify correct input dimensions!" <<std::endl;
    else{
      int m_maxgreyvalue;
      if (m_info.outputtype == 0)
       m_maxgreyvalue = 65535;
      else
       m_maxgreyvalue = 255;
      
      typedef itk::Image<unsigned short,3> ImageType;
      typedef ImageType::Pointer ImagePointer;
      typedef itk::ImageRegionIterator<ImageType> IteratorType;
      ImagePointer m_outputimage = ImageType::New();
      
      int _imDimX, _imDimY, _imDimZ;
      double _deltaxi, _deltayi, _deltazi;
      double _deltaxf, _deltayf, _deltazf;
      
      if(m_info.byuproperties){
       _imDimX = 0; _imDimY = 0; _imDimZ = 0;
       _deltaxi=m_info.byupixdims[0]; 
       _deltayi=m_info.byupixdims[1];
       _deltazi=m_info.byupixdims[2];
       _deltaxf=m_info.byupixdims[0]; 
       _deltayf=m_info.byupixdims[1];
       _deltazf=m_info.byupixdims[2];
      }
      else{
       _imDimX=m_info.imagesize[0];
       _imDimY=m_info.imagesize[1];
       _imDimZ=m_info.imagesize[2]; 
       _deltaxi=m_info.imagepixdims[0]; 
       _deltayi=m_info.imagepixdims[1];
       _deltazi=m_info.imagepixdims[2];
       _deltaxf=m_info.imagepixdims[0]; 
       _deltayf=m_info.imagepixdims[1];
       _deltazf=m_info.imagepixdims[2];
      }
      if(m_info.outputchangeresolution){
       _deltaxf=m_info.outputpixdims[0];
       _deltayf=m_info.outputpixdims[1];
       _deltazf=m_info.outputpixdims[2];
      }  
      
      int imagesize[3];
      imagesize[0] = (int)floor(_imDimX*_deltaxi/_deltaxf);
      imagesize[1] = (int)floor(_imDimY*_deltayi/_deltayf); 
      imagesize[2] = (int)floor(_imDimZ*_deltazi/_deltazf); 
      
      ImageType::SpacingType  spacing;
      spacing[0] = _deltaxf;
      spacing[1] = _deltayf;
      spacing[2] = _deltazf;
    
      float values[3];
      values[0]= spacing[0]; 
      values[1]= spacing[1];
      values[2]= spacing[2];
      float origin_x= ((imagesize[0]/2)*values[0]*(-1));
      float origin_y= ((imagesize[1]/2)*values[1]*(-1));
      float origin_z= ((imagesize[0]/2)*values[2]*(-1));
      float origin[3] = {origin_x, origin_y, origin_z};
      
      ImageType::RegionType region;
      ImageType::SizeType size;
      size[0]= imagesize[0];
      size[1]= imagesize[1];
      size[2]= imagesize[2];
      region.SetSize( size );
      m_outputimage->SetRegions( region );
      m_outputimage->Allocate();
      m_outputimage->SetOrigin(origin);
      m_outputimage->SetSpacing(values);
      
      // INIT
      IteratorType m_outputitimageINIT(m_outputimage,m_outputimage->GetLargestPossibleRegion());
      m_outputitimageINIT.GoToBegin();
      ImageType::SizeType m_imagesize = (m_outputimage->GetLargestPossibleRegion().GetSize());
      for (unsigned int z=0;z<m_imagesize[2];z++)
        for (unsigned int y=0;y<m_imagesize[1];y++)
          for (unsigned int x=0;x<m_imagesize[0];x++){
            m_outputitimageINIT.Set(0);
            ++m_outputitimageINIT;
          }
      
      // SAUVEGARDE sur le MEME MASQUE
      std::vector<convertBYU*>::iterator m_iter;
      int offset = 0;
      int maxVal = 0;// assures continuous numbering of the objects from different convertimages
      int startVal = 0;
      for (m_iter = m_convertlist.begin();m_iter != m_convertlist.end();m_iter++)  {
       unsigned int voxminx=(unsigned int)floor(m_convertlist[offset]->minx*_deltaxi/_deltaxf);
       unsigned int voxminy=(unsigned int)floor(m_convertlist[offset]->miny*_deltayi/_deltayf);  
       unsigned int voxminz=(unsigned int)floor(m_convertlist[offset]->minz*_deltazi/_deltazf);
       unsigned int voxmaxx=voxminx+m_convertlist[offset]->pixes[0];
       unsigned int voxmaxy=voxminy+m_convertlist[offset]->pixes[1];
       unsigned int voxmaxz=voxminz+m_convertlist[offset]->pixes[2];
  
       //Copy data to the ITK image
       IteratorType m_outputitimage(m_outputimage,m_outputimage->GetLargestPossibleRegion());
       m_outputitimage.GoToBegin();
       //ImageType::SizeType m_imagesize = (m_outputimage->GetLargestPossibleRegion().GetSize());
  
       for (unsigned int z=0;z<m_imagesize[2];z++)
         for (unsigned int y=0;y<m_imagesize[1];y++)
           for (unsigned int x=0;x<m_imagesize[0];x++){
             if ((x>=voxminx) && (x<voxmaxx) && (y>=voxminy) && (y<voxmaxy) && (z>=voxminz) && (z<voxmaxz)) {
              short voxval = m_convertlist[offset]->inImage->getVoxelValue(x-voxminx,y-voxminy,z-voxminz);
              if (voxval != 0)      m_outputitimage.Set(voxval + startVal);
              if (voxval > maxVal)  maxVal = voxval;
             }
             ++m_outputitimage;
           }
  
      offset++;
      startVal += maxVal;
      } // for m_iter = m_convertlist.....
      
      
      // NOM DU MASQUE DE SORTIE
      // Writing output
      int index = m_info.byuname.findRev("/");
      int extindex = m_info.byuname.findRev(".");
      
      // output name 
      if (m_info.outputdirectory.isEmpty())
       m_outputpath =   m_info.byuname.mid(0,index);
      else
       m_outputpath = m_info.outputdirectory;
      
      if (m_info.outputname.isEmpty())
       m_outputname = m_info.byuname.mid(index+1,extindex-index-1);  
      else
       m_outputname =   m_info.outputname;
      
      if (argc == 1){
       m_name = m_outputpath +  
         "/" + m_outputname + 
         m_info.outputsuffix + 
         g_outputformat->text(m_info.outputformat);
      } else {
       m_name = m_outputpath +  
         "/" + m_outputname + 
          m_info.outputsuffix + 
         m_outputformat;
      }
      
      if (m_info.outputtype ==0){
       WriterType::Pointer writer = WriterType::New();
       writer->SetFileName(m_name.latin1());
       writer->SetInput(m_outputimage);
       writer->Update();
      }
      else{
       typedef itk::CastImageFilter<ImageType,ImageTypeChar> CastFilterType;
       CastFilterType::Pointer castFilter = CastFilterType::New();
       castFilter->SetInput(m_outputimage);
       castFilter->Update();
       WriterTypeChar::Pointer writer = WriterTypeChar::New();
       writer->SetFileName(m_name.latin1());
       writer->SetInput(castFilter->GetOutput());
       writer->Update();  
      }
    }
  }
}

void surf2volGUIControls::subdivide_BBox (convertBYU * m_convert, byuinfostruct m_info)
{
  int subdivNum = m_info.subdivNum;
  // subdivide uniformly
  Image3D * labelImage = m_convert->inImage;
  int width;
  if (m_info.subdivAxis == 0) { width = labelImage->getXDim();}
  else if (m_info.subdivAxis == 1) { width = labelImage->getYDim();}
  else { width = labelImage->getZDim();}
  
  double border_factor = 1 / 1.1;
  int border = (int) ((double) width * (1 - border_factor) / 2);
  width = (int) ((double) width * border_factor);
  
  int increment = (int) ceil(width / subdivNum) + 1;
  std::cout << "subdiving the object into "<< subdivNum << " parts " 
             << "INC " << increment << std::endl;
  
  // Relabel image according to axis subdivision
  for (int z=0;z<labelImage->getZDim();z++) {
    for (int y=0;y<labelImage->getYDim();y++) {
      for (int x=0;x<labelImage->getXDim();x++){
         if (labelImage->getVoxelValue(x,y,z)) {
           unsigned short label;
           if (m_info.subdivAxis == 0) {
             label =  (x - border) / increment + 1;
           } else  if (m_info.subdivAxis == 1) {
             label =  (y - border) / increment + 1;
          } else {
             label = (z - border) / increment + 1;
           }
           labelImage->setVoxel(x,y,z,label);
         }
      }
    }
  }

}

void surf2volGUIControls::subdivide_Planes (convertBYU * m_convert, byuinfostruct m_info,
                                                 QString m_basename, QString m_outputformat)
{
  Image3D * labelImage = m_convert->inImage;

  char line[1000];
  double center[3 * 1000], norm[3 * 1000],  entry[6];
  int num_planes = 0;
  double origin[3];
  double scale[3];

  origin[0] = origin[1] =origin[2] = 0.0;
  m_convert->TransformWorldToImage(origin);
  scale[0] = scale[1] = scale[2] = 1.0;
  m_convert->TransformWorldToImage(scale);
  for (int i = 0; i < 3; i++) scale[i] = scale[i] - origin[i]; 

  // subdivide according to the selected file
  // read the file
  FILE * file = fopen(m_info.subdivFile,"r");
  if (!file) {
    std::cerr << "cannot open file for reading: " << m_info.subdivFile << std::endl;
    exit(1);
  }
  while (fgets(line,1000,file)) {
    int num_read = sscanf(line, " %lf %lf %lf %lf %lf %lf ",
                    &(entry[0]),&(entry[1]),&(entry[2]),
                    &(entry[3]),&(entry[4]),&(entry[5]));
    if (num_read == 6) {
      center[num_planes * 3 + 0] =  entry[0];
      center[num_planes * 3 + 1] =  entry[1];
      center[num_planes * 3 + 2] =  entry[2];
      norm[num_planes * 3 + 0]   =  entry[3];
      norm[num_planes * 3 + 1]   =  entry[4];
      norm[num_planes * 3 + 2]   =  entry[5];
      std::cout << "C:" << center[num_planes * 3 + 0] << "," << center[num_planes * 3 + 1] <<
         "," << center[num_planes * 3 + 2] << " --- ";
      std::cout << "N:" << norm[num_planes * 3 + 0] << "," << norm[num_planes * 3 + 1] <<
         "," << norm[num_planes * 3 + 2] << " ++++ ";
      m_convert->TransformWorldToImage(&(center[num_planes * 3]));
      std::cout << "Cimage:" << center[num_planes * 3 + 0] << "," << center[num_planes * 3 + 1] <<
         "," << center[num_planes * 3 + 2] << std::endl;
      num_planes ++;
    }
  }
  fclose(file);
  //m_info.subdivNum = num_planes;
  
  bool reorder = false;
  //check ordering and mirrored normals?, re-oder if necessary
  if (norm[num_planes/2 * 3 + 0] < 0) { // check a normal in the center
    std::cout << "mirroring normals" << std::endl;
    for (int i = 0; i < num_planes; i++) {
      for (int j = 0; j < 3 ; j++) {
         norm[i * 3 + j] = -  norm[i * 3 + j];
      }
    }
  }
  double dot_product = (center[3 + 0]-center[0 + 0]) * norm[0] +
    (center[3 + 1]-center[0 + 1])  * norm[1] + (center[3 + 2]-center[0 + 2])  * norm[2];
  if (dot_product < 0) {
    std::cout << "resorting planes" << std::endl;
    reorder = true;
    double swap_c[3], swap_n[3];
    for (int i = 0; i < num_planes/2; i++) {
      for (int j = 0; j < 3; j++) { 
         swap_c[j] = center[i*3 + j]; 
         swap_n[j] = norm[i*3 + j];}
      for (int j = 0; j < 3; j++) { 
         center[i*3 + j] = center[(num_planes - i - 1)*3 + j];
         norm[i*3 + j] = norm[(num_planes - i - 1)*3 + j];  }
      for (int j = 0; j < 3; j++) { 
         center[(num_planes - i - 1)*3 + j] = swap_c[j];
         norm[(num_planes - i - 1)*3 + j] = swap_n[j]; }
    } 
  }
  
  std::cout << "number of subdivision planes: " << num_planes << std::endl;
  
  // relabel the image according to plane subdivision
  for (int z=0;z<labelImage->getZDim();z++) {
    for (int y=0;y<labelImage->getYDim();y++) {
      for (int x=0;x<labelImage->getXDim();x++){
         if (labelImage->getVoxelValue(x,y,z)) {
           GreyValue label = 1;
           bool stopLabel = false;
           for (int i = 0; i < num_planes && !stopLabel; i++) {
             double diff[3];
             diff[0] = x - center[i * 3 + 0];
             diff[1] = y - center[i * 3 + 1];
             diff[2] = z - center[i * 3 + 2];
             double dot_product = diff[0] * norm[i * 3 + 0] +
               diff[1] * norm[i * 3 + 1] + diff[2] * norm[i * 3 + 2];
             if (dot_product >=  0) 
               label ++;
             else 
               stopLabel = true;
           }
           labelImage->setVoxel(x,y,z,label);
         }
      }
    }
  }
  
  if ( ! m_info.subdivVarFile.isEmpty() ) {
    // read variabilities
    FILE * varfile = fopen(m_info.subdivVarFile,"r");
    if (!varfile) {
      std::cerr << "cannot open file for reading: " << m_info.subdivVarFile << std::endl;
           exit(1);
    }
    char line[1000];
    double * variances = new double[num_planes];
    int cnt = 0;
    while (fgets(line,1000,varfile)) {
      double var;
      int num_read = sscanf(line, " %lf", &var);
      if (num_read == 1) {
         variances[cnt] = var;
         cnt++;
      }
    }
    fclose(varfile);
    // scale variabilities and reorder if necessary
    if (reorder) {
      for (int i = 0; i < num_planes/2; i++) {
         double swap = variances[i];
         variances[i] = variances[num_planes - i - 1];
         variances[num_planes - i - 1] = swap;
      }
    }
    double avgScale = pow(scale[0] * scale[1] * scale[2], 1.0 / 3); // geometric mean
    std::cout << "std_dev: " ;
    for (int i = 0; i < num_planes; i++) { 
      std::cout << variances[i] << "->";
      variances[i] = variances[i] * avgScale; 
      std::cout << variances[i] << "  ";
    }
    std::cout << std::endl;
    //preserve the label Image
    Image3D * labelSaveImage = new Image3D (labelImage->getXDim(), labelImage->getYDim(),
                                            labelImage->getZDim());
    if (!labelSaveImage) 
         std::cerr << "Cannot allocate the label preserve image" << std::endl;
      
    for (int z=0;z<labelImage->getZDim();z++) {
      for (int y=0;y<labelImage->getYDim();y++) {
         for (int x=0;x<labelImage->getXDim();x++){
           labelSaveImage->setVoxel(x,y,z, labelImage->getVoxelValue(x,y,z));
         }
      }
    }
    int num_regions = num_planes + 1;
    std::cout << "number of Regions: " << num_regions << std::endl;
    Image3D ** probImages = new Image3D * [num_regions];
    for (int i = 0; i < num_regions; i++) 
      probImages[i] = new Image3D (labelImage->getXDim(), labelImage->getYDim(),
                                   labelImage->getZDim());

    std::cout << "computing probability maps" << std::endl;
    // create and save a probability map for each subdivision plane
    GreyValue MaxProb = 255;
    GreyValue MaxIntProb = 10000;
    for (int cur_region = 0; cur_region < num_regions; cur_region++) {
      probImages[cur_region]->clear(); // clear the probability image

      for (int z=0;z<labelImage->getZDim();z++) {
         for (int y=0;y<labelImage->getYDim();y++) {
           for (int x=0;x<labelImage->getXDim();x++){
             GreyValue label = labelSaveImage->getVoxelValue(x,y,z);
             if (label) {
               double prob = 0;
               // uses Hessian formula for distance of a point P to a plane with 
               // normal and any point Q on the plane: D = n . (Q - P)
               // cdf of mu = 0, var = variances[i]^2 is used for the probability
               // away from each plane
               if (cur_region == 0 || cur_region == (num_regions - 1) ) {
                  int cur_plane = 0; // first plane
                              if (cur_region == num_regions - 1) 
                                 cur_plane = num_planes - 1; // last plane
                  double distance = fabs( 
                           norm[cur_plane * 3 + 0] * (center[cur_plane * 3 + 0] - x) +
                           norm[cur_plane * 3 + 1] * (center[cur_plane * 3 + 1] - y) +
                           norm[cur_plane * 3 + 2] * (center[cur_plane * 3 + 2] - z));
                  if (label == cur_region + 1) { 
                    // > 0.5 (positive) flank of cumulativ Gaussian of distance (0.5 ... 1)
                    prob = normal_cdf(distance, 0, variances[cur_plane]); 
                  } else { 
                    // < 0.5 (negative) flank of cumulativ Gaussian of distance (0 ... 0.5)
                    prob = normal_cdf(-distance, 0, variances[cur_plane]); 
                  }
               } else {
                  int cur_plane1 = cur_region - 1; // first plane
                  int cur_plane2 = cur_region; // second plane
                  double distance1 = fabs( 
                           norm[cur_plane1 * 3 + 0] * (center[cur_plane1 * 3 + 0] - x) +
                           norm[cur_plane1 * 3 + 1] * (center[cur_plane1 * 3 + 1] - y) +
                           norm[cur_plane1 * 3 + 2] * (center[cur_plane1 * 3 + 2] - z));
                  double distance2 = fabs( 
                           norm[cur_plane2 * 3 + 0] * (center[cur_plane2 * 3 + 0] - x) +
                           norm[cur_plane2 * 3 + 1] * (center[cur_plane2 * 3 + 1] - y) +
                           norm[cur_plane2 * 3 + 2] * (center[cur_plane2 * 3 + 2] - z));
                  if (label == cur_region + 1) { 
                    prob = normal_cdf(distance1, 0, variances[cur_plane1]) * 
                      normal_cdf(distance2, 0, variances[cur_plane2]); 
                  } else if (label < cur_region + 1){ 
                    prob = normal_cdf(-distance1, 0, variances[cur_plane1]) * 
                      normal_cdf(distance2, 0, variances[cur_plane2]); 
                  } else if (label > cur_region + 1){ 
                    prob = normal_cdf(distance1, 0, variances[cur_plane1]) * 
                      normal_cdf(-distance2, 0, variances[cur_plane2]); 
                  }
               }
               double prob_val = prob * MaxIntProb;
               probImages[cur_region]->setVoxel(x,y,z,(GreyValue) prob_val);
             }  
           }
         }
      }
    } 
    std::cout << "normalizing probability maps" << std::endl;
    // normalize the probability maps (each voxel sums to 1)
    for (int z=0;z<labelImage->getZDim();z++) {
      for (int y=0;y<labelImage->getYDim();y++) {
              for (int x=0;x<labelImage->getXDim();x++){
                double sum = 0;
                for (int cur_region = 0; cur_region < num_regions; cur_region++) {
                  sum = sum + probImages[cur_region]->getVoxelValue(x,y,z);
                }
                double norm_factor = (double) MaxProb / sum;
                for (int cur_region = 0; cur_region < num_regions; cur_region++) {
                  double prob_val = probImages[cur_region]->getVoxelValue(x,y,z) * norm_factor;
                  probImages[cur_region]->setVoxel(x,y,z,(GreyValue) prob_val);
                }
              }
      }
    }

    for (int cur_region = 0; cur_region < num_regions; cur_region++) {
      // copy the probability map
      for (int z=0;z<labelImage->getZDim();z++) {
         for (int y=0;y<labelImage->getYDim();y++) {
           for (int x=0;x<labelImage->getXDim();x++){
             labelImage->setVoxel(x,y,z, probImages[cur_region]->getVoxelValue(x,y,z));
           }
         }
      }
      //save the probability maps
      char probSuffix[100];
      sprintf(probSuffix,"_prob%03d",cur_region + 1);
      QString Probname = m_basename + probSuffix + m_outputformat;
      m_convert->WriteVolume(Probname.latin1(),m_info.outputtype);
    }

    //revert the label image
    for (int z=0;z<labelImage->getZDim();z++) {
      for (int y=0;y<labelImage->getYDim();y++) {
         for (int x=0;x<labelImage->getXDim();x++){
           labelImage->setVoxel(x,y,z, labelSaveImage->getVoxelValue(x,y,z));
         }
      }
    }

    delete variances;
    delete labelSaveImage;

  }
}
