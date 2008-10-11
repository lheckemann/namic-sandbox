class Project {

  String PID
  String name
  Date createdDateTime = new Date();
  String description
  // ExptDesign, undocumented in the XCEDE 2.0 spec.  Projects have a list of ExptDesign objects.

  static hasMany = [subjectGroups:SubjectGroup]; // , contributors:Person]

  def renderXML ( xml ) {
    xml.project ( title:name )
  }

}
