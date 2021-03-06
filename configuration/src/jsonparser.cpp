#include <sphase/jsonparser.h>
#include <QFile>
#include <QDebug>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <fstream>
#include <exception>
#include <sstream>

namespace Sceneparser {

template <typename number>
JSONParser<number>::JSONParser(const std::string& json_file, const std::string& import_file, const std::string& export_file, const number& rec_step, const number& max_time, const std::string& inflow_file, bool grid_align)
    : json_file(json_file), import_file(import_file), export_file(export_file), rec_step(rec_step), max_time(max_time), inflow_file(inflow_file), grid_align(grid_align) {
    createConfiguration();
}

template <typename number>
void JSONParser<number>::createConfiguration() {
    QString qfilename = QString::fromStdString(json_file);
    if (!QFile::exists(qfilename)) {
        throw std::invalid_argument("file does not exist");
    }
    QFile f(qfilename);
    f.open(QIODevice::ReadOnly);
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &err);
    if (err.error !=  QJsonParseError::NoError) {
        std::stringstream sst;
        sst << err.offset << ": " << err.errorString().toStdString();
        throw std::runtime_error(sst.str().c_str());
    } else if (doc.isNull()) {
        throw std::runtime_error("JSON document is empty.");
    } else if (!doc.isObject()) {
        throw std::runtime_error("JSON document not in valid SPHASE input format");
    }

    QJsonObject root = doc.object();

    c.json_file = json_file;
    if (root.contains("json_file"))
        c.json_file = root["json_file"].toString().toStdString();
    c.import_file = import_file;
    if (root.contains("import_file"))
        c.import_file = root["import_file"].toString().toStdString();
    c.export_file = export_file;
    if (root.contains("export_file"))
        c.export_file = root["export_file"].toString().toStdString();
    c.inflow_file = inflow_file;
    if (root.contains("inflow_file"))
        c.inflow_file = root["inflow_file"].toString().toStdString();
    c.rec_step = rec_step;
    if (root.contains("rec_step"))
        c.rec_step = root["rec_step"].toDouble();
    c.max_time = max_time;
    if (root.contains("max_time"))
        c.max_time = root["max_time"].toDouble();

    if (!root.contains("scene") || !root["scene"].isObject()) {
        throw std::runtime_error("JSON document needs to contain scene configuration");
    }
    QJsonObject scene = root["scene"].toObject();
    if (scene.contains("hydrostatic_pressure")) {
        c.hydrostatic_pressure = scene["hydrostatic_pressure"].toBool();
    }

    // perform aligning arithmetic in double precision
    const double sampling_distance = scene["sampling_dist"].toDouble();
    auto align_to_grid = [&](double coordinate) -> double
    {
        if (true == grid_align){
            return round(coordinate / sampling_distance) * sampling_distance;
        } else {
            return coordinate;
        }
    };
    const number weird_shifting = grid_align ? 0.0 : sampling_distance / 2;

    c.scene.sample_dist = sampling_distance;
    c.scene.width = align_to_grid(scene["width"].toDouble());
    c.scene.height = align_to_grid(scene["height"].toDouble());
    if (scene.contains("neighbours")) {
        c.scene.neighbours = scene["neighbours"].toDouble();
    }
    if (scene.contains("hydrostatic_pressure")) {
        c.scene.hydrostatic_pressure = scene["hydrostatic_pressure"].toBool();
    }
    if (scene.contains("origin_x")) {
        c.scene.originx = align_to_grid(scene["origin_x"].toDouble());
        c.scene.originy = align_to_grid(scene["origin_y"].toDouble());
    }

    if (scene.contains("connect_boundaries")) {
        c.scene.connect_boundaries = scene["connect_boundaries"].toBool();
    }

    c.sph.sd = c.scene.sample_dist;
    if (root["fluid_parameters"].toObject().contains("density1")) {
        c.sph.rho0_1 = root["fluid_parameters"].toObject()["density1"].toDouble();
    }
    if (root["fluid_parameters"].toObject().contains("density2")) {
        c.sph.rho0_2 = root["fluid_parameters"].toObject()["density2"].toDouble();
    }
    if (root["fluid_parameters"].toObject().contains("polytropic_index1")) {
        c.sph.gamma1 = root["fluid_parameters"].toObject()["polytropic_index1"].toDouble();
    }
    if (root["fluid_parameters"].toObject().contains("polytropic_index2")) {
        c.sph.gamma2 = root["fluid_parameters"].toObject()["polytropic_index2"].toDouble();
    }
    if (root["sph"].toObject().contains("c")) {
        c.sph.c1 = root["sph"].toObject()["c"].toDouble();
    }
    if (c.sph.rho0_2 != 0)
        c.sph.c2 = c.sph.c1*sqrt(c.sph.rho0_1*c.sph.gamma2/(c.sph.rho0_2*c.sph.gamma1));
	else
		c.sph.c2 = 0;
    if (root["sph"].toObject().contains("nu")) {
        c.sph.nu1 = root["sph"].toObject()["nu"].toDouble();
    }
    else if (root["fluid_parameters"].toObject().contains("nu1")) {
        c.sph.nu1 = root["fluid_parameters"].toObject()["nu1"].toDouble();
    }
    if (root["fluid_parameters"].toObject().contains("nu2")) {
        c.sph.nu2 = root["fluid_parameters"].toObject()["nu2"].toDouble();
    }
    if (root["sph"].toObject().contains("alpha")) {
        c.sph.alpha = root["sph"].toObject()["alpha"].toDouble();
    }
    if (root["sph"].toObject().contains("no_slip")) {
        c.sph.no_slip = root["sph"].toObject()["no_slip"].toBool();
    }
    if (root["sph"].toObject().contains("shepard")) {
        c.sph.shepard = root["sph"].toObject()["shepard"].toBool();
    }
    if (root["sph"].toObject().contains("moving_least_squares")) {
        c.sph.moving_least_squares = root["sph"].toObject()["moving_least_squares"].toBool();
    }
    if (root["sph"].toObject().contains("epsilon_xsph")) {
        c.sph.epsilon_xsph = root["sph"].toObject()["epsilon_xsph"].toDouble();
    }
    if (root["sph"].toObject().contains("epsilon_repulsion")) {
        c.sph.epsilon_repulsion = root["sph"].toObject()["epsilon_repulsion"].toDouble();
    }
    if (root["sph"].toObject().contains("t_damp")) {
        c.sph.t_damp = root["sph"].toObject()["t_damp"].toDouble();
    }
    if (root["sph"].toObject().contains("hydrostatic_initialization")) {
        c.sph.hydrostatic_initialization = root["sph"].toObject()["hydrostatic_initialization"].toBool();
    }

    QJsonArray g_List = root["sph"].toObject()["g"].toArray();
    c.sph.g.x = g_List[0].toDouble();
    c.sph.g.y = g_List[1].toDouble();

    number calc_volume =  c.scene.sample_dist*c.scene.sample_dist;
    c.sph.volume = calc_volume;

    if (root["stirrer"].toObject().count()==6) {
        c.sph.stirrer.is_active = true;
        switch (root["stirrer"].toObject()["type"].toInt()) {
        case 1:
            c.sph.stirrer.type = Sph<number>::Stirrer::ellipsoidal_rotator;
            break;
        case 2:
            c.sph.stirrer.type = Sph<number>::Stirrer::rectangular_fan;
            break;
        default:
            throw std::invalid_argument("stirrer type unknown");
            break;
        }
        c.sph.stirrer.origin_x = (root["stirrer"].toObject()["origin_x"].toDouble());
        c.sph.stirrer.origin_y = (root["stirrer"].toObject()["origin_y"].toDouble());
        if (root["stirrer"].toObject().contains("velocity")) {
            c.sph.stirrer.velocity = root["stirrer"].toObject()["velocity"].toDouble();
            c.sph.stirrer.is_vertical = true;
        } else if (root["stirrer"].toObject().contains("velocity_x")) {
            c.sph.stirrer.velocity = root["stirrer"].toObject()["velocity_x"].toDouble();
            c.sph.stirrer.is_vertical = false;
        }
        c.sph.stirrer.length_x = (root["stirrer"].toObject()["length_x"].toDouble());
        c.sph.stirrer.length_y = (root["stirrer"].toObject()["length_y"].toDouble());
    }

    if (root["stirrer2"].toObject().count()==6) {
        c.sph.stirrer2.is_active = true;
        switch (root["stirrer2"].toObject()["type"].toInt()) {
        case 1:
            c.sph.stirrer2.type = Sph<number>::Stirrer::ellipsoidal_rotator;
            break;
        case 2:
            c.sph.stirrer2.type = Sph<number>::Stirrer::rectangular_fan;
            break;
        default:
            throw std::invalid_argument("stirrer type unknown");
            break;
        }
        c.sph.stirrer2.origin_x = (root["stirrer2"].toObject()["origin_x"].toDouble());
        c.sph.stirrer2.origin_y = (root["stirrer2"].toObject()["origin_y"].toDouble());
        if (root["stirrer2"].toObject().contains("velocity")) {
            c.sph.stirrer2.velocity = root["stirrer2"].toObject()["velocity"].toDouble();
            c.sph.stirrer2.is_vertical = true;
        } else if (root["stirrer2"].toObject().contains("velocity_x")) {
            c.sph.stirrer2.velocity = root["stirrer2"].toObject()["velocity_x"].toDouble();
            c.sph.stirrer2.is_vertical = false;
        }
        c.sph.stirrer2.length_x = (root["stirrer2"].toObject()["length_x"].toDouble());
        c.sph.stirrer2.length_y = (root["stirrer2"].toObject()["length_y"].toDouble());
    }

    foreach(QJsonValue fluid1, root["fluids"].toArray()) {
        if ((fluid1.toObject()["x"].toDouble() >= 0) && (fluid1.toObject()["y"].toDouble() >= 0)) {
            c.fluid1.emplace_back(
                align_to_grid(fluid1.toObject()["x"].toDouble()) - c.scene.originx
                , align_to_grid(fluid1.toObject()["y"].toDouble()) - c.scene.originy
                , align_to_grid(fluid1.toObject()["width"].toDouble())
                , align_to_grid(fluid1.toObject()["height"].toDouble())
            );
        }
    }

    foreach(QJsonValue fluid1, root["fluid1"].toArray()) {
        if ((fluid1.toObject()["x"].toDouble() >= 0) && (fluid1.toObject()["y"].toDouble() >= 0)) {
            c.fluid1.emplace_back(
                align_to_grid(fluid1.toObject()["x"].toDouble()) - c.scene.originx
                , align_to_grid(fluid1.toObject()["y"].toDouble()) - c.scene.originy
                , align_to_grid(fluid1.toObject()["width"].toDouble())
                , align_to_grid(fluid1.toObject()["height"].toDouble())
            );
        }
    }

    foreach(QJsonValue fluid2, root["fluid2"].toArray()) {
        if ((fluid2.toObject()["x"].toDouble() >= 0) && (fluid2.toObject()["y"].toDouble() >= 0)) {
            c.fluid2.emplace_back(
                align_to_grid(fluid2.toObject()["x"].toDouble()) - c.scene.originx
                , align_to_grid(fluid2.toObject()["y"].toDouble()) - c.scene.originy
                , align_to_grid(fluid2.toObject()["width"].toDouble())
                , align_to_grid(fluid2.toObject()["height"].toDouble())
            );
        }
    }

    foreach(QJsonValue outer_line_strip, root["boundaries"].toArray())
        foreach(QJsonValue line_strip, outer_line_strip.toArray())
        {
            // add line segments
            bool is_moving = line_strip.toObject()["moving"].toBool();
            QJsonArray velList = line_strip.toObject()["velocity"].toArray();
            Vector2<number> wall_velocity;
            wall_velocity.x = velList[0].toDouble();
            wall_velocity.y = velList[1].toDouble();
            Vector2<number> *last = 0;
            QJsonArray lines = line_strip.toObject()["lines"].toArray();
            foreach(QJsonValue line_item, lines) {
                number x = align_to_grid(line_item.toObject()["x"].toDouble()) + weird_shifting - c.scene.originx;
                number y = align_to_grid(line_item.toObject()["y"].toDouble()) + weird_shifting - c.scene.originy;
                Vector2<number> p(x, y);
                if (last == 0) {
                    last = new Vector2<number>(p);
                    continue;
                }
                c.solid_boundaries.push_back(LineSegment<number>(*last,p,wall_velocity,is_moving));
                delete last;
                last = new Vector2<number>(p);
            }
            delete last;
        }

    foreach(QJsonValue line_strip, root["solid_boundaries"].toArray())
    {
        bool is_moving = line_strip.toObject()["moving"].toBool();
        QJsonArray velList = line_strip.toObject()["velocity"].toArray();
        Vector2<number> wall_velocity;
        wall_velocity.x = velList[0].toDouble();
        wall_velocity.y = velList[1].toDouble();

        QJsonValue from_point = line_strip.toObject()["from"];
        Vector2<number> from(align_to_grid(from_point.toObject()["x"].toDouble()), align_to_grid(from_point.toObject()["y"].toDouble()));

        QJsonValue to_point = line_strip.toObject()["to"];
        Vector2<number> to(align_to_grid(to_point.toObject()["x"].toDouble()), align_to_grid(to_point.toObject()["y"].toDouble()));

        c.solid_boundaries.push_back(LineSegment<number>(from,from+to,wall_velocity,is_moving));
    }

    foreach(QJsonValue outer_line_strip, root["periodic_boundaries"].toArray())
    {
        bool is_rigid_periodic = false;
        Vector2<number> *in_from = 0, *in_to = 0, *out_from = 0, *out_to = 0;
        foreach(QJsonValue line_strip, outer_line_strip.toArray())
        {
            is_rigid_periodic = line_strip.toObject()["rigid_periodic"].toBool();
            QJsonArray lines = line_strip.toObject()["lines"].toArray();
            foreach(QJsonValue line_item, lines) {
                number x = align_to_grid(line_item.toObject()["x"].toDouble()) + weird_shifting - c.scene.originx;
                number y = align_to_grid(line_item.toObject()["y"].toDouble()) + weird_shifting - c.scene.originy;
                Vector2<number> p(x, y);
                if (in_from == 0) {
                    in_from = new Vector2<number>(p);
                    continue;
                }
                else if ((in_from != 0) && (in_to == 0)) {
                    in_to = new Vector2<number>(p);
                }
                else if ((in_to != 0) && (out_from == 0)) {
                    out_from = new Vector2<number>(p);
                }
                else if (out_to == 0) {
                    out_to = new Vector2<number>(p);
                }
            }
        }
        if (is_rigid_periodic) {
            c.rigid_periodic_in.push_back(LineSegment<number>(*in_from,*in_to,Vector2<number> (0,0),false));
            c.rigid_periodic_out.push_back(LineSegment<number>(*out_from,*out_to,Vector2<number> (0,0),false));
        } else {
            c.periodic_in.push_back(LineSegment<number>(*in_from,*in_to,Vector2<number> (0,0),false));
            c.periodic_out.push_back(LineSegment<number>(*out_from,*out_to,Vector2<number> (0,0),false));
        }
    }

    foreach(QJsonValue line_strip, root["periodic_in"].toArray())
    {
        bool is_moving = line_strip.toObject()["moving"].toBool();
        QJsonArray velList = line_strip.toObject()["velocity"].toArray();
        Vector2<number> wall_velocity;
        wall_velocity.x = velList[0].toDouble();
        wall_velocity.y = velList[1].toDouble();

        QJsonValue from_point = line_strip.toObject()["from"];
        Vector2<number> from(align_to_grid(from_point.toObject()["x"].toDouble()), align_to_grid(from_point.toObject()["y"].toDouble()));

        QJsonValue to_point = line_strip.toObject()["to"];
        Vector2<number> to(align_to_grid(to_point.toObject()["x"].toDouble()), align_to_grid(to_point.toObject()["y"].toDouble()));

        c.periodic_in.push_back(LineSegment<number>(from,from+to,wall_velocity,is_moving));
    }

    foreach(QJsonValue line_strip, root["periodic_out"].toArray())
    {
        bool is_moving = line_strip.toObject()["moving"].toBool();
        QJsonArray velList = line_strip.toObject()["velocity"].toArray();
        Vector2<number> wall_velocity;
        wall_velocity.x = velList[0].toDouble();
        wall_velocity.y = velList[1].toDouble();

        QJsonValue from_point = line_strip.toObject()["from"];
        Vector2<number> from(align_to_grid(from_point.toObject()["x"].toDouble()), align_to_grid(from_point.toObject()["y"].toDouble()));

        QJsonValue to_point = line_strip.toObject()["to"];
        Vector2<number> to(align_to_grid(to_point.toObject()["x"].toDouble()), align_to_grid(to_point.toObject()["y"].toDouble()));

        c.periodic_out.push_back(LineSegment<number>(from,from+to,wall_velocity,is_moving));
    }

    foreach(QJsonValue line_strip, root["rigid_periodic_in"].toArray())
    {
        bool is_moving = line_strip.toObject()["moving"].toBool();
        QJsonArray velList = line_strip.toObject()["velocity"].toArray();
        Vector2<number> wall_velocity;
        wall_velocity.x = velList[0].toDouble();
        wall_velocity.y = velList[1].toDouble();

        QJsonValue from_point = line_strip.toObject()["from"];
        Vector2<number> from(align_to_grid(from_point.toObject()["x"].toDouble()), align_to_grid(from_point.toObject()["y"].toDouble()));

        QJsonValue to_point = line_strip.toObject()["to"];
        Vector2<number> to(align_to_grid(to_point.toObject()["x"].toDouble()), align_to_grid(to_point.toObject()["y"].toDouble()));

        c.rigid_periodic_in.push_back(LineSegment<number>(from,from+to,wall_velocity,is_moving));
    }

    foreach(QJsonValue line_strip, root["rigid_periodic_out"].toArray())
    {
        bool is_moving = line_strip.toObject()["moving"].toBool();
        QJsonArray velList = line_strip.toObject()["velocity"].toArray();
        Vector2<number> wall_velocity;
        wall_velocity.x = velList[0].toDouble();
        wall_velocity.y = velList[1].toDouble();

        QJsonValue from_point = line_strip.toObject()["from"];
        Vector2<number> from(align_to_grid(from_point.toObject()["x"].toDouble()), align_to_grid(from_point.toObject()["y"].toDouble()));

        QJsonValue to_point = line_strip.toObject()["to"];
        Vector2<number> to(align_to_grid(to_point.toObject()["x"].toDouble()), align_to_grid(to_point.toObject()["y"].toDouble()));

        c.rigid_periodic_out.push_back(LineSegment<number>(from,from+to,wall_velocity,is_moving));
    }

    foreach(QJsonValue outer_line_strip, root["horizontal_inflow"].toArray())
        foreach(QJsonValue line_strip, outer_line_strip.toArray())
        {
            bool is_second_phase = line_strip.toObject()["second_phase"].toBool();
            QJsonArray velList = line_strip.toObject()["velocity"].toArray();
            Vector2<number> inflow_velocity;
            inflow_velocity.x = velList[0].toDouble();
            inflow_velocity.y = velList[1].toDouble();
            Vector2<number> *last = 0;
            QJsonArray lines = line_strip.toObject()["lines"].toArray();
            foreach(QJsonValue line_item, lines) {
                number x = (line_item.toObject()["x"].toDouble()) + weird_shifting - c.scene.originx;
                number y = (line_item.toObject()["y"].toDouble()) + weird_shifting - c.scene.originy;
                Vector2<number> p(x, y);
                if (last == 0) {
                    last = new Vector2<number>(p);
                    continue;
                }
                c.horizontal_in.push_back(LineSegment<number>(*last,p,inflow_velocity,is_second_phase));
                delete last;
                last = new Vector2<number>(p);
            }
            delete last;
        }

    foreach(QJsonValue line_strip, root["horizontal_in"].toArray())
    {
        bool is_moving = line_strip.toObject()["moving"].toBool();
        QJsonArray velList = line_strip.toObject()["velocity"].toArray();
        Vector2<number> wall_velocity;
        wall_velocity.x = velList[0].toDouble();
        wall_velocity.y = velList[1].toDouble();

        QJsonValue from_point = line_strip.toObject()["from"];
        Vector2<number> from((from_point.toObject()["x"].toDouble()), (from_point.toObject()["y"].toDouble()));

        QJsonValue to_point = line_strip.toObject()["to"];
        Vector2<number> to((to_point.toObject()["x"].toDouble()), (to_point.toObject()["y"].toDouble()));

        c.horizontal_in.push_back(LineSegment<number>(from,from+to,wall_velocity,is_moving));
    }

    foreach(QJsonValue outer_line_strip, root["bottom_inflow"].toArray())
        foreach(QJsonValue line_strip, outer_line_strip.toArray())
        {
            bool is_second_phase = line_strip.toObject()["second_phase"].toBool();
            QJsonArray velList = line_strip.toObject()["velocity"].toArray();
            Vector2<number> inflow_velocity;
            inflow_velocity.x = velList[0].toDouble();
            inflow_velocity.y = velList[1].toDouble();
            Vector2<number> *last = 0;
            QJsonArray lines = line_strip.toObject()["lines"].toArray();
            foreach(QJsonValue line_item, lines) {
                number x = (line_item.toObject()["x"].toDouble()) + weird_shifting - c.scene.originx;
                number y = (line_item.toObject()["y"].toDouble()) + weird_shifting - c.scene.originy;
                Vector2<number> p(x, y);
                if (last == 0) {
                    last = new Vector2<number>(p);
                    continue;
                }
                c.bottom_in.push_back(LineSegment<number>(*last,p,inflow_velocity,is_second_phase));
                delete last;
                last = new Vector2<number>(p);
            }
            delete last;
        }

    foreach(QJsonValue line_strip, root["bottom_in"].toArray())
    {
        bool is_moving = line_strip.toObject()["moving"].toBool();
        QJsonArray velList = line_strip.toObject()["velocity"].toArray();
        Vector2<number> wall_velocity;
        wall_velocity.x = velList[0].toDouble();
        wall_velocity.y = velList[1].toDouble();

        QJsonValue from_point = line_strip.toObject()["from"];
        Vector2<number> from((from_point.toObject()["x"].toDouble()), (from_point.toObject()["y"].toDouble()));

        QJsonValue to_point = line_strip.toObject()["to"];
        Vector2<number> to((to_point.toObject()["x"].toDouble()), (to_point.toObject()["y"].toDouble()));

        c.bottom_in.push_back(LineSegment<number>(from,from+to,wall_velocity,is_moving));
    }

    foreach(QJsonValue outer_line_strip, root["open_outflow_boundary"].toArray())
        foreach(QJsonValue line_strip, outer_line_strip.toArray())
        {
            Vector2<number> *last = 0;
            QJsonArray lines = line_strip.toObject()["lines"].toArray();
            foreach(QJsonValue line_item, lines) {
                number x = (line_item.toObject()["x"].toDouble()) + weird_shifting - c.scene.originx;
                number y = (line_item.toObject()["y"].toDouble()) + weird_shifting - c.scene.originy;
                Vector2<number> p(x, y);
                if (last == 0) {
                    last = new Vector2<number>(p);
                    continue;
                }
                c.open_out.push_back(LineSegment<number>(*last,p,Vector2<number> (0,0),false));
                delete last;
                last = new Vector2<number>(p);
            }
            delete last;
        }

    foreach(QJsonValue line_strip, root["open_out"].toArray())
    {
        bool is_moving = line_strip.toObject()["moving"].toBool();
        QJsonArray velList = line_strip.toObject()["velocity"].toArray();
        Vector2<number> wall_velocity;
        wall_velocity.x = velList[0].toDouble();
        wall_velocity.y = velList[1].toDouble();

        QJsonValue from_point = line_strip.toObject()["from"];
        Vector2<number> from((from_point.toObject()["x"].toDouble()), (from_point.toObject()["y"].toDouble()));

        QJsonValue to_point = line_strip.toObject()["to"];
        Vector2<number> to((to_point.toObject()["x"].toDouble()), (to_point.toObject()["y"].toDouble()));

        c.open_out.push_back(LineSegment<number>(from,from+to,wall_velocity,is_moving));
    }

    foreach(QJsonValue outer_line_strip, root["velocity_profile"].toArray())
        foreach(QJsonValue line_strip, outer_line_strip.toArray())
        {
            Vector2<number> *last = 0;
            QJsonArray lines = line_strip.toObject()["lines"].toArray();
            foreach(QJsonValue line_item, lines) {
                number x = (line_item.toObject()["x"].toDouble()) + weird_shifting - c.scene.originx;
                number y = (line_item.toObject()["y"].toDouble()) + weird_shifting - c.scene.originy;
                Vector2<number> p(x, y);
                if (last == 0) {
                    last = new Vector2<number>(p);
                    continue;
                }
                c.vprofile.push_back(LineSegment<number>(*last,p,Vector2<number> (0,0),false));
                delete last;
                last = new Vector2<number>(p);
            }
            delete last;
        }

    foreach(QJsonValue line_strip, root["vprofile"].toArray())
    {
        bool is_moving = line_strip.toObject()["moving"].toBool();
        QJsonArray velList = line_strip.toObject()["velocity"].toArray();
        Vector2<number> wall_velocity;
        wall_velocity.x = velList[0].toDouble();
        wall_velocity.y = velList[1].toDouble();

        QJsonValue from_point = line_strip.toObject()["from"];
        Vector2<number> from((from_point.toObject()["x"].toDouble()), (from_point.toObject()["y"].toDouble()));

        QJsonValue to_point = line_strip.toObject()["to"];
        Vector2<number> to((to_point.toObject()["x"].toDouble()), (to_point.toObject()["y"].toDouble()));

        c.vprofile.push_back(LineSegment<number>(from,from+to,wall_velocity,is_moving));
    }

    foreach(QJsonValue outer_line_strip, root["qcounter"].toArray())
        foreach(QJsonValue line_strip, outer_line_strip.toArray())
        {
            Vector2<number> *last = 0;
            QJsonArray lines = line_strip.toObject()["lines"].toArray();
            foreach(QJsonValue line_item, lines) {
                number x = (line_item.toObject()["x"].toDouble()) + weird_shifting - c.scene.originx;
                number y = (line_item.toObject()["y"].toDouble()) + weird_shifting - c.scene.originy;
                Vector2<number> p(x, y);
                if (last == 0) {
                    last = new Vector2<number>(p);
                    continue;
                }
                c.qcounter.push_back(LineSegment<number>(*last,p,Vector2<number> (0,0),false));
                delete last;
                last = new Vector2<number>(p);
            }
            delete last;
        }

    foreach(QJsonValue line_strip, root["q_counter"].toArray())
    {
        bool is_moving = line_strip.toObject()["moving"].toBool();
        QJsonArray velList = line_strip.toObject()["velocity"].toArray();
        Vector2<number> wall_velocity;
        wall_velocity.x = velList[0].toDouble();
        wall_velocity.y = velList[1].toDouble();

        QJsonValue from_point = line_strip.toObject()["from"];
        Vector2<number> from((from_point.toObject()["x"].toDouble()), (from_point.toObject()["y"].toDouble()));

        QJsonValue to_point = line_strip.toObject()["to"];
        Vector2<number> to((to_point.toObject()["x"].toDouble()), (to_point.toObject()["y"].toDouble()));

        c.qcounter.push_back(LineSegment<number>(from,from+to,wall_velocity,is_moving));
    }

    foreach(QJsonValue outer_line_strip, root["hcounter"].toArray())
        foreach(QJsonValue line_strip, outer_line_strip.toArray())
        {
            Vector2<number> *last = 0;
            QJsonArray lines = line_strip.toObject()["lines"].toArray();
            foreach(QJsonValue line_item, lines) {
                number x = (line_item.toObject()["x"].toDouble()) + weird_shifting - c.scene.originx;
                number y = (line_item.toObject()["y"].toDouble()) + weird_shifting - c.scene.originy;
                Vector2<number> p(x, y);
                if (last == 0) {
                    last = new Vector2<number>(p);
                    continue;
                }
                c.hcounter.push_back(LineSegment<number>(*last,p,Vector2<number> (0,0),false));
                delete last;
                last = new Vector2<number>(p);
            }
            delete last;
        }

    foreach(QJsonValue line_strip, root["h_counter"].toArray())
    {
        bool is_moving = line_strip.toObject()["moving"].toBool();
        QJsonArray velList = line_strip.toObject()["velocity"].toArray();
        Vector2<number> wall_velocity;
        wall_velocity.x = velList[0].toDouble();
        wall_velocity.y = velList[1].toDouble();

        QJsonValue from_point = line_strip.toObject()["from"];
        Vector2<number> from((from_point.toObject()["x"].toDouble()), (from_point.toObject()["y"].toDouble()));

        QJsonValue to_point = line_strip.toObject()["to"];
        Vector2<number> to((to_point.toObject()["x"].toDouble()), (to_point.toObject()["y"].toDouble()));

        c.hcounter.push_back(LineSegment<number>(from,from+to,wall_velocity,is_moving));
    }

}

template <typename number>
Configuration<number> JSONParser<number>::getConfiguration() {
    return c;
}


template class JSONParser<float>;
template class JSONParser<double>;

}
