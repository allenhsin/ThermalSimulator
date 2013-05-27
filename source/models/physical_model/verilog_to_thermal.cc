
#include <stddef.h>
#include <cassert>
#include <queue>

#include "source/models/physical_model/verilog_to_thermal.h"
#include "source/parser/verilog/VerilogParser.h"
#include "libutil/LibUtil.h"

using namespace std;
using namespace LibUtil;
using namespace VerilogParser;

namespace Thermal
{
    const string VerilogToThermal::HIER_SEPARATOR = ".";

    VerilogToThermal::Connection::Connection(const string& instance_name, const string& port_name)
        : _instance_name(instance_name), _port_name(port_name)
    {}
    
    VerilogToThermal::Connection::~Connection()
    {}

    VerilogToThermal::Net::Net(const string& name)
        : _name(name)
    {        
        _sinks = new vector<Connection*>();
        _driver = NULL;
    }
    
    VerilogToThermal::Net::~Net()
    {
        deletePtrVector<Connection>(_sinks);
        delete _driver;        
        _driver = NULL;
        _sinks = NULL;
    }
    
    void VerilogToThermal::Net::setDriver(Connection* driver)
    {
        if (_driver != NULL)
            LibUtil::Log::printFatalLine(std::cerr, "\nERROR: Multiple drivers on same verilog net: " + _name + ".\n");
        _driver = driver;
    }
    
    VerilogToThermal::VerilogToThermal()
    {}

    VerilogToThermal::~VerilogToThermal()
    {}
    
    // Call this function to dump the verilog instances and connectivity in the given
    // verilog files to the provided device manager
    void VerilogToThermal::dumpDeviceFromVerilog(
            DeviceManager* device_manager, 
            config::Config* physical_config,
            DeviceFloorplanMap* device_floorplan_map, 
            const vector<string>& verilog_files
        )
    {
        VerilogFileReader* reader = new VerilogFileReader();
        vector<string>::const_iterator it;
        for (it = verilog_files.begin(); it != verilog_files.end(); ++it)
        {
            VerilogFile* file = new VerilogFile(*it);
            reader->parse(file);
            delete file;
        }
        
        reader->elaborate();        
        const VerilogItems* items = reader->getFlattenedItems("AND_X1M_A12TR");
        const VerilogEquivNetMap equiv_map* = reader->mapEquivNets(items);
        
        // map<string, Net*>* net_map = parseNets(reader);
        // map<string, DeviceModel*>* device_map = parseInstances(reader, physical_config, device_floorplan_map);
        // finalize(net_map, device_map);
        
        deletePtrVector<items>;
        delete equiv_map;
        delete reader;
    }

    // Parse verilog nets, returning a data structure of nets
    map<string, VerilogToThermal::Net*>* VerilogToThermal::parseNets(const VerilogFileReader* reader)
    {
        const VerilogModules* modules = reader->getModules();
        // Iterate through all modules
        for(VerilogModules::const_iterator m_it = modules->begin(); m_it != modules->end(); ++m_it)
        {
            const VerilogModule* module = *m_it;
            const VerilogItemMap* items = module->getItemMap();
            cout << "    " << module->getName() << endl;            
            for(VerilogItemMap::const_iterator i_it = items->begin(); i_it != items->end(); ++i_it)
                cout << "        " << (*i_it).second->toString() << endl;
        }            
        return NULL;
    }
    
    // Parse verilog instances, returning a data structure of device instances            
    map<string, DeviceModel*>* VerilogToThermal::parseInstances(const VerilogFileReader* reader, 
        config::Config* physical_config, DeviceFloorplanMap* device_floorplan_map)
    {
        const VerilogModules* modules = reader->getModules();
        for(VerilogModules::const_iterator m_it = modules->begin(); m_it != modules->end(); ++m_it)
        {
            const VerilogModule* module = *m_it;
            const VerilogItemMap* items = module->getItemMap();
            cout << "    " << module->getName() << endl;            
            for(VerilogItemMap::const_iterator i_it = items->begin(); i_it != items->end(); ++i_it)
                cout << "        " << (*i_it).second->toString() << endl;        
        }
        return NULL;
    }

    // Finalize connections between device instances
    void VerilogToThermal::finalize(map<string, Net*>* net_map, map<string, DeviceModel*>* device_map)
    {
    
    }
            
            
    // bool VerilogToThermal::startupManager()
    // {
        // assert(_config);

    // // check if manager is enabled --------------------------------------------
        // if(!_config->getBool("device_manager/enable"))
        // {
            // LibUtil::Log::printLine( "    Device Manager not enabled" );
            // return false;
        // }
    // // ------------------------------------------------------------------------

    // // set device manager constants -------------------------------------------
        // // sampling interval
        // _sub_bit_sampling_intvl = _config->getFloat("device_manager/sub_bit_sampling_intvl");
    // // ------------------------------------------------------------------------

    // // Load Files and Initialize Devices --------------------------------------
        // // load floorplan map file
        // _device_floorplan_map->loadFloorplanMap( _config->getString("device_manager/flpmap_file") );

        // // load device netlist
        // //FIXME: hardcode device now just for test ------------------------------------------------

        // // waveguide x 9, laser, modulator, modulator driver, receiver ring.
        // _device_instances.push_back( DeviceModel::createDevice( LOSSY_OPTICAL_NET, _config, _device_floorplan_map) );
        // _device_instances.push_back( DeviceModel::createDevice( LOSSY_OPTICAL_NET, _config, _device_floorplan_map) );
        // _device_instances.push_back( DeviceModel::createDevice( LOSSY_OPTICAL_NET, _config, _device_floorplan_map) );
        // _device_instances.push_back( DeviceModel::createDevice( LOSSY_OPTICAL_NET, _config, _device_floorplan_map) );
        // _device_instances.push_back( DeviceModel::createDevice( LOSSY_OPTICAL_NET, _config, _device_floorplan_map) );
        // _device_instances.push_back( DeviceModel::createDevice( LOSSY_OPTICAL_NET, _config, _device_floorplan_map) );
        // _device_instances.push_back( DeviceModel::createDevice( LOSSY_OPTICAL_NET, _config, _device_floorplan_map) );
        // _device_instances.push_back( DeviceModel::createDevice( LOSSY_OPTICAL_NET, _config, _device_floorplan_map) );
        // _device_instances.push_back( DeviceModel::createDevice( LOSSY_OPTICAL_NET, _config, _device_floorplan_map) );
        // _device_instances.push_back( DeviceModel::createDevice( RESONANT_RING, _config, _device_floorplan_map) );
        // _device_instances.push_back( DeviceModel::createDevice( RESONANT_RING_DEPLETION_MODULATOR, _config, _device_floorplan_map) );
        // _device_instances.push_back( DeviceModel::createDevice( LASER_SOURCE_OFF_CHIP, _config, _device_floorplan_map) );
        // _device_instances.push_back( DeviceModel::createDevice( MODULATOR_DRIVER, _config, _device_floorplan_map) );
        
        // _device_instances[0]->setDeviceName("waveguide_0");
        // _device_instances[1]->setDeviceName("waveguide_1");
        // _device_instances[2]->setDeviceName("waveguide_2");
        // _device_instances[3]->setDeviceName("waveguide_3");
        // _device_instances[4]->setDeviceName("waveguide_4");
        // _device_instances[5]->setDeviceName("waveguide_5");
        // _device_instances[6]->setDeviceName("waveguide_6");
        // _device_instances[7]->setDeviceName("waveguide_7");
        // _device_instances[8]->setDeviceName("waveguide_8");
        // _device_instances[9]->setDeviceName("receiver_ring");
        // _device_instances[10]->setDeviceName("modulator");
        // _device_instances[11]->setDeviceName("laser_source");
        // _device_instances[12]->setDeviceName("modulator_driver");
        
        // // laser -> waveguide_0
        // _device_instances[11]->setTargetPortName("out");
        // _device_instances[11]->setTargetPortConnectedPort( _device_instances[0]->getPort("in") );
        // _device_instances[0]->setTargetPortName("in");
        // _device_instances[0]->setTargetPortConnectedPort( _device_instances[11]->getPort("out") );

        // // modulator driver -> modulator
        // _device_instances[12]->setTargetPortName("out");
        // _device_instances[12]->setTargetPortConnectedPort( _device_instances[10]->getPort("mod_driver") );
        // _device_instances[10]->setTargetPortName("mod_driver");
        // _device_instances[10]->setTargetPortConnectedPort( _device_instances[12]->getPort("out") );

        // // waveguide_0 -> waveguide_1
        // _device_instances[0]->setTargetPortName("out");
        // _device_instances[0]->setTargetPortConnectedPort( _device_instances[1]->getPort("in") );
        // _device_instances[1]->setTargetPortName("in");
        // _device_instances[1]->setTargetPortConnectedPort( _device_instances[0]->getPort("out") );
        
        // // waveguide_1 -> modulator
        // _device_instances[1]->setTargetPortName("out");
        // _device_instances[1]->setTargetPortConnectedPort( _device_instances[10]->getPort("in") );
        // _device_instances[10]->setTargetPortName("in");
        // _device_instances[10]->setTargetPortConnectedPort( _device_instances[1]->getPort("out") );
        
        // // modulator -> waveguide_2
        // _device_instances[10]->setTargetPortName("thru");
        // _device_instances[10]->setTargetPortConnectedPort( _device_instances[2]->getPort("in") );
        // _device_instances[2]->setTargetPortName("in");
        // _device_instances[2]->setTargetPortConnectedPort( _device_instances[10]->getPort("thru") );
        
        // // waveguide_2 -> waveguide_3
        // _device_instances[2]->setTargetPortName("out");
        // _device_instances[2]->setTargetPortConnectedPort( _device_instances[3]->getPort("in") );
        // _device_instances[3]->setTargetPortName("in");
        // _device_instances[3]->setTargetPortConnectedPort( _device_instances[2]->getPort("out") );

        // // waveguide_3 -> waveguide_4
        // _device_instances[3]->setTargetPortName("out");
        // _device_instances[3]->setTargetPortConnectedPort( _device_instances[4]->getPort("in") );
        // _device_instances[4]->setTargetPortName("in");
        // _device_instances[4]->setTargetPortConnectedPort( _device_instances[3]->getPort("out") );
        
        // // waveguide_4 -> waveguide_5
        // _device_instances[4]->setTargetPortName("out");
        // _device_instances[4]->setTargetPortConnectedPort( _device_instances[5]->getPort("in") );
        // _device_instances[5]->setTargetPortName("in");
        // _device_instances[5]->setTargetPortConnectedPort( _device_instances[4]->getPort("out") );
        
        // // waveguide_5 -> waveguide_6
        // _device_instances[5]->setTargetPortName("out");
        // _device_instances[5]->setTargetPortConnectedPort( _device_instances[6]->getPort("in") );
        // _device_instances[6]->setTargetPortName("in");
        // _device_instances[6]->setTargetPortConnectedPort( _device_instances[5]->getPort("out") );
        
        // // waveguide_6 -> receiver ring
        // _device_instances[6]->setTargetPortName("out");
        // _device_instances[6]->setTargetPortConnectedPort( _device_instances[9]->getPort("in") );
        // _device_instances[9]->setTargetPortName("in");
        // _device_instances[9]->setTargetPortConnectedPort( _device_instances[6]->getPort("out") );
        
        // // receiver_ring -> waveguide_7
        // _device_instances[9]->setTargetPortName("thru");
        // _device_instances[9]->setTargetPortConnectedPort( _device_instances[7]->getPort("in") );
        // _device_instances[7]->setTargetPortName("in");
        // _device_instances[7]->setTargetPortConnectedPort( _device_instances[9]->getPort("thru") );

        // // waveguide_7 -> waveguide_8
        // _device_instances[7]->setTargetPortName("out");
        // _device_instances[7]->setTargetPortConnectedPort( _device_instances[8]->getPort("in") );
        // _device_instances[8]->setTargetPortName("in");
        // _device_instances[8]->setTargetPortConnectedPort( _device_instances[7]->getPort("out") );
        
        // //END FIXME -------------------------------------------------------------------------------
        
    // } // startup

} // namespace Thermal

