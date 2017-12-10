#include "stdafx.h"
#include "ProtocolGame.h"
#include <AB/ProtocolCodes.h>
#include "Time.h"

namespace Client {

ProtocolGame::ProtocolGame() :
    Protocol(),
    enterWorldCallback_(nullptr),
    pingCallback_(nullptr)
{
    checksumEnabled_ = ProtocolGame::UseChecksum;
}

ProtocolGame::~ProtocolGame()
{
}

void ProtocolGame::Login(const std::string& accountName,
    const std::string& accountPass, const std::string& charName,
    const std::string& map,
    const std::string& host, uint16_t port, const EnterWorldCallback& callback)
{
    accountName_ = accountName;
    accountPass_ = accountPass;
    charName_ = charName;
    map_ = map;
    enterWorldCallback_ = callback;

    Connect(host, port);
}

void ProtocolGame::OnConnect()
{
    firstRevc_ = true;
    Protocol::OnConnect();

    SendLoginPacket();

    Receive();
}

void ProtocolGame::OnReceive(const std::shared_ptr<InputMessage>& message)
{
    if (firstRevc_)
    {
        firstRevc_ = false;
    }
    ParseMessage(message);
    Receive();
}

void ProtocolGame::OnError(const asio::error_code& err)
{
    Protocol::OnError(err);
}

void ProtocolGame::ParseMessage(const std::shared_ptr<InputMessage>& message)
{
    uint8_t opCode = 0;
    while (!message->Eof())
    {
        opCode = message->Get<uint8_t>();

        switch (opCode)
        {
        case AB::GameProtocol::Error:
            ParseError(message);
            break;
        case AB::GameProtocol::GameEnter:
            ParseEnterWorld(message);
            break;
        case AB::GameProtocol::GamePong:
            ParsePong(message);
            break;
        case AB::GameProtocol::GameUpdate:
            ParseUpdate(message);
            break;
        case AB::GameProtocol::GameSpawnObjectExisting:
        case AB::GameProtocol::GameSpawnObject:
            ParseSpawnObject(opCode == AB::GameProtocol::GameSpawnObjectExisting, message);
            break;
        case AB::GameProtocol::GameLeaveObject:
            ParseLeaveObject(message);
            break;
        }
    }
}

void ProtocolGame::ParseLeaveObject(const std::shared_ptr<InputMessage>& message)
{
    uint32_t objectId = message->Get<uint32_t>();
    if (despawnCallback_)
        despawnCallback_(objectId);
}

void ProtocolGame::ParseSpawnObject(bool existing, const std::shared_ptr<InputMessage>& message)
{
    uint32_t objectId = message->Get<uint32_t>();
    float x = message->Get<float>();
    float y = message->Get<float>();
    float z = message->Get<float>();
    float rot = message->Get<float>();
    std::string data = message->GetString();
    PropReadStream stream;
    stream.Init(data.c_str(), data.length());
    if (spawnCallback_)
        spawnCallback_(objectId, x, y, z, rot, stream, existing);
}

void ProtocolGame::ParseUpdate(const std::shared_ptr<InputMessage>& message)
{
    int64_t tick = message->Get<int64_t>();
}

void ProtocolGame::ParsePong(const std::shared_ptr<InputMessage>& message)
{
    AB_UNUSED(message);
    lastPing_ = static_cast<int>(AbTick() - pingTick_);
    if (pingCallback_)
        pingCallback_(lastPing_);
}

void ProtocolGame::ParseError(const std::shared_ptr<InputMessage>& message)
{
    uint8_t error = message->Get<uint8_t>();
    if (error != 0)
        ProtocolError(error);
}

void ProtocolGame::ParseEnterWorld(const std::shared_ptr<InputMessage>& message)
{
    std::string map = message->GetString();
    uint32_t playerId = message->Get<uint32_t>();
    if (enterWorldCallback_)
        enterWorldCallback_(map, playerId);
}

void ProtocolGame::SendLoginPacket()
{
    std::shared_ptr<OutputMessage> msg = std::make_shared<OutputMessage>();
    msg->Add<uint8_t>(ProtocolGame::ProtocolIdentifier);
    msg->Add<uint16_t>(1);   // Client OS
    msg->Add<uint16_t>(1);   // Client Version
    msg->AddString(accountName_);
    msg->AddString(accountPass_);
    msg->AddString(charName_);
    msg->AddString(map_);
    Send(msg);
}

void ProtocolGame::Logout()
{
    std::shared_ptr<OutputMessage> msg = std::make_shared<OutputMessage>();
    msg->Add<uint8_t>(AB::GameProtocol::PacketTypeLogout);
    Send(msg);
    Connection::Poll();
}

void ProtocolGame::Ping(const PingCallback& callback)
{
    pingCallback_ = callback;
    std::shared_ptr<OutputMessage> msg = std::make_shared<OutputMessage>();
    msg->Add<uint8_t>(AB::GameProtocol::PacketTypePing);
    pingTick_ = AbTick();
    Send(msg);
    Connection::Run();
}

}
