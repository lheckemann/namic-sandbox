class ProjectController {
  def scaffold = Project

    /*
    def showXML = {
      if ( params.id && Project.exists ( params.id ) ) {
        def p = Project.get(params.id)
        render ( contentType:'text/xml') {
          Projects {
            Project ( name:p.name, description:p.description, id:p.id )
          }
        }
      } else {
        def all = Project.list()
        render ( contentType:'text/xml') {
          Projects {
            for ( p in all ) {
              Project ( name:p.name, description:p.description, id:p.id )
            }
          }
        }
      }
    }
    */
}
