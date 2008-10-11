class BootStrap {
  // def init = {};
  def init = { servletContext ->
               def steve = new Subject ( name:"Steve Pieper", sex:"m", species:"human", birthDate: new Date() ).save()
               // def ron = new Subject ( subjectID:"5678",name:"Ron Kikinis" ).save()
               // def bill = new Subject ( subjectID:"9012",name:"Bill Lorensen" ).save()
               def p = new Project(name:"BWH-0001", description:"Neurological implications of working at BWH", PID:"FooBar").save()
               // def sg = new SubjectGroup(name:"Foo-bar")
               // sg.save()
               // sg.addToSubjects ( steve )
               // p.addToSubjectGroups ( sg );
               // new Project ( name:"Foo",description:"Bar",createdDateTime: new Date() )
               // .addToSubjectGroups ( new SubjectGroup(name:"bar-foo").addToSubjects ( bill ) ).save()
  };
  def destroy = {
  }
} 
