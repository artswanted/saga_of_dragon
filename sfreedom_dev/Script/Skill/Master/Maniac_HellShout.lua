------------------------------------------------
-- Definitions
------------------------------------------------
-- Param
-- 10[int] : 종료 체크
-- 11[float] : 시간
-- 12[int] : timer count
-- 15[int] : AT_COUNT
-- 16[float] : total timer time

function Skill_Maniac_HellShout_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return true;
end

function Skill_Maniac_HellShout_OnCastingCompleted(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:CutSkillCasting();
end

function Skill_Maniac_HellShout_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	if actor:IsMyActor() == true then
		if actor:IsMeetFloor() == false then		-- 공중에서는 사용할 수 없다.
			return	false;
		end
	end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if actor:IsMyActor() then
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, 0.0, 0.7, 0.4,true,true);
	end

	action:SetParamInt(10,0) --종료 체크	
	action:SetParamFloat(11,g_world:GetAccumTime()) -- 시간
	action:SetParamInt(15, action:GetAbil(AT_COUNT));
	action:SetParamFloat(16, 1);
	Skill_Maniac_HellShout_Fire(actor,action)
		
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
    end
    
	local kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	
	
	local kEffectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
	local kEffectNode = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE");
	if kEffectID ~= "" and kEffectNode ~= "" then
		actor:AttachParticle(201, kEffectNode, kEffectID);
	end
	
	return true
end

function Skill_Maniac_HellShout_Fire(actor,action)
		
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local kTargetPos = actor:GetPos()
	Skill_Maniac_HellShout_CreateElemental(actor,action,kTargetPos)
end

function Skill_Maniac_HellShout_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local	animDone = actor:IsAnimationDone();
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local	iState = action:GetParamInt(10);
	if iState == 0 then
		if animDone == true then
			action:SetParamInt(10,1)
		end
	else
		return false
	end

	return true
end

function Skill_Maniac_HellShout_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	actor:DetachFrom(201, true);
	actor:DetachFrom(202);

	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
	end	
	
	return true;
end


function Skill_Maniac_HellShout_OnLeave(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if action:GetEnable() == false then
		return	false;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end	
	
	local	curAction = actor:GetAction();
	if curAction:GetParamInt(10) == 1 then
		return true
	end

	return false
end

function Skill_Maniac_HellShout_OnEvent(actor,textKey,seqID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if textKey == "hit" then
		if actor:IsMyActor() == true then
			-- action:CreateActionTargetList(actor);
			-- action:BroadCastTargetListModify(actor:GetPilot());
			-- action:ClearTargetList();
			local totalTime = action:GetParamFloat(16);
			local termTime = totalTime / (action:GetParamInt(15) + 1);
			action:StartTimer(totalTime,termTime,0);
			action:SetParamInt(12,action:GetParamInt(15));
		end
		
		local kEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID");
		local kEffectNode = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE");
		if kEffectID ~= "" and kEffectNode ~= "" then
			actor:AttachParticle(202, kEffectNode, kEffectID);
		end
		QuakeCamera(1.0, 0.5);
	end
	return	true;
end

function Skill_Maniac_HellShout_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if bIsBefore == false then
		Act_Melee_Default_HitOneTime(actor,action);
    end

end

function Skill_Maniac_HellShout_CreateElemental(actor,action,kTargetPos)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local guid = GUID("123")
	guid:Generate()
	
	local pilot = g_pilotMan:NewPilot(guid, 1000704, 0)
	if pilot:IsNil() == false then
		local newActor = pilot:GetActor();
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddActor(guid, newActor, kTargetPos, 9)
		newActor:FreeMove(true)
		newActor:ReserveTransitAction("a_HellShout_Play")
		newActor:ClearActionState()
		local	kLookAt = actor:GetLookingDir()
		newActor:LookAt(actor:GetTranslate():_Add(kLookAt), true)	--캐릭터랑 같은 방향으로
		newActor:PlayCurrentSlot();
	end
end

function Skill_Maniac_HellShout_OnTimer(actor, accumTime, action,iTimerID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local count = action:GetParamInt(12);
	
	if count == 0 then
		return true
	end

	action:SetParamInt(12, count-1);
	
	action:CreateActionTargetList(actor);
	action:BroadCastTargetListModify(actor:GetPilot());
	action:ClearTargetList();
	
	return true;
end

