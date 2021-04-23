#include "Mesh.h"
#include <algorithm>
#include <float.h>
void Mesh::computeBB(){
    
    BBMin = Vec3Df( FLT_MAX, FLT_MAX, FLT_MAX );
    BBMax = Vec3Df( -FLT_MAX, -FLT_MAX, -FLT_MAX );
    
    for( unsigned int i = 0 ; i < vertices.size() ; i ++ ){
        const Vec3Df & point = vertices[i];
        for( int v = 0 ; v < 3 ; v++ ){
            float value = point[v];
            if( BBMin[v] > value ) BBMin[v] = value;
            if( BBMax[v] < value ) BBMax[v] = value;
        }
    }
    
    radius = (BBMax - BBMin).norm();
}

void Mesh::update(){
    computeBB();
    recomputeNormals();
    std::cout << "Mesh : " << vertices.size() << " vertices, " << triangles.size() << " triangles " << std::endl;
}

void Mesh::clear(){
    vertices.clear();
    
    triangles.clear();

    normals.clear();
    verticesNormals.clear();

}





void Mesh::recomputeNormals () {
    
    computeTriangleNormals();
    computeVerticesNormals();
    
}

void Mesh::computeTriangleNormals(){
    
    normals.clear();
    
    for(unsigned int i = 0 ; i < triangles.size() ; i++){
        normals.push_back(computeTriangleNormal(i));
    }
    
}

Vec3Df Mesh::computeTriangleNormal( int id ){
    
    const Triangle & t = triangles[id];
    Vec3Df normal = Vec3Df::crossProduct(vertices[t.getVertex (1)] - vertices[t.getVertex (0)], vertices[t.getVertex (2)]- vertices[t.getVertex (0)]);
    normal.normalize();
    return normal;
    
}


void Mesh::computeVerticesNormals(){
    
    verticesNormals.clear();
    verticesNormals.resize( vertices.size() , Vec3Df(0.,0.,0.) );

    for( unsigned int t = 0 ; t < triangles.size(); ++t )
    {
        Vec3Df const & tri_normal = normals[t];

        verticesNormals[ triangles[t].getVertex(0) ] += tri_normal;
        verticesNormals[ triangles[t].getVertex(1) ] += tri_normal;
        verticesNormals[ triangles[t].getVertex(2) ] += tri_normal;
    }

    for( unsigned int v = 0 ; v < verticesNormals.size() ; ++v )
    {
        verticesNormals[ v ].normalize();
    }
}

void Mesh::collectOneRing (std::vector< std::vector<unsigned int> > & oneRing) const {
    oneRing.resize (vertices.size ());
    for (unsigned int i = 0; i < triangles.size (); i++) {
        const Triangle & ti = triangles[i];
        for (unsigned int j = 0; j < 3; j++) {
            unsigned int vj = ti.getVertex (j);
            for (unsigned int k = 1; k < 3; k++) {
                unsigned int vk = ti.getVertex ((j+k)%3);
                if (std::find (oneRing[vj].begin (), oneRing[vj].end (), vk) == oneRing[vj].end ())
                    oneRing[vj].push_back (vk);
            }
        }
    }
}

void Mesh::glTriangle(unsigned int i){

    const Triangle & t = triangles[i];
    for( int j = 0 ; j < 3 ; j++ ){
        glNormal(verticesNormals[t.getVertex(j)]*normalDirection);
        glVertex(vertices[t.getVertex(j)]);
    }
    
}


void Mesh::sortFaces( FacesQueue & facesQueue ){
    float modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX , modelview);
    
    
    for (unsigned int t = 0 ; t < triangles.size() ; ++t )
    {
        Vec3Df _center = (
                vertices[ triangles[t].getVertex(0) ]+
                vertices[ triangles[t].getVertex(1) ]+
                vertices[ triangles[t].getVertex(2) ]) / 3.f;
        facesQueue.push( std::make_pair( modelview[2] * _center[0] + modelview[6] * _center[1] + modelview[10] * _center[2] + modelview[14] , t ) );
    }
    
}

void Mesh::draw( std::vector<bool> & selected, std::vector<bool> & fixed)
{

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH);

    glBegin (GL_TRIANGLES);

    for(unsigned int i = 0 ; i < triangles.size(); i++){

        int vi[3] = {triangles[i].getVertex(0), triangles[i].getVertex(1), triangles[i].getVertex(2)};
        if( selected[ vi[0] ] && selected[ vi[1] ] && selected[ vi[2] ])
            glColor3f( 0.8, 0.,0. );
        if( fixed[ vi[0] ] && fixed[ vi[1] ] && fixed[ vi[2] ])
            glColor3f( 0., 0.8,0. );
        else
            glColor3f(0.37,0.55,0.82);
        glTriangle( i );

    }

    glEnd();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_DEPTH);

}


void Mesh::draw()
{
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH);
        
    glBegin (GL_TRIANGLES);

    for(unsigned int i = 0 ; i < triangles.size(); i++){

        glTriangle( i);

    }
    
    glEnd();
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_DEPTH);
    
}
