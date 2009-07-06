def attribute_list( i, name ):
  """Returns a list of the specified attributes for the objects in the image.
  
  i: the input LabelImage
  name: the attribute name
  """
  import itk
  i = itk.output(i)
  relabel = itk.StatisticsRelabelLabelMapFilter[i].New(i, Attribute=name, ReverseOrdering=True, InPlace=False)
  relabel.UpdateLargestPossibleRegion()
  r = relabel.GetOutput()
  l = []
  for i in range(1, r.GetNumberOfLabelObjects()+1):
    l.append( r.GetLabelObject(i).__getattribute__("Get"+name)() )
  return l


def attributes_list( i, names ):
  """Returns a list of the specified attributes for the objects in the image.
  
  i: the input LabelImage
  name: the attribute name
  """
  import itk
  i = itk.output(i)
  relabel = itk.StatisticsRelabelLabelMapFilter[i].New(i, Attribute=names[0], ReverseOrdering=True, InPlace=False)
  relabel.UpdateLargestPossibleRegion()
  r = relabel.GetOutput()
  l = []
  for i in range(1, r.GetNumberOfLabelObjects()+1):
    attrs = []
    for name in names :
      attrs.append( r.GetLabelObject(i).__getattribute__("Get"+name)() )
    l.append( tuple( attrs ) )
  return l


def attribute_dict( i, name ):
  """Returns a dict with the attribute values in keys and a list of the corresponding objects in value
  
  i: the input LabelImage
  name: the name of the attribute
  """
  import itk
  i = itk.output(i)
  relabel = itk.StatisticsRelabelLabelMapFilter[i].New(i, Attribute=name, ReverseOrdering=True, InPlace=False)
  relabel.UpdateLargestPossibleRegion()
  r = relabel.GetOutput()
  d = {}
  for i in range(1, r.GetNumberOfLabelObjects()+1):
    lo = r.GetLabelObject(i)
    v = lo.__getattribute__("Get"+name)()
    l = d.get( v, [] )
    l.append( lo )
    d[v] = l
  return d


def number_of_objects( i ):
  """Returns the number of objets in the image.
  
  i: the input LabelImage
  """
  import itk
  i.UpdateLargestPossibleRegion()
  i =  itk.output(i)
  return i.GetNumberOfLabelObjects()


