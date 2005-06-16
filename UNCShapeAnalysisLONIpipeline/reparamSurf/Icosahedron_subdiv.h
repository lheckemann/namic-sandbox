
#ifndef ICOSAHEDRON_SUBDIV_HH
#define ICOSAHEDRON_SUBDIV_HH

void set_up_icosahedron_triangs(int       subdiv,
        int      &n_vert,
        double * &icos,
        int      &n_triangs,
        int *    &triangs);
// calculate icosahedron subdivision on the sphere
// returning the spherical R3 sampling plus the 3D triangles 
// (as indexes to phi-theta-array)

//index = triangs[i*3 +0,1,2]
//icos-pts = [index*3 +0,1,2]
#endif
