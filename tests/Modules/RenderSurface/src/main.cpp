#define STB_IMAGE_IMPLEMENTATION
#include <Modules/RenderSurface.hpp>
#include <iostream>
#include <cstdio>

using namespace cm3d;

const auto tabulate(int depth) {
	for (int i = depth; i > 0; --i) std::cout << " ";
}

void displayNode(Graphics::Node const &n, Modules::RenderSurface const &rs, int depth = 0, bool showVert = false)
{
	tabulate(depth * 4);
	printf("%s [msh: %u | ch: %u]\n", n.name.c_str(), n.numMeshes, n.numInner);
	for (int row = 0; row < 4; ++row) {
		const auto &r = n.transform[row];
		tabulate(depth * 4);
		printf("[%5.2f, %5.2f, %5.2f, %5.2f]\n", r.x, r.y, r.z, r.w);
	}
	putchar('\n');
	
	for (auto i = 0; i < n.numMeshes; ++i)
	{
		const auto &v = rs.getMeshes()[n.meshesIdx[i]].getVertices();
		tabulate(depth * 4 + 4);
		printf("%s [%d]\n", rs.getMeshes()[n.meshesIdx[i]].getName().c_str(), i);
		
		if (showVert) {
			for (const auto &vert: v) {
				tabulate(depth * 4 + 8);
				std::cout << '(' << vert.x << ", " << vert.y << ", " << vert.z << ")\n";
			}
		}
	}
	tabulate(depth * 4);
	std::cout << "===========================================\n";
	
	for (auto i = 0; i < n.numInner; ++i) {
		displayNode(n.inner[i], rs, depth + 1, showVert);
	}
}

int main(int argc, char **argv)
{
	const char *openfile = argc > 1 ? argv[1] : "model.obj";
	Modules::RenderSurface surf;
	std::cout << "id: " << surf.getId() << '\n';
	String outp;
	surf.load(openfile, &outp);
	if (outp.size())
		std::cout << outp.c_str() << '\n';
	
	bool sv = argc > 2 && !strcmp(argv[2], "-v");
	displayNode(surf.getRoot(), surf, 0, sv);
}
