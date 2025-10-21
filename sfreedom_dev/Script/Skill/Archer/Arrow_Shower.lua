function Skill_Arrow_Shower_OnCastingCompleted(actor,action)
	
	Skill_Arrow_Shower_Fire(actor,action)

 	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

   ODS("Skill_Arrow_Shower_OnCastingCompleted \n")
end

function Skill_Arrow_Shower_OnCheckCanEnter(actor,action)
   ODS("Skill_Arrow_Shower_OnCheckCanEnter \n")
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

   ODS("Skill_Arrow_Shower_OnCheckCanEnter OK! \n")
	return		true;
	
end
function Arrow_Shower_FindTarget(kActor,kAction)

	local	kTargets = CreateActionTargetList(kActor:GetPilotGuid(),kAction:GetActionInstanceID(),kAction:GetActionNo());
	local	iFound = Skill_Arrow_Shower_OnFindTarget(kActor,kAction,kTargets);

   ODS("애로우 샤워 타겟 : " .. iFound .. "\n")
	
	return	kTargets;
end
function Skill_Arrow_Shower_OnFindTarget(actor,action,kTargets)
	local range = action:GetSkillRange(0,actor)

	local	kPos = actor:GetPos();
	local 	kOrigin = kPos
	local	kDir = actor:GetLookingDir();
	kDir:Multiply(range) -- cm을 인치로 변환
	kPos:Add(kDir)
	local range2 = action:GetAbil((AT_CUSTOMDATA1))

   	kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(range,360,range2,0);
	kParam:SetParam_3(true,FTO_NORMAL+FTO_DOWN);
	
	return	action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);
end

function Skill_Arrow_Shower_OnEnter(actor, action)
	local	actorID = actor:GetID()
	local	actionID = action:GetID()
	local	weapontype = actor:GetEquippedWeaponType();

	action:SetParamInt(6,0);
	
	ODS("Skill_Arrow_Shower_OnEnter weapontype : "..weapontype.."\n");
	
	-- 파티클 붙이자
	local range = action:GetSkillRange(0,actor)
	ODS("애로우 샤워 거리 : " .. range .. "\n")

	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	kDir:Multiply(range)
	kPos:Add(kDir)
	local ptpoint = Point3(kPos:GetX(), kPos:GetY(), kPos:GetZ() + 100)
  ODS("OnEnter ptpoint X : " ..ptpoint:GetX().. " Y : " ..ptpoint:GetY().. " Z : " ..ptpoint:GetZ().. "\n")
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local ptcl = g_world:ThrowRay(ptpoint, Point3(0,0,-1), 200)
  if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
     ODS("바닥체크 실패\n")
  end
	ptcl:SetZ(ptcl:GetZ()+5)
	actor:AttachParticleToPoint(1508, ptcl, "ef_arrow_shower_ring")
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end
	
	Skill_Arrow_Shower_OnCastingCompleted(actor,action);
		
	return true
end
function Skill_Arrow_Shower_Fire(actor,action)
	
	 action:SetParamInt(5,1)
	 	
	if actor:IsMyActor() == true then
		local kTargets = Arrow_Shower_FindTarget(actor,action);
		action:SetTargetList(kTargets);
		DeleteActionTargetList(kTargets);
	end

	actor:AttachSound(2785,"ArrowShower_Start");

	GetActionResultCS(action, actor)
	return	true;
end

function	Skill_Arrow_Shower_LoadArrowToWeapon(actor,action)
	local	kProjectileID = actor:GetEquippedWeaponProjectileID();
	local	kAttackNodeName = action:GetScriptParam("ATTACH_NODE");	
	
	if action:GetScriptParam("PROJECTILE_ID")~="" then
		kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	end

	local	kProjectileMan = GetProjectileMan();
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());
	if kAttackNodeName == "" then
		kNewArrow:LoadToWeapon(actor);	--	장전
	else
		kNewArrow:LoadToHelper(actor,kAttackNodeName);
	end
	return kNewArrow;
end

function Skill_Arrow_Shower_OnUpdate(actor, accumTime, frameTime)

	local	actorID = actor:GetID()
	local	action = actor:GetAction()
	local	animDone = actor:IsAnimationDone()
	local	nextActionName = action:GetNextActionName()
	local	iMaxActionSlot = action:GetSlotCount();
	local	weapontype = actor:GetEquippedWeaponType();
	local	actionID = action:GetID();

	if animDone == true then
				
		action:SetParam(1, "end")
		return false
	end

	return true
end
function Skill_Arrow_Shower_OnCleanUp(actor,action)
	
	return true;
end
function Skill_Arrow_Shower_OnLeave(actor, action)
	local curAction = actor:GetAction();
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()

	ODS("Skill_Arrow_Shower_OnLeave NEXTACTION : "..actionID.."\n");
	if actor:IsMyActor() == false then
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

function Skill_Arrow_Shower_DoEffect(actor)

	ODS("Skill_Arrow_Shower_DoEffect \n");

	local	action = actor:GetAction()
	local	iAttackRange = action:GetSkillRange(0,actor);

	action:SetParam(4,"HIT");
	
	local	kProjectileMan = GetProjectileMan();
	local	kArrow = Skill_Arrow_Shower_LoadArrowToWeapon(actor,action);
	
	local	targetobject = nil;
	
	if kArrow:IsNil() then
		ODS("kArrow:IsNil() iProjectileUID : "..iProjectileUID.." action : "..action:GetID().."\n");
	end


	if kArrow:IsNil() == false then
		kArrow:SetMovingType(1)
		kArrow:SetParam_SinCurveLineType(300, 0)
	
		local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
      ODS("actor:GetLookingDir() : " ..kMovingDir:GetX().. " Y : " ..kMovingDir:GetY().. " Z : " ..kMovingDir:GetZ().. "\n")
		local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
      ODS("kArrow:GetWorldPos() : " ..arrow_worldpos:GetX().. " Y : " ..arrow_worldpos:GetY().. " Z : " ..arrow_worldpos:GetZ().. "\n")
		
		local	iTargetCount = action:GetTargetCount();
 	
      if iTargetCount > 0 then
         kArrow:SetTargetObjectList(action:GetTargetList());	
      end
     
		kArrow:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp());
      ODS("iAttackRange : " .. iAttackRange .. "\n")
		kMovingDir:Multiply(iAttackRange);
		kMovingDir:Add(actor:GetPos());

		local ptpoint = Point3(kMovingDir:GetX(), kMovingDir:GetY(), kMovingDir:GetZ() + 100)
      ODS("ptpoint X : " ..ptpoint:GetX().. " Y : " ..ptpoint:GetY().. " Z : " ..ptpoint:GetZ().. "\n")
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local ptcl = g_world:ThrowRay(ptpoint, Point3(0,0,-1), 200)
		ptcl:SetZ(ptcl:GetZ()+5)

		kArrow:SetTargetLoc(ptcl);	
      ODS("애로우샤워 화살 목표 X : " ..ptcl:GetX().. " Y : " ..ptcl:GetY().. " Z : " ..ptcl:GetZ().. "\n")
		
		kArrow:Fire();
		
	end
	
	action:ClearTargetList();
end

function Skill_Arrow_Shower_OnEvent(actor, textKey)
	if textKey == "fire" or textKey=="hit" then
		local kAction = actor:GetAction();
		if kAction:GetParamInt(6) ~= 0 then
			return true;
		end
		kAction:SetParamInt(6,1);

		Skill_Arrow_Shower_DoEffect(actor);

	end
end
