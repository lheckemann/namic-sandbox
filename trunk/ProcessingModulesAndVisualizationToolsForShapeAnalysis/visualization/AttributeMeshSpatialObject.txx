// AttributeMeshSpatialObject.txx: implementation of the AttributeMeshSpatialObject class.
//
//////////////////////////////////////////////////////////////////////

#include "AttributeMeshSpatialObject.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template < class TMesh, class TAttributeType>
AttributeMeshSpatialObject<TMesh, TAttributeType>::AttributeMeshSpatialObject() 
{
    m_AttributeDimension = 1 ;
}

template < class TMesh, class TAttributeType>
AttributeMeshSpatialObject<TMesh, TAttributeType>::~AttributeMeshSpatialObject()
{

}

template < class TMesh, class TAttributeType>
void AttributeMeshSpatialObject<TMesh, TAttributeType>::MakeAttributeMesh()
{
  this->SetTypeName("AttributeMeshSpatialObject");
  m_Attributes.clear () ;
}


template < class TMesh, class TAttributeType>
void AttributeMeshSpatialObject<TMesh, TAttributeType>::ReadAttributes ( std::string attrFilename ) 
{
    std::ifstream attrFile ;
    attrFile.open ( attrFilename.c_str() ) ;

    int nPts = m_Mesh->GetNumberOfPoints () ;
    
    cout << nPts << endl ;

    m_Attributes.clear () ;
    
    // this has to be changed for nAttrDim > 1 !!!
    int nAttrDim = 1 ;
    TAttributeType temp ;

    for ( int i = 0 ; i < nPts ; i++ )
    {
        for ( int j = 0 ; j < nAttrDim ; j++ )
        {
            attrFile >> temp ;
            m_Attributes.push_back ( temp ) ;
            cout << temp << endl ;
        }
    }
    attrFile.close () ;
}

template < class TMesh, class TAttributeType>
TAttributeType AttributeMeshSpatialObject<TMesh, TAttributeType>::GetAttribute (int vertIndex) 
{
    if ( vertIndex < m_Attributes.size() )
    {
        //cout << m_Attributes[vertIndex] << " " ;
        return m_Attributes[vertIndex] ;
    }

    return 0 ;
}

template < class TMesh, class TAttributeType>
void AttributeMeshSpatialObject<TMesh, TAttributeType>::writeToFile(const char *filename) 
{
    std::ofstream outputFile ;
    outputFile.open ( filename ) ;

    int nPts = m_Mesh->GetNumberOfPoints () ;
    int i ;

    TMesh::PointType point ;
    
    outputFile << "ObjectType = Mesh Attribute" << endl ;
    outputFile << "NDims = 3" << endl ;
    outputFile << "ID = 0" << endl ;
    outputFile << "TransformMatrix = 1 0 0 0 1 0 0 0 1" << endl ;
    outputFile << "Offset = 0 0 0" << endl ;
    outputFile << "CenterOfRotation = 0 0 0" << endl ;
    outputFile << "ElementSpacing = 1 1 1" << endl ;
    outputFile << "PointType = MET_FLOAT" << endl ;
    outputFile << "PointDataType = MET_FLOAT" << endl ;
    outputFile << "CellDataType = MET_FLOAT" << endl ;
    outputFile << "NCellTypes = 1" << endl ;
    outputFile << "PointDim = ID x y ..." << endl ;
    outputFile << "NPoints = " << nPts << endl ;
    outputFile << "Points = " << endl ;

    for ( i = 0 ; i < nPts ; i++ )
    {
        if ( m_Mesh->GetPoint (i, &point) ) 
        {
            outputFile << i << " " ;
            outputFile << point[0] << " " ;
            outputFile << point[1] << " " ;
            outputFile << point[2] << " " ;
            //for ( int j ; j < m_AttributeDimension ; j++ )
            //{
                outputFile << m_Attributes[i] << " " ;
            //}
            outputFile << endl ;
        }
    }

    int nTris = m_Mesh->GetNumberOfCells () ;
    TMesh::CellAutoPointer cell ;

    outputFile << endl ;
    outputFile << "CellType = TRI" << endl ;
    outputFile << "NCells = " << nTris << endl ;
    outputFile << "Cells = " << endl ;
    
    for ( i = 0 ; i < nTris ; i++ )
    {
        if ( m_Mesh->GetCell ( i, cell ) )
        {
            TMesh::CellType::PointIdConstIterator points = cell->GetPointIds() ;
            outputFile << i << " " ;
            outputFile << points[0] << " " ;
            outputFile << points[1] << " " ;
            outputFile << points[2] << endl ;
        }
    }

    outputFile.close () ;
}

template < class TMesh, class TAttributeType>
void AttributeMeshSpatialObject<TMesh, TAttributeType>::loadFromFile(const char *filename) 
{
    std::ifstream inputFile ;
    inputFile.open ( filename ) ;

    int i ;
    int nPts, nTris ;
    int dummy ;
    char line[200] ;
    bool skip ;

    // skip the beginning 
    skip = true ;
    while ( skip )
    {
        inputFile.getline ( line, 200 ) ;
        skip = strncmp ( line, "NPoints = ", 10 ) ;
    }
    // get the number of points
    nPts = atoi ( line+10 ) ;
    cout << nPts << endl ;

    // skip more
    skip = true ;
    while ( skip )
    {
        inputFile.getline ( line, 200 ) ;
        skip = strncmp ( line, "Points = ", 9 ) ;
    }
    
    // Add Points
    TAttributeType attribute ;
    m_Attributes.clear () ;
    for ( i = 0 ; i < nPts ; i++ )
    {
        typename MeshType::PointType pt;
        inputFile >> dummy >> pt[0] >> pt[1] >> pt[2] >> attribute ;
        m_Mesh->SetPoint(dummy,pt);
        m_Attributes.push_back ( attribute ) ;
    }

    // skip the beginning 
    skip = true ;
    while ( skip )
    {
        inputFile.getline ( line, 200 ) ;
        skip = strncmp ( line, "NCells = ", 9 ) ;
    }
    // get the number of points
    nTris = atoi ( line+9 ) ;
    
    // skip more
    skip = true ;
    while ( skip )
    {
        inputFile.getline ( line, 200 ) ;
        skip = strncmp ( line, "Cells = ", 8 ) ;
    }
    
    // Add Cells
    typedef typename MeshType::CellType CellType;
    typedef typename CellType::CellAutoPointer CellAutoPointer;
    m_Mesh->SetCellsAllocationMethod ( MeshType::CellsAllocatedDynamicallyCellByCell );
   
    typedef typename MeshType::CellType  CellInterfaceType;
    typedef itk::TriangleCell<CellInterfaceType> TriangleCellType;
    
    int v0, v1, v2 ;

    for ( i = 0 ; i < nTris ; i++ )
    {
        CellAutoPointer cell;
        cell.TakeOwnership ( new TriangleCellType ); 
        
        inputFile >> dummy >> v0 >> v1 >> v2 ;
        cell->SetPointId(0, v0) ;
        cell->SetPointId(1, v1) ;
        cell->SetPointId(2, v2) ;
      
        m_Mesh->SetCell(i,cell);
    } 

    inputFile.close () ;
}

