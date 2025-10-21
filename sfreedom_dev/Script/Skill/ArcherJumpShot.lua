
-- 대각선아래방향으로 세발의 화살을 동시에 발사!
-- [PROJECTILE_ID] : 발사할 프로젝틸 ID. 빈문자열일 경우 기본 프로젝틸이 발사된다.
-- [ATTACH_NODE] : 프로젝틸을 붙일 노드. 빈문자열일 경우 무기에 붙이게 된다.
-- [HorizMoveSpeed] : 수평으로 이동할 속도
-- [ARROW_ROTATE] : 화살 발사 회전 각도 Degree
-- [ATTCK_DOWN_STATE] : 넘어져있는 놈도 공격할수 있는가

function Archer_Jump_Shot_OnCastingCompleted(actor, action)
end

function Archer_Jump_Shot_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local kCurAction = actor:GetAction()
	if( false == kCurAction:IsNil() ) then
		local weapontype = actor:GetEquippedWeaponType();
		if weapontype == 0 then
			kCurAction:SetNextActionName("a_idle")
			return false
		end
	end

	return true
	
end
function Archer_Jump_Shot_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local	actorID = actor:GetID()
--	local	actionID = action:GetID()
	local	weapontype = actor:GetEquippedWeaponType();
	
	ODS("Archer_Jump_Shot_OnEnter weapontype : "..weapontype.." ComboCount:"..actor:GetComboCount().."\n");

	--Archer_Jump_Shot_LoadToWeapon(actor,action);
	
	if actor:IsMyActor() == true then
	
		actor:SetComboCount(0);	
	
	end
	
	actor:SetComboCount(0);

	return true
end
function Archer_Jump_Shot_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iAttackRange = action:GetSkillRange(0,actor)

	local	kParam = FindTargetParam();
	
	kParam:SetParam_1(actor:GetTranslate(),actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,20,0,1);
	kParam:SetParam_3(true,FTO_NORMAL);
	action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
	
	return	kTargets:size();
end

function Archer_Jump_Shot_FindTargets(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    action:CreateActionTargetList(actor,true);
	
end

function Archer_Jump_Shot_LoadToWeapon(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kProjectileID = actor:GetEquippedWeaponProjectileID();
	local	kAttackNodeName = "";
	
	if action:GetScriptParam("PROJECTILE_ID")~="" then
		kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	end
	
	kAttackNodeName = "p_ef_heart";
	
	local	kPilotGuid = actor:GetPilotGuid();
	local	kProjectileMan = GetProjectileMan();
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,kPilotGuid);
	if kAttackNodeName == "" then
		kNewArrow:LoadToWeapon(actor);	--	장전
	else
		kNewArrow:LoadToHelper(actor,kAttackNodeName);
	end

	return	kNewArrow;
end

function Archer_Jump_Shot_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local	actorID = actor:GetID()
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	animDone = actor:IsAnimationDone()
	local	nextActionName = action:GetNextActionName()
--	local	iMaxActionSlot = action:GetSlotCount();
--	local	weapontype = actor:GetEquippedWeaponType();
--	local	actionID = action:GetID();
	
	if animDone then
	
		local	curActionSlot = action:GetCurrentSlot()
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
	end

	return true
end
function Archer_Jump_Shot_OnCleanUp(actor)
	return true;
end
function Archer_Jump_Shot_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	
	local	curParam = curAction:GetParam(1)
	local	actionID = action:GetID()
	
	if actor:IsMyActor() == false then
		return true;
	end
	if action:GetActionType()=="EFFECT" then
		return true;
	end	
	
	if action:GetEnable() then
		ODS("Archer_Jump_Shot_OnLeave action:"..action:GetID().." Enable\n");
	else
		ODS("Archer_Jump_Shot_OnLeave action:"..action:GetID().." Disable\n");
	end
	
	if action:GetEnable() == false then
		
		if curAction:GetParam(1)==actionID then
			curAction:SetParam(0,"");
		end
		
	end	
	if action:GetEnable() == true then
		
		if curParam == "end" then 
			return true;
		end
		
		if actionID == "a_archer_shot_01" then
		
		
		    if Act_Melee_IsToUpAttack(actor,action) then
		    
		        actionID = "a_ArcherFloatShot_01"
		    else
		        return false;
		    end
		
        end	
		curAction:SetParam(0,"GoNext");
		curAction:SetParam(1,actionID);
		
		return false;
	
	elseif curParam == "end" and
		(actionID == "a_run_right" or
		actionID == "a_run_left") then
		return true;
	elseif 	actionID == "a_idle" or
		actionID == "a_die" or
		actionID == "a_handclaps" then
		return true;
	end

	return false 
end

function Archer_Jump_Shot_OnEvent(actor, textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
    ODS("Archer_Jump_Shot_OnEvent textKey : "..textKey.."\n");

	if textKey == "fire" or textKey=="hit" then
	
	
		local	action = actor:GetAction()
		
		CheckNil(nil==action);
		CheckNil(action:IsNil());
	
	    if action:GetParam(7) == "FIRED" then
	        return
        end

		
		local	iAttackRange = action:GetSkillRange(0,actor);
			
		local	kProjectileMan = GetProjectileMan();
		
		local	iTargetCount = action:GetTargetCount();
		local	iTargetABVShapeIndex = 0;

		local	i = 0;
		
		Archer_Jump_Shot_FindTargets(actor,action);
		
		local	kActionTargetList = action:GetTargetList();
		
		local	kTargetActor = nil;
		local	kActionTargetInfo = kActionTargetList:GetTargetInfo(0);
		iTargetABVShapeIndex = 0;
		
		local pt = nil;
		
		if kActionTargetInfo:IsNil() == false then
		
			iTargetABVShapeIndex = kActionTargetInfo:GetABVIndex();
			local	kTargetPilot = g_pilotMan:FindPilot(kActionTargetInfo:GetTargetGUID());
			if kTargetPilot:IsNil() == false then
				kTargetActor = kTargetPilot:GetActor();
				
				pt = kTargetActor:GetNodeWorldPos("char_root");
				
			end			
			
		end

		local	kArrow = Archer_Jump_Shot_LoadToWeapon(actor,action);
		local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
		
		if pt == nil then
		    local	kFireTarget = actor:GetLookingDir();
		    kFireTarget:Multiply(iAttackRange-50);
		    kFireTarget:Add(arrow_worldpos);			
		    kArrow:SetTargetLoc(kFireTarget);	
		else
    		kArrow:SetTargetLoc(pt);	
		end
		
		kArrow:Fire();--	발사!

		
		action:SetParam(7,"FIRED");
		kActionTargetList:clear();
		actor:ClearIgnoreEffectList();
		
	end
end
