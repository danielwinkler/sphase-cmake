#include <sphase/global.h>
#include <sphase/particles.h>

#include <algorithm>
#include <cassert>

#ifdef _MSC_VER
#include <Windows.h>
#endif

using namespace Sceneparser;

namespace Particlegenerator {

template<typename number>
ParticleHelper<number>::ParticleHelper(const Configuration<number>& configuration, bool grid_align)
    : configuration(configuration)
    , sample_dist(configuration.scene.sample_dist)
    , h(configuration.scene.neighbours * configuration.scene.sample_dist)
    , water_level(0.f)
    , hydro_pressure(configuration.scene.hydrostatic_pressure)
    , grid_align(grid_align)
{
    const double sdhalf = double(sample_dist) / 2.0;
    align_to_grid = [=](Vector2<number>& v)
    {
        if (true == grid_align) {
            v.x = round(double(v.x) / sdhalf) * sdhalf;
            v.y = round(double(v.y) / sdhalf) * sdhalf;
        }
    };
}

template<typename number>
ParticleHelper<number>::~ParticleHelper()
{
}

template<typename number>
Particles2<number>* ParticleHelper<number>::getParticles()
{
    initFromConfiguration();
    Particles2<number>* ret = new Particles2<number>();
    std::swap(*ret, particles);
    return ret;
}

template<typename number>
Particles2<number>* ParticleHelper<number>::createDigester(number radius, number centerx, number centery, number angletop, number angledown)
{
    const int wallWidth = floor(abs(h / sample_dist)) + 1;
    number fluidTop = centery + 10.21 + 0.05;
    number draftTop = centery + 9.876;
    number draftBottom = centery - 11.31;
    number draftRadius = 0.175;

    // helper
    number eps = sample_dist*0.01;

    auto alignToGrid = [&](Vector2<number>& v) {
        v.x = round(double(v.x) / sample_dist) * sample_dist;
        v.y = round(double(v.y) / sample_dist) * sample_dist;
    };

    // make sure we align to grid
    auto addBoundaryParticle = [&](Vector2<number> v, bool leftSide)
    {
        alignToGrid(v);

        number xInitial = v.x;
        number xInc = leftSide ? -sample_dist : sample_dist;

        // Boundary particles away from center
        for (int i = 0; i < wallWidth; ++i)
        {
            addParticle(new Particle2<number>(generateParticleID(), v, true, false));
            v.x += xInc;

//            printf("%f,%f;\n", v.x, v.y);
        }

        // Fluid particles towards center if below threshold
        if (v.y > fluidTop)
        {
            return;
        }

        const bool inDraftTube = v.y >= draftBottom && v.y <= draftTop;
        const bool inTopPlate = (v.y >= draftTop + 0.25) && v.y <= (draftTop + 0.25 + wallWidth*sample_dist);

        xInc *= -1.0;
        v.x = xInitial + xInc;
        bool stop(leftSide ? v.x > centerx : v.x < centerx);
        while (false == stop)
        {
            number distance = fabs(v.x - centerx);
            if (inDraftTube && distance > draftRadius && distance < (draftRadius + wallWidth*sample_dist)) {
                addParticle(new Particle2<number>(generateParticleID(), v, true, false));
            } else if (inTopPlate && distance < (draftRadius + (3+wallWidth)*sample_dist)) {
//                printf("in top plate\n");
                addParticle(new Particle2<number>(generateParticleID(), v, true, false));
            } else {
                addParticle(new Particle2<number>(generateParticleID(), v, false, true));
            }

            v.x += xInc;
            if (leftSide) {
                stop = v.x > centerx + eps;
            } else {
                stop = v.x < centerx;
            }
        }
    };

    // add center particle just to see where the center is
    // addParticle(new Particle2<number>(generateParticleID(), Vector2<number>(centerx,centery), true, false));


    // convert angles
    angletop *= M_PI/180.0;
    angledown *= M_PI/180.0;

    // set starting point to <centerx,centery>
    Vector2<number> center(centerx, centery);

    // go to side half focal distance
    number focalDistance = 8.0;

    //////////////////
    /// LEFT WALL
    //////////////////
    {
    Vector2<number> focalRight = center + Vector2<number>(focalDistance * 0.5, 0.0);

    // get left lower point of circle
    Vector2<number> startPoint(-cos(angledown)*radius, -sin(angledown)*radius);

    // get left top point of circle
    Vector2<number> endPoint(-cos(angletop)*radius, +sin(angletop)*radius);

    // align initial point and add
    addBoundaryParticle(focalRight + startPoint, true);

    size_t increaseY = 1;
    while (startPoint.y + increaseY * sample_dist < endPoint.y + eps)
    {
        number yPosition = startPoint.y + increaseY * sample_dist;
        number angle = asin(yPosition / radius);
        number xPosition = -cos(angle) * radius;
        addBoundaryParticle(focalRight + Vector2<number>(xPosition, yPosition), true);

        ++increaseY;
    }

    number topHeight = 1.02;
    number bottomHeight = 3.53;
///////////////////////////////////////////
    increaseY = 1;
    number angle = M_PI_2 - angledown;
    Vector2<number> tmp;
    while (increaseY * sample_dist < bottomHeight + eps)
    {
        number yPosition = increaseY * sample_dist;
        number xPosition = yPosition / tan(angle);
        tmp = focalRight + startPoint + Vector2<number>(xPosition, -yPosition);
        addBoundaryParticle(tmp, true);

        ++increaseY;
    }
    // now create bottom wall
    alignToGrid(tmp);
    tmp.x -= (wallWidth-1) * sample_dist;
    tmp.y -= sample_dist;
    number xEnd = centerx + fabs(centerx-tmp.x) + eps;
    for (size_t w = 0; w < wallWidth; ++w)
    {
        Vector2<number> start = tmp + Vector2<number>(0.0, -sample_dist * w);
        while (start.x < xEnd)//
        {
            addParticle(new Particle2<number>(generateParticleID(), start, true, false));
//            printf("%f,%f;\n", start.x, start.y);

            start.x += sample_dist;
        }
    }
//////////////////////////////////////////////
    increaseY = 1; //from 1 (gap)
    angle = M_PI_2 - angletop;
    while (increaseY * sample_dist < topHeight + eps)
    {
        number yPosition = increaseY * sample_dist;
        number xPosition = yPosition / tan(angle);
        tmp = focalRight + endPoint + Vector2<number>(xPosition, yPosition);
        addBoundaryParticle(tmp, true);

        ++increaseY;
    }
    // now create top wall
    alignToGrid(tmp);
    tmp.x -= (wallWidth-1) * sample_dist;
    tmp.y += sample_dist;
    xEnd = centerx + fabs(centerx-tmp.x) + eps;
    for (size_t w = 0; w < wallWidth; ++w)
    {
        Vector2<number> start = tmp + Vector2<number>(0.0, sample_dist * w);
        while (start.x < xEnd)//
        {
            addParticle(new Particle2<number>(generateParticleID(), start, true, false));
//            printf("%f,%f;\n", start.x, start.y);

            start.x += sample_dist;
        }
    }
    }
    //////////////////
    /// LEFT WALL END
    //////////////////

    //////////////////
    /// RIGHT WALL
    //////////////////
    {
    Vector2<number> focalLeft = center + Vector2<number>(-focalDistance * 0.5, 0.0);

    // get left lower point of circle
    Vector2<number> startPoint(cos(angledown)*radius, -sin(angledown)*radius);

    // get left top point of circle
    Vector2<number> endPoint(cos(angletop)*radius, +sin(angletop)*radius);

    // align initial point and add
    addBoundaryParticle(focalLeft + startPoint, false);

    size_t increaseY = 1;
    while (startPoint.y + increaseY * sample_dist < endPoint.y + eps)
    {
        number yPosition = startPoint.y + increaseY * sample_dist;
        number angle = asin(yPosition / radius);
        number xPosition = cos(angle) * radius;
        addBoundaryParticle(focalLeft + Vector2<number>(xPosition, yPosition), false);

        ++increaseY;
    }

    number topHeight = 1.02;
    number bottomHeight = 3.53;

    increaseY = 1;
    number angle = M_PI_2 - angledown;
    while (increaseY * sample_dist < bottomHeight + eps)
    {
        number yPosition = increaseY * sample_dist;
        number xPosition = -yPosition / tan(angle);
        addBoundaryParticle(focalLeft + startPoint + Vector2<number>(xPosition, -yPosition), false);

        ++increaseY;
    }

    increaseY = 1; // from 1
    angle = M_PI_2 - angletop;
    while (increaseY * sample_dist < topHeight + eps)
    {
        number yPosition = increaseY * sample_dist;
        number xPosition = -yPosition / tan(angle);
        addBoundaryParticle(focalLeft + endPoint + Vector2<number>(xPosition, yPosition), false);

        ++increaseY;
    }
    }
    //////////////////
    /// RIGHT WALL END
    //////////////////


    // create new vector
    Particles2<number>* ret = new Particles2<number>();
    // swap to give pointer ownership to caller
    std::swap(*ret, particles);
    return ret;
}


template<typename number>
unsigned int ParticleHelper<number>::generateParticleID()
{
    static unsigned int id = 0;
#ifdef __GNUC__
    return __sync_fetch_and_add(&id, 1);
#elif _MSC_VER
    return InterlockedIncrement(&id);
#endif
}

template<typename number>
void ParticleHelper<number>::initFromConfiguration()
{
    std::for_each(configuration.fluid1.cbegin(), configuration.fluid1.cend(), [this] (const AreaStruct<number>& ar) {
        addSquareFluid1(pair<number>(ar.x, ar.y), pair<number>(ar.width, ar.height));
    });
    std::for_each(configuration.fluid2.cbegin(), configuration.fluid2.cend(), [this] (const AreaStruct<number>& ar) {
        addSquareFluid2(pair<number>(ar.x, ar.y), pair<number>(ar.width, ar.height));
    });
    // TODO check rigid particles and stirrer particles
    //    std::for_each(configuration.boundaries.cbegin(), configuration.boundaries.cend(), [this] (const Boundary<number>& b) {
    //        std::unique_ptr<Vector2<number>> previous(nullptr);
    //        std::for_each(b.lines.cbegin(), b.lines.cend(), [this, &previous, &b] (const Vector2<number>& current) {
    //            if (previous) {
    //                addSolidBoundary(LineSegment<number>(*previous, current, b.velocity, b.is_moving));
    //            }
    //            previous.reset(new Vector2<number>(current));
    //        });
    //    });
    add_boundary_particles();
    add_inflow_particles();
}

template<typename number>
void ParticleHelper<number>::addParticle(Particle2<number>* p) {
    assert(p->pos.x >= 0 - configuration.sph.originx && p->pos.x < configuration.scene.width - configuration.sph.originx + configuration.scene.sample_dist); //Problematic (<15.55-12.3) & origin_x was missing in "sph"
    assert(p->pos.y >= 0 - configuration.sph.originy && p->pos.y < configuration.scene.height - configuration.sph.originy + configuration.scene.sample_dist);
    particles.push_back(p);
}

template<typename number>
void ParticleHelper<number>::addRigidParticle(const Vector2<number>& pos, const Vector2<number>& vel) {
    Particle2<number> *p_i = new Particle2<number>(generateParticleID(), pos, true, false, true);
    p_i->v_wall = vel;
    p_i->set(Rigid);
    addParticle(p_i);
}

template<typename number>
void ParticleHelper<number>::addStirrerParticle(const Vector2<number>& pos) {
    Particle2<number> *p_i = new Particle2<number>(generateParticleID(), pos, true, false, false);
    p_i->set(Stirrer);
    addParticle(p_i);
}

template<typename number>
void ParticleHelper<number>::popParticle() {
    particles.pop_back();
}

template<typename number>
void ParticleHelper<number>::addParticles(pair<number> pos, pair<number> dim, std::function<void(Particle2<number>&)> modifier) {
    const number sd2 = sample_dist / static_cast<number>(2.0);
    if (true == grid_align) {
        const number xsgn = (dim.first < 0 ? -1.0 : 1.0);
        const number ysgn = (dim.second < 0 ? -1.0 : 1.0);

        const number xstart = round(pos.first / sample_dist) * sample_dist + xsgn * sd2;
        const number ystart = round(pos.second / sample_dist) * sample_dist + ysgn * sd2;

//        printf("xstart=%f, ystart=%f\n",xstart,ystart);

        const number xend = round((pos.first + dim.first) / sample_dist) * sample_dist - xsgn * sd2;
        const number yend = round((pos.second + dim.second) / sample_dist) * sample_dist - ysgn * sd2;

//        printf("xend=%f, yend=%f\n",xend,yend);

        const size_t xsamples = abs(lround((xstart - xend) / sample_dist)) + 1;
        const size_t ysamples = abs(lround((ystart - yend) / sample_dist)) + 1;

//        printf("xsamples=%ld, ysamples=%ld\n",xsamples,ysamples);


        for (size_t yit = 0; yit < ysamples; ++yit) {
            for (size_t xit = 0; xit < xsamples; ++xit) {
                Vector2<number> p(xstart + xsgn * (xit * sample_dist)
                    , ystart + ysgn * (yit * sample_dist));
                align_to_grid(p);
                Particle2<number> *p_i = new Particle2<number>(generateParticleID(), p, false, false);
                modifier(*p_i);
                addParticle(p_i);
            }
        }
    } else {
        for (number y = pos.second + sd2; y < pos.second + dim.second - sd2; y += sample_dist) {
            for (number x = pos.first + sd2; x < pos.first + dim.first - sd2; x += sample_dist) {
                Vector2<number> p(x, y);
                Particle2<number> *p_i = new Particle2<number>(generateParticleID(), p, false, true);
                modifier(*p_i);
                addParticle(p_i);
            }
        }
    }
}

template<typename number>
void ParticleHelper<number>::addSquareFluid1(pair<number> pos, pair<number> dim) {
    addParticles(pos, dim, [](Particle2<number>& p)
    {
        p.set(Fluid);
    });
}

template<typename number>
void ParticleHelper<number>::addSquareFluid2(pair<number> pos, pair<number> dim) {
    addParticles(pos, dim, [](Particle2<number>& p)
    {
        p.set(Fluid);
        p.set(SecondPhase);
    });
}

template<typename number>
void ParticleHelper<number>::add_inflow_particles() {

    const number rho0_1 = configuration.sph.rho0_1;
    const number rho0_2 = configuration.sph.rho0_2;
    const number c1 = configuration.sph.c1;
    const number c2 = configuration.sph.c2;
    const number rho_part1 = configuration.sph.gamma1 / (rho0_1*c1*c1);
    const number rho_part2 = configuration.sph.gamma2 / (rho0_2*c2*c2);

    for (size_t b = 0; b < configuration.horizontal_in.size(); b++) {
        LineSegment<number> &in = configuration.horizontal_in[b];
        int layer = -1;
        const Vector2<number> in_m_normalized = in.m.normalized();

        while ((++layer)*sample_dist <= h)   {
            Vector2<number> dxv = in_m_normalized*sample_dist;
            Vector2<number> base = in.b + (in.wall_velocity.normalized()*sample_dist*layer);

            while ((base - in.b)*in_m_normalized < (in.m.length() + dxv.length() / 2)) {
                Particle2<number> *p = new Particle2<number>(generateParticleID(), base, false, false);

                if (!in.is_moving) {
                    p->p = hydro_pressure*configuration.sph.rho0_1*abs((configuration.sph.g*in.m.normalized())*((p->pos - in.b)*in.m.normalized()));
                    p->rho = rho0_1*std::pow(p->p*rho_part1 + 1, 1 / configuration.sph.gamma1);
                }
                else {
                    p->p = hydro_pressure*configuration.sph.rho0_2*abs((configuration.sph.g*in_m_normalized)*((p->pos - in.b)*in_m_normalized));
                    p->rho = rho0_2*std::pow(p->p*rho_part2 + 1, 1 / configuration.sph.gamma2);
                    p->set(SecondPhase);
                }
                p->v = in.wall_velocity;
                if (layer == 0)
                    p->set(InFlowNew);
                else
                    p->set(InFlow);
                particles.push_back(p);
                base += dxv;
            }
        }
    }

    for (size_t b = 0; b < configuration.bottom_in.size(); b++) {
        LineSegment<number> &in = configuration.bottom_in[b];
        int layer = -1;
        const Vector2<number> in_m_normalized = in.m.normalized();

        while ((++layer)*sample_dist <= h) {
            Vector2<number> dxv = in_m_normalized*sample_dist;
            Vector2<number> base = in.b + (in.wall_velocity.normalized()*sample_dist*layer);

            while ((base - in.b)*in_m_normalized < (in.m.length() + dxv.length() / 2)) {
                Particle2<number> *p = new Particle2<number>(generateParticleID(), base, false, false);
                p->p = 0;
                if (in.is_moving) {
                    p->rho = rho0_2;
                    p->set(SecondPhase);
                }
                else {
                    p->rho = rho0_1;
                }

                if (layer == 0)
                    p->set(InFlowNew);
                else
                    p->set(InFlow);
                particles.push_back(p);
                base += dxv;
            }
        }
        for (size_t i = 0; i < particles.size(); i++) {
            Particle2<number> *p_i = particles.at(i);

            if (!(p_i->is_wall()))
                water_level = std::max(water_level, in.normal_distance(p_i->pos).length());
        }
    }
}

template<typename number>
void ParticleHelper<number>::add_boundary_particles() {
    int wall_width = lround(fabs(h / sample_dist));
    // increase if cutoff radius is higher, e.g. 2.2, but not if it is a precision issue, e.g. 2.000002
    if (h > sample_dist * 1.001 * wall_width)
    {
        ++wall_width;
    }
    for (size_t i = 0; i < configuration.solid_boundaries.size(); i++) {
        LineSegment<number> &ls = configuration.solid_boundaries[i];

        if (true == grid_align) {
            // check if in continuous line
            bool is_line_strip = false;
            if (i > 0)
            {
                auto&& prev = configuration.solid_boundaries[i - 1];
                Vector2<number> diff(prev.b + prev.m - ls.b);
                if (configuration.scene.connect_boundaries && diff.length() < (sample_dist/10.0))
                    is_line_strip = true;
            }

            for (size_t wall_depth = 0; wall_depth < wall_width; ++wall_depth)
            {
                int samples = round(ls.m.length() / sample_dist);
                Vector2<number> dxv = ls.m.normalized()*sample_dist; // sampling step size
                Vector2<number> base = ls.b;
                base += ls.normal()*sample_dist*(number(wall_depth) + 0.5); // into wall depth dir
                base += dxv / 2; // into line dir
                if (is_line_strip)
                {
                    base -= dxv * wall_width;
                    samples += wall_width;
                }

                for (int s = 0; s < samples; ++s)
                {
                    align_to_grid(base);
                    Particle2<number> *p = new Particle2<number>(generateParticleID(), base, true, false, ls.is_moving);
                    p->v_wall = ls.wall_velocity;
                    particles.push_back(p);
                    base += dxv;
                }
            }
        } else {
             int layer = 0;

             while ((++layer)*sample_dist <= (h + sample_dist)) {
                 Vector2<number> dxv = ls.m.normalized()*sample_dist;
                 Vector2<number> base = ls.b + (ls.normal()*sample_dist*layer);
                 while ((base - ls.b)*ls.m.normalized() < (ls.m.length() + dxv.length() / 2)) {
                     Particle2<number> *p = new Particle2<number>(generateParticleID(), base, true, false, ls.is_moving);
                     p->v_wall = ls.wall_velocity;
                     if (ls.wall_velocity.length() > 0)
                         p->set(MovingWall);
                     particles.push_back(p);
                     base += dxv;
                 }
             }
        }
    }
}

template class ParticleHelper<float>;
template class ParticleHelper<double>;

}
