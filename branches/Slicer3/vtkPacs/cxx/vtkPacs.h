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

#ifndef __vtkPacs_h
#define __vtkPacs_h

#include "vtkPacsConfigure.h"
#include "vtkObject.h"




class VTK_PACS_EXPORT vtkPacs : public vtkObject
{
public:
        static vtkPacs *New();
        vtkTypeMacro(vtkPacs,vtkObject);
        // Routines

        //Set main variables, hostname, port and application entity
        void SetConnection_Parameters (char* hostname, char* portnumber, char* app_entity);
        //Set main variables, hostname, port, application entity, application entity destination, port destination for retrieve SCU
        void SetConnection_ParametersRetrieve(char* hostname, char* portnumber, char* app_entity, char* app_entityDestination, char* portnumberDestination);

        //Set DICOM FIle to transmit
        void SetDICOM_File (char* dicom_image);
    
        //Set keys for find scu in PATIENT LEVEL
        //void Set_FindKeysPATIENT (char* query_level,char* key1,char* Pname,char* key2,char* Pid,char* key3,char* PBData,char* key4,char* PBTime,char* key5,char* PSex);
        void Set_FindKeys (char* query_level,char* dicom);
        int Verification_SOP();
        //C_STORE SCU, storage SOP
        int StoreSCU();
        //C_FIND SCU, find SOP 
        int FindSCU();
        //C_MOVE SCU, find SOP 
        int RetrieveSCU();

protected:

        vtkPacs();
        ~vtkPacs();

    //Variables

        //Network connection variables
        char* host;
        char* port;
        char* aet;
        char* portDestination;
        char* aem;

        //DICOM File to transmit
        char* dicom_file;
        //keys for query information
        char* query_level;

        //PATIENT LEVEL
        char* Pname;
        char *Pid;
        char* PBData;
        char* PBTime;
        char* PSex;

        //STUDY LEVEL
        char* SDate;
        char* SAccession;
        char* SPhysician;
        char* SUID;
        char* SID;

        //SERIES LEVEL
        char* SeModality;
        char* SeUID;
        char* SeNumber;

        //IMAGE LEVEL
        char* ICUID;
        char* IIUID;
        char* IInstance;

private:

        vtkPacs(const vtkPacs&);           // Not implemented.
        void operator=(const vtkPacs&);   // Not implemented.
};

#endif
