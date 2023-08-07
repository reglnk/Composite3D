#include <cm3d/Core/FileSystem.hpp>
#include <cstdio>
#include <cinttypes>

using namespace cm3d;

int main(int argc, char **argv)
{
	FileSystem::sPath path(".");
	if (argc > 1)
		path = argv[1];
	
	DynArray<FileSystem::Entry> list;
	int res = FileSystem::listDirectory(path, &list);
	if (res != 0)
		printf("WARN: %d\n", res);
	
	for (auto &e: list) {
		const char * ntype =
			e.st.type == FileSystem::NodeType::Directory ? "dir" :
			e.st.type == FileSystem::NodeType::SymbolicLink ? "lnk" :
			e.st.type == FileSystem::NodeType::RegularFile ? "reg" :
			e.st.type == FileSystem::NodeType::Device ? "dev" :
			e.st.type == FileSystem::NodeType::Unknown ? "unk" :
				"err";
		
		char attrib[8];
		size_t aiter = 0;
		if (e.st.attrib & FileSystem::Attribute::pRead)
			attrib[aiter++] = 'r';
		if (e.st.attrib & FileSystem::Attribute::pWrite)
			attrib[aiter++] = 'w';
		if (e.st.attrib & FileSystem::Attribute::pExec)
			attrib[aiter++] = 'x';
		if (e.st.attrib & FileSystem::Attribute::aHidden)
			attrib[aiter++] = 'H';
		attrib[aiter] = 0;

		printf("%s | %-30s | owner: %-20s | size: %" PRId64 " | attrib: %s\n",
			ntype, e.name.c_str(), e.st.owner.c_str(), e.st.size, attrib);
	}
}
