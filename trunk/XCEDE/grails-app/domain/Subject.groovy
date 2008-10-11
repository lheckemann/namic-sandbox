class Subject {
  // static hasMany = [subjectGroups:SubjectGroup];
  // static belongsTo = SubjectGroup;
  // SubjectGroup subjectGroup;
  String sex
  String species
  Date birthDate
  String name
  static hasMany = [subjectGroups:SubjectGroup]
}
