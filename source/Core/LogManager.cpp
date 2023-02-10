#include <Core/LogManager.hpp>
#include <Utility/MtBasic.hpp>

#include <cstdio>
#include <mutex>
#include <thread>

namespace cm3d
{
	void LogManager::push(String const &s)
	{
		dMtx.lock();
		logData.push(s);
		textSupply.put();
		dMtx.unlock();
	}
	bool LogManager::open(const char *mode)
	{
		fMtx.lock();
		logFile = openStream(logPath.c_str(), mode);
		
		// If something will close the file before deriving the result, the current
		// logPath might be considered unavailable, though file was opened here.
		bool p = logFile != NULL;
		fMtx.unlock();
		return p;
	}
	void LogManager::join()
	{
		// Set the flag that logThr should exit
		CM3D_ACCESS(lsMtx, logState |= SExit);

		// Let it read this flag
		textSupply.put();
		
		if (logThr.joinable())
			logThr.join();
		
		// No other threads should access this
		logState &= ~SExit;
	}
	void LogManager::logMain(LogManager *m)
	{
		while (true)
		{
			m->textSupply.take();

			// This thread should exit when all data is processed and join() called
			bool shouldExit = false;
			CM3D_ACCESS(m->lsMtx, shouldExit = m->logState & SExit);
			CM3D_ACCESS(m->dMtx, shouldExit &= !(m->logData.length()));
			if (shouldExit) return;

#		if CM3D_DEBUG
			auto qSize1 = m->logData.size();
#		endif
			int res = m->logFunc(
				&m->logFile, m->logData, m->logPath,
				m->fMtx, m->dMtx, m->pMtx
			);
			if (res)
			{
#			if CM3D_DEBUG
				CM3D_ASSERT(qSize1 == m->logData.size());
#			endif
				m->textSupply.put();
			}
			CM3D_ACCESS(m->lsMtx, m->logState |= res);
		}
	}
}

