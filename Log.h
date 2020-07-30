#pragma once

#include <string>
#include <iostream>
#include <iomanip>

namespace Log {

	inline std::ostream& warning()
	{
		std::cout << "[Warining] ";
		return std::cout;
	}

	inline std::ostream& error()
	{
		std::cerr << "[Error] ";
		return std::cerr;
	}

	inline std::ostream& info()
	{
		std::cout << "[Info] ";
		return std::cout;
	}

	inline std::ostream& winError(int code)
	{
		error() << "Error (" << code << ") ";
		switch (code)
		{
		case 0x2: std::cerr << "File Not Found"; break;
		default: break;
		}
		return std::cerr;
	}
}