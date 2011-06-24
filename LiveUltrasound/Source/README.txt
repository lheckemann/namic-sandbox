Until Slicer4 doesn't support the OpenIGTLinkIF module we need to patch it: 
- Delete the Modules\OpenIGTLinkIF folder under Slicer4 source 
- Copy the LiveUltrasound\OpenIGTLinkIF folder to the Modules directory under Slicer4 source
- Uncheck the OpenIGTLinkIF_USE_VERSION_2 checkbox in the OpenIGTLink cmake configuration (under Slicer4 superbuild OpenIGTLink-build folder)
- Build the OpenIGTLink project 
- Build the Slicer project (not the superbuild)
- Configure LiveUltrasound module and build it
