//
// NETWORKING PROJECT
//
/// \author Kevin Watchuk
/// \date Nov 22, 2018
///
/// Useful functions for the networking projects
///
#ifndef _USEFULFUNCTIONS
#define _USEFULFUNCTIONS

#include <string>

std::string DetermineParity(std::string package);
std::string AsciiToBinary(std::string data);
std::string BinaryToAscii(std::string data);
std::string TypetoHeader(char a);
bool CheckParity(std::string packet);
bool CheckAckOrNak(std::string packet);
std::string CheckType(std::string packet);
std::string GeneratePacket(char type, std::string data);

///\class Line
#endif // !_USEFULFUNCTIONS