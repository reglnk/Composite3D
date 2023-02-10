#ifndef CM3D_FILE_SYSTEM_HPP
#define CM3D_FILE_SYSTEM_HPP

#include <Types/Aliases.hpp>
#include <Types/BasicString.hpp>
#include <Types/DynArray.hpp>

#include <cstdint>

// @todo symbolic links
// @todo code for *nix

namespace cm3d
{
	namespace FileSystem
	{
		// Intended to store normalized unix-like path
		typedef String sPath;

		enum class NodeType
		{
			NotExisting = 0,
			RegularFile,
			Directory,
			SymbolicLink,
			Device,
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
		
		sPath concat(const sPath &p, const sPath &r, const char sep = '/');
		void parentDir(String *path, bool allowRealloc = true);

		bool isPathSymbol(char Ch);

		bool isNormalized(const sPath &path);
		sPath normalize(const sPath &path);
		
		bool isUnixLike(const sPath &path);
		int toUnixLike(sPath *path);

		bool isWindowsLike(const sPath &path);
		int toWindowsLike(sPath *path);

		inline int toNative(sPath *path)
		{
#		ifdef _WIN32
			return toUnixLike(path);
#		else
			return toWindowsLike(path);
#		endif
		}

		int readFile(const char *path, sByte *buffer, sSize readSize, const char *modes = "rb", sSize begPosition = 0); // fin

		// @todo
		int createNode(const sPath &path, const NodeType type, bool createDirs = false);

		// File attributes are 4-byte on both linux and windows.
		// If default provided, the function will check actual ones.
		// If overridden, rawAttrib is just parsed.
		int checkNode(const sPath &path, NodeState *nS, uint32_t rawAttrib = 0U);
		
		int listDirectory(const sPath &path, DynArray<Entry> *entList);
	}
}

#endif // CM3D_UTILITY_FILE_SYSTEM_HPP
