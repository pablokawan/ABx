include("/scripts/includes/consts.lua")
include("/scripts/includes/skill_consts.lua")

costEnergy = 0
costAdrenaline = 0
activation = 2000
recharge = 4000
overcast = 0

function onStartUse(source, target)
  return SkillErrorNone
end

function onSuccess(source, target)
  return SkillErrorNone
end
