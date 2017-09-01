#include <mutex>
#include <memory>
#include <condition_variable>

class semaphore
{
  public:
    constexpr semaphore(unsigned long count = 0) noexcept :
    m_count(count) {}

    semaphore(const semaphore&) = delete;

    semaphore& operator=(const semaphore&) = delete;

    void notify()
    {
	{
	    std::unique_lock<std::mutex> lock(m_mutex);
	    check_cv_ptr();
	    ++m_count;
	}
	m_cv_uptr->notify_one();
    }

    void wait()
    {
	std::unique_lock<std::mutex> lock(m_mutex);
	check_cv_ptr();
	while (m_count == 0) { // we may have spurious wakeup!
	    m_cv_uptr->wait(lock);
	}
	--m_count;
    }

    bool try_wait()
    {
	std::unique_lock<std::mutex> lock(m_mutex);
	check_cv_ptr();
	if (m_count == 0) {
	    return false;
	}
	else {
	    --m_count;
	    return true;
	}
    }

  private:
    void check_cv_ptr()
    {
	if (m_cv_uptr == nullptr) {
	    m_cv_uptr.reset(new std::condition_variable);
	}
    }
    
  private:
    std::mutex m_mutex;
    std::unique_ptr<std::condition_variable> m_cv_uptr;
    unsigned long m_count;
};


// Copyright Xingli Qi qixingli2009@gmail.com

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
