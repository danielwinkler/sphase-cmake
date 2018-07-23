#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <vector>
#include "vector2.h"
#include "linesegment.h"

namespace Sceneparser {

template <typename number>
struct AreaStruct
{
    AreaStruct() : x(0.0), y(0.0), width(0.0), height(0.0) {}
    AreaStruct(number x, number y, number width, number height)
        : x(x), y(y), width(width), height(height) {}
    ~AreaStruct() { }

    number x;
    number y;
    number width;
    number height;
};

template <typename number>
struct Sph
{
    Sph();
    ~Sph() { }

    struct Stirrer {
        Stirrer();
        ~Stirrer() { }

        bool is_active;
        bool is_vertical;
        number origin_x, origin_y, velocity, length_x, length_y;
        enum Type {ellipsoidal_rotator=1, rectangular_fan=2};
        Type type;
    };

    number volume;
    number rho0_1;
    number rho0_2;
    number nu1;
    number nu2;
    number sd;
    number c1;
    number c2;
    number gamma1;
    number gamma2;
    number alpha;
    number epsilon_xsph;
    number epsilon_repulsion;
    number D_air;
    number D_water;
    number originx;
    number originy;
    number n_pl;
    number k_pl;
    bool no_slip;
    bool shepard;
    bool moving_least_squares;
    bool nonNewtonian;
    bool power_law;
    struct Stirrer stirrer, stirrer2;
    Vector2<number> g;
    number t_damp;
    bool hydrostatic_initialization;
};

template <typename number>
struct Scene
{
    Scene() : sample_dist(0.0), width(0.0), height(0.0), neighbours(0.0), originx(0.0), originy(0.0), hydrostatic_pressure(false), connect_boundaries(false) { }
    ~Scene() { }

    number		sample_dist; //initial interparticular distance (sd)
    number		width;
    number		height;
    number		neighbours; //smoothing length to sd ratio (h/sd)
    number      originx;
    number      originy;

    bool        hydrostatic_pressure;
    bool        connect_boundaries;
};

template <typename number>
struct Boundary
{
    Boundary() : is_moving(false) { }
    ~Boundary() { }

    bool is_moving;
    Vector2<number> velocity;
    std::vector<Vector2<number> >   lines;
};

template <typename number>
class Configuration
{
public:
    Configuration();
    ~Configuration();

    Scene<number> scene;
    Sph<number> sph;

    std::string json_file;
    std::string import_file;
    std::string export_file;
    std::string inflow_file;
    bool hydrostatic_pressure;

    number rec_step;
    number max_time;

    std::vector<AreaStruct<number> > fluid1;
    std::vector<AreaStruct<number> > fluid2;
    std::vector<Boundary<number> > boundaries;

    std::vector<LineSegment<number> > solid_boundaries;
    std::vector<LineSegment<number> > periodic_in, periodic_out, rigid_periodic_in, rigid_periodic_out;
    std::vector<LineSegment<number> > horizontal_in, bottom_in, open_out;
    std::vector<LineSegment<number> > vprofile;
    std::vector<LineSegment<number> > qcounter;
    std::vector<LineSegment<number> > hcounter;
};

}

#endif // CONFIGURATION_H
