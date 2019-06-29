#pragma once

#include "Actor.h"
#include "Script.h"
#include "Ray.h"

namespace Game {

class Item;

/// Projectile is an Actor and has an Item. Both have a script. This is similar to ItemDrop:
/// it is a GameObject and wraps an Item.
class Projectile : public Actor
{
private:
    std::unique_ptr<Item> item_;
    enum Function : uint32_t
    {
        FunctionNone = 0,
        FunctionUpdate = 1,
        FunctionOnCollide = 1 << 1,
        FunctionOnHitTarget = 1 << 2,
        FunctionOnStart = 1 << 3,
    };
    kaguya::State luaState_;
    bool luaInitialized_;
    bool startSet_;
    std::shared_ptr<Script> script_;
    Math::Vector3 start_;
    Math::Vector3 targetPos_;
    Math::Ray ray_;
    float distance_{ std::numeric_limits<float>::max() };
    std::weak_ptr<Actor> source_;
    std::weak_ptr<Actor> target_;
    float speed_{ 0.0f };
    bool started_{ false };
    uint32_t functions_{ FunctionNone };
    void InitializeLua();
    bool HaveFunction(Function func) const
    {
        return (functions_ & func) == func;
    }
public:
    static void RegisterLua(kaguya::State& state);

    explicit Projectile(std::unique_ptr<Item>& item);
    Projectile() = delete;
    // non-copyable
    Projectile(const Projectile&) = delete;
    Projectile& operator=(const Projectile&) = delete;

    bool LoadScript(const std::string& fileName);
    AB::GameProtocol::GameObjectType GetType() const final override
    {
        return AB::GameProtocol::ObjectTypeProjectile;
    }
    void SetSource(std::shared_ptr<Actor> source);
    void SetTarget(std::shared_ptr<Actor> target);
    void SetSpeed(float speed);
    float GetSpeed() const { return speed_; }
    void Update(uint32_t timeElapsed, Net::NetworkMessage& message) override;

    void OnCollide(GameObject* other) override;
    bool OnStart();
    Item* GetItem() const;

    bool Serialize(IO::PropWriteStream& stream) override;
    void WriteSpawnData(Net::NetworkMessage& msg) override;
};

}
