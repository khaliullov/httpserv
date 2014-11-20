#ifndef GUARD_SPINLOCK_H
#define GUARD_SPINLOCK_H 1

#include <atomic>

class spin_lock
{
 public:
	spin_lock() : spin_flag{ATOMIC_FLAG_INIT} { }

	spin_lock(const spin_lock &) = delete;
	spin_lock & operator = (const spin_lock &) = delete;

	// Is this correct?
	~spin_lock() { lock(); }

	void lock()
	{
		while (spin_flag.test_and_set(std::memory_order_acquire))
		{
			asm("pause" : : :);
		}
	}
	
	bool try_lock()
		{ return (! spin_flag.test_and_set(std::memory_order_acquire)); }

	void unlock() { spin_flag.clear(std::memory_order_release); }

 private:
	std::atomic_flag spin_flag;
};

#endif // GUARD_SPINLOCK_H
