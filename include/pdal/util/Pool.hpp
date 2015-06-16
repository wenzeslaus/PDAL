/******************************************************************************
* Copyright (c) 2015, Connor Manning (connor@hobu.co)
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following
* conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in
*       the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of Hobu, Inc. or Flaxen Geo Consulting nor the
*       names of its contributors may be used to endorse or promote
*       products derived from this software without specific prior
*       written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
****************************************************************************/

#pragma once

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace pdal
{

class Pool
{
public:
    // After numThreads tasks are actively running, and queueSize tasks have
    // been enqueued to wait for an available worker thread, subsequent calls
    // to Pool::add will block until an enqueued task has been popped from the
    // queue.
    Pool(std::size_t numThreads, std::size_t queueSize = 1);
    ~Pool();

    // Start worker threads
    void go();

    // Wait for all currently running tasks to complete.
    void join();

    // Add a threaded task, blocking until a thread is available.  If join() is
    // called, add() may not be called again until go() is called and completes.
    void add(std::function<void()> task);

private:
    // Worker thread function.  Wait for a task and run it - or if stop() is
    // called, complete any outstanding task and return.
    void work();

    // Atomically set/get the stop flag.
    bool stop();
    void stop(bool val);

    std::size_t m_numThreads;
    std::size_t m_queueSize;
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;

    std::atomic<bool> m_stop;
    std::mutex m_mutex;
    std::condition_variable m_produceCv;
    std::condition_variable m_consumeCv;

    // Disable copy/assignment.
    Pool(const Pool& other);
    Pool& operator=(const Pool& other);
};

} // namespace pdal

