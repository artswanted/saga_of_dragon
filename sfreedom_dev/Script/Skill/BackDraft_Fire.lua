-- [ATTACK_TIME] : 공격 틱 시간을 설정

function Skill_BackDraft_Fire_OnEnter(actor, action)

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
	
	action:SetParamInt(7,0);
	if("TRUE" == action:GetScriptParam("NO_TICK_ATTACK") ) then
		action:SetParamInt(8,1);
	end
	
	local kTime = actor:GetAbil(AT_LIFETIME)
	if 0~= kTime then
		kTime = kTime + 2000	--있으면 2초 더 주자
		action:SetParamFloat(10, action:GetParamFloat(1)+kTime*0.001)
	else
		action:SetParamFloat(10, 0)
	end
	
	if("TRUE" == action:GetScriptParam("DARK_BG") ) then
		local kMyPilot = actor:GetPilot()
		if not kMyPilot:IsNil() then 	
			local kMyUnit = kMyPilot:GetUnit()
			if not kMyUnit:IsNil() then 
				local kCallerPilot = g_pilotMan:FindPilot(kMyUnit:GetCaller())
				if not kCallerPilot:IsNil() then 
					local kCallerActor = kCallerPilot:GetActor()
					if not kCallerActor:IsNil() then
						SkillHelpFunc_MakeDarkBG(kCallerActor)
					end	
				end
			end
		end
	end
	
	local   fLifeTime = actor:GetAbil(AT_LIFETIME)/1000.0;
	if fLifeTime == 0 then
	    fLifeTime = 3.0;
    end	
	action:SetParamFloat(30, fLifeTime)
	
	local fAttackTickTime = tonumber(action:GetScriptParam("ATTACK_TIME"));
	if fAttackTickTime == nil then
		local iMaxHitCount = action:GetAbil(AT_COUNT)
		ODS("iMaxHitCount:"..iMaxHitCount.."\n", false, 912)
		if(0 == iMaxHitCount) then 			
			fAttackTickTime = 0.3;
		else
			fAttackTickTime = fLifeTime/iMaxHitCount
		end
	end
	
	action:SetParamFloat(40, fAttackTickTime)
	action:SetParamInt(20, 0)
	
	local iMaxHitCount = action:GetAbil(AT_COUNT)
	ODS("iMaxHitCount:"..iMaxHitCount.."\n", false, 912)
	actor:FreeMove(true)	
	return true
end

function Skill_BackDraft_Fire_OnFindTarget(actor,action,kTargets)

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
    	kParam:SetParam_2(0,0,50,0);
	end
	
	return action:FindTargets(iFindTargetType,kParam,kTargets,kTargets);
end

function Skill_BackDraft_Fire_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
		if action:GetScriptParam("NO_DAMAGE_HOLD") ~= "TRUE" then
			SkillHelpFunc_DefaultHitOneTime(actor,action, true);
		else
			SkillHelpFunc_DefaultHitOneTime(actor,action)
		end
		
		local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end			
    end

end

function Skill_BackDraft_Fire_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local iSlotNum = action:GetCurrentSlot();
	local iState = action:GetParamInt(7);

	local   fLifeTime = action:GetParamFloat(30)
	if fLifeTime == 0 then
	    fLifeTime = 3.0;
    end	
	
	local fAttackTickTime = action:GetParamFloat(40)
	local iMaxHitCount = action:GetAbil(AT_COUNT)
	if fAttackTickTime == nil then
		ODS("iMaxHitCount:"..iMaxHitCount.."\n", false, 912)
		if(0 == iMaxHitCount) then 			
			fAttackTickTime = 0.3;
		else
			fAttackTickTime = fLifeTime/iMaxHitCount
		end
	end
	
	local iHitCount = action:GetParamInt(20);
	
	local   fElapsedTIme = accumTime - action:GetParamFloat(1);
	if(0 == action:GetParamInt(8)) then
		if fElapsedTIme > fAttackTickTime 
			and iHitCount < iMaxHitCount
		then
						
			action:SetParamInt(20, iHitCount + 1); --카운터를 하나 증가시켜 준다.
			action:SetParamFloat(1,accumTime);
			action:CreateActionTargetList(actor);
			
			if IsSingleMode() or action:GetTargetCount() == 0 then
			
				Skill_BackDraft_Fire_OnTargetListModified(actor,action,false);
			
			else
			
				action:BroadCastTargetListModify(actor:GetPilot());
				action:ClearTargetList();
			
			end
			
		end
	end
	
	if iSlotNum == 0 then
		local   fTotalElapsedTime = accumTime - action:GetParamFloat(0);
		if fTotalElapsedTime > fLifeTime then
			actor:PlayNext();
		end

	elseif iSlotNum == 1 then
	
		if iState == 0 then
			action:SetParamInt(7,1);
			action:SetParamFloat(6,g_world:GetAccumTime());
		end
	end

	if iState == 1 then
		if accumTime - action:GetParamFloat(6) > 0.4 then
			action:SetParamFloat(6,g_world:GetAccumTime());
			actor:SetTargetAlpha(actor:GetAlpha(),0, 0.7);
			action:SetParamInt(7,2);
		end
	elseif iState == 2 then
		if accumTime - action:GetParamFloat(6) > 1.0 then
			return false;
		end
	end
	
	if(0 < action:GetParamFloat(10) ) then
		if accumTime > action:GetParamFloat(10) then
			g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())	--강제로 지움
			return false
		end
	end


	return true
end

function Skill_BackDraft_Fire_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	if actor:IsUnderMyControl() then
--		Net_C_M_REQ_DELETE_ENTITY(actor:GetPilotGuid());
--	end

	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid());
end

function Skill_BackDraft_Fire_OnLeave(actor, action)

	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local iSlotNum = action:GetCurrentSlot();

	if iSlotNum == 0 then
		return false;
	end

	return true;
end

function Skill_BackDraft_Fire_OnCastingCompleted(actor, action)
end

function Skill_BackDraft_Fire_OnEvent(actor,textKey)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local kAction = actor:GetAction();
		
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end	
	
	if "hit" == textKey or "fire" == textKey then
		local iMaxHitCount = kAction:GetAbil(AT_COUNT)
		
		local iHitCount = kAction:GetParamInt(20);
		if( iHitCount >= iMaxHitCount ) then return end
		
		kAction:SetParamInt(20, iHitCount + 1); --카운터를 하나 증가시켜 준다.
	
		if actor:IsUnderMyControl() then
			
			kAction:CreateActionTargetList(actor);
			if IsSingleMode() then		        
				SkillHelpFunc_DefaultHitOneTime(actor,kAction)
				Skill_BackDraft_Fire_OnTargetListModified(actor,kAction,false)		            
				return;
			else
				local	kTargetList = kAction:GetTargetList();
				local	iTargetCount = kTargetList:size();

				if 0 < iTargetCount then
					kAction:BroadCastTargetListModify(actor:GetPilot());
				end
			end			
			
		end

		kAction:ClearTargetList();
	elseif "fade_out" == textKey then
		actor:SetTargetAlpha(actor:GetAlpha(), 0, 0.5)
				
		if("TRUE" == kAction:GetScriptParam("DARK_BG") ) then
			local kMyPilot = actor:GetPilot()
			if not kMyPilot:IsNil() then 	
				local kMyUnit = kMyPilot:GetUnit()
				if not kMyUnit:IsNil() then 
					local kCallerPilot = g_pilotMan:FindPilot(kMyUnit:GetCaller())
					if not kCallerPilot:IsNil() then 
						local kCallerActor = kCallerPilot:GetActor()
						if not kCallerActor:IsNil() then
							SkillHelpFunc_RemoveDarkBG(kCallerActor)
						end	
					end
				end
			end
		end
		
	end	
		
	return	true;
end