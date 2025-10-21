-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [FIRE_SOUND_ID] : 스킬이 FIRE 될때 플레이 할 사운드 ID
-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

-- Melee
function ShoulderTackle_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function ShoulderTackle_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ShoulderTackle_Fire(actor,action);
	action:SetSlot(0);
	actor:PlayCurrentSlot();

end
function ShoulderTackle_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	action:CreateActionTargetList(actor);
	
	actor:StartBodyTrail("../Data/5_Effect/9_Tex/swift_01.tga",500,0,0);
	action:SetParamFloat(2,actor:GetPos():GetX());	--	시작 위치 기록
	action:SetParamFloat(3,actor:GetPos():GetY());	--	시작 위치 기록
	action:SetParamFloat(4,actor:GetPos():GetZ());	--	시작 위치 기록
	action:SetParamFloat(5,g_world:GetAccumTime());	--	시작 시간 기록
	action:SetParamInt(6,0)	--	Step	
		
	actor:FreeMove(true);
	actor:StopJump();
	actor:SetMovingDelta(Point3(0,0,0));

	--local	iBaseClassID = actor:GetPilot():GetBaseClassID()
	--if iBaseClassID == CT_MAGICIAN then
		--actor:AttachParticle(494,"char_root","ef_Lightningamor_01_char_root");
	--end	
end
function ShoulderTackle_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(0,"");
	return	ShoulderTackle_FindTargetEx(actor,action,action:GetActionNo(),action:GetSkillRange(0,actor),kTargets);
end

function ShoulderTackle_FindTargetEx(actor,action,iActionNo,iAttackRange,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 10;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);
	
	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 50
	end

	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,iParam2,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	local	iFoundTarget = action:FindTargetsEx(iActionNo,TAT_BAR,kParam,kTargets,kTargets);
	
	ODS("ShoulderTackle_FindTargetEx iActionNo:"..iActionNo.." iAttackRange:"..iAttackRange.." iFoundTarget:"..iFoundTarget.."\n");

	
	return	iFoundTarget;
end

function ShoulderTackle_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	SkillFunc_InitUseDmgEffect(action)
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("ShoulderTackle_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");

	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	ShoulderTackle_OnCastingCompleted(actor,action);

	-- 시작 위치 저장
	action:SetParamAsPoint(7, actor:GetPos());
	
	return true
end

function ShoulderTackle_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
--	local	iStep = action:GetParamInt(6);

	local curAnimSlot = action:GetCurrentSlot();

	if curAnimSlot == 0 then
		if animDone == true then
			action:SetSlot(action:GetCurrentSlot()+1);
			actor:PlayCurrentSlot();
			local	iBaseClassID = actor:GetPilot():GetBaseClassID()
			if iBaseClassID == CT_FIGHTER then
				actor:AttachParticle(493,"char_root","ef_fig_tackle_char_root");
			elseif iBaseClassID == CT_MAGICIAN then
				actor:AttachParticle(494,"char_root","ef_Lightningamor_01_char_root");
			end

			local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
			if kSoundID~="" then
				actor:AttachSound(2783,kSoundID);
			end	
		end	
	else
		if action:GetActionParam() == AP_FIRE then
			local	kStartPos = Point3(action:GetParamFloat(2),action:GetParamFloat(3),action:GetParamFloat(4));
			local	fMoveSpeed = 600;
			   
			local kNewPos = actor:GetPos();
			local kOldPos = action:GetParamAsPoint(7);
			kOldPos:SetZ(0)
			kNewPos:SetZ(0); --Z를 무시하고 계산한다. 떨어지는 것은 중력에 의한 것

			local fDistance = kNewPos:Distance(kOldPos);
			local fMaxDistance = action:GetSkillRange(0,actor)-30;

			--이동한 거리가 있을 경우
			if 0.0 < fDistance then
				--남은 이동 거리
				local fDistance2 = fMaxDistance - fDistance;

				--두 벡터를 이용하여 방향 벡터를 구하고
				--방향 벡터를 이용하여 실제로 이동하는 곳의 위치를 구한다.
				local kDirection = kNewPos:_Subtract(kOldPos);
				kDirection:Unitize();
				kDirection:Multiply(fMoveSpeed * frameTime);
				kDirection:Add(kNewPos);

				--이번 프레임에 이동 하는 거리
				local fDistance3 = kDirection:Distance(kOldPos);

				--이미 최대 대쉬 이동거리 거리를 벗어난 경우
				if fMaxDistance - fDistance3 < 0 then
					--fMoveSpeed값을 다시 세팅해야 한다.				
					fMoveSpeed = fDistance2 / frameTime;
				end
			end
		
			local kMovingDir = actor:GetLookingDir()
			kMovingDir:Multiply(fMoveSpeed);
			
			actor:SetMovingDelta(kMovingDir);
			
			local	fCurrentDistance = kStartPos:Distance(actor:GetPos());
			local	fElapsedTime = accumTime - action:GetParamFloat(5);

			ODS("fCurrentDistance : " .. fCurrentDistance .. " fMaxDistance : " .. fMaxDistance .. " fElapsedTime : " .. fElapsedTime, false, 1509);
			ODS("\n", false, 1509);
			
			if fCurrentDistance >= fMaxDistance or fElapsedTime > 1 or 1 > fMoveSpeed then
				
				action:SetParamInt(6,1);
				action:SetParamFloat(5,accumTime);
				
				ShoulderTackle_DoDamageToTargets(actor);
				
				actor:DetachFrom(493);
				
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
		end
	end

	return true
end

function ShoulderTackle_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(493);
	actor:DetachFrom(494);
	actor:EndBodyTrail();
	actor:FreeMove(false);
	
	return true;
end

function ShoulderTackle_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	if actor:IsMyActor() == false then
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
	
	if action:GetEnable() == false then
		if curParam == actionID then
			curAction:SetParam(0,"");
		end
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

	return false 
end
--[[
function ShoulderTackle_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
	
	CheckNil(nil==kActionResult);
	CheckNil(nil==kActionTargetInfo);
	
	local kAction = actor:GetAction()
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	local kActionID = kAction:GetID()
	
	if actor:GetPilotGuid():IsEqual(actorTarget:GetPilotGuid()) then
		return
	end
	
	local	iABVIndex = kActionTargetInfo:GetABVIndex();
	local pt = actorTarget:GetABVShapeWorldPos(iABVIndex);

	local   kHitTargetEffectID = kAction:GetScriptParam("HIT_TARGET_EFFECT_ID");
    local   kHitTargetEffectTargetNodeID = kAction:GetScriptParam("HIT_TARGET_EFFECT_TARGET_NODE");
    
    if kHitTargetEffectID ~= "" and kHitTargetEffectTargetNodeID ~= "" then
		local nUnitSize = actorTarget:GetAbil(AT_UNIT_SIZE);
		local nEffectScale = 1.0;
		if 1 == nUnitSize then --1:Small
			nEffectScale = 1.0;
			ODS("Small Monster\n");
		elseif 2 == nUnitSize then --2:Middle
			nEffectScale = 1.5;
			ODS("Middle Monster\n");
		elseif 3 == nUnitSize then --3:Large
			nEffectScale = 2.0;
			ODS("Large Monster\n");
		end
		
        actorTarget:AttachParticleS(12,kHitTargetEffectTargetNodeID,kHitTargetEffectID, nEffectScale);
	else
		if kActionResult:GetCritical() then
			actorTarget:AttachParticleToPoint(12, pt, "e_dmg_cri")
		else
			actorTarget:AttachParticleToPoint(12, pt, "e_dmg")
		end	
	end
		
	-- 충격 효과 적용
	local iShakeTime = g_fAttackHitHoldingTime * 1000;
	actor:SetAnimSpeedInPeriod(0.01, iShakeTime);
	actor:SetShakeInPeriod(5, iShakeTime/2);
	actorTarget:SetShakeInPeriod(5, iShakeTime);		
	
	-- 피격 소리 재생
    local actionName = kActionID
	if kActionResult:GetCritical() then
		actionName = "Critical_dmg"
	end
	actorTarget:PlayWeaponSound(WST_HIT, actor, actionName, 0, kActionTargetInfo);
end
]]
function ShoulderTackle_DoDamageToTargets(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	ODS("ShoulderTackle_DoDamageToTargets\n");
	local	kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	SkillHelpFunc_DefaultHitOneTime(actor,kAction, true);

end


function ShoulderTackle_OnEvent(actor,textKey)

	return	true;
end
