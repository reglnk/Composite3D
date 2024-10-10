#ifndef CM3D_FILE_SYSTEM_HPP
#define CM3D_FILE_SYSTEM_HPP

#include <cm3d/Tl/Aliases.hpp>
#include <cm3d/Tl/String.hpp>
#include <cm3d/Tl/DynArray.hpp>

#ifdef _WIN32
#  include <windows.h>
#endif

#include <cstdint>

namespace cm3d
{
	namespace FileSystem
	{
		// Intended to store normalized unix-like path
		typedef String sPath;

		// @todo more types
		enum class NodeType
		{
			NotExisting = 0,
			RegularFile,
			Directory,
			SymbolicLink,
			Socket, // *nix-only
			SystemFile, // windows-only
			Device, // char devices, block devices, ...
			Unknown
		};
		namespace Attribute
		{
			// Current process permissions
			constexpr uint32_t pRead = 0x01;

			// @os:windows If the process has write permission, but the file is read-only,
			// then pWrite will be disabled.
			constexpr uint32_t pWrite = 0x02;
			constexpr uint32_t pExec = 0x04;

			constexpr uint32_t pRW = pRead | pWrite;
			constexpr uint32_t pRWX = pRead | pWrite | pExec;

			// @os:*nix aHidden will be set if file name begins with dot.
			constexpr uint32_t aHidden = 0x1000;
		}

		struct NodeState
		{
			NodeType type;
			uint32_t attrib;
			sSize size;
			String owner;

			inline NodeState(): type(NodeType::NotExisting), attrib(0), size(0), owner() {}

			inline NodeState(
				NodeType type,
				uint32_t attrib,
				sSize size,
				String const &owner
			): type(type), attrib(attrib), size(size), owner(owner) {}
		};

		struct Entry {
			sPath name;
			NodeState st;

			inline Entry(): name(), st() {}
			inline Entry(sPath const &name, NodeState const &st):
				name(name), st(st) {}
		};
		
		sPath concat(const sPath &p, const sPath &r, char sep = '/');
		
		void parentDir(sPath *path);
		inline sPath parentDir(sPath const &path) {
			sPath p = path;
			parentDir(&p);
			return p;
		}

		constexpr inline bool isPathSymbol(char Ch)
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
		
		bool isAbsolute(const sPath &path);

		bool isNormalized(const sPath &path);
		sPath normalize(const sPath &path);
		
		bool isUnixLike(const sPath &path);
		int toUnixLike(sPath *path);

		bool isWindowsLike(const sPath &path);
		int toWindowsLike(sPath *path);

		inline int toNative(sPath *path)
		{
#		ifndef _WIN32
			return toUnixLike(path);
#		else
			return toWindowsLike(path);
#		endif
		}

		int getFileSize(const char *path, sSize *size, const char *modes = "rb");
		int readFile(const char *path, void *buffer, size_t readSize, const char *modes = "rb", sSize begPosition = 0);
		int writeFile(const char *path, const void *buffer, size_t wSize, const char *modes = "wb");

		// @todo
		// int createNode(const sPath &path, const NodeType type, bool createDirs = false);

		// File attributes are 4-byte on both linux and windows.
		// If rawAttrib has default value, the function will check actual ones.
		// Otherwise, attributes of nS are set based on the value.
		int checkNode(const sPath &path, NodeState *nS, uint32_t rawAttrib = 0U);
		
		int listDirectory(const sPath &path, DynArray<Entry> *entList);
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
#	endif // _WIN32

} // namespace cm3d
#endif // CM3D_UTILITY_FILE_SYSTEM_HPP
