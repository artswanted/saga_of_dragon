function MagicianShot_OnCastingCompleted(actor, action)
end

function MagicianShot_OnCheckCanEnter(actor,action)
	if nil==actor or actor:IsNil() then
		return false
	end
	if nil==action or action:IsNil() then
		return false
	end
	   

    ODS("MagicianShot_OnCheckCanEnter\n");
    
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	
	if actor:IsMeetFloor() == false then
	
		if 	actor:GetFloatHeight(30)<40 then
		
			return	false;	
			
		end
		

	end
	
	-- 안전거품 공격 막기
	if g_pilotMan:IsLockMyInput() == true then
		return
	end		
			
	return		true;
	
end
function MagicianShot_FindTarget(kActor,kAction)
	if nil==actor or actor:IsNil() then
		return false
	end
	if nil==action or action:IsNil() then
		return false
	end
	   

	local	kTargets = CreateActionTargetList(kActor:GetPilotGuid(),kAction:GetActionInstanceID(),kAction:GetActionNo());
	local	iFound = MagicianShot_OnFindTarget(kActor,kAction,kTargets);
	
	return	kTargets;
end
function MagicianShot_OnFindTarget(actor,action,kTargets)
	if nil==actor or actor:IsNil() then
		return false
	end
	if nil==action or action:IsNil() then
		return false
	end
	   

	local	iAttackRange = action:GetSkillRange(0,actor)

	local	kParam = FindTargetParam();
	

	kParam:SetParam_1(actor:GetTranslate(),actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,g_fMagicianShot_BarWidth,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
	
	return	kTargets:size();
	
end

function MagicianShot_OnEnter(actor, action)
	if nil==actor or actor:IsNil() then
		return false
	end
	if nil==action or action:IsNil() then
		return false
	end

	local	actorID = actor:GetID()
	local	actionID = action:GetID()
	local	weapontype = actor:GetEquippedWeaponType();
	
	ODS("MagicianShot_OnEnter\n");
	
	if actor:IsMyActor() == true then
				
		if actor:IsMeetFloor() == false then	
			if Act_Melee_ToFloatAttack(actor,action) then
				return	true
			end
			return	false;
		end
		
		if Act_Melee_IsToDownAttack(actor,action) then
			if	Act_Melee_ToDownAttack(actor,action) then
				return	true
			end
		end		
		if Act_Melee_IsToUpAttack(actor,action) then
			if	Act_Melee_ToBlowUpAttack(actor,action) then
				return	true
			end
		end		
		
		local	iComboCount = actor:GetComboCount();
		if iComboCount > 0 then
			if actor:CanNowConnectToNextComboAttack(0.2) == false 	 then
				actor:SetComboCount(0);	
			end	
		end		
	
		if actor:GetAbil(AT_SKILL_EXTENDED) == 1 then	--	확산포 발사!!
			--	마나 체크
			if actor:GetAbil(AT_MP)>= action:GetAbil(AT_NEED_MP) then
				actor:SetComboCount(0);	
				action:ClearTargetList();
				if 0 == Config_GetValue(HEADKEY_ETC, "USE_OLD_COMBO") then
					actor:ReserveTransitAction("a_st_combo_a_ThreeWay");
					actor:ResetAnimation();
					return false;
				end
				action:SetNextActionName("a_Three Way_Fire");
				action:ChangeToNextActionOnNextUpdate(true, true)	
				return	true;
			end
		elseif actor:GetAbil(AT_SKILL_EXTENDED) == 2 then -- 더블샷 발사
			if action:CheckCanEnter(actor,"a_mag_doubleshot") then
				if 0 == Config_GetValue(HEADKEY_ETC, "USE_OLD_COMBO") then
					actor:ReserveTransitAction("a_st_combo_a_Doubleshot");
					actor:ResetAnimation();
					return false;
				end
				action:SetNextActionName("a_mag_doubleshot");
				action:ChangeToNextActionOnNextUpdate(true, true)	
				return	true;	
			end
			
		end
		
		--	만약 배틀메이지라면, 근접한 적이 있을 경우, 근접타격을 한다.
		if actor:GetPilot():IsCorrectClass(CT_BATTLEMAGE) then
			if action:CheckTargetExist(actor,"a_mag_melee_01") and
				action:CheckCanEnter(actor,"a_mag_melee_01") then
				action:SetNextActionName("a_mag_melee_01");
				action:ChangeToNextActionOnNextUpdate(true, true)	
				
				return	true;	
			end
		end

		if 0 == Config_GetValue(HEADKEY_ETC, "USE_OLD_COMBO") then
			local iWeaponType = actor:GetEquippedWeaponType();
			local bUseCombo = false	
			if 3 == iWeaponType then
				actor:ReserveTransitAction("a_st_combo_a");
				bUseCombo = true
			elseif 4 == iWeaponType then
				actor:ReserveTransitAction("a_sp_combo_a");
				bUseCombo = true
			end
			if true == bUseCombo then
				actor:ResetAnimation();
				return false;
			end
		end
	end
	
	local iTargetCount = action:GetTargetCount();
	ODS("MagicianShot_Fire iTargetCount : "..iTargetCount.."\n");	
	actor:SetComboCount(0);
	
	if iTargetCount>0 then
			--	타겟을 찾아 바라본다
		local	kTargetGUID = action:GetTargetGUID(0);
		local	targetobject = g_pilotMan:FindPilot(kTargetGUID);
		local	iTargetABVShapeIndex = action:GetTargetABVShapeIndex(0);
		if targetobject:IsNil() == false then
			local pt = targetobject:GetActor():GetABVShapeWorldPos(iTargetABVShapeIndex);
			actor:LookAt(pt);
		end
	end

	actor:ResetAnimation();
		
	action:SetParamInt(6,0);
	action:SetParamInt(7,1);
	
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());	
	    GetComboAdvisor():AddNextAction(action:GetID());	
	end	
	return true
end

function	MagicianShot_LoadArrowToWeapon(actor,action)
	if( CheckNil(nil==actor) ) then return nil end
	if( CheckNil(actor:IsNil()) ) then return nil end
	
	if( CheckNil(nil==action) ) then return nil end
	if( CheckNil(action:IsNil()) ) then return nil end
	
	local	kProjectileMan = GetProjectileMan();
	local 	kProjID = actor:GetEquippedWeaponProjectileID()
	if nil==kProjID or ""==kProjID then
		kProjID = "PROJECTILE_MAGICIAN_DEFAULT_SHOT"
		local weapontype = actor:GetEquippedWeaponType()
		if nil~=weapontype and WT_SPR==weapontype then	--전투마법사 계열은 스피어
			kProjID = "PROJECTILE_MAGE_DEFAULT_SHOT"
		end
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjID,action,actor:GetPilotGuid());

	local	kPosition = actor:GetNodeWorldPos("p_ef_heart");
	local	kDir = actor:GetLookingDir();
	local	fMoveDistance = 30.0;
	
	kDir:Multiply(fMoveDistance)
	kPosition:Add(kDir)
	
	kNewArrow:SetWorldPos(kPosition)

	return	kNewArrow;	
end

function MagicianShot_Finished(actor,action)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("MagicianShot_Finished\n");
	
	local	nextActionName = action:GetNextActionName()
	local	iMaxActionSlot = action:GetSlotCount();
	local	actionID = action:GetID();
			
	actor:SetNormalAttackEndTime();
	
		

	local	curActionSlot = action:GetCurrentSlot()
	local	curActionParam = action:GetParam(0)
	if curActionParam == "GoNext" then
		nextActionName = action:GetParam(1);

		action:SetParam(0, "null")
		action:SetParam(1, "end")
		return false;
	else

		action:SetParam(0, "null")
		action:SetParam(1, "end")
		
		return false
	end
	
	return	true;
end

function MagicianShot_OnUpdate(actor, accumTime, frameTime)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local	action = actor:GetAction()
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	actorID = actor:GetID()
	local	animDone = actor:IsAnimationDone()
	local	weapontype = actor:GetEquippedWeaponType();
	
	if animDone == true then
		return	MagicianShot_Finished(actor,action);
	end
	
	
	local fMovingSpeed = 0;

	if IsSingleMode() == true then
		fMovingSpeed = 120
	else
	    fMovingSpeed = actor:GetAbil(AT_C_MOVESPEED)
	end	

	local dir = actor:GetDirection()
	if dir ~= DIR_NONE then
		actor:Walk(dir, fMovingSpeed)
	end

	return true
end
function MagicianShot_OnCleanUp(actor)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	ODS("MagicianShot_ReallyLeave\n");

	--  만약 아직 발사하지 않은 상태라면 강제로 발사한다(Text Key Event 가 제대로 발생을 안해서 그런거같은데...흠...)
	if actor:GetAction():IsNil() == false then
	
	    if actor:GetAction():GetParamInt(7) == 1 then
			actor:GetAction():SetParamInt(7,2);
			MagicianShot_DoEffect(actor);
	    end
	
	end	
	return true;
end
function MagicianShot_OnLeave(actor, action)
	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local curAction = actor:GetAction();
	if( CheckNil(nil==curAction) ) then return true end
	if( CheckNil(curAction:IsNil()) ) then return true end
	
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	ODS("MagicianShot_OnLeave NEXTACTION : "..actionID.." curParam:"..curParam.."\n");
	if actor:IsMyActor() == false then
		return true;
	end
	if actionID == "a_jump" then 
		return false;
	end	
	
	if actionID =="a_magician_charge_laser" then
		return true;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end	
	
	local	kNextActionName = curAction:GetParam(1);
	if action:GetEnable() == false then
		
		if kNextActionName==actionID then
			curAction:SetParam(0,"");
		end
		
	end
		
	if action:GetEnable() == true then
		
		if curParam == "end" then 
			return true;
		end
		
		--if actionID == "a_MagicianShot_01" then
		--	return	false;
		--end
		
		--if curAction:GetParam(4)~="" then
			curAction:SetParam(0,"GoNext");
			curAction:SetParam(1,actionID);
		--end
		
		if curAction:GetParam(4) == "HIT" and
		    actionID=="a_run" then
		    if KeyIsDown(KEY_LEFT) or KeyIsDown(KEY_RIGHT) then
		        return  true;
		    end
		end		    
	

		return false;
	
	elseif curParam == "end" and
		(actionID == "a_run_right" or
		actionID == "a_run_left") then
		return true;
	elseif actionID == "a_idle" or
		actionID == "a_die" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function MagicianShot_DoEffect(actor)
	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	local	action = actor:GetAction()
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	local	iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then	--	아직 DB 입력이 안되었으므로...
		iAttackRange = 200
	end
	
	actor:ClearIgnoreEffectList();
	
	action:SetParam(4,"HIT");
	
	
	local	kProjectileMan = GetProjectileMan();
	local	kArrow = MagicianShot_LoadArrowToWeapon(actor,action);
	

	local	targetobject = nil;

	if kArrow:IsNil() == false then
	
		local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
		local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
	
		local szEffName = "shot_rebound_01"
		local szNode = "char_root"
		local kScale = 1.0
		local kAEffect = actor:GetAttackEffect(action:GetID())
		if false==kAEffect:IsNil() and ""~=kAEffect:GetEffectName() then
			szEffName = kAEffect:GetEffectName()
			szNode = kAEffect:GetNodeName()
			kScale = kAEffect:GetScale()
		end

		actor:AttachParticleS(421,szNode,szEffName,kScale)
		
		kArrow:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp());
		
		kMovingDir:Multiply(iAttackRange-45);
		kMovingDir:Add(arrow_worldpos);
		kArrow:SetTargetLoc(kMovingDir);	
		
		kArrow:Fire();
	
	end
	
	
end

function MagicianShot_OnEvent(actor, textKey)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	ODS("MagicianShot_OnEvent textKey:"..textKey.."\n");
	if textKey == "fire" or textKey=="hit"then
		local kCurAction = actor:GetAction();
		if( CheckNil(kCurAction:IsNil()) ) then return false end

		if kCurAction:GetParamInt(7) == 1 then
		
			kCurAction:SetParamInt(7,2);
			MagicianShot_DoEffect(actor);
		
		end
		


	end
end
