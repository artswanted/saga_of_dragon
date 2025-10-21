------------------------------------------------
-- Definitions
------------------------------------------------
-- Param
-- 10[int] : 종료 체크
-- 11[float] : 시간
-- 15[int] : AT_COUNT
-- 16[float] : total timer time
-- 17[int] : 히트 횟수



function Skill_WarLord_SoulDrain_OnEnter(actor, action)
	
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
	action:SetParamInt(10,0);
	action:SetParamFloat(11,g_world:GetAccumTime());
	action:SetParamInt(15, action:GetAbil(AT_COUNT));
	action:SetParamFloat(16,5.0);
	action:SetParamInt(17,0);
	
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
    end
    
	local kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	
	
	local kEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID");
    local kEffectNode = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE");
	if kEffectID ~= "" and kEffectTargetNodeID ~= "" then
		actor:AttachParticle(201, kEffectNode, kEffectID);
	end	
	
	Skill_WarLord_SoulDrain_Fire(actor,action);

	return true
end

function Skill_WarLord_SoulDrain_OnCastingCompleted(actor,action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:CutSkillCasting();
end

function Skill_WarLord_SoulDrain_OnUpdate(actor, accumTime, frameTime)

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
		local fElapsedTime = accumTime - action:GetParamFloat(11);
		if fElapsedTime > action:GetParamInt(16) then
			action:SetParamInt(10,1);
		end
	else
		return false
	end

	return true
end

function Skill_WarLord_SoulDrain_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(101);
	actor:DetachFrom(102);
	
	actor:RestoreLockBidirection();
	
	return true;
end

function Skill_WarLord_SoulDrain_OnLeave(actor, action)

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

function Skill_WarLord_SoulDrain_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
		Skill_WarLord_SoulDrain_HitOneTime(actor,action);
    end
end

function Skill_WarLord_SoulDrain_OnEvent(actor,textKey,seqID)
	return true;
end

function Skill_WarLord_SoulDrain_OnTimer(actor,accumtime,action,iTimerID)
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
    	Skill_WarLord_SoulDrain_OnTargetListModified(actor,action,false);
    	return true;
	end	
	
	return true;
end

function Skill_WarLord_SoulDrain_Fire(actor,action)
		
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:PlayNext();
	
	if actor:IsMyActor() == true then
		local totalTime = action:GetParamFloat(16);
		local termTime = totalTime / (action:GetParamInt(15) + 1);

		action:StartTimer(totalTime,termTime,0);
	end
end

function Skill_WarLord_SoulDrain_CreateProjectile(kActor, kAction, kPos)

	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	local	kProjectileMan = GetProjectileMan()
	local	kProjectileID = kActor:GetAnimationInfo("PROJECTILE_ID")
	if nil == kProjectileID then
		kProjectileID = kActor:GetEquippedWeaponProjectileID()
		if kProjectileID == "" then
			kProjectileID = "PROJECTILE_Big_Master_machine_shot_02"
		end
	end
	
	local	fProjetileScale = tonumber(kActor:GetAnimationInfo("PROJECTILE_SCALE"))
	if nil  == fProjetileScale then
		fProjetileScale = 1.0
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID, kAction, kActor:GetPilotGuid())
	if kNewArrow:IsNil() then
		return nil
	end
	
	local	kTargetNodeID = kActor:GetAnimationInfo("FIRE_START_NODE")
	if nil == kTargetNodeID then
		kTargetNodeID = "p_ef_heart"
	end
	
	local	fProjectileSpeed = kActor:GetAnimationInfo("PROJECTILE_SPEED")
	if nil ~= fProjectileSpeed then
		kNewArrow:SetSpeed(fProjectileSpeed)
	end
	
	kNewArrow:SetScale(fProjetileScale*kNewArrow:GetScale())
	--kNewArrow:LoadToHelper(kActor,kTargetNodeID)
	kNewArrow:LoadToPosition(kPos)

	return kNewArrow
end

function Skill_WarLord_SoulDrain_FireProjectile(kActor, kAction)
	CheckNil(nil==kActor);
	CheckNil(kActor:IsNil());	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	local kTargetList = kAction:GetTargetList();
	local iTargetCount = kTargetList:size();
	local i = 0
	
	while i<iTargetCount do
		local kTargetGUID = kAction:GetTargetGUID(i)
		local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID)
		if kTargetPilot:IsNil() == false then
			local kTargetActor = kTargetPilot:GetActor()
			if kTargetActor:IsNil() == false then
				local kTargetPos = kTargetActor:GetPos()
				local kProjectile = Skill_WarLord_SoulDrain_CreateProjectile(kActor, kAction, kTargetPos)
				
				if kProjectile ~= nil then
					if kProjectile:IsNil() == false then
						local kGoalPos = kTargetActor:GetPos()
						kGoalPos:SetZ(kGoalPos:GetZ() + 500)
						kProjectile:SetTargetLoc(kGoalPos)
						kProjectile:Fire()
					end
				end
			end
		end
		i = i + 1
	end
end

function Skill_WarLord_SoulDrain_HitOneTime(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iHitCount = action:GetParamInt(2);
	
	if iHitCount == 0 then
	
	    local   kHitEffectID = action:GetScriptParam("HIT_CASTER_EFFECT_ID");
	    local   kHitEffectTargetNodeID = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE");
		local   kHitEffectTargetNodeIDToPos = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS");
		local   kHitEffectTargetNodeIDToPosWithRotate = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE");

	    if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" or kHitEffectTargetNodeIDToPos ~= ""  or kHitEffectTargetNodeIDToPosWithRotate ~= "" then
			if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" then
				actor:AttachParticle(7214,kHitEffectTargetNodeID,kHitEffectID);
			elseif kHitEffectTargetNodeID ~= "" then
	            actor:AttachParticle(7214,kHitEffectTargetNodeID,kHitEffectID);
			elseif kHitEffectTargetNodeIDToPos ~= "" then
				actor:AttachParticleToPoint(7214,actor:GetNodeWorldPos(kHitEffectTargetNodeIDToPos),kHitEffectID);			
			else
			    actor:AttachParticleToPointWithRotate(7214,actor:GetNodeWorldPos(kHitEffectTargetNodeIDToPosWithRotate), kHitEffectID, actor:GetRotateQuaternion())
		    end
		end
	end

	local	iTotalHit = tonumber(action:GetScriptParam("MULTI_HIT_NUM"));
	if iTotalHit == nil or iTotalHit == 0 then
	    iTotalHit = 1;
	end
	
	if iHitCount == iTotalHit then
		return
	end
	
	local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2784,kSoundID);
	end	
	
	--남성용 사운드
	if 1 == actor:GetAbil(AT_GENDER) then
		local	kSoundID2 = action:GetScriptParam("HIT_SOUND_ID_MALE");
		if kSoundID2~="" then
			actor:AttachSound(2785,kSoundID2);
		end	
	--여성용 사운드
	else
		local	kSoundID2 = action:GetScriptParam("HIT_SOUND_ID_FEMALE");
		if kSoundID2~="" then
			actor:AttachSound(2785,kSoundID2);
		end	
	end
		
	local iTargetCount = action:GetTargetCount();
	local i =0;
	if iTargetCount>0 then
		
		local bWeaponSoundPlayed = false;
		
		while i<iTargetCount do
		
			local actionResult = action:GetTargetActionResult(i);
			if actionResult:IsNil() == false then
			
				local kTargetGUID = action:GetTargetGUID(i);
				local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
				if kTargetPilot:IsNil() == false and actionResult:IsMissed() == false then
					
					local actorTarget = kTargetPilot:GetActor();
					
                    if bWeaponSoundPlayed == false then
                        bWeaponSoundPlayed = true;
                        -- 피격 소리 재생
						local actionName = action:GetID();
						if actionResult:GetCritical() then
							actionName = "Critical_dmg"
						end
                        actorTarget:PlayWeaponSound(WST_HIT, actor, actionName, 0, action:GetTargetInfo(i));
                    end
					Skill_WarLord_SoulDrain_DoDamage(actor,actorTarget,actionResult, action:GetTargetInfo(i));
				end
				
			end
			
			i=i+1;
		
		end
	end

	action:GetTargetList():ApplyActionEffects();
	actor:ClearIgnoreEffectList();
	
	iHitCount = iHitCount + 1;
	action:SetParamInt(2,iHitCount);
	
end

function Skill_WarLord_SoulDrain_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
	
	CheckNil(nil==kActionResult);
	CheckNil(nil==kActionTargetInfo);
	
	local kAction = actor:GetAction()
	local kActionID = kAction:GetID()
	
	if actor:GetPilotGuid():IsEqual(actorTarget:GetPilotGuid()) then
		return
	end
	
	local	iABVIndex = kActionTargetInfo:GetABVIndex();
	local pt = actorTarget:GetABVShapeWorldPos(iABVIndex);
	
    local   kHitTargetEffectID = kAction:GetScriptParam("HIT_TARGET_EFFECT_ID");
    local   kHitTargetEffectTargetNodeID = kAction:GetScriptParam("HIT_TARGET_EFFECT_TARGET_NODE");
    
    if kHitTargetEffectID ~= "" and kHitTargetEffectTargetNodeID ~= "" then
		local	iHitCount = actor:GetAction():GetParamInt(2);
        actorTarget:AttachParticle(7111+iHitCount,kHitTargetEffectTargetNodeID,kHitTargetEffectID);
    
    end	
	
	
	if kAction:GetScriptParam("NO_DEFAULT_HIT_EFFECT") ~= "TRUE" then
		local   kHitTargetEffectOffset = tonumber(kAction:GetScriptParam("HIT_TARGET_EFFECT_RANDOM_OFFSET"));
		
		if kHitTargetEffectOffset ~= nil then
			pt:SetX(pt:GetX() + math.random(-kHitTargetEffectOffset,kHitTargetEffectOffset))
			pt:SetY(pt:GetY() + math.random(-kHitTargetEffectOffset,kHitTargetEffectOffset))
			pt:SetZ(pt:GetZ() + math.random(-kHitTargetEffectOffset,kHitTargetEffectOffset))
		end

		local	iHitCount = actor:GetAction():GetParamInt(2);

	    if kActionResult:GetCritical() then
		    actorTarget:AttachParticleToPoint(12 + iHitCount, pt, "e_dmg_cri")
	    else
		    actorTarget:AttachParticleToPoint(12 + iHitCount, pt, "e_dmg")
	    end	
	    
	    
	    if kAction:GetScriptParam("NO_DAMAGE_HOLD") ~= "TRUE" then
    	    
	        -- 충격 효과 적용
	        local iShakeTime = g_fAttackHitHoldingTime * 1000;
	        actor:SetAnimSpeedInPeriod(0.01, iShakeTime);
	        actor:SetShakeInPeriod(5, iShakeTime/2);
	        actorTarget:SetShakeInPeriod(5, iShakeTime);		
	    end
	end
end
