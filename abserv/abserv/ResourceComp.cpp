#include "stdafx.h"
#include "ResourceComp.h"
#include <AB/ProtocolCodes.h>
#include "Actor.h"

namespace Game {
namespace Components {

void ResourceComp::SetHealth(SetValueType t, int16_t value)
{
    if (SetValue(t, value, health_))
        dirtyFlags_ |= ResourceDirty::DirtyHealth;
}

void ResourceComp::SetEnergy(SetValueType t, int16_t value)
{
    if (SetValue(t, value, energy_))
        dirtyFlags_ |= ResourceDirty::DirtyEnergy;
}

void ResourceComp::SetAdrenaline(SetValueType t, int16_t value)
{
    if (SetValue(t, value, adrenaline_))
        dirtyFlags_ |= ResourceDirty::DirtyAdrenaline;
}

void ResourceComp::SetOvercast(SetValueType t, int16_t value)
{
    if (SetValue(t, value, overcast_))
        dirtyFlags_ |= ResourceDirty::DirtyOvercast;
}

void ResourceComp::SetHealthRegen(SetValueType t, int8_t value)
{
    if (SetValue(t, value, healthRegen_))
        dirtyFlags_ |= ResourceDirty::DirtyHealthRegen;
}

void ResourceComp::SetEnergyRegen(SetValueType t, int8_t value)
{
    if (SetValue(t, value, energyRegen_))
        dirtyFlags_ |= ResourceDirty::DirtyEnergyRegen;
}

void ResourceComp::SetMaxHealth(int16_t value)
{
    if (maxHealth_ != value)
    {
        maxHealth_ = value;
        dirtyFlags_ |= ResourceDirty::DirtyMaxHealth;
    }
}

void ResourceComp::SetMaxEnergy(int16_t value)
{
    if (maxEnergy_ != value)
    {
        maxEnergy_ = value;
        dirtyFlags_ |= ResourceDirty::DirtyMaxEnergy;
    }
}

void ResourceComp::Update(uint32_t timeElapsed)
{
    SetValue(SetValueType::Increase, healthRegen_ * timeElapsed, health_);
    SetValue(SetValueType::Increase, energyRegen_ * timeElapsed, energy_);
}

void ResourceComp::Write(Net::NetworkMessage& message, bool ignoreDirty /* = false */)
{
    if (!ignoreDirty && dirtyFlags_ == 0)
        return;

    if (ignoreDirty || (dirtyFlags_ & ResourceDirty::DirtyHealth) == ResourceDirty::DirtyHealth)
    {
        message.AddByte(AB::GameProtocol::GameObjectResourceChange);
        message.Add<uint32_t>(owner_.id_);
        message.AddByte(AB::GameProtocol::ResourceTypeHealth);
        message.Add<int16_t>(static_cast<int16_t>(health_));
    }
    if (ignoreDirty || (dirtyFlags_ & ResourceDirty::DirtyEnergy) == ResourceDirty::DirtyEnergy)
    {
        message.AddByte(AB::GameProtocol::GameObjectResourceChange);
        message.Add<uint32_t>(owner_.id_);
        message.AddByte(AB::GameProtocol::ResourceTypeEnergy);
        message.Add<int16_t>(static_cast<int16_t>(energy_));
    }
    if (ignoreDirty || (dirtyFlags_ & ResourceDirty::DirtyAdrenaline) == ResourceDirty::DirtyAdrenaline)
    {
        message.AddByte(AB::GameProtocol::GameObjectResourceChange);
        message.Add<uint32_t>(owner_.id_);
        message.AddByte(AB::GameProtocol::ResourceTypeAdrenaline);
        message.Add<int16_t>(static_cast<int16_t>(adrenaline_));
    }
    if (ignoreDirty || (dirtyFlags_ & ResourceDirty::DirtyOvercast) == ResourceDirty::DirtyOvercast)
    {
        message.AddByte(AB::GameProtocol::GameObjectResourceChange);
        message.Add<uint32_t>(owner_.id_);
        message.AddByte(AB::GameProtocol::ResourceTypeOvercast);
        message.Add<int16_t>(static_cast<int16_t>(overcast_));
    }
    if (ignoreDirty || (dirtyFlags_ & ResourceDirty::DirtyHealthRegen) == ResourceDirty::DirtyHealthRegen)
    {
        message.AddByte(AB::GameProtocol::GameObjectResourceChange);
        message.Add<uint32_t>(owner_.id_);
        message.AddByte(AB::GameProtocol::ResourceTypeHealthRegen);
        message.Add<int8_t>(static_cast<int8_t>(healthRegen_));
    }
    if (ignoreDirty || (dirtyFlags_ & ResourceDirty::DirtyEnergyRegen) == ResourceDirty::DirtyEnergyRegen)
    {
        message.AddByte(AB::GameProtocol::GameObjectResourceChange);
        message.Add<uint32_t>(owner_.id_);
        message.AddByte(AB::GameProtocol::ResourceTypeEnergyRegen);
        message.Add<int8_t>(static_cast<int8_t>(energyRegen_));
    }
    if (ignoreDirty || (dirtyFlags_ & ResourceDirty::DirtyMaxHealth) == ResourceDirty::DirtyMaxHealth)
    {
        message.AddByte(AB::GameProtocol::GameObjectResourceChange);
        message.Add<uint32_t>(owner_.id_);
        message.AddByte(AB::GameProtocol::ResourceTypeMaxHealth);
        message.Add<int16_t>(maxHealth_);
    }
    if (ignoreDirty || (dirtyFlags_ & ResourceDirty::DirtyMaxEnergy) == ResourceDirty::DirtyMaxEnergy)
    {
        message.AddByte(AB::GameProtocol::GameObjectResourceChange);
        message.Add<uint32_t>(owner_.id_);
        message.AddByte(AB::GameProtocol::ResourceTypeMaxEnergy);
        message.Add<int16_t>(maxEnergy_);
    }

    if (!ignoreDirty)
        dirtyFlags_ = 0;
}

}
}
