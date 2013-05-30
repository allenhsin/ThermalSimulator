
#include <iostream>

#include "source/models/physical_model/verilog_to_thermal.h"
#include "source/models/physical_model/device_models/device_model.h"
#include "libutil/LibUtil.h"

namespace Thermal
{
    using std::string;
    using std::map;
    using std::vector;
    using namespace VerilogParser;

    // const string VerilogToThermal::HIER_SEPARATOR = ".";
    // const std::map<std::string, DeviceType> VerilogToThermal::NAME_MAP = initNameMap();

    // map<string, DeviceType> VerilogToThermal::initNameMap()
    // {
        // map<string, DeviceType> out;
        // out["LaserSourceOffChip"]               = LASER_SOURCE_OFF_CHIP;
        // out["LossyOpticalNet"]                  = LOSSY_OPTICAL_NET;
        // out["ResonantRing"]                     = RESONANT_RING;
        // out["ResonantRingDepletionModulator"]   = RESONANT_RING_DEPLETION_MODULATOR;
        // out["ModulatorDriver"]                  = MODULATOR_DRIVER;
        // out["ThermalTuner"]                     = THERMAL_TUNER;
        // return out;
    // }

    VerilogToThermal::Net::Net(const ElabNet* net, DeviceModel* inst)
        : _net(net), _inst(inst)
    {}
    
    VerilogToThermal::Net::~Net()
    {}
    
    VerilogToThermal::VerilogToThermal(
            vector<DeviceModel*>& devices,
            const map<string, DeviceModel*>& primitive_devices
        )
        : _devices(devices),
        _primitive_devices(primitive_devices),
        _reader(),
        _nets() 
    {}

    VerilogToThermal::~VerilogToThermal()
    {}
    
    void VerilogToThermal::dumpDevicesFromVerilog(
            vector<DeviceModel*>& devices, 
            const map<string, DeviceModel*>& primitive_devices,
            const vector<string>& verilog_files,
            const string& top_module
        )
    {    
        VerilogToThermal v_to_t(devices, primitive_devices);
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
        for(NetsMap::iterator it = _nets.begin(); it != _nets.end(); it = _nets.begin())
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
                 _nets.erase(net->getNet());
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

        LibUtil::Log::printLine("Done connecting devices");
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
        _nets[net] = new Nets(1, net_ds);
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
                // bool l_is_leaf = (*_nets[lnet])[0]->isLeaf();
                // bool r_is_leaf = (*_nets[rnet])[0]->isLeaf();
                
                Nets* keep_equivs = _nets[lnet];
                Nets* merge_equivs = _nets[rnet];
                
                // Simple optimization, always merge the smaller of the nets
                if(keep_equivs->size() < merge_equivs->size())
                    std::swap<Nets*>(keep_equivs, merge_equivs);

                // // If neither is a leaf or they are both leaves, merge the one with fewer
                // // equivalent nets into the one with more equivalent nets
                // if (r_is_leaf == l_is_leaf)
                // {
                    // if(_nets[lnet]->size() <= _nets[rnet]->size())
                    // {
                        // keep_equivs = _nets[rnet];
                        // merge_equivs = _nets[lnet];
                    // }
                    // else
                    // {
                        // keep_equivs = _nets[lnet];
                        // merge_equivs = _nets[rnet];                    
                    // }
                // }
                // // If there is only one thing that is a leaf, merge with it, otherwise
                // // be merged with something that is a leaf
                // else if (r_is_leaf)
                // {
                    // keep_equivs = _nets[rnet];
                    // merge_equivs = _nets[lnet];
                // }
                // else
                // {
                    // keep_equivs = _nets[lnet];
                    // merge_equivs = _nets[rnet];
                // }
                
                // Remap every single equivalent net and also copy over the equivalent nets in the
                // merge nets to the keep nets
                for (Nets::const_iterator it = merge_equivs->begin(); it != merge_equivs->end(); ++it)
                {
                    // Remap a net equivalent to rnet to the nets equivalent lnet;
                    _nets[(*it)->getNet()] = keep_equivs;
                    // add it to the list of nets equivalent to the lnet
                    keep_equivs->push_back(*it);
                }
                // Delete the container holding the merged nets
                delete merge_equivs;
            }
        }
    }
    
    bool VerilogToThermal::isPrimitive(const ElabModule* module)
    {
        return (_primitive_devices.count(module->getName()) != 0);
    }
    
    DeviceModel* VerilogToThermal::getPrimitive(const ElabInstance* inst)
    {
        const ElabModule* module = inst->getModule();        

        // Check to make sure the module exists
        if (!isPrimitive(module))
            LibUtil::Log::printFatalLine(std::cerr, "ERROR: Not a primitive module: " + 
                module->getName());

        // Create the device
        DeviceModel* out = _primitive_devices.at(module->getName())->clone();                        
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

