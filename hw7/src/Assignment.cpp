#include "Assignment.hpp"

#include "model.hpp"
#include "UI.hpp"
#include "Scene.hpp"
#include "PNGMaker.hpp"

#define XRES 250
#define YRES 250

using namespace std;
using namespace Eigen;

namespace Assignment {
    /* Assignment Part A, Part 1 */
    static const float k_min_io_test_x = -7.0;
    static const float k_max_io_test_x = 7.0;
    static const float k_min_io_test_y = -7.0;
    static const float k_max_io_test_y = 7.0;
    static const float k_min_io_test_z = -7.0;
    static const float k_max_io_test_z = 7.0;
    static const int k_io_test_x_count = 25;
    static const int k_io_test_y_count = 25;
    static const int k_io_test_z_count = 25;

    // Normalizes a vector described by three arguments.
    //
    // Arguments: float &x - Reference to x coordinate
    //            float &y - Reference to y coordinate
    //            float &z - Reference to z coordinate
    // Returns:   Nothing.
    void normalize(float &x, float &y, float &z) {
        float mag = sqrt(x*x + y*y + z*z);
        if (mag != 0) {
            x /= mag;
            y /= mag;
            z /= mag;
        }
    }

    // Normalizes a 4D MatrixXd vector
    //
    // Arguments: MatrixXd v - Vector to normalize
    // Returns:   MatrixXd - Normalized vector
    MatrixXd normalize(MatrixXd v) {
        // Get magnitude
        float mag = sqrt(v(0)*v(0) + v(1)*v(1) + v(2)*v(2));

        // Divide by magnitude and return;
        MatrixXd vNormalized(4, 1);
        vNormalized << v(0) / mag, v(1) / mag, v(2) / mag, 1;
        return vNormalized;
    }

    // Creates rotation matrix
    //
    // Arguments: float x - x component of rotation axis
    //            float y - y component of rotation axis
    //            float z - z component of rotation axis
    //            float angle - Degree measure of angle to rotate about
    //                  rotation axis
    // Returns:   MatrixXd - Rotation matrix
    MatrixXd generateRotationMatrix(float x, float y, float z, float angle) {
        // Generate camera rotation matrix
        MatrixXd Rc(4, 4);
        float cosT = cos(angle), sinT = sin(angle);
        Rc <<    x*x+(1-x*x)*cosT,   x*y*(1-cosT)-z*sinT,   x*z*(1-cosT)+y*sinT, 0,
              x*y*(1-cosT)+z*sinT,      y*y+(1-y*y)*cosT,   y*z*(1-cosT)-x*sinT, 0,
              x*z*(1-cosT)-y*sinT,   y*z*(1-cosT)+x*sinT,      z*z+(1-z*z)*cosT, 0,
                                0,                     0,                     0, 1;

        return Rc;
    }

    // Creates translation matrix
    //
    // Arguments: float x - translation in x direction
    //            float y - translation in y directin
    //            float z - translation in z direction
    // Returns:   MatrixXd - Translation matrix
    MatrixXd generateTranslationMatrix(float x, float y, float z) {
        // Generate camera translation matrix
        MatrixXd Tc(4, 4);
        Tc <<  1,  0,  0, x,
               0,  1,  0, y,
               0,  0,  1, z,
               0,  0,  0, 1;

        return Tc;
    }

    // Creates scaling matrix
    //
    // Arguments: float x - Scale in x dimension
    //            float y - Scale in y dimension
    //            float z - Scale in z dimension
    // Returns:   MatrixXd - Scaling matrix
    MatrixXd generateScalingMatrix(float x, float y, float z) {
        MatrixXd tMat(4, 4);
        // Scale
        tMat << x, 0, 0, 0,
                0, y, 0, 0,
                0, 0, z, 0,
                0, 0, 0, 1;

        return tMat;
    }

    // Applies the transformation matrix to the point p, inverting the matrix
    // first if the invert flag is true.
    //
    // Arguments: Transformation t - Transformation matrix for point p
    //            MatrixXd p - 4D point that transformation applies to
    //            bool invert - True if t should be inverted before applying
    // Returns:   Nothing.
    void applyTransformation(Transformation t, MatrixXd *p, bool invert) {
        float x = t.trans(0);
        float y = t.trans(1);
        float z = t.trans(2);
        float theta = t.trans(3);
        MatrixXd tMat(4, 4);
        // Create the transformation matrix depending on the type of
        // transformation
        switch (t.type) {
            case (TRANS):
                // Translation
                tMat = generateTranslationMatrix(x, y, z);
                break;
            case (SCALE):
                // Scale
                tMat = generateScalingMatrix(x, y, z);
                break;
            case (ROTATE):
                // Rotation
                // Normalize the axis of rotation
                normalize(x, y, z);
                tMat = generateRotationMatrix(x, y, z, theta);
                break;
        }
        // Apply inverse transform on p
        if (invert)
            tMat = tMat.inverse();
        *p = tMat * *p;
    }

    // Returns normal on primitive at point p
    //
    // Arguments: Primitive *prm - Primitive to find normal on
    //            Vector3f p - Point on primitive to find normal at
    //            vector<Transformation> *trans - Transformations the primitive
    //                  has underwent
    // Returns:   Vector3f - Normal vector
    Vector3f getNormal(Primitive *prm, Vector3f p,
        vector<Transformation> *trans) {
        // We need to apply (R^-1 S^-1)^T to the normal before using it.
        // Accumulate all of the scaling and rotations.
        MatrixXd S = MatrixXd::Identity(4, 4);
        MatrixXd R = MatrixXd::Identity(4, 4);
        MatrixXd T = MatrixXd::Identity(4, 4);
        for (unsigned int i = 0; i < trans->size(); ++i) {
            // Extract transformation and its values
            Transformation t = trans->at(i);
            float x = t.trans[0], y = t.trans[1], z = t.trans[2];
            float angle = t.trans[3];
            if (t.type == SCALE)
                S = S * generateScalingMatrix(x, y, z);
            else if (t.type == ROTATE)
                R = R * generateRotationMatrix(x, y, z, angle);
        }

        // We want to find normal at point without scale
        MatrixXd point(4, 1);
        point << p[0], p[1], p[2], 1;
        point = S.inverse() * point;
        p[0] = point(0);
        p[1] = point(1);
        p[2] = point(2);

        // First get the normal on the untransformed shape
        Vector3f norm = prm->getNormal(p);

        // Compute (R^-1 S^-1)^T
        MatrixXd transformation =
            (R.inverse() * S.inverse()).transpose();

        // Apply transformation to normal
        MatrixXd normMat(4, 1);
        normMat << norm[0], norm[1], norm[2], 1;
        normMat = transformation * normMat;

        // Normalize and put updated values back in normal vector
        norm[0] = normMat(0);
        norm[1] = normMat(1);
        norm[2] = normMat(2);
        normalize(norm[0], norm[1], norm[2]);

        return norm;
    }

    // Returns value of inside-outside function, computed by:
    //   f(x, y, z, e, n) = ((x^2)^(1/e) + (y^2)^(1/e))^(e/n) + (z^2)^(1/n) - 1
    //
    // Arguments: float x - x component of point
    //            float y - y component of point
    //            float z - z component of point
    //            float e - e exponent of primitive
    //            float n - n exponent of primitive
    // Returns:   float - Value of inside outside function.
    float insideOutside(float x, float y, float z, float e, float n) {
        // Compute square of point coordinates
        float x2 = x*x;
        float y2 = y*y;
        float z2 = z*z;

        // Compute ((x^2)^(1/e) + (y^2)^(1/e))^(e/n)
        float out = pow(x2, 1.0/e) + pow(y2, 1.0/e);
        out = pow(out, e/n);
        // Output final value
        return out + powf(z2, 1.0/n) - 1.0;
    }

    // Returns value of gradient of inside-outside function
    //
    // Arguments: float ax - x component of ray direction dotted with gradient
    //            float ay - y component of ray direction dotted with gradient
    //            float az - z component of ray direction dotted with gradient
    //            float x - x component of point
    //            float y - y component of point
    //            float z - z component of point
    //            Primitive *prm - Primitive the inside outside function is
    //                  computed from
    // Returns:   float - Value of gradient of inside outside function
    float insideOutsidePrime(float ax, float ay, float az,
        float x, float y, float z, Primitive *prm,
        vector<Transformation> *t) {
        // Components of normal (gradient)
        float dx, dy, dz;

        // Compute normal
        Vector3f p(x, y, z);
        Vector3f norm = getNormal(prm, p, t);

        dx = norm[0];
        dy = norm[1];
        dz = norm[2];

        // Result is the dot of a and gradient
        return ax*dx + ay*dy + az*dz;
    }

    bool IOTest(
        Renderable *ren,
        vector<Transformation> &transformation_stack,
        float x,
        float y,
        float z)
    {
        bool IO_result = false;
        if (ren->getType() == PRM) {
            Primitive *prm = dynamic_cast<Primitive*>(ren);

            // Make a vector out of out points
            MatrixXd p(4, 1);
            p << x / prm->getCoeff()[0],
                 y / prm->getCoeff()[1],
                 z / prm->getCoeff()[2],
                 1;
            // Perform all of the transformations onto this point
            for (int i = transformation_stack.size()-1; i >= 0; --i) {
                Transformation t = transformation_stack.at(i);
                // Apply the transformation
                applyTransformation(t, &p, true);
            }
            // Check if the transformed point is in the superquadric
            x = p(0);
            y = p(1);
            z = p(2);
            float e = prm->getExp0();
            float n = prm->getExp1();
            // Get the value of the inside-outside function
            IO_result = (insideOutside(x, y, z, e, n) < 0);
        } else if (ren->getType() == OBJ) {
            Object *obj = dynamic_cast<Object*>(ren);
            const vector<Transformation>& overall_trans =
                obj->getOverallTransformation();
            for (int i = overall_trans.size() - 1; i >= 0; i--) {
                transformation_stack.push_back(overall_trans.at(i));
            }

            for (auto& child_it : obj->getChildren()) {
                const vector<Transformation>& child_trans =
                    child_it.second.transformations;
                for (int i = child_trans.size() - 1; i >= 0; i--) {
                    transformation_stack.push_back(child_trans.at(i));
                }
                IO_result |= IOTest(
                    Renderable::get(child_it.second.name),
                    transformation_stack,
                    x, y, z);
                transformation_stack.erase(
                    transformation_stack.end() - child_trans.size(),
                    transformation_stack.end());
            }

            transformation_stack.erase(
                transformation_stack.end() - overall_trans.size(),
                transformation_stack.end());
        } else {
            fprintf(stderr, "Renderer::draw ERROR invalid RenderableType %d\n",
                ren->getType());
            exit(1);
        }

        return IO_result;
    }

    void drawIOTest() {
        const Line* cur_state = CommandLine::getState();
        Renderable* current_selected_ren = NULL;

        if (cur_state) {
            current_selected_ren = Renderable::get(cur_state->tokens[1]);
        }

        if (current_selected_ren == NULL) {
            return;
        }

        const float I_test_color[3] = {0.5, 0.0, 0.0};
        for (int x = 0; x < k_io_test_x_count; x++) {
            for (int y = 0; y < k_io_test_y_count; y++) {
                for (int z = 0; z < k_io_test_z_count; z++) {
                    float test_x = k_min_io_test_x
                        + x * (k_max_io_test_x - k_min_io_test_x)
                            / (float) k_io_test_x_count;
                    float test_y = k_min_io_test_y
                        + y * (k_max_io_test_y - k_min_io_test_y)
                            / (float) k_io_test_y_count;
                    float test_z = k_min_io_test_z
                        + z * (k_max_io_test_z - k_min_io_test_z)
                            / (float) k_io_test_z_count;

                    vector<Transformation> transformation_stack;
                    if (IOTest(
                            current_selected_ren,
                            transformation_stack,
                            test_x,
                            test_y,
                            test_z))
                    {
                        glPushMatrix();
                        glMaterialfv(GL_FRONT, GL_AMBIENT, I_test_color);
                        glTranslatef(test_x, test_y, test_z);
                        glutWireSphere(0.05, 4, 4);
                        glPopMatrix();
                    }
                }
            }
        }
    }

    /* Assignment Part A, Part 2 */
    struct Ray {
        float origin_x;
        float origin_y;
        float origin_z;

        float direction_x;
        float direction_y;
        float direction_z;

        Vector3f getLocation(float t) {
            Vector3f loc;
            loc << origin_x + t * direction_x,
                origin_y + t * direction_y,
                origin_z + t * direction_z;
            return loc;
        }
    };

    static vector<Ray> rays;
    static vector<float> times;
    static vector<Primitive> prms;
    static vector<vector<Transformation>> transformation_stacks;

    // Finds the point of intersection of the ray on primitive and adds it
    // to a global list of intersections
    //
    // Arguments: const Ray &camera_ray - Ray we are finding intersections
    //                  for
    //            Primitive *prm - Primitive we are looking for intersection
    //                  with
    //            vector<Transformation> trans - Transformations done on the
    //                  primitive
    // Returns:   Nothing.
    void findIntersections(const Ray &camera_ray, Primitive *prm,
        vector<Transformation> trans)
    {
        float tMin = -1;
        Ray intersection_ray;

        // Add the ray and corresponding intersection time to the vectors
        times.push_back(tMin);
        rays.push_back(intersection_ray);

        // Instantiate variables
        float e = prm->getExp0();
        float n = prm->getExp1();

        // Extract x,y,z of vectors a and b (direction and origin respectively)
        float ax = camera_ray.direction_x;
        float ay = camera_ray.direction_y;
        float az = camera_ray.direction_z;
        float bx = camera_ray.origin_x;
        float by = camera_ray.origin_y;
        float bz = camera_ray.origin_z;
        // Apply just rotation and scaling transformations to ray direction
        MatrixXd rDir(4, 1);
        rDir << ax, ay, az, 1;
        // Apply all transformations to ray origin
        MatrixXd rOrig(4, 1);
        rOrig << bx / prm->getCoeff()(0),
                 by / prm->getCoeff()(1),
                 bz / prm->getCoeff()(2),
                 1;
        // Apply relevant respective transformations
        for (int i = trans.size()-1; i >= 0; --i) {
            Transformation t = trans.at(i);
            // Apply all transformations to origin
            applyTransformation(t, &rOrig, true);
            // Don't apply translations to direction
            if (t.type != TRANS)
                applyTransformation(t, &rDir, true);
        }
        // Get transformed values back
        ax = rDir(0);
        ay = rDir(1);
        az = rDir(2);
        normalize(ax, ay, az);
        bx = rOrig(0);
        by = rOrig(1);
        bz = rOrig(2);

        // Want to solve for 0 = at^2 + bt + c where a = vecA dot vecA,
        // b = 2(vecA dot vecB) and c = vecB dot vecB - 3
        float a = ax*ax + ay*ay + az*az;
        float b = 2 * (ax*bx + ay*by + az*bz);
        float c = bx*bx + by*by + bz*bz - 3;
        float disc = b*b - 4*a*c;
        if (disc <  0)
            return;

        float tPlus = (-1 * b + sqrt(disc)) / (2 * a);
        float tMinus = (-1 * b - sqrt(disc)) / (2 * a);

        // If both are negative, we can't see the object with this ray
        if (tPlus < 0)
            return;

        // If both are negative, we only need to check value at tMinus, as
        // we are outside the bounding sphere and tMinus is closer.
        float tNew = tPlus;
        if (tMinus > 0)
            tNew = tMinus;

        // Do iteration until one twentieth of a pixel's width from surface
        float tOld;
        float goft, gpoft;
        int numIters = 0;
        for (bool done = false; !done; tNew = tMinus) {
            done = (tNew == tMinus);
            do {
                tOld = tNew;

                // Compute ray(t) so we can compute t_new
                float rayx, rayy, rayz;

                // Compute ray(tOld)
                rayx = ax * tOld + bx;
                rayy = ay * tOld + by;
                rayz = az * tOld + bz;

                // g(t) = sq_io(ray(t))
                goft = insideOutside(rayx, rayy, rayz, e, n);

                // g'(t) = vecA dot del sq_io(ray(t))
                gpoft = insideOutsidePrime(
                    ax, ay, az, rayx, rayy, rayz, prm, &trans
                );

                if (gpoft != 0)
                    // Compute t_new = t_old - g/g'
                    tNew = tOld - SLOWDOWN * (goft / gpoft);
                else
                    break;
            } while (abs(goft) > THRESHOLD and ++numIters < MAX_NEWTON);
            // Update tMin if necessary
            if (tOld > 0) {
                if (tOld < tMin or tMin < 0)
                    tMin = tOld;
            } else {
                // If we got a negative solution for either, we can't see the
                // object.
                tMin = -1;
                done = true;
            }
        }

        // Generate the ray we are returning based on the closest intersect
        if (tMin <= 0)
            return;

        // The origin is just on the original ray at t = tMin
        intersection_ray.origin_x = ax * tMin + bx;
        intersection_ray.origin_y = ay * tMin + by;
        intersection_ray.origin_z = az * tMin + bz;
        // Direction is the normal at the intersection point
        Vector3f point(intersection_ray.origin_x,
                       intersection_ray.origin_y,
                       intersection_ray.origin_z);
        Vector3f norm = getNormal(prm, point, &trans);
        intersection_ray.direction_x = norm[0];
        intersection_ray.direction_y = norm[1];
        intersection_ray.direction_z = norm[2];

        // Undo all the transformations
        MatrixXd p2(4, 1);
        p2 << ax * tMin + bx, ay * tMin + by, az * tMin + bz, 1;
        for (unsigned int i = 0; i < trans.size(); ++i) {
            Transformation t = trans.at(i);
            // Apply the transformation
            applyTransformation(t, &p2, false);
        }
        intersection_ray.origin_x = p2(0);
        intersection_ray.origin_y = p2(1);
        intersection_ray.origin_z = p2(2);

        // Add the ray and corresponding intersection time to the vectors
        times[times.size()-1] = tMin;
        rays[rays.size()-1] = intersection_ray;

        return;
    }

    // Fill global vector with all primitives that this renderable birthed
    //
    // Arguments: Renderable *ren - Renderable to find PRMS from.
    //            vector<Transformation> &trans - Transformations done on
    //                  renderable up to this point in recursion
    //            int depth - Current recursion depth
    // Returns:   Nothing.
    void getAllPrms(Renderable *ren, vector<Transformation> &trans, int depth) {
        if (depth > MAX_RECURSION)
            return;

        if (ren->getType() == PRM) {
            // Add to list of PRMs
            Primitive *prm = dynamic_cast<Primitive*>(ren);
            prms.push_back(*prm);
            vector<Transformation> prmTrans;
            for (int i = trans.size() - 1; i >= 0; i--) {
                prmTrans.push_back(trans.at(i));
            }
            transformation_stacks.push_back(prmTrans);
        } else if (ren->getType() == OBJ) {
            // Add all children to list of PRMs
            Object *obj = dynamic_cast<Object*>(ren);
            for (auto& child_it : obj->getChildren()) {
                // Get transformations for this child
                const vector<Transformation>& child_trans =
                    child_it.second.transformations;
                for (int i = child_trans.size() - 1; i >= 0; i--) {
                    trans.push_back(child_trans.at(i));
                }
                // Get all PRM children
                getAllPrms(Renderable::get(child_it.second.name), trans, depth + 1);
            }
        } else {
            fprintf(stderr, "Renderer::draw ERROR invalid RenderableType %d\n",
                ren->getType());
            exit(1);
        }
    }

    // Fill global vector with all primitives that this renderable birthed
    //
    // Arguments: Renderable *ren - Renderable to find PRMS from.
    //            vector<Transformation> &trans - Transformations done on
    //                  renderable up to this point in recursion
    //            int depth - Current recursion depth
    // Returns:   Nothing.
    void getAllPrms() {
        // Get closest intersection. Start with current renderable and apply
        // to all of its children recursively
        const Line* cur_state = CommandLine::getState();
        Renderable* ren = NULL;
        if (cur_state) {
            ren = Renderable::get(cur_state->tokens[1]);
        }
        if (ren == NULL) {
            return;
        }

        // Get all of the PRMs.  We will find the ray intersection with each
        // one then select the one closest to the camera
        prms.clear();
        transformation_stacks.clear();
        vector<Transformation> trans;
        getAllPrms(ren, trans, 0);
    }

    // Finds the closest intersection out of the list of intersection points
    // after filling vector of all intersection points.
    //
    // Arguments: Ray &intersection_ray - Normal for closest intersection
    //            Ray camera_ray - Ray we are looking for intersections with
    //            Primitive *prm - Primitive of closest intersection put here
    // Returns:   bool - True if camera ray intersects any primitives.
    bool findClosestIntersection(Ray &intersection_ray, Ray camera_ray, Primitive *prm) {
        // Find the intersection with all PRMs
        times.clear();
        rays.clear();
        for (unsigned int i = 0; i < prms.size(); ++i)
            findIntersections(camera_ray, &prms.at(i), transformation_stacks.at(i));

        // Return boolean that indicates whether there was a "closest
        // intersection"
        if (rays.size() == 0)
            return false;

        // Find the ray corresponding to smallest time
        float tMin = times.at(0), minIdx = 0;
        for (unsigned int i = 1; i < times.size(); ++i) {
            if (times.at(i) < tMin and tMin > 0) {
                tMin = times.at(i);
                minIdx = i;
            }
        }

        // Load the output parameter
        intersection_ray = rays.at(minIdx);
        if (prm)
            *prm = prms.at(minIdx);

        return true;
    }

    void drawIntersectTest(Camera *camera) {
        // Reset the list of PRMs in case anything has changed
        getAllPrms();

        // Closest intersection
        Ray camera_ray;
        // Get translation matrix
        MatrixXd Tc = generateTranslationMatrix(
            camera->position.x, camera->position.y, camera->position.z
        );
        // Get rotation matrix
        MatrixXd Rc = generateRotationMatrix(
            camera->axis.x, camera->axis.y, camera->axis.z, camera->angle
        );
        // Generate camera transformation matrix
        MatrixXd TcRc = (Tc * Rc).inverse();
        // We want to transform the original point (0, 0, -1)
        MatrixXd camOriginalDir(4, 1);
        camOriginalDir << 0, 0, -1, 1;
        MatrixXd camDir = TcRc * camOriginalDir;

        camera_ray.origin_x = camera->position.x;
        camera_ray.origin_y = camera->position.y;
        camera_ray.origin_z = camera->position.z;
        // Normalize camera ray direction
        float mag = sqrt(camDir(0)*camDir(0) +
            camDir(1)*camDir(1) + camDir(2)*camDir(2));
        camera_ray.direction_x = camDir(0) / mag;
        camera_ray.direction_y = camDir(1) / mag;
        camera_ray.direction_z = camDir(2) / mag;

        // Find the closest intersection, if it exists
        Ray intersection_ray;
        if (findClosestIntersection(intersection_ray, camera_ray, NULL)) {
            const float IO_test_color[3] = {0.0, 0.0, 0.5};
            glMaterialfv(GL_FRONT, GL_AMBIENT, IO_test_color);
            glLineWidth(3.0);
            glBegin(GL_LINES);
            glVertex3f(
                intersection_ray.origin_x,
                intersection_ray.origin_y,
                intersection_ray.origin_z);
            Vector3f endpoint = intersection_ray.getLocation(1.0);
            glVertex3f(endpoint[0], endpoint[1], endpoint[2]);
            glEnd();
        }
    }

    /* Assignment Part B */
    // Returns true if there is an object between the argued primitive and the
    // light source.
    //
    // Arguments: Primitive *prm - The primitive we are checking if obstructed
    //            PointLight l - Light source
    // Returns:   bool - True if the light is obstructed
    bool lightObstructed(Primitive *prm, Ray point, PointLight l) {
        // Create a ray from the light toward the point on this primitive,
        // which is just the origin of the argued normal
        Ray light_ray;
        light_ray.origin_x = l.position[0];
        light_ray.origin_y = l.position[1];
        light_ray.origin_z = l.position[2];
        light_ray.direction_x = point.origin_x - l.position[0];
        light_ray.direction_y = point.origin_y - l.position[1];
        light_ray.direction_z = point.origin_z - l.position[2];

        // We want to see if the first thing in front of the light is the same
        // as the primitive that is argued.
        Ray normal;
        findClosestIntersection(normal, light_ray, NULL);

        // Return true if they are the same, which is if the point of
        // intersection is on the surface of the argued primitive
        return (2 * THRESHOLD >=
            insideOutside(normal.origin_x, normal.origin_y, normal.origin_z,
                prm->getExp0(), prm->getExp1()));
    }

    // Implements Phong shading and returns the color at the given pixel
    //
    // Arguments: Primitive *prm - Primitive to shade
    //            Camera *e - Viewpoint of the camera/eye
    //            Scene *s - Information about the scene (particularly lights)
    // Returns:   Nothing
    void phongShader(Primitive *prm, Ray normal, Camera *e, Scene *s,
        float &r, float &g, float &b) {
        // Ensure normal is normalized
        normalize(normal.direction_x, normal.direction_y, normal.direction_z);

        // Phong shader described in assignment 2 lecture notes
        float cd = prm->getDiffuse();
        float ca = prm->getAmbient();
        float cs = prm->getSpecular();

        // Accumulate color values
        float dSum[3] = {0, 0, 0};
        float spSum[3] = {0, 0, 0};

        // Compute direction of view
        float eDirx = e->position.x - normal.origin_x;
        float eDiry = e->position.y - normal.origin_y;
        float eDirz = e->position.z - normal.origin_z;
        normalize(eDirx, eDiry, eDirz);

        // Compute contribution from each light
        for (unsigned int i = 0; i < s->lights.size(); ++i) {
            // i'th light
            PointLight l = s->lights.at(i);
            if (lightObstructed(prm, normal, l))
                continue;

            // Light direction
            float lDirx = l.position[0] - normal.origin_x;
            float lDiry = l.position[1] - normal.origin_y;
            float lDirz = l.position[2] - normal.origin_z;
            normalize(lDirx, lDiry, lDirz);

            // Dot light direction and normal.  Multiply this by each color
            // component and add to diffuse sum
            float dotProd = max((float) 0,
                normal.direction_x * lDirx +
                normal.direction_y * lDiry +
                normal.direction_z * lDirz
            );
            dSum[0] += l.color[0] * dotProd;
            dSum[1] += l.color[1] * dotProd;
            dSum[2] += l.color[2] * dotProd;

            // Add light direction to camera direction and normalize it
            float elDirx = eDirx + lDirx;
            float elDiry = eDiry + lDiry;
            float elDirz = eDirz + lDirz;
            normalize(elDirx, elDiry, elDirz);
            dotProd = max((float) 0,
                normal.direction_x * elDirx +
                normal.direction_y * elDiry +
                normal.direction_z * elDirz
            );
            spSum[0] += l.color[0] * dotProd;
            spSum[1] += l.color[1] * dotProd;
            spSum[2] += l.color[2] * dotProd;
        }

        // Add contribution of each light to compute r, g, b and scale it by
        // color components of the material itself
        r = min((float) 1, ca + dSum[0]*cd + spSum[0]*cs);// * prm->getColor().r;
        g = min((float) 1, ca + dSum[1]*cd + spSum[1]*cs);// * prm->getColor().g;
        b = min((float) 1, ca + dSum[2]*cd + spSum[2]*cs);// * prm->getColor().b;
    }

    /* Ray traces the scene. */
    void raytrace(Camera camera, Scene scene) {
        // Reset the list of PRMs in case anything has changed
        getAllPrms();

        // LEAVE THIS UNLESS YOU WANT TO WRITE YOUR OWN OUTPUT FUNCTION
        PNGMaker png = PNGMaker(XRES, YRES);

        // Get height and width of the front plane of the frustum
        float h = 2 * camera.near * tan(camera.fov / 2);
        float w = h * camera.aspect;

        // Get translation matrix
        MatrixXd Tc = generateTranslationMatrix(
            camera.position.x, camera.position.y, camera.position.z
        );
        // Get rotation matrix
        MatrixXd Rc = generateRotationMatrix(
            camera.axis.x, camera.axis.y, camera.axis.z, camera.angle
        );
        // Transformation matrix
        MatrixXd t = (Tc * Rc).inverse();
        // Create and transform basis vectors based on camera transformations
        MatrixXd e1(4, 1);
            e1 << 0, 0, -1, 1;
            e1 = normalize(t * e1);
        MatrixXd e2(4, 1);
            e2 << 1, 0, 0, 1;
            e2 = normalize(t * e2);
        MatrixXd e3(4, 1);
            e3 << 0, 1, 0, 1;
            e3 = normalize(t * e3);

        // This ray will update as we iterate, and we will use it to find each
        // intersection in our ray trace.
        Ray incident, normal;

        // Already know the origin is just the position fo the camera
        incident.origin_x = camera.position.x;
        incident.origin_y = camera.position.y;
        incident.origin_z = camera.position.z;

        // Variables used during iteration, consistent with homework 7b
        // prompt
        float x_i, y_j;

        // Shorthand
        float n = camera.near;

        // Compute pixel color on each pixel of the image
        for (int i = 0; i < XRES; i++) {
            x_i = (i - (XRES/2)) * (w/XRES);
            for (int j = 0; j < YRES; j++) {
                y_j = (j - (YRES/2)) * (h/YRES);
                // Define camera ray for this pixel
                incident.direction_x = n*e1(0) + x_i*e2(0) + y_j*e3(0);
                incident.direction_y = n*e1(1) + x_i*e2(1) + y_j*e3(1);
                incident.direction_z = n*e1(2) + x_i*e2(2) + y_j*e3(2);
                // Find normal at closest intersection (if it exists) and
                // color the PNG accordingly
                float r = 0, g = 0, b = 0;
                Primitive prm;
                if (findClosestIntersection(normal, incident, &prm)) {
                    phongShader(&prm, normal, &camera, &scene, r, g, b);
                }
                png.setPixel(i, j, r, g, b);
            }
            printf("%d of %d\n", i+1, XRES);
        }

        // LEAVE THIS UNLESS YOU WANT TO WRITE YOUR OWN OUTPUT FUNCTION
        if (png.saveImage()) {
            fprintf(stderr, "Error: couldn't save PNG image\n");
        } else {
            printf("DONE!\n");
        }
    }
};
