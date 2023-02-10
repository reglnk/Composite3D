#include <Core/OSSpecific/FileSystem.hpp>
#include <Core/FileSystem.hpp>
#include <Utility/SCstring.hpp>
#include <Utility/Debug.hpp>

#include <fstream>
#include <cstdio>
#include <cstring>
#include <utility>

#ifndef _WIN32
#  define FS_PATH_SEP '/'
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <dirent.h>
#else
#  define FS_PATH_SEP '\\'
#  include <windows.h>
#  include <aclapi.h>
#  include <accctrl.h>
#endif

template<typename intType, typename intType2>
CM3D_CXX14_CONSTEXPR_INLINE bool expectRange(intType i, intType2 beg, intType2 end) {
	return i >= beg && i <= end;
}

CM3D_CXX14_CONSTEXPR_INLINE bool isSlash(char Ch) {
	return Ch == '/' || Ch == '\\';
}

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

		void parentDir(String *p, bool allowRealloc)
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
			const auto isCurrentPointer = [](const char *it) {
				return it[0] == '.' && (!it[1] || it[1] == '/' || it[1] == '\\');
			};
			const auto isParentPointer = [](const char *it) {
				return it[0] == '.' && it[1] == '.' && (!it[2] || it[2] == '/' || it[2] == '\\');
			};

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
			const auto isCurrentPointer = [](const char *it) {
				return it[0] == '.' && (!it[1] || isSlash(it[1]));
			};
			const auto isParentPointer = [](const char *it) {
				return it[0] == '.' && it[1] == '.' && (!it[2] || isSlash(it[2]));
			};

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
		int createNode(const sPath &path, const NodeType type, bool createDirs) {
			throw "FS::createNode";
		}

		int checkNode(const sPath &path, NodeState *nS, uint32_t rawAttrib)
		{
			using namespace Attribute;

#		ifndef _WIN32
			struct stat st;
			auto res = stat(path.c_str(), &st;
			if (res) != 0)
				return res;
			
			nS->size = (sSize)(st.st_size);			
/*
			nS.attrib |= dRead * ((st.st_mode & S_IROTH) != 0);
			nS.attrib |= dWrite * ((st.st_mode & S_IWOTH) != 0);
			nS.attrib |= dExec * ((st.st_mode & S_IXOTH) != 0);

			nS.attrib |= gRead * ((st.st_mode & S_IRGRP) != 0);
			nS.attrib |= gWrite * ((st.st_mode & S_IWGRP) != 0);
			nS.attrib |= gExec * ((st.st_mode & S_IXGRP) != 0);
			
			nS.attrib |= oRead * ((st.st_mode & S_IRUSR) != 0);
			nS.attrib |= oWrite * ((st.st_mode & S_IWUSR) != 0);
			nS.attrib |= oExec * ((st.st_mode & S_IXUSR) != 0);
*/			
#		else
			using namespace FileSystemWin;

		/* ^== Parse attributes ==^ */

			if (!rawAttrib)
				rawAttrib = GetFileAttributesA(path.c_str());
			
			if (rawAttrib & FILE_ATTRIBUTE_HIDDEN)
				nS->attrib |= aHidden;
			
			constexpr uint32_t unknownFileTypeMask =
				FILE_ATTRIBUTE_COMPRESSED | FILE_ATTRIBUTE_ENCRYPTED |
				FILE_ATTRIBUTE_SPARSE_FILE | FILE_ATTRIBUTE_REPARSE_POINT;
			
			if (rawAttrib & FILE_ATTRIBUTE_REPARSE_POINT)
				nS->type = NodeType::SymbolicLink;
			else if (rawAttrib & FILE_ATTRIBUTE_DIRECTORY)
				nS->type = NodeType::Directory;
			else if (rawAttrib & FILE_ATTRIBUTE_DEVICE)
				nS->type = NodeType::Device;
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
} // namespace cm3d
