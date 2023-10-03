/**
 * @file npcap_wrapper.h
 * @author Deniz Cakiroglu (dcakiroglu@torontomu.ca)
 * @brief
 * @date 2023-10-01
 *
 */
#ifndef PIPE_UTILS_H
#define PIPE_UTILS_H

#include <iostream>
#include <windows.h>
#include <vector>
#include <string>

namespace pipe_utils
{
	using ByteArray = std::vector<unsigned char>;

	class PipeUtils
	/**
		 * @brief 
		 * Some common pipe errors:
		 * 
		 * ERROR_PIPE_BUSY - 
		 * 
		 * ERROR_NO_DATA - 
		 * 
		 * ERROR_PIPE_NOT_CONNECTED  
		 * 
		 * @return DWORD 
	*/
	{
	public:
		ByteArray read()
		{
			ByteArray buffer(output_buffer_size);
			DWORD bytesRead;
			ByteArray data;

			while (1)
			{
				if (!ReadFile(hPipe, buffer.data(), 12, &bytesRead, NULL) || bytesRead != 12)
				{
					throw std::runtime_error("Failed to read data from pipe");
				}

				std::string dataSizeStr(buffer.begin(), buffer.begin() + 12);
				buffer.resize(std::stoi(dataSizeStr));

				try
				{
					unsigned int size = std::stoi(dataSizeStr);

					if (size > 0)
					{
						if (ReadFile(hPipe, buffer.data(), static_cast<DWORD>(size), &bytesRead, NULL))
						{
							data.assign(buffer.begin(), buffer.begin() + bytesRead);
							return data;
						}
						else
						{
							throw std::runtime_error("Failed to read data from pipe");
						}
					}
					else
					{
						throw std::invalid_argument("Invalid size");
					}
				}
				catch (const std::invalid_argument &e)
				{
				}
			}
		}

		BOOL write(ByteArray data)
		{
			DWORD dataSize = static_cast<DWORD>(data.size());

			std::string dataSizeStr = std::to_string(dataSize);
			while (dataSizeStr.length() < 12)
			{
				dataSizeStr = "0" + dataSizeStr;
			}

			ByteArray dataToSend;
			dataToSend.reserve(dataSizeStr.length() + data.size());
			dataToSend.insert(dataToSend.end(), dataSizeStr.begin(), dataSizeStr.end());
			dataToSend.insert(dataToSend.end(), data.begin(), data.end());

			DWORD bytesWritten;
			return WriteFile(hPipe, dataToSend.data(), static_cast<DWORD>(dataToSend.size()), &bytesWritten, NULL);
		}

		void close()
		{
			CloseHandle(hPipe);
		}

		~PipeUtils()
		{
			CloseHandle(hPipe);
		}

	protected:
		HANDLE hPipe;
		unsigned long input_buffer_size = 512;
		unsigned long output_buffer_size = 512;
	};

	class ParentPipe : public PipeUtils
	{
	public:
		ParentPipe(const char *pipeName)
		{
			hPipe = CreateNamedPipe(
				pipeName,
				PIPE_ACCESS_OUTBOUND,
				// PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE
				PIPE_TYPE_BYTE | PIPE_READMODE_BYTE
				// | PIPE_NOWAIT, // writing and reading are non-blocking
				| PIPE_WAIT,
				// PIPE_UNLIMITED_INSTANCES,
				1,					// Number of child pipes
				input_buffer_size,	// Output buffer size in bytes
				output_buffer_size, // Input buffer size in bytes
				NMPWAIT_WAIT_FOREVER,
				NULL);
			if (hPipe == INVALID_HANDLE_VALUE)
			{
				throw std::runtime_error("Failed to create named pipe. Error code: " + GetLastError());
			}
		}

		/**
		 * @brief If no_wait is set to true, the pipe will be duplex
		 * 
		 * @param pipeName 
		 * @param no_wait 
		 */
		ParentPipe(const char *pipeName, bool no_wait)
		{
			hPipe = CreateNamedPipe(
				pipeName,
				PIPE_ACCESS_DUPLEX,
				// PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE
				PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | (no_wait ? PIPE_NOWAIT : PIPE_WAIT),
				// | PIPE_NOWAIT, // writing and reading are non-blocking
				// | PIPE_WAIT,
				// PIPE_UNLIMITED_INSTANCES,
				1,					// Number of child pipes
				input_buffer_size,	// Output buffer size in bytes
				output_buffer_size, // Input buffer size in bytes
				NMPWAIT_WAIT_FOREVER,
				NULL);
			if (hPipe == INVALID_HANDLE_VALUE)
			{
				throw std::runtime_error("Failed to create named pipe. Error code: " + GetLastError());
			}
		}

		/**
		 * @brief wait until a client connects to the pipe
		 *
		 * @return true
		 * @return false
		 */
		bool wait()
		{
			return ConnectNamedPipe(hPipe, NULL);
		}
	};

	class ChildPipe : public PipeUtils
	{
	public:
		ChildPipe(const char *pipeName, bool can_read, bool can_write)
		{
			hPipe = CreateFile(
				pipeName,
				(can_read ? GENERIC_READ : 0) | (can_write ? GENERIC_WRITE : 0),
				0,
				NULL,
				OPEN_EXISTING,
				0,
				NULL);
			if (hPipe == INVALID_HANDLE_VALUE)
			{
				throw std::runtime_error("Failed to create named pipe. Error code: " + GetLastError());
			}
		}
	};
}

#endif // PIPE_UTILS_H
