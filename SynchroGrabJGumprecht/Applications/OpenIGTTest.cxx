
#include "vtkJPEGReader.h"
#include "vtkImageData.h"

#include "igtlclientsocket.h"
#include "igtlImageMessage.h"
#include "igtlMath.h"

void main(void)
{
    vtkJPEGReader *reader = vtkJPEGReader::New();

    reader->SetFileName("C:\\FreehandReconstruction\\Data\\Images\\CapturedImageID_NO_0000.jpg");
    reader->Update();

    vtkImageData *image = reader->GetOutput();
    int *dim = image->GetDimensions();
    double *spacing = image->GetSpacing();
    
    unsigned char* imgPtr = (unsigned char*)image->GetScalarPointer();
    
    cout << "Dimensions " << dim[0] << " " << dim[1] << " " << endl;
    cout << "Spacing " << spacing[0] << " " << spacing[1] << " " << spacing[2] << endl;
    cout << "Data dimension : " << image->GetDataDimension() << endl;
    cout << "Scalar Type : " << image->GetScalarType() << " ( " << image->GetScalarTypeAsString() << ")" << endl;
    cout << "Number of scalar components : " << image->GetNumberOfScalarComponents() << endl;


    // 1 - Opening an OpenIGTLink socket
    igtl::ClientSocket::Pointer socket = NULL;
    socket = igtl::ClientSocket::New();
    int e = socket->ConnectToServer("127.0.0.1",18944);

    if(e != 0)
        cout << "Error code : " << e << endl;
    else
        cout << "Successful connection to the OpenIGTLink server" << endl;


    //  Creating an image frame
    igtl::ImageMessage::Pointer imMessage = igtl::ImageMessage::New();
    
    imMessage->SetDimensions(dim[0], dim[1], 1);
    imMessage->SetSpacing(spacing[0], spacing[1], spacing[2]);
    imMessage->SetScalarType(image->GetScalarType());
    imMessage->SetDeviceName("SonixRP");
    imMessage->AllocateScalars();

    int svd[3];
    int svoff[3];
    imMessage->GetSubVolume(svd, svoff);

    cout << "SubVolume dimensions : " << svd[0] << " " << svd[1] << " " << svd[2] << endl;
    cout << "SubVolume offsets : " << svoff[0] << " " << svoff[1] << " " << svoff[2] << endl;

    int imSize = imMessage->GetImageSize();

    //faire un memcopy a la pace
    memcpy(imMessage->GetScalarPointer(), reader->GetOutput()->GetScalarPointer(), imSize);

    igtl::Matrix4x4 matrix;
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            matrix[i][j]=(float)0.0;
    for(int i=0;i<4;i++)
        matrix[i][i]=(float)1.0;

    matrix[0][3]=-200;
    matrix[1][3]=-100;

    imMessage->SetMatrix(matrix);
    imMessage->Pack();
  
    // sending this frame to the server
    std::cout << "PackSize:  " << imMessage->GetPackSize() << std::endl;
    std::cout << "BodyMode:  " << imMessage->GetBodyType() << std::endl;
      
    int ret = socket->Send(imMessage->GetPackPointer(), imMessage->GetPackSize());
    if (ret == 0)
        {
        std::cerr << "Error : Connection Lost!\n";
        }
    else 
        cout << "image frame send successfully" << endl;


    // closing the connection 
    socket->CloseSocket();

    // memory cleanup
    reader->Delete();
}
