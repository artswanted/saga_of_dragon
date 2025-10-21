-- Pickout 

function Act_WeaponSwitch_OnEnter(actor, action)
	local curAction = actor:GetAction()

	actor:ActivateLayerAnimation("WeaponSwitch")

	return true
end

function Act_WeaponSwitch_OnUpdate(actor, accumTime, frameTime)
	local action = actor:GetAction()
	local currentSlot = action:GetCurrentSlot()
	local isAnimDone = actor:IsTargetAnimationDone("WeaponSwitch")

	if isAnimDone == true then
		action:SetParam(911, "end")
	end

	
	
end
function Act_WeaponSwitch_OnCleanUp(actor, action)
end

function Act_WeaponSwitch_OnLeave(actor, action)
	local actionID = action:GetID()

	actor:DeactivateLayerAnimation("WeaponSwitch")

	return false
end
