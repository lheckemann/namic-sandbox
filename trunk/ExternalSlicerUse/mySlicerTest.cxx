#include "vtkMrmlTree.h"

int main()
{
  vtkMrmlTree* tree = vtkMrmlTree::New();
  tree->Print(cout);
  tree->Delete();
  return 0;
}
