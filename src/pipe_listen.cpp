#include <pipe_utils.h>

using pipe_utils::ChildPipe;
using pipe_utils::ByteArray;

int main() {
    const char* pipeName = "\\\\.\\pipe\\sdn_pipe";

	ChildPipe childPipe(pipeName);
	// while (1) {
	// 	ByteArray binaryData = childPipe.read();

	// 	if (binaryData.empty()) {
	// 		std::cout << "No data received" << std::endl;
	// 		break;
	// 	}

	// 	// for (auto byte : binaryData) {
	// 	// 	std::cout << std::hex << static_cast<int>(byte) << " ";
	// 	// }

	// 	std::cout << std::endl;
		
	// 	std::cout << "Received data: ";

	// 	std::cout << std::dec << binaryData.size() << " bytes" << std::endl;

	// 	std::cout << "---------------------------------------------" << std::endl;

	// }

    return 0;
}
