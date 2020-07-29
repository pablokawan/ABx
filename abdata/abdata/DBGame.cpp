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

#include "DBGame.h"
#include <sa/TemplateParser.h>

namespace DB {

static std::string PlaceholderCallback(Database* db, const AB::Entities::Game& game, const sa::templ::Token& token)
{
    switch (token.type)
    {
    case sa::templ::Token::Type::Variable:
        if (token.value == "uuid")
            return db->EscapeString(game.uuid);
        if (token.value == "name")
            return db->EscapeString(game.name);

        LOG_WARNING << "Unhandled placeholder " << token.value << std::endl;
        return "";
    default:
        return token.value;
    }
}

bool DBGame::Create(AB::Entities::Game&)
{
    // Do nothing
    return true;
}

bool DBGame::Load(AB::Entities::Game& game)
{
    Database* db = GetSubsystem<Database>();

    sa::templ::Parser parser;
    sa::templ::Tokens tokens = parser.Parse("SELECT * FROM game_maps WHERE ");
    if (!Utils::Uuid::IsEmpty(game.uuid))
        parser.Append("uuid = ${uuid}", tokens);
    else if (!game.name.empty())
        parser.Append("name = ${name}", tokens);
    else
    {
        LOG_ERROR << "UUID and name are empty" << std::endl;
        return false;
    }

    const std::string query = tokens.ToString(std::bind(&PlaceholderCallback, db, game, std::placeholders::_1));

    std::shared_ptr<DB::DBResult> result = db->StoreQuery(query);
    if (!result)
        return false;

    game.uuid = result->GetString("uuid");
    game.name = result->GetString("name");
    game.type = static_cast<AB::Entities::GameType>(result->GetUInt("type"));
    game.mode = static_cast<AB::Entities::GameMode>(result->GetUInt("game_mode"));
    game.directory = result->GetString("directory");
    game.script = result->GetString("script_file");
    game.queueMapUuid = result->GetString("queue_map_uuid");
    game.landing = result->GetUInt("landing") != 0;
    game.partySize = static_cast<uint8_t>(result->GetUInt("party_size"));
    game.partyCount = static_cast<uint8_t>(result->GetUInt("party_count"));
    game.randomParty = result->GetUInt("random_party") != 0;
    game.mapCoordX = result->GetInt("map_coord_x");
    game.mapCoordY = result->GetInt("map_coord_y");
    game.defaultLevel = static_cast<int8_t>(result->GetInt("default_level"));

    return true;
}

bool DBGame::Save(const AB::Entities::Game&)
{
    // Do nothing
    return true;
}

bool DBGame::Delete(const AB::Entities::Game&)
{
    // Do nothing
    return true;
}

bool DBGame::Exists(const AB::Entities::Game& game)
{
    Database* db = GetSubsystem<Database>();

    sa::templ::Parser parser;
    sa::templ::Tokens tokens = parser.Parse("SELECT COUNT(*) AS count FROM game_maps WHERE ");
    if (!Utils::Uuid::IsEmpty(game.uuid))
        parser.Append("uuid = ${uuid}", tokens);
    else if (!game.name.empty())
        parser.Append("name = ${name}", tokens);
    else
    {
        LOG_ERROR << "UUID and name are empty" << std::endl;
        return false;
    }

    const std::string query = tokens.ToString(std::bind(&PlaceholderCallback, db, game, std::placeholders::_1));

    std::shared_ptr<DB::DBResult> result = db->StoreQuery(query);
    if (!result)
        return false;
    return result->GetUInt("count") != 0;
}

}
