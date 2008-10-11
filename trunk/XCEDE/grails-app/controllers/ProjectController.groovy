
import grails.converters.*

class ProjectController {
  def scaffold = Project
  
  def retrieve = {
    def results = null;
    if (params.id && Project.exists(params.id)) {
      results = Project.get( params.id );
    } else {
      results = Project.list();
    }
    switch ( params.output ) {
      case ~/(?i)json/:
        render results as JSON
      break
      default:
        def xml = new groovy.xml.MarkupBuilder ( response.getWriter() )
        xml.projects {
          for ( p in results ) {
            p.renderXML ( xml )
          }
        }
        break
      }
    };

    // Create a new object     
    def newProject = {
      // render params.keySet() as XML
      // render params.entrySet() as XML

      def node = new XmlSlurper().parse ( request.getReader() );

      render "Found tag ${node.name()}\n"
      render "Project title ${node.project.@title}"
      
      
      /*
      def p = new Project ( params['project'] )
      if ( p.save() ) {
        render p as XML
      } else {
        def errors = p.errors.allErrors.collect { g.message(error:it) }
        render ( contentType:"text/xml" ) {
          error {
            for ( err in errors ) {
              message ( error:err )
            }
          }
        }
      }
      */
    };
    
    def delete = {
      def p = Project.get(params.id)
      if ( p && p.delete() ) {
        response.setStatus ( 200 )
        render "${product.id} deleted"
      }
    };

    def update = {
      if (params["id"]) {
        // So cool!  Just grab the project indexed by id, update it's contents and save
      def p = Project.get(params["id"])
      p.properties = params["project"]
      // def p = new Project ( params['project'] )
      if ( p.save() ) {
        render p as XML
      } else {
        /*
          render ( "Update no id\n" )
          params.keySet().each() { render ( "${it}\n" )  }
          params.entrySet().each() { render ( "${it}\n" ) }
        */
        def errors = p.errors.allErrors.collect { p.message(error:it) }
        render ( contentType:"text/xml" ) {
          error {
            for ( err in errors ) {
              message ( error:err )
            }
          }
        }
      }
    }
  };
  
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
    };
    */
};
                
