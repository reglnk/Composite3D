#include <cm3d/Core/World.hpp>
#include <cm3d/IO/MemStream.hpp>
#include <cm3d/IO/FileStream.hpp>

struct idLocData
{
	uint64_t id;
	uint64_t fileOffset;
};
static_assert(sizeof(idLocData) == 16);

int cm3d::World::load(FileSystem::sPath const &file)
{
	sSize fsize;
	if (FileSystem::getFileSize(file.c_str(), &fsize) || fsize < 32)
		return 1;

	std::vector<uint8_t> buffer(fsize);
	if (FileSystem::readFile(file.c_str(), buffer.data(), fsize))
		return 1;

	uint8_t *iter = buffer.data();
	uint8_t *bufend = buffer.data() + buffer.size();
	if (memcmp(iter, "CM3W", 4))
		return 2;

	iter += 4;
	uint32_t endian;
	memcpy(&endian, iter, 4); iter += 4;
	if (endian != 0xf5eed071)
		return 2;

	uint32_t version;
	memcpy(&version, iter, 4); iter += 4;
	printf("cm3w format version: %u\n", version);
	if (version > 0)
		return 2;

	uint32_t blkMetaSize;
	memcpy(&blkMetaSize, iter, 4); iter += 4;

	uint64_t blkObjMapSize;
	memcpy(&blkObjMapSize, iter, 8); iter += 8;

	uint64_t blkSharedMapSize;
	memcpy(&blkSharedMapSize, iter, 8); iter += 8;

	// skipping metadata (@todo maybe add optional argument to function, to read here)
	iter += blkMetaSize;

	if (iter > bufend) return 2;
	uint64_t ui64;

	// @todo more of that
	// if constexpr (sizeof(uint64_t) > sizeof(size_t)) {
	// 	if (ui64 > SIZE_MAX)
	// 		return 3;
	// }
	// @todo check whether adding the offset will cause integer overflow at the pointer
	// if (iter + blkObjMapSize > bufend)
	// 	return 3;

	std::vector<idLocData> objectLoc(blkObjMapSize);
	std::memcpy(objectLoc.data(), iter, blkObjMapSize);

	// preloading finished ==========================
	// Now we can do some hashmap and fill it with IDs, and select IDs
	// of objects which are needed, and load them separately.
	// The shared modules likewise.
	// but the method loads everything so do it

	for (auto loc: objectLoc)
	{
		iter = buffer.data() + loc.fileOffset;
		if (iter + 8 > bufend)
			return 2;

		Object &obj = this->objects.insert(Object(), loc.id);

		uint32_t numModules;
		std::memcpy(&numModules, iter, 4); iter += 4;
		for (auto i = 0; i != numModules; ++i)
		{
			uint32_t mType;
			std::memcpy(&mType, iter, 4); iter += 4;
			uint64_t mSize;
			std::memcpy(&mSize, iter, 8); iter += 8;

			// deserialize...
			// obj.addModule(...)
		}
	}
	return 0;
}

// @todo save to binary stream instead of file
int cm3d::World::save(FileSystem::sPath const &file, cm3d::World::Metadata const &meta)
{
	// 1. calculate all space required to save the file
	// 2. save the file =)

	size_t fileSize = 24; // header size

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
	fileSize += shrModulesBlkSize * 2 + 64; // extra space

	// writing ==============================

	std::vector<uint8_t> buffer(fileSize);
	uint8_t *iter = buffer.data();

	const char mark[4] = {'C', 'M', '3', 'W'};
	memcpy(iter, mark, 4); iter += 4;

	uint32_t ui32 = 0xf5eed071;
	memcpy(iter, &ui32, 4); iter += 4;

	ui32 = 0; // version
	memcpy(iter, &ui32, 4);

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

	std::unordered_map<Modules::Base *, size_t> sharedAssoc;
	for (auto it = sharedModules.begin(); it != sharedModules.end(); ++it)
		sharedAssoc[it->get()] = ~it;

	size_t objcount = 0;
	for (auto it = objects.begin(); it != objects.end(); ++it)
	{
		Object const &obj = *it;
		objLocTable[objcount].id = ~it;
		objLocTable[objcount++].fileOffset = iter - buffer.data();

		const uint32_t numModules = obj.getModules().length();
		memcpy(iter, &numModules, 4); iter += 4;
		const uint32_t numShared = obj.getSharedModules().length();
		memcpy(iter, &numShared, 4); iter += 4;

		for (auto const &m: obj.getSharedModules())
		{
			const uint64_t modId = sharedAssoc[m.get()];
			memcpy(iter, &modId, 8); iter += 8;
		}
		for (auto const &m: obj.getModules())
		{
			uint64_t mod_size = m->binSize();
			io::MemStream ms(iter, mod_size + 8);
			ms << mod_size;
			m->binSerialize(ms);
			CM3D_ASSERT(ms.tellg() == (int64_t)mod_size);
			iter += mod_size;
		}
	}
	return FileSystem::writeFile(file.c_str(), buffer.data(), buffer.size());
}

