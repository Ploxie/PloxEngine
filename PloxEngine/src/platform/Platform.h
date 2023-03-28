//
// Created by Ploxie on 2023-03-23.
//

#pragma once
#include <cstdint>
#include "filesystem/FileSystem.h"

class IPlatform : public FileSystem
{
public:

};

class Platform
{
public:

	static const FileSystem* const FileSystem;

private:
	static IPlatform* s_instance;

};
