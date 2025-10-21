
-- 대각선아래방향으로 세발의 화살을 동시에 발사!
-- [PROJECTILE_ID] : 발사할 프로젝틸 ID. 빈문자열일 경우 기본 프로젝틸이 발사된다.
-- [ATTACH_NODE] : 프로젝틸을 붙일 노드. 빈문자열일 경우 무기에 붙이게 된다.
function Skill_Down_Shot_OnCastingCompleted(actor,action)
end

function Skill_Down_Shot_OnCheckCanEnter(actor,action)

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
function Skill_Down_Shot_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if actor:IsMyActor() == true then
	
		if actor:GetJumpAccumHeight()<50 then
			action:ChangeToNextActionOnNextUpdate(true,true);
			return	true;	
		end
	
	end
	
--	local	actorID = actor:GetID()
--	local	actionID = action:GetID()
	local	weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_Down_Shot_OnEnter weapontype : "..weapontype.."\n");

	if actor:IsMyActor() == true then
		Skill_Down_Shot_FindTargets(actor,action);
	else
		GetActionResultCS(action, actor)
	end
	
	actor:StopJump();
	actor:FreeMove(true);
	
	return true
end
function Skill_Down_Shot_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	

	local iAttackRange = action:GetSkillRange(0,actor);
	local	arrow_worldpos = actor:GetPos();--kNewArrow:GetWorldPos();	--	화살의 위치
	
-- 회전축 구하기
	local	kRotateAxis = actor:GetLookingDir();
	kRotateAxis:Cross(Point3(0,0,1));
	kRotateAxis:Unitize();
	local	fRotateAngle = 20.0*3.141592/180.0;

	local kParam = FindTargetParam();
	kParam:SetParam_1(actor:GetTranslate(),actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,20,0,1);
	kParam:SetParam_3(true,FTO_NORMAL+FTO_DOWN);		

	local	kFireTargetDir = Point3(0.1,0.1,-1);
	kFireTargetDir:Rotate(kRotateAxis,fRotateAngle);
	kParam:SetParam_1(arrow_worldpos,kFireTargetDir);
	action:FindTargets(TAT_RAY_DIR,kParam,kTargets,kTargets);
	
	kFireTargetDir = Point3(0.1,0.1,-1);
	kFireTargetDir:Rotate(kRotateAxis,fRotateAngle*2);
	kParam:SetParam_1(arrow_worldpos,kFireTargetDir);
	action:FindTargets(TAT_RAY_DIR,kParam,kTargets,kTargets);
	
	kFireTargetDir = Point3(0.1,0.1,-1);
	kFireTargetDir:Rotate(kRotateAxis,fRotateAngle*3);
	kParam:SetParam_1(arrow_worldpos,kFireTargetDir);
	action:FindTargets(TAT_RAY_DIR,kParam,kTargets,kTargets);
	
	return	kTargets:size();
end

function Skill_Down_Shot_FindTargets(actor,action)

    action:CreateActionTargetList(actor);
	
end

function Skill_Down_Shot_LoadToWeapon(actor,action)

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
	return kNewArrow;
end

function Skill_Down_Shot_OnUpdate(actor, accumTime, frameTime)

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

	if animDone == true then
	
--		local	curActionSlot = action:GetCurrentSlot()
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
function Skill_Down_Shot_OnCleanUp(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:FreeMove(false);
	
	return true;
end
function Skill_Down_Shot_OnLeave(actor, action)

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
	if action:GetActionType()=="EFFECT"  then
		return true;
	end	
	if actionID == "a_jump" then 
		return false;
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

function Skill_Down_Shot_OnEvent(actor, textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	if textKey == "fire" or textKey=="hit" then
		local	action = actor:GetAction()
		
		CheckNil(nil==action);
		CheckNil(action:IsNil());
		
		local	iAttackRange = action:GetSkillRange(0,actor);
		actor:FreeMove(false);	
		
		
		
--		local	kProjectileMan = GetProjectileMan();
		
--		local	iTargetCount = action:GetTargetCount();
		local	iTargetABVShapeIndex = 0;

		-- 회전축 구하기
		local	kRotateAxis = actor:GetLookingDir();
		kRotateAxis:Cross(Point3(0,0,1));
		kRotateAxis:Unitize();
		fRotateAngle = 20.0*3.141592/180.0;			
		local	i = 0;
		
		local	kActionTargetList = action:GetTargetList();
		
		while i<3 do
		
			local	kTargetActor = nil;
			local	kActionTargetInfo = kActionTargetList:GetTargetInfo(0);
			iTargetABVShapeIndex = 0;
			
			if kActionTargetInfo:IsNil() == false then
			
				iTargetABVShapeIndex = kActionTargetInfo:GetABVIndex();
				local	kTargetPilot = g_pilotMan:FindPilot(kActionTargetInfo:GetTargetGUID());
				if kTargetPilot:IsNil() == false then
					kTargetActor = kTargetPilot:GetActor();
				end			
				
			end

			local	kArrow = Skill_Down_Shot_LoadToWeapon(actor,action);
			local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
		
			if kTargetActor ~= nil and kTargetActor:IsNil() == false and kActionTargetInfo~=nil and kActionTargetInfo:IsNil() == false then
				kArrow:SetTargetObject(kActionTargetInfo);	
				kActionTargetList:DeleteTargetInfo(0);
			end
			
			local	kFireTarget = Point3(0.1,0.1,-1);
			kFireTarget:Multiply(iAttackRange);
			kFireTarget:Rotate(kRotateAxis,fRotateAngle*(i+1));
			kFireTarget:Add(arrow_worldpos);			
			kArrow:SetTargetLoc(kFireTarget);	
			kArrow:Fire();--	발사!

			i=i+1;
		end
		
		actor:GetAction():SetParam(7,"FIRED");
		actor:ClearIgnoreEffectList();

	end
end
