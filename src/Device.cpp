#include "Device.hpp"
#include "keyboard_lock.hpp"

#include "libevdev/libevdev.h"
#include <cstdint>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <fcntl.h>

#ifdef GROUP_PERMISSIONS_ENABLED

Device::Device()
{
	this->freeze_signal.store(false, std::memory_order_release);
	this->period = default_period;

	if (change_group_permissions() != 0)
	{
		this->num_of_dev = 0;
		this->pp_dev = NULL;
		this->p_freeze_threads = NULL;
		return;
	}
	else
	{
		this->num_of_dev = count_events();
		this->pp_dev = new struct libevdev*[this->num_of_dev];
		this->p_freeze_threads = new std::thread[this->num_of_dev];

		for (unsigned n = 0; n < this->num_of_dev; ++n)
		{
			
			if (libevdev_new_from_fd(open(eventfile(n).c_str(), O_RDONLY), &this->pp_dev[n]) < 0)
			{
				// Error creating device
				libevdev_free(this->pp_dev[n]);
				this->~Device();
				return;
			}
			this->p_freeze_threads[n] = std::thread(&Device::grab_inputs, this, n);
			this->thread_counter.fetch_add(1, std::memory_order_acq_rel);
		}
	}
	restore_orig_permissions();
}

#else

Device::Device()
{
	this->freeze_signal.store(false, std::memory_order_release);
	this->period = default_period;
	this->num_of_dev = count_events();
	this->pp_dev = new struct libevdev*[this->num_of_dev];
	this->p_freeze_threads = new std::thread[this->num_of_dev];

	for (unsigned n = 0; n < this->num_of_dev; ++n)
	{
		
		if (libevdev_new_from_fd(open(eventfile(n).c_str(), O_RDONLY), &this->pp_dev[n]) < 0)
		{
			// Error creating device
			libevdev_free(this->pp_dev[n]);
			this->~Device();
			
			continue;
		}
		this->p_freeze_threads[n] = std::thread(&Device::grab_inputs, this, n);
		this->thread_counter.fetch_add(1, std::memory_order_acq_rel);
	}
}

#endif

Device::~Device()
{
	if (this->num_of_dev != 0)
	{
		this->freeze_signal.store(false, std::memory_order_seq_cst);
		this->unfreeze_cond.notify_all();

		for (unsigned n = 0; n < this->num_of_dev; ++n)
		{
			this->p_freeze_threads[n].join();
		}
		for (unsigned n = 0; n < this->num_of_dev; ++n)
		{
			close(libevdev_get_fd(this->pp_dev[n]));
			libevdev_free(this->pp_dev[n]);
		}
		delete[] this->p_freeze_threads;
		delete[] this->pp_dev;
	}
}

void Device::freeze()
{
	this->freeze_signal.store(true, std::memory_order_release);
	this->freeze_signal.notify_all();
}

void Device::freeze(std::chrono::seconds& freeze_time)
{
	this->period = freeze_time;
	this->freeze();
}

void Device::grab_inputs(unsigned n)
{
	if (this->freeze_signal.load(std::memory_order_acquire) == false)
	{
		this->freeze_signal.wait(false, std::memory_order_acquire);
	}
	libevdev_grab(this->pp_dev[n], LIBEVDEV_GRAB);
	{
		std::unique_lock<std::mutex> lock(this->lock);
		this->unfreeze_cond.wait_for(lock, this->period, [this]{ return this->freeze_signal.load(std::memory_order_acquire) == false; });
	}
	libevdev_grab(this->pp_dev[n], LIBEVDEV_UNGRAB);

	this->thread_counter.fetch_sub(1, std::memory_order_acq_rel);
	this->thread_counter.notify_all();
}

void Device::wait_for_threads()
{
	if (this->thread_counter.load(std::memory_order_acquire) != 0)
	{
		uint64_t active_thread;
		while ((active_thread = this->thread_counter.load(std::memory_order_acquire)) != 0)
		{
			this->thread_counter.wait(active_thread, std::memory_order_acquire);
		}

	}
}