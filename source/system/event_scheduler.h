
#ifndef __THERMAL_EVENT_SCHEDULER_H__
#define __THERMAL_EVENT_SCHEDULER_H__

namespace Thermal
{
    class EventScheduler
    {
    public:
        static void allocate();
        static void release();
        static EventScheduler* getSingleton();

    protected:
        EventScheduler();
        ~EventScheduler();

    private:
        static EventScheduler* _event_scheduler_singleton; 

    }; // class EventScheduler

} // namespace Thermal

#endif // __THERMAL_EVENT_SCHEDULER_H__
