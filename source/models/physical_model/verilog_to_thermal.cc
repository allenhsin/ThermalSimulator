
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

    map<string, DeviceType> VerilogToThermal::initNameMap()
    {
        map<string, DeviceType> out;
        out["LaserSourceOffChip"]               = LASER_SOURCE_OFF_CHIP;
        out["LossyOpticalNet"]                  = LOSSY_OPTICAL_NET;
        out["ResonantRing"]                     = RESONANT_RING;
        out["ResonantRingDepletionModulator"]   = RESONANT_RING_DEPLETION_MODULATOR;
        out["ModulatorDriver"]                  = MODULATOR_DRIVER;
        out["ThermalTuner"]                     = THERMAL_TUNER;
        return out;
    }

    VerilogToThermal::Net::Net(const ElabNet* net, DeviceModel* inst)
        : _net(net), _inst(inst)
    {}
    
    VerilogToThermal::Net::~Net()
    {}
    
    VerilogToThermal::VerilogToThermal(
            vector<DeviceModel*>& devices,
            config::Config* physical_config,    
            DeviceFloorplanMap* device_floorplan_map
        )
        : _devices(devices),
        _physical_config(physical_config),
        _device_floorplan_map(device_floorplan_map),
        _reader(),
        _leaf_nets(),
        _int_nets() 
    {}

    VerilogToThermal::~VerilogToThermal()
    {}
    
    void VerilogToThermal::dumpDevicesFromVerilog(
            vector<DeviceModel*>& devices, 
            config::Config* physical_config,
            DeviceFloorplanMap* device_floorplan_map, 
            const vector<string>& verilog_files,
            const string& top_module
        )
    {
    
        VerilogToThermal v_to_t(devices, physical_config, device_floorplan_map);    
        v_to_t.readVerilog(verilog_files, top_module);
        v_to_t.dumpDevices();
        v_to_t.connectDevices();
    }

    // Finalize connections between device instances
    // void VerilogToThermal::finalize(map<string, Net*>* net_map, map<string, DeviceModel*>* device_map)
    // {
    
    // }
    
    void VerilogToThermal::readVerilog(const vector<string>& verilog_files, const string& top_module)
    {
        // Read all verilog files
        for (vector<string>::const_iterator it = verilog_files.begin(); it != verilog_files.end(); ++it)
        {
            LibUtil::Log::printLine("Reading verilog file: " + *it);
            _reader.parse(*it);
        }
        // Elaborate the verilog files
        LibUtil::Log::printLine("Elaborating top module: " + top_module);
        _reader.elaborate(top_module);        
    }
    
    void VerilogToThermal::dumpDevices()
    {   
        const ElabModule* top_module = _reader.getVerilogScope()->getTopModule();
        dumpDevicesFromModule(top_module, NULL);        
    }
    
    void VerilogToThermal::connectDevices()
    {
        LibUtil::Log::printLine("Connecting devices...");
        // Iterate through the net map and connect together all the equivalent nets
        for(NetsMap::iterator it = _int_nets.begin(); it != _int_nets.end(); it = _int_nets.begin())
        {
            // Find the driver and sink nets
            Net* driver = NULL;
            Net* sink = NULL;            
            Nets* equiv_nets = it->second;
            for(Nets::const_iterator n_it = equiv_nets->begin(); n_it != equiv_nets->end(); ++n_it)
            {
                // Get the net
                Net* net = *n_it;
                // Check if the net is a leaf net
                if (net->isLeaf())
                {
                    // Set the net to be the driver if it is a output port
                    if (net->getNet()->getPortType() == PORT_OUTPUT)
                    {
                        if (driver != NULL)
                        {
                            LibUtil::Log::printFatalLine(std::cerr, "ERROR: Net has multiple drivers: (" +
                                driver->getNet()->toString() + ", " + net->getNet()->toString() + ")");
                        }
                        driver = net;
                    }
                    // Set the net to be the sink if it is an input port
                    else if (net->getNet()->getPortType() == PORT_INPUT)
                    {
                        if (sink != NULL)
                        {
                            LibUtil::Log::printFatalLine(std::cerr, "ERROR: Net has multiple sinks: (" +
                                sink->getNet()->toString() + ", " + net->getNet()->toString() + ")");
                        }
                        sink = net;
                    }                    
                }                
                // Delete the entry in the net map                
                 _int_nets.erase(net->getNet());
            }
            // If the net has a driver and a sink, make the connection!
            if (driver && sink)
            {
                LibUtil::Log::printLine("Connection (driver=" + driver->getNet()->toString() + ", "
                    "sink=" + sink->getNet()->toString()+")");
                    
                DeviceModel* driver_model = driver->getInst();
                DeviceModel* sink_model = sink->getInst();
                const string& driver_port = driver->getNet()->getIdentifier();
                const string& sink_port = sink->getNet()->getIdentifier();
                
                driver_model->setTargetPortName(driver_port);
                driver_model->setTargetPortConnectedPort(sink_model->getPort(sink_port));
                sink_model->setTargetPortName(sink_port);
                sink_model->setTargetPortConnectedPort(driver_model->getPort(driver_port));
            }
            // Delete the current set of equivalent nets
            LibUtil::deletePtrVector<Net>(equiv_nets);            
        }
    }
    
    void VerilogToThermal::dumpDevicesFromModule(const ElabModule* module, DeviceModel* device)
    {
        if (device == NULL) 
            LibUtil::Log::printLine("Reading non-primitive module: " + module->getName());
        
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
                    dumpItemAssign((ElabAssign*) item);
                    break;
                }                
            }
        }
    }
    
    void VerilogToThermal::dumpItemNet(DeviceModel* device, const ElabNet* net)
    {
        // Create a new Net data structure
        Net* net_ds = new Net(net, device);
        // Create a new vector of nets representing equivalent nets
        _int_nets[net] = new Nets(1, net_ds);
    }
    void VerilogToThermal::dumpItemInst(const ElabInstance* inst)
    {
        // If it is a primitive, create the primitive
        if(isPrimitive(inst->getModule()))
        {
            LibUtil::Log::printLine("Device (" + inst->toString() + ")");
            // Get the device and add it to the device model
            DeviceModel* device = getPrimitive(inst);            
            _devices.push_back(device);
            // Dump devices from the module it instantiates
            dumpDevicesFromModule(inst->getModule(), device);            
        }
        else dumpDevicesFromModule(inst->getModule(), NULL);
        
    }
    void VerilogToThermal::dumpItemAssign(const ElabAssign* assign)
    {
        const BitVector& lh = assign->getLeft();
        const BitVector& rh = assign->getRight();
        // Total bits in the assignment
        unsigned int width = std::min(lh.width(), rh.width());
        for (unsigned int i = 0; i < width; ++i)
        {
            int li = lh.low() + i;
            int ri = rh.low() + i;
            
            // Get the assignment as directly between an assignment
            // between two elaborated nets. The thermal simulator does not care
            // about assignments to/from constant numbers
            const ElabNet* lnet = NULL;
            const ElabNet* rnet = NULL;            
            if (!lh[li]->isConstant()) lnet = ((const NetBit*) lh[li])->getNet();
            if (!rh[ri]->isConstant()) rnet = ((const NetBit*) rh[ri])->getNet();
            if (lnet != NULL && rnet != NULL)
            {   
                // Check to see if either of them are a leaf
                // bool l_is_leaf = (*_int_nets[lnet])[0]->isLeaf();
                bool r_is_leaf = (*_int_nets[rnet])[0]->isLeaf();
                
                // If there is something that is a leaf, merge with it, otherwise
                // be merged with something that is a leaf
                Nets* keep_equivs = NULL;
                Nets* merge_equivs = NULL;
                if (r_is_leaf)
                {
                    keep_equivs = _int_nets[rnet];
                    merge_equivs = _int_nets[lnet];
                }
                else
                {
                    keep_equivs = _int_nets[lnet];
                    merge_equivs = _int_nets[rnet];
                }    
                // Remap every single equivalent net and also copy over anything that is a leaf
                for (Nets::const_iterator it = merge_equivs->begin(); it != merge_equivs->end(); ++it)
                {
                    // Remap a net equivalent to rnet to the nets equivalent lnet;
                    _int_nets[(*it)->getNet()] = keep_equivs;
                    // add it to the list of nets equivalent to the lnet
                    keep_equivs->push_back(*it);
                }
                // Delete rnet
                delete merge_equivs;
            }
        }
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
            
} // namespace Thermal

