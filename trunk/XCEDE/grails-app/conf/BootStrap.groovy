class BootStrap {

     def init = { servletContext ->
                  def steve = new Subject ( subjectID:"1234",name:"Steve Pieper" ).save()
                  def ron = new Subject ( subjectID:"5678",name:"Ron Kikinis" ).save()
                  def bill = new Subject ( subjectID:"9012",name:"Bill Lorensen" ).save()
                  def p = new Project(name:"BWH-0001", description:"Neurological implications of working at BWH").save()
                  def sg = new SubjectGroup()
                  sg.save()
                  sg.addToSubjects ( bill )
                  p.addToSubjectGroups ( sg );
                  new Project ( name:"Foo",description:"Bar",createdDateTime: new Date() )
                  .addToSubjectGroups ( new SubjectGroup().addToSubjects ( bill ) ).save()
     }
     def destroy = {
     }
} 
