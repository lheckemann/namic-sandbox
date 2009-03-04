/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMeshToMeshRegistrationMethod.txx,v $
  Language:  C++
  Date:      $Date: 2003-11-08 17:58:32 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkQuadEdgeMesh.h"
#include "itkVTKPolyDataReader.h"
#include "itkVTKPolyDataWriter.h"
#include "itkMeshToMeshRegistrationMethod.h"

namespace itk
{
template< class TMeshFixed, class TMeshMoving >
class RegistrationMethodHelper : public MeshToMeshRegistrationMethod< TMeshFixed, TMeshMoving >
{
public:
  typedef RegistrationMethodHelper Self;
  typedef MeshToMeshRegistrationMethod< TMeshFixed, TMeshMoving > Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  itkNewMacro( Self );

  itkTypeMacro( RegistrationMethodHelper, MeshToMeshRegistrationMethod );

protected:
  RegistrationMethodHelper() {}
  virtual ~RegistrationMethodHelper() {}

private:

};

}

#define TRY_EXPECT_EXCEPTION( command ) \
  try \
    {  \
    command;  \
    std::cout << "Failed to catch Expected exception" << std::endl;  \
    return EXIT_FAILURE;  \
    }  \
  catch( itk::ExceptionObject & excp )  \
    {  \
    std::cout << "Catched expected exception" << std::endl;  \
    std::cout << excp << std::endl; \
    }  


int main( int argc, char * argv [] )
{
  if( argc != 2 )
    {
    std::cout <<"It requires 1 arguments" <<std::endl;
    std::cout <<"1-Input FileName" <<std::endl;
    return EXIT_FAILURE;
    }

  typedef double Coord;

  typedef itk::QuadEdgeMesh< Coord, 3 >                 MeshType;
  typedef MeshType::Pointer                             MeshPointer;
  typedef itk::VTKPolyDataReader< MeshType >            ReaderType;
  typedef itk::VTKPolyDataWriter< MeshType >            WriterType;

  ReaderType::Pointer readerFixed = ReaderType::New();
  readerFixed->SetFileName( argv[1] );

  ReaderType::Pointer readerMoving = ReaderType::New();
  readerMoving->SetFileName( argv[1] );

  try
    {
    readerFixed->Update( );
    readerMoving->Update( );
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << "Exception thrown while reading the input file " << std::endl;
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }

  MeshPointer meshFixed  = readerFixed->GetOutput();
  MeshPointer meshMoving = readerMoving->GetOutput();

  typedef itk::RegistrationMethodHelper< MeshType, MeshType >  RegistrationMethodType;

  RegistrationMethodType::Pointer registrator = RegistrationMethodType::New();

  registrator->Print( std::cout );


  std::cout << std::endl;
  std::cout << "Exercise Exception Catching" << std::endl;
  std::cout << std::endl;

  TRY_EXPECT_EXCEPTION( registrator->Initialize() );

  registrator->SetFixedMesh( meshFixed );

  TRY_EXPECT_EXCEPTION( registrator->Initialize() );

  registrator->SetFixedMesh( meshMoving );

  TRY_EXPECT_EXCEPTION( registrator->Initialize() );
  

  return EXIT_SUCCESS;
}
