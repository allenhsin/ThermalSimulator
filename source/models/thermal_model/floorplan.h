
#ifndef __THERMAL_FLOORPLAN_H__
#define __THERMAL_FLOORPLAN_H__

#include <string>
#include <stdio.h>

#include "source/models/thermal_model/thermal_constants.h"

namespace Thermal
{

    struct FloorplanUnit
    {
        char    _name[STR_SIZE];
        double  _width;
        double  _height;
        double  _leftx;
        double  _bottomy;
    };

    struct FloorplanHolder
    {
        FloorplanUnit*  _flp_units;
        int             _n_units;

        double          _total_width;
        double          _total_height;

        FloorplanHolder()
            : _flp_units    (NULL)
            , _n_units      (0)
            , _total_width  (0)
            , _total_height (0)
        {}

        FloorplanHolder(int n_units)
            : _flp_units    (new FloorplanUnit [n_units])
            , _n_units      (n_units)
            , _total_width  (0)
            , _total_height (0)
        {}

        ~FloorplanHolder()
        { if(_flp_units) delete [] _flp_units; }
    };

    class Floorplan
    {
    public:
        Floorplan();
        ~Floorplan();

        void readFloorplan(std::string flp_file);

        FloorplanHolder* getFloorplanHolder() { return _floorplan_holder; }

        static bool isVertAdj(FloorplanHolder* floorplan_holder, int i, int k);
        static bool isHorizAdj(FloorplanHolder* floorplan_holder, int i, int k);
        static double getSharedLength(FloorplanHolder* floorplan_holder, int i, int k);

    protected:
        int countFloorplanUnits(FILE* fp);
        void populateFloorplanUnits(FILE* fp);
        void offsetFloorplanCoordinate(double x, double y);

        void calculateChipTotalWidth();
        void calculateChipTotalHeight();

    private:
       FloorplanHolder* _floorplan_holder; 


    }; // class Floorplan

} // namespace Thermal

#endif // __THERMAL_FLOORPLAN_H__

