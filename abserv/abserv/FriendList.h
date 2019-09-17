#pragma once

#include <AB/Entities/FriendList.h>
#include <sa/Iteration.h>

namespace Game {

class FriendList
{
public:
    enum class Error
    {
        Success,
        PlayerNotFound,
        AlreadyFriend,
        NoFriend,
        InternalError
    };
private:
    std::string accountUuid_;
    AB::Entities::FriendList friendList_;
    FriendList::Error AddFriendAccount(const std::string& accountUuid, const std::string& name,
        AB::Entities::FriendRelation relation);
public:
    FriendList(const std::string accountUuid) :
        accountUuid_(accountUuid),
        friendList_{}
    { }
    ~FriendList() = default;

    void Load();
    FriendList::Error AddFriendByName(const std::string& playerName, AB::Entities::FriendRelation relation);
    FriendList::Error ChangeNickname(const std::string& currentName, const std::string& newName);
    FriendList::Error Remove(const std::string& accountUuid);

    bool IsFriend(const std::string& accountUuid);
    bool IsIgnored(const std::string& accountUuid);
    bool GetFriendByName(const std::string& name, AB::Entities::Friend& f);
    bool GetFriendByAccount(const std::string& accountUuid, AB::Entities::Friend& f);
    template <typename Callback>
    void VisitAll(const Callback& callback)
    {
        for (auto& f : friendList_.friends)
            if (callback(f) != Iteration::Continue)
                break;
    }
    size_t Count() const { return friendList_.friends.size(); }
};

}
