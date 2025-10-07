#include "Persistence.h"
#include <fstream>

namespace BankCore {
    std::vector<User> loadUsers(const std::string &filename) {
        std::vector<User> users;
        std::ifstream in(filename, std::ios::binary);
        
        if (!in) { return users; }

        size_t userCount;
        in.read(reinterpret_cast<char*>(&userCount), sizeof(userCount));

        for (size_t i = 0; i < userCount; i++) {
            users.push_back(User::load(in));
        }
        
        return users;
    }

    void saveUsers(const std::vector<User> &users, const std::string &filename) {
        std::ofstream out(filename, std::ios::binary);
        size_t userCount = users.size();
        out.write(reinterpret_cast<const char*>(&userCount), sizeof(userCount));

        for (const auto &user: users) {
            user.save(out);
        }
    }
}