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

    void applyTransformation(Transformation t, MatrixXd *p, bool invert) {
        float x = t.trans(0);
        float y = t.trans(1);
        float z = t.trans(2);
        float theta = t.trans(3);
        float cosT, sinT, mag;
        MatrixXd tMat(4, 4);
        // Create the transformation matrix depending on the type of
        // transformation
        switch (t.type) {
            case (TRANS):
                // Translation
                tMat << 1, 0, 0, x,
                        0, 1, 0, y,
                        0, 0, 1, z,
                        0, 0, 0, 1;
                break;
            case (SCALE):
                // Scale
                tMat << x, 0, 0, 0,
                        0, y, 0, 0,
                        0, 0, z, 0,
                        0, 0, 0, 1;
                break;
            case (ROTATE):
                // Rotation
                // Normalize the axis of rotation
                mag = sqrt(x*x + y*y + z*z);
                x /= mag;
                y /= mag;
                z /= mag;

                // Last value of rotation is angle
                sinT = sin(theta);
                cosT = cos(theta);

                tMat <<    x*x+(1-x*x)*cosT,   x*y*(1-cosT)-z*sinT,   x*z*(1-cosT)+y*sinT, 0,
                        x*y*(1-cosT)+z*sinT,      y*y+(1-y*y)*cosT,   y*z*(1-cosT)-x*sinT, 0,
                        x*z*(1-cosT)-y*sinT,   y*z*(1-cosT)+x*sinT,      z*z+(1-z*z)*cosT, 0,
                                          0,                     0,                     0, 1;
                break;
        }
        // Apply inverse transform on p
        if (invert)
            tMat = tMat.inverse();
        *p = tMat * *p;
    }

    // Returns value of inside-outside function, computed by:
    //   f(x, y, z, e, n) = ((x^2)^(1/e) + (y^2)^(1/e))^(e/n) + (z^2)^(1/n) - 1
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

    // Returns gradient components
    void superquadricNormal(float *dx, float *dy, float *dz,
        float x, float y, float z, float e, float n) {
        // Compute squares for positive math
        float x2 = x*x;
        float y2 = y*y;
        float z2 = z*z;

        // Compute partials in each direction
        *dx = 2*x*pow(x2, (1/e)-1)*pow((pow(x2, 1/e) + pow(y2, 1/e)), (e/n)-1);
        *dy = 2*y*pow(y2, (1/e)-1)*pow((pow(x2, 1/e) + pow(y2, 1/e)), (e/n)-1);
        *dz = 2*z*pow(z2, (1/n)-1);
        *dx /= n;
        *dy /= n;
        *dz /= n;

        // Compute magnitude so we can normalize
        float mag = sqrt((*dx)*(*dx)+(*dy)*(*dy)+(*dz)*(*dz));
        if (mag != 0) {
            *dx /= mag;
            *dy /= mag;
            *dz /= mag;
        }
    }

    // Returns value of gradient of inside-outside function:
    float insideOutsidePrime(float ax, float ay, float az,
        float x, float y, float z, Primitive *prm) {
        // Components of normal (gradient)
        float dx, dy, dz;

        // Compute normal
        Vector3f p(x, y, z);
        Vector3f norm = prm->getNormal(p);
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
            for (unsigned int i = 0; i < transformation_stack.size(); ++i) {
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

    void findIntersections(const Ray &camera_ray, Primitive *prm,
        vector<Transformation> trans)
    {
        float tMin = -1;

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

        // Apply transformations to the ray
        MatrixXd p(4, 1);
        p << bx / prm->getCoeff()(0),
             by / prm->getCoeff()(1),
             bz / prm->getCoeff()(2),
             1;
        // Perform all of the transformations onto this point
        for (unsigned int i = 0; i < trans.size(); ++i) {
            Transformation t = trans.at(i);
            // Apply the transformation
            applyTransformation(t, &p, true);
        }
        bx = p(0);
        by = p(1);
        bz = p(2);

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

        // If both are negative, we can't see the object
        if (tPlus < 0)
            return;

        // Do iteration until one twentieth of a pixel's width from surface
        float tNew = tMinus, tOld;
        float goft, gpoft;
        for (bool done = false; !done; tNew = tPlus) {
            done = (tNew == tPlus);
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
                gpoft = insideOutsidePrime(ax, ay, az, rayx, rayy, rayz, prm);

                if (gpoft > 0)
                    // Compute t_new = t_old - g/g'
                    tNew = tOld - (goft / gpoft);
                else
                    break;
            } while (abs(goft) > THRESHOLD);
            // Update tMin if necessary
            if (tOld > 0) {
                if (tOld < tMin or tMin < 0)
                    tMin = tOld;
            }
        }

        // Generate the ray we are returning based on the closest intersect
        if (tMin <= 0)
            return;

        Ray intersection_ray;
        intersection_ray.origin_x = ax * tMin + bx;
        intersection_ray.origin_y = ay * tMin + by;
        intersection_ray.origin_z = az * tMin + bz;
        printf("%f %f %f\n", tMin, az, bz);
        superquadricNormal(
            &intersection_ray.direction_x,
            &intersection_ray.direction_y,
            &intersection_ray.direction_z,
            intersection_ray.origin_x,
            intersection_ray.origin_y,
            intersection_ray.origin_z,
            e, n);
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
        times.push_back(tMin);
        rays.push_back(intersection_ray);

        return;
    }

    // Fill global vector with all primitives that this renderable birthed
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

    bool findClosestIntersection(Ray &intersection_ray) {
        // Return boolean that indicates whether there was a "closest
        // intersection"
        if (rays.size() == 0)
            return false;

        // Find the ray corresponding to smallest time
        float tMin = times.at(0), minIdx = 0;
        for (unsigned int i = 1; i < times.size(); ++i) {
            if (times.at(i) < tMin) {
                tMin = times.at(i);
                minIdx = i;
            }
        }

        // Load the output parameter
        intersection_ray = rays.at(minIdx);

        return true;
    }

    void drawIntersectTest(Camera *camera) {
        // Closest intersection
        Ray camera_ray;
        float cx = camera->position.x;
        float cy = camera->position.y;
        float cz = camera->position.z;
        // Generate camera translation matrix
        MatrixXd Tc(4, 4);
        Tc <<  1,  0,  0, cx,
               0,  1,  0, cy,
               0,  0,  1, cz,
               0,  0,  0, 1;
        // Generate camera rotation matrix
        MatrixXd Rc(4, 4);
        float x = camera->axis.x;
        float y = camera->axis.y;
        float z = camera->axis.z;
        float cosT = cos(camera->angle), sinT = sin(camera->angle);
        Rc <<    x*x+(1-x*x)*cosT,   x*y*(1-cosT)-z*sinT,   x*z*(1-cosT)+y*sinT, 0,
              x*y*(1-cosT)+z*sinT,      y*y+(1-y*y)*cosT,   y*z*(1-cosT)-x*sinT, 0,
              x*z*(1-cosT)-y*sinT,   y*z*(1-cosT)+x*sinT,      z*z+(1-z*z)*cosT, 0,
                                0,                     0,                     0, 1;
        // Generate camera transformation matrix
        MatrixXd t = (Tc * Rc).inverse();
        // We want to transform the original point (0, 0, -1)
        MatrixXd camOrigin(4, 1);
        camOrigin << 0, 0, -1, 1;
        MatrixXd camDir = t * camOrigin;

        camera_ray.origin_x = cx;
        camera_ray.origin_y = cy;
        camera_ray.origin_z = cz;
        // Normalize camera ray direction
        float mag = sqrt(camDir(0)*camDir(0) +
            camDir(1)*camDir(1) + camDir(2)*camDir(2));
        camera_ray.direction_x = camDir(0) / mag;
        camera_ray.direction_y = camDir(1) / mag;
        camera_ray.direction_z = camDir(2) / mag;

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

        // Find the intersection with all PRMs
        times.clear();
        rays.clear();
        for (unsigned int i = 0; i < prms.size(); ++i)
            findIntersections(camera_ray, &prms.at(i), transformation_stacks.at(i));

        // Find the closest intersection, if it exists
        Ray intersection_ray;
        if (findClosestIntersection(intersection_ray)) {
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
            printf("%f %f %f %f %f %f\n", intersection_ray.origin_x, intersection_ray.origin_y, intersection_ray.origin_z, intersection_ray.direction_x, intersection_ray.direction_y, intersection_ray.direction_z);
        }
    }

    /* Assignment Part B */

    /* Ray traces the scene. */
    void raytrace(Camera camera, Scene scene) {
        // LEAVE THIS UNLESS YOU WANT TO WRITE YOUR OWN OUTPUT FUNCTION
        PNGMaker png = PNGMaker(XRES, YRES);

        // REPLACE THIS WITH YOUR CODE
        for (int i = 0; i < XRES; i++) {
            for (int j = 0; j < YRES; j++) {
                png.setPixel(i, j, 1.0, 1.0, 1.0);
            }
        }

        // LEAVE THIS UNLESS YOU WANT TO WRITE YOUR OWN OUTPUT FUNCTION
        if (png.saveImage()) {
            fprintf(stderr, "Error: couldn't save PNG image\n");
        } else {
            printf("DONE!\n");
        }
    }
};
