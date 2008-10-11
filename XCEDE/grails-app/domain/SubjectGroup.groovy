class SubjectGroup {
  // static belongsTo = [project:Project, subject:Subject];
  // String name;
  Project project;
  // Subject subject;
  static belongsTo = [Project, Subject]
  static hasMany = [subjects:Subject];
}
