
/***
 * Structure to remember the pedestrian and vehicle used roads to create
 * sidewalks and crossings.
 *
 * length is in kilometers (pgRouting-style)
 *
 * Sidewalk Characters are:
 *  'l' = left
 *  'r' = right
 *  'n' = none
 *
 */

#ifndef ROAD_HPP_
#define ROAD_HPP_

class Road {

    GeomOperate go;
    geom::GEOSFactory<> geos_factory;

public: 

    string name;
    string type;
    double length;
    Geometry *geometry;

    void init_road(Way& way) {
        name = TagCheck::get_name(way);
        type = TagCheck::get_highway_type(way);
        try {
            geometry = nullptr;
            geometry = geos_factory.create_linestring(way,
                    geom::use_nodes::unique,
                    geom::direction::forward).release();
        } catch (...) {
            cerr << " GEOS ERROR at way: " << way.id() << endl;
        }

        length = geometry->getLength();
	if (!geometry) {
	    cerr << "bad reference for geometry" << endl;
	    exit(1);
	}
    }

    void init_road(Way& way, Geometry* geometry) {
        name = TagCheck::get_name(way);
        type = TagCheck::get_highway_type(way);
        this->geometry = geometry;
        length = geometry->getLength();
	if (!geometry) {
	    cerr << "bad reference for geometry" << endl;
	    exit(1);
	}
    }

    OGRGeometry *get_ogr_geom() {
        return go.geos2ogr(geometry);
    }
};


class PedestrianRoad : public Road {

public:

    string osm_id;

    PedestrianRoad(Way& way) {
        init_road(way);
        osm_id = to_string(way.id());
    }

    PedestrianRoad(Way& way, Geometry* geometry) {
        init_road(way, geometry);
        osm_id = to_string(way.id());
    }


};


class VehicleRoad : public Road {
    
public:

    string osm_id;
    int lanes;
    char sidewalk;

    VehicleRoad(Way& way) {
        init_road(way);
        osm_id = to_string(way.id());
        sidewalk = TagCheck::get_sidewalk(way);
        lanes = TagCheck::get_lanes(way);
    }
};


class Sidewalk : public Road {
    
public:

    string osm_id;

    Sidewalk(string name, Geometry *geometry,
            string type, double length) {

        this->name = name;
	if (!geometry) {
	    cerr << "bad reference for geometry" << endl;
	    exit(1);
	}
        this->geometry = geometry;
        this->type = type;
        this->length = length;
        //this->osm_id = osm_id;
    }

    Sidewalk(Geometry *geometry, VehicleRoad *vehicle_road) {
        this->name = vehicle_road->name;
        this->geometry = geometry;
        this->type = vehicle_road->type;
        this->length = geometry->getLength();
        this->osm_id = vehicle_road->osm_id;
    }        
};


class Crossing : public Road {

public:
    
    string osm_id;

    Crossing(string name, Geometry *geometry,
            string type, double length) {

        this->name = name;
	if (!geometry) {
	    cerr << "bad reference for geometry" << endl;
	    exit(1);
	}
        this->geometry = geometry;
        this->type = type;
        this->length = length;
        this->osm_id = osm_id;
    }
};

#endif /* ROAD_HPP_ */
