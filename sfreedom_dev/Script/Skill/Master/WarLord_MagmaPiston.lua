------------------------------------------------
-- Definitions
------------------------------------------------
-- Param
-- 10[int] : 종료 체크
-- 11[float] : 시간
-- 12[int] : 타이머 횟수

function Skill_WarLord_MagmaPiston_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return false;
	end

	return true;
end

function Skill_WarLord_MagmaPiston_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParamInt(10,0);
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(11,g_world:GetAccumTime());
	action:SetParamInt(12,0);
	
	if actor:IsMyActor() == true then
		if actor:IsMeetFloor() == false then		-- 공중에서는 사용할 수 없다.
			return	false;
		end
	end

	if actor:IsMyActor() then
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, 0.0, 0.7, 0.4,true,true);
	end

	
	if action:GetActionParam() == AP_CASTING then
		-- 양손에 캐스팅 이펙트를 붙인다.
		actor:AttachParticle(101,"p_wp_r_hand","ef_mag_charge01_loop_p_wp_r_hand_p_wp_l_hand");	
		actor:AttachParticle(102,"p_wp_l_hand","ef_mag_charge01_loop_p_wp_r_hand_p_wp_l_hand");	
	end
	
	local kEffectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
	local kEffectNode = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE");
	if kEffectID ~= "" and kEffectNode ~= "" then
		actor:AttachParticle(201, kEffectNode, kEffectID);
	end
	
	-- local kTargetPos = actor:GetPos();
	-- Skill_WarLord_MagmaPiston_CreateDarkElemental(actor,action,kTargetPos);
    
   return true
end

function Skill_WarLord_MagmaPiston_OnCastingCompleted(actor,action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:CutSkillCasting();
	local kTargetPos = actor:GetPos();
	Skill_WarLord_MagmaPiston_CreateDarkElemental(actor,action,kTargetPos);
	local kEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID");
    local kEffectNode = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE");
	if kEffectID ~= "" and kEffectTargetNodeID ~= "" then
		actor:AttachParticle(202, kEffectNode, kEffectID);
	end
end

function Skill_WarLord_MagmaPiston_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local	animDone = actor:IsAnimationDone();
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:LockBidirection(false);
	actor:SeeFront(true);
	
	local	curSlot = action:GetCurrentSlot();
	local	iState = action:GetParamInt(10);
	
	if iState == 0 then
		if animDone == true then
			if curSlot == 0 then
				Skill_WarLord_MagmaPiston_Fire(actor,action);
				return true
			end
		end

		local	fElapsedTime = accumTime - action:GetParamFloat(11);

		if curSlot == 1 and fElapsedTime > 5 then
			actor:PlayNext();
			return true;
		end
		if animDone == true and curSlot == 2 then
			action:SetParamInt(10,1);
			return true;
		end
	else
		return false
	end

	return true
end

function Skill_WarLord_MagmaPiston_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	actor:DetachFrom(101, true);
	actor:DetachFrom(102, true);
	actor:DetachFrom(201);
	actor:DetachFrom(202, true);

	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
	end	

	actor:RestoreLockBidirection();
	
	return true;
end

function Skill_WarLord_MagmaPiston_OnLeave(actor, action)

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

function Skill_WarLord_MagmaPiston_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
end

function Skill_WarLord_MagmaPiston_OnEvent(actor,textKey,seqID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if textKey == "hit" then
		local kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2783,kSoundID);
		end	
		if actor:IsMyActor() then
			QuakeCamera(4, 1);
		end
	end
	return true;
end

function Skill_WarLord_MagmaPiston_Fire(actor,action)
		
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	-- local kEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID");
    -- local kEffectNode = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE");
	-- if kEffectID ~= "" and kEffectTargetNodeID ~= "" then
		-- actor:AttachParticle(202, kEffectNode, kEffectID);
	-- end
	
	actor:PlayNext();
end

function Skill_WarLord_MagmaPiston_CreateDarkElemental(actor,action,kTargetPos)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local guid = GUID("123");
	guid:Generate();
	
	local pilot = g_pilotMan:NewPilot(guid, 1000702, 0);
		if pilot:IsNil() == false then
		local kNewActor = pilot:GetActor();
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddActor(guid, kNewActor, kTargetPos, 9);
		kNewActor:FreeMove(true);
		kNewActor:SetTranslate(kTargetPos);
		kNewActor:ReserveTransitAction("a_MagmaPiston_Play");
		kNewActor:ClearActionState()
		local	kPathNormal = actor:GetPathNormal();
		kPathNormal:Multiply(-1);
		kNewActor:LookAt(kNewActor:GetTranslate():_Add(kPathNormal), true)	
		kNewActor:PlayCurrentSlot();
	end
end
