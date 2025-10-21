------------------------------------------------
-- Definitions
------------------------------------------------
-- Param
-- 10[int] : 종료 체크
-- 11[float] : 시간

function Skill_WarLord_SoulDrain_Ready_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return false;
	end

	return true;
end

function Skill_WarLord_SoulDrain_Ready_OnEnter(actor, action)
	
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
	
	actor:AttachParticle(101,"p_wp_r_hand","ef_mag_charge01_loop_p_wp_r_hand_p_wp_l_hand");	
	actor:AttachParticle(102,"p_wp_l_hand","ef_mag_charge01_loop_p_wp_r_hand_p_wp_l_hand");	
	
	local kEffectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
	local kEffectNode = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE");
	if kEffectID ~= "" and kEffectNode ~= "" then
		actor:AttachParticle(201, kEffectNode, kEffectID);
	end
    
	local kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	
	
	actor:AttachParticleS(201, "char_root", "ef_skill_souldrain_01_char_root",0.4);
   return true
end

function Skill_WarLord_SoulDrain_Ready_OnCastingCompleted(actor,action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:CutSkillCasting();
end

function Skill_WarLord_SoulDrain_Ready_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local	animDone = actor:IsAnimationDone();
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:LockBidirection(false);
	actor:SeeFront(true);
	
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

function Skill_WarLord_SoulDrain_Ready_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:RestoreLockBidirection();
	
	return true;
end

function Skill_WarLord_SoulDrain_Ready_OnLeave(actor, action)

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

function Skill_WarLord_SoulDrain_Ready_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
		Act_Melee_Default_HitOneTime(actor,action);
	end
end

function Skill_WarLord_SoulDrain_Ready_OnEvent(actor,textKey,seqID)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction()
			
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if textKey == "hit" then
		Skill_WarLord_SoulDrain_Ready_Fire(actor,action);
	end
	
	return	true;
end

function Skill_WarLord_SoulDrain_Ready_Fire(actor,action)
		
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:PlayNext();
	
	if actor:IsMyActor() == true then
		action:CreateActionTargetList(actor);
		action:BroadCastTargetListModify(actor:GetPilot());
		action:ClearTargetList();
	end
    
	local kTargetPos = actor:GetPos();
	Skill_WarLord_SoulDrain_Ready_CreateDarkElemental(actor,action,kTargetPos);
	local kEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID");
	local kEffectNode = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE");
	if kEffectID ~= "" and kEffectNode ~= "" then
		actor:AttachParticle(202, kEffectNode, kEffectID);
	end
end

function Skill_WarLord_SoulDrain_Ready_CreateDarkElemental(actor,action,kTargetPos)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local guid = GUID("123");
	guid:Generate();
	
	local pilot = g_pilotMan:NewPilot(guid, 1000701, 0);

	if pilot:IsNil() == false then
		local kNewActor = pilot:GetActor();
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddActor(guid, kNewActor, kTargetPos, 9);
		kNewActor:FreeMove(true);
		kNewActor:SetTranslate(kTargetPos);
		kNewActor:ReserveTransitAction("a_SoulDrain_Play");
		kNewActor:ClearActionState()
		local	kPathNormal = actor:GetPathNormal();
		kPathNormal:Multiply(-1);
		kNewActor:LookAt(kNewActor:GetTranslate():_Add(kPathNormal), true)	
		kNewActor:PlayCurrentSlot();
	end
end
