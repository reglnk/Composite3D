#include <cm3d/Core/FileSystem.hpp>
#include <cm3d/Utility/GL/Preprocessor.hpp>

#include <cstdio>

using namespace cm3d;

int main(int argc, char **argv)
{
	if (argc < 3) {
		printf("Usage: %s <input> <output>\n", argv[0]);
		return 1;
	}
	
	sSize fsize;
	if (FileSystem::getFileSize(argv[1], &fsize)) return 2;
	String somestr(fsize);
	if (FileSystem::readFile(argv[1], (sByte *)somestr.begin().data(), fsize)) return 2;
	somestr[fsize] = 0;

	DynArray<char> output;
	int result = Utility::GL::Preprocessor::processSource(somestr.begin(), somestr.end(), &output);

	FILE *fsh = fopen(argv[2], "wb"); if (!fsh) return 2;
	fwrite(output.begin(), 1, output.length(), fsh);
	fclose(fsh);

	return result;
}
