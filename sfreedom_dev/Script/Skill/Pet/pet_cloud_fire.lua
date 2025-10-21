-- [ATTACK_TIME] : 공격 틱 시간을 설정

function Skill_Pet_CloudFire_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()

	action:SetDoNotBroadCast(true)
	
	--	시작 시간 기록
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(0,g_world:GetAccumTime());
	action:SetParamFloat(1,g_world:GetAccumTime());
	
	-- Looking Dir
	local kLookDir = actor:GetLookingDir();
	action:SetParamFloat( 4, kLookDir:GetX() );
	action:SetParamFloat( 5, kLookDir:GetY() );
	action:SetParamFloat( 6, kLookDir:GetZ() );
	
	action:SetParamInt(4,0);
	
	-- 이펙트로 인해 스킬 범위가 조절될경우 그 값을 미리 계산하여 얻어온다
	local	iAttackRange = action:GetSkillRange(0,actor, true);
	local fCloudScale = iAttackRange/100
	actor:SetTargetScale(fCloudScale)
--	ODS("펫 루아iAttackRange:"..iAttackRange.."\n",false, 912)
	
--시작할때 한대 때리고 시작
	local   fLifeTime = actor:GetAbil(AT_LIFETIME)/1000.0;
	if fLifeTime == 0 then
	    fLifeTime = 3.0;
    end	
	action:SetParamFloat(9,fLifeTime)
	action:SetParamFloat(10,action:GetAbil(AT_COUNT));
	
	local iAttackTickTime = action:GetAbil(AT_COUNT);
	if iAttackTickTime == 0 then
		iAttackTickTime = 0.3;
	else
		 iAttackTickTime = fLifeTime/action:GetAbil(AT_COUNT);
--		 ODS("라이프타임:"..fLifeTime.."\natcount:"..action:GetAbil(AT_COUNT).."\n틱 타임: "..iAttackTickTime.."\n", false, 912)
	end	
	action:SetParamFloat(11,iAttackTickTime);
	
    action:CreateActionTargetList(actor);	    
    if IsSingleMode() or action:GetTargetCount() == 0 then	    
        Skill_Pet_CloudFire_OnTargetListModified(actor,action,false);	    
    else	    
        action:BroadCastTargetListModify(actor:GetPilot());
        action:ClearTargetList();	    
    end	   

	return true
end

function Skill_Pet_CloudFire_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    local   kParam = FindTargetParam();

	local	iAttackRange = action:GetSkillRange(0,actor);
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();

	kParam:SetParam_1(kPos,kDir);
   	kParam:SetParam_2(iAttackRange,30,50,0);

	local strFindTargetType = action:GetScriptParam("FIND_TARGET_TYPE");

	local iFindTargetType = TAT_RAY_DIR;
	
	if strFindTargetType == "RAY" then
	
	    iFindTargetType = TAT_RAY_DIR;
    	kParam:SetParam_1(kPos,kDir);
    	kParam:SetParam_2(iAttackRange,30,50,0);
	
	elseif strFindTargetType == "BAR" then
	    iFindTargetType = TAT_BAR;
    	kParam:SetParam_1(kPos,kDir);
    	kParam:SetParam_2(iAttackRange,60,50,0);
	
	elseif strFindTargetType == "SPHERE" then
	    iFindTargetType = TAT_SPHERE;
    	kParam:SetParam_1(kPos,kDir);
    	kParam:SetParam_2(0,0,iAttackRange,0);
	end
	
	return action:FindTargets(iFindTargetType,kParam,kTargets,kTargets);
end

function Skill_Pet_CloudFire_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
        SkillHelpFunc_DefaultHitOneTime(actor,action, true);
		
		local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end			
    end

end

function Skill_Pet_CloudFire_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local iSlotNum = action:GetCurrentSlot();
	local iState = action:GetParamInt(4);
	
	local   fLifeTime = action:GetParamFloat(9)	
	local iAttackTickTime = action:GetParamFloat(11)	
	local   fElapsedTIme = accumTime - action:GetParamFloat(1);
	--ODS("fElapsedTIme>iAttackTickTime:"..fElapsedTIme.." > "..iAttackTickTime.."\n", false, 912)
	
	if fElapsedTIme > iAttackTickTime then
--		ODS("데미지 들어감\n", false, 912)
	    action:SetParamFloat(1,accumTime);
	    action:CreateActionTargetList(actor);
	    
	    if IsSingleMode() or action:GetTargetCount() == 0 then	    
	        Skill_Pet_CloudFire_OnTargetListModified(actor,action,false);	    
	    else	    
	        action:BroadCastTargetListModify(actor:GetPilot());
	        action:ClearTargetList();
	    end	    
	end

	if iSlotNum == 0 then
		local   fTotalElapsedTime = accumTime - action:GetParamFloat(0);
		if fTotalElapsedTime > fLifeTime then
			actor:PlayNext();
		end

	elseif iSlotNum == 1 then
	
		if iState == 0 then
			action:SetParamInt(4,1);
			action:SetParamFloat(6,g_world:GetAccumTime());
		end
	end

	if iState == 1 then
		if accumTime - action:GetParamFloat(6) > 0.4 then
			action:SetParamFloat(6,g_world:GetAccumTime());
			actor:SetTargetAlpha(actor:GetAlpha(),0, 0.7);
			action:SetParamInt(4,2);
		end
	elseif iState == 2 then
		if accumTime - action:GetParamFloat(6) > 1.0 then
			return false;
		end
	end


	return true
end

function Skill_Pet_CloudFire_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	if actor:IsUnderMyControl() then
--		Net_C_M_REQ_DELETE_ENTITY(actor:GetPilotGuid());
--	end

	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid());
end

function Skill_Pet_CloudFire_OnLeave(actor, action)

	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local iSlotNum = action:GetCurrentSlot();

	if iSlotNum == 0 then
		return false;
	end

	return true;
end

function Skill_Pet_CloudFire_OnCastingCompleted(actor, action)
end