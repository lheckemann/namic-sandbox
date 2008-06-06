class SubjectGroup {
  static belongsTo = Project
  static hasMany = [subjects:Subject]
}
