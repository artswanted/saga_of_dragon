-- [WEAPON_XML] : 무기 외형을 바꾸고 싶을 경우 무기 아이템의 XML 경로.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [NO_WEAPON_TRAIL] : 웨폰 트레일 사용하지 않을것인가

-- Melee
function StormBlade_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function StormBlade_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	StormBlade_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end
function StormBlade_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("RestoreItemModel StormBlade_Fire\n");
	
	if actor:IsMyActor() then
		action:StartTimer(2.4,0.2,0);
	end
	
	--	무기 바꾸기 처리
	if action:GetScriptParam("WEAPON_XML") ~= "" then
		actor:ChangeItemModel(ITEM_WEAPON, action:GetScriptParam("WEAPON_XML"));
		ODS("ChangeItemModel "..action:GetScriptParam("WEAPON_XML").."\n");
	end
			
	if action:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    actor:StartWeaponTrail();
	end
		
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(3,g_world:GetAccumTime())	--	 시작시간 기록
	actor:StopJump();
	actor:FreeMove(true);
	
	action:SetParamInt(4,1)	--	Start Rotation
	action:SetParamFloat(5,g_world:GetAccumTime())	--	Rotation Start Time
	action:SetParamFloat(6,actor:GetLookingDir():GetX());
	action:SetParamFloat(7,actor:GetLookingDir():GetY());
	action:SetParamFloat(8,actor:GetLookingDir():GetZ());
	action:SetParamInt(9,0)	--	Hit Count	
	action:SetParamFloat(10,g_world:GetAccumTime());
	action:SetParamFloat(11,actor:GetPos():GetX());
	action:SetParamFloat(12,actor:GetPos():GetY());
	action:SetParamFloat(13,actor:GetPos():GetZ());

	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	
	
	 local   kFireEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID");
	local   kFireEffectTargetNodeID = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE");
	local   kFireEffectTargetNodeIDToPos = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS");
	local   kFireEffectTargetNodeIDToPosWithRotate = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE");
	
	if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" or kFireEffectTargetNodeIDToPos ~= ""  or kFireEffectTargetNodeIDToPosWithRotate ~= "" then
		local	fFireEffectScale = tonumber(action:GetScriptParam("FIRE_CASTER_EFFECT_SCALE"));
		if(0 == fFireEffectScale) then fFireEffectScale = 1 end
		
		if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
			
			local   kDir = actor:GetLookingDir();
			local   fDistance = 30;
			kDir:Multiply(fDistance);
			kDir:Add(actor:GetPos());
			kDir:SetZ(kDir:GetZ()-28);
			
			actor:AttachParticleToPointS(7212,kDir,kFireEffectID, fFireEffectScale);
		elseif kFireEffectTargetNodeID ~= "" then
			actor:AttachParticle(7212,kFireEffectTargetNodeID,kFireEffectID);
		elseif kFireEffectTargetNodeIDToPos ~= "" then
			actor:AttachParticleToPointS(7212,actor:GetNodeWorldPos(kFireEffectTargetNodeIDToPos),kFireEffectID, fFireEffectScale);
		else
			actor:AttachParticleToPointWithRotate(7212,actor:GetNodeWorldPos(kFireEffectTargetNodeIDToPosWithRotate), kFireEffectID, actor:GetRotateQuaternion(), fFireEffectScale)
		end
	
	end	
	kFireEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID2");
	kFireEffectTargetNodeID = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE2");
	
	if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" then	
		if kFireEffectTargetNodeID == "__FRONT_ACTOR" then			
			local   kDir = actor:GetLookingDir();
			local   fDistance = 30;
			kDir:Multiply(fDistance);
			kDir:Add(actor:GetPos());
			kDir:SetZ(kDir:GetZ()-28);			
			actor:AttachParticleToPoint(7213,kDir,kFireEffectID);			
		else
			actor:AttachParticle(7213,kFireEffectTargetNodeID,kFireEffectID);
		end	
	end		

	local iTotalHit = 0;
	local kSkillDef = GetSkillDef(action:GetActionNo());
	if kSkillDef:IsNil() == false then
		iTotalHit  = kSkillDef:GetAbil(AT_COUNT);
	end

	if iTotalHit == nil or iTotalHit == 0 then
		if kSkillDef:IsNil() == false then
			iTotalHit = kSkillDef:GetAbil(AT_LEVEL) + 1;
		end		
	end
	
--	action:GetID() == "a_Sky Pirce01"

	action:SetParamInt(30,iTotalHit);
	ODS("iTotalHit : "..iTotalHit.."\n");
	
end
function StormBlade_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
		SkillHelpFunc_DefaultHitOneTime(actor,action, false);
    end

end

function StormBlade_OnTimer(actor,fAccumTime,action,iTimerID)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("StormBlade_OnTimer fAccumTime:"..fAccumTime.."\n",false, 1509);

	local	iHitCount = action:GetParamInt(9);
	local	iTotalHit = action:GetParamInt(30);	
	if iHitCount == iTotalHit then
		return true
	end

	action:CreateActionTargetList(actor);
	
	local	kTargetList = action:GetTargetList();
	local	iTargetCount = kTargetList:size();
	if 0 < iTargetCount then
		action:BroadCastTargetListModify(actor:GetPilot());
	end

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(10,g_world:GetAccumTime());	
	
	ODS("StormBlade_HitOneTime iHitCount:"..iHitCount.."\n",false, 1509);	
	
	iHitCount = iHitCount + 1;
	action:SetParamInt(9,iHitCount);
		
	return	true	

end

function StormBlade_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(0,"");
	return	StormBlade_FindTargetEx(actor,action,action:GetActionNo(),action:GetSkillRange(0,actor),kTargets);
end

function StormBlade_FindTargetEx(actor,action,iActionNo,iAttackRange,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kPos = Point3(action:GetParamFloat(11),action:GetParamFloat(12),action:GetParamFloat(13) - 30);
	local	kDir = actor:GetLookingDir();

	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,Point3(0,0,1));
	kParam:SetParam_2(400,iAttackRange*2,0,0);
	kParam:SetParam_3(true,FTO_BLOWUP);

	local iFound = action:FindTargetsEx(iActionNo,TAT_BAR,kParam,kTargets,kTargets);
	return iFound;
end

function StormBlade_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
-- 	local actorID = actor:GetID()
-- 	local actionID = action:GetID()
-- 	local weapontype = actor:GetEquippedWeaponType();
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end
	local fRigingMovingDelta = action:GetScriptParamAsFloat("RISING_MOVING_DELTA")
	if(0 == fRigingMovingDelta) then fRigingMovingDelta = 50 end
	action:SetParamFloat(20, fRigingMovingDelta)
	StormBlade_OnCastingCompleted(actor,action);
	
	SkillFunc_InitSpinDegPerSec(action)
	return true
end

function StormBlade_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
	
	local	iTotalHit = action:GetParamInt(30);

	local	fElapsedTime = accumTime - action:GetParamFloat(3);
	if fElapsedTime > ((0.2 * iTotalHit) + 0.1) or action:GetParamInt(9) >= iTotalHit then
		ODS("\n EndEndEnd \n",false, 1509)
	
		actor:FreeMove(false);
	
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
	if action:GetParamInt(9) < iTotalHit then
		ODS("\n---------- Cur Hit "..action:GetParamInt(9).."     Total "..iTotalHit.."    ---------\n",false, 1509)
		
		local fSpinDeg = SkillFunc_GetSpinDegPerSec(action)
		if(0 < fSpinDeg) then
			actor:IncRotate(frameTime*fSpinDeg*DEGTORAD);
		end
		local fRigingMovingDelta = action:GetParamFloat(20)
		actor:SetMovingDelta(Point3(0,0,fRigingMovingDelta));
		
	end

	return true
end

function StormBlade_OnCleanUp(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	ODS("RestoreItemModel StormBlade_ReallyLeave\n");
	
	--	무기 원래대로
	if actor:GetAction():GetScriptParam("WEAPON_XML") ~= "" then
		actor:RestoreItemModel(ITEM_WEAPON);
	end
	actor:DetachFrom(7212, true)
	actor:FreeMove(false);
	actor:StopJump();
	
	if actor:GetAction():GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    actor:EndWeaponTrail();
	end
	
	return true;
end

function StormBlade_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
		
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	

	
	if actionID == "a_jump" then 
		if curParam == "end" then 
			action:SetSlot(2);
			action:SetDoNotBroadCast(true);
			return true;
		end
		return	false;
	end
	
	if actor:IsMyActor() == false then
		return true;
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

function StormBlade_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
	
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
	
	if kActionResult:GetCritical() then
		actorTarget:AttachParticleToPoint(12, pt, "e_dmg_cri")
	else
		actorTarget:AttachParticleToPoint(12, pt, "e_dmg")
	end	
	

	-- 충격 효과 적용
	local iShakeTime = g_fAttackHitHoldingTime * 1000;
	actor:SetAnimSpeedInPeriod(0.01, iShakeTime);
	actor:SetShakeInPeriod(5, iShakeTime/2);
	actorTarget:SetShakeInPeriod(5, iShakeTime);		
	
	-- 피격 소리 재생
	

end
--[[
function StormBlade_HitOneTime(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	
	
	local iTargetCount = action:GetTargetCount();
	local i = 0;
	if iTargetCount>0 then
		
		ODS("사운드 나온다\n",false, 1509)
		local bWeaponSoundPlayed = false;
		while i<iTargetCount do
		
			local actionResult = action:GetTargetActionResult(i);
			if actionResult:IsNil() == false then

				local kTargetGUID = action:GetTargetGUID(i);
				local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
				if kTargetPilot:IsNil() == false and actionResult:IsMissed() == false then
					
					local actorTarget = kTargetPilot:GetActor();
					
					if bWeaponSoundPlayed == false then
					    bWeaponSoundPlayed = true;
					    local actionName = action:GetID()
						if actionResult:GetCritical() then
							actionName = "Critical_dmg"
						end
						actorTarget:PlayWeaponSound(WST_HIT, actor, actionName, 0, action:GetTargetInfo(i));
					end
					
					StormBlade_DoDamage(actor,actorTarget,actionResult,action:GetTargetInfo(i));
					
				end
				
			end
			
			i=i+1;
		
		end
	end
			
	action:GetTargetList():ApplyActionEffects();	
end
]]
function StormBlade_OnEvent(actor,textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	return	true;
end
