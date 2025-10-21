function Skill_Cushion_OnCheckCanEnter(actor,action)

	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 1 then
		return	false;
	end
	
	return		true;
	
end
function Cushion_FindTarget(kActor, kAction)

	
	local	kTargets = CreateActionTargetList(kActor:GetPilotGuid(),kAction:GetActionInstanceID(),kAction:GetActionNo());
	local	iFound = Skill_Cushion_OnFindTarget(kActor,kAction,kTargets);

	ODS("쿠션샷 타겟 : " .. iFound .. "\n")
	
	return	kTargets;
end
function Skill_Cushion_OnFindTarget(actor,action,kTargets)
	local range1 = action:GetSkillRange(0,actor)


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
      local info = kTargets:GetTargetInfo(0)
      local	kTargetGUID = info:GetTargetGUID();
      local targetobject = g_pilotMan:FindPilot(kTargetGUID);
      local iTargetABVShapeIndex = action:GetTargetABVShapeIndex(0);
      if targetobject:IsNil() == false then
			local pt = targetobject:GetActor():GetABVShapeWorldPos(iTargetABVShapeIndex)

         kParam:SetParam_1(pt,targetobject:GetActor():GetLookingDir())
         local targetNum = action:GetAbil(AT_MAX_TARGETNUM)
         if targetNum < 3 then
            targetNum = 3
         end

         local range = action:GetAbil(AT_CUSTOMDATA1)
         if range <= 47.244095 then
            range = 47.244095
         end
         kParam:SetParam_2(0,360,range,targetNum);	-- 그놈 범위로 찾자
         kParam:SetParam_3(true,FTO_NORMAL);
         kTargets:clear()
         i = action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets)	--2 =>TAT_SPHERE
      end
	end

   ODS("스킬에서 걸린 GUID \n")
   for k=0,i-1 do
      ODS(k)
      kTargets:GetTargetInfo(k):GetTargetGUID():ODS()
   end
   ODS("-----------------\n")
	
	return	i
end

function Skill_Cushion_OnCastingCompleted(actor, action)
	Skill_Cushion_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
end

function Skill_Cushion_OnEnter(actor, action)
	local	actorID = actor:GetID()
	local	actionID = action:GetID()
	local	weapontype = actor:GetEquippedWeaponType();

	action:SetParam(101,"");
		
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Cushion_OnCastingCompleted(actor,action);
		
	return true
end
function Skill_Cushion_Fire(actor,action)
	
	if actor:IsMyActor() == true then
		local kTargets = Cushion_FindTarget(actor,action);
		action:SetTargetList(kTargets);
		DeleteActionTargetList(kTargets);	
	end
	
	local	iTargetCount = action:GetTargetCount();
	ODS("Skill_Cushion_Fire iTargetCount : "..iTargetCount.."\n");

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

function	Skill_Cushion_LoadArrowToWeapon(actor,action)
	local	kProjectileMan = GetProjectileMan();
	local	kProjectileID = actor:GetEquippedWeaponProjectileID();
	if action:GetScriptParam("PROJECTILE_ID")~="" then
		kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	end
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());
	kNewArrow:LoadToWeapon(actor);	--	장전
	kNewArrow:SetParamValue("Target", "0")
	return kNewArrow;
end

function Skill_Cushion_OnUpdate(actor, accumTime, frameTime)

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
	end

	return true
end
function Skill_Cushion_OnCleanUp(actor,action)
	return true;
end
function Skill_Cushion_OnLeave(actor, action)
	local curAction = actor:GetAction();
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
--	ODS("Skill_Cushion_OnLeave NEXTACTION : "..actionID.."\n");

	if actor:IsMyActor() == false or curAction:GetParam(101)=="end" then
		return true;
	end
	if actionID == "a_jump" then 
		return false;
	end	
	if action:GetActionType()=="EFFECT"then
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

function Skill_Cushion_DoEffect(actor)

	actor:SetNormalAttackEndTime();

	local	action = actor:GetAction()
	local	iAttackRange = action:GetSkillRange(0,actor);

	action:SetParam(4,"HIT");
	
	local	kProjectileMan = GetProjectileMan();
	local	kArrow = Skill_Cushion_LoadArrowToWeapon(actor,action);
	
	local	targetobject = nil;

	if kArrow:IsNil() == false then
	
		kArrow:SetParamValue("DMG_SOUND","CushionShot");
	
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

function Skill_Cushion_OnEvent(actor, textKey)
	ODS("Skill_Cushion_OnEvent key : "..textKey.."\n");
	if textKey == "fire" or textKey=="hit" then
		
		Skill_Cushion_DoEffect(actor);

	end
end
