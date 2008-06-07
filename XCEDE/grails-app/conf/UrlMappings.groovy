class UrlMappings {
    static mappings = {

      // Our REST implementation
      "/rest/v1/$action?/$id?" (controller: "rest" )

      "/$controller/$action?/$id?"{
	      constraints {
			 // apply constraints here
		  }
	  }
	  "500"(view:'/error')
	}
}
