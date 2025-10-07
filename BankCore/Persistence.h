#ifndef BANKCORE_PERSISTENCE_H
#define BANKCORE_PERSISTENCE_H

#include <vector>
#include <string>
#include "User.h"

// This is for saving and loading user accounts

namespace BankCore {
    std::vector<User> loadUsers(const std::string &filename);
    void saveUsers(const std::vector<User> &users, const std::string &filename);
}

#endif