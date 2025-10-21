
-- 좌우로 세발의 화살을 동시에 발사!
-- [PROJECTILE_ID] : 발사할 프로젝틸 ID. 빈문자열일 경우 기본 프로젝틸이 발사된다.
-- [ATTACH_NODE] : 프로젝틸을 붙일 노드. 빈문자열일 경우 무기에 붙이게 된다.
function Skill_Multi_Shot_OnCheckCanEnter(actor,action)

	if actor:IsMeetFloor() == false then	
		return	false;
	end

	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 1 then
		return	false;
	end

	return		true;
	
end

function Skill_Multi_Shot_OnCastingCompleted(actor, action)
	Skill_Multi_Shot_Fire(actor,action)
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end

function Skill_Multi_Shot_Fire(actor,action)
	action:SetParamInt(5,1)
	
	if actor:IsMyActor() == true then
		Skill_Multi_Shot_FindTargets(actor,action);
	else
		GetActionResultCS(action, actor)	
	end
end

function Skill_Multi_Shot_OnEnter(actor, action)
	if actor:IsMyActor() == true then
	
		if actor:IsMeetFloor() == false then
			action:SetParam(100,"finish");
			return	true;
		end
	
	end
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Multi_Shot_OnCastingCompleted(actor,action);
	
	actor:StopJump();
	actor:FreeMove(true);
	
	return true
end
function Skill_Multi_Shot_OnFindTarget(actor,action,kTargets)

	local	iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then
		iAttackRange = 250
	end

	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 10;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);

	local	arrow_worldpos = actor:GetPos();--kNewArrow:GetWorldPos();	--	화살의 위치
	
-- 회전축 구하기
	local	kRotateAxis = actor:GetLookingDir();
	kRotateAxis:Cross(Point3(0,-1,0));
	kRotateAxis:Unitize();
	local	fRotateAngle = 20.0*math.pi/180.0;

	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,20,0,1);
	kParam:SetParam_3(false,FTO_NORMAL);		

	local	kFireTarget = actor:GetLookingDir()-- Point3(0,1,0);

	kFireTarget:Rotate(kRotateAxis,fRotateAngle*(-1))
	kParam:SetParam_1(arrow_worldpos,kFireTarget);
	action:FindTargets(TAT_RAY_DIR,kParam,kTargets,kTargets);
	
	kFireTarget = actor:GetLookingDir()
	kFireTarget:Rotate(kRotateAxis,fRotateAngle*(0));
	kParam:SetParam_1(arrow_worldpos,kFireTarget);
	action:FindTargets(TAT_RAY_DIR,kParam,kTargets,kTargets);
	
	kFireTarget = actor:GetLookingDir()
	kFireTarget:Rotate(kRotateAxis,fRotateAngle*(1));
	kParam:SetParam_1(arrow_worldpos,kFireTarget);
	action:FindTargets(TAT_RAY_DIR,kParam,kTargets,kTargets);
	
	return	kTargets:size();
end

function Skill_Multi_Shot_FindTargets(actor,action)

	local	kTargets = CreateActionTargetList(actor:GetPilotGuid(),action:GetActionInstanceID(),action:GetActionNo());
	
	Skill_Multi_Shot_OnFindTarget(actor,action,kTargets);
	
	action:SetTargetList(kTargets);
	GetActionResultCS(action, actor)	
	local s = kTargets:size()
	ODS("멀티샷 : " ..s.."개\n")
	
	DeleteActionTargetList(kTargets);
	
end

function Skill_Multi_Shot_LoadToWeapon(actor,action)

	local	kProjectileID = actor:GetEquippedWeaponProjectileID();
	local	kAttackNodeName = "";
	
	if action:GetScriptParam("PROJECTILE_ID")~="" then
		kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	end
	
	kAttackNodeName = action:GetScriptParam("ATTACH_NODE");	
	
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

function Skill_Multi_Shot_OnUpdate(actor, accumTime, frameTime)
	local	actorID = actor:GetID()
	local	action = actor:GetAction()
	local	animDone = actor:IsAnimationDone()
	local	nextActionName = action:GetNextActionName()
	local	iMaxActionSlot = action:GetSlotCount();
	local	weapontype = actor:GetEquippedWeaponType();
	local	actionID = action:GetID();
	if action:GetParam(100) == "finish" then
		return	false;
	end

	if animDone == true then
	
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
function Skill_Multi_Shot_OnCleanUp(actor)

	actor:FreeMove(false);
	
	return true;
end
function Skill_Multi_Shot_OnLeave(actor, action)
	local	curAction = actor:GetAction();
	local	curParam = curAction:GetParam(1)
	local	actionID = action:GetID()

	if actor:IsMyActor() == false then
		return true;
	end
	if action:GetActionType()=="EFFECT" or curAction:GetParam(100)=="finish" then
		return true;
	end	
	if actionID == "a_jump" then 
		return false;
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

function Skill_Multi_Shot_OnEvent(actor, textKey)
	if textKey == "fire" or textKey=="hit" then
		local	action = actor:GetAction()
		local	iAttackRange = action:GetSkillRange(0,actor);

		actor:AttachSound(7283,"MultiShot");

		actor:FreeMove(false);	
		
		local	kProjectileMan = GetProjectileMan();
		
		local	iTargetCount = action:GetTargetCount();
		local	iTargetABVShapeIndex = 0;

		-- 회전축 구하기
		local	kRotateAxis = actor:GetLookingDir();
		kRotateAxis:Cross(Point3(0,-1,0));
		kRotateAxis:Unitize();
		local fRotateAngle = 20.0*math.pi/180.0
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
						
			local	kArrow = Skill_Multi_Shot_LoadToWeapon(actor,action);
			local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
		
			if kTargetActor ~= nil and kTargetActor:IsNil() == false and kActionTargetInfo~=nil and kActionTargetInfo:IsNil() == false then
				kArrow:SetTargetObject(kActionTargetInfo);	
				kActionTargetList:DeleteTargetInfo(0);
			end
			
			local	kFireTarget = actor:GetLookingDir()-- Point3(0,1,0);
			kFireTarget:Unitize()
			kFireTarget:Multiply(iAttackRange);
			kFireTarget:Rotate(kRotateAxis,fRotateAngle*(i-1));
			kFireTarget:Add(arrow_worldpos);			
			kArrow:SetTargetLoc(kFireTarget);	
			kArrow:Fire();--	발사!

			i=i+1;
		end
		
		actor:GetAction():SetParam(7,"FIRED");

	end
end

