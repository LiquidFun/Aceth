#include <iostream>

#ifndef EKU_ANSICOL
#define EKU_ANSICOL

namespace eku
{

	// PREFIX = "\033["; // Alternatively: "\001b["
	const std::string P = "\033[";

	const std::string black        = P + "30m";
	const std::string dark_red     = P + "31m";
	const std::string dark_green   = P + "32m";
	const std::string dark_yellow  = P + "33m";
	const std::string dark_blue    = P + "34m";
	const std::string dark_purple  = P + "35m";
	const std::string dark_pink    = dark_purple;
	const std::string dark_magenta = dark_purple;
	const std::string dark_aqua    = P + "36m";
	const std::string dark_cyan    = dark_aqua;
	const std::string dark_white   = P + "37m";

	const std::string gray         = P + "30;1m";
	const std::string red          = P + "31;1m";
	const std::string green        = P + "32;1m";
	const std::string yellow       = P + "33;1m";
	const std::string blue         = P + "34;1m";
	const std::string purple       = P + "35;1m";
	const std::string pink         = purple;
	const std::string magenta      = purple;
	const std::string cyan         = P + "36;1m";
	const std::string aqua         = cyan;
	const std::string white        = P + "37;1m";

	inline void settextcolor(std::string newTextColor)
	{
		std::cout << newTextColor;
	}

	inline void setbackcolor(std::string newBackColor)
	{
		int from = P.size() + 1;
		int to = newBackColor.size();
		std::cout << P + "4" + newBackColor.substr(from, to);
	}

	inline void setcolor(std::string newTextColor, std::string newBackColor)
	{
		settextcolor(newTextColor);
		setbackcolor(newBackColor);
	}
	
	// Compatibility with concol.h
	inline void update_colors() {}

	// Compatibility with concol.h
	inline void concolinit() {}
}
#endif	//_INC_EKU_IO_CONCOL
