
#ifndef __THERMAL_VERILOG_TO_THERMAL_H__
#define __THERMAL_VERILOG_TO_THERMAL_H__

#include <vector>
#include <string>

#include "source/models/physical_model/device_floorplan_map.h"
#include "source/models/physical_model/device_models/device_type.h"
#include "source/misc/common_types.h"
#include "config.hpp"
#include "source/parser/verilog/VerilogParser.h"

namespace Thermal
{
    class DeviceManager;
    class DeviceModel;
    
    // Class that translates the post-elaboration netlist of VerilogFileReader
    // to a list of device instances with the correct port connections
    class VerilogToThermal
    {
        public:
            // Hierarchy separator
            static const string HIER_SEPARATOR; 
            // Mapping of verilog primitive module names to DeviceType
            static const std::map<std::string, DeviceType> NAME_MAP;
    
        private:
            // Data-structure containing the information that describes what a net is connected to
            class Net
            {
                public:
                    Net(const VerilogParser::ElabNet* net, const DeviceModel* inst);
                    ~Net();
                    
                    inline bool isLeaf() const { return _inst != NULL; }
                    inline const VerilogParser::ElabNet* getNet() const { return _net; }
                    inline const DeviceModel* getInst() const { return _inst; }
                    
                private:
                    const VerilogParser::ElabNet* _net;
                    const DeviceModel* _inst;                    
            };
    
            typedef std::vector<Net*> Nets;
            typedef std::map<const VerilogParser::ElabNet*, Nets*> NetsMap;
        
        public:
            ~VerilogToThermal();
        
        // Private constructor, use the public static method to dump devices and connectivity
        // from a verilog file
        private:
            VerilogToThermal(
                    DeviceManager* device_manager,
                    config::Config* physical_config,    
                    DeviceFloorplanMap* device_floorplan_map
                );

        public:
            // Call this function to dump the verilog instances and connectivity in the given
            // verilog files to the provided device manager
            static void dumpDevicesFromVerilog(
                    DeviceManager* device_manager,
                    config::Config* physical_config,
                    DeviceFloorplanMap* device_floorplan_map,
                    const std::vector<string>& verilog_files,
                    const std::string& top_module
                );                
                
        // Helpers
        private:        
            // Reads verilog files and elaborates
            void readVerilog(const std::vector<string>& verilog_files, 
                const std::string& top_module);

            // Creates all DeviceModel* instances, maps all
            // equivalent nets and connects all DeviceModel* instances correctly
            void dumpDevices();            
            // Module-level helper for dumpDevices
            void dumpDevicesFromModule(const VerilogParser::ElabModule* module, const DeviceModel* device);
            
            // Case-specific helpers for dump devices
            void dumpItemNet(const DeviceModel* device, const VerilogParser::ElabNet* net);
            void dumpItemInst(const VerilogParser::ElabInstance* inst);
            void dumpItemAssign(const VerilogParser::ElabModule* module, const DeviceModel* device,
                const VerilogParser::ElabAssign* assign);
        
            // Dumps the verilog and connectivity from an elaborated verilog module
            // static void VerilogToThermal::dumpDeviceFromModule(
                    // DeviceManager* device_manager, 
                    // config::Config* physical_config,
                    // DeviceFloorplanMap* device_floorplan_map, 
                    // const ElabModule* module
            // )
        
            // Identifies whether an elaborated module is a primitive
            bool isPrimitive(const VerilogParser::ElabModule* module);
            // Get a primitive instance from an elaborated instance
            DeviceType getPrimitiveType(const VerilogParser::ElabModule* module);            
            // Get a primitive type from an elaborated module
            DeviceModel* getPrimitive(const VerilogParser::ElabInstance* inst);            
        
            // Initializes the name map
            static std::map<std::string, DeviceType> initNameMap();
            
                
        private:        
            // The device manager to dump devics to
            DeviceManager* _device_manager;
            // Pointers needed to create primitives
            config::Config* _physical_config;
            DeviceFloorplanMap* _device_floorplan_map;
            // Verilog file reader
            VerilogParser::VerilogFileReader _reader;            
            // Maps used to help out with getting rid of nets
            NetsMap _equiv_nets;
            NetsMap _leaf_nets;
            NetsMap _int_nets;  
        
                
    }; // class VerilogToThermal

} // namespace Thermal

#endif // __THERMAL_VERILOG_TO_THERMAL_H__

