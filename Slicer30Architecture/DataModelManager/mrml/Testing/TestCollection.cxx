#include "mrmlCollection.h"

int TestCollection(int argc, char *argv[])
{
  mrml::Collection::Pointer c = mrml::Collection::New();
  c->Print();

  unsigned long nItems = c->GetNumberOfItems();
  if( nItems != 0 || !c->IsEmpty() )
    return 1;

  mrml::Object::Pointer o = mrml::Object::New();
  c->AddItem( o );
  c->AddItem( o );
  nItems = c->GetNumberOfItems();
  if( nItems != 2 )
    {
    return 1;
    }

  c->RemoveAllItems();

  nItems = c->GetNumberOfItems();
  if( nItems != 0 || !c->IsEmpty() )
    return 1;

  return 0;
}
