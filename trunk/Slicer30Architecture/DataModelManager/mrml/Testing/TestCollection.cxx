#include "mrmlCollection.h"

int TestCollection(int /*argc*/, char * /*argv*/[])
{
  mrml::Collection::Pointer c = mrml::Collection::New();
  c->Print();

  unsigned long nItems = c->GetNumberOfItems();
  if( nItems != 0 || !c->IsEmpty() )
    return 1;

  mrml::Object::Pointer o = mrml::Object::New();
  c->AddItem( o );
  c->AddItem( o );
  c->Print();
  nItems = c->GetNumberOfItems();
  if( nItems != 2 )
    return 1;

  c->RemoveItem(3);
  c->RemoveItem(1);
  nItems = c->GetNumberOfItems();
  if( nItems != 1 )
    return 1;

  c->Clear(); //RemoveAllItems();

  nItems = c->GetNumberOfItems();
  if( nItems != 0 || !c->IsEmpty() )
    return 1;

  c->Print();
  o->Print();
  return 0;
}
