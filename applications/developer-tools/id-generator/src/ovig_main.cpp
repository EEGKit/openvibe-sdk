#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>

int main(int /*argc*/, char** /*argv*/)
{
	srand(uint32_t(time(nullptr)));
	for (int i = 0; i < 16; ++i)
	{
		const uint32_t v1 = (rand() & 0xffff), v2 = (rand() & 0xffff), 
					   v3 = (rand() & 0xffff), v4 = (rand() & 0xffff);
		printf("#define OV_ClassId_\t\t\tOpenViBE::CIdentifier(0x%04X%04X, 0x%04X%04X)\n", int(v1), int(v2), int(v3), int(v4));
	}

	return 0;
}
