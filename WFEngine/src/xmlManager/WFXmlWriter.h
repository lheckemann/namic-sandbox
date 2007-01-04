#ifndef WFXMLWRITER_H_
#define WFXMLWRITER_H_

#include <xercesc/dom/DOMLSSerializerFilter.hpp>

XERCES_CPP_NAMESPACE_USE

class WFXmlWriter : public DOMLSSerializerFilter
{
public:

    /** @name Constructors */
  WFXmlWriter(unsigned long whatToShow = DOMNodeFilter::SHOW_ALL);
    //@{

    /** @name Destructors */
  ~WFXmlWriter(){};
    //@{

  /** @ interface from WFXmlWriter */
  virtual short acceptNode(const DOMNode*) const;
    //@{

  virtual unsigned long getWhatToShow() const {return fWhatToShow;};

private:
  // unimplemented copy ctor and assignement operator
  WFXmlWriter(const WFXmlWriter&);
  WFXmlWriter & operator = (const WFXmlWriter&);

  unsigned long fWhatToShow;

};

#endif /*WFXMLWRITER_H_*/
