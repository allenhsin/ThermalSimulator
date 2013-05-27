
#ifndef __THERMAL_VERILOG_TO_THERMAL_H__
#define __THERMAL_VERILOG_TO_THERMAL_H__

#include <vector>
#include <string>

#include "source/models/physical_model/device_floorplan_map.h"
#include "source/misc/common_types.h"
#include "source/parser/verilog/VerilogFileReader.h"
#include "config.hpp"

namespace Thermal
{
    class DeviceManager;
    class DeviceModel;
    
    // Class that translates the post-elaboration netlist of VerilogFileReader
    // to a list of device instances with the correct port connections
    class VerilogToThermal
    {
        // Hierarchy separator
        static const string HIER_SEPARATOR; 
    
        protected:
            // Data-structure containing the information that describes an instance
            // connection
            class Connection
            {
                public:
                    Connection(const std::string& instance_name, const std::string& port_name);
                    ~Connection();
                    
                public:
                    const std::string _instance_name;
                    const std::string _port_name;
            };
    
            // Data-structure containing the information that describes what a net is connected to
            class Net
            {
                public:
                    Net(const std::string& name);
                    ~Net();
                
                    void setDriver(Connection* driver);                    
                    inline void addSink(Connection* sink) { _sinks->push_back(sink); }
                    inline const Connection* getDriver() { return _driver; }
                    inline const vector<Connection*>* getSinks() { return _sinks; }
                
                private:
                    const std::string _name;
                    Connection* _driver;
                    vector<Connection*>* _sinks;                        
            };
    
        public:
            VerilogToThermal();
            ~VerilogToThermal();

        public:
            // Call this function to dump the verilog instances and connectivity in the given
            // verilog files to the provided device manager
            static void dumpDeviceFromVerilog(DeviceManager* device_manager, config::Config* physical_config,
                DeviceFloorplanMap* device_floorplan_map, const vector<string>& verilog_files);
        
            // Parse verilog nets, returning a data structure of nets
            static std::map<std::string, Net*>* parseNets(const VerilogParser::VerilogFileReader* reader);
            
            // Parse verilog instances, returning a data structure of device instances            
            static std::map<std::string, DeviceModel*>* parseInstances(const VerilogParser::VerilogFileReader* reader, 
                config::Config* physical_config, DeviceFloorplanMap* device_floorplan_map);

            // Finalize connections between device instances
            static void finalize(std::map<std::string, Net*>* net_map, 
                std::map<std::string, DeviceModel*>* device_map);
            
    }; // class VerilogToThermal

} // namespace Thermal

#endif // __THERMAL_VERILOG_TO_THERMAL_H__

