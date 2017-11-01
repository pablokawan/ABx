#pragma once

#include <vector>
#include <string>
#include <stdint.h>

enum AccountType : uint8_t
{
    AccountTypeNormal = 1,
    AccountTypeTutor = 2,
    AccountTypeSeniorTutor = 3,
    AccountTypeGamemaster = 4,
    AccountTypeGod = 5
};

struct AccountCharacter
{
    uint64_t i;
    std::string name;
};

class Account
{
public:
    Account() = default;

    std::vector<AccountCharacter> characters_;
    std::string name_;
    std::string key_;
    uint64_t id_ = 0;
    uint32_t warnings_ = 0;
    AccountType type_ = AccountTypeNormal;
    bool loggedIn_ = false;
};
