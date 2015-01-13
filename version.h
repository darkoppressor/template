#ifndef version_h
#define version_h

#include <string>

namespace Version{
	//Build date
	static const char YEAR[]="2015";
	static const char MONTH[]="01";
	static const char DAY[]="12";

	//Version
	static const int MAJOR=0;
	static const int MINOR=0;
	static const int MICRO=1;

	//Status
	static const char STATUS[]="alpha";

	//Engine version
	static const std::string ENGINE_VERSION="1.2.1";

	//Engine date
	static const std::string ENGINE_DATE="2015-01-12";
}

#endif
