
import grails.converters.*

class RestController {

  def project = {
    def results = null
    // get data needed
    if (params.id && Project.exists(params.id)) {
      results = Project.get( params.id )
    } else {
      results = Project.list()
    }
    switch (params.output) {
    case ~/(?i)html/:
    if (params.id == null) {
      // Call another action in another controller class
      redirect(controller:'project', action:'list', params:params)
    } else {
      // Call another action in another controller class
      redirect(controller:'project', action:'show', params:params)
    }
    break
    case ~/(?i)json/:
    render results as JSON
    break
    default:        
    render results as XML
    break
    }
  }
  
    def subject = {
      def results = null
      // get data needed
      if (params.id && Subject.exists(params.id)) {
        results = Subject.get( params.id )
      } else  if ( params.name ) {
        results = Subject.findByNameLike ( '%' + params.name + '%' )
        // results = Subject.list()
      }
      switch (params.output) {
      case ~/(?i)html/:
      if (params.id == null) {
        // Call another action in another controller class
        redirect(controller:'subject', action:'list', params:params)
      } else {
        // Call another action in another controller class
        redirect(controller:'subject', action:'show', params:params)
      }
      break
      case ~/(?i)json/:
      render results as JSON
      break
      default:        
      render results as XML
      break
      }
    }
      


    def foo = {
      if ( params.id && Project.exists ( params.id ) ) {
        def p = Project.get(params.id)
        // render ( p as xml )
        new XML ( p ).render ( response )
        /*
          render ( contentType:'text/xml') {
          Projects {
          Project ( name:p.name, description:p.description, id:p.id )
          }
          }
        */
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
}
