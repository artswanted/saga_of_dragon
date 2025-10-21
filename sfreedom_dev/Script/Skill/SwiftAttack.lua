function Skill_Swiftattack_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 1 then
		return	false;
	end
	
	return		true;
	
end
function Swiftattack_FindTarget(kActor, kAction)

	CheckNil(nil==kActor);	
	CheckNil(kActor:IsNil());
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());	
	
	local	kTargets = CreateActionTargetList(kActor:GetPilotGuid(),kAction:GetActionInstanceID(),kAction:GetActionNo());
	local	iFound = Skill_Swiftattack_OnFindTarget(kActor,kAction,kTargets);

	ODS("쿠션샷 타겟 : " .. iFound .. "\n")
	
	return	kTargets;
end
function Skill_Swiftattack_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local range1 = action:GetSkillRange(0,actor)

	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();

	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(0,0,range1,0);	-- 일단 한놈 먼저 찾자
	kParam:SetParam_3(true,FTO_NORMAL);

	return	action:FindTargetsEx(action:GetActionNo(),TAT_SPHERE,kParam,kTargets,kTargets);
end

function Skill_Swiftattack_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_Swiftattack_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
	actor:AttachSound(106,"SwiftAttack");
	actor:HideNode("Scene Root",true);
	action:SetParamInt(10, 0);
	action:SetParamInt(13, 0);
end

function Skill_Swiftattack_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(101,"");	
		
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Swiftattack_OnCastingCompleted(actor,action);	
		
	return true
end
function Skill_Swiftattack_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if actor:IsMyActor() == true then
		local kTargets = Swiftattack_FindTarget(actor,action);
		action:SetTargetList(kTargets);
		DeleteActionTargetList(kTargets);	
	end

	actor:AttachParticleToPoint(8776,actor:GetNodeWorldPos("char_root"),"ef_Shadowleap_01_01_char_root");
	
	local	iTargetCount = action:GetTargetCount();
	ODS("Skill_Swiftattack_Fire iTargetCount : "..iTargetCount.."\n");

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

function	Skill_Swiftattack_LoadArrowToWeapon(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
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

function Skill_Swiftattack_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local	action = actor:GetAction()
		
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	animDone = actor:IsAnimationDone()
	if action:GetParamInt(10) == 2 then
		action:SetParam(1, "end");
    	    
		local   kMyPos = actor:GetPos();
		local   kTargetPos = action:GetParamAsPoint(0);
		local   kDir = kTargetPos:_Subtract(kMyPos);
		kDir:Unitize();
        
		if actor:IsToLeft() == action:GetParamInt(11) then
			kDir:Multiply(-30);							
		elseif actor:IsToLeft() ~= action:GetParamInt(11) then
			kDir:Multiply(30);
		end

		local   kNewPos = kTargetPos:_Add(kDir);
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		kNewPos = g_world:FindActorFloorPos(kNewPos);
        
		--actor:AttachParticle(8777,"char_root","ef_Shadowleap_01_02_char_root");

		return false;
	end

	if action:GetParam(101) =="end" then
		return	false;
	end

	if animDone == true then
		action:SetParam(1, "end")
	end

	return true
end
function Skill_Swiftattack_OnCleanUp(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:HideNode("Scene Root",false);
	return true;
end
function Skill_Swiftattack_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
		
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
--	ODS("Skill_Swiftattack_OnLeave NEXTACTION : "..actionID.."\n");

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

function Skill_Swiftattack_DoEffect(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:SetNormalAttackEndTime();

	local	action = actor:GetAction()
		
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iAttackRange = action:GetSkillRange(0,actor);

	action:SetParam(4,"HIT");

	local	iTargetCount = action:GetTargetCount();
	if iTargetCount > 0 then
--		local	kProjectileMan = GetProjectileMan();
		local	kArrow = Skill_Swiftattack_LoadArrowToWeapon(actor,action);
		
		local	targetobject = nil;

		if kArrow:IsNil() == false then
		
			kArrow:SetParamValue("DMG_SOUND","CushionShot");
		
			local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
			local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
			
			local	iTargetABVShapeIndex = 0;
			local	kActionTargetInfo = nil
		
			--	타겟을 찾아 바라본다
			local	kTargetGUID = action:GetTargetGUID(0);
			iTargetABVShapeIndex = action:GetTargetABVShapeIndex(0);
			kActionTargetInfo = action:GetTargetInfo(0);
			targetobject = g_pilotMan:FindPilot(kTargetGUID);
			if targetobject:IsNil() == false then
				local pt = targetobject:GetActor():GetABVShapeWorldPos(iTargetABVShapeIndex);
				actor:LookAt(pt);
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
	else
		action:SetParamInt(10,2);
		action:SetParam(1, "end");
	end

	action:ClearTargetList();
end

function Skill_Swiftattack_OnEvent(actor, textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	ODS("Skill_Swiftattack_OnEvent key : "..textKey.."\n");
	if textKey == "fire" or textKey=="hit" then		
		Skill_Swiftattack_DoEffect(actor);
	end
end
