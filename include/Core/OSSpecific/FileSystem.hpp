#ifndef CM3D_OS_SPECIFIC_FILE_SYSTEM_HPP
#define CM3D_OS_SPECIFIC_FILE_SYSTEM_HPP

#include <Types/String.hpp>

#ifdef _WIN32
#  include <windows.h>
#endif

namespace cm3d
{
	namespace FileSystem
	{
		typedef String sPath;
	}
#	ifdef _WIN32
	namespace FileSystemWin
	{
		class AccessViewer
		{
		protected:
			PSECURITY_DESCRIPTOR pSecurityDesc;
		public:
			inline AccessViewer(LPCSTR objPath): pSecurityDesc(NULL) {
				init(objPath);
			}
			inline ~AccessViewer() {
				shutdown();
			}
			inline WINBOOL check() {
				return pSecurityDesc != NULL;
			}
			inline void shutdown() {
				if (pSecurityDesc) {
					HANDLE hHeap = GetProcessHeap();
					HeapFree(hHeap, 0, pSecurityDesc);
					pSecurityDesc = NULL;
				}
			}
			DWORD init(LPCSTR objPath);
			DWORD getPerms(HANDLE hToken, LPDWORD pdwAccessMode);
			DWORD getOwner(LPSTR *pUsername, LPSTR *pHost);
		};
		
		FileSystem::sPath ensureAsterisk(const FileSystem::sPath &path);
		DWORD getLocalAcctName(PSID pUserSID, LPSTR *pUsername, LPSTR *pHost);

	} // namespace FileSystemWin
#	endif
} // namespace cm3d

#endif // CM3D_FILE_SYSTEM_OS_SPECIFIC_HPP
