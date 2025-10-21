
function Sum_Rifle_Jump_Shot_OnCastingCompleted(actor, action)
end

function Sum_Rifle_Jump_Shot_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local kCurAction = actor:GetAction()
	if( false == kCurAction:IsNil() ) then
		if actor:GetJumpAccumHeight() < 50 then
			kCurAction:SetNextActionName("a_idle")
			return false
		end
		
		local weapontype = actor:GetEquippedWeaponType();
		if weapontype == 0 then
			kCurAction:SetNextActionName("a_jump")
			kCurAction:SetSlot(2)
			return false
		end
	end

	return true

end

function Sum_Rifle_Jump_Shot_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	return true
end

function Sum_Rifle_Jump_Shot_OnFindTarget(actor,action,kTargets)
	
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

function Sum_Rifle_Jump_Shot_FindTargets(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    action:CreateActionTargetList(actor,true);
	
end

function Sum_Rifle_Jump_Shot_LoadToWeapon(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kProjectileID = actor:GetEquippedWeaponProjectileID();
	local	kAttackNodeName = "";
	
	if action:GetScriptParam("PROJECTILE_ID")~="" then
		kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	end
	
	kAttackNodeName = "p_ef_fire";
	
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

function Sum_Rifle_Jump_Shot_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	animDone = actor:IsAnimationDone()
	
	if animDone == true then
		--actor:FreeMove(false);	-- 이제 공중에서 떨어지거나, 움직일수 있음
		
		if actor:IsMeetFloor() == false then
			action:SetNextActionName("a_jump");
		end
			
		local	nextActionName = action:GetNextActionName()
		actor:SetNormalAttackEndTime();

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

function Sum_Rifle_Jump_Shot_OnCleanUp(actor)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	return true;
end

function Sum_Rifle_Jump_Shot_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	
	local	curParam = curAction:GetParam(1)
	local	actionID = action:GetID()
	
	ODS("Sum_Rifle_Shot_OnLeave actionID:"..actionID.."\n", 3851);
	
	if actor:IsMyActor() == false then
	
		if actionID == "a_jump" then
			action:SetSlot(2)
		end
	
		return true;
	end
	if action:GetActionType()=="EFFECT"  then
		return true;
	end	
	if curParam == "end" and actionID == "a_jump" then 
		action:SetSlot(2)
		return  true;
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
		
		if actionID == "a_Sum_Rifle_Shot" then
		    return  false
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

function Sum_Rifle_Jump_Shot_OnEvent(actor, textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
    ODS("Sum_Rifle_Jump_Shot_OnEvent textKey : "..textKey.."\n");

	if textKey == "fire" or textKey=="hit" then
	
		actor:AttachParticleS(3851, "p_ef_fire", "eff_sum_skill_smn_muzz00", 1.0)
		
		local	action = actor:GetAction()
		
		CheckNil(nil==action);
		CheckNil(action:IsNil());
	
	    if action:GetParam(7) == "FIRED" then
	        return
        end

		local	iAttackRange = action:GetSkillRange(0,actor);
		if iAttackRange == 0 then	--	아직 DB 입력이 안되었으므로...
			iAttackRange = 200
		end
			
		local	kProjectileMan = GetProjectileMan();
		
		local	iTargetCount = action:GetTargetCount();
		local	iTargetABVShapeIndex = 0;

		local	i = 0;
		
		Sum_Rifle_Jump_Shot_FindTargets(actor,action);
		
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
				pt = kTargetActor:GetNodeWorldPos("p_ef_heart");
			end			
		end

		local	kArrow = Sum_Rifle_Jump_Shot_LoadToWeapon(actor,action);
		local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
		local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
	
		local kAEffect = actor:GetAttackEffect(action:GetID())
		if false==kAEffect:IsNil() and ""~=kAEffect:GetEffectName() then
			actor:AttachParticleS(421,kAEffect:GetNodeName(),kAEffect:GetEffectName(),kAEffect:GetScale())
		end
		
		kArrow:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp());
		
		kMovingDir:Multiply(iAttackRange+45);
		kMovingDir:Add(arrow_worldpos);
		kArrow:SetTargetLoc(kMovingDir);	
		
		kArrow:Fire();
		actor:ClearIgnoreEffectList();
		
		kArrow:Fire();--	발사!

		action:SetParam(7,"FIRED");
		kActionTargetList:clear();
		actor:ClearIgnoreEffectList();
		
	end
end
