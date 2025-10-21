
function Sum_Rifle_Shot_OnCheckCanEnter(actor,action)
	
	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end	
	
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
function Sum_Rifle_Shot_FindTarget(kActor,kAction)
	
	if( CheckNil(nil==kActor) ) then return nil end
	if( CheckNil(kActor:IsNil()) ) then return nil end
	
	if( CheckNil(nil==kAction) ) then return nil end
	if( CheckNil(kAction:IsNil()) ) then return nil end
	
	local	kTargets = CreateActionTargetList(kActor:GetPilotGuid(),kAction:GetActionInstanceID(),kAction:GetActionNo());
	local	iFound = Sum_Rifle_Shot_OnFindTarget(kActor,kAction,kTargets);
	
	return	kTargets;
end
function Sum_Rifle_Shot_OnFindTarget(actor,action,kTargets)
	
	if( CheckNil(nil==actor) ) then return 0 end
	if( CheckNil(actor:IsNil()) ) then return 0 end
	
	if( CheckNil(nil==action) ) then return 0 end
	if( CheckNil(action:IsNil()) ) then return 0 end
	
	local	iAttackRange = action:GetSkillRange(0,actor)

	local	kParam = FindTargetParam();
	
	kParam:SetParam_1(actor:GetTranslate(),actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,20,0,1);
	kParam:SetParam_3(true,FTO_NORMAL);
	action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
	
	return	kTargets:size();
	
end
function Sum_Rifle_Shot_ToFloatAttack(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end	

	if Act_Melee_IsToDownAttack(actor,action) then
		kNextActionName = "a_Air_Smash";
	else
		kNextActionName = "a_Sum_Rifle_Jump_Shot_01";
				
	end
	
	
	if action:CheckCanEnter(actor,kNextActionName) then
		action:SetNextActionName(kNextActionName);
		action:ChangeToNextActionOnNextUpdate(true, true)
		return	true;
	end
	return	false;
end

function Sum_Rifle_Shot_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Sum_Rifle_Shot_OnEnter\n");
	
	if LOCALE.NC_DEVELOP == GetLocale() then
		if 0 == Config_GetValue(HEADKEY_ETC, "USE_OLD_COMBO") then
			actor:ReserveTransitAction("a_gs_combo_a");
			actor:ResetAnimation();
			return false;
		end
	end
	
	--	타겟을 찾자
	local	iTargetCount = 0;

	if actor:IsMyActor() == true then

		if actor:IsMeetFloor() == false then
			if	Sum_Rifle_Shot_ToFloatAttack(actor,action)	then
				return	true
			else 
				return false
			end
		end	

		local	iComboCount = actor:GetComboCount();
		if iComboCount > 0 then
		
			if actor:CanNowConnectToNextComboAttack(1.0) == false 	 then
				iComboCount = 0;
			end	
		end		
		
		if Act_Melee_IsToDownAttack(actor,action) then
			if	Act_Melee_ToDownAttack(actor,action) then
				return	true
			end
		elseif Act_Melee_IsToUpAttack(actor,action) then
			if Act_Melee_ToDownAttack(actor,action,true) then
				return	true
			end
		end
		
		-- 일반 발사가 변형 될때
		local iExtendSkillNo =  actor:GetAbil(AT_SKILL_EXTENDED)
		if(0 < actor:GetAbil(AT_SKILL_EXTENDED)) then 
			actor:SetComboCount(0)
			action:ClearTargetList()	
			if 0 == Config_GetValue(HEADKEY_ETC, "USE_OLD_COMBO") then
				actor:ReserveTransitAction("a_smart_shot_fire_combo_a");
				actor:ResetAnimation();
				return false;
			end
			actor:ReserveTransitActionByActionNo(iExtendSkillNo)	
			action:ChangeToNextActionOnNextUpdate(true, true)			
			return	true
		end
	
		if 0 == Config_GetValue(HEADKEY_ETC, "USE_OLD_COMBO") then
			actor:ReserveTransitAction("a_gs_combo_a");
			actor:ResetAnimation();
			return false;
		end
		local	iNextComboCount = iComboCount+1;
		
		actor:SetComboCount(iNextComboCount);
		ODS("Sum_Rifle Shot ComboCount : "..iNextComboCount.."\n");
		
			
		--	마지막 발사시간 가져오기
		local	fLastShotTime = tonumber(actor:GetParam("Sum_Rifle_Shot_TIME"));
		if fLastShotTime == nil then
			fLastShotTime = 0
		end
		
		local iAttackCoolTime = g_fSum_RifleDefaultAttackCoolTime + (actor:GetAbil(AT_ADD_ATTACK_COOL_TIME) / 1000);
		if iAttackCoolTime < 0 then
			iAttackCoolTime = 0.1;
		end

		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		ODS("g_fSum_RifleDefaultAttackCoolTime : " ..iAttackCoolTime .. "\n");
		if	g_world:GetAccumTime() - fLastShotTime < iAttackCoolTime then
			ODS("Shot Time Not Over\n");
			action:ChangeToNextActionOnNextUpdate(true,true);
			return	true;	
		end
		
		actor:SetParam("FLOAT_SHOT_COUNT","0");
		
        GetComboAdvisor():OnNewActionEnter(action:GetID());
        GetComboAdvisor():AddNextAction(action:GetID());
        
        --	발사 시간 기록
        actor:SetParam("Sum_Rifle_Shot_TIME",""..g_world:GetAccumTime());
	end
	

	actor:ResetAnimation();
	action:SetParamInt(6,0);
	
	actor:StopJump();
	
	return true
end

function	Sum_Rifle_Shot_LoadArrowToWeapon(actor,action)

	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	
	local	kProjectileMan = GetProjectileMan();
	if action:GetScriptParam("PROJECTILE_ID")~="" then
		kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	end
	
	if kProjectileID == "" then
		kProjectileID = action:GetEquippedWeaponProjectileID();
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());
	
	local	kPosition = actor:GetNodeWorldPos("p_ef_fire");
	local	kDir = actor:GetLookingDir();
	kPosition:Add(kDir);
	
	kNewArrow:SetWorldPos(kPosition);
	
	return	kNewArrow;
	
end

function Sum_Rifle_Shot_Finished(actor,action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Sum_Rifle_Shot_Finished\n");

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

function Sum_Rifle_Shot_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
--	local	actorID = actor:GetID()
	
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	animDone = actor:IsAnimationDone()
	local	weapontype = actor:GetEquippedWeaponType();
	
	if animDone == true then
		return	Sum_Rifle_Shot_Finished(actor,action);
	end
	
	return true
end
function Sum_Rifle_Shot_OnCleanUp(actor,action)
	
	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	
	ODS("Sum_Rifle_Shot_ReallyLeave nextaction : "..action:GetID().."\n");
	
	actor:FreeMove(false);
	
	return true;
end
function Sum_Rifle_Shot_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	ODS("Sum_Rifle_Shot_OnLeave NEXTACTION : "..actionID.." curParam:"..curParam.."\n");

	if curParam == "end" and actionID == "a_jump" then 
		action:SetSlot(2)
		action:SetDoNotBroadCast(true)
		return  true;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
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

		-- if curAction:GetParamInt(7)==1 and actionID == "a_run" then	--	발사한 후에 이동은 즉시 이동하도록 한다.
			-- return	true;
		-- end
		
		if curAction:GetParamInt(7)==1 and actionID == "a_jump" then	--	발사한 후에 점프는 즉시 하도록 한다.
			return	true;
		end
		
		if curAction:GetParamInt(7)==1 
			--or actionID == "a_run" 
		then
			curAction:SetNextActionName(actionID);
			curAction:SetNextActionInputSlotInfo(action:GetInputSlotInfo());
        end

		return false;
	
	 elseif (curParam == "end" or curAction:GetParamInt(7)==1) 
			--and (actionID == "a_run") 
		then
		return true;
	elseif actionID == "a_idle" or
		actionID == "a_die" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function Sum_Rifle_Shot_DoEffect(actor)

	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
    actor:FreeMove(false);

	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	
	local	iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then	--	아직 DB 입력이 안되었으므로...
		iAttackRange = 200
	end
	
	action:SetParam(4,"HIT");
	
	
--	local	kProjectileMan = GetProjectileMan();
	local	kArrow = Sum_Rifle_Shot_LoadArrowToWeapon(actor,action);
	

--	local	targetobject = nil;

	if kArrow:IsNil() == false then
		local kHitEffectTargetNode = action:GetScriptParam("HIT_TARGET_EFFECT_TARGET_NODE")
		kArrow:SetParamValue("HIT_TARGET_EFFECT_TARGET_NODE", kHitEffectTargetNode)
		
		local kHitEffectID = action:GetScriptParam("HIT_TARGET_EFFECT_ID")
		kArrow:SetParamValue("HIT_TARGET_EFFECT_ID", kHitEffectID);
		
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

function Sum_Rifle_Shot_OnEvent(actor, textKey)
	
	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	
	ODS("Sum_Rifle_Shot_OnEvent textKey:"..textKey.."\n");
	if textKey == "fire" then
	
		actor:AttachParticleS(3851, "p_ef_fire", "eff_sum_skill_smn_muzz00", 1.0)
		
		if action:GetParamInt(7) == 0 then
		
			action:SetParamInt(7,1);
			Sum_Rifle_Shot_DoEffect(actor);	
	
		end

	end
end

function Sum_Rifle_Shot_OnCastingCompleted(actor, action)
end