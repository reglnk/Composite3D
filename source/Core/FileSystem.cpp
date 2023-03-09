#include <Core/FileSystem.hpp>
#include <Utility/SCstring.hpp>
#include <Utility/Debug.hpp>

#include <fstream>
#include <cstdio>
#include <cstring>
#include <utility>

#ifndef _WIN32
#  define CM3D_FS_PATH_SEP '/'
#  include <unistd.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <dirent.h>
#  include <pwd.h>
#else
#  define CM3D_FS_PATH_SEP '\\'
#  include <windows.h>
#  include <aclapi.h>
#  include <accctrl.h>
#  include <securitybaseapi.h>
#endif

template<typename intType, typename intType2>
static inline bool expectRange(intType i, intType2 beg, intType2 end) {
	return i >= beg && i <= end;
}
static inline bool isSlash(char Ch) {
	return Ch == '/' || Ch == '\\';
}
static inline auto isCurrentPointer(const char *it) {
	return it[0] == '.' && (!it[1] || isSlash(it[1]));
};
static inline auto isParentPointer(const char *it) {
	return it[0] == '.' && it[1] == '.' && (!it[2] || isSlash(it[2]));
};

namespace cm3d
{
	namespace FileSystem
	{
		sPath concat(sPath const &p, sPath const &r, const char sep)
		{
			if (p.back() == sep)
				return p + r;
			
			sPath ncat(p.size() + r.size() + 2);
			memcpy(ncat.data(), p.data(), p.size());
			ncat[p.size()] = sep;
			strcpy(ncat.data() + p.size() + 1, r.data());
			
			return ncat;
		}

		void parentDir(sPath *p, bool allowRealloc)
		{
			auto iter = p->rBegin();

			if (iter == p->rEnd())
				return;

			bool endsWithSlash = false;
			if (*iter == '/' || *iter == '\\')
				++iter, endsWithSlash = true;
			
			if (iter == p->rEnd()) {
				if (!endsWithSlash && isPathSymbol((*p)[0]))
					(*p)[0] = '.';
				return;
			}
			
			auto prevSlash = iter;
			while (++prevSlash != p->rEnd()) {
				if (*prevSlash == '/' || *prevSlash == '\\')
					break;
			}
			--prevSlash;
			const size_t pS = prevSlash - iter;

			if (
				(pS == 0 && prevSlash[0] == '.') ||
				(pS == 1 && prevSlash[0] == '.' && prevSlash[-1] == '.')
			) {
				const char *part = endsWithSlash ? "../" : "/../";
				if (allowRealloc)
					*p += part;
				else {
					strcpy(p->end(), part);
					p->length() += 3 + !endsWithSlash;
				}
				return;
			}
			
			while (++iter != p->rEnd()) {
				if (*iter == '/' || *iter == '\\') {
					iter[-1] = '\0';
					p->length() = p->rEnd() - iter;
					return;
				}
			}
			
			// Avoid replacing such paths like "C:" with "."
			for (auto it = p->begin(); it < p->end() - endsWithSlash; ++it)
				if (!isPathSymbol(*it))
					return;
			
			if (allowRealloc)
				*p = "./";
			else {
				strcpy(p->data(), "./");
				p->length() = 2;
			}
		};

		bool isPathSymbol(char Ch)
		{
			switch(Ch)
			{
				case '<':
				case '>':
				case '?':
				case '*':
				case '|':
				case ':':
				case ';':
				case '\"':
				// case '\r':
				// case '\n':
				// case '\t':
				// case '\v':
				// case '\f':
				case '\0':
					return false;
				default:
					return true;
			}
		}

		bool isNormalized(const sPath &path)
		{
			if (!path.size())
				return false;

			sPath::cIterator nameIter;
			for (nameIter = path.begin(); nameIter != path.end(); ++nameIter)
			{
				if (nameIter == path.begin() || nameIter[-1] == '/' || nameIter[-1] == '\\')
				{
					if (isCurrentPointer(nameIter))
						continue;
					
					if (isParentPointer(nameIter)) {
						++nameIter;
						continue;
					}
				}
				if (isPathSymbol(*nameIter) && *nameIter != '/' && *nameIter != '\\')
				{
					for (auto iter = nameIter; iter != path.end(); ++iter)
						if ((iter[-1] == '/' || iter[-1] == '\\') &&
							(isCurrentPointer(iter) || isParentPointer(iter))
						) return false;
					return true;
				}
			}
			return false;
		}

		sPath normalize(const sPath &path)
		{
			if (
				(expectRange(path.size(), 1, 2) && isCurrentPointer(path.c_str())) ||
				(expectRange(path.size(), 2, 3) && isParentPointer(path.c_str()))
			) return sPath(path);

			sPath newp(path.size() + 1);
			newp.length() = 0;
			auto destIter = newp.begin();
			auto srcIter = path.begin();

			for (auto iter = srcIter;; ++iter)
			{
				if (iter == path.end() || isSlash(*iter))
				{
					const size_t copySize = iter + 1 - srcIter;
					memcpy(destIter, srcIter, copySize);
					srcIter = iter + 1;
					destIter += copySize;
					newp.length() = destIter - newp.begin();
				}
				else if (iter == path.begin() || isSlash(iter[-1]))
				{
					const size_t sizeLeft = path.end() - iter;

					if (sizeLeft >= 1 && isCurrentPointer(iter)) {
						srcIter = ++iter + 1;
					}
					else if (sizeLeft >= 2 && isParentPointer(iter))
					{
						parentDir(&newp, false);
						destIter = newp.end();
						
						srcIter = (iter += 1 + (iter[2] != '\0')) + 1;
					}
				}

				if (iter == path.end())
					break;
			}
			
			return newp;
		}

		bool isUnixLike(const sPath &path)
		{
			for (const auto Ch: path)
				if (Ch == '\\' || !isPathSymbol(Ch))
					return false;
			
			return true;
		}

		int toUnixLike(sPath *path)
		{
			// 1. Path separators
			for (auto &el: *path)
				if (el == '\\')
					el = '/';
			
			// 2. Convert "X:/" to "/x/"
			if (path->size() >= 2 && (*path)[1] == ':') {
				char &Ch = (*path)[0];
				if (Ch >= 'A' && Ch <= 'Z')
					Ch += 'a' - 'A';
				if (Ch < 'a' || Ch > 'z')
					return 1;
				(*path)[1] = Ch;
				(*path)[0] = '/';
			}
			return 0;
		}

		bool isWindowsLike(const sPath &path)
		{
			for (size_t i = 0; i < path.size(); ++i)
			{
				char Ch = path[i];
				if (Ch == '/')
					return false;
				
				if (!isPathSymbol(Ch) & ~(Ch == ':'))
					return false;
			}
			return true;
		}

		int toWindowsLike(sPath *path)
		{
			// 1. Path separators
			for (auto &el: *path)
				if (el == '/')
					el = '\\';
			
			// 2. Convert "\\x\\" to "X:\\"
			if (path->size() >= 2 && (*path)[0] == '\\') {
				char &Ch = (*path)[1];
				if (Ch >= 'a' && Ch <= 'z')
					Ch += 'A' - 'a';
				if (Ch < 'A' || Ch > 'Z')
					return 1;
				
				(*path)[0] = Ch;
				(*path)[1] = ':';

				if (path->size() == 2)
					*path += "\\";
			}
			return 0;
		}

		int getFileSize(const char *path, sSize *size, const char *modes)
		{
			FILE *rs = std::fopen(path, modes);
			if (!rs)
				return 1;
			
			if (std::fseek(rs, 0, SEEK_END))
				return 2;
			
			*size = std::ftell(rs);
			return 0;
		}

		int readFile(const char *path, sByte *buffer, sSize readSize, const char *modes, sSize begPosition)
		{
			FILE *rs = std::fopen(path, modes);
			if (!rs)
				return -1;
			
			if (std::fseek(rs, begPosition, SEEK_SET)) {
				std::fclose(rs);
				return -1;
			}

			sSize r_objects = std::fread(buffer, 1, readSize, rs);
			std::fclose(rs);

			return !(r_objects == readSize);
		}

		// @todo
		// int createNode(const sPath &path, const NodeType type, bool createDirs) {
		// 	throw "FS::createNode";
		// }

		int checkNode(const sPath &path, NodeState *nS, uint32_t rawAttrib)
		{
			using namespace Attribute;
			CM3D_ASSERT(path.size());

#		ifndef _WIN32
			struct stat st;
			int res = stat(path.c_str(), &st);
			if (res != 0)
				return res;
			
			nS->size = (sSize)(st.st_size);
			
			const auto md = st.st_mode;
			const auto tp = st.st_mode & S_IFMT;
			nS->type =
				tp == S_IFREG ? NodeType::RegularFile :
				tp == S_IFLNK ? NodeType::SymbolicLink:
				tp == S_IFDIR ? NodeType::Directory :
				tp == S_IFSOCK ? NodeType::Socket :
				tp == S_IFBLK ||
				tp == S_IFIFO ||
				tp == S_IFCHR ? NodeType::Device :
				NodeType::Unknown;
		
			if (geteuid() == st.st_uid)
			{
				nS->attrib |= pRead * ((md & S_IRUSR) != 0);
				nS->attrib |= pWrite * ((md & S_IWUSR) != 0);
				nS->attrib |= pExec * ((md & S_IXUSR) != 0);
			}
			else if (getegid() == st.st_gid)
			{
				nS->attrib |= pRead * ((md & S_IRGRP) != 0);
				nS->attrib |= pWrite * ((md & S_IWGRP) != 0);
				nS->attrib |= pExec * ((md & S_IXGRP) != 0);
			}
			else {
				nS->attrib |= pRead * ((md & S_IROTH) != 0);
				nS->attrib |= pWrite * ((md & S_IWOTH) != 0);
				nS->attrib |= pExec * ((md & S_IXOTH) != 0);
			}
			
			String::crIterator iter;
			for (iter = path.crEnd(); iter != path.crBegin(); ++iter)
				if (*iter == '/')
					break;
			const char *fname = iter;
			
			nS->attrib |= aHidden * (iter[1] == '.' &&
				// for windows compatibility
				!(isCurrentPointer(fname) || isParentPointer(fname))
			);
			auto usr = getpwuid(geteuid());
			if (!usr)
				return 2;
			nS->owner = usr->pw_name;
			return 0;
		
#		else
			using namespace FileSystemWin;

		/* ^== Parse attributes ==^ */

			if (!rawAttrib)
				rawAttrib = GetFileAttributesA(path.c_str());
			
			if (rawAttrib & FILE_ATTRIBUTE_HIDDEN)
				nS->attrib |= aHidden;
			
			constexpr uint32_t unknownFileTypeMask =
				FILE_ATTRIBUTE_COMPRESSED | FILE_ATTRIBUTE_ENCRYPTED |
				FILE_ATTRIBUTE_SPARSE_FILE;
			
			if (rawAttrib & FILE_ATTRIBUTE_REPARSE_POINT)
				nS->type = NodeType::SymbolicLink;
			else if (rawAttrib & FILE_ATTRIBUTE_DIRECTORY)
				nS->type = NodeType::Directory;
			else if (rawAttrib & FILE_ATTRIBUTE_SYSTEM)
				nS->type = NodeType::SystemFile;
			else if (rawAttrib & FILE_ATTRIBUTE_DEVICE)
				nS->type = NodeType::Device;
			else if (rawAttrib & unknownFileTypeMask)
				nS->type = NodeType::Unknown;
			else nS->type = NodeType::RegularFile;

		/* ^== View permissions and owner ==^ */

			AccessViewer av(path.c_str());
			if (!av.check())
				return GetLastError();
			
			HANDLE hToken;
			DWORD dwRetCode;
			HANDLE hHeap = GetProcessHeap();
			WINBOOL bRetCode = OpenProcessToken(GetCurrentProcess(),
				STANDARD_RIGHTS_READ | TOKEN_QUERY | TOKEN_IMPERSONATE | TOKEN_DUPLICATE,
				&hToken
			);
			if (!bRetCode)
				return GetLastError();
			
			DWORD accessMode;
			dwRetCode = av.getPerms(hToken, &accessMode);
			if (dwRetCode)
				return dwRetCode;
			
			nS->attrib |= pRead * ((accessMode & FILE_GENERIC_READ) == FILE_GENERIC_READ);
			if (rawAttrib & ~FILE_ATTRIBUTE_READONLY)
				nS->attrib |= pWrite * ((accessMode & FILE_GENERIC_WRITE) == FILE_GENERIC_WRITE);
			nS->attrib |= pExec * ((accessMode & FILE_GENERIC_EXECUTE) == FILE_GENERIC_EXECUTE);

			LPSTR sOwner;
			dwRetCode = av.getOwner(&sOwner, NULL);
			if (dwRetCode)
				return dwRetCode;
			
			nS->owner = sOwner;
			HeapFree(hHeap, 0, sOwner);
			
			av.shutdown();
		
		/* ^== Get size ==^ */

			if (nS->type != NodeType::Directory)
			{
				HANDLE hFile = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_VALID_FLAGS, NULL, OPEN_EXISTING, 0, NULL);
				
				if (hFile == INVALID_HANDLE_VALUE)
					return GetLastError();

				DWORD fsizeHigh;
				DWORD fsizeLow = GetFileSize(hFile, &fsizeHigh);
				nS->size = (sSize)(fsizeHigh) * ((sSize)(MAXDWORD) + 1) + fsizeLow;
			}
#		endif
			return 0;
		}
		
		int listDirectory(const sPath &path, DynArray<Entry> *entList)
		{
#		ifndef _WIN32
			struct dirent **entries;

			int q = scandir(path.c_str(), &entries, NULL, alphasort);
			if (q == -1)
				return 1;
			int retcode = 0;

			for (int e = 0; e < q; ++e)
			{
				struct dirent *entry = entries[e];
				Entry fEnt = {entry->d_name, NodeState()};
				retcode |= checkNode(FileSystem::concat(path, fEnt.name), &fEnt.st);
				free(entry);
			}
			free(entries);
			return retcode;
			
#		else
			using namespace FileSystemWin;

			sPath pathAst = ensureAsterisk(path);
			toWindowsLike(&pathAst);
			sPath dirPath = pathAst;
			dirPath[--dirPath.length()] = '\0';

			WIN32_FIND_DATAA fData;
			HANDLE hFile = FindFirstFileA(pathAst.c_str(), &fData);
			if (hFile == INVALID_HANDLE_VALUE)
				return -1;
			
			do {
				if (!strcmp(fData.cFileName, ".") || !strcmp(fData.cFileName, ".."))
					continue;
				
				sPath fullPath = dirPath + fData.cFileName;
				Entry fEnt = {fData.cFileName, NodeState()}; // @idea full path to node?
				checkNode(fullPath, &fEnt.st);
				entList->pushBack(std::move(fEnt));
			}
			while (FindNextFileA(hFile, &fData));

			FindClose(hFile);
			return 0;
#		endif
		}
	} // namespace FileSystem

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
