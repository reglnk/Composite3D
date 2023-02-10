#ifndef CM3D_LOG_MANAGER_HPP
#define CM3D_LOG_MANAGER_HPP

/* ======================================================
 * A minimalistic log manager for multithreaded access.
 * Goals:
 * 1. Provide output to single file from multiple threads
 * 2. Don't delay calling threads on writing
 * 3. Don't stay idle when data is loaded and
 * 4. Don't depend on time (e.g. int dataCheckPeriod = 500)
**/

#include <cstddef>
#include <cstdio>
#include <cstdint>

#include <thread>
#include <mutex>

#include <Types/Aliases.hpp>
#include <Types/String.hpp>
#include <Types/Queue.hpp>

#include <Utility/SysBasic.hpp>
#include <Utility/MtBasic.hpp>
#include <Utility/IOBasic.hpp>

namespace cm3d
{
	class LogManager
	{
	public:
		static constexpr int WError = 0x01;
		static constexpr int SExit = 0x8000;
		static constexpr const char *stdOpenMode = "wb";
		
		typedef int (*pLoggerFunc)(
			FILE **logFile,
			Queue<String> &logData,
			String &logPath,
			std::mutex &fMtx,
			std::mutex &dMtx,
			std::mutex &pMtx
		);

		static inline int stdLoggerFunc(
			FILE **logFile,
			Queue<String> &logData,
			String &logPath,
			std::mutex &fMtx,
			std::mutex &dMtx,
			std::mutex &pMtx
		) {
			// Allocate given file
			sUlock fLock(fMtx);
			if (!logFile) {
				CM3D_ACCESS(pMtx, *logFile =
					openStream(logPath.c_str(), stdOpenMode));
				if (!logFile)
					return WError;
			}
			
			// Allocate the data queue
			sUlock dLock(dMtx);
			auto &text = logData.front();

			sSize p_beg = ftell(*logFile);
			fwrite(text.c_str(), text.length(), 1, *logFile);
			sSize delta_p = ftell(*logFile) - p_beg;

			if (delta_p != (sSize)text.length())
				return WError;
			
			logData.pop();
			return 0;
		}
/* =============================================================== */

		inline LogManager(pLoggerFunc func = stdLoggerFunc):
			logFunc(func), logState(0)
		{ logThr = std::thread(logMain, this); }

		inline LogManager(
			const char *path,
			const char *mode = stdOpenMode,
			pLoggerFunc func = stdLoggerFunc
		): logPath(path), logFunc(func), logState(0)
		{
			logThr = std::thread(logMain, this);
			logFile = openStream(path, mode);
		}
		inline ~LogManager() {
			// TODO: configurable exceptions
			join();
		}
/* ===============================================================
 * Read-only accessors
**/
		constexpr String const &getLogPath() const {
			return logPath;
		}
		constexpr Queue<String> const &getLogData() const {
			return logData;
		}
		constexpr pLoggerFunc const &getLogFunc() const {
			return logFunc;
		}
		constexpr int getLoggerState() const {
			return logState;
		}
		inline bool is_open() const {
			return logFile != NULL;
		}
/* =============================================================== */

		// Push the text to write
		void push(String const &s);

		inline int collectErrors() {
			lsMtx.lock();
			// Don't return SExit flag which is not an error
			int r = logState & ~SExit;
			logState ^= r;
			lsMtx.unlock();
			return r;
		}
		// Flush the queue
		inline void flushAll() {
			dMtx.lock();
			while (logData.length()) {
				// this call shouldn't wait
				textSupply.take();
				logData.pop();
			}
			dMtx.unlock();
		}
		inline void setPath(String const &path) {
			sUlock pLock(pMtx);
			logPath = path;
		}
		inline void setLogFunc(pLoggerFunc func) {
			// TODO: test this
			logFunc = func;
		}
/* ===============================================================
 * File management
**/
		bool open(const char *mode = stdOpenMode);
		inline bool open(const char *path, const char *mode = stdOpenMode)
		{
			sUlock lock(pMtx);
			logPath = path;
			return this->open(path, mode);
		}
		inline void close() {
			sUlock lock(fMtx);
			fclose(logFile);
		}

/* ===============================================================
 * Thread management
**/
		void launch();
		// Wait for writing data, then join
		void join();

	private:
		// The only thread which writes to logFile
		std::thread logThr;

		/* This function may be overriden. Must guarantee that logData
		content isn't changed unless 0 returned */
		pLoggerFunc logFunc;

		String logPath;
		Queue<String> logData;
		FILE *logFile;
		Semaphore textSupply;
		
		// errors of logFunc and flags like SExit are accumulated here
		int logState;

		std::mutex fMtx; // mutex for logFile
		std::mutex dMtx; // mutex for logData
		std::mutex pMtx; // mutex for logPath
		std::mutex lsMtx; // mutex for logState

		static void logMain(LogManager *m);
	};
}

#endif
