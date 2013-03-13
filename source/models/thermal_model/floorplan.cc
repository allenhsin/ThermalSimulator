
#include <cassert>
#include <stddef.h>
#include <string.h>

#include "source/models/thermal_model/floorplan.h"
#include "libutil/Log.h"

namespace Thermal
{
    Floorplan::Floorplan()
        : _floorplan_holder (NULL)
    {}

    Floorplan::~Floorplan()
    {
        if(_floorplan_holder)
            delete _floorplan_holder;
    }

    int Floorplan::countFloorplanUnits(FILE* fp)
    {
        char str1[LINE_SIZE], str2[LINE_SIZE];
        char name[STR_SIZE];
        double leftx, bottomy, width, height;
        char *ptr;
        int count = 0;
    
        fseek(fp, 0, SEEK_SET);
        while(!feof(fp)) 
        {
            fgets(str1, LINE_SIZE, fp);
            if (feof(fp))
                break;
            strcpy(str2, str1);
            
            /* ignore comments and empty lines  */
            ptr = strtok(str1, " \r\t\n");
            if (!ptr || ptr[0] == '#')
                continue;
    
            /* functional block placement information   */
            if (sscanf(str2, "%s%lf%lf%lf%lf", name, &leftx, &bottomy, &width, &height) == 5)
                count++;
            else
            {
                LibUtil::Log::printLine(stderr, "\nERROR: wrong floorplan file format around line: %d.\n", count);
                abort();
            }
        }
        return count;
    } // countFloorplanUnits

    void Floorplan::populateFloorplanUnits(FILE* fp)
    {

        int i=0;
        char str[LINE_SIZE], copy[LINE_SIZE]; 
        char name1[STR_SIZE];
        double width, height, leftx, bottomy;
        char *ptr;
    
        assert(_floorplan_holder);

        fseek(fp, 0, SEEK_SET);
        while(!feof(fp))
        {
            fgets(str, LINE_SIZE, fp);
            if (feof(fp))
                break;
            strcpy(copy, str);
    
            /* ignore comments and empty lines  */
            ptr = strtok(str, " \r\t\n");
            if (!ptr || ptr[0] == '#')
                continue;
    
            if (sscanf(copy, "%s%lf%lf%lf%lf", name1, &width, &height, &leftx, &bottomy) == 5) 
            {
                strcpy(_floorplan_holder->_flp_units[i]._name, name1);
                _floorplan_holder->_flp_units[i]._width     = width;
                _floorplan_holder->_flp_units[i]._height    = height;
                _floorplan_holder->_flp_units[i]._leftx     = leftx;
                _floorplan_holder->_flp_units[i]._bottomy   = bottomy;
                i++;
            }
            else
            {
                LibUtil::Log::printLine(stderr, "\nERROR: wrong floorplan file format around line: %d.\n", i);
                abort();
            }
                
        }

        assert(i == _floorplan_holder->_n_units);
    } // populateFloorplanUnits


    void offsetFloorplanCoordinate(double x, double y)
    {
        int i;
        double minx = _floorplan_holder->_flp_units[0].leftx;
        double miny = _floorplan_holder->_flp_units[0].bottomy;

        for (i=1; i < _floorplan_holder->_n_units; i++) 
        {
            if (minx > _floorplan_holder->_flp_units[i].leftx)
                minx = _floorplan_holder->_flp_units[i].leftx;
            if (miny > _floorplan_holder->_flp_units[i].bottomy)
                miny = _floorplan_holder->_flp_units[i].bottomy;
        }
        for (i=0; i < _floorplan_holder->_n_units; i++) 
        {
            _floorplan_holder->_flp_units[i].leftx += (x - minx);
            _floorplan_holder->_flp_units[i].bottomy += (y - miny);
        }
    }

    void Floorplan::readFloorplan(std::string flp_file)
    {
        
        char    str[STR_SIZE];
        FILE    *fp;
        int     i, j;
        int     n_units = 0;
    
        fp = fopen (flp_file.c_str(), "r");

        if (!fp) 
        {
            LibUtil::Log::printLine(stderr, "\nERROR: cannot open floorplan file.\n");
            abort();
        }
    
        // 1st pass - find n_units
        n_units = countFloorplanUnits(fp);
        if(n_units<=0)
        {
            LibUtil::Log::printLine(stderr, "\nERROR: no units in floorplan file.\n");
            abort();
        }
        // allocate floorplan holder
        _floorplan_holder = new FloorplanHolder(n_units);
    
        // 2nd pass - populate units info
        populateFloorplanUnits(fp);

        fclose(fp); 

        // make sure the origin is (0,0)
        offsetFloorplanCoordinate(0, 0);   

    } // readFloorplan

} // namespace Thermal

