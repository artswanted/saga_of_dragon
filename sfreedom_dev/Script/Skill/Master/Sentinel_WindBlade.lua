------------------------------------------------
-- Definitions
------------------------------------------------
-- Param
-- 10[int] : 종료 체크
-- 11[float] : 시간
-- 15[int] : AT_COUNT
-- 16[float] : total timer time
-- 17[int] : 히트 횟수

function Skill_Sentinel_WindBlade_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return false;
	end

	return true;
end

function Skill_Sentinel_WindBlade_OnEnter(actor, action)
	
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
	
	action:SetParamInt(10,0);
	action:SetParamFloat(11,g_world:GetAccumTime());
	action:SetParamInt(15, action:GetAbil(AT_COUNT));
	action:SetParamFloat(16,1.0);
	action:SetParamInt(17,0);
	
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
    end
    

	local effectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
	local effectNode = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE");
	if effectID ~= "" and effectNode ~= "" then
		actor:AttachParticle(201, effectNode, effectID);
	end
	
	local fireEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID");
	local fireEffectNode = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE");
	if fireEffectID ~= "" and fireEffectNode ~= "" then
		actor:AttachParticle(202, fireEffectNode, fireEffectID);
	end

	Skill_Sentinel_WindBlade_CreateElemental(actor, action, actor:GetPos());
	return true
end

function Skill_Sentinel_WindBlade_OnCastingCompleted(actor,action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:CutSkillCasting();
end

function Skill_Sentinel_WindBlade_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local	animDone = actor:IsAnimationDone();
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iState = action:GetParamInt(10);
	
	if iState == 0 then
		if animDone == true then
			action:SetParamInt(10,1);
		end
	else
		return false
	end

	return true
end

function Skill_Sentinel_WindBlade_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(201);
	actor:DetachFrom(202);
	actor:DetachFrom(203);
	
	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
	end	

	return true;
end

function Skill_Sentinel_WindBlade_OnLeave(actor, action)

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

function Skill_Sentinel_WindBlade_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
		Act_Melee_Default_HitOneTime(actor,action);
    end
end

function Skill_Sentinel_WindBlade_OnEvent(actor,textKey,seqID)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction()
			
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if textKey == "hit" then
		local fireEffectID2 = action:GetScriptParam("FIRE_CASTER_EFFECT_ID2");
		local fireEffectNode2 = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE2");
		if fireEffectID2 ~= "" and fireEffectNode2 ~= "" then
			actor:AttachParticle(203, fireEffectNode2, fireEffectID2);
		end
		Skill_Sentinel_WindBlade_Fire(actor,action);
	end
	return true;
end

function Skill_Sentinel_WindBlade_OnTimer(actor,accumtime,action,iTimerID)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local hitcount = action:GetParamInt(17);
	if hitcount >= action:GetParamInt(15) then
		return true;
	end
	
	action:SetParamInt(17, hitcount + 1 );
	action:CreateActionTargetList(actor);
	action:BroadCastTargetListModify(actor:GetPilot());
	action:ClearTargetList();
		
	if IsSingleMode() then
    	Skill_Sentinel_WindBlade_OnTargetListModified(actor,action,false);
    	return true;
	end	
	
	return true;
end

function Skill_Sentinel_WindBlade_Fire(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if actor:IsMyActor() == true then
		local totalTime = action:GetParamFloat(16);
		local termTime = totalTime / (action:GetParamInt(15) + 1);

		action:StartTimer(totalTime,termTime,0);
	end
	
	local kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	

	
end

function Skill_Sentinel_WindBlade_CreateElemental(actor,action,kTargetPos)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local guid = GUID("123");
	guid:Generate();
	
	local pilot = g_pilotMan:NewPilot(guid, 1000703, 0);
	if pilot:IsNil() == false then
		newActor = pilot:GetActor();
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddActor(guid, newActor, kTargetPos, 9);
		newActor:FreeMove(true)
		newActor:ReserveTransitAction("a_WindBladeSA");
		newActor:ClearActionState()
		local fScale = (action:GetSkillLevel()%10)*0.05 + 1.0;
		newActor:SetTargetScale(fScale);
		local	kLookAt = actor:GetLookingDir()
		newActor:LookAt(actor:GetTranslate():_Add(kLookAt), true)	--캐릭터랑 같은 방향으로
		newActor:PlayCurrentSlot();
	end
end
