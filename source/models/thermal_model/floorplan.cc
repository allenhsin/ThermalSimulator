
#include <cassert>
#include <stddef.h>
#include <string.h>
#include <map>

#include "source/models/thermal_model/floorplan.h"
#include "source/data/data.h"
#include "source/misc/misc.h"
#include "libutil/Log.h"
#include "libutil/String.h"

using LibUtil::String;
using std::map;

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
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: wrong floorplan file format around line: " + (String) count + ".\n");
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
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: wrong floorplan file format around line: i" + (String) i + ".\n");
                
        }

        assert(i == _floorplan_holder->_n_units);
    } // populateFloorplanUnits


    void Floorplan::offsetFloorplanCoordinate(double x, double y)
    {
        int i;
        double minx = _floorplan_holder->_flp_units[0]._leftx;
        double miny = _floorplan_holder->_flp_units[0]._bottomy;

        for (i=1; i < _floorplan_holder->_n_units; i++) 
        {
            if (minx > _floorplan_holder->_flp_units[i]._leftx)
                minx = _floorplan_holder->_flp_units[i]._leftx;
            if (miny > _floorplan_holder->_flp_units[i]._bottomy)
                miny = _floorplan_holder->_flp_units[i]._bottomy;
        }
        for (i=0; i < _floorplan_holder->_n_units; i++) 
        {
            _floorplan_holder->_flp_units[i]._leftx += (x - minx);
            _floorplan_holder->_flp_units[i]._bottomy += (y - miny);
        }
    } // offsetFloorplanCoordinate

    void Floorplan::calculateChipTotalWidth()
    {
        int i;
        double min_x =  _floorplan_holder->_flp_units[0]._leftx;
        double max_x =  _floorplan_holder->_flp_units[0]._leftx + 
                        _floorplan_holder->_flp_units[0]._width;
    
        for (i=1; i < _floorplan_holder->_n_units; i++) 
        {
            if (_floorplan_holder->_flp_units[i]._leftx < min_x)
                min_x = _floorplan_holder->_flp_units[i]._leftx;
            if (_floorplan_holder->_flp_units[i]._leftx + _floorplan_holder->_flp_units[i]._width > max_x)
                max_x = _floorplan_holder->_flp_units[i]._leftx + 
                        _floorplan_holder->_flp_units[i]._width;
        }
        
        _floorplan_holder->_total_width = (max_x - min_x);
    } // calculateChipTotalWidth

    void Floorplan::calculateChipTotalHeight()
    {
        int i;
        double min_y =  _floorplan_holder->_flp_units[0]._bottomy;
        double max_y =  _floorplan_holder->_flp_units[0]._bottomy + 
                        _floorplan_holder->_flp_units[0]._height;
     
        for (i=1; i < _floorplan_holder->_n_units; i++) 
        {
            if (_floorplan_holder->_flp_units[i]._bottomy < min_y)
                min_y = _floorplan_holder->_flp_units[i]._bottomy;
            if (_floorplan_holder->_flp_units[i]._bottomy + _floorplan_holder->_flp_units[i]._height > max_y)
                max_y = _floorplan_holder->_flp_units[i]._bottomy + 
                        _floorplan_holder->_flp_units[i]._height;
        }

        _floorplan_holder->_total_height = (max_y - min_y);
    } // calculateChipTotalHeight

    bool Floorplan::isVertAdj(FloorplanHolder* floorplan_holder, int i, int j)
    {
        assert(floorplan_holder);

        double x1, x2, x3, x4;
        double y1, y2, y3, y4;
    
        if (i == j)
            return false;
    
        x1 = floorplan_holder->_flp_units[i]._leftx;
        x2 = x1 + floorplan_holder->_flp_units[i]._width;
        x3 = floorplan_holder->_flp_units[j]._leftx;
        x4 = x3 + floorplan_holder->_flp_units[j]._width;
    
        y1 = floorplan_holder->_flp_units[i]._bottomy;
        y2 = y1 + floorplan_holder->_flp_units[i]._height;
        y3 = floorplan_holder->_flp_units[j]._bottomy;
        y4 = y3 + floorplan_holder->_flp_units[j]._height;
    
        /* diagonally adjacent => not adjacent */
        if (Misc::eq(x2,x3) && Misc::eq(y2,y3))
            return false;
        if (Misc::eq(x1,x4) && Misc::eq(y1,y4))
            return false;
        if (Misc::eq(x2,x3) && Misc::eq(y1,y4))
            return false;
        if (Misc::eq(x1,x4) && Misc::eq(y2,y3))
            return false;
    
        if (Misc::eq(y1,y4) || Misc::eq(y2,y3))
            if ((x3 >= x1 && x3 <= x2) || (x4 >= x1 && x4 <= x2) ||
                (x1 >= x3 && x1 <= x4) || (x2 >= x3 && x2 <= x4))
                return true;
    
        return false;
    } // isVertAdj

    bool Floorplan::isHorizAdj(FloorplanHolder* floorplan_holder, int i, int j)
    {
        assert(floorplan_holder);

        double x1, x2, x3, x4;
        double y1, y2, y3, y4;
    
        if (i == j) 
            return false;
    
        x1 = floorplan_holder->_flp_units[i]._leftx;
        x2 = x1 + floorplan_holder->_flp_units[i]._width;
        x3 = floorplan_holder->_flp_units[j]._leftx;
        x4 = x3 + floorplan_holder->_flp_units[j]._width;
    
        y1 = floorplan_holder->_flp_units[i]._bottomy;
        y2 = y1 + floorplan_holder->_flp_units[i]._height;
        y3 = floorplan_holder->_flp_units[j]._bottomy;
        y4 = y3 + floorplan_holder->_flp_units[j]._height;
    
        /* diagonally adjacent => not adjacent */
        if (Misc::eq(x2,x3) && Misc::eq(y2,y3))
            return false;
        if (Misc::eq(x1,x4) && Misc::eq(y1,y4))
            return false;
        if (Misc::eq(x2,x3) && Misc::eq(y1,y4))
            return false;
        if (Misc::eq(x1,x4) && Misc::eq(y2,y3))
            return false;
    
        if (Misc::eq(x1,x4) || Misc::eq(x2,x3))
            if ((y3 >= y1 && y3 <= y2) || (y4 >= y1 && y4 <= y2) ||
                (y1 >= y3 && y1 <= y4) || (y2 >= y3 && y2 <= y4))
                return true;
    
        return false;
    } // isHorizAdj

    double Floorplan::getSharedLength(FloorplanHolder* floorplan_holder, int i, int j)
    {
        assert(floorplan_holder);

        double p11, p12, p21, p22;
        p11 = p12 = p21 = p22 = 0.0;
    
        if (i==j) 
            return false;
    
        if (isHorizAdj(floorplan_holder, i, j)) 
        {
            p11 = floorplan_holder->_flp_units[i]._bottomy;
            p12 = p11 + floorplan_holder->_flp_units[i]._height;
            p21 = floorplan_holder->_flp_units[j]._bottomy;
            p22 = p21 + floorplan_holder->_flp_units[j]._height;
        }
    
        if (isVertAdj(floorplan_holder, i, j)) 
        {
            p11 = floorplan_holder->_flp_units[i]._leftx;
            p12 = p11 + floorplan_holder->_flp_units[i]._width;
            p21 = floorplan_holder->_flp_units[j]._leftx;
            p22 = p21 + floorplan_holder->_flp_units[j]._width;
        }
    
        return (MIN(p12, p22) - MAX(p11, p21));
        
    } // getSharedLength

    int Floorplan::getUnitIndexFromName(FloorplanHolder* floorplan_holder, char* name)
    {
        assert(floorplan_holder);

        int i;
    
        for (i = 0; i < floorplan_holder->_n_units; i++) 
        {
            if (!strcmp(name, floorplan_holder->_flp_units[i]._name)) 
                return i;
        }
    
        LibUtil::Log::printFatalLine(std::cerr, "ERROR: block " + (String) name + " not found\n");
        return -1;
    } // getUnitIndexFromName

    void Floorplan::setFloorplanUnitNamesInData()
    {
        assert(_floorplan_holder);
        
        map<char*, double>& temperature = Data::getSingleton()->getTemperature();
        map<char*, double>& power       = Data::getSingleton()->getPower();

        for (int i=0; i<_floorplan_holder->_n_units; ++i)
        {
            temperature[ _floorplan_holder->_flp_units[i]._name ] = 0;
            power[ _floorplan_holder->_flp_units[i]._name ] = 0;
        }
    }

    void Floorplan::readFloorplan(std::string flp_file)
    {
        
        FILE    *fp;
        int     n_units = 0;
    
        fp = fopen (flp_file.c_str(), "r");

        if (!fp) 
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: cannot open floorplan file.\n");
    
        // 1st pass - find n_units
        n_units = countFloorplanUnits(fp);
        if(n_units<=0)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: no units in floorplan file.\n");
        // allocate floorplan holder
        _floorplan_holder = new FloorplanHolder(n_units);
    
        // 2nd pass - populate units info
        populateFloorplanUnits(fp);

        fclose(fp); 

        // make sure the origin is (0,0)
        offsetFloorplanCoordinate(0, 0);   

        // calculate total chip dimension
        calculateChipTotalWidth();
        calculateChipTotalHeight();

    } // readFloorplan

} // namespace Thermal

