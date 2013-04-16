
#ifndef __THERMAL_DATA_GENERATOR_H__
#define __THERMAL_DATA_GENERATOR_H__

namespace Thermal
{
    typedef double Time;

    class DataGenerator
    {
    public:
        DataGenerator();
        ~DataGenerator();

        // set name and bit period
        void setName(std::string name);
        void setBitPeriod(double bit_period);
        
        // get the bit corresponding to the time
        bool getBit(Time time);

    private:
        // name
        std::string _name;
        // bit period
        double _bit_period;
        // the current bit
        bool _current_bit;
        // time since last bit
        Time _time_since_last_bit;

    };

} // namespace Thermal

#endif // __THERMAL_DATA_GENERATOR_H__

