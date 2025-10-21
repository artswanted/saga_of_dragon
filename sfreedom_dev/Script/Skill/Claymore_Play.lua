function Skill_Claymore_Play_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_Claymore_Play_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
	
	if actor:IsUnderMyControl() then
	
		local   fLifeTime = actor:GetAbil(AT_LIFETIME)/1000.0;
	        	
		if fLifeTime == 0 then
			fLifeTime = 8.5;
		end
		actor:SetInstallTimerGauge(fLifeTime);
	end

end

function Skill_Claymore_Play_Fire(actor,action)
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    --  시작 시간
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
    action:SetParamFloat(2,g_world:GetAccumTime());
    action:SetParamFloat(3,g_world:GetAccumTime());


end

function Skill_Claymore_Play_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local actionID = action:GetID()
	action:SetDoNotBroadCast(true);
	
	--ODS("Skill_Claymore_Play_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	action:SetParam(0,"Idle");  --  State

	local kCallerPilot = g_pilotMan:FindPilot(actor:GetCallerGuid());

	if kCallerPilot ~= nil or kCallerPilot:IsNil() == false then
		local kCallerActor = kCallerPilot:GetActor();

		if kCallerActor ~= nil or kCallerActor:IsNil() == false then
			local bSettingClaymore = false;
			for i=1,8 do
				local kClaymoreGUID = GUID(kCallerActor:GetParam("Claymore_GUID_"..i));
				local kClaymorePilot = g_pilotMan:FindPilot(kClaymoreGUID);
				if kClaymorePilot == nil or kClaymorePilot:IsNil() == true then
					if bSettingClaymore == false then
						kCallerActor:SetParam("Claymore_GUID_"..i, actor:GetPilotGuid():GetString());
						bSettingClaymore = true;
						--WriteToConsole("Claymore_GUID_"..i.." Setting\n");
					end					
				end
			end
		end
	end

	--actor:SetParam("Claymore_GUID_"..i,kGUID:GetString());
	
	-- Looking Dir
	local kLookDir = actor:GetLookingDir();
	action:SetParamFloat( 4, kLookDir:GetX() );
	action:SetParamFloat( 5, kLookDir:GetY() );
	action:SetParamFloat( 6, kLookDir:GetZ() );
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Claymore_Play_OnCastingCompleted(actor,action);
	
	actor:FreeMove(true)
	return true
end

function Skill_Claymore_Play_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
    
		--ODS("Skill_Claymore_Play_FindTarget Skill_Claymore_Play_OnTargetListModified\n");
    
        SkillHelpFunc_DefaultHitOneTime(actor,action, true);
        
        actor:AttachParticle(1,"Dummy01","ef_Claymore_dummy01");
        
        --  5초 뒤에 제거한다. 현재 시간을 기록한다.
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
        action:SetParamFloat(1,g_world:GetAccumTime());
        action:SetParam(0,"WaitForDestroying");
    end

end

function Skill_Claymore_Play_FindTarget(actor,action,bForceToBomb)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(4,"FullSearch");
	
    action:CreateActionTargetList(actor,true);
    
    if IsSingleMode() or action:GetTargetCount() == 0 then		
        Skill_Claymore_Play_OnTargetListModified(actor,action,false);        
    else
    	--ODS("Skill_Claymore_Play_FindTarget BroadCastTargetListModify\n");
        action:BroadCastTargetListModify(actor:GetPilot());
        action:ClearTargetList();    
    end    
    
    return true
end

function Skill_Claymore_Play_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local   kState = action:GetParam(0);
	
	--ODS("kState : "..kState.."\n");
	
	if  kState == "Idle" then
        --  10초가 흐르면 스스로 제거된다.
        local   fTotalElapsedTime = accumTime - action:GetParamFloat(2)
        local   fLifeTime = actor:GetAbil(AT_LIFETIME)/1000.0;
    	
        if fLifeTime == 0 then
            fLifeTime = 10.0;
        end
        
        ODS("Skill_Claymore_Play_OnUpdate fTotalElapsedTime : "..fTotalElapsedTime.."\n");
        
        if fTotalElapsedTime > fLifeTime-2 then
            action:SetParam(0,"WaitForDestroying");
            Skill_Claymore_Play_FindTarget(actor,action,true)
			actor:AttachSound(2784, "Claymore1");
            return true;
        end
	elseif  kState == "bomb_force" then --  강제 폭발(디토네이터 스킬같은거)
        Skill_Claymore_Play_FindTarget(actor,action,true)
        action:SetParam(0,"WaitForDestroying");
		actor:AttachSound(2784, "Claymore1");
        --  타이머 제거
	elseif kState == "WaitForDestroying" then
		local   fTotalElapsedTime = accumTime - action:GetParamFloat(1)
		if fTotalElapsedTime> 1 then
			action:SetParam(0,"end");
			return  false;
        end	
	end

	return true
end

function Skill_Claymore_Play_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

--	local	kFindType = action:GetParam(4)
	
	local	fFindRange = action:GetSkillRange(0,actor);

    local   kParam = FindTargetParam();

	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 40
	end	

	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
	kParam:SetParam_2(fFindRange,iParam2,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return action:FindTargets(TAT_RAY_DIR,kParam,kTargets,kTargets);

end

function Skill_Claymore_Play_OnCleanUp(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
			
    if actor:IsUnderMyControl() then
		actor:DestroyInstallTimerGauge();
	end
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
    g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid());

	return true;
end

function Skill_Claymore_Play_OnLeave(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--ODS("Skill_Claymore_Play_OnLeave NextAction : "..action:GetID().."\n");
	
	local	kParam = actor:GetAction():GetParam(0)
	
	return (kParam == "end");
end
