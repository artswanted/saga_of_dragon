
function Skill_DemolitionChargeSA_OnCheckCanEnter(actor,action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	CheckNil(nil==action)
	CheckNil(action:IsNil())

	local kCurAction = actor:GetAction();
	if( false == kCurAction:IsNil() ) then
		if action:GetID() == kCurAction:GetID() then
			return false
		end
	end
	return true
end
function Skill_DemolitionChargeSA_OnEnter(actor, action)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	CheckNil(nil==action)
	CheckNil(action:IsNil())

	action:SetDoNotBroadCast(true)

	return true
end

function Skill_DemolitionChargeSA_OnCastingCompleted(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	actor:PlayCurrentSlot()
end

function Skill_DemolitionChargeSA_Shot_Fire(actor,action)

end


function Skill_DemolitionChargeSA_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	local action = actor:GetAction()
	CheckNil(nil==action)
	CheckNil(action:IsNil())

	local iSlot = action:GetCurrentSlot()
	local bDone = actor:IsAnimationDone()
	if true==bDone then
		if 3==iSlot	then
			return false
		elseif 0==iSlot then
			actor:AttachParticleS(124, "p_ef_r_hand", "ef_dmg01_b", 2.0)
			actor:AttachParticle(125, "p_ef_r_hand", "ef_skill_demolition_charge_02_p_ef_r_hand")
		elseif 1==iSlot then
			actor:AttachParticleS(125, "p_ef_r_hand", "ef_skill_demolition_charge_02_p_ef_r_hand",2.0)
			actor:AttachParticleS(126, "p_ef_r_hand", "ef_dmg01_b", 4.5)
		elseif 2==iSlot then
			actor:DetachFrom(125)
			actor:AttachParticle(125, "char_root", "ef_skill_demolition_charge_03_char_root")
		end

		actor:PlayNext()
	end

	if 2>iSlot then
		local guid = actor:GetCallerGuid()
		if false==guid:IsNil() then
			local Caller = g_pilotMan:FindPilot(guid)
			if false==Caller:IsNil() then		
				local CallerActor = Caller:GetActor()
				if false == CallerActor:IsNil() then
					actor:SetTranslate(CallerActor:GetTranslate())

					local CallerAction = CallerActor:GetAction()
					if false==CallerAction:IsNil() then
						local kName = CallerAction:GetActionName()
						local iRet = string.find(kName, "demolitioncharge")
						if nil==iRet then
							return false
						end
					end
				end
			else
				return false
			end
		end
	end
	
	return true
end
function Skill_DemolitionChargeSA_OnCleanUp(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())

	CheckNil(nil==action);
	CheckNil(action:IsNil());
	action:SetDoNotBroadCast(true)

	return true;
end
function Skill_DemolitionChargeSA_OnLeave(actor, action)
	return true
end

function Skill_DemolitionChargeSA_OnEvent(actor, textKey)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
		
	if textKey == "alpha_end" then
		actor:SetTargetAlpha(actor:GetAlpha(),0.0, 0.15); -- 일단 처음엔 투명하게
	end
end
