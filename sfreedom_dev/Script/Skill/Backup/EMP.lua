function Skill_EMP_OnCheckCanEnter(actor,action)

	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 1 then
		return	false;
	end
	
	if actor:IsMeetFloor() == false then
		if actor:GetJumpAccumHeight()<50 then	
			return	false;
		end	
	end

	return		true;
	
end

function EMP_FindTarget(kActor, kAction)

	
	local	kTargets = CreateActionTargetList(kActor:GetPilotGuid(),kAction:GetActionInstanceID(),kAction:GetActionNo());
	local	iFound = Skill_EMP_OnFindTarget(kActor,kAction,kTargets);

	ODS("EMP 타겟 : " .. iFound .. "\n")
	
	return	kTargets;
end
function Skill_EMP_OnFindTarget(actor,action,kTargets)
	local range1 = action:GetSkillRange(0,actor)
	if range1 <= 50 then
		range1 = 150
	end	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 15;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);

	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(range1,30,0,1);	-- 일단 한놈 먼저 찾자
	kParam:SetParam_3(true,FTO_NORMAL);
	local i = action:FindTargets(0,kParam,kTargets,kTargets)
	if i > 0 then
		kParam:SetParam_1(kPos,actor:GetLookingDir());
		local range = action:GetAbil((AT_CUSTOMDATA1))
		if range <= 0 then
			range = 47.244095
		end
		local targetNum = action:GetAbil(AT_MAX_TARGETNUM)
		if targetNum <= 0 then
			targetNum = 10
		end
		kParam:SetParam_2(range,360,range,targetNum)	-- 그놈 범위로 찾자
		kParam:SetParam_3(false,FTO_NORMAL+FTO_DOWN);
		i = action:FindTargets(2,kParam,kTargets,kTargets)	--2 =>TAT_SPHERE
	end
	
	return	i
end

function Skill_EMP_OnCastingCompleted(actor, action)
	if action:IsNil() == false then
		 action:SetParamInt(5,1)
		action:SetSlot(action:GetCurrentSlot()+1);
		actor:PlayCurrentSlot();
   end
end

function Skill_EMP_OnEnter(actor, action)
	local	actorID = actor:GetID()
	local	actionID = action:GetID()
	local	weapontype = actor:GetEquippedWeaponType();
	actor:ToLeft(actor:IsToLeft(),true)
	action:SetParam(101,"");
	
	ODS("Skill_EMP_OnEnter weapontype : "..weapontype.."\n");

	if actor:IsMyActor() == true then
	
		local kTargets = EMP_FindTarget(actor,action);
		local iTargetCount = kTargets:size();
		DeleteActionTargetList(kTargets);

		if iTargetCount == 0 and actor:GetJumpAccumHeight()>50 then
		
			actor:SetComboCount(0);	
			action:SetNextActionName("a_jump_shot");
			action:ChangeToNextActionOnNextUpdate(true,true);
			return	true;	
		end
	
	end
		
	if Skill_EMP_Fire(actor,action) == false then
		actor:SetComboCount(0);	
		action:SetNextActionName("a_archer_melee");
		action:ChangeToNextActionOnNextUpdate(true,true);
		return	true;	
	end
	
	actor:ResetAnimation();
		
	return true
end
function Skill_EMP_Fire(actor,action)
	
	if actor:IsMyActor() == true then
		local kTargets = EMP_FindTarget(actor,action);
		action:SetTargetList(kTargets);
		DeleteActionTargetList(kTargets);	
	end
	
	local	iTargetCount = action:GetTargetCount();
	ODS("Skill_EMP_Fire iTargetCount : "..iTargetCount.."\n");

	local dis = 0
	local lookNum = 0

	for i = 0, iTargetCount - 1 do
		local	kTargetGUID = action:GetTargetGUID(i);
		local	targetobject = g_pilotMan:FindPilot(kTargetGUID);
		local	iTargetABVShapeIndex = action:GetTargetABVShapeIndex(i);
		if targetobject:IsNil() == false then
			local pt = targetobject:GetActor():GetABVShapeWorldPos(iTargetABVShapeIndex);
			pt:Subtract(actor:GetTranslate())
			if dis < pt:Length() then
				dis = pt:Length()
				lookNum = i
			end
		end
	end
	
	if iTargetCount>0 then
			--	타겟을 찾아 바라본다
		local	kTargetGUID = action:GetTargetGUID(lookNum);
		local	targetobject = g_pilotMan:FindPilot(kTargetGUID);
		local	iTargetABVShapeIndex = action:GetTargetABVShapeIndex(lookNum);
		if targetobject:IsNil() == false then
			local pt = targetobject:GetActor():GetABVShapeWorldPos(iTargetABVShapeIndex);
			actor:LookAt(pt);
		end
	end

	GetActionResultCS(action, actor)
	return	true;
end

function	Skill_EMP_LoadArrowToWeapon(actor,action)
	local	kProjectileMan = GetProjectileMan();
	local	kProjectileID = actor:GetEquippedWeaponProjectileID();
	local	kAttackNodeName = action:GetScriptParam("ATTACH_NODE");	
	if action:GetScriptParam("PROJECTILE_ID")~="" then
		kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	end
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());
	if kAttackNodeName == "" then
		kNewArrow:LoadToWeapon(actor);	--	장전
	else
		kNewArrow:LoadToHelper(actor,kAttackNodeName);
	end
	return kNewArrow;
end

function Skill_EMP_OnUpdate(actor, accumTime, frameTime)

	local	actorID = actor:GetID()
	local	action = actor:GetAction()
	local	animDone = actor:IsAnimationDone()
	local	nextActionName = action:GetNextActionName()
	local	iMaxActionSlot = action:GetSlotCount();
	local	weapontype = actor:GetEquippedWeaponType();
	local	actionID = action:GetID();

	if action:GetParam(101) =="end" then
		return	false;
	end

	if animDone == true then
	

		actor:SetNormalAttackEndTime();
	
		local	iCurrentComboCount = actor:GetComboCount();
		local	iNextComboCount = iCurrentComboCount + 1;
		if iNextComboCount>=GetMaxComboCount(actor) then
			actor:StartNormalAttackFreeze();
			iNextComboCount = 0;
		end
		actor:SetComboCount(iNextComboCount);
		ODS("iCurrentComboCount :"..iCurrentComboCount.." iNextComboCount:"..iNextComboCount.."\n");
		
		local	curActionSlot = action:GetCurrentSlot()
		local	curActionParam = action:GetParam(0)
		if curActionParam == "GoNext" then
			nextActionName = action:GetParam(1);

			action:SetParam(0, "null")
			action:SetParam(1, "end")
			
			if iNextComboCount == 2 and nextActionName == "a_shot_01" then
				nextActionName = "a_shot_02";
			end
			
			action:SetNextActionName(nextActionName)
			return false;
		else

			if iCurrentComboCount == GetMaxComboCount(actor)-1 then
				actor:StartNormalAttackFreeze();
			end
			
			action:SetParam(1, "end")

			return false
		end
	end

	return true
end
function Skill_EMP_OnCleanUp(actor,action)

	return true;
end
function Skill_EMP_OnLeave(actor, action)
	local curAction = actor:GetAction();
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
--	ODS("Skill_EMP_OnLeave NEXTACTION : "..actionID.."\n");

	if actor:IsMyActor() == false or curAction:GetParam(101)=="end" then
		return true;
	end
	if actionID == "a_jump" then 
		return false;
	end	
	if action:GetActionType()=="EFFECT"  then
		return true;
	end	
	if actionID == "a_shot_01" then
		action:SetParam(0,"Continue");
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
		
		if actor:GetComboCount() < GetMaxComboCount(actor)-1 then
		
			if curAction:GetParam(4)~="" then
				curAction:SetParam(0,"GoNext");
				curAction:SetParam(1,actionID);
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

function Skill_EMP_DoEffect(actor)

	actor:SetNormalAttackEndTime();

	local	action = actor:GetAction()
	local	iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange <= 50 then
		iAttackRange = 150
	end
	
	action:SetParam(4,"HIT");
	
	local	kProjectileMan = GetProjectileMan();
	local	kArrow = Skill_EMP_LoadArrowToWeapon(actor,action);
	
	local	targetobject = nil;

	if kArrow:IsNil() == false then
	
		local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
		local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
		
		local	iTargetCount = action:GetTargetCount();
		local	iTargetABVShapeIndex = 0;
		local	kActionTargetInfo = nil
	
		if iTargetCount>0 then
				--	타겟을 찾아 바라본다
			local	kTargetGUID = action:GetTargetGUID(0);
			iTargetABVShapeIndex = action:GetTargetABVShapeIndex(0);
			kActionTargetInfo = action:GetTargetInfo(0);
			targetobject = g_pilotMan:FindPilot(kTargetGUID);
			if targetobject:IsNil() == false then
				local pt = targetobject:GetActor():GetABVShapeWorldPos(iTargetABVShapeIndex);
				actor:LookAt(pt);
			end						
		end
		
		kArrow:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp());
		
		if targetobject == nil or targetobject:IsNil()==true or kActionTargetInfo==nil or kActionTargetInfo:IsNil() == true then
			kMovingDir:Multiply(iAttackRange);
			kMovingDir:Add(arrow_worldpos);
			kArrow:SetTargetLoc(kMovingDir);		
		else
			kArrow:SetTargetObjectList(action:GetTargetList());	
		end
		
		kArrow:Fire();
	end
	action:ClearTargetList();
end

function Skill_EMP_OnEvent(actor, textKey)
	ODS("Skill_EMP_OnEvent key : "..textKey.."\n");
	if textKey == "fire" or textKey=="hit" then
		
		Skill_EMP_DoEffect(actor);

	end
end
