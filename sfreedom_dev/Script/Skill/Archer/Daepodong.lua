function Skill_Daepodong_OnCheckCanEnter(actor,action)

	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	
	return		true;
	
end
function Daepodong_FindTarget(kActor, kAction)

	
	local	kTargets = CreateActionTargetList(kActor:GetPilotGuid(),kAction:GetActionInstanceID(),kAction:GetActionNo());
	local	iFound = Skill_Daepodong_OnFindTarget(kActor,kAction,kTargets);

	ODS("대포동 1 타겟 : " .. iFound .. "\n")
	
	return	kTargets;
end
function Skill_Daepodong_OnFindTarget(actor,action,kTargets)
	local iAttackRange = action:GetSkillRange(0,actor)

	local	kPos = actor:GetPos();
	local	kMovingDir = actor:GetLookingDir();
	kMovingDir:Multiply(iAttackRange);
	kPos:Add(kMovingDir);
	kPos:SetZ(kPos:GetZ());		

	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 71
	end
	
	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(0,30,iParam2,0);	-- 일단 한놈 먼저 찾자
	kParam:SetParam_3(true,FTO_NORMAL);

	local	iSkillLevel = action:GetAbil(AT_LEVEL);
	if 5 == iSkillLevel then
		kParam:SetParam_3(true,FTO_NORMAL + FTO_BLOWUP);
	end
	
	return	action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets)
end

function Skill_Daepodong_OnCastingCompleted(actor, action)
	Skill_Daepodong_Fire(actor,action)

	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
end

function Skill_Daepodong_OnEnter(actor, action)
	local	actorID = actor:GetID()
	local	actionID = action:GetID()
	local	weapontype = actor:GetEquippedWeaponType();

	action:SetParam(101,"");
	action:SetParam(4,"");

	actor:HideParts(EQUIP_POS_WEAPON, true);
	
	ODS("Skill_Daepodong_OnEnter weapontype : "..weapontype.."\n");

	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Daepodong_OnCastingCompleted(actor,action);
	
	return true
end
function Skill_Daepodong_Fire(actor,action)
	
	action:SetParamInt(5,1)	
	if actor:IsMyActor() == true then
		local kTargets = Daepodong_FindTarget(actor,action);
		action:SetTargetList(kTargets);
		DeleteActionTargetList(kTargets);	
	end
	
	local	iTargetCount = action:GetTargetCount();
	ODS("Skill_Daepodong_Fire iTargetCount : "..iTargetCount.."\n");

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

	actor:AttachSound(2785,"Deapodong_Start");

	GetActionResultCS(action, actor)
	return	true;
end

function	Skill_Daepodong_LoadArrowToWeapon(actor,action)
	local	kProjectileMan = GetProjectileMan();
	local	kProjectileID = actor:GetEquippedWeaponProjectileID();
	local	kAttackNodeName = action:GetScriptParam("ATTACH_NODE");	
	if action:GetScriptParam("PROJECTILE_ID")~="" then
		kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	end
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());

	local	iAttackRange = action:GetSkillRange(0,actor);
	local	kPos = actor:GetPos();
	local	kMovingDir = actor:GetLookingDir();
	local	iRandom = math.random(-200,200);
	if iRandom == 0 then
		iRandom = 1;
	end

	kMovingDir:Multiply(iAttackRange+iRandom);
	kPos:Add(kMovingDir);
	kPos:SetZ(kPos:GetZ()+500);

	local	iSkillLevel = action:GetAbil(AT_LEVEL);
	if 0 == iSkillLevel then
		iSkillLevel = 1;
	end
	kNewArrow:SetScale(0.2 + (iSkillLevel * 0.16)); --스킬 레벨에 따라 크기가 변한다. 최대치 1
	kNewArrow:SetWorldPos(kPos);
	kNewArrow:SetParamValue("SkillLevel", ""..iSkillLevel);
	
	return kNewArrow;
end

function Skill_Daepodong_OnUpdate(actor, accumTime, frameTime)

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
		local curActionParam = action:GetParam(0)
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
function Skill_Daepodong_OnCleanUp(actor,action)
	actor:HideParts(EQUIP_POS_WEAPON, false);
	return true;
end
function Skill_Daepodong_OnLeave(actor, action)
	local curAction = actor:GetAction();
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
--	ODS("Skill_Daepodong_OnLeave NEXTACTION : "..actionID.."\n");

	if actor:IsMyActor() == false or curAction:GetParam(101)=="end" then
		return true;
	end
	if actionID == "a_jump" then 
		return false;
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
	if curParam == "end" then 
		return true;
	end		

	if action:GetEnable() == true then

		curAction:SetParam(0,"GoNext");
		curAction:SetParam(1,actionID);

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

function Skill_Daepodong_DoEffect(actor)
	
	if "HIT" ==	 actor:GetAction():GetParam(4) then
		ODS("두번쏜다\n");
		return;
	end

	actor:SetNormalAttackEndTime();

	local	action = actor:GetAction()
	local	iAttackRange = action:GetSkillRange(0,actor);
	
	action:SetParam(4,"HIT");
	
	local	kProjectileMan = GetProjectileMan();
	local	kArrow = Skill_Daepodong_LoadArrowToWeapon(actor,action);
	
	local	targetobject = nil;

	if kArrow:IsNil() == false then
	
		local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
		local	kPos = actor:GetPos();
		local	kMovingDir = actor:GetLookingDir();
		kMovingDir:Multiply(iAttackRange);
		kPos:Add(kMovingDir);
		kPos:SetZ(kPos:GetZ()-20);		
		
		local	iTargetCount = action:GetTargetCount();
		
		actor:LookAt(kPos);
	
		kArrow:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp());
		
		if iTargetCount>0 then
			kArrow:SetTargetObjectList(action:GetTargetList());	
		end

		kArrow:SetTargetLoc(kPos);		
		kArrow:Fire();

		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local ptcl = g_world:ThrowRay(kPos, Point3(0,0,-1), 100)
		ptcl:SetZ(ptcl:GetZ()+5)

			 local	iSkillLevel = action:GetAbil(AT_LEVEL);
		if 0 == iSkillLevel then
			iSkillLevel = 1;
		end
		actor:AttachParticleToPointS(13,ptcl,"ef_Roketlauncher_01", iSkillLevel);
		
	end
	action:ClearTargetList();
end

function Skill_Daepodong_OnEvent(actor, textKey)
	ODS("Skill_Daepodong_OnEvent key : "..textKey.."\n");
	if textKey == "fire" or textKey=="hit" then
		
		Skill_Daepodong_DoEffect(actor);

	end
end
