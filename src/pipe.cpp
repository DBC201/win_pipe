#include <pipe_utils.h>

#include <chrono>
#include <thread>

using pipe_utils::ParentPipe;
using pipe_utils::ByteArray;

int main()
{
	// Define the pipe name
	const char *pipeName = "\\\\.\\pipe\\interface_pipe";

	ParentPipe parentPipe(pipeName);

	parentPipe.wait();

	while (1)
	{
		// ByteArray binaryData = {0x01, 0x02, 0x03, 0x04, 0x05};

		ByteArray binaryData;

		for (int i = 0; i < 1000; i++)
		{
			binaryData.push_back(i);
		}

		parentPipe.write(binaryData);

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return 0;
}
