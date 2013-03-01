#ifndef __THERMAL_DATA_H__
#define __THERMAL_DATA_H__

namespace Thermal
{
    class Data
    {
    public:
        static void allocate();
        static void release();
        static Data* getSingleton();

    protected:
        Data();
        ~Data();

    private:
        static Data* _data_singleton; 

    }; // class Data

} // namespace Thermal

#endif // __THERMAL_DATA_H__
