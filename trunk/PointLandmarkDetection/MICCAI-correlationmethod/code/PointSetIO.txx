#ifndef _itkPointSetIO_txx
#define _itkPointSetIO_txx

#include "PointSetIO.h"




namespace itk
{

template <class TPointSetType>
PointSetIO<TPointSetType>
::PointSetIO()
    {
    DELIMS = std::string(" \t");
    }

template <class TPointSetType>
PointSetIO<TPointSetType>
::~PointSetIO()
  {
  }

template <class TPointSetType>
void
PointSetIO<TPointSetType>
::WritePointSet( PointSetType *  mesh, const char * filename )
  {



    std::ofstream mesh_fstream(filename);
    //mesh_fstream << "# vtk DataFile Version 3.0" << std::endl << "vtk output" << std::endl;
    //mesh_fstream << "ASCII" << std::endl << "DATASET UNSTRUCTURED_GRID" << std::endl;
    mesh_fstream << "POINTS " << mesh->GetNumberOfPoints() << " float" << std::endl;


    PointsContainerPointer  points  = mesh->GetPoints();
    if (points)
    {
    PointsContainerIterator  pointIterator = points->Begin();
    PointsContainerIterator  end_point = points->End();
    while( pointIterator != end_point )
      {
      PointType p = pointIterator.Value();   // access the point
      mesh_fstream << p[0] << DELIMS << p[1] << DELIMS << p[2] << std::endl;           // print the point
      ++pointIterator;                       // advance to next point
      }
    }


    PointDataContainerPointer data = mesh->GetPointData();
    if (data)
    {
    PointDataContainerIterator dataIterator = data->Begin();
    PointDataContainerIterator end_data = data->End();


    mesh_fstream << "DATA " << mesh->GetNumberOfPoints() << std::endl;
    while( dataIterator != end_data )
      {
      PixelType p = dataIterator.Value();       // access the data
      mesh_fstream << p << std::endl;           // print the data
      ++dataIterator;                           // advance to next data
      }
    }

    std::cout << "point set saved in : " << filename << std::endl;
  }



template <class TPointSetType>
void
PointSetIO<TPointSetType>
::WritePointSetMatlab( PointSetType *  mesh, const char * filename )
  {


    PointsContainerPointer  points  = mesh->GetPoints();
    PointsContainerIterator  pointIterator = points->Begin();
    PointsContainerIterator  end_point = points->End();

    PointDataContainerPointer data = mesh->GetPointData();



    std::ofstream mesh_fstream(filename);

    if (data)
    {
      PointDataContainerIterator dataIterator = data->Begin();
      PointDataContainerIterator end_data = data->End();
      while( pointIterator != end_point )
        {
        PointType p = pointIterator.Value();   // access the point
        PixelType v = dataIterator.Value();   // access the data
        mesh_fstream << p[0] << DELIMS << p[1] << DELIMS << p[2] << DELIMS << v << std::endl;           // print the point
        ++pointIterator;                       // advance to next point
        ++dataIterator;                       // advance to next data
        }
    }
    else
    {
      while( pointIterator != end_point )
        {
        PointType p = pointIterator.Value();   // access the point
        mesh_fstream << p[0] << DELIMS << p[1] << DELIMS << p[2] << std::endl;           // print the point
        ++pointIterator;                       // advance to next point
        }
    }

    fprintf(stderr, "point set saved\n");
  }



template <class TPointSetType>
typename PointSetIO<TPointSetType>::PointSetPointer
PointSetIO<TPointSetType>
::ReadPointSet( const char * filename )
  {

    PointSetPointer mesh = PointSetType::New();
    PointsContainerPointer  points = PointsContainer::New();
    PointDataContainerPointer data = PointDataContainer::New();

    bool isreadingpoints = false;
    bool isreadingdata = false;


    std::ifstream mesh_istream(filename);
    std::string line;
    std::string::size_type idx;

long unsigned int count1 = 0;

    while (getline(mesh_istream, line)) {
      // Skip lines that are blank or start with a #
      if (line.length() == 0) continue;
      if (line[0] == '#') continue;
      if (line.find("POINTS") != std::string::npos) {
        isreadingpoints=true;
   isreadingdata=false;
   continue;
      }
      if (line.find("DATA") != std::string::npos) {
        isreadingpoints=false;
   isreadingdata=true;
   continue;
      }

      if (isreadingpoints)
      {
        PointType p;
   float pf[3];


        int count = sscanf(line.c_str(),
                   "%f %f %f",
                   &pf[0], &pf[1], &pf[2] );
   if (count!=3)
     {
     std::cerr << "couldn't read points" << std::endl;
          exit(3);
     }
   p[0] = pf[0]; p[1] = pf[1]; p[2] = pf[2];
        points->push_back( p );

      }

      if (isreadingdata)
      {
        PixelType p;

        int count = sscanf(line.c_str(),"%f", &p ); //substr(idx).
   if (count!=1)
     {
     std::cerr << "couldn't read data" << std::endl;
          exit(3);
     }
//std::cout << ++count1 << " : " << p << std::endl;
   data->push_back( p );
      }

    }


    mesh->SetPoints( points );
    mesh->SetPointData( data );

    std::cout << "Number of points in file: " << mesh->GetNumberOfPoints() << std::endl;

    fprintf(stderr, "point set read\n");

    return mesh;
  }

}
#endif
