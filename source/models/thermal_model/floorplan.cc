
#include <cassert>
#include <stddef.h>
#include <string.h>
#include <map>

#include "source/models/thermal_model/floorplan.h"
#include "source/data/data.h"
#include "source/misc/misc.h"
#include "libutil/LibUtil.h"

using LibUtil::String;
using std::map;
using std::vector;
using std::string;

namespace Thermal
{
    Floorplan::Floorplan()
        : _floorplan_holder     (NULL)
        , _top_flp_object_found (false)
    {
        _floorplan_objects.clear();
    }

    Floorplan::~Floorplan()
    {
        if(_floorplan_holder)
            delete _floorplan_holder;
    }

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

    bool Floorplan::isVertAdj(const FloorplanHolder* floorplan_holder, int i, int j)
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

    bool Floorplan::isHorizAdj(const FloorplanHolder* floorplan_holder, int i, int j)
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

    double Floorplan::getSharedLength(const FloorplanHolder* floorplan_holder, int i, int j)
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

    int Floorplan::getUnitIndexFromName(const FloorplanHolder* floorplan_holder, const char* name)
    {
        assert(floorplan_holder);

        int i;
    
        for (i = 0; i < floorplan_holder->_n_units; i++) 
        {
            if (floorplan_holder->_flp_units[i]._name == (string) name ) 
                return i;
        }
    
        LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Block " + (String) name + " not found.\n");
        return -1;
    } // getUnitIndexFromName

    void Floorplan::setFloorplanUnitNamesInTemperatureData()
    {
        assert(_floorplan_holder);
        assert(Data::getSingleton()->getTemperatureDataSize() == 0);

        for (int i=0; i<_floorplan_holder->_n_units; ++i)
            Data::getSingleton()->addTemperatureData(_floorplan_holder->_flp_units[i]._name, 0);
    }

    void Floorplan::parseFloorplanFile(string flp_file, string top_flp_object_name)
    {
        int     i = 0;
        char    line[LINE_SIZE];
        string  line_copy;
        char*   line_token;

        string  new_file_path;

        char    flp_obj_name[STR_SIZE];
        char    unit_name[STR_SIZE];
        double  leftx, bottomy, width, height;
                            
        FloorplanUnit floorplan_unit;

        FILE* fp = fopen (flp_file.c_str(), "r");
        if(!fp)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Cannot open floorplan file: " + flp_file + ".\n");

        fseek(fp, 0, SEEK_SET);
        while(!feof(fp))
        {
            // if top level floorplan object is already found then escape
            if(_top_flp_object_found)
                break;
            
            // read a line from the file
            fgets(line, LINE_SIZE, fp);
            if (feof(fp))
                break;

            line_copy = (string) line;
            line_token = strtok(line, " \r\t\n");
            
            // ignore comments and empty lines
            if      (!line_token || line_token[0] == '#')
                continue;
            // read included floorplan file when keyword "include" is read
            else if (!strcmp(line_token, "include"))
            {
                line_token = strtok(NULL, " \r\t\n");
                if(!line_token)
                    LibUtil::Log::printFatalLine(std::cerr, "\nERROR: No file specified for include.\n");

                new_file_path = flp_file.substr(0, (flp_file.find_last_of("/\\")+1) ) + ((string) line_token);

                parseFloorplanFile( new_file_path, top_flp_object_name );
                Misc::isEndOfLine(0);
            }
            // read a sub-floorplan object when keyword "floorplan" is read
            else if (!strcmp(line_token, "floorplan"))
            {
                // extract the object name
                line_token = strtok(NULL, " \r\t\n");
                strcpy(flp_obj_name, line_token);
                // check floorplan object name validity
                if(!flp_obj_name)
                    LibUtil::Log::printFatalLine(std::cerr, "\nERROR: No floorplan object name.\n");
                // check if there are duplicated floorplan objs names
                if(_floorplan_objects.count( (string) flp_obj_name ))
                    LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Duplicated floorplan object names: " + (string)flp_obj_name + ".\n");
                Misc::isEndOfLine(0);

                // read floorplan blocks in this object
                while(!feof(fp))
                {
                    fgets(line, LINE_SIZE, fp);
                    if (feof(fp))
                        LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Floorplan object definition not completed.\n");
                    
                    line_copy = (string) line;
                    line_token = strtok(line, " \r\t\n");
                    
                    // ignore comments and empty lines
                    if      (!line_token || line_token[0] == '#')
                        continue;
                    // exit floorplan object definition
                    else if (!strcmp(line_token, "endfloorplan"))
                    {
                        Misc::isEndOfLine(0);
                        break;
                    }
                    // atomic blocks
                    else if (!strcmp(line_token, "atomic"))
                    {
                        if (sscanf(line_copy.c_str(), "%*s%s%lf%lf%lf%lf", unit_name, &width, &height, &leftx, &bottomy) == 5) 
                        {
                            floorplan_unit._name    = (string) unit_name;
                            floorplan_unit._width   = width;
                            floorplan_unit._height  = height;
                            floorplan_unit._leftx   = leftx;
                            floorplan_unit._bottomy = bottomy;
                            floorplan_unit._filler  = false;
                        }
                        else
                            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Wrong atomic floorplan block format.\n");
                        
                        // check if the instance name is duplicated
                        if(_floorplan_objects[(string) flp_obj_name].count(floorplan_unit))
                            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Duplicated floorplan instance names: " + floorplan_unit._name + ".\n");

                        // push atomic block into current object
                        _floorplan_objects[(string) flp_obj_name].insert(floorplan_unit);
                        Misc::isEndOfLine(5);
                    }
                    // filler blocks
                    else if (!strcmp(line_token, "filler"))
                    {
                        if (sscanf(line_copy.c_str(), "%*s%s%lf%lf%lf%lf", unit_name, &width, &height, &leftx, &bottomy) == 5) 
                        {
                            floorplan_unit._name    = (string) unit_name;
                            floorplan_unit._width   = width;
                            floorplan_unit._height  = height;
                            floorplan_unit._leftx   = leftx;
                            floorplan_unit._bottomy = bottomy;
                            floorplan_unit._filler  = true;
                        }
                        else
                            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Wrong filler floorplan block format.\n");
                        
                        // check if the instance name is duplicated
                        if(_floorplan_objects[(string) flp_obj_name].count(floorplan_unit))
                            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Duplicated floorplan instance names: " + floorplan_unit._name + ".\n");

                        // push filler block into current object
                        _floorplan_objects[(string) flp_obj_name].insert(floorplan_unit);
                        Misc::isEndOfLine(5);
                    }
                    // instantiate floorplan object
                    else
                    {
                        // check if the instantiated object is in the floorplan object map
                        if(!_floorplan_objects.count( (string) line_token ))
                            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Cannot find floorplan object: " + (string)line_token + ".\n");
                        
                        if (sscanf(line_copy.c_str(), "%*s%s%lf%lf", unit_name, &leftx, &bottomy) != 3) 
                            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Wrong floorplan object instantiation format.\n");

                        // put every block in the instantiated object to the new object with new name and new coordinates
                        for(set<FloorplanUnit>::iterator it = _floorplan_objects[(string) line_token].begin(); it != _floorplan_objects[(string) line_token].end(); ++it)
                        {
                            floorplan_unit._name    = ((string) unit_name) + "." + (*it)._name;
                            floorplan_unit._width   = (*it)._width;
                            floorplan_unit._height  = (*it)._height;
                            floorplan_unit._leftx   = (*it)._leftx + leftx;
                            floorplan_unit._bottomy = (*it)._bottomy + bottomy;
                            floorplan_unit._filler  = (*it)._filler;
                            
                            // push hierarchy block into current object
                            _floorplan_objects[(string) flp_obj_name].insert(floorplan_unit);
                        }
                        Misc::isEndOfLine(3);
                    }
                } // while loop in floorplan module block
                
                // if this floorplan object is the top level object,
                // stop reading further objects and copy this object
                // back to the floorplan holder
                if( ((string) flp_obj_name) == top_flp_object_name )
                {
                    _floorplan_holder = new FloorplanHolder( _floorplan_objects[top_flp_object_name].size() );
                    
                    i = 0;
                    for(set<FloorplanUnit>::iterator it = _floorplan_objects[top_flp_object_name].begin(); it != _floorplan_objects[top_flp_object_name].end(); ++it)
                    {
                        _floorplan_holder->_flp_units[i] = (*it);
                        ++i;
                    }
                    
                    _top_flp_object_found = true;

                    break;
                }
            }
            // unrecognized syntax
            else
                LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Wrong syntax: " + line_copy + 
                                                        "\n       include another floorplan file or define a floorplan object!\n");
        } // while loop

        // close floorplan file
        fclose(fp);
    } // parseFloorplanFile

    void Floorplan::loadFloorplan(string flp_file, string top_flp_object_name)
    {
        parseFloorplanFile(flp_file, top_flp_object_name);
        if(!_top_flp_object_found)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Top level floorplan object not found!\n");
        
        // print unit name out for convenience
        printf("\n\n");
        for(int i=0; i<_floorplan_holder->_n_units; ++i)
            printf("%s\n", _floorplan_holder->_flp_units[i]._name.c_str());
        printf("\n\n");

        // make sure the origin is (0,0)
        offsetFloorplanCoordinate(0, 0);   
        // calculate total chip dimension
        calculateChipTotalWidth();
        calculateChipTotalHeight();

    } // readFloorplan

} // namespace Thermal

