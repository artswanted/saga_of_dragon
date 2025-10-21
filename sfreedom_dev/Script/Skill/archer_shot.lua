
function archer_shot_OnCheckCanEnter(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	
	-- 안전거품 공격 막기
	if g_pilotMan:IsLockMyInput() == true then
		return
	end	
			
	return		true;
	
end
function archer_shot_FindTarget(kActor,kAction)
	
	if( CheckNil(nil==kActor) ) then return false end
	if( CheckNil(kActor:IsNil()) ) then return false end
	
	if( CheckNil(nil==kAction) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	local	kTargets = CreateActionTargetList(kActor:GetPilotGuid(),kAction:GetActionInstanceID(),kAction:GetActionNo());
	local	iFound = archer_shot_OnFindTarget(kActor,kAction,kTargets);
	
	return	kTargets;
end
function archer_shot_OnFindTarget(actor,action,kTargets)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iAttackRange = action:GetSkillRange(0,actor)

	local	kParam = FindTargetParam();
	
	kParam:SetParam_1(actor:GetTranslate(),actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,20,0,1);
	kParam:SetParam_3(true,FTO_NORMAL);
	action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
	
	return	kTargets:size();
	
end
function archer_shot_ToFloatAttack(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end	

	if (Act_Melee_IsToLeftAttack(actor,action) and actor:IsToLeft()) or 
		(Act_Melee_IsToRightAttack(actor,action) and actor:IsToLeft() == false)   then
	    
		if actor:GetPilot():IsCorrectClass(CT_RANGER) 
			and action:CheckCanEnter(actor,"a_ran_floatdownshot") then
		
			local	iShotCount = tonumber(actor:GetParam("RANGER_FLOATDOWNSHOT_COUNT"));
			if iShotCount==nil then
				iShotCount = 0
			end
	
			ODS("archer_shot_ToFloatAttack iShotCount:"..iShotCount.."\n");
			
			if iShotCount<5 then
			
				ODS("archer_shot_ToFloatAttack To Ranger FloatDownShot\n");
				actor:SetComboCount(0);	
				action:SetNextActionName("a_ran_floatdownshot");
				action:ChangeToNextActionOnNextUpdate(true, true)
				return	true;		
				
			end
		end
	    
	end
	
	if actor:GetJumpTime()>0.5 or Act_Melee_IsToDownAttack(actor,action) then
		if action:CheckCanEnter(actor,"a_archer_down_shot") then
			actor:SetComboCount(0);	
			action:SetNextActionName("a_archer_down_shot");
			action:ChangeToNextActionOnNextUpdate(true, true)
			return	true;
	    end
	end
	return	false;
end

function archer_shot_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	local	actorID = actor:GetID()
--	local	actionID = action:GetID()
--	local	weapontype = actor:GetEquippedWeaponType();
	
	ODS("archer_shot_OnEnter\n");
	
	--	타겟을 찾자
	local	iTargetCount = 0;

	if actor:IsMyActor() == true then

		if actor:IsMeetFloor() == false then
			if	archer_shot_ToFloatAttack(actor,action)	then
				return	true
			end
		end	

		local	iComboCount = actor:GetComboCount();
		
		ODS("archer_shot iComboCount:"..iComboCount.."\n");
		
		if iComboCount > 0 then
		
			if actor:CanNowConnectToNextComboAttack(1.0) == false 	 then
				iComboCount = 0;
			end	
		end		
		
		if Act_Melee_IsToDownAttack(actor,action) then
			if	Act_Melee_ToDownAttack(actor,action) then
				return	true
			end
		end		
		
		if Act_Melee_IsToUpAttack(actor,action) then
			if Act_Melee_ToDownAttack(actor,action,true) then
				return	true
			end
		end		
		
		-- 일반 발사가 변형 될때
		if(0 < actor:GetAbil(AT_SKILL_EXTENDED)) then 
			--공통 부분
			actor:SetComboCount(0)
			action:ClearTargetList()
			
			if actor:GetAbil(AT_SKILL_EXTENDED) == 1 then	--	난사 발사!!			
				if 0 == Config_GetValue(HEADKEY_ETC, "USE_OLD_COMBO") then
					actor:ReserveTransitAction("a_cbw_combo_a_Multishot");
					actor:ResetAnimation();
					return false;
				end
				action:SetNextActionName("a_Rapidly Shot_Fire")
			elseif actor:GetAbil(AT_SKILL_EXTENDED) == 2 then	--	투액션 샷 발사
				if 0 == Config_GetValue(HEADKEY_ETC, "USE_OLD_COMBO") then
					actor:ReserveTransitAction("a_cbw_combo_a_twoaction");
					actor:ResetAnimation();
					return false;
				end
				action:SetNextActionName("a_twoaction_fire")
			end
			--공통 부분
			action:ChangeToNextActionOnNextUpdate(true, true)
			return	true
		end
			
		-- New SkillCombo System -----------------------------------------------------------
		if 0 == Config_GetValue(HEADKEY_ETC, "USE_OLD_COMBO") then
			local	iBaseActorType = actor:GetPilot():GetBaseClassID()
			local iWeaponType = actor:GetEquippedWeaponType();
			local bUseCombo = false
			if iBaseActorType == CT_ARCHER then
				if actor:IsMeetFloor() == true then
					if 5 == iWeaponType then
						actor:ReserveTransitAction("a_bw_combo_a");
						bUseCombo = true
					elseif 6 == iWeaponType then
						actor:ReserveTransitAction("a_cbw_combo_a");
						bUseCombo = true
					end
				end
			end
			if true == bUseCombo then
				actor:ResetAnimation();
				return false;
			end
		end
		-- End : New SkillCombo System --------------------------------------------------------		
			
		local	iNextComboCount = iComboCount+1;
		
		actor:SetComboCount(iNextComboCount);
		ODS("Archer Shot ComboCount : "..iNextComboCount.."\n");
		
			
		--	마지막 발사시간 가져오기
		local	fLastShotTime = tonumber(actor:GetParam("ARCHER_SHOT_TIME"));
		if fLastShotTime == nil then
			fLastShotTime = 0
		end
		
		local iAttackCoolTime = g_fArcherDefaultAttackCoolTime + (actor:GetAbil(AT_ADD_ATTACK_COOL_TIME) / 1000);
		if iAttackCoolTime < 0 then
			iAttackCoolTime = 0.1;
		end

		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
			ODS("g_fArcherDefaultAttackCoolTime : " ..iAttackCoolTime .. "\n");
		if	g_world:GetAccumTime() - fLastShotTime < iAttackCoolTime then
			ODS("Shot Time Not Over\n");
			action:ChangeToNextActionOnNextUpdate(true,true);
			return	true;	
		end
		
		actor:SetParam("FLOAT_SHOT_COUNT","0");
		
        GetComboAdvisor():OnNewActionEnter(action:GetID());
        GetComboAdvisor():AddNextAction(action:GetID());
        
        --	발사 시간 기록
        actor:SetParam("ARCHER_SHOT_TIME",""..g_world:GetAccumTime());
	end
	

	actor:ResetAnimation();
	action:SetParamInt(6,0);
	
	actor:StopJump();
	
	return true
end

function	archer_shot_LoadArrowToWeapon(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	kProjectileMan = GetProjectileMan();
	local	kNewArrow = kProjectileMan:CreateNewProjectile(actor:GetEquippedWeaponProjectileID(),action,actor:GetPilotGuid());

	local	kPosition = actor:GetNodeWorldPos("p_ef_heart");
	local	kDir = actor:GetLookingDir();
	local	fMoveDistance = 30.0;
	
	kDir:Multiply(fMoveDistance);
	kPosition:Add(kDir);
	
	kNewArrow:SetWorldPos(kPosition);
	
	return	kNewArrow;
	
end

function archer_shot_Finished(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("archer_shot_Finished\n");

	if actor:IsMeetFloor() == false then
	    action:SetNextActionName("a_jump");
	end
	
		
	local	nextActionName = action:GetNextActionName()
--	local	iMaxActionSlot = action:GetSlotCount();
--	local	actionID = action:GetID();
			
	actor:SetNormalAttackEndTime();

		
--	local	curActionSlot = action:GetCurrentSlot()
	local	curActionParam = action:GetParam(0)
	if curActionParam == "GoNext" then
		nextActionName = action:GetParam(1);

		action:SetParam(0, "null")
		action:SetParam(1, "end")
		
		action:SetNextActionName(nextActionName)
		return false;
	else

		action:SetParam(1, "end")

		return false
	end
	
	return	true;
end

function archer_shot_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
--	local	actorID = actor:GetID()
	
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	animDone = actor:IsAnimationDone()
	local	weapontype = actor:GetEquippedWeaponType();
	
	if animDone == true then
		return	archer_shot_Finished(actor,action);
	end
	
	return true
end
function archer_shot_OnCleanUp(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("archer_shot_ReallyLeave nextaction : "..action:GetID().."\n");
	
	actor:FreeMove(false);
	
	return true;
end
function archer_shot_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local curAction = actor:GetAction();
	
	if( CheckNil(nil==curAction) ) then return true end
	if( CheckNil(curAction:IsNil()) ) then return true end
	
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	ODS("archer_shot_OnLeave NEXTACTION : "..actionID.." curParam:"..curParam.."\n");

	if curParam == "end" and actionID == "a_jump" then 
		action:SetSlot(2)
		action:SetDoNotBroadCast(true)
		return  true;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end	
	
	if actionID == "a_archer_backstep_shot" then
		return	true
	end
	
	local	kNextActionName = curAction:GetNextActionName();
	if action:GetEnable() == false then
		
		if kNextActionName==actionID then
			curAction:SetNextActionName("a_battle_idle");
			return false;
		end
		
	end
		
	if action:GetEnable() == true then
		
		if curParam == "end" then 
			return true;
		end

		if curAction:GetParamInt(7)==1 and actionID == "a_run" then	--	발사한 후에 이동은 즉시 이동하도록 한다.
			return	true;
		end
		
		if curAction:GetParamInt(7)==1 or actionID == "a_run" then
			curAction:SetNextActionName(actionID);
			curAction:SetNextActionInputSlotInfo(action:GetInputSlotInfo());
        end

		return false;
	
	elseif (curParam == "end" or curAction:GetParamInt(7)==1) and
		(actionID == "a_run") then
		return true;
	elseif actionID == "a_idle" or
		actionID == "a_die" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function archer_shot_DoEffect(actor)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
    actor:FreeMove(false);

	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then	--	아직 DB 입력이 안되었으므로...
		iAttackRange = 200
	end
	
	action:SetParam(4,"HIT");
	
	
--	local	kProjectileMan = GetProjectileMan();
	local	kArrow = archer_shot_LoadArrowToWeapon(actor,action);
	

--	local	targetobject = nil;

	if kArrow:IsNil() == false then
			
		local iPentrationCount = actor:GetAbil(AT_PENETRATION_COUNT);
		kArrow:SetPenetrationCount(iPentrationCount);-- 몇번을 관통할것인지 세팅
		if iPentrationCount ~= 0 then
			kArrow:SetMultipleAttack(true);
		end
			
		local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
		local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
	
		local kAEffect = actor:GetAttackEffect(action:GetID())
		if false==kAEffect:IsNil() and ""~=kAEffect:GetEffectName() then
			actor:AttachParticleS(421,kAEffect:GetNodeName(),kAEffect:GetEffectName(),kAEffect:GetScale())
		end
		
		kArrow:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp());
		
			
		kMovingDir:Multiply(iAttackRange-45);
		kMovingDir:Add(arrow_worldpos);
		kArrow:SetTargetLoc(kMovingDir);	
		
		kArrow:Fire();
		actor:ClearIgnoreEffectList();
	
	end
	
	
end

function archer_shot_OnEvent(actor, textKey)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("archer_shot_OnEvent textKey:"..textKey.."\n");
	if textKey == "fire" or textKey=="hit" or textKey == "end" then
	
		if action:GetParamInt(7) == 0 then
		
			action:SetParamInt(7,1);
			archer_shot_DoEffect(actor);	
	
		end

	end
end

function archer_shot_OnCastingCompleted(actor, action)
end