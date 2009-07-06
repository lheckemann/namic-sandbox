/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLabelObject.h,v $
  Language:  C++
  Date:      $Date: 2005/01/21 20:13:31 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLabelObject_h
#define __itkLabelObject_h

#include <deque>
#include <algorithm>
#include <itkLightObject.h>
#include "itkLabelMap.h"
#include "itkLabelObjectLine.h"

namespace itk
{


namespace Functor {

template< class TLabelObject >
class ITK_EXPORT LabelLabelObjectAccessor
{
public:
  typedef TLabelObject                        LabelObjectType;
  typedef typename LabelObjectType::LabelType AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetLabel();
    }
};

template< class TLabelObject >
class ITK_EXPORT NumberOfLinesLabelObjectAccessor
{
public:
  typedef TLabelObject     LabelObjectType;
  typedef int              AttributeValueType;

  inline const AttributeValueType operator()( const LabelObjectType * labelObject )
    {
    return labelObject->GetNumberOfLines();
    }
};

template< class TLabelObject, class TAttributeAccessor >
class LabelObjectComparator
{
public:
  typedef TLabelObject       LabelObjectType;
  typedef TAttributeAccessor AttributeAccessorType;
  bool operator()( const LabelObjectType * a, const LabelObjectType * b )
    {
    return m_Accessor( a ) > m_Accessor( b );
    }
private:
  AttributeAccessorType m_Accessor;
};

template< class TLabelObject, class TAttributeAccessor >
class LabelObjectReverseComparator
{
public:
  typedef TLabelObject       LabelObjectType;
  typedef TAttributeAccessor AttributeAccessorType;
  bool operator()( const LabelObjectType * a, const LabelObjectType * b )
    {
    return m_Accessor( a ) < m_Accessor( b );
    }
private:
  AttributeAccessorType m_Accessor;
};

}


/** \class LabelObject
 *  \brief The base class for the representation of an labeled binary object in an image
 * 
 * LabelObject is the base class to represent a labeled object in an image.
 * It should be used associated with the LabelMap.
 *
 * LabelObject store mainly 2 things: the label of the object, and a set of lines
 * which are part of the object.
 * No attribute is available in that class, so this class can be used as a base class
 * to implement a label object with attribute, or when no attribute is needed (see the
 * reconstruction filters for an example. If a simple attribute is needed,
 * AttributeLabelObject can be used directly.
 *
 * All the subclasses of LabelObject have to reinplement the CopyAttributesFrom() method.
 *
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * \sa LabelMapFilter, AttributeLabelObject
 * \ingroup DataRepresentation 
 */
template < class TLabel, unsigned int VImageDimension >
class ITK_EXPORT LabelObject : public LightObject
{
public:
  /** Standard class typedefs */
  typedef LabelObject               Self;
  typedef LightObject               Superclass;
  typedef Self                      LabelObjectType;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>   ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(LabelObject, LightObject);

  itkStaticConstMacro(ImageDimension, unsigned int, VImageDimension);

  typedef itk::Index< ImageDimension > IndexType;

  typedef TLabel LabelType;

  typedef LabelObjectLine< VImageDimension > LineType;

  typedef typename LineType::LengthType LengthType;

  typedef typename std::deque< LineType > LineContainerType;

  typedef unsigned int AttributeType;
  static const AttributeType LABEL=0;

  static AttributeType GetAttributeFromName( const std::string & s )
    {
    if( s == "Label" )
      {
      return LABEL;
      }
    // can't recognize the name
    throw std::runtime_error("Unknown attribute.");
    }

  static std::string GetNameFromAttribute( const AttributeType & a )
    {
    switch( a )
      {
      case LABEL:
        return "Label";
        break;
      }
    // can't recognize the namespace
    throw std::runtime_error("Unknown attribute.");
    }

  /**
   * Set/Get the label associated with that object.
   */
  const LabelType & GetLabel() const
    {
    return m_Label;
    }

  void SetLabel( const LabelType & label )
    {
    m_Label = label;
    }

  /**
   * Return true if the object contain the given index and false otherwise.
   * Worst case complexity is O(L) where L is the number of lines in the object.
   */
  bool HasIndex( const IndexType & idx ) const
    {
    for( typename LineContainerType::const_iterator it=m_LineContainer.begin();
      it != m_LineContainer.end();
      it++ )
      {
      if( it->HasIndex( idx ) )
        {
        return true;
        }
      }
    return false;
    }

  /**
   * Add an index to the object. If the index is already in the object, the index can
   * be found several time in the object.
   */
  void AddIndex( const IndexType & idx ) 
    {
    assert( !this->HasIndex( idx ) );

    if( !m_LineContainer.empty() )
      {
      // can we use the last line to add that index ?
      LineType & lastLine = * m_LineContainer.rbegin();
      if( lastLine.IsNextIndex( idx ) )
        {
        lastLine.SetLength( lastLine.GetLength() + 1 );
        return;
        }
      }
    // create a new line
    this->AddLine( idx, 1 );
    }

  /**
   * Add a new line to the object, without any check.
   */
  void AddLine( const IndexType & idx, const LengthType & length )
    {
    LineType line( idx, length );
    this->AddLine( line );
    }

  /**
   * Add a new line to the object, without any check.
   */
  void AddLine( const LineType & line )
    {
    // TODO: add an assert to be sure that some indexes in the line are not already stored here
    m_LineContainer.push_back( line );
    }
  
  /** Return the line container of this object */
  const LineContainerType & GetLineContainer() const
    {
    return m_LineContainer;
    }

  LineContainerType & GetLineContainer()
    {
    return m_LineContainer;
    }

  void SetLineContainer( LineContainerType & lineContainer )
    {
    m_LineContainer = lineContainer;
    }

  int GetNumberOfLines() const
    {
    return m_LineContainer.size();
    }

  const LineType & GetLine( int i ) const
    {
    return m_LineContainer[i];
    }
  
  LineType & GetLine( int i )
    {
    return m_LineContainer[i];
    }

  int Size() const
    {
    int size = 0;
    for( typename LineContainerType::const_iterator it=m_LineContainer.begin();
      it != m_LineContainer.end();
      it++ )
      {
      size += it->GetLength();
      }
    return size;
    }
  
  bool Empty() const
    {
    return m_LineContainer.empty();
    }
  
  IndexType GetIndex( int offset ) const
    {
    int o = offset;
    for( typename LineContainerType::const_iterator it=m_LineContainer.begin();
      it != m_LineContainer.end();
      it++ )
      {
      int size = it->GetLength();
      if( o >= size)
        {
        o -= size;
        }
      else
        {
        IndexType idx = it->GetIndex();
        idx[0] += o;
        return idx;
        }
      }
    itkGenericExceptionMacro(<< "Invalid offset: " << offset);
    }
  
  /** Copy the attributes of another node to this one */
  virtual void CopyAttributesFrom( const Self * src )
    {
    assert( src != NULL );
    // nothing to do here - this class has no attribute
    }
   
  /** Copy the lines, the label and the attributes from another node. */
  void CopyAllFrom( const Self * src )
    {
    assert( src != NULL );
    m_LineContainer = src->m_LineContainer;
    m_Label = src->m_Label;
    // also copy the attributes
    this->CopyAttributesFrom( src );
    }
    
  /** Reorder the lines, merge the touching lines and ensure that no
   * pixel is covered by two lines
   */
  void Optimize()
    {
    if( ! m_LineContainer.empty() )
      {
      // first copy the lines in another container and clear the current one
      LineContainerType lineContainer = m_LineContainer;
      m_LineContainer.clear();
      
      // reorder the lines
      typename Functor::LabelObjectLineComparator< LineType > comparator;
      std::sort( lineContainer.begin(), lineContainer.end(), comparator );
      
      // then check the lines consistancy
      // we'll proceed line index by line index
      IndexType currentIdx = lineContainer.begin()->GetIndex();
      long int currentLength = lineContainer.begin()->GetLength();
      
      for( typename LineContainerType::const_iterator it=lineContainer.begin(); 
           it != lineContainer.end();
           it++ )
        {
        const LineType & line = *it;
        IndexType idx = line.GetIndex();
        unsigned long length = line.GetLength();
        
        // check the index to be sure that we are still in the same line idx
        bool sameIdx = true;
        for( int i=1; i<ImageDimension; i++ )
          {
          if( currentIdx[i] != idx[i] )
            {
            sameIdx = false;
            }
          }
        
        // try to extend the current line idx, or create a new line
        if( sameIdx && currentIdx[0] + currentLength >= idx[0] )
          {
          // we may expand the line
          long int newLength = idx[0] + length - currentIdx[0];
          currentLength = std::max( newLength, currentLength );
          }
        else
          {
          // add the previous line to the new line container and use the new line index and size
          // std::cout << currentIdx << "  " << currentLength << std::endl;
          this->AddLine( currentIdx, currentLength );
          currentIdx = idx;
          currentLength = length;
          }
        // std::cout << line.GetIndex() << "  " << line.GetLength() << std::endl;
        }
      // complete the last line
      // std::cout << currentIdx << "  " << currentLength << std::endl;
      this->AddLine( currentIdx, currentLength );
      }
    }

protected:
  LabelObject()
    {
    m_Label = NumericTraits< LabelType >::Zero;
    m_LineContainer.clear();
    }
  
  void PrintSelf(std::ostream& os, Indent indent) const
    {
    Superclass::PrintSelf( os, indent );
    os << indent << "LineContainer: " << & m_LineContainer << std::endl;
    os << indent << "Label: " << static_cast<typename NumericTraits<LabelType>::PrintType>(m_Label) << std::endl; 
    }


private:
  LabelObject(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  LineContainerType m_LineContainer;
  LabelType         m_Label;
};

} // end namespace itk

#endif
