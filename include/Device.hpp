#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <atomic>
#include <condition_variable>
#include <libevdev/libevdev.h>
#include <thread>
#include <mutex>

class Device
{
	private:
		struct libevdev** pp_dev;
		std::thread* p_freeze_threads;
		unsigned num_of_dev;
		std::condition_variable unfreeze_cond;
		
		std::chrono::seconds period;
		std::mutex lock;
		std::atomic_bool freeze_signal;
		std::atomic_uint64_t thread_counter;
		
		void grab_inputs(unsigned n);
		
	public:
		Device();
		~Device();

		void freeze();
		void freeze(std::chrono::seconds& freeze_time);
		void wait_for_threads();
};

#endif // DEVICE_HPP