#pragma once

#include <iostream>
#include <memory>
#include <functional>
#include <vector>
#include <unordered_map>

namespace muduo
{
    using TaskFunc = std::function<void()>;
    using ReleaseFunc = std::function<void()>;

    class TimeTask
    {
    private:
        uint64_t _id;
        uint32_t _timeout;
        bool _canceled; //任务是否被取消

        TaskFunc _task_cb;
        ReleaseFunc _release_cb;

    public:
        TimeTask(uint64_t id,uint32_t timeout,TaskFunc task_cb)
            :_id(id),_timeout(timeout),_task_cb(task_cb),_canceled(false)
        {}

        ~TimeTask()
        {
            if(_canceled == false)
            {
                _task_cb();
                _release_cb();
            }
        }

        void Cancel()
        {
            _canceled = true;
        }

        void SetRelease(const ReleaseFunc& cb)
        {
            _release_cb = cb;
        }

        int DelayTime()
        {
            return _timeout;
        }
    };


    class TimeWheel
    {
        using SharePtr = std::shared_ptr<TimeTask>;
        using WeakPtr = std::weak_ptr<TimeTask>;

    private:
        // remove
        int _tick; // 秒针
        int _capacity;

        std::vector<std::vector<SharePtr>> _Wheel;
        std::unordered_map<uint64_t, WeakPtr> _timetask_map;

        void Remove(uint64_t id)
        {
            auto it = _timetask_map.begin();

            while(it!= _timetask_map.end())
            {
                if(it->first == id)
                {
                    _timetask_map.erase(it);
                }
                it++;
            }
        }

    public:
        TimeWheel():_capacity(60),_tick(0),_Wheel(_capacity){}


        void TaskAdd(uint64_t id,uint32_t timeout,TaskFunc task_cb)
        {
            SharePtr pt(new TimeTask(id, timeout, task_cb));
            pt->SetRelease(std::bind(&TimeWheel::Remove, this, id));
            int pos = (_tick + timeout) % _capacity;
            _Wheel[pos].push_back(pt);

            WeakPtr _pt(pt);
            _timetask_map[id] = _pt;
        }

        void Refresh(uint64_t id)
        {
            auto it = _timetask_map.find(id);
            if(it == _timetask_map.end())
            {
                return;
            }

            SharePtr ptr = it->second.lock(); 
            int delay = ptr->DelayTime();

            int pos = (_tick + delay) % _capacity;
            _Wheel[pos].push_back(ptr);
        }

        void TimerCancel(uint64_t id)
        {
            auto it = _timetask_map.find(id);
            if(it == _timetask_map.end())
            {
                return;
            }

            SharePtr ptr = it->second.lock(); 
            if(ptr)
            {
                ptr->Cancel();  //取消
            }
        }

        void RunTimeTask()
        {
            _tick = (_tick + 1) % _capacity;
            _Wheel[_tick].clear(); // ->析构函数 执行任务
        }
    };
}