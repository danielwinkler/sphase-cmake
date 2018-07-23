#include <sphase/configurationparser.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

namespace Sceneparser {


namespace
{
template <typename number>
QJsonArray parseLineSegmentVector(std::vector<LineSegment<number>> tmpLineSegmentVector) {
    QJsonArray list;
    LineSegment<number> lineStruct;
    for (int i = 0; i < tmpLineSegmentVector.size(); i++) {
        QJsonObject map;

        lineStruct = tmpLineSegmentVector[i];

        QJsonArray velocity;
        velocity.append(lineStruct.wall_velocity.x);
        velocity.append(lineStruct.wall_velocity.y);
        map["velocity"] = velocity;

        map["moving"] = lineStruct.is_moving;

        QJsonObject from;
        from["x"] = lineStruct.b.x;
        from["y"] = lineStruct.b.y;
        map["from"] = from;

        QJsonObject to;
        to["x"] = lineStruct.m.x;
        to["y"] = lineStruct.m.y;
        map["to"] = to;

        list.append(map);
    }

    return list;
}

template <typename number>
QJsonArray parseAreaStructVector(const Configuration<number>& c, std::vector<AreaStruct<number>> tmpAreaStructVector) {
    QJsonArray list;
    AreaStruct<number> areaStruct;

    for (int i = 0; i < tmpAreaStructVector.size(); i++) {
        QJsonObject area;
        areaStruct = tmpAreaStructVector[i];
        area["x"] = areaStruct.x + c.scene.originx;
        area["y"] = areaStruct.y + c.scene.originy;
        area["width"] = areaStruct.width;
        area["height"] = areaStruct.height;
        list.push_back(area);
    }

    return list;
}
}

template <typename number>
std::string ConfigurationParser<number>::getJson() {
    return json;
}

template <typename number>
ConfigurationParser<number>::ConfigurationParser(const Configuration<number> configuration)
    : c(configuration) {
    createJson();
}

template <typename number>
void ConfigurationParser<number>::createJson() {

    // TODO check if fields are available

    QJsonObject root;

    root["export_file"] = QString::fromStdString(c.export_file);
    root["import_file"] = QString::fromStdString(c.import_file);
    root["inflow_file"] = QString::fromStdString(c.inflow_file);
    root["max_time"] = c.max_time;
    root["json_file"] = QString::fromStdString(c.json_file);
    root["rec_step"] = c.rec_step;

    QJsonObject scene;
    scene["height"] = c.scene.height;
    scene["hydrostatic_pressure"] = c.scene.hydrostatic_pressure;
    scene["neighbours"] = c.scene.neighbours;
    scene["origin_x"] = c.scene.originx;
    scene["origin_y"] = c.scene.originy;
    scene["sampling_dist"] = c.scene.sample_dist;
    scene["width"] = c.scene.width;
    root["scene"] = scene;

    QJsonObject sph;
    sph["alpha"] = c.sph.alpha;
    sph["c1"] = c.sph.c1;
    sph["c2"] = c.sph.c2;
    sph["d_air"] = c.sph.D_air;
    sph["d_water"] = c.sph.D_water;
    sph["epsilon_repulsion"] = c.sph.epsilon_repulsion;
    sph["epsilon_xsph"] = c.sph.epsilon_xsph;
    QJsonArray g;
    g.append(c.sph.g.x);
    g.append(c.sph.g.y);
    sph["g"] = g;
    sph["gamma1"] = c.sph.gamma1;
    sph["gamma2"] = c.sph.gamma2;
    sph["moving_least_squares"] = c.sph.moving_least_squares;
    sph["no_slip"] = c.sph.no_slip;
    sph["nu1"] = c.sph.nu1;
    sph["nu2"] = c.sph.nu2;
    sph["origin_x"] = c.sph.originx;
    sph["origin_y"] = c.sph.originy;
    sph["rho0_1"] = c.sph.rho0_1;
    sph["rho0_2"] = c.sph.rho0_2;
    sph["sd"] = c.sph.sd;
    sph["shepard"] = c.sph.shepard;
    sph["t_damp"] = c.sph.t_damp;
    sph["volume"] = c.sph.volume;
    QJsonObject stirrer;
    stirrer["is_active"] = c.sph.stirrer.is_active;
    stirrer["length_x"] = c.sph.stirrer.length_x;
    stirrer["length_y"] = c.sph.stirrer.length_y;
    stirrer["origin_x"] = c.sph.stirrer.origin_x;
    stirrer["origin_y"] = c.sph.stirrer.origin_y;
    stirrer["type"] = c.sph.stirrer.type;
    if (c.sph.stirrer.is_vertical) {
        stirrer["velocity"] = c.sph.stirrer.velocity;
    } else {
        stirrer["velocity_x"] = c.sph.stirrer.velocity;
    }
    QJsonObject stirrer2;
    stirrer2["is_active"] = c.sph.stirrer.is_active;
    stirrer2["length_x"] = c.sph.stirrer.length_x;
    stirrer2["length_y"] = c.sph.stirrer.length_y;
    stirrer2["origin_x"] = c.sph.stirrer.origin_x;
    stirrer2["origin_y"] = c.sph.stirrer.origin_y;
    stirrer2["type"] = c.sph.stirrer.type;
    if (c.sph.stirrer2.is_vertical) {
        stirrer2["velocity"] = c.sph.stirrer2.velocity;
    } else {
        stirrer2["velocity_x"] = c.sph.stirrer2.velocity;
    }
    sph["stirrer"] = stirrer;
    sph["stirrer2"] = stirrer2;
    root["sph"] = sph;

    root["fluid1"] = parseAreaStructVector(c, c.fluid1);
    root["fluid2"] = parseAreaStructVector(c, c.fluid2);

    root["solid_boundaries"] = parseLineSegmentVector(c.solid_boundaries);

    root["periodic_in"] = parseLineSegmentVector(c.periodic_in);
    root["periodic_out"] = parseLineSegmentVector(c.periodic_out);
    root["rigid_periodic_in"] = parseLineSegmentVector(c.rigid_periodic_in);
    root["rigid_periodic_out"] = parseLineSegmentVector(c.rigid_periodic_out);

    root["horizontal_in"] = parseLineSegmentVector(c.horizontal_in);
    root["bottom_in"] = parseLineSegmentVector(c.bottom_in);
    root["open_out"] = parseLineSegmentVector(c.open_out);
    root["vprofile"] = parseLineSegmentVector(c.vprofile);
    root["q_counter"] = parseLineSegmentVector(c.qcounter);
    root["h_counter"] = parseLineSegmentVector(c.hcounter);

    QJsonDocument doc(root);
    json = QString::fromUtf8(doc.toJson().data()).toStdString();
}

template class ConfigurationParser<float>;
template class ConfigurationParser<double>;

}
