function Skill_MultiShot_OnEnter(actor, action)
	actorID = actor:GetID()
	actionID = action:GetID()
	weapontype = actor:GetEquippedWeaponType();

	if actor:IsMyActor() == true then

--		if actor:IsExistParts(64) == false then
--			AddWarnDataStr(GetTextW(37), 1)
--			return false
--		end

		if actor:GetNormalAttackFreezeElapsedTime()~=-1 and actor:GetNormalAttackFreezeElapsedTime()<200 then
			return false;
		end
		
		actor:StopNormalAttackFreeze();

		actor:SetNormalAttackActionID("a_shot_0"..math.random(1,GetMaxMeleeShotType(actor)));	
	
	end
	
	actor:ResetAnimation();
		
	kProjectileMan = GetProjectileMan();
	kNewArrow = kProjectileMan:CreateNewProjectile(action:GetScriptParam("PROJECTILE_ID"),action,actor:GetPilotGuid());
	kNewArrow:LoadToWeapon(actor);	--	장전
	action:SetParamInt(3,kNewArrow:GetUID());
	
	if actor:IsMyActor() == true then
		action:LockCurrentTarget();
	end
	
	iTargetCount = action:GetTargetCount();
	if iTargetCount>0 then
			--	타겟을 찾아 바라본다
		kTargetGUID = action:GetTargetGUID(0);
		targetobject = g_pilotMan:FindPilot(kTargetGUID);
		iTargetABVShapeIndex = action:GetTargetABVShapeIndex(0);
		if targetobject:IsNil() == false then
			targetobjpos = targetobject:GetActor():GetHitABVCenterPos();
			actor:LookAt(targetobjpos);
		end
	end
		

	
	return true
end

function Skill_MultiShot_OnUpdate(actor, accumTime, frameTime)
	actorID = actor:GetID()
	action = actor:GetAction()
	animDone = actor:IsAnimationDone()
	nextActionName = action:GetNextActionName()
	iMaxActionSlot = action:GetSlotCount();
	weapontype = actor:GetEquippedWeaponType();
	actionID = action:GetID();

	if animDone == true then
	
		actor:SetNormalAttackEndTime();
	
		iCurrentComboCount = actor:GetComboCount();
		iNextComboCount = iCurrentComboCount + 1;
		if iNextComboCount>=GetMaxComboCount(actor) then
			actor:StartNormalAttackFreeze();
			iNextComboCount = 0;
		end
		actor:SetComboCount(iNextComboCount);
		
		curActionSlot = action:GetCurrentSlot()
		curActionParam = action:GetParam(0)
		if curActionParam == "GoNext" then
			nextActionName = action:GetParam(1);

			action:SetParam(0, "null")
			action:SetParam(1, "end")
			action:SetNextActionName(nextActionName)
			return false;
		else

			if iCurrentComboCount == GetMaxComboCount(actor)-1 then
				actor:StartNormalAttackFreeze();
			end
			
			actor:SetNormalAttackActionID("a_shot_0"..math.random(1,GetMaxMeleeShotType(actor)));	
			action:SetParam(1, "end")

			return false
		end
	end

	return true
end

function Skill_MultiShot_OnCleanUp(actor, action)
	if actor:GetAction():GetParam(7)~="FIRED" then	-- 장전은 됬는데, 발사가 되지 않았다
		--	지워주자
		kProjectileMan = GetProjectileMan();
		kProjectileMan:DeleteProjectileByUID(actor:GetAction():GetParamInt(3));
	end
	
	return true;
end

function Skill_MultiShot_OnLeave(actor, action)
	curAction = actor:GetAction();
	curParam = curAction:GetParam(1)
	actionID = action:GetID()

	if actor:IsMyActor() == false then
		return true;
	end
	if actionID == "a_jump" then 
		return false;
	end	
	if action:GetActionType()=="EFFECT" then
		return true;
	end	
	if action:GetEnable() == true then
		
		if curParam == "end" then 
			return true;
		end
		
		if actor:GetComboCount() < GetMaxComboCount(actor)-1 then
		
			curAction:SetParam(0,"GoNext");
			curAction:SetParam(1,actionID);
		
		end

		return false;
	
	elseif curParam == "end" and
		(actionID == "a_run_right" or
		actionID == "a_run_left") then
		return true;
	elseif 	actionID == "a_idle" or
		actionID == "a_die" or
		actionID == "a_handclaps" then
		actor:SetNormalAttackActionID("a_archer_shot_01");
		return true;
	end

	return false 
end


function Skill_MultiShot_OnEvent(actor, textKey)
	if textKey == "fire" or textKey=="hit" then
		action = actor:GetAction()
		iAttackRange = GetAttackRange(actor);
		
		kProjectileMan = GetProjectileMan();
		kArrow = kProjectileMan:GetProjectile(actor:GetAction():GetParamInt(3));
		
		targetobject = nil;
	
		if kArrow:IsNil() == false then
		
			kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
			arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
			
			iTargetCount = action:GetTargetCount();
			iTargetABVShapeIndex = 0;
			if iTargetCount>0 then
					--	타겟을 찾아 바라본다
				kTargetGUID = action:GetTargetGUID(0);
				iTargetABVShapeIndex = action:GetTargetABVShapeIndex(0);
				targetobject = g_pilotMan:FindPilot(kTargetGUID);
				if targetobject:IsNil() == false then
					targetobjpos = targetobject:GetActor():GetHitABVCenterPos();
					actor:LookAt(targetobjpos);
				end						
			end
			ODS("Shot iTargetCount : "..iTargetCount);
			
			if targetobject == nil or targetobject:IsNil()==true then
				
				ODS(" targetobject is nil \n");
				
				kMovingDir:Multiply(iAttackRange);
				
				kMovingDir:Add(arrow_worldpos);
				
				kArrow:SetTargetLoc(kMovingDir);	--	발사!
				
			else
				ODS(" targetobject is "..targetobject:GetName():GetStr().." \n");
				
				kArrow:SetTargetObject(targetobject:GetGuid(),iTargetABVShapeIndex);	--	발사!
				
			end
			
			actor:GetAction():SetParam(7,"FIRED");
		
		end

	end
end
