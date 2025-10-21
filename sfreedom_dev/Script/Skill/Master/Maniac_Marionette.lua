------------------------------------------------
-- Definitions
------------------------------------------------
-- Param
-- 10[int] : 종료 체크
-- 11[float] : 시간
-- 19[Int] : 이동력

function Skill_Maniac_Marionette_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return false;
	end

	return true;
end

function Skill_Maniac_Marionette_OnEnter(actor, action)
	
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
	action:SetParamInt(19,200);
	
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
    end
    
	local kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	
	
	local   kEffectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
    local   kEffectTargetNode = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE");
	if kEffectID ~= "" and kEffectTargetNode ~= "" then
		actor:AttachParticle(201, kEffectTargetNode, kEffectID);
	end
	
	Skill_Maniac_Marionette_CreateElemental(actor, action, actor:GetPos());
	
	return true
end

function Skill_Maniac_Marionette_OnCastingCompleted(actor,action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:CutSkillCasting();
end

function Skill_Maniac_Marionette_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local	animDone = actor:IsAnimationDone();
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local iState = action:GetParamInt(10);
	local iSlotNum = action:GetCurrentSlot();
	
	if iState == 0 then
		if animDone == true then
			if iSlotNum == 0 then
				action:SetSlot(iSlotNum+1);
				actor:PlayCurrentSlot();
				actor:DetachFrom(201, true);
			elseif iSlotNum == 1 or iSlotNum == 2 then
				action:SetSlot(iSlotNum+1);
				actor:PlayCurrentSlot();
				local   kEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID");
				local   kEffectTargetNode = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE");
				if kEffectID ~= "" and kEffectTargetNode ~= "" then
					actor:AttachParticle(202, kEffectTargetNode, kEffectID);
				end
			elseif iSlotNum == 3 then
				action:SetParamInt(10,1);
			end
		end

		if iSlotNum == 1 then
			local findTarget = Skill_Maniac_Marionette_SearchTarget(actor,action);
			if findTarget == true then
				action:SetSlot(2);
				actor:PlayCurrentSlot();
				local guid = action:GetGUIDInContainer(0);
				if( CheckNil(g_world == nil) ) then return false end
				if( CheckNil(g_world:IsNil()) ) then return false end
				local elemental = g_world:FindActor(guid);
				if elemental:IsNil() == false then
					local elementalAction = elemental:GetAction();
					if elementalAction:IsNil() == false then
						elementalAction:SetSlot(2);
						elemental:PlayCurrentSlot();
					else
						elemental:PlayNext();
					end
					elemental:SetTranslate(actor:GetPos());
				end
				local   kEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID");
				local   kEffectTargetNode = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE");
				if kEffectID ~= "" and kEffectTargetNode ~= "" then
					actor:AttachParticle(202, kEffectTargetNode, kEffectID);
				end
			else
				local velocity = action:GetParamInt(19);
				local dir = actor:GetLookingDir();
				dir:Multiply(velocity);
				actor:SetMovingDelta(dir);
			end
		end
	else
		return false
	end

	return true
end

function Skill_Maniac_Marionette_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(201);
	actor:DetachFrom(202);
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if actor:IsMyActor() == true then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
	end

	return true;
end

function Skill_Maniac_Marionette_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction()	
	local actionID = action:GetID()
	
	if curAction:GetNextActionName() ~= actionID then
		ODS("다른게 들어옴:"..actionID.."\n",false, 912)
		return false;
	end

	
	if action:GetEnable() == false then
		return	false;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end	

	if curAction:GetParamInt(10) == 1 then
		return true
	end

	return false
end

function Skill_Maniac_Marionette_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
		Act_Melee_Default_HitOneTime(actor,action);
    end
end

function Skill_Maniac_Marionette_OnEvent(actor,textKey,seqID)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction()
			
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if textKey == "hit" then
		Skill_Maniac_Marionette_Fire(actor,action);
	end
	return true;
end

function Skill_Maniac_Marionette_Fire(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if actor:IsMyActor() == true then
		action:CreateActionTargetList(actor);
		action:BroadCastTargetListModify(actor:GetPilot());
		action:ClearTargetList();
	end
	
	if IsSingleMode() then
    	Skill_Maniac_Marionette_OnTargetListModified(actor,action,false);
	end	
end

function Skill_Maniac_Marionette_CreateElemental(actor,action,kTargetPos)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local guid = GUID("123");
	guid:Generate();
	
	local pilot = g_pilotMan:NewPilot(guid, 1000706, 0);
	if pilot:IsNil() == false then
		action:AddNewGUID(guid);
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddActor(guid, pilot:GetActor(), kTargetPos, 9);
		pilot:GetActor():FreeMove(true)
		pilot:GetActor():ReserveTransitAction("a_MarionetteSA");
		pilot:GetActor():ClearActionState()
		local	kLookAt = actor:GetLookingDir()
		pilot:GetActor():LookAt(actor:GetTranslate():_Add(kLookAt), true)	--캐릭터랑 같은 방향으로
		pilot:GetActor():PlayCurrentSlot();
		local entityAction = pilot:GetActor():GetReservedTransitAction();
		if entityAction:IsNil() == false then
			entityAction:SetParamInt(19, action:GetParamInt(19)); -- 이동력 전달
			entityAction:AddNewGUID(actor:GetPilotGuid());
		end
		actor:AddSummonUnit(guid);
	end
end

function Skill_Maniac_Marionette_SearchTarget(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:CreateActionTargetList(actor, true);
	local	kTargetList = action:GetTargetList();
	local	iTargetCount = kTargetList:size();
	local findTarget = false;
	if iTargetCount > 0 then
		local actorPos = actor:GetPos();
		local kSkillDef = GetSkillDef(action:GetActionNo())
		local iSkillRange = 100;
		if nil~=kSkillDef and false==kSkillDef:IsNil() then
			iSkillRange = kSkillDef:GetAbil(AT_ATTACK_RANGE);
			if iSkillRange == 0 then
				iSkillRange = 100;
			end
		end
		local checkRange = iSkillRange * 0.7;
		local i=0;
		while i < iTargetCount do
			local targetInfo = kTargetList:GetTargetInfo(i);
			local targetGUID = targetInfo:GetTargetGUID();
			local targetPilot = g_pilotMan:FindPilot(targetGUID);
			if targetPilot:IsNil() == false then
				local targetActor = targetPilot:GetActor();
				if targetActor:IsNil() == false then
					local targetPos = targetActor:GetPos();
					local distance = actorPos:Distance(targetPos);
					if distance < checkRange then
						findTarget = true;
						break;
					end
				end
			end
			i = i + 1;
		end
	end
	action:ClearTargetList();
	
	return findTarget;
end
