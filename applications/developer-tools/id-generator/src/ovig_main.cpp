#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>

int main(int /*argc*/, char** /*argv*/)
{
	srand(static_cast<unsigned int>(time(nullptr)));
	for (int i = 0; i < 16; i++)
	{
		const unsigned short int value1 = (rand() & 0xffff);
		const unsigned short int value2 = (rand() & 0xffff);
		const unsigned short int value3 = (rand() & 0xffff);
		const unsigned short int value4 = (rand() & 0xffff);
		printf("#define OV_ClassId_                                        OpenViBE::CIdentifier(0x%04X%04X, 0x%04X%04X)\n", int(value1), int(value2), int(value3), int(value4));
	}

	return 0;
}
