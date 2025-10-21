
function Skill_Rust_OnEnter(actor, action)

	return true
end

function Skill_Rust_OnUpdate(actor, accumTime, frameTime)
	local action = actor:GetAction()
	local iSlot = action:GetCurrentSlot()
	local bIsAnimDone = actor:IsAnimationDone()

	if bIsAnimDone == true then
		return false
	end

	return true
end

function Skill_Rust_OnCleanUp(actor, action)
end

function Skill_Rust_OnLeave(actor, action)
	
	return true
end

function Skill_Rust_OnEvent(actor, textKey)

	if textKey == "hit" then
		Skill_Rust_DoEffect(actor)
	end

	return true
end

function Skill_Rust_DoEffect(actor)
	local action = actor:GetAction()
	local iNbProjectile = 5 --action:GetSkillLevel()
	local kProjectileMan = GetProjectileMan()
	local kGuid = actor:GetPilotGuid()

	local kHelperPos = actor:GetNodeWorldPos("p_wp_l_hand")

	local i = 0
	while i < iNbProjectile do
		kProjectile = kProjectileMan:CreateNewProjectile(action:GetScriptParam("PROJECTILE_ID"), action, kGuid)
		if kProjectile:IsNil() == false then
			local kTargetDir = actor:GetTranslate()
			local kMovingDir = actor:GetLookingDir()
			kMovingDir:Multiply(200)
			kTargetDir:Add(kMovingDir)
			kProjectile:SetWorldPos(kHelperPos)
			kProjectile:SetTargetLoc(kTargetDir)
			kProjectile:SetMovingType(1)
			kProjectile:SetParam_SinCurveLineType(50, 0)
			kProjectile:Fire()
		end
		i = i + 1
	end
	
	action:ClearTargetList();
end
