#include "keyboard_lock.hpp"

#include <filesystem>
#include <string>
#include <sys/types.h>
#include <unistd.h>

#ifdef GROUP_PERMISSIONS_ENABLED

#include <grp.h>
#include <iostream>

int change_group_permissions()
{
	auto grp = getgrnam("input");
	if (grp == NULL)
	{
		std::cerr << "Group name retrieval failed" << std::endl;
		return -1;
	}
	orig_gid  = getgid();
	if (setgid(grp->gr_gid) < 0)
	{
		std::cerr << "Failed to change group ID to \"input\"" << std::endl;
		return -1;
	}
	return 0;
}

int restore_orig_permissions()
{
	if (setgid(orig_gid) < 0)
	{
		std::cerr << "Could not restore group ID" << std::endl;
		return -1;
	}
	return 0;
}

#endif

std::string eventfile(unsigned event_num)
{
	return "/dev/input/event" + std::to_string(event_num);
}

unsigned count_events()
{
	unsigned count = 0;
	while (std::filesystem::exists(eventfile(count).c_str()))
	{
		++count;
	}
	
	return count;
}

