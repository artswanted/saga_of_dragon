-- [PROJECTILE_ID] : 발사할 발사체 ID, "" 일 경우 무기에 붙어있는 발사체 ID 를 사용한다.
-- [ATTACH_NODE_ID] : 발사체를 붙일 액터의 노드 ID, "" 일 경우 캐릭터 좌표에서 발사된다.

-- Melee
function SwordDance_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function SwordDance_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	SwordDance_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end

function	SwordDance_LoadToWeapon(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kProjectileMan = GetProjectileMan();
	local	kProjectileID = action:GetScriptParam("PROJECTILE_ID");
	
	if kProjectileID == "" then
		kProjectileID = actor:GetEquippedWeaponProjectileID();
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());
	
	local	kTargetNodeID = action:GetScriptParam("ATTACH_NODE_ID");

	local	iSkillLevel = action:GetAbil(AT_LEVEL);
		if 0 == iSkillLevel then
		iSkillLevel = 1;
	end

	kNewArrow:SetParamValue("SkillLevel", ""..iSkillLevel);
	
	kNewArrow:LoadToHelper(actor,kTargetNodeID);	--	장전
	return kNewArrow;

end

function SwordDance_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	
	
	action:CreateActionTargetList(actor);
	
end
function SwordDance_OnFindTarget(actor,action,kTargets)
		
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--	먼저 가장 앞에 있는 한놈을 찾는다.
	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 10;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);
	
	action:SetParam(0,"");
	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(action:GetSkillRange(0,actor),50,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	local iFoundCount = action:FindTargets(TAT_BAR,kParam,kTargets,kTargets);		
	
	if iFoundCount == 0 then
		return	iFoundCount;
	end

	return iFoundCount;
	
	--	찾은 녀석을 중심으로 나머지 타겟을 찾는다.
--	local	kTargetInfo = kTargets:GetTargetInfo(0);
--	local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID());			
	
--	kParam:SetParam_1(kTargetPilot:GetActor():GetPos(),actor:GetLookingDir());
--	kParam:SetParam_2(0,0,50,0);
--	kParam:SetParam_3(true,FTO_NORMAL);
	
--	return	action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);	
	
end

function SwordDance_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	SwordDance_OnCastingCompleted(actor,action);
	
	return true
end

function SwordDance_OnUpdate(actor, accumTime, frameTime)

--	local actorID = actor:GetID()
	local action = actor:GetAction()
	local animDone = actor:IsAnimationDone()
--	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
--	local iComboCount = actor:GetComboCount();
	
	--if action:GetParam(1) == "end" then
	--	return	false;
	--end

	if animDone == true then
		return false;
	end

	return true
end

function SwordDance_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	--if actor:GetAction():GetParamInt(4) == 1 then
		actor:HideParts(6, false)	--	칼 숨김 해제
	--	actor:GetAction():SetParamInt(4,0);
	--end	

	return true;
end

function SwordDance_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
		
	return true;
--[[	if actor:IsMyActor() == false then
		return true;
	end
	
	if actionID == "a_jump" then 
		return false;
	end
	
	if action:GetActionType()=="EFFECT" then
		
		return true;
	end
	
	if curParam == "end" then 
		return true;
	end
	
	if action:GetEnable() == true then
	

		curAction:SetParam(0,"GoNext");
		curAction:SetParam(1,actionID);

		return false;
	
	elseif curParam == "end" and (actionID == "a_run_right" or actionID == "a_run_left") then
		return true;
	elseif 	actionID == "a_idle" or
		actionID == "a_handclaps" then
		return true;
	end

	return false ]]--
end

function SwordDance_SwordReturn(actor,iActionInstanceID)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	
	
	if action:GetActionInstanceID() == iActionInstanceID then
	
		if action:GetParamInt(4) == 1 then
			actor:HideParts(6, false)	--	칼 숨김 해제
			action:SetParamInt(4,0);
		end
	
		action:SetParam(1,"end");
	
	end

end

function SwordDance_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	ODS("SwordDance_OnEvent textKey:"..textKey.."\n");
	
	if (textKey == "hit" or textKey == "fire") and kAction:GetParamInt(5) == 0 then
	
	    kAction:SetParamInt(5,1);   --  중복 실행 방지
	    
		actor:AttachSound(3852,"ShieldBoom");

		actor:HideParts(6, true)	--	칼 숨기자
		kAction:SetParamInt(4,1);	--	칼 숨겼음
		
		local	iAttackRange = kAction:GetSkillRange(0,actor);
		
--		local	kProjectileMan = GetProjectileMan();
		local	kArrow = SwordDance_LoadToWeapon(actor,kAction);
		
		local	targetobject = nil;

		if kArrow:IsNil() == false then
		
			local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
			local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
			
			local	iTargetCount = kAction:GetTargetCount();
			local	iTargetABVShapeIndex = 0;
			local	kActionTargetInfo = nil
			local	kTargetABVPos = nil;
			if iTargetCount>0 then
					--	타겟을 찾아 바라본다
				local	kTargetGUID = kAction:GetTargetGUID(0);
				iTargetABVShapeIndex = kAction:GetTargetABVShapeIndex(0);
				kActionTargetInfo = kAction:GetTargetInfo(0);
				targetobject = g_pilotMan:FindPilot(kTargetGUID);
				if targetobject:IsNil() == false then
					kTargetABVPos = targetobject:GetActor():GetABVShapeWorldPos(iTargetABVShapeIndex);
					actor:LookAt(kTargetABVPos);
				end						
			end
			
			kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp());
			
			if targetobject == nil or targetobject:IsNil()==true or kActionTargetInfo==nil or kActionTargetInfo:IsNil() == true then
				
				kMovingDir:Multiply(iAttackRange);
				kMovingDir:Add(arrow_worldpos);
				kArrow:SetTargetLoc(kMovingDir);	
				
			else
				kArrow:SetTargetObjectList(kAction:GetTargetList());	
				
				if targetobject ~= nil and targetobject:IsNil() == false then
				
					local	kArrowTargetPos = Point3(kTargetABVPos:GetX(),kTargetABVPos:GetY(),arrow_worldpos:GetZ());
					kArrow:SetTargetLoc(kArrowTargetPos);			
				
				end
				
			end
			
			kArrow:Fire();
			
		else
			
			ODS("Arrow is nil\n");
		
		end
		
		kAction:ClearTargetList();
		actor:ClearIgnoreEffectList();
					
	end

	return	true;
end
