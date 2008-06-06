class Project {
  String name
  Date createdDateTime = new Date()
  String description

  static hasMany = [subjectGroups:SubjectGroup]
}
