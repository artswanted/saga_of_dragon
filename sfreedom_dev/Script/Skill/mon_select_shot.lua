function Act_Mon_Select_Shot_OnCheckCanEnter(actor, action)
end

function Act_Mon_Select_Shot_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	kSkillDef = GetSkillDef(action:GetActionNo())
	local kClassNo = kSkillDef:GetAbil(3322)
	if kClassNo ~= 0 then
		local	kProjectileMan = GetProjectileMan()
		local	kArrow = Act_Mon_Select_Shot_LoadToWeapon(actor,action)
		if kArrow ~= nil then
			if not kArrow:IsNil() then
				local kTargetActor = g_pilotMan:FindActorByClassNo(kClassNo)
				if kTargetActor ~= nil and kTargetActor:IsNil() == false then
					kArrow:SetTargetLoc( kTargetActor:GetNodeTranslate("p_ef_head") )
					kArrow:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp())
					kArrow:Fire()
				end
			end
		end
		action:ClearTargetList()
	end
	
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end
	
	actor:ResetAnimation()
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_Select_Shot_OnCastingCompleted(actor,action)
	else
	
		if( Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
		
	    ODS("Act_Mon_Select_Shot_OnEnter Start Casting\n");
		
		-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
		local	kTargetList = action:GetTargetList();
		local	iTargetCount = kTargetList:size();
	
		ODS("Act_Mon_Melee Casting Start iTargetCount :"..iTargetCount.."\n");

		
		if iTargetCount>0 then
			local	kTargetInfo = kTargetList:GetTargetInfo(0);
			if kTargetInfo:IsNil() == false then
			
				local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID());
				if kTargetPilot:IsNil() == false then
				
					local	kTargetActor = kTargetPilot:GetActor();
					if kTargetActor:IsNil() == false then
					
						--actor:LookAtBidirection(kTargetActor:GetPos());
						actor:LookAt(kTargetActor:GetPos(),true,true);
					
					end
				end
			end
		end
	end
	

	return true
end
function Act_Mon_Select_Shot_OnCastingCompleted(actor,action)	

	Act_Mon_Melee_SetState(actor,action,"FIRE")
	
	Act_Mon_Select_Shot_Fire(actor,action);
	
end

function Act_Mon_Select_Shot_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	--GetActionResultCS(action, actor)

	-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
	local	kTargetList = action:GetTargetList();
	local	iTargetCount = kTargetList:size();
	
	ODS("Act_Mon_Select_Shot_Fire iTargetCount:"..iTargetCount.."\n");
	
	
	if iTargetCount>0 then
		local	kTargetInfo = kTargetList:GetTargetInfo(0);
		if kTargetInfo:IsNil() == false then
		
			local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID());
			if kTargetPilot:IsNil() == false then
			
				local	kTargetActor = kTargetPilot:GetActor();
				if kTargetActor:IsNil() == false then
				
					ODS("Act_Mon_Select_Shot_Fire LookAtBidirection Attacker:"..actor:GetID().." Target:"..kTargetActor:GetID().."\n");
					--actor:LookAtBidirection(kTargetActor:GetPos());
					actor:LookAt(kTargetActor:GetPos(),true,true);
				end
			end
		end
	end
	
end

function Act_Mon_Select_Shot_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then
	
		if actor:IsAnimationDone() == true then
			return false
		end
		
	elseif action:GetActionParam() == AP_CASTING then
		
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
	
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_Select_Shot_OnCastingCompleted(actor,action)
			
			end
		end
	
	end
	return true
end
function Act_Mon_Select_Shot_OnCleanUp(actor, action)
end

function Act_Mon_Select_Shot_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	kCurAction = actor:GetAction();
	local	actionID = action:GetID()
	ODS("Act_Mon_Select_Shot_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n");
	if action:GetActionType()=="EFFECT" or
		actionID == "a_freeze" or
		actionID == "a_reverse_gravity" or
		actionID == "a_blow_up" or
		actionID == "a_blow_up_small" or
		actionID == "a_knock_back" or
		actionID == "a_die" or
		actionID == "a_idle" or
		actionID == "a_run_right" or
		actionID == "a_run_left" or
		actionID == "a_handclaps" then
		return true
	end
	return false 
end

function Act_Mon_Select_Shot_LoadToWeapon(kActor, kAction)
	--ODS( GetAccumTime() .. " MagicCannon_Shot_LoadToWeapon..... \n", false, 998 )
	if( CheckNil(nil==kActor) ) then return nil end
	if( CheckNil(kActor:IsNil()) ) then return nil end
	
	if( CheckNil(nil==kAction) ) then return nil end
	if( CheckNil(kAction:IsNil()) ) then return nil end
	
	local	kProjectileMan = GetProjectileMan()
	local	kProjectileID = kActor:GetAnimationInfo("PROJECTILE_ID")
	if nil == kProjectileID then
		return nil
	end
	
	local	fProjetileScale = tonumber(kActor:GetAnimationInfo("PROJECTILE_SCALE"))
	if nil  == fProjetileScale then
		fProjetileScale = 1.0
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID, kAction, kActor:GetPilotGuid())
	if kNewArrow:IsNil() then
		return nil
	end
	
	local	kTargetNodeID = kActor:GetAnimationInfo("FIRE_START_NODE")
	if nil == kTargetNodeID then
		kTargetNodeID = "p_ef_heart"
	end
	
	local	fProjectileSpeed = kActor:GetAnimationInfo("PROJECTILE_SPEED")
	if nil ~= fProjectileSpeed then
		fProjectileSpeed = tonumber(fProjectileSpeed)
		kNewArrow:SetSpeed(fProjectileSpeed)
	end
	
	kNewArrow:SetScale(fProjetileScale*kNewArrow:GetScale())
	kNewArrow:LoadToHelper(kActor,kTargetNodeID)
	
	-- local kPos = Point3(kAction:GetParamFloat(10), kAction:GetParamFloat(11), kAction:GetParamFloat(12))
	-- kPos:SetZ(kPos:GetZ() + 100)
	-- kNewArrow:LoadToPosition(kPos)
	--ODS(" x:"..kPos:GetX().." y:"..kPos:GetY().." z:"..kPos:GetZ().." ID:"..kAction:GetID().."\n",false,987)
	return kNewArrow
end

function Act_Mon_Select_Shot_OnEvent(actor,textKey)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	return true;
end

function Act_Mon_Select_Shot_OnTargetListModified(actor,action,bIsBefore)
end
