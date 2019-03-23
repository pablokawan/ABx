#include "stdafx.h"
#include "Item.h"
#include "DataProvider.h"
#include "ScriptManager.h"
#include "IOItem.h"
#include "Mechanic.h"
#include "ItemFactory.h"
#include "Subsystems.h"

namespace Game {

void Item::RegisterLua(kaguya::State& state)
{
    state["Item"].setClass(kaguya::UserdataMetatable<Item>()
        .addFunction("GetWeaponRange", &Item::GetWeaponRange)
        .addFunction("GetWeaponAttackSpeed", &Item::GetWeaponAttackSpeed)
        .addFunction("GetWeaponDamageType", &Item::GetWeaponDamageType)
        .addFunction("GetWeaponDamage", &Item::GetWeaponDamage)
    );
}

void Item::InitializeLua()
{
    ScriptManager::RegisterLuaAll(luaState_);
    luaState_["self"] = this;
}

bool Item::LoadConcrete(const AB::Entities::ConcreteItem& item)
{
    concreteItem_ = item;
    IO::PropReadStream stream;
    stream.Init(item.itemStats.data(), item.itemStats.length());
    if (!stats_.Load(stream))
    {
        LOG_WARNING << "Error loading item stats" << std::endl;
    }
    baseMinDamage_ = stats_.GetMinDamage();
    baseMaxDamage_ = stats_.GetMaxDamage();
    auto itemFactory = GetSubsystem<ItemFactory>();
    if (!item.upgrade1Uuid.empty() && !uuids::uuid(item.upgrade1Uuid).nil())
        upgrades_[ItemUpgrade::Pefix] = std::move(itemFactory->LoadConcrete(item.upgrade1Uuid));
    if (!item.upgrade2Uuid.empty() && !uuids::uuid(item.upgrade2Uuid).nil())
        upgrades_[ItemUpgrade::Suffix] = std::move(itemFactory->LoadConcrete(item.upgrade2Uuid));
    if (!item.upgrade3Uuid.empty() && !uuids::uuid(item.upgrade3Uuid).nil())
        upgrades_[ItemUpgrade::Inscription] = std::move(itemFactory->LoadConcrete(item.upgrade3Uuid));

    return true;
}

bool Item::LoadScript(const std::string& fileName)
{
    script_ = GetSubsystem<IO::DataProvider>()->GetAsset<Script>(fileName);
    if (!script_)
        return false;
    if (!script_->Execute(luaState_))
        return false;

    if (ScriptManager::IsFunction(luaState_, "onUpdate"))
        functions_ |= FunctionUpdate;
    if (ScriptManager::IsFunction(luaState_, "getDamage"))
        functions_ |= FunctionGetDamage;
    if (ScriptManager::IsFunction(luaState_, "getDamageType"))
        functions_ |= FunctionGetDamageType;
    return true;
}

void Item::CreateWeaponStats(uint32_t level)
{
    if (ScriptManager::IsFunction(luaState_, "getDamageStats"))
    {
        int32_t minDamage = 0;
        int32_t maxDamage = 0;
        kaguya::tie(minDamage, maxDamage) = luaState_["getDamageStats"](level);
        stats_.SetValue(Stat::MinDamage, minDamage);
        stats_.SetValue(Stat::MaxDamage, maxDamage);
    }
}

void Item::CreateFocusStats(uint32_t level)
{
    if (ScriptManager::IsFunction(luaState_, "getEnergyStats"))
    {
        int32_t energy = luaState_["getEnergyStats"](level);
        stats_.SetValue(Stat::Energy, energy);
    }
}

void Item::CreateShieldStats(uint32_t level)
{
    if (ScriptManager::IsFunction(luaState_, "getArmorStats"))
    {
        int32_t armor = luaState_["getArmorStats"](level);
        stats_.SetValue(Stat::Armor, armor);
    }
}

bool Item::GenerateConcrete(AB::Entities::ConcreteItem& ci, uint32_t level)
{
    concreteItem_ = ci;

    switch (data_.type)
    {
    case AB::Entities::ItemTypeAxe:
    case AB::Entities::ItemTypeSword:
    case AB::Entities::ItemTypeWand:
    case AB::Entities::ItemTypeSpear:
    case AB::Entities::ItemTypeHammer:
    case AB::Entities::ItemTypeFlatbow:
    case AB::Entities::ItemTypeHornbow:
    case AB::Entities::ItemTypeShortbow:
    case AB::Entities::ItemTypeLongbow:
    case AB::Entities::ItemTypeRecurvebow:
    case AB::Entities::ItemTypeStaff:
    case AB::Entities::ItemTypeDaggers:
    case AB::Entities::ItemTypeScyte:
        CreateWeaponStats(level);
    case AB::Entities::ItemTypeFocus:
        CreateFocusStats(level);
    case AB::Entities::ItemTypeShield:
        CreateShieldStats(level);
    default:
        break;
    }

    IO::PropWriteStream stream;
    stats_.Save(stream);
    size_t ssize = 0;
    const char* s = stream.GetStream(ssize);
    concreteItem_.itemStats = std::string(s, ssize);

    baseMinDamage_ = stats_.GetMinDamage();
    baseMaxDamage_ = stats_.GetMaxDamage();
    return true;
}

void Item::Update(uint32_t timeElapsed)
{
    if (HaveFunction(FunctionUpdate))
        luaState_["onUpdate"](timeElapsed);
    for (const auto& upg : upgrades_)
        if (upg.second)
            upg.second->Update(timeElapsed);
}

void Item::SetUpgrade(ItemUpgrade type, uint32_t index)
{
    AB::Entities::Item item;
    if (!IO::IOItem::LoadItemByIndex(item, index))
    {
        LOG_ERROR << "Failed to load item with index " << index << std::endl;
        return;
    }
    std::unique_ptr<Item> i = std::make_unique<Item>(item);
    if (i->LoadScript(item.script))
    {
        upgrades_[type] = std::move(i);
    }
}

void Item::RemoveUpgrade(ItemUpgrade type)
{
    if (upgrades_[type])
        upgrades_[type].reset();
}

EquipPos Item::GetEquipPos() const
{
    switch (data_.type)
    {
    case AB::Entities::ItemTypeAxe:
    case AB::Entities::ItemTypeSword:
    case AB::Entities::ItemTypeWand:
    case AB::Entities::ItemTypeSpear:
        return EquipPos::WeaponLeadHand;
    case AB::Entities::ItemTypeHammer:
    case AB::Entities::ItemTypeFlatbow:
    case AB::Entities::ItemTypeHornbow:
    case AB::Entities::ItemTypeShortbow:
    case AB::Entities::ItemTypeLongbow:
    case AB::Entities::ItemTypeRecurvebow:
    case AB::Entities::ItemTypeStaff:
    case AB::Entities::ItemTypeDaggers:
    case AB::Entities::ItemTypeScyte:
        return EquipPos::WeaponTwoHanded;
    case AB::Entities::ItemTypeFocus:
    case AB::Entities::ItemTypeShield:
        return EquipPos::WeaponOffHand;
    default:
        return EquipPos::None;
    }
}

float Item::GetWeaponRange() const
{
    switch (data_.type)
    {
    case AB::Entities::ItemTypeAxe:
    case AB::Entities::ItemTypeSword:
    case AB::Entities::ItemTypeHammer:
        return RANGE_TOUCH;
    case AB::Entities::ItemTypeFlatbow:
        return RANGE_FLATBOW;
    case AB::Entities::ItemTypeHornbow:
        return RANGE_HORNBOW;
    case AB::Entities::ItemTypeShortbow:
        return RANGE_SHORTBOW;
    case AB::Entities::ItemTypeLongbow:
        return RANGE_LONGBOW;
    case AB::Entities::ItemTypeRecurvebow:
        return RANGE_RECURVEBOW;
    case AB::Entities::ItemTypeStaff:
    case AB::Entities::ItemTypeWand:
        return RANGE_PROJECTILE;
    case AB::Entities::ItemTypeDaggers:
        return RANGE_TOUCH;
    case AB::Entities::ItemTypeScyte:
        return RANGE_TOUCH;
    case AB::Entities::ItemTypeSpear:
        return RANGE_SPEAR;
    default:
        return 0.0f;
    }
}

uint32_t Item::GetWeaponAttackSpeed() const
{
    switch (data_.type)
    {
    case AB::Entities::ItemTypeAxe:
        return ATTACK_SPEED_AXE;
    case AB::Entities::ItemTypeSword:
        return ATTACK_SPEED_SWORD;
    case AB::Entities::ItemTypeHammer:
        return ATTACK_SPEED_HAMMER;
    case AB::Entities::ItemTypeFlatbow:
        return ATTACK_SPEED_FLATBOW;
    case AB::Entities::ItemTypeHornbow:
        return ATTACK_SPEED_HORNBOW;
    case AB::Entities::ItemTypeShortbow:
        return ATTACK_SPEED_SHORTBOW;
    case AB::Entities::ItemTypeLongbow:
        return ATTACK_SPEED_LONGBOW;
    case AB::Entities::ItemTypeRecurvebow:
        return ATTACK_SPEED_RECURVEBOW;
    case AB::Entities::ItemTypeStaff:
        return ATTACK_SPEED_STAFF;
    case AB::Entities::ItemTypeWand:
        return ATTACK_SPEED_WAND;
    case AB::Entities::ItemTypeDaggers:
        return ATTACK_SPEED_DAGGERS;
    case AB::Entities::ItemTypeScyte:
        return ATTACK_SPEED_SCYTE;
    case AB::Entities::ItemTypeSpear:
        return ATTACK_SPEED_SPEAR;
    default:
        return 0;
    }
}

void Item::GetWeaponDamageType(DamageType& value) const
{
    value = stats_.GetDamageType();
    for (const auto& upg : upgrades_)
        if (upg.second)
            upg.second->GetWeaponDamageType(value);

    if (value != DamageType::Unknown)
        return;

    // Default weapon damage type
    switch (data_.type)
    {
    case AB::Entities::ItemTypeAxe:
        value = DamageType::Piercing;
        break;
    case AB::Entities::ItemTypeSword:
        value = DamageType::Slashing;
        break;
    case AB::Entities::ItemTypeHammer:
        value = DamageType::Blunt;
        break;
    case AB::Entities::ItemTypeFlatbow:
    case AB::Entities::ItemTypeHornbow:
    case AB::Entities::ItemTypeShortbow:
    case AB::Entities::ItemTypeLongbow:
    case AB::Entities::ItemTypeRecurvebow:
        value = DamageType::Piercing;
        break;
    case AB::Entities::ItemTypeStaff:
    case AB::Entities::ItemTypeWand:
        value = DamageType::Slashing;
        break;
    case AB::Entities::ItemTypeDaggers:
        value = DamageType::Piercing;
        break;
    case AB::Entities::ItemTypeScyte:
        value = DamageType::Slashing;
        break;
    case AB::Entities::ItemTypeSpear:
        value = DamageType::Piercing;
        break;
    default:
        value = DamageType::Unknown;
        break;
    }
}

AttributeIndices Item::GetWeaponAttribute() const
{
    auto ret = stats_.GetAttribute();
    if (ret != AttributeIndices::None)
        return ret;

    // Default weapon attributes
    switch (data_.type)
    {
    case AB::Entities::ItemTypeAxe:
        return AttributeIndices::AxeMatery;
    case AB::Entities::ItemTypeSword:
        return AttributeIndices::SwordsManship;
    case AB::Entities::ItemTypeHammer:
        return AttributeIndices::HammerMastery;
    case AB::Entities::ItemTypeFlatbow:
    case AB::Entities::ItemTypeHornbow:
    case AB::Entities::ItemTypeShortbow:
    case AB::Entities::ItemTypeLongbow:
    case AB::Entities::ItemTypeRecurvebow:
        return AttributeIndices::MarkMansship;
    default:
        return AttributeIndices::None;
    }
}

uint32_t Item::GetWeaponRequirement() const
{
    return stats_.GetRequirement();
}

void Item::GetArmor(DamageType damageType, int& value) const
{
    value = stats_.GetArmor(damageType);

    for (const auto& upg : upgrades_)
        if (upg.second)
            upg.second->GetArmor(damageType, value);
}

void Item::GetArmorPenetration(float& value) const
{
    switch (data_.type)
    {
    case AB::Entities::ItemTypeHornbow:
        value += 0.1f;
    }
    for (const auto& upg : upgrades_)
        if (upg.second)
            upg.second->GetArmorPenetration(value);
}

void Item::GetAttributeValue(uint32_t index, uint32_t& value)
{
    // Equipment (e.g. runes) may increase the attributes
    value += stats_.GetAttributeIncrease(index);
    for (const auto& upg : upgrades_)
        if (upg.second)
            upg.second->GetAttributeValue(index, value);
}

void Item::GetWeaponDamage(int32_t& value, bool critical)
{
    if (HaveFunction(FunctionGetDamage))
    {
        value = luaState_["getDamage"](baseMinDamage_, baseMaxDamage_, critical);
    }
    for (const auto& upg : upgrades_)
        if (upg.second)
            upg.second->GetWeaponDamage(value, critical);
}

}
