/******************************************************************************

 laplace.cpp

 Contains Laplace operator and related functions for smoothing.
 Takes each vertex and applies a variant of Laplace transform to them to apply
 a smoothing effect.  Details can be found at CS171 hw5 notes (last accessed
 11/28/16).  The vertices are transformed according to:
      v = (I - h∆)v_i
 where v is the resultant vertex coordinates, v_i is the original vertex
 coordinates (both 3D), I is the identity matrix in 3 dimensions, h is the
 supplied timestep and ∆ is the Laplace operator.  Here, v is argued.

 To compute the new vertex, it will be useful to know that we can compute
 ∆v as:
      (∆v)_i = 1 / (2A) * SUM_j { (cot(α_j) + cot(β_j)) (v_j - v) } = v0
 where A is the area of all faces who share vertex v and v_j is a vertex that
 shares an edge with v.  Call the edge e = (v, v_j) on the undirected graph,
 then there are two faces touching e, assuming a closed surface.  Each face
 has three vertices, two of which are v and v_j.  Call the third in each face
 v_α and v_β.  It should be clear that which is which does not matter, from
 the definition of Δv.  Then, α_j and β_j are the angles at vertices v_α and
 v_β, respectively.  Instead of computing cotangent of an angle Θ with sine
 and cosine, which can be computationally expensive, we take the vectors
 a = (v_Θ, v_j) and b = (v_Θ, v) and divide (a dot b) by || a cross b ||.
 Note that we can do this because a dot b = ||a|| ||b|| cosΘ and similarly,
 || a cross b || = ||a|| ||b|| sinΘ, whose quotient is cos/sin=cot.

 To take care of the vector subtraction, I also accumulate the sum of each
 row of the operator and include that on the diagonal.  This will perfectly
 offset the vector subtraction so we can do the backward Euler with a vector
 of v0 to vn, instead of a matrix of subtracted values.

 Author: Tim Menninger

******************************************************************************/

#include "laplace.h"

using namespace std;
using namespace Eigen;

/*
 computeVertexArea

 Computes the area of all of the faces that touch this vertex.

 Arguments: HEV *v1 - Halfedge vertex around which all faces that contain this
                vertex will be included in area sum.

 Returns:   (double) - The area of the triangles around this vertex.

 Revisions: 11/28/16 - Tim Menninger: Created
*/
double computeVertexArea(HEV *v1) {
    HE *he = v1->out;
    HEV *v2, *v3;
    double a = 0; // area

    do // iterate over all vertices adjacent to v_i
    {
        // Want all three vertices, which we will use to compute triangle area
        v1 = he->vertex;
        v2 = he->next->vertex;
        v3 = he->next->next->vertex;

        // Compute the area of this face
        a += computeTriangleArea(v1->toVec3f(), v2->toVec3f(), v3->toVec3f());

        he = he->flip->next;
    } while( he != v1->out );

    return a;
}

/*
 computeOperatorEntry

 Computes entry in operator based on two vertices, time step and area of
 the faces connected to v1.  The matrix is given by (I - h∆), so we have
 1-h on the diagonal and -h(cotα + cotβ) elsewhere, all multiplied by
 1 / (2*area).

 Arguments: HE *he - Halfedge of the vertex we are computing an entry for
            double h - The timestep used as a scalar
            double area - The area of the region used as a scalar

 Returns:   (double) - The value that should go in the operator for this
                halfedge.

 Revisions: 11/28/16 - Tim Menninger: Created
*/
double computeOperatorEntry(HE *he, double h, double area) {
    // The two vertices share an edge that splits 2 faces.  Get the two
    // vertices in the two-face system that we don't currently have.  We need
    // these to create vectors and compute cotangents
    HEV *v1 = he->vertex;
    HEV *v2 = he->flip->vertex;
    HEV *v_alpha = he->next->next->vertex;
    HEV *v_beta  = he->flip->next->next->vertex;

    // Add the cotangent of angles alpha and beta and add them.
    double cotSum = 0;
    for (HEV *v = v_alpha; v != v_beta; v = v_beta) {
        // The cotangent of an angle separating two vectors a and b is
        // (a dot b) / magnitude(a cross b).  The vector order does not matter
        // for this operation
        Vec3f a = v1->toVec3f() - v->toVec3f();
        Vec3f b = v2->toVec3f() - v->toVec3f();

        cotSum += a.dot(b) / a.cross(b).magnitude();
    }

    return area <= MIN_AREA ? 0 : -1 * h / (2 * area) * cotSum;
}

/*
 buildOperator

 Function to construct our operator in matrix form, according to CS171 hw5.
 This will be of the form F = (I - h∆).

 Arguments: Object *obj - The object for which we are building a Laplacian
                smoothing operator
            double h - The smoothing timestep

 Returns:   (SparseMatrix<double>) - A square matrix of side length equal to
                the number of vertices.  This is sparse because most entries
                are zero, so we only remember the indices and values of
                nonzero entries.

 Revisions: 11/28/16 - Tim Menninger: Created
*/
SparseMatrix<double> buildOperator( Object *obj, double h )
{
    vector<HEV*> *hevs = obj->hevs;
    vector<Vertex*> *vertices = obj->mesh.vertices;

    VectorXd sums( vertices->size() );

    // recall that due to 1-indexing of obj files, index 0 of our list doesn’t actually contain a vertex
    int numVertices = vertices->size() - 1;

    // initialize a sparse matrix to represent our ∆ operator
    SparseMatrix<double> D( numVertices, numVertices );

    // reserve room for non-zeros in each row of ∆
    D.reserve( VectorXi::Constant( numVertices, OP_NONZEROS ) );

    for( int i = 1; i < vertices->size(); ++i )
    {
        HEV *startHEV = hevs->at(i);
        HE  *he = startHEV->out;

        // Compute area of all faces that use this vertex
        double area = computeVertexArea(startHEV);

        // Sum of the row will be accumulated in place
        sums( i-1 ) = 0;

        do // iterate over all vertices adjacent to v_i
        {
            int j = he->next->vertex->index; // get index of adjacent vertex to v_i

            // Compute cotα + cotβ for this set of vertices (explained more in
            // file header)
            double entry = computeOperatorEntry(he, h, area);

            // fill the j-th slot of row i of our ∆ matrix with appropriate
            // value and add it to the accumulator
            D.insert( i-1, j-1 ) = entry;
            sums( i-1 ) += entry;

            he = he->flip->next;
        } while( he != startHEV->out );

        // Diagonal has to cancel out the vector subtraction
        D.insert( i-1, i-1 ) = 1 - sums( i-1 );
    }

    D.makeCompressed(); // optional; tells Eigen to more efficiently store our sparse matrix
    return D;
}

/*
 laplaceThisBitch

 Function that smooths all vertices in argued object for argued time step.

 Arguments: Object *obj - The object to smooth
            double h - The timestep to smooth by

 Returns:   Nothing.

 Revisions: 11/28/16 - Tim Menninger: Created
*/
void laplaceThisBitch( Object *obj, double h )
{
    vector<Vertex*> *vertices = obj->mesh.vertices;
    int numVertices = vertices->size() - 1;

    // get our matrix representation of ∆
    SparseMatrix<double> D = buildOperator( obj, h );

    // initialize Eigen’s sparse solver
    SparseLU<SparseMatrix<double>, COLAMDOrdering<int> > solver;

    // the following two lines essentially tailor our solver to our operator ∆
    solver.analyzePattern( D );
    if (solver.lastErrorMessage() != "")
        cout << "analyzePattern: " << solver.lastErrorMessage() << endl;
    solver.factorize( D );
    if (solver.lastErrorMessage() != "")
        cout << "factorize: " << solver.lastErrorMessage() << endl;

    // initialize our vector representation of v0
    VectorXd x0( numVertices );
    VectorXd y0( numVertices );
    VectorXd z0( numVertices );
    for( int i = 1; i < vertices->size(); ++i ) {
        Vertex *v = vertices->at(i);
        x0( i-1 ) = v->x;
        y0( i-1 ) = v->y;
        z0( i-1 ) = v->z;
    }

    // have Eigen solve for our new vertex vh
    VectorXd xh( numVertices );
    VectorXd yh( numVertices );
    VectorXd zh( numVertices );
    xh = solver.solve( x0 );
    yh = solver.solve( y0 );
    zh = solver.solve( z0 );

    // Create a new mesh to hold the new smoothed values.  The faces are the
    // same as the original mesh.
    Mesh_Data smoothed;
    smoothed.vertices = new vector<Vertex*>;
    smoothed.faces = obj->mesh.faces;

    // Record new vertex values
    smoothed.vertices->push_back(NULL);
    for( int i = 1; i < vertices->size(); ++i ) {
        Vertex *v = new Vertex(xh( i-1 ), yh( i-1 ), zh( i-1 ));
        smoothed.vertices->push_back(v);
    }

    // Create HEVs and HEFs based on vertices in smoothed mesh.
    vector<HEV*> *hevs = new vector<HEV*>;
    vector<HEF*> *hefs = new vector<HEF*>;
    build_HE(&smoothed, hevs, hefs);
    fillNormals(hevs);

    // Fill buffers in object based on new mesh
    obj->fillBuffers(hefs);

    // Done with smoothed things now that buffers are filled
    delete_HE(hevs, hefs);
    for (int i = 1; i < smoothed.vertices->size(); ++i) {
        delete smoothed.vertices->at(i);
    }
    delete smoothed.vertices;
}

/*
 smoothObjects

 Smooths all of the objects in the vector according to argued timestep.

 Arguments: vector<Object> *objs - Vector of objects to smooth.

 Returns:   Nothing.

 Revisions: 11/28/16 - Tim Menninger: Created
*/
void smoothObjects(vector<Object> *objs, double h) {
    vector<Object>::iterator obj = objs->begin();
    for (; obj != objs->end(); ++obj)
        laplaceThisBitch((Object *) &(*obj), h);
}
