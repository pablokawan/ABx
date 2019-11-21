include("/scripts/includes/skill_consts.lua")

local function useSkill(action)
  local result = node(action)
    result:SetCondition(condition("IsInSkillRange"))
  return result
end

local function aoeDamage()
  local result = node("UseDamageSkill", { SkillTargetAoe })
    local cond = condition("Filter")
      cond:SetFilter(filter("SelectMob"))

    result:SetCondition(cond)
  return result
end

local function singleDamage()
  local result = node("UseDamageSkill", { SkillTargetTarget })

    local cond = condition("Filter")
      cond:SetFilter(filter("SelectAttackTarget"))

    result:SetCondition(cond)
  return result
end

local function interruptSpell()
  local result = node("Interrupt", { SkillTypeSpell })

    local cond = condition("Filter")
      cond:SetFilter(filter("SelectTargetUsingSkill", { SkillTypeSpell }))

    result:SetCondition(cond)
  return result
end

local function interruptAttack()
  local result = node("Interrupt", { SkillTypeAttack })

    local orCond = condition("Or")
      local skillCond = condition("Filter")
        skillCond:SetFilter(filter("SelectTargetUsingSkill", { SkillTypeAttack }))
      orCond:AddCondition(skillCond)

      local attackCond = condition("Filter")
        attackCond:SetFilter(filter("SelectTargetAttacking"))
      orCond:AddCondition(attackCond)

    result:SetCondition(orCond)
  return result
end

local function avoidSelfMeleeDamage()
  -- Dodge melee attacks
  local nd = node("Flee")
    nd:SetCondition(condition("IsMeleeTarget"))
  return nd
end

local function avoidSelfAoeDamage()
  -- Move out of AOE
  local nd = node("MoveOutAOE")
    nd:SetCondition(condition("IsInAOE"))
  return nd
end

function damageSkill()
  local result = node("Priority")
    result:AddNode(aoeDamage())
    result:AddNode(interruptSpell())
    result:AddNode(interruptAttack())
    result:AddNode(singleDamage())

    -- If out of range move to target
    local move = node("MoveTo")
      -- Only move there when not in range
      local notinrange = condition("Not")
        notinrange:AddCondition(condition("IsInSkillRange"))

      move:SetCondition(notinrange)
    result:AddNode(move)

  return result
end

function checkEnergy()
  local result = node("GainEnergy")
    result:SetCondition(condition("IsEnergyLow"))
  return result
end

function idle(time)
  return node("Idle", { time })
end

function goHome()
  local nd = node("GoHome")
    nd:SetCondition(condition("HaveHome"))
  return nd
end

function wander()
  local nd = node("Wander")
    nd:SetCondition(condition("HaveWanderRoute"))
  return nd
end

function avoidSelfDamage()
  local nd = node("Priority")
    nd:AddNode(avoidSelfMeleeDamage())
    nd:AddNode(avoidSelfAoeDamage())
  return nd
end

function stayAlive()
  -- Execute the first child that does not fail
  local nd = node("Priority")
    local cond = condition("IsSelfHealthLow")
    -- If we have low HP
    nd:SetCondition(cond)
    -- 1. try to heal
    nd:AddNode(node("HealSelf"))
    -- 2. If that fails, flee
    nd:AddNode(node("Flee"))
  return nd
end

function defend()
  local nd = node("AttackSelection")
    -- If we are getting attacked AND there is an attacker
    local andCond = condition("And")
      andCond:AddCondition(condition("IsAttacked"))
      local haveAttackers = condition("Filter")
        haveAttackers:SetFilter(filter("SelectAttackers"))
      andCond:AddCondition(haveAttackers)
    nd:SetCondition(andCond)
  return nd
end

function healAlly()
  -- Priority: Execute the first child that does not fail, either HealOther or MoveTo
  local nd = node("Priority")
    local andCond = condition("And")
      andCond:AddCondition(condition("IsAllyHealthLow"))
      local haveTargets = condition("Filter")
        haveTargets:SetFilter(filter("SelectLowHealth"))
      andCond:AddCondition(haveTargets)
    nd:SetCondition(andCond)
    -- Heal fails if out of range
    local heal = useSkill("HealOther")
    nd:AddNode(heal)

    -- If out of range move to target
    local move = node("MoveTo")
      -- Only move there when not in range
      local notinrange = condition("Not")
        notinrange:AddCondition(condition("IsInSkillRange"))

      move:SetCondition(notinrange)

    -- If out of range move to target
    nd:AddNode(move)

  return nd
end

function attackAggro()
  -- Melee attack without skills
  local nd = node("AttackSelection")
    local haveAggro = condition("Filter")
      haveAggro:SetFilter(filter("SelectAggro"))
    nd:SetCondition(haveAggro)
  return nd
end

function rezzAlly()
  local nd = node("Priority")
    local haveDeadAllies = condition("Filter")
      haveDeadAllies:SetFilter(filter("SelectDeadAllies"))
    nd:SetCondition(haveDeadAllies)

    local rezz = useSkill("ResurrectSelection")
    nd:AddNode(rezz)

    -- If out of range move to target
    local move = node("MoveTo")
      -- Only move there when not in range
      local notinrange = condition("Not")
        notinrange:AddCondition(condition("IsInSkillRange"))

      move:SetCondition(notinrange)
    nd:AddNode(move)

  return nd
end
