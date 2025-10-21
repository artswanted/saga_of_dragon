function Skill_Thrust_OnCheckCanEnter(actor,action)

	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 1 then
		return	false;
	end
	
	return		true;
	
end
function Thrust_FindTarget(kActor,kAction)

	
	local	kTargets = CreateActionTargetList(kActor:GetPilotGuid(),kAction:GetActionInstanceID(),kAction:GetActionNo());
	local	iFound = Skill_Thrust_OnFindTarget(kActor,kAction,kTargets);

	ODS("관통샷 타겟 : " .. iFound .. "\n")
	
	return	kTargets;
end
function Skill_Thrust_OnFindTarget(actor,action,kTargets)
	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 15;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);
   
   local range = action:GetSkillRange(0,actor)
   ODS("관통샷 거리 : " .. range .. "\n")
   if range <= 50 then
		range =150
	end
	kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(range,30,0,10);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return	action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);
end

function Skill_Thrust_OnCastingCompleted(actor, action)
	Skill_Thrust_Fire(actor,action);

	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
end

function Skill_Thrust_OnEnter(actor, action)
	local	actorID = actor:GetID()
	local	actionID = action:GetID()
	local	weapontype = actor:GetEquippedWeaponType();
	action:SetParam(101,"");
	
	ODS("Skill_Thrust_OnEnter weapontype : "..weapontype.."\n");
	
	actor:ResetAnimation();
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Thrust_OnCastingCompleted(actor,action);
	
	return true
end
function Skill_Thrust_Fire(actor,action)
	
	action:SetParamInt(5,1)
		
	if actor:IsMyActor() == true then
		local kTargets = Thrust_FindTarget(actor,action);
		action:SetTargetList(kTargets);
		DeleteActionTargetList(kTargets);	
	end
	
	local	iTargetCount = action:GetTargetCount();
	ODS("Skill_Thrust_Fire iTargetCount : "..iTargetCount.."\n");

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
			local tlist = action:GetTargetList()
--			tlist:SwapPosition(0, lookNum)	--보낼 때는 정상으로 보내자
		end
	end

	GetActionResultCS(action, actor)
	return	true;
end

function	Skill_Thrust_LoadArrowToWeapon(actor,action)
	local	kProjectileMan = GetProjectileMan();
	local	kProjectileID = actor:GetEquippedWeaponProjectileID();
	if action:GetScriptParam("PROJECTILE_ID")~="" then
		kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	end
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());
	kNewArrow:LoadToWeapon(actor);	--	장전
	return kNewArrow;
end

function Skill_Thrust_OnUpdate(actor, accumTime, frameTime)

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
		action:SetParam(1, "end")
		return false
	end

	return true
end
function Skill_Thrust_OnCleanUp(actor,action)

	return true;
end
function Skill_Thrust_OnLeave(actor, action)
	local curAction = actor:GetAction();
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
--	ODS("Skill_Thrust_OnLeave NEXTACTION : "..actionID.."\n");

	if actor:IsMyActor() == false or curAction:GetParam(101)=="end" then
		return true;
	end
	if actionID == "a_jump" then 
		return false;
	end	
	if action:GetActionType()=="EFFECT" then
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

function Skill_Thrust_DoEffect(actor)

	local	action = actor:GetAction()
	local	iAttackRange = action:GetSkillRange(0,actor);
	
	actor:AttachSound(7283,"Thrust");	
	
	action:SetParam(4,"HIT");
	
	local	kProjectileMan = GetProjectileMan();
	local	kArrow = Skill_Thrust_LoadArrowToWeapon(actor,action);
	
	local	targetobject = nil;

	if kArrow:IsNil() == false then
	
		local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
		local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
		
		local	iTargetCount = action:GetTargetCount();
		local	iTargetABVShapeIndex = 0;
		local	kActionTargetInfo = nil
		local dis = 0
		local lookNum = 0

		for i = 0, iTargetCount -1 do
			local	kTargetGUID = action:GetTargetGUID(i);
			local	targetobject = g_pilotMan:FindPilot(kTargetGUID);
			local	iTargetABVShapeIndex = action:GetTargetABVShapeIndex(i);
			if targetobject:IsNil() == false then
				local pt = targetobject:GetActor():GetABVShapeWorldPos(iTargetABVShapeIndex);
				pt:Subtract(actor:GetTranslate())
				ODS("거리 : " .. pt:Length() .. " i : " .. i .. "\n")
				if dis < pt:Length() then
					dis = pt:Length()
					lookNum = i
				end
			end

		end
	
		if iTargetCount>0 then
				--	타겟을 찾아 바라본다
			ODS("바라본다 : " .. lookNum .. "\n")
			local	kTargetGUID = action:GetTargetGUID(lookNum);
			iTargetABVShapeIndex = action:GetTargetABVShapeIndex(lookNum);
			kActionTargetInfo = action:GetTargetInfo(lookNum);
			targetobject = g_pilotMan:FindPilot(kTargetGUID);
			if targetobject:IsNil() == false then
				local pt = targetobject:GetActor():GetABVShapeWorldPos(iTargetABVShapeIndex);
				actor:LookAt(pt);
				local tlist = action:GetTargetList()
				--tlist:SwapPosition(0, lookNum)
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

function Skill_Thrust_OnEvent(actor, textKey)
	ODS("Skill_Thrust_OnEvent key : "..textKey.."\n");
	if textKey == "fire" or textKey=="hit" then
		
		Skill_Thrust_DoEffect(actor);

	end
end
