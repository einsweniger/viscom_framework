/*************************************************************************
Copyright (c) 2012-2015 Miroslav Andel
All rights reserved.

For conditions of distribution and use, see copyright notice in sgct.h
*************************************************************************/

#ifndef _SGCT_VERSION_H_
#define _SGCT_VERSION_H_

#define SGCT_VERSION_MAJOR    2
#define SGCT_VERSION_MINOR    7
#define SGCT_VERSION_REVISION 3

#include <stdio.h>
#include <string>

namespace sgct
{
	std::string getSGCTVersion();
}

#endif
