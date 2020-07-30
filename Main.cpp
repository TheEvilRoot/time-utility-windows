#include <iostream>
#include <sstream>

#include <vector>
#include <string>
#include <chrono>
#include <array>
#include <unordered_map>

#include "Log.h"

#include "Windows.h"

struct Options 
{
	std::string executablePath;
	std::vector<std::string> executableArgs;

	bool useSuffixes = false;

	std::array<std::string, 4> suffixes { "ms", "s", "m", "h" };
	std::array<int, 4>         dividers { 1000, 60, 60, 1 };
};

struct TimeValue
{
	double value;
	std::string suffix;
};

auto retrieveArgs(int argc, const char* argv[])
{
	std::vector<std::string> args(argc);
	for (int i = 0; i < argc; i++)
	{
		args[i] = std::string(argv[i]);
	}
	return args;
}

auto parseArgs(const std::vector<std::string>& args)
{
	Options opt;
	bool asExecutableArgs = false;

	for (auto i = 1; i < args.size(); i++)
	{
		const auto& arg = args[i];
		if (arg.empty()) continue;

		if (asExecutableArgs)
		{
			opt.executableArgs.push_back(arg);
			continue;
		}

		if (arg.front() == '-')
		{
			if (arg == "--")
			{
				asExecutableArgs = true;
				continue;
			}
			else if (arg == "-h")
			{
				opt.useSuffixes = true;
			}
			else 
			{
				Log::warning() << "Unknown parameter " << arg << "\n";
			}
		} 
		else
		{
			if (opt.executablePath.empty())
			{
				opt.executablePath = arg;
			} 
			else
			{
				Log::warning() << "Ambiguous executable paths. The last one will be used\n";
			}
		}
	}

	return opt;
}

auto* getCommandLine(const std::string& executable, const std::vector<std::string>& args)
{
	std::ostringstream stream;
	stream << "\"" << executable << "\" ";

	for (const auto& a : args)
		stream << "\"" << a << "\" ";

	auto string = stream.str();
	
	char* buffer = new char[string.size() + 1];
	for (int i = 0; i < string.size(); i++)
		buffer[i] = string[i];
	buffer[string.size()] = 0;
	return buffer;
}

auto getCurrentTimeMills()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now()
			.time_since_epoch());
}

auto createProcess(const Options& opt)
{
	auto* commandLine = getCommandLine(opt.executablePath, opt.executableArgs);

	PROCESS_INFORMATION processInfo{};
	STARTUPINFOA startUpInfo{};

	Log::info() << "Starting " << commandLine << "\n";
	Log::info() << std::string(25, '=') << "\n";

	startUpInfo.cb = sizeof(startUpInfo);

	auto start = getCurrentTimeMills();
	auto createResult = CreateProcessA(nullptr,
		commandLine,
		nullptr, nullptr, false,
		0, nullptr, nullptr,
		&startUpInfo, &processInfo);

	if (createResult)
	{
		WaitForSingleObject(processInfo.hProcess, INFINITE);
	}
	else
	{
		Log::error() << "Failed to create new process with executable " << opt.executablePath << "\n";
		Log::winError(GetLastError()) << "\n";
	}

	auto end = getCurrentTimeMills();
	delete[] commandLine;
	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);

	return end - start;
}

TimeValue convertTime(const Options &opt, uint64_t ms)
{
	double value = double(ms);

	if (!opt.useSuffixes)
		return { value, opt.suffixes.front() };

	for (int i = 0; i < opt.suffixes.size(); i++)
	{
		if (value < opt.dividers[i])
		{
			return { value, opt.suffixes[i] };
		}

		value /= opt.dividers[i];
	}
}

int main(int argc, const char* argv[])
{
	auto args = retrieveArgs(argc, argv);
	auto opt = parseArgs(args);

	if (opt.executablePath.empty())
	{
		Log::error() << "Executable path must be specified\n";
		return 1;
	}
	
	auto delta = createProcess(opt);

	Log::info() << std::string(25, '=') << "\n";
	Log::info() << "Process has been stopped\n";

	auto tv = convertTime(opt, delta.count());
	Log::info() << tv.value << tv.suffix << "\n";

	return 0;
}