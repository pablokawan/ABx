#pragma once

#include "ServerApp.h"
#include "Service.h"
#include "DataClient.h"
#include "MessageClient.h"

constexpr int64_t QUEUE_UPDATE_INTERVAL_MS = 1000;

class Application final : public ServerApp
{
private:
    asio::io_service ioService_;
    int64_t lastUpdate_{ 0 };
    bool LoadMain();
    void PrintServerInfo();
    void ShowLogo();
    void UpdateQueue();
    void MainLoop();
    void HandleMessage(const Net::MessageMsg& msg);
    void HandleQueueAdd(const Net::MessageMsg& msg);
    void HandleQueueRemove(const Net::MessageMsg& msg);
protected:
    void ShowVersion() override;
public:
    Application();
    ~Application() override;

    bool Initialize(const std::vector<std::string>& args) override;
    void Run() override;
    void Stop() override;
};

