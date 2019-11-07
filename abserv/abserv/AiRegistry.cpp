#include "stdafx.h"
#include "AiRegistry.h"
#include "actions/AiAttackSelection.h"
#include "actions/AiDie.h"
#include "actions/AiIdle.h"
#include "actions/AiSay.h"
#include "actions/AiGoHome.h"
#include "actions/AiMoveTo.h"
#include "actions/AiHealSelf.h"
#include "actions/AiHealOther.h"
#include "actions/AiResurrectSelection.h"
#include "actions/AiGainEnergy.h"
#include "conditions/AiIsSelectionAlive.h"
#include "conditions/AiIsCloseToSelection.h"
#include "conditions/AiIsSelfHealthLow.h"
#include "conditions/AiIsSelfHealthCritical.h"
#include "conditions/AiIsAllyHealthLow.h"
#include "conditions/AiIsAllyHealthCritical.h"
#include "conditions/AiIsAttacked.h"
#include "conditions/AiIsEnergyLow.h"
#include "filters/AiSelectVisible.h"
#include "filters/AiSelectAggro.h"
#include "filters/AiSelectLowHealth.h"
#include "filters/AiSelectAttackers.h"
#include "filters/AiSelectDeadAllies.h"
#include "filters/AiSelectRandom.h"
#include "filters/AiSelectAttackTarget.h"

namespace AI {

AiRegistry::AiRegistry() :
    Registry()
{ }

void AiRegistry::Initialize()
{
    Registry::Initialize();

    // We don't use Zones
    UnregisterFilterFactory("Zone");

    RegisterNodeFactory("GoHome", Actions::GoHome::GetFactory());
    RegisterNodeFactory("Die", Actions::Die::GetFactory());
    RegisterNodeFactory("Idle", Actions::Idle::GetFactory());
    RegisterNodeFactory("AttackSelection", Actions::AttackSelection::GetFactory());
    RegisterNodeFactory("HealOther", Actions::HealOther::GetFactory());
    RegisterNodeFactory("HealSelf", Actions::HealSelf::GetFactory());
    RegisterNodeFactory("Say", Actions::Say::GetFactory());
    RegisterNodeFactory("MoveTo", Actions::MoveTo::GetFactory());
    RegisterNodeFactory("ResurrectSelection", Actions::ResurrectSelection::GetFactory());
    RegisterNodeFactory("GainEnergy", Actions::GainEnergy::GetFactory());

    RegisterConditionFactory("IsSelectionAlive", Conditions::IsSelectionAlive::GetFactory());
    RegisterConditionFactory("IsCloseToSelection", Conditions::IsCloseToSelection::GetFactory());
    RegisterConditionFactory("IsSelfHealthLow", Conditions::IsSelfHealthLow::GetFactory());
    RegisterConditionFactory("SelfHealthCritical", Conditions::IsSelfHealthCritical::GetFactory());
    RegisterConditionFactory("IsAllyHealthLow", Conditions::IsAllyHealthLow::GetFactory());
    RegisterConditionFactory("AllyHealthCritical", Conditions::IsAllyHealthCritical::GetFactory());
    RegisterConditionFactory("IsAttacked", Conditions::IsAttacked::GetFactory());
    RegisterConditionFactory("IsEnergyLow", Conditions::IsEnergyLow::GetFactory());

    RegisterFilterFactory("SelectVisible", Filters::SelectVisible::GetFactory());
    RegisterFilterFactory("SelectAggro", Filters::SelectAggro::GetFactory());
    RegisterFilterFactory("SelectLowHealth", Filters::SelectLowHealth::GetFactory());
    RegisterFilterFactory("SelectAttackers", Filters::SelectAttackers::GetFactory());
    RegisterFilterFactory("SelectDeadAllies", Filters::SelectDeadAllies::GetFactory());
    RegisterFilterFactory("SelectRandom", Filters::SelectRandom::GetFactory());
    RegisterFilterFactory("SelectAttackTarget", Filters::SelectAttackTarget::GetFactory());
}

}
