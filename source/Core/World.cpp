#include <cm3d/Core/World.hpp>
#include <cm3d/IO/MemStream.hpp>
#include <cm3d/IO/FileStream.hpp>

using namespace cm3d;

constexpr size_t cm3wHeaderSize = 32;
constexpr uint32_t cm3wEndianMark = 0xf5eed071;
constexpr uint32_t cm3wFormatVersion = 0;

struct idLocData
{
	uint64_t id;
	uint64_t fileOffset;
};
static_assert(sizeof(idLocData) == 16);

template<typename T>
void readvar(uint8_t *&iter, T &v) {
	std::memcpy(&v, iter, sizeof(T));
	iter += sizeof(T);
}

template<typename T>
void writevar(uint8_t *&iter, T const &v) {
	std::memcpy(iter, &v, sizeof(T));
	iter += sizeof(T);
}

World::IOResult World::load(FileSystem::sPath const &file)
{
	sSize fsize;
	if (FileSystem::getFileSize(file.c_str(), &fsize) || fsize < cm3wHeaderSize)
		return FSError;

	std::vector<uint8_t> buffer(fsize);
	if (FileSystem::readFile(file.c_str(), buffer.data(), fsize))
		return FSError;

	uint8_t *iter = buffer.data();
	uint8_t *bufend = buffer.data() + buffer.size();
	if (memcmp(iter, "CM3W", 4))
		return FormatError;

	iter += 4;
	uint32_t endian;
	readvar(iter, endian);
	if (endian != cm3wEndianMark)
		return NotSupported;

	uint32_t version;
	readvar(iter, version);
	printf("cm3w format version: %u\n", version);
	if (version > cm3wFormatVersion)
		return FormatError;

	uint32_t blkMetaSize;
	readvar(iter, blkMetaSize);

	uint64_t blkObjMapSize;
	readvar(iter, blkObjMapSize);

	uint64_t blkSharedMapSize;
	readvar(iter, blkSharedMapSize);

	// skipping metadata (@todo maybe add optional argument to function, to read here)
	iter += blkMetaSize;

	if (iter > bufend) return FormatError;

	std::vector<idLocData> objectLoc(blkObjMapSize);
	std::memcpy(objectLoc.data(), iter, blkObjMapSize);

	std::vector<idLocData> sharedLoc(blkSharedMapSize);
	std::memcpy(sharedLoc.data(), iter, blkSharedMapSize);

	// preloading finished ==========================
	// Now we can do some hashmap and fill it with IDs, and select IDs
	// of objects which are needed, and load them separately.
	// The shared modules likewise.
	// but the method loads everything so do it

	for (auto loc: sharedLoc)
	{
		iter = buffer.data() + loc.fileOffset;
		if (iter + sizeof(uint64_t) + sizeof(uint32_t) > bufend)
			return FormatError;

		uint64_t modSize;
		uint32_t modType;
		readvar(iter, modSize);
		readvar(iter, modType);

		// load module at key ID
		auto m = std::make_shared<Modules::Base>();
		io::MemStreamR ms(iter, modSize);
		m->binDeserialize(ms);
		if (ms.tellg() != modSize)
			return FormatError;

		sharedModules.insert(std::move(m), loc.id);
	}

	for (auto loc: objectLoc)
	{
		iter = buffer.data() + loc.fileOffset;
		if (iter + 8 > bufend)
			return FormatError;

		Object &obj = this->objects.insert(Object(), loc.id);

		uint32_t numModules;
		readvar(iter, numModules);
		uint32_t numShrModules;
		readvar(iter, numShrModules);

		for (auto i = 0; i != numShrModules; ++i)
		{
			uint64_t modId;
			readvar(iter, modId);
			obj.addShared(sharedModules[modId]);
		}

		for (auto i = 0; i != numModules; ++i)
		{
			uint32_t mType;
			readvar(iter, mType);
			uint64_t mSize;
			readvar(iter, mSize);

			io::MemStreamR ms(iter, mSize);
			auto mod = new Modules::Base;
			mod->binDeserialize(ms);

			if (ms.tellg() != mSize)
				return FormatError;

			obj.addModule(mod);
		}
	}
	return Success;
}

// @todo save to binary stream instead of file
World::IOResult World::save(FileSystem::sPath const &file, cm3d::World::Metadata const &meta)
{
	// 1. calculate all space required to save the file
	// 2. save the file =)

	size_t fileSize = cm3wHeaderSize;

	fileSize += meta.name.length() + 1;
	size_t metaBlkSize;
	{
		size_t fileSizeAligned = 64;
		while (fileSizeAligned <= fileSize)
			fileSizeAligned <<= 1;

		metaBlkSize = fileSizeAligned - fileSize;
		fileSize = fileSizeAligned;
	}

	size_t objLocTableBlkSize = sizeof(idLocData) * objects.length();
	fileSize += objLocTableBlkSize *= 2; // extra space

	size_t shrLocTableBlkSize = sizeof(idLocData) * sharedModules.length();
	fileSize += shrLocTableBlkSize *= 2; // extra space

	size_t objectsBlkSize = objects.length() * sizeof(uint32_t) * 2;
	for (auto const &obj: objects) {
		objectsBlkSize += obj.getSharedModules().length() * sizeof(uint64_t);
		objectsBlkSize += obj.getModules().length() * (sizeof(uint64_t) + sizeof(uint32_t));
		for (auto const &m: obj.getModules())
			objectsBlkSize += m->binSize();
	}
	fileSize += objectsBlkSize * 2 + 64; // extra space

	size_t shrModulesBlkSize = sharedModules.size() * (sizeof(uint64_t) + sizeof(uint32_t));
	for (auto const &m: sharedModules)
		shrModulesBlkSize += m->binSize();

	fileSize += shrModulesBlkSize * 2 + 64; // extra space

	// writing ==============================

	std::vector<uint8_t> buffer(fileSize);
	uint8_t *iter = buffer.data();

	const char mark[4] = {'C', 'M', '3', 'W'};
	memcpy(iter, mark, 4); iter += 4;

	uint32_t ui32;

	writevar(iter, cm3wEndianMark);
	writevar(iter, cm3wFormatVersion);

	// writing metadata =============
	{
		uint8_t *metaIter = iter;
		memcpy(metaIter, meta.name.data(), meta.name.length());
		metaIter += meta.name.length();
		*metaIter = 0;
	}
	iter += metaBlkSize;

	idLocData *objLocTable = reinterpret_cast<idLocData *>(iter);
	iter += objLocTableBlkSize;

	idLocData *shrLocTable = reinterpret_cast<idLocData *>(iter);
	iter += shrLocTableBlkSize;

	std::unordered_map<Modules::Base *, size_t> sharedModulesIdMap;
	for (auto it = sharedModules.begin(); it != sharedModules.end(); ++it)
		sharedModulesIdMap[it->get()] = ~it;

	size_t objcount = 0;
	for (auto it = objects.begin(); it != objects.end(); ++it)
	{
		Object const &obj = *it;
		objLocTable[objcount].id = ~it;
		objLocTable[objcount++].fileOffset = iter - buffer.data();

		writevar(iter, obj.getModules().length());
		writevar(iter, obj.getSharedModules().length());

		for (auto const &m: obj.getSharedModules())
			writevar(iter, sharedModulesIdMap[m.get()]);

		for (auto const &m: obj.getModules())
		{
			uint64_t modSize = m->binSize();
			const size_t fullSize = modSize + sizeof(uint64_t) + sizeof(uint32_t);
			io::MemStream ms(iter, fullSize);
			ms << modSize << m->getId();
			m->binSerialize(ms);
			CM3D_ASSERT(ms.tellg() == (int64_t)fullSize);
			iter += fullSize;
		}
	}

	size_t modcount = 0;
	for (auto it = sharedModules.begin(); it != sharedModules.end(); ++it)
	{
		shrLocTable[modcount].id = ~it;
		shrLocTable[modcount++].fileOffset = iter - buffer.data();

		auto const &m = *it;
		uint64_t modSize = m->binSize();
		const size_t fullSize = modSize + sizeof(uint64_t) + sizeof(uint32_t);
		io::MemStream ms(iter, fullSize);
		ms << modSize << m->getId();
		m->binSerialize(ms);
		CM3D_ASSERT(ms.tellg() == (int64_t)fullSize);
		iter += fullSize;
	}

	return FileSystem::writeFile(file.c_str(), buffer.data(), buffer.size())
	? FSError
	: Success;
}

