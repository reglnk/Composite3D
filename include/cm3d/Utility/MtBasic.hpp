#ifndef CM3D_MT_BASIC_HPP
#define CM3D_MT_BASIC_HPP

#include <mutex>
#include <condition_variable>

// "fmtx.lock(); f = 78; fmtx.unlock();" to be
// replaced with CM3D_ACCESS(fmtx, f = 78);
#define CM3D_ACCESS(mtx, ...) do { \
	(mtx).lock(); \
	__VA_ARGS__; \
	(mtx).unlock(); \
} while (0)

namespace cm3d
{
	typedef std::unique_lock<std::mutex> sUlock;

	class Semaphore {
		int res;
		std::mutex mtx;
		std::condition_variable cv;
	public:
		inline Semaphore(int res = 0) : res(res) {}
		
		constexpr int get() {
			return res;
		}
		inline void put() {
			sUlock lock(mtx);
			++res;
			cv.notify_one();
		}
		inline void put(int q) {
			sUlock lock(mtx);
			res += q;
			cv.notify_one();
		}
		inline void take() {
			sUlock lock(mtx);
			while (!res)
				cv.wait(lock);
			--res;
		}
	};
}

#endif