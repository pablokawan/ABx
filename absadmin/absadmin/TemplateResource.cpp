/**
 * Copyright 2017-2020 Stefan Ascher
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "TemplateResource.h"
#include "Application.h"
#include "ContentTypes.h"
#include "Version.h"
#include <sa/StringHash.h>
#include <sa/Assert.h>
#include <AB/Entities/Account.h>

namespace Resources {

static std::string ReadFile(const std::string& fileName)
{
    std::ifstream f(fileName);
    if (!f.is_open())
    {
        LOG_ERROR << "Unable to open file " << fileName << std::endl;
        return "";
    }

    std::string line;
    std::stringstream ss;
    while (std::getline(f, line))
    {
        ss << line << '\n';
    }
    f.close();
    return ss.str();
}

bool TemplateResource::GetContext(LuaContext& objects)
{
    ASSERT(Application::Instance);
    kaguya::State& state = objects.GetState();

    state["title"] = Utils::XML::Escape(Application::Instance->GetServerName());
    state["copy_year"] = SERVER_YEAR;
    auto it = session_->values_.find(sa::StringHashRt("username"));
    if (it != session_->values_.end())
    {
        state["user"] = Utils::XML::Escape((*it).second.GetString());
    }
    else
        state["user"] = "";
    auto accIt = session_->values_.find(sa::StringHashRt("account_type"));
    AB::Entities::AccountType accType = AB::Entities::AccountType::Unknown;
    if (accIt != session_->values_.end())
        accType = static_cast<AB::Entities::AccountType>((*accIt).second.GetInt());
    state["is_user"] = accType >= AB::Entities::AccountType::Normal;
    state["is_tutor"] = accType >= AB::Entities::AccountType::Tutor;
    state["is_sentutor"] = accType >= AB::Entities::AccountType::SeniorTutor;
    state["is_gm"] = accType >= AB::Entities::AccountType::Gamemaster;
    state["is_god"] = accType >= AB::Entities::AccountType::God;

    state["styles"] = styles_;
    state["header_scripts"] = headerScripts_;
    state["footer_scripts"] = footerScripts_;

    return true;
}

TemplateResource::TemplateResource(std::shared_ptr<HttpsServer::Request> request) :
    Resource(request)
{
    styles_.push_back("vendors/bootstrap/dist/css/bootstrap.min.css");
    styles_.push_back("vendors/font-awesome/css/font-awesome.min.css");
    styles_.push_back("vendors/nprogress/nprogress.css");
    styles_.push_back("css/custom.less");

    headerScripts_.push_back("vendors/jquery/dist/jquery.min.js");
    footerScripts_.push_back("vendors/bootstrap/dist/js/bootstrap.min.js");
    footerScripts_.push_back("vendors/nprogress/nprogress.js");
    footerScripts_.push_back("js/common.js");
}

void TemplateResource::LoadTemplates(std::string& result)
{
    std::string body = GetTemplateFile(template_);
    if (body.empty())
        return;

    const std::string path = Utils::AddSlash(Utils::ExtractFileDir(template_));

    std::ifstream ifs(body, std::ifstream::in | std::ios::binary);
    if (!ifs)
    {
        LOG_ERROR << "Unable to open file " << body << std::endl;
        throw std::invalid_argument("Error opening file");
    }

    sa::lpp::Tokenizer t;
    t.onGetFile_ = [this, &path](const std::string& f) -> std::string
    {
        return ReadFile(GetTemplateFile(path + f));
    };
    const std::string contents = ReadFile(body);
    const sa::lpp::Tokens tokens = t.Parse(contents);
    std::stringstream ss;
    sa::lpp::Generate(tokens, [&ss](const std::string& value)
    {
        ss << value;
    });
    result = ss.str();
}

std::string TemplateResource::GetTemplateFile(const std::string& templ)
{
    if (templ.empty())
        return "";

    const std::string& root = Application::Instance->GetRoot();

    auto web_root_path = fs::canonical(root);
    auto path = fs::canonical(web_root_path / templ);
    if (fs::is_directory(path))
    {
        LOG_ERROR << request_->remote_endpoint_address() << ":" << request_->remote_endpoint_port() << ": "
            << "Trying to access a directory " << path.string() << std::endl;
        throw std::invalid_argument("not a file");
    }
    if (Utils::IsHiddenFile(path.string()))
    {
        LOG_ERROR << request_->remote_endpoint_address() << ":" << request_->remote_endpoint_port() << ": "
            << "Trying to access a hidden file " << path.string() << std::endl;
        throw std::invalid_argument("hidden file");
    }

    return path.string();
}

void TemplateResource::Render(std::shared_ptr<HttpsServer::Response> response)
{
    AB_PROFILE;
    SimpleWeb::CaseInsensitiveMultimap header = Application::GetDefaultHeader();
    auto contT = GetSubsystem<ContentTypes>();

    int64_t start = Utils::Tick();
    std::string buffer;
    try
    {
        LoadTemplates(buffer);
    }
    catch (const std::exception& ex)
    {
        LOG_ERROR << "Template Load Error: " << ex.what() << std::endl;
        response->write(SimpleWeb::StatusCode::server_error_internal_server_error,
            "Internal Server Error " + request_->path);
        return;
    }

    // Don't cache templates
    header.emplace("Cache-Control", "no-cache, no-store, must-revalidate");
    responseCookies_->Write(header);
    auto ct = contT->Get(Utils::GetFileExt(template_));
    header.emplace("Content-Type", ct);

    LuaContext context(*this, &header);

    if (!GetContext(context))
    {
        LOG_ERROR << "Failed to get objects" << std::endl;
        response->write(SimpleWeb::StatusCode::client_error_not_found,
            "Not found " + request_->path);
        return;
    }

    if (context.Execute(buffer))
    {
        std::string realCt = ct;
        const auto it = header.find("Content-Type");
        if (it != header.end())
            realCt = (*it).second;

        std::stringstream& ss = context.GetStream();
        if (realCt == "text/html")
            ss << "\n<!-- Generated in " << Utils::TimeElapsed(start) << "ms -->\n";

        ss.seekg(0, std::ios::end);
        size_t ssize = ss.tellg();
        ss.seekg(0, std::ios::beg);
        header.emplace("Content-Length", std::to_string(ssize));
        response->write(ss, header);
    }
    else
    {
        LOG_ERROR << "Execution Error" << std::endl;
        response->write(SimpleWeb::StatusCode::server_error_internal_server_error,
            "Internal Server Error " + request_->path);

    }
}

HTTP::Session* TemplateResource::GetSession() const
{
    return session_.get();
}

HTTP::Cookies* TemplateResource::GetRequestCookies() const
{
    return requestCookies_.get();
}

HTTP::Cookies* TemplateResource::GetResponseCookies() const
{
    return responseCookies_.get();
}

}
