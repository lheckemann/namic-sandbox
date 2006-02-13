/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================auto=*/

#include "vtkPacs.h"
#include "vtkObjectFactory.h"

#include <cstdlib>
#include <cstdio>
#include <string.h>


//--------Imported function to generate Verification SOP classes-------
//---------------------------------------------------------------------
#ifdef _WIN32
//Imported function to verify DICOM connectivity
__declspec(dllimport)int echo_primero(int argc, char* argv[]);

//Imported function to generate Store SCU
__declspec(dllimport)int storescu_main(int argc, char* argv[]);

//Imported function to query information to PACS
__declspec(dllimport)int findscu_main(int argc, char *argv[]);

//Imported function to retrieve information to PACS
__declspec(dllimport)int movescu_main(int argc, char *argv[]);

#else

extern int main_echo(int argc, char *argv[]);
extern int main_find(int argc, char *argv[]);
extern int main_move(int argc, char *argv[]);
extern int main_storescu (int argc, char *argv[]);
#endif

//---------------------------------------------------------------------
//---------------------------------------------------------------------


vtkPacs* vtkPacs::New()
{
        // First try to create the object from the vtkObjectFactory
        vtkObject* ret = vtkObjectFactory::CreateInstance("vtkPacs");
        if(ret)
            return (vtkPacs*)ret;
        // If the factory was unable to create the object, then create it here.
        return new vtkPacs;
}

vtkPacs::vtkPacs() {
//      this->host="10.26.12.118";
        this->host="10.26.12.141";
        this->port="1234";
        this->aet="PacsOne"; 
}

vtkPacs::~vtkPacs() {  
}


void vtkPacs::SetConnection_Parameters(char* hostname, char* portnumber, char* app_entity) {
        this->host = hostname;
        this->port = portnumber;
        this->aet  = app_entity; 
}


void vtkPacs::SetConnection_ParametersRetrieve(char* hostname, char* portnumber, char* app_entity, char* app_entityDestination, char* portnumberDestination) {
        this->host             = hostname;
        this->port             = portnumber;
        this->aet              = app_entity; 
        this->aem              = app_entityDestination;
        this->portDestination  = portnumberDestination;
}



void vtkPacs::SetDICOM_File (char* dicom_image) {
        this->dicom_file = dicom_image;
}

void vtkPacs::Set_FindKeys (char* query_level,char* dicom) {
        this->query_level = query_level;
        this->dicom_file  = dicom;
}

int vtkPacs::Verification_SOP() {
    //Input Arguments
        int argc;
        char* argv[6];

        argc=6;

        argv[0] = "echoscu";
        argv[1] ="-v";
        argv[2] = this->host;
        argv[3] = this->port;
        argv[4] = "-aet";
        argv[5] = this->aet;





        //TclConsoleStreambufSetup;
   
        puts("Hola");
    //fflush(stdout);
        //cout << "Adios" << endl;
        //vtkErrorMacro(<< argv[0] << this->port);
        //Imported function from DLL library
        int result=0;
        //result =echoscu_main(argc,argv);
#ifdef _WIN32
        result =echo_primero(argc,argv);
#else 
        result =main_echo(argc,argv);
#endif
        return result;
}

int vtkPacs::StoreSCU() {

        int argc=7;
        char *argv[7];


        argv[0] = "storescu";
        argv[1] ="-v";
        argv[2] = this->host;
        argv[3] = this->port;
        argv[4] = "-aet";
        argv[5] = this->aet;
        argv[6] = this->dicom_file;

        int result;
        //result = store_main(argc,argv);
#ifdef _WIN32
        result = storescu_main(argc,argv);
#else
        result = main_storescu(argc,argv);
#endif
        return result;

}

//Execute C-FIND SCU 
int vtkPacs::FindSCU() {

        int argc =9;
        char* argv[9];

        argv[0] = "findscu";
        argv[1] ="-v";
        argv[2] = this->host;
        argv[3] = this->port;
        argv[4] = "-aet";
        argv[5] = this->aet;
        argv[6] = "-P";
        argv[7] = this->dicom_file;
        argv[8]="-X";

        int result=0;
#ifdef _WIN32
        result = findscu_main(argc,argv);
#else
        result = main_find(argc,argv);
#endif
        return result;
}

//Execute C-MOVE SCU 
int vtkPacs::RetrieveSCU() {

        int argc =12;
        char* argv[12];

        argv[0]  = "movescu";
        argv[1]  ="-v";
        argv[2]  = this->host;
        argv[3]  = this->port;
        argv[4]  = "-aet";
        argv[5]  = this->aet;
        argv[6]  = "-aem";
        argv[7]  = this->aem;
        argv[8]  = "--port";
        argv[9]  = this->portDestination;
        argv[10] = "-P";
        argv[11] = this->dicom_file;

        int result=0;
#ifdef _WIN32
        result = movescu_main(argc,argv);
#else
        result = main_move(argc,argv);
#endif
        return result;
}

