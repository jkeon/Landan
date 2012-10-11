/*********************************
*Class: Landan.h
*Description: Central class for the library. Simply include this to gain access
*to every class in the Landan Framework.
*Author: jkeon
**********************************/

#ifndef _LANDAN_H_
#define _LANDAN_H_


//////////////////////////////////////////////////////////////////////
// PLATFORMS /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//Windows Machine 32 Bit
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

//TODO: Windows Machine 64 Bit
//#ifdef _WIN64
//#endif

//TODO: MacOS
//#ifdef __APPLE__
//#endif

//TODO: Linux
//#ifdef __linux__
//#endif


//////////////////////////////////////////////////////////////////////
// INCLUDES //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//LANDAN INCLUDES

//core
#include <landan/core/LandanTypes.h>

//application - config
#include <landan/application/config/ApplicationConfig.h>





#endif /* _LANDAN_H_ */
