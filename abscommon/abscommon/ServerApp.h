#pragma once

#include <AB/Entities/Service.h>
#include "UuidUtils.h"

namespace IO {
class DataClient;
}

class ServerApp
{
protected:
    bool running_;
    std::string serverId_;
    AB::Entities::ServiceType serverType_;
    std::string serverName_;
    std::string serverLocation_;
    std::string configFile_;
    std::string logDir_;
    /// Get a generic currently unique server name
    std::string GetFreeName(IO::DataClient* client);
public:
    ServerApp() :
        running_(false),
        serverId_(Utils::Uuid::EMPTY_UUID),
        serverType_(AB::Entities::ServiceTypeUnknown),
        serverName_(""),
        serverLocation_("")
    { }
    virtual ~ServerApp() = default;
    virtual bool Initialize(int argc, char** argv);
    virtual void Run()
    { }
    virtual void Stop()
    { }
    /// Returns the Server UUID from the config file
    const std::string& GetServerId() const
    {
        return serverId_;
    }
    const std::string& GetServerName() const
    {
        return serverName_;
    }

    std::string path_;
    std::string exeFile_;
    std::vector<std::string> arguments_;
};
