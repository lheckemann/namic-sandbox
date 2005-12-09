#include "surf2volGUIControls.h"
#include <iostream>
#include <qstring.h>
#include <qapplication.h>
#include <qplatinumstyle.h>
#include <CommandLineArgumentParser.h>

using namespace std;

int main( int argc, char* argv[] ) 
{
  // Parse command line parameters
  CommandLineArgumentParser parser;
  // input
  parser.AddOption("-in1",1);  // obligatoire
  parser.AddOption("-in2",1);  // obligatoire
  parser.AddOption("-inpixdims",3); // obligatoire
  parser.AddOption("-imdims",3);    // facultatif
  
  // output
  parser.AddOption("-outpixdims",3); // facultatif
  parser.AddOption("-saveas",1);     // obligatoire
  parser.AddOption("-type",1);       // facultatif : default value = uchar
  parser.AddOption("-path",1);        // facultatif. Repertoire de sauvegarde, par defaut, c'est le repertoire de la premiere byu.
  parser.AddOption("-name",1);       // facultatif. Nom sans extension 
  parser.AddOption("-suffix",1);      // facultatif. 
  parser.AddOption("-format",1);      // faculatif, par defaut format=.gipl
  
  //    parser.AddOption("-volume",1);      // Facultatif. Nom du fichier ou stocker le volume.
  parser.AddOption("-volume",0);      // Facultatif. Nom du fichier ou stocker le volume.
  parser.AddOption("-subdiv",2); // facultatif
  parser.AddOption("-subdivPlaneFile",1); // facultatif
  parser.AddOption("-subdivPlaneVarFile",1); // facultatif
  parser.AddOption("-addFrameSize",1);     // obligatoire
  
  CommandLineArgumentParseResult parseResult;
  if(argc ==1 || !parser.TryParseCommandLine(argc,argv,parseResult)) {
    // print usage info and exit
    cerr<<"surf2vol Command Line Usage:"<<endl;
    cerr<<"surf2vol [options]"<<endl;
    
    cerr<<"Options: "<<endl;
    
    cerr<<"-in1 FILE            : "<<
      "Load surface file (either byu or iv) FILE - compulsory option"<<endl;
    cerr<<"-in2 FILE            : "<<
      "Load second byu file FILE - facultative option"<<endl;
    cerr<<"-inpixdims dX dY dZ  : "<<
      "Load original voxel dimensions - compulsory option"<<endl;
    cerr<<"-imdims X Y Z        : "<<
      "Load original image dimensions - facultative option, \n  useful to save a segmentation mask"<<endl;
    
    cerr<<"-outpixdims dX dY dZ : "<<
      "Specify the wanted output resolution if different \n                       from the input resolution - facultative option"<<endl;
    cerr<<"-saveas 0/1/2        : "<<
      "Use 0 to save the voxel volume in a bounding box,\n                       Use 1 to save one segmentation mask per byu," << endl
  <<" Use 2 to save all byus on a same segmentation mask\n                       - compulsory option"<<endl;
    cerr<<"-type TYPE           : "<<
      "Choose the type of the output: 0 for unsigned short or \n                       1 for unsigned char - default value: TYPE=uchar"<<endl;
    cerr<<"-path PATH           : "<<
      "Where to save output files - facultative option \n                       Default value = path of the first byu"<<endl;
    cerr<<"-name NAME           : "<<
      "Output name without extension - facultative option \n                       Default value = name of the first byu"<<endl;
    cerr<<"-suffix SUFFIX       : "<<
      "Output suffix - facultative option"<<endl;
    cerr<<"-format FORMAT       : "<<
      "Output format, e.g. 'gipl' - facultative option \n                       Default value = gipl" <<endl;
    cerr<<"-addFrameSize size      : "<<
      "Size of additional Frame around Bounding box in % (combined both sides of each axis), DEFAULT is 10% (i.e. 5% each side)\n" <<endl;
    //    cerr<<"-volume              : "<<
    //     "Writes the ouptut volume - facultative option"<<endl;
    cerr<<"-subdiv x|y|z num    : "<<
      "Subdivide the object uniformly into <num> Regions along specified axis (either x,y or z) - facultative option"<<endl;
    cerr<<"-subdivPlaneFile SUBDIVFILE: "<<
      "Subdivide the object according to planes stored in SUBDIVFILE - facultative option"<<endl;
    cerr<<  "subdivision file has one line per plane and each plane identified by a white space " <<
            "separated list of the x y z coordinates of the plane center and then the plane normal,"<<
            "the planes have to be in order"<<endl;
    cerr<<"-subdivPlaneVarFile SUBDIVVARFILE: "<<
      "Associate the subdivision with the variance given in SUBDIVVARFILE - facultative option"<<endl;
    return -1; 
  }

  QApplication myApp( argc, argv );
  surf2volGUIControls m_GUI( 0, 0, FALSE );
  // AddBYU (the first one and the second one)
  QStringList filelist;
  if (!parseResult.IsOptionPresent("-in1")){
    cerr<<"\"-in1\" is a compulsory option. Exit."<<endl;
    return -1;
  }
  else{
    filelist.append((QString)(parseResult.GetOptionParameter("-in1")));
  }
  
  if(parseResult.IsOptionPresent("-in2")){
    filelist.append((QString)(parseResult.GetOptionParameter("-in2")));
  }

  int index=0;
  
  for ( QStringList::Iterator it = filelist.begin(); it != filelist.end(); ++it ) 
    {
      surf2volGUIControls::byuinfostruct m_byuproperties;
      
      // INPUT parameters
      if (parseResult.IsOptionPresent("-imdims")){
  m_byuproperties.byuproperties = FALSE;
  m_byuproperties.imageproperties = TRUE;
      }
      else{
  m_byuproperties.byuproperties = TRUE;
  m_byuproperties.imageproperties = FALSE;
      }
      if (!parseResult.IsOptionPresent("-inpixdims")){
  cerr<<"\"-inpixdims\" is a compulsory option. Exit."<<endl;
  return -1;
      }
      else{
  if (m_byuproperties.byuproperties){
    m_byuproperties.byupixdims[0] = atof(parseResult.GetOptionParameter("-inpixdims",0));
    m_byuproperties.byupixdims[1] = atof(parseResult.GetOptionParameter("-inpixdims",1));
    m_byuproperties.byupixdims[2] = atof(parseResult.GetOptionParameter("-inpixdims",2));
  }
  else{
    m_byuproperties.byupixdims[0] = 0;
    m_byuproperties.byupixdims[1] = 0;
    m_byuproperties.byupixdims[2] = 0;
  }
  if (m_byuproperties.imageproperties){
    m_byuproperties.imagepixdims[0] = atof(parseResult.GetOptionParameter("-inpixdims",0));
    m_byuproperties.imagepixdims[1] = atof(parseResult.GetOptionParameter("-inpixdims",1));
    m_byuproperties.imagepixdims[2] = atof(parseResult.GetOptionParameter("-inpixdims",2));
  }
  else{
    m_byuproperties.imagepixdims[0] = 0;
    m_byuproperties.imagepixdims[1] = 0;
    m_byuproperties.imagepixdims[2] = 0;
  }
      }
      if (parseResult.IsOptionPresent("-imdims")){
        m_byuproperties.imagesize[0] = atoi(parseResult.GetOptionParameter("-imdims",0));
        m_byuproperties.imagesize[1] = atoi(parseResult.GetOptionParameter("-imdims",1));
        m_byuproperties.imagesize[2] = atoi(parseResult.GetOptionParameter("-imdims",2));
      }
      else{
        m_byuproperties.imagesize[0] = 0;
        m_byuproperties.imagesize[1] = 0;
        m_byuproperties.imagesize[2] = 0;
      }
      
      // OUTPUT parameters
      if (parseResult.IsOptionPresent("-outpixdims")){
        m_byuproperties.outputchangeresolution = TRUE;
        m_byuproperties.outputpixdims[0]=atof(parseResult.GetOptionParameter("-outpixdims",0));
        m_byuproperties.outputpixdims[1]=atof(parseResult.GetOptionParameter("-outpixdims",1));
        m_byuproperties.outputpixdims[2]=atof(parseResult.GetOptionParameter("-outpixdims",2));
      }
      else{
        m_byuproperties.outputchangeresolution = FALSE;
        m_byuproperties.outputpixdims[0]=0;
        m_byuproperties.outputpixdims[1]=0;
        m_byuproperties.outputpixdims[2]=0;
      }
      
      if(!parseResult.IsOptionPresent("-saveas")){
        cerr<<"\"-saveas\" is a compulsory option. Exit."<<endl;
        return -1;
      }
      else{
        m_byuproperties.saveas = atoi(parseResult.GetOptionParameter("-saveas"));
      }
      
      if(parseResult.IsOptionPresent("-type")){
        m_byuproperties.outputtype = atoi(parseResult.GetOptionParameter("-type"));
      } else{
        m_byuproperties.outputtype = 1;
      }
      
      if (parseResult.IsOptionPresent("-path")){
        m_byuproperties.outputdirectory = (QString)(parseResult.GetOptionParameter("-path"));
      } else{
        //QSTring byu1path = filelist.first().latin1();
        //m_byuproperties.outputdirectory = byu1path.mid(0,byu1path.findRev("/")+1)
        m_byuproperties.outputdirectory = "";
      }
      if(parseResult.IsOptionPresent("-addFrameSize")){
                  m_byuproperties.addFrameSize = atof(parseResult.GetOptionParameter("-addFrameSize"))/100; // input in %
      } else{
                  m_byuproperties.addFrameSize = 0.1;
      }
      if(parseResult.IsOptionPresent("-name")){
        m_byuproperties.outputname = (QString)(parseResult.GetOptionParameter("-name"));
      } else{
        //QString byu1name = filelist.first().latin1();
        //byu1name = byu1name.mid(0,byu1name.findRev("."));
        //m_byuproperties.outputname = byu1name.section("/",-1,-1);
        m_byuproperties.outputname = "";
      }
      
      if (parseResult.IsOptionPresent("-suffix")){
        m_byuproperties.outputsuffix = (QString)(parseResult.GetOptionParameter("-suffix"));
      } else{
        m_byuproperties.outputsuffix = "";
      }
      
      if (parseResult.IsOptionPresent("-format")){
         QString format = (QString)(parseResult.GetOptionParameter("-format"));
         if ((format=="gipl")||(format=="hdr")||(format=="mha")||(format=="raw3")){
           if (format=="gipl") m_byuproperties.outputformat = 0;  
           if (format=="hdr") m_byuproperties.outputformat = 1;
           if (format=="mha") m_byuproperties.outputformat = 2;
           if (format=="raw3") m_byuproperties.outputformat = 3;
         } else{
           cerr<<"Ask for one of this format: gipl, hdr, mha or raw3"<<endl;
         }
      } else{
         m_byuproperties.outputformat = 0;
      }

      if (parseResult.IsOptionPresent("-subdiv")){
         char axis = parseResult.GetOptionParameter("-subdiv",0)[0]; 
         if (axis == 'x' || axis == 'X') m_byuproperties.subdivAxis = 0;
         else if (axis == 'y' || axis == 'Y') m_byuproperties.subdivAxis = 1;
         else if (axis == 'z' || axis == 'Z') m_byuproperties.subdivAxis = 2;
         else {
           cerr << " Error: incorrect axis specification, specify x, y or z. Continue with X axis" << endl;
           m_byuproperties.subdivAxis = 0;
         }
         m_byuproperties.subdivNum = atoi(parseResult.GetOptionParameter("-subdiv",1));
      } else{
         m_byuproperties.subdivAxis = 0;
         m_byuproperties.subdivNum = 0;
      }
      
      if (parseResult.IsOptionPresent("-subdivPlaneFile")){
         m_byuproperties.subdivFile = (QString)(parseResult.GetOptionParameter("-subdivPlaneFile"));
      } else{
         m_byuproperties.subdivFile = "";
      }
      if (parseResult.IsOptionPresent("-subdivPlaneVarFile")){
         m_byuproperties.subdivVarFile = (QString)(parseResult.GetOptionParameter("-subdivPlaneVarFile"));
      } else{
         m_byuproperties.subdivVarFile = "";
      }
      

      m_byuproperties.outputvolumevalue = 0.0;
      
      m_byuproperties.byuname = (*it).latin1();
      m_GUI.m_byulist.push_back(m_byuproperties);

      index++;
    }
  //    g_addbyu->setDown(false);
  m_GUI.m_index = 0;
  
  
  //Conversion
  m_GUI.Convert(argc);
  return 0;
}

