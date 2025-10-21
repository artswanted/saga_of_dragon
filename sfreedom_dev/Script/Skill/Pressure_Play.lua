
function Skill_Pressure_Play_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	--	시작 시간 기록
	action:SetParamFloat(0,g_world:GetAccumTime());
	
	-- 바닥 높이
	action:SetParamFloat(2,actor:GetPos():GetZ());
	
	--	State
	action:SetParamInt(4,0);
	action:SetParamFloat(6,0);
	
	actor:SetMovingDelta(Point3(0,0,0));
	actor:StopJump();
	actor:FreeMove(true);

	local iSkillLevel = action:GetParamInt(16);
	actor:SetTargetScale(0.5 + (iSkillLevel * 0.2));
	-- 바닥을 찾자

	return true
end

function Skill_Pressure_Play_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if( CheckNil(g_world == nil) ) then return false end 
	if( CheckNil(g_world:IsNil()) ) then return false end 
	
	local iState = action:GetParamInt(4)
		
	if iState == 0 then
		actor:SetHideShadow(true);
    	
	    --  시작 높이
	    local   fStartHeight = action:GetParamFloat(1)
    	
	    --  흐른 시간
	    local   fElapsedTime = accumTime - action:GetParamFloat(0);
	    local   fMaxDownTime = 10;
    	
	    if fElapsedTime>fMaxDownTime then
	        fElapsedTime = fMaxDownTime;
	    end

	    local   fMoveSpeed = -(12000*fElapsedTime);
    	
	    --  현재 높이
	    local   fNowHeight = fStartHeight +fMoveSpeed*fElapsedTime;
	    ODS("fNowHeight : "..fNowHeight.."\n")
    	
	    -- 바닥 높이
	    local   fFloorHeight = action:GetParamFloat(2);
    	
	    if fNowHeight<fFloorHeight then
	        fNowHeight = fFloorHeight
	    end
    	
	    actor:SetTranslate(Point3(actor:GetPos():GetX(),actor:GetPos():GetY(),fNowHeight));
    	
	    if fElapsedTime == fMaxDownTime or fNowHeight<=fFloorHeight then
        	
	        Skill_Pressure_Play_DoDamage(actor,action)
	        action:SetParamInt(4,1)
			action:SetParamFloat(6,g_world:GetAccumTime());
			action:SetSlot(1);
			actor:PlayCurrentSlot();
	    end
	elseif iState == 1 then
		if accumTime - action:GetParamFloat(6) > 0.5 then
			action:SetParamFloat(6,g_world:GetAccumTime());			
			action:SetParamInt(4,2);
		end
	elseif iState == 2 then
		if accumTime - action:GetParamFloat(6) > 1.0 then
			action:SetParamFloat(6,g_world:GetAccumTime());
			actor:SetTargetAlpha(actor:GetAlpha(),0, 0.7);
			action:SetParamInt(4,3);
		end
	elseif iState == 3 then
		if accumTime - action:GetParamFloat(6) > 1.0 then
			return false;
		end
	end
	

	return true
end

function Skill_Pressure_Play_DoDamage(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2784,kSoundID);
	end	

	local kOriginalActorGUID = action:GetGUIDInContainer(0);
	local kOriginalActor = nil;
	if kOriginalActorGUID:IsNil() == false then

		local	kPilot = g_pilotMan:FindPilot(kOriginalActorGUID);
		if kPilot:IsNil() == false then
			kOriginalActor = kPilot:GetActor();
		end
	
	end	    

	if kOriginalActor ~= nil then
		if kOriginalActor:IsNil() == false then
			if kOriginalActor:IsMyActor() then
				QuakeCamera(g_fMeleeDropQuakeValue[1]
				, g_fMeleeDropQuakeValue[2]
				, g_fMeleeDropQuakeValue[3]
				, g_fMeleeDropQuakeValue[4]
				, g_fMeleeDropQuakeValue[5])
			end
		end
	end
	
    local	kPos = actor:GetPos();
	local iSkillLevel = action:GetParamInt(16);
    actor:AttachParticleToPointS(12, kPos, "ef_thi_jump_down_attk", 0.6 + (iSkillLevel * 0.06))
	
    SkillHelpFunc_DefaultHitOneTime(actor,action, true); 

end

function Skill_Pressure_Play_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:EndBodyTrail();
--	if actor:IsUnderMyControl() then
--		Net_C_M_REQ_DELETE_ENTITY(actor:GetPilotGuid());
--	end
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
end

function Skill_Pressure_Play_OnLeave(actor, action)
	return true;
end

function PressurePlayInit(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	actor:SetHideShadow(true);
	actor:SetTargetAlpha(0,1, 0.3);
end

function Skill_Pressure_Play_OnCastingCompleted(actor, action)
end