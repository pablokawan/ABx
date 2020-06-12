/**
 * Copyright 2017-2020 Stefan Ascher
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include "Scheduler.h"
#include "Dispatcher.h"
#include "Logger.h"
#include "Subsystems.h"

namespace Asynch {

void Scheduler::SchedulerThread()
{
#ifdef DEBUG_SCHEDULER
    LOG_DEBUG << "Scheduler threat started" << std::endl;
#endif

    std::unique_lock<std::mutex> lockUnique(lock_, std::defer_lock);
    while (state_ != State::Terminated)
    {
        ScheduledTask* task = nullptr;
        std::cv_status ret = std::cv_status::no_timeout;

        lockUnique.lock();

        if (events_.empty())
        {
            signal_.wait(lockUnique);
        }
        else
        {
            ret = signal_.wait_until(
                lockUnique,
                events_.top()->GetCycle()
            );
        }

        if (ret == std::cv_status::timeout)
        {
            // Timeout
            task = events_.top();
            events_.pop();

            auto it = eventIds_.find(task->GetEventId());
            if (it == eventIds_.end())
            {
                lockUnique.unlock();
                delete task;
                continue;
            }

            eventIds_.erase(it);
            lockUnique.unlock();

            task->SetDontExpires();
            auto* disp = GetSubsystem<Asynch::Dispatcher>();
            if (disp)
                disp->Add(task, true);
        }
        else
            lockUnique.unlock();

    }
#ifdef DEBUG_SCHEDULER
    LOG_DEBUG << "Scheduler threat stopped" << std::endl;
#endif
}

uint32_t Scheduler::Add(ScheduledTask* task)
{
    bool doSignal = false;

    {
        std::scoped_lock lock(lock_);

        if (state_ == State::Running)
        {
            // Check for valid ID
            if (task->GetEventId() == 0)
                // Generate new ID
                task->SetEventId(idGenerator_.Next());
            // Insert this ID in the list of active events
            eventIds_.insert(task->GetEventId());

            // Add it to the Queue
            events_.push(task);
            // Signal if the list was empty or this event is at the top
            doSignal = (task == events_.top());

#ifdef DEBUG_SCHEDULER
            LOG_DEBUG << "Added event " << task->GetEventId() << std::endl;
#endif
        }
        else
        {
            LOG_ERROR << "Scheduler thread not running" << std::endl;
            delete task;
            return 0;
        }
    }

    if (doSignal)
        signal_.notify_one();

    return task->GetEventId();
}

bool Scheduler::StopEvent(uint32_t eventId)
{
    if (eventId == 0)
        return false;

#ifdef DEBUG_SCHEDULER
    LOG_DEBUG << "Stopping event " << eventId;
#endif

    std::scoped_lock lock(lock_);

    auto it = eventIds_.find(eventId);
    if (it != eventIds_.end())
    {
        eventIds_.erase(it);
        return true;
    }

    // Not found
    return false;
}

void Scheduler::Start()
{
    if (state_ != State::Running)
    {
        state_ = State::Running;
        thread_ = std::thread(&Scheduler::SchedulerThread, this);
    }
}

void Scheduler::Stop()
{
    if (state_ == State::Running)
    {
        {
            std::scoped_lock lock(lock_);
            state_ = State::Terminated;
            while (!events_.empty())
            {
                ScheduledTask* task = events_.top();
                events_.pop();
                delete task;
            }
            eventIds_.clear();
        }

        signal_.notify_one();
        thread_.join();
    }
}

}
