
function Act_Revive_OnEnter(actor, action)
	actor:Blink(false, 30)
	actor:SetCanHit(true)
	actor:SetAbil(AT_DIE_MOTION,0)
	
	
	local kPilot = actor:GetPilot()
	if( kPilot:IsNil() ) then return true end
	
	local	iBaseActorType = kPilot:GetBaseClassID()
	if(iBaseActorType == CT_DOUBLE_FIGHTER) then
		local kSubActor = actor:GetSubPlayer()
		if( false == kSubActor:IsNil() ) then	
			if( actor:IsMyActor() ) then
				local kSubActorAction = kSubActor:GetAction()
				if( false == kSubActorAction:IsNil() ) then
					if(kSubActorAction:GetID() ~= "a_twin_sub_repos") then
						TryMustChangeActorAction(kSubActor, "a_twin_sub_repos")
					end
				end
			end
			kSubActor:SetCanHit(true)
		end
	end		
	
	
	return true
end

function Act_Revive_OnUpdate(actor, accumTime, frameTime)
	local action = actor:GetAction()

	local isAnimDone = actor:IsAnimationDone()
	return false
end

function Act_Revive_OnCleanUp(actor, action)
end

function Act_Revive_OnLeave(actor, action)
	return true
end

