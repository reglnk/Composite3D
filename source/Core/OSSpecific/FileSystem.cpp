#include <Core/OSSpecific/FileSystem.hpp>
#include <Core/FileSystem.hpp>

#ifdef _WIN32
#  include <securitybaseapi.h>
#endif

namespace cm3d
{
#	ifdef _WIN32
	namespace FileSystemWin
	{
		DWORD AccessViewer::init(LPCSTR objPath)
		{
			HANDLE hHeap = GetProcessHeap();
			WINBOOL bRetCode;
			DWORD dwRetCode = 0;
			DWORD dwBufSize = 0;

			bRetCode = GetFileSecurityA(objPath,
				OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION,
				NULL, 0, &dwBufSize
			);
			if (bRetCode || (dwRetCode = GetLastError()) != ERROR_INSUFFICIENT_BUFFER)
				return dwRetCode;
			
			pSecurityDesc = (PSECURITY_DESCRIPTOR)HeapAlloc(hHeap, 0, dwBufSize);
			if (!pSecurityDesc)
				return GetLastError();

			bRetCode = GetFileSecurityA(objPath,
				OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION,
				pSecurityDesc, dwBufSize, &dwBufSize
			);
			if (!bRetCode) {
				HeapFree(hHeap, 0, pSecurityDesc);
				pSecurityDesc = NULL;
				return GetLastError();
			}
			return 0;
		}
		DWORD AccessViewer::getPerms(HANDLE hToken, LPDWORD pdwAccessMode)
		{
			HANDLE hImpToken = NULL;
			WINBOOL bRetCode;
			DWORD dwRetCode = 0;
			DWORD dwBufSize = 0;
			*pdwAccessMode = 0;
			
			bRetCode = DuplicateToken(hToken, SecurityImpersonation, &hImpToken);
			if (!bRetCode)
				return GetLastError();
			
			DWORD dwRightsG;
			GENERIC_MAPPING rightsMap;

			PRIVILEGE_SET privSet;
			DWORD privSize = sizeof(privSet);
			BOOL bAccStatus = FALSE;

			do {
				DWORD dwReadRights;
				DWORD dwWriteRights;
				DWORD dwExecRights;

				rightsMap = {GENERIC_READ, 0, 0, 0};
				dwRightsG = FILE_GENERIC_READ;
				MapGenericMask(&dwRightsG, &rightsMap);

				bRetCode = AccessCheck(
					pSecurityDesc, hImpToken, dwRightsG,
					&rightsMap, &privSet, &privSize, &dwReadRights, &bAccStatus
				);
				if (!bRetCode) break;

				rightsMap = {0, GENERIC_WRITE, 0, 0};
				dwRightsG = FILE_GENERIC_WRITE;
				MapGenericMask(&dwRightsG, &rightsMap);

				bRetCode = AccessCheck(
					pSecurityDesc, hImpToken, dwRightsG,
					&rightsMap, &privSet, &privSize, &dwWriteRights, &bAccStatus
				);
				if (!bRetCode) break;

				rightsMap = {0, 0, GENERIC_EXECUTE, 0};
				dwRightsG = FILE_GENERIC_EXECUTE;
				MapGenericMask(&dwRightsG, &rightsMap);

				bRetCode = AccessCheck(
					pSecurityDesc, hImpToken, dwRightsG,
					&rightsMap, &privSet, &privSize, &dwExecRights, &bAccStatus
				);
				if (!bRetCode) break;

				*pdwAccessMode = dwReadRights | dwWriteRights | dwExecRights;
			} while (0);
			
			CloseHandle(hImpToken);
			return 0;
		}
		DWORD AccessViewer::getOwner(LPSTR *pUsername, LPSTR *pHost)
		{
			PSID pSidOwner;
			BOOL bOwnerDefaulted;
			WINBOOL bRes = GetSecurityDescriptorOwner(pSecurityDesc, &pSidOwner, &bOwnerDefaulted);
			if (!bRes)
				return GetLastError();
			DWORD dwRes = getLocalAcctName(pSidOwner, pUsername, pHost);
			return dwRes;
		}
		FileSystem::sPath ensureAsterisk(const FileSystem::sPath &path)
		{
			String newp;
			if (!path.size())
				newp = "*";
			else if (path.back() == '\\' || path.back() == '/')
				newp = path + "*";
			else newp = path + "\\*";

			FileSystem::toWindowsLike(&newp);
			return newp;
		}
		DWORD getLocalAcctName(PSID pUserSID, LPSTR *pUsername, LPSTR *pHost)
		{
			WINBOOL bRtnBool;
			DWORD dwAcctNameBufsize = 0, dwHostBufsize = 0;
			SID_NAME_USE sidType = SidTypeUnknown;
			HANDLE hHeap = GetProcessHeap();

			bRtnBool = LookupAccountSidA(
				NULL, // local machine
				pUserSID,
				pUsername? *pUsername: NULL, (LPDWORD)&dwAcctNameBufsize,
				pHost? *pHost: NULL, (LPDWORD)&dwHostBufsize,
				&sidType
			);

			if (pUsername && (*pUsername = (LPSTR)HeapAlloc(hHeap, 0, dwAcctNameBufsize * sizeof(char))) == NULL)
				return GetLastError();

			if (pHost && (*pHost = (LPSTR)HeapAlloc(hHeap, 0, dwHostBufsize * sizeof(char))) == NULL) {
				HeapFree(hHeap, 0, *pUsername);
				return GetLastError();
			}

			bRtnBool = LookupAccountSidA(
				NULL, // local machine
				pUserSID,
				pUsername? *pUsername: NULL, (LPDWORD)&dwAcctNameBufsize,
				pHost? *pHost: NULL, (LPDWORD)&dwHostBufsize,
				&sidType
			);
			
			if (!bRtnBool)
				return GetLastError();
			
			return 0;
		}
	} // namespace FileSystem
#	endif // _WIN32
} // namespace cm3d
