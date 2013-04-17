
#ifndef __THERMAL_FLOORPLAN_H__
#define __THERMAL_FLOORPLAN_H__

#include <string>
#include <stdio.h>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "source/models/thermal_model/thermal_constants.h"

namespace Thermal
{

    class FloorplanUnit
    {
    public:
        std::string _name;
        double      _width;
        double      _height;
        double      _leftx;
        double      _bottomy;
        bool        _filler;
    };

    class CompareFloorplanUnit
    {
    public:
        bool operator()(const FloorplanUnit& floorplan_unit_lhs, const FloorplanUnit& floorplan_unit_rhs) const
        { return (floorplan_unit_lhs._name < floorplan_unit_rhs._name); }
    }; // class CompareFloorplanUnit
    
    class FloorplanHolder
    {
    public:
        std::vector<FloorplanUnit>  _flp_units;
        int                         _n_units;

        double                      _total_width;
        double                      _total_height;

        FloorplanHolder()
            : _n_units      (0)
            , _total_width  (0)
            , _total_height (0)
        { _flp_units.clear(); }

        FloorplanHolder(int n_units)
            : _n_units      (n_units)
            , _total_width  (0)
            , _total_height (0)
        { 
            _flp_units.clear();
            _flp_units.resize(n_units);
        }

        ~FloorplanHolder()
        {}
    };

    class Floorplan
    {
    public:
        Floorplan();
        ~Floorplan();

        void loadFloorplan(std::string flp_file, std::string top_flp_object_name);

        const FloorplanHolder* getFloorplanHolder() { return _floorplan_holder; }

        void setFloorplanUnitNamesInTemperatureData();

        static bool isVertAdj(const FloorplanHolder* floorplan_holder, int i, int j);
        static bool isHorizAdj(const FloorplanHolder* floorplan_holder, int i, int j);
        static double getSharedLength(const FloorplanHolder* floorplan_holder, int i, int j);
        static int getUnitIndexFromName(const FloorplanHolder* floorplan_holder, const char* name);

    protected:
        void parseFloorplanFile(std::string flp_file, std::string top_flp_object_name);

        void offsetFloorplanCoordinate(double x, double y);
        void calculateChipTotalWidth();
        void calculateChipTotalHeight();

    private:
       FloorplanHolder* _floorplan_holder; 
       
       // data holder for floorplan file parsing
       std::map<std::string, std::set<FloorplanUnit, CompareFloorplanUnit> > _floorplan_objects;

       bool _top_flp_object_found;

    }; // class Floorplan

} // namespace Thermal

#endif // __THERMAL_FLOORPLAN_H__

