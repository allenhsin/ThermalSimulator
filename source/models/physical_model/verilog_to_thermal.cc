
#include <iostream>

#include "source/models/physical_model/verilog_to_thermal.h"
#include "source/models/physical_model/device_models/device_model.h"
#include "libutil/LibUtil.h"

namespace Thermal
{
    using std::string;
    using std::map;
    using namespace VerilogParser;

    const string VerilogToThermal::HIER_SEPARATOR = ".";
    const std::map<std::string, DeviceType> VerilogToThermal::NAME_MAP = initNameMap();


    VerilogToThermal::Net::Net(const ElabNet* net, const DeviceModel* inst)
        : _net(net), _inst(inst)
    {}
    
    VerilogToThermal::Net::~Net()
    {}
    
    VerilogToThermal::VerilogToThermal(
            DeviceManager* device_manager,
            config::Config* physical_config,    
            DeviceFloorplanMap* device_floorplan_map
        )
        : _device_manager(device_manager),
        _physical_config(physical_config),
        _device_floorplan_map(device_floorplan_map),
        _reader(),
        _equiv_nets(),
        _leaf_nets(),
        _int_nets() 
    {}

    VerilogToThermal::~VerilogToThermal()
    {}
    
    void VerilogToThermal::dumpDevicesFromVerilog(
            DeviceManager* device_manager, 
            config::Config* physical_config,
            DeviceFloorplanMap* device_floorplan_map, 
            const vector<string>& verilog_files,
            const string& top_module
        )
    {
    
        VerilogToThermal v_to_t(device_manager, physical_config, device_floorplan_map);    
        v_to_t.readVerilog(verilog_files, top_module);
        v_to_t.dumpDevices();
    }

    // Finalize connections between device instances
    // void VerilogToThermal::finalize(map<string, Net*>* net_map, map<string, DeviceModel*>* device_map)
    // {
    
    // }
    
    void VerilogToThermal::readVerilog(const vector<string>& verilog_files, const string& top_module)
    {
        // Read all verilog files
        for (vector<string>::const_iterator it = verilog_files.begin(); it != verilog_files.end(); ++it)
            _reader.parse(*it);            
        // Elaborate the verilog files
        _reader.elaborate(top_module);        
    }
    
    void VerilogToThermal::dumpDevices()
    {   
        const ElabModule* top_module = _reader.getVerilogScope()->getTopModule();
        
        // _reader.dumpRawModules(cout);
        // _reader.dumpElabModules(cout);

        dumpDevicesFromModule(top_module, NULL);
        
        // Delete and clear everything
        for(NetsMap::const_iterator it = _equiv_nets.begin(); it != _equiv_nets.end(); ++it)
            LibUtil::clearPtrVector<Net>(it->second);
        _leaf_nets.clear();
        _int_nets.clear();
    }
    
    void VerilogToThermal::dumpDevicesFromModule(const ElabModule* module, const DeviceModel* device)
    {
        const ElabItems& items = module->getItems();
        for (ElabItems::const_iterator it = items.begin(); it != items.end(); ++it)
        {
            const ElabItem* item = (*it);
            // Do several different things depending on what the item is
            switch(item->getType())
            {
                case ELAB_ITEM_PARAM: 
                {
                    break;
                }
                case ELAB_ITEM_NET:
                {
                    dumpItemNet(device, (ElabNet*) item);
                    break;
                }
                case ELAB_ITEM_INST:
                {
                    dumpItemInst((ElabInstance*) item);
                    break;
                }
                case ELAB_ITEM_ASSIGN:
                {
                    dumpItemAssign(module, device, (ElabAssign*) item);
                    break;
                }                
            }
        }
    }
    
    void VerilogToThermal::dumpItemNet(const DeviceModel* device, const ElabNet* net)
    {
        // Check to see if this is a non-leaf net
        // Create a new vector of nets representing equivalent nets
        if (device == NULL)
            _int_nets[net] = new Nets(1, new Net(net, device));
        else
            _leaf_nets[net] = new Nets(1, new Net(net, device));
        
        if (device != NULL)
            std::cout << net->toString() << std::endl;            
    }
    void VerilogToThermal::dumpItemInst(const ElabInstance* inst)
    {
        // If it is a primitive, create the primitive
        if(isPrimitive(inst->getModule()))
        {
            DeviceModel* device = getPrimitive(inst);            
            // TODO: Add to device manager
            // Dump nets from the module it instantiates
            dumpDevicesFromModule(inst->getModule(), device);            
        }
        else
            dumpDevicesFromModule(inst->getModule(), NULL);
        
    }
    void VerilogToThermal::dumpItemAssign(const ElabModule* module, const DeviceModel* device, const ElabAssign* assign)
    {
    
    }
    
    
    bool VerilogToThermal::isPrimitive(const ElabModule* module)
    {
        return (NAME_MAP.count(module->getName()) != 0);
    }
    
    DeviceType VerilogToThermal::getPrimitiveType(const ElabModule* module)
    {
        if (!isPrimitive(module))
            LibUtil::Log::printFatalLine(std::cerr, "ERROR: Not a primitive module: " + 
                module->getName());
       return NAME_MAP.at(module->getName());
    }
            
    DeviceModel* VerilogToThermal::getPrimitive(const ElabInstance* inst)
    {
        // Create the device
        DeviceModel* out = DeviceModel::createDevice(
                getPrimitiveType(inst->getModule()),
                _physical_config, 
                _device_floorplan_map);
                
        // Set its instance name
        out->setDeviceName(inst->getPath());
        // Get the device's overwritten parameters
        const SymbolMap& params = inst->getModule()->getParams();
        // Set the parameters
        for (SymbolMap::const_iterator it = params.begin(); it != params.end(); ++it)
        {
            out->setTargetParameterName(it->first);
            out->setTargetParameterValue(atof(it->second->getConstStr().c_str()));
        }        
        return out;
    }
            
    map<string, DeviceType> VerilogToThermal::initNameMap()
    {
        map<string, DeviceType> out;
        out["LaserSourceOffChip"]   = LASER_SOURCE_OFF_CHIP;
        out["LossyOpticalNet"]      = LOSSY_OPTICAL_NET;
        out["ResonantRing"]         = RESONANT_RING;
        return out;
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

