#include "mat.h"
#include "mesh.h"
#include "texture.h"
#include "orbiter.h"
#include "draw.h"
#include "image_io.h"
#include "app_camera.h"        // classe Application a deriver
#include "uniforms.h"

#include "bezier.hpp"
#include "surface2D.hpp"
#include "deformation.hpp"

class CurveApp : public App {
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    CurveApp(const std::vector<Point> &ctrlPts, const std::vector<std::vector<Point>> &pts) : App(1024, 640),
                                                                                              m_camera(), bc(ctrlPts),
                                                                                              bs(pts) {
        // projection par defaut, adaptee a la fenetre
        m_camera.projection(window_width(), window_height(), 45);
    }

    int init() override {
        m_program_wireframe = read_program("Bezier/line.glsl");

        Point mesh_pmin, mesh_pmax;
        float precision = 0.01;


        //Use either Curve or Surface Calculations, surface will overwrite curve if both are used
        //Curve Calculation
        bc.CalculateCurvePointsAnalytical(curvePoints, precision);
        bc.getBounds(mesh_pmin, mesh_pmax);
        m_camera.lookat(mesh_pmin, mesh_pmax); // used if you generate no surface of revolution

        //Surface Calculation
        // Choose one
        //bs.CalculateSurfacePointsCasteljau(surfacePoints, precision, precision);
        bs.CalculateSurfacePointsAnalytical(surfacePoints, precision, precision);

        //Surface mesh generation or SOR mesh generation, choose one
        m_mesh = BezierSurface::genMesh(surfacePoints);
        //m_mesh = BezierCurve::makeSOR(curvePoints, 5);
        
        
        //Deformations Will aply
        //Taper::TaperMesh(m_mesh,2,0.2,1,2);
        //globalTwist(m_mesh, 0, [](float z) -> float {return z; });
        Taper::LocalTaper(m_mesh, 0, vec3(0, 0, 1), vec3(3, 6, 2), 0.2);
        

        //Wireframe curve, Uncomment from transform model to glDrawElements in render to use
        /*for (unsigned int i = 0; i < curvePoints.size() - 1; ++i) {
            ids.emplace_back(i);
            ids.emplace_back(i + 1);
        }
        glLineWidth(5.0);
        GLuint arrayBuffer, elementBuffer;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &arrayBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer);
        glBufferData(GL_ARRAY_BUFFER, curvePoints.size() * sizeof(vec3), &curvePoints[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &elementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (ids.size()) * sizeof(unsigned int), &ids[0], GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);*/

         // Wireframe surface, Uncomment from transform model to glDrawElements in render to use
        /*int sizeU = surfacePoints.size();
        int sizeV = surfacePoints[0].size();
        std::vector<Point> dataBuffer;
        for (int i = 0; i < sizeU; ++i)
            dataBuffer.insert(dataBuffer.end(), surfacePoints[i].begin(), surfacePoints[i].end());
        for (unsigned int i = 0; i < sizeU-1; ++i) {
            for (unsigned int j = 0; j < sizeV-1; ++j) {
                
                ids.emplace_back(i * sizeU + j);
                ids.emplace_back(i * sizeU + j + 1);
                ids.emplace_back(i * sizeU + j);
                ids.emplace_back((i + 1) * sizeU + j);
                ids.emplace_back((i + 1) * sizeU + j);
                ids.emplace_back((i + 1) * sizeU + j +1);
                ids.emplace_back((i + 1) * sizeU + j +1);
                ids.emplace_back(i * sizeU + j + 1);
                ids.emplace_back(i * sizeU + j + 1);
                ids.emplace_back((i + 1) * sizeU + j);
            }
        }

        GLuint arrayBuffer, elementBuffer;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &arrayBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer);
        glBufferData(GL_ARRAY_BUFFER, dataBuffer.size() * sizeof(vec3), &dataBuffer[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &elementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (ids.size()) * sizeof(unsigned int), &ids[0], GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);*/

        m_mesh.bounds(mesh_pmin, mesh_pmax);
        m_camera.lookat(mesh_pmin, mesh_pmax);


        return 0;
    }
        // dessiner une nouvelle image
    int render() override {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Comment from there to glDrawElements(included) if not wdrawing wireframe
        /*Transform model = Identity();//RotationY(global_time() / 40);
        Transform view= m_camera.view();
        
        Transform projection= m_camera.projection();
        
        Transform mv= view * model;
        Transform mvp= projection * mv;

        glBindVertexArray(vao);
        glUseProgram(m_program_wireframe);
        program_uniform(m_program_wireframe, "mvpMatrix", mvp);
        glDrawElements(GL_LINES,ids.size(), GL_UNSIGNED_INT, 0);*/
        
        draw(m_mesh, m_camera);
        return 1;
    }

    // destruction des objets de l'application
    int quit() override {
        m_mesh.release();
        return 0;
    }

    int prerender() override {
        // recupere les mouvements de la souris
        int mx, my;
        unsigned int mb = SDL_GetRelativeMouseState(&mx, &my);
        int mousex, mousey;
        SDL_GetMouseState(&mousex, &mousey);

        // deplace la camera
        if (mb & SDL_BUTTON(1))
            m_camera.rotation(float(mx), float(my));      // tourne autour de l'objet
        else if (mb & SDL_BUTTON(3))
            m_camera.translation((float) mx / (float) window_width(),
                                 (float) my / (float) window_height()); // deplace le point de rotation
        else if (mb & SDL_BUTTON(2))
            m_camera.move(float(mx));           // approche / eloigne l'objet

        SDL_MouseWheelEvent wheel = wheel_event();
        if (wheel.y != 0) {
            clear_wheel_event();
            m_camera.move(8.f * float(wheel.y));  // approche / eloigne l'objet
        }

        // appelle la fonction update() de la classe derivee
        return update(global_time(), delta_time());
    }

protected:
    Mesh m_mesh;
    Orbiter m_camera;
    GLuint vao{}, m_program_wireframe{};
    std::vector<vec3> curvePoints;
    std::vector<std::vector<vec3>> surfacePoints;
    BezierCurve bc;
    BezierSurface bs;

    std::vector<vec3> m_curve;
    std::vector<unsigned int> ids;
};

int main(int argc, char **argv) {
    std::vector<Point> ctrlPts = {Point(0, 0, 0), Point(3, 1, 0), Point(0.5, 2, 0), Point(1, 3, 0)};
    std::vector<std::vector<Point>> surfacePts = {
            {Point(0, 0, 0), Point(0, 0, 1), Point(0, 0, 2), Point(0, 0, 3)},
            {Point(1, 0, 0), Point(1, 1, 1), Point(1, 3, 2), Point(1, 0, 3)},
            {Point(2, 0, 0), Point(2, 2, 1), Point(2, 2, 2), Point(2, 0, 3)},
            {Point(3, 0, 0), Point(3, 3, 1), Point(3, 1, 2), Point(3, 0, 3)},
            {Point(4, 0, 0), Point(4, 0, 1), Point(4, 0, 2), Point(4, 0, 3)},
            {Point(5, 0, 0), Point(5, 0, 1), Point(5, 0, 2), Point(5, 0, 3)},
            {Point(6, 0, 0), Point(6, 1, 1), Point(6, 3, 2), Point(6, 0, 3)},
            {Point(7, 0, 0), Point(7, 2, 1), Point(7, 2, 2), Point(7, 0, 3)},
            {Point(8, 0, 0), Point(8, 3, 1), Point(8, 1, 2), Point(8, 0, 3)},
            {Point(9, 0, 0), Point(9, 0, 1), Point(9, 0, 2), Point(9, 0, 3)}
    };

    CurveApp curveApp(ctrlPts, surfacePts);
    curveApp.run();

    return 0;
}
