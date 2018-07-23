#include <sphase/configuration.h>

namespace Sceneparser {

template <typename number>
Sph<number>::Stirrer::Stirrer()
    : is_active(false)
    , is_vertical(true)
    , origin_x(0)
    , origin_y(0)
    , velocity(0)
    , length_x(0)
    , length_y(0)
    , type(ellipsoidal_rotator)
{
}

template <typename number>
Sph<number>::Sph()
    : volume(0)
    , rho0_1(1000)
    , rho0_2(0)
    , nu1(1.00e-6)
    , nu2(1.28e-4)
    , sd(1)
    , c1(30)
    , c2(30)
    , gamma1(7)
    , gamma2(7)
    , alpha(0)
    , epsilon_xsph(0)
    , epsilon_repulsion(0.08)
    , D_air(2.14e-5)
    , D_water(10.0e-3)
    , originx(0)
    , originy(0)
    , no_slip(true)
    , shepard(true)
    , moving_least_squares(false)
    , t_damp(0.0)
    , hydrostatic_initialization(false)
    , nonNewtonian(false)
    , power_law(false)
    , n_pl(0.294) // power-law index
    , k_pl(1.0) // consistency index
{
}

template <typename number>
Configuration<number>::Configuration()
{

}

template <typename number>
Configuration<number>::~Configuration()
{

}

template class Configuration<float>;
template class Configuration<double>;
template struct Sph<float>;
template struct Sph<double>;

}
