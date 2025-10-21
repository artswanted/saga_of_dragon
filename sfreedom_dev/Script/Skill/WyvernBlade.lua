-- [WEAPON_XML] : 무기 외형을 바꾸고 싶을 경우 무기 아이템의 XML 경로.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [NO_WEAPON_TRAIL] : 웨폰 트레일 사용하지 않을것인가

-- Melee
function WyvernBlade_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function WyvernBlade_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	WyvernBlade_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end
function WyvernBlade_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	ODS("RestoreItemModel WyvernBlade_Fire\n");
	
	if actor:IsMyActor() then
		action:StartTimer(3,0.3,0);
	end
	
	--	무기 바꾸기 처리
	if action:GetScriptParam("WEAPON_XML") ~= "" then
		actor:ChangeItemModel(ITEM_WEAPON, action:GetScriptParam("WEAPON_XML"));
		ODS("ChangeItemModel "..action:GetScriptParam("WEAPON_XML").."\n");
	end
			
	if action:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    actor:StartWeaponTrail();
	end
		
	action:SetParamFloat(3,g_world:GetAccumTime())	--	 시작시간 기록
	
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

	local iTotalHit = 0;
	local kSkillDef = GetSkillDef(action:GetActionNo());
	if kSkillDef:IsNil() == false then
		iTotalHit  = kSkillDef:GetAbil(AT_COUNT);
	end
	action:SetParamInt(30,iTotalHit);
	
	actor:AttachParticle(581,"char_root","ef_wybernblade_01_char_root");
	
	actor:AttachSound(2784,"Wybern_Blade");
	
end
function WyvernBlade_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then

        if actor:IsMyActor()  then
			action:GetTargetList():ApplyActionEffects(true,true);
			return;
		end
		
        WyvernBlade_HitOneTime(actor,action);
    
    end

end

function WyvernBlade_OnTimer(actor,fAccumTime,action,iTimerID)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("WyvernBlade_OnTimer fAccumTime:"..fAccumTime.."\n");

	action:CreateActionTargetList(actor);
	
	if IsSingleMode() then
    
		WyvernBlade_HitOneTime(actor,action)
		WyvernBlade_OnTargetListModified(actor,action,false)
        
		return true;
	else
    
		action:BroadCastTargetListModify(actor:GetPilot());
        
	end			
	
	if actor:IsMyActor() then
		WyvernBlade_HitOneTime(actor,action)
	end
	
	return	true	

end

function WyvernBlade_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(0,"");
	return	WyvernBlade_FindTargetEx(actor,action,action:GetActionNo(),action:GetSkillRange(0,actor),kTargets);
end

function WyvernBlade_FindTargetEx(actor,action,iActionNo,iAttackRange,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kPos = Point3(action:GetParamFloat(11),action:GetParamFloat(12),action:GetParamFloat(13) - 20);
	local	kDir = actor:GetLookingDir();

	local kParam = FindTargetParam();
	kParam:SetParam_1(kPos,Point3(0,0,1));
	kParam:SetParam_2(400,iAttackRange*2,0,0);
	kParam:SetParam_3(true,FTO_BLOWUP);

	local iSkillLevel = action:GetAbil(AT_LEVEL);
	if iSkillLevel <= 4 then
		kParam:SetParam_3(true, FTO_NORMAL + FTO_DOWN + FTO_BLOWUP);
	elseif iSkillLevel <= 2 then
		kParam:SetParam_3(true, FTO_NORMAL + FTO_BLOWUP);
	else
		kParam:SetParam_3(true, FTO_BLOWUP);
	end
	
	local iFound = action:FindTargetsEx(iActionNo,TAT_BAR,kParam,kTargets,kTargets);
	return iFound;
end

function WyvernBlade_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();

	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end
	
	action:SetParamFloat(30, g_world:GetAccumTime())
	WyvernBlade_OnCastingCompleted(actor,action);
	
	return true
end

function WyvernBlade_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local nextActionName = action:GetNextActionName()
	
	local iTotalHit = action:GetParamInt(30);

	if actor:IsMyActor() then
		local	bKeyUp = KeyIsDown(KEY_UP);
		local	bKeyDown = KeyIsDown(KEY_DOWN);
		local	bKeyLeft = KeyIsDown(KEY_LEFT);
		local	bKeyRight = KeyIsDown(KEY_RIGHT);
		
		local	iDir = 0
		if bKeyUp then 
			iDir = iDir + DIR_UP
		end
		if bKeyDown then
			iDir = iDir + DIR_DOWN
		end
		if bKeyLeft then
			iDir = iDir + DIR_LEFT
		end
		if bKeyRight then
			iDir = iDir + DIR_RIGHT
		end

		if iDir ~= 0 then
			local iMoveSpeed = actor:GetAbil(AT_C_MOVESPEED) * 0.5
			local kDirection = actor:GetDirectionVector(iDir);

			kDirection:Multiply(iMoveSpeed);
			actor:SetMovingDelta(kDirection);
			local fTimeDelta = g_world:GetAccumTime() - action:GetParamFloat(30)
			if(fTimeDelta > 0.02) then -- 서버로 이동 위치 갱신 보내는 시간 간격
				g_pilotMan:BroadcastDirection(actor:GetPilot(), iDir)
				action:SetParamFloat(30, g_world:GetAccumTime())
			end
		end
	end	

	local	fElapsedTime = accumTime - action:GetParamFloat(3);
	if fElapsedTime > 2 or action:GetParamInt(9) >= iTotalHit then
		ODS("\n EndEndEnd \n")
	
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
		--ODS("\n---------- Cur Hit "..action:GetParamInt(9).."     Total "..iTotalHit.."    ---------\n")
	
		actor:IncRotate(frameTime*5*460.0*3.141592/180.0);
		
	end

	return true
end

function WyvernBlade_OnCleanUp(actor)

	ODS("RestoreItemModel WyvernBlade_ReallyLeave\n");

	--	무기 원래대로
	if actor:GetAction():GetScriptParam("WEAPON_XML") ~= "" then
		actor:RestoreItemModel(ITEM_WEAPON);
	end

	if actor:GetAction():GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    actor:EndWeaponTrail();
	end
	
	return true;
end

function WyvernBlade_OnLeave(actor, action)

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

function WyvernBlade_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
	
	CheckNil(nil==kActionResult);
	CheckNil(nil==kActionTargetInfo);
	
	local kAction = actor:GetAction()
	local kActionID = kAction:GetID()
		
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
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
function WyvernBlade_HitOneTime(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	action:SetParamFloat(10,g_world:GetAccumTime());
	local	iHitCount = action:GetParamInt(9);
	
	ODS("WyvernBlade_HitOneTime iHitCount:"..iHitCount.."\n");
	
	local	iTotalHit = 5;
	
	if iHitCount == iTotalHit then
		return
	end
	
	SkillHelpFunc_DefaultHitOneTime(actor,action, true);
	
	iHitCount = iHitCount + 1;
	action:SetParamInt(9,iHitCount);
	
	if iHitCount == iTotalHit then
		return	true
	end
end

function WyvernBlade_OnEvent(actor,textKey)
	
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
