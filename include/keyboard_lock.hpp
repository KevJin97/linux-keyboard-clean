#ifndef kEYBOARD_LOCK_HPP
#define kEYBOARD_LOCK_HPP

#include <string>
#include <chrono>

inline const std::chrono::seconds default_period(30);

#ifdef GROUP_PERMISSIONS_ENABLED
#include <grp.h>

static inline __gid_t orig_gid = 0;
int change_group_permissions();
int restore_orig_permissions();

#endif

std::string eventfile(unsigned event_num);
unsigned count_events();

#endif // KEYBOARD_LOCK_HPP