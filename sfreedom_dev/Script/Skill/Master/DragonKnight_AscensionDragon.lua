------------------------------------------------
-- Definitions
------------------------------------------------
-- Param
-- 10[int] : 종료 체크
-- 11[float] : 시간
-- 12[float] : 발동 시작 시간
-- 13[float] : 기준 거리
-- 14[float] : 이동 거리
-- 15[int] : AT_COUNT
-- 16[float] : total timer time
-- 17[int] : 히트 횟수

-- Param Point
-- 101 : 시전자 초기 위치

function Skill_DragonKnight_AscensionDragon_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return true;

end

function Skill_DragonKnight_AscensionDragon_OnCastingCompleted(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:CutSkillCasting();
end

function Skill_DragonKnight_AscensionDragon_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if actor:IsMyActor() == true then
		if actor:IsMeetFloor() == false then		-- 공중에서는 사용할 수 없다.
			return	false;
		end
	end
	
	action:SetParamInt(10,0)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(11,g_world:GetAccumTime());
	action:SetParamFloat(13,20);
	action:SetParamFloat(14,20);
	action:SetParamInt(15, action:GetAbil(AT_COUNT));
	action:SetParamFloat(16, 0.7);
	action:SetParamInt(17, 0);
	action:SetParamAsPoint(101,actor:GetPos());

	local   kEffectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
	if kEffectID ~= "" then
		local kDir = actor:GetPos();
		kDir:SetZ(kDir:GetZ()-28);
		actor:AttachParticleToPoint(201,kDir, kEffectID)
	end

	local kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	

	actor:StartWeaponTrail();

	if actor:IsMyActor() then
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, 0.0, 0.7, 0.4,true,true);
	end

	return true
end

function Skill_DragonKnight_AscensionDragon_Fire(actor,action)
		
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	actor:PlayNext();
	
	local   kFireEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID");
    local   kFireEffectTargetNodeID = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE");
	if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" then
		actor:AttachParticleToPointWithRotate(202,actor:GetPos(),kFireEffectID,actor:GetRotateQuaternion(),1.0);            
	end
	
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
		local totalTime = action:GetParamFloat(16);
		local termTime = totalTime / (action:GetParamInt(15) + 1);
		action:StartTimer(totalTime,termTime,0);
    end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(12,g_world:GetAccumTime());

	actor:StopJump();
	actor:FreeMove(true);

end

function Skill_DragonKnight_AscensionDragon_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local	animDone = actor:IsAnimationDone();
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	curSlot = action:GetCurrentSlot();
	local	iState = action:GetParamInt(10);
	
	if iState == 0 then
		if animDone == true then
			if curSlot == 0 then
				Skill_DragonKnight_AscensionDragon_Fire(actor,action)
			end
		end

		local fElapsedTime = accumTime - action:GetParamFloat(12);
		local totalTime = action:GetParamFloat(16);
		
		if curSlot == 2 and animDone == true then
			action:SetParamInt(10,1);
			actor:FreeMove(false);
		elseif curSlot == 1 and fElapsedTime > totalTime then
			actor:PlayNext();
		elseif curSlot == 1 then
			local	kMovingVec = actor:GetLookingDir();
			local fDirRate = 1;
			if fElapsedTime < totalTime then
				local angle = (math.pi/2) * (fElapsedTime / totalTime);
				fDirRate = math.cos(angle);
			end
			local fDistance = fDirRate * 180;
			local height = fDirRate * 300;
			kMovingVec:Multiply(fDistance);
			kMovingVec:SetZ(height);
			actor:SetMovingDelta(kMovingVec);
		end
	else
		return false;
	end

	return true;
end

function Skill_DragonKnight_AscensionDragon_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	actor:EndWeaponTrail();

	actor:FreeMove(false);
		
	actor:DetachFrom(201);
	actor:DetachFrom(202);

	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
	end	
	
	return true;
end


function Skill_DragonKnight_AscensionDragon_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	
	local actionID = action:GetID()

	if actionID == "a_jump" then 
	
		if curAction:GetParamInt(10) == 1 then 

			action:SetSlot(2);
			action:SetDoNotBroadCast(true);
			return true;
		end
		return	false;
	end

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

function Skill_DragonKnight_AscensionDragon_OnEvent(actor,textKey,seqID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction()
			
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	return	true;
end

function Skill_DragonKnight_AscensionDragon_OnTargetListModified(actor,action,bIsBefore)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
		Act_Melee_Default_HitOneTime(actor,action)
	end
end

function  Skill_DragonKnight_AscensionDragon_OnTimer(actor,accumtime,action,iTimerID)
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
    	Skill_DragonKnight_AscensionDragon_OnTargetListModified(actor,action,false);
    	return true;
	end	
	
	return true;
end
