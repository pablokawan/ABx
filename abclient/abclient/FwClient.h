#pragma once

#include "Client.h"
#include "Receiver.h"
#include <AB/ProtocolCodes.h>
#include "Structs.h"
#include <AB/Entities/Character.h>
#include <map>

struct EventItem
{
    StringHash eventId;
    VariantMap eventData;
};

class FwClient : public Object, public Client::Receiver
{
    URHO3D_OBJECT(FwClient, Object);
private:
    String currentLevel_;
    String currentMapUuid_;
    bool levelReady_;
    Vector<EventItem> queuedEvents_;
    uint32_t playerId_;
    Client::Client client_;
    Client::Client::ClientState lastState_;
    AB::Entities::CharacterList characters_;
    std::map<std::string, AB::Entities::Game> games_;
    std::vector<AB::Entities::MailHeader> mailHeaders_;
    AB::Entities::Mail currentMail_;
    String currentCharacterUuid_;
    bool loggedIn_;
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandleLevelReady(StringHash eventType, VariantMap& eventData);
    void QueueEvent(StringHash eventType, VariantMap& eventData);
public:
    static String GetProtocolErrorMessage(uint8_t err);

    FwClient(Context* context);
    ~FwClient();

    uint32_t GetIp() const
    {
        if (loggedIn_)
            return client_.GetIp();
        return 0;
    }
    /// Get difference of server and client clock
    int64_t GetClockDiff() const
    {
        if (loggedIn_)
            return client_.GetClockDiff();
        return 0;
    }
    bool Start();
    void Stop();
    void Login(const String& name, const String& pass);
    void CreateAccount(const String& name, const String& pass, const String& email, const String& accKey);
    void CreatePlayer(const String& name, const String& profUuid, AB::Entities::CharacterSex sex, bool isPvp);
    void EnterWorld(const String& charUuid, const String& mapUuid);
    void ChangeWorld(const String& mapUuid);
    void Logout();
    void GetMailHeaders();
    void ReadMail(const std::string& uuid);
    void DeleteMail(const std::string& uuid);
    void Move(uint8_t direction);
    void Turn(uint8_t direction);
    void SetDirection(float rad);
    void SelectObject(uint32_t sourceId, uint32_t targetId);
    void Command(AB::GameProtocol::CommandTypes type, const String& data);
    void GotoPos(const Vector3& pos);
    void FollowObject(uint32_t objectId);

    /// asio network error
    void OnNetworkError(const std::error_code& err) override;
    /// Protocol error, e.g. Login failed
    void OnProtocolError(uint8_t err) override;

    void OnGetCharlist(const AB::Entities::CharacterList& chars) override;
    void OnGetGamelist(const std::vector<AB::Entities::Game>& games) override;
    void OnAccountCreated() override;
    void OnPlayerCreated(const std::string& uuid, const std::string& mapUuid) override;

    void OnGetMailHeaders(int64_t updateTick, const std::vector<AB::Entities::MailHeader>& headers) override;
    void OnGetMail(int64_t updateTick, const AB::Entities::Mail& mail) override;
    void OnEnterWorld(int64_t updateTick, const std::string& mapUuid, uint32_t playerId) override;
    void OnSpawnObject(int64_t updateTick, uint32_t id, const Vec3& pos, const Vec3& scale, float rot,
        PropReadStream& data, bool existing) override;
    void OnDespawnObject(int64_t updateTick, uint32_t id) override;
    void OnObjectPos(int64_t updateTick, uint32_t id, const Vec3& pos) override;
    void OnObjectRot(int64_t updateTick, uint32_t id, float rot, bool manual) override;
    void OnObjectStateChange(int64_t updateTick, uint32_t id, AB::GameProtocol::CreatureState state) override;
    void OnObjectSelected(int64_t updateTick, uint32_t sourceId, uint32_t targetId) override;
    void OnServerMessage(int64_t updateTick, AB::GameProtocol::ServerMessageType type,
        const std::string& senderName, const std::string& message) override;
    void OnChatMessage(int64_t updateTick, AB::GameProtocol::ChatMessageChannel channel,
        uint32_t senderId, const std::string& senderName, const std::string& message) override;

    void SetState(Client::Client::ClientState state)
    {
        if (state == Client::Client::StateDisconnected || state == Client::Client::StateCreateAccount)
            loggedIn_ = false;
        client_.state_ = state;
    }
    const String& GetCurrentCharacterUuid() const
    {
        return currentCharacterUuid_;
    }
    const AB::Entities::CharacterList& GetCharacters() const
    {
        return characters_;
    }
    const std::map<std::string, AB::Entities::Game>& GetGames() const
    {
        return games_;
    }
    String GetGameName(const String& uuid) const
    {
        auto it = games_.find(std::string(uuid.CString()));
        if (it == games_.end())
            return "";
        return String((*it).second.name.c_str());
    }
    const std::vector<AB::Entities::MailHeader>& GetCurrentMailHeaders() const
    {
        return mailHeaders_;
    }
    const AB::Entities::Mail& GetCurrentMail() const
    {
        return currentMail_;
    }
    int GetAvgPing() const
    {
        return client_.GetAvgPing();
    }
    int GetLastPing() const
    {
        return client_.GetLastPing();
    }
    uint32_t GetPlayerId() const
    {
        return playerId_;
    }

    String accountName_;
    String accountPass_;
};

