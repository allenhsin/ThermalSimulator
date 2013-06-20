
#ifndef __THERMAL_PHYSICAL_MODEL_H__
#define __THERMAL_PHYSICAL_MODEL_H__

#include <string>
#include <map>

#include "source/models/model.h"
#include "source/misc/common_types.h"

namespace Thermal
{

    class PhysicalModel : public Model
    {
    public:
        virtual ~PhysicalModel();

        void startup();
        void execute(Time scheduled_time);
        
        virtual bool startupManager() = 0;
        virtual bool executeManager(Time scheduled_time) = 0;
        virtual std::string getModelName() { return "Physical Model"; }

    protected:
        PhysicalModel();
        void loadConfig(); 
        
        // update the accumulated energy in data by taking
        // the power from the previous time and the elapsed
        // time since last update
        void addEnergyData();
        void setEnergyData(Time time_elapsed_since_last_update);
        
        bool hasFloorplanUnit(std::string floorplan_unit_name);
        void addFloorplanUnit(std::string floorplan_unit_name);
        unsigned int getFloorplanUnitSize(){ return _floorplan_unit_power.size(); }
        void setFloorplanUnitPower(std::string floorplan_unit_name, double floorplan_unit_power);

    private:
        // store the power for floorplan units at current time
        // the units must be mapped to the floorplan
        std::map<std::string, double> _floorplan_unit_power;

        Time _last_energy_update_time;
        

    }; // class PhysicalModel

} // namespace Thermal

#endif // __THERMAL_PHYSICAL_MODEL_H__

