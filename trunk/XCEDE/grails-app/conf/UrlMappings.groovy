class UrlMappings {
    static mappings = {

      // Project settings
      // Normal Controller mapping
      "/project/$action?/$id?" (controller: "project" )
      // Our REST implementation
      "/rest/v1/project/$id?" (controller: "project" ) {
        action = [GET:"retrieve", PUT:"update", DELETE:"delete", POST:"newProject" ]
      }
      "/$controller/$action?/$id?"{
	      constraints {
			 // apply constraints here
		  }
	  }
	  "500"(view:'/error')
	}
}
