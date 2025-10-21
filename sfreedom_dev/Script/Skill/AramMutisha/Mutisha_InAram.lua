-- skill

function Act_Mutisha_InAram_OnCheckCanEnter(actor, action)
	--if actor:GetAbil(AT_ATTACK_RANGE_C)
end

function Act_Mutisha_InAram_SetState(actor,action,kState)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iNewState = -1;
	if kState == "BATTLEIDLE_START" then
		if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then
			return	false	
		end
		action:SetSlot(0)
		iNewState = 0;
	elseif kState == "BATTLEIDLE_LOOP" then
		action:SetSlot(1)
		if actor:GetAnimationLength(action:GetSlotAnimName(1)) == 0 then
			--ODS("Act_Mutisha_InAram_SetState SlotName["..action:GetSlotAnimName(1).."] Not Exist -> SetSlot To Next\n");
			action:SetSlot(3);	--	배틀 Idle 모션이 없을 때는 그냥 Idle 모션을 하자.
		end
		iNewState = 1;
		
	elseif kState == "FIRE" then
	
		action:SetSlot(2)
		iNewState = 2;
	elseif kState == "RETURN" then
		
		action:SetSlot(4)
		iNewState = 3;

	end
	
	actor:ResetAnimation();
	actor:PlayCurrentSlot();
	action:SetParamInt(0,iNewState)
	
	return	true;

end

function Act_Mutisha_InAram_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--ODS("Act_Mutisha_InAram_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n");
	
	local	prevAction = actor:GetAction()
	if prevAction:GetID() ~= "a_jump" then
		actor:Stop()
	end
	
	actor:ResetAnimation()
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mutisha_InAram_OnCastingCompleted(actor,action)
	else
	
		if( Act_Mutisha_InAram_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mutisha_InAram_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
		
	    ODS("Act_Mutisha_InAram_OnEnter Start Casting\n");
		
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
function Act_Mutisha_InAram_OnCastingCompleted(actor,action)	

	Act_Mutisha_InAram_SetState(actor,action,"FIRE")
	
	Act_Mutisha_InAram_Fire(actor,action);
	
end

function Act_Mutisha_InAram_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--GetActionResultCS(action, actor)

	-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
	local	kTargetList = action:GetTargetList();
	local	iTargetCount = kTargetList:size();
	
	ODS("Act_Mutisha_InAram_Fire iTargetCount:"..iTargetCount.."\n");
	
	
	if iTargetCount>0 then
		local	kTargetInfo = kTargetList:GetTargetInfo(0);
		if kTargetInfo:IsNil() == false then
		
			local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID());
			if kTargetPilot:IsNil() == false then
			
				local	kTargetActor = kTargetPilot:GetActor();
				if kTargetActor:IsNil() == false then
				
					ODS("Act_Mutisha_InAram_Fire LookAtBidirection Attacker:"..actor:GetID().." Target:"..kTargetActor:GetID().."\n");
					--actor:LookAtBidirection(kTargetActor:GetPos());
					actor:LookAt(kTargetActor:GetPos(),true,true);
				end
			end
		end
	end
	
end

function Act_Mutisha_InAram_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then
	
		if actor:IsAnimationDone() == true then
			return false
		end
		
	elseif action:GetActionParam() == AP_CASTING then
		
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mutisha_InAram_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
	
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mutisha_InAram_OnCastingCompleted(actor,action)
			
			end
		end
	
	end
	return true
end
function Act_Mutisha_InAram_OnCleanUp(actor, action)
end

function Act_Mutisha_InAram_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kCurAction = actor:GetAction();
	local	actionID = action:GetID()
	ODS("Act_Mutisha_InAram_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n");
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

function Act_Mutisha_InAram_LoadToWeapon(kActor, kAction)
	--ODS( GetAccumTime() .. " MagicCannon_Shot_LoadToWeapon..... \n", false, 998 )
	
	local	kProjectileMan = GetProjectileMan()
	local	kProjectileID = kActor:GetAnimationInfo("PROJECTILE_ID")
	local	fProjetileScale = tonumber(kActor:GetAnimationInfo("PROJECTILE_SCALE"))
	if nil  == fProjetileScale then
		fProjetileScale = 1.0
	end
	if nil == kProjectileID then
		return nil
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
	--local kPos = kActor:GetPos()
	--kNewArrow:LoadToHelper(kActor,kTargetNodeID)
	local kPos = Point3(kAction:GetParamFloat(10), kAction:GetParamFloat(11), kAction:GetParamFloat(12))
	kPos:SetZ(kPos:GetZ() + 100)
	kNewArrow:LoadToPosition(kPos)
	--ODS(" x:"..kPos:GetX().." y:"..kPos:GetY().." z:"..kPos:GetZ().." ID:"..kAction:GetID().."\n",false,987)
	return kNewArrow
end

function Act_Mutisha_InAram_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
       ODS("Act_Mutisha_InAram_OnEvent actor:"..actor:GetID().." textKey:"..textKey.."\n");
	   
	   
	if textKey == "spell" then
        local   action = actor:GetAction();
		
		CheckNil(nil==action);
		CheckNil(action:IsNil());
	
		local QuakeTime = actor:GetAnimationInfo("QUAKE_TIME")
		if nil ~= QuakeTime and "" ~= QuakeTime then
			local QuakeFactor = actor:GetAnimationInfo("QUAKE_FACTOR")	
			if  nil == QuakeFactor then
				QuakeFactor = 1.0
			else
				QuakeFactor = tonumber(QuakeFactor)
			end
			QuakeCamera(QuakeTime, QuakeFactor)
		end

		local EffectNode = actor:GetAnimationInfo("FIRE_EFFECT_NODE")
		local Effect = actor:GetAnimationInfo("FIRE_EFFECT")
		local EffectScale = actor:GetAnimationInfo("FIRE_EFFECT_SCALE")
		if nil==EffectScale or ""==EffectScale then
			EffectScale = 1
		else
			EffectScale = tonumber(EffectScale)
		end

		if nil~=EffectNode and nil~=Effect then
			actor:AttachParticleS(126, EffectNode, Effect, EffectScale)

		end
	end
       
	if textKey == "attk" then
		local action = actor:GetAction();
		local kPos = actor:GetPos()
		--ODS(" x:"..kPos:GetX().." y:"..kPos:GetY().." z:"..kPos:GetZ().." ID:"..action:GetID().."\n",false,987)
		action:SetParamFloat(10, kPos:GetX())
		action:SetParamFloat(11, kPos:GetY())
		action:SetParamFloat(12, kPos:GetZ())
	end
	
	if textKey == "hit" or textKey == "fire" then
		local kAction = actor:GetAction()
		--ODS("aa:"..kAction:GetParamFloat(10).."\n",false,987)		
		local	kProjectileMan = GetProjectileMan()
		local	kArrow = Act_Mutisha_InAram_LoadToWeapon(actor,kAction)
		if kArrow ~= nil then
			if not kArrow:IsNil() then
				local	iTargetCount = kAction:GetTargetCount()
				local	iTargetABVShapeIndex = 0
				local	kActionTargetInfo = nil
				
				local kTargetActor = g_pilotMan:FindActorByClassNo(6000930)
				if kTargetActor ~= nil and kTargetActor:IsNil() == false then
					kArrow:SetTargetLoc( kTargetActor:GetNodeTranslate("p_ef_head") )
				end
				
				-- if iTargetCount>0 then
					-- local kTargetGUID = kAction:GetTargetGUID(0)
					-- iTargetABVShapeIndex = kAction:GetTargetABVShapeIndex(0)
					-- kActionTargetInfo = kAction:GetTargetInfo(0)
					-- local kTargetobject = g_pilotMan:FindPilot(kTargetGUID)
					-- if kTargetobject:IsNil() == false then
						-- local kTargetActor = kTargetobject:GetActor()
						-- if not kTargetActor:IsNil() then
							-- local kTargetLoc = kTargetActor:GetABVShapeWorldPos(iTargetABVShapeIndex)
							-- --kActor:LookAt(kTargetLoc,true,false)
							-- --kTargetLoc:Add( Point3(0,-100,150) )
							-- --kArrow:SetTargetLoc(kTargetLoc)
							-- --kArrow:SetTargetLoc( Point3(-9,125,225) )
							-- kArrow:SetTargetLoc( kTargetActor:GetNodeTranslate("p_ef_head") )
							-- --kArrow:SetTargetObject( kActionTargetInfo )
							
							-- actor:ReleaseAllParticles()
							-- kArrow:SetParamValue("exValue", tostring(-150));
						-- end
					-- end
				-- end
				
				kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp())
				kArrow:Fire()
			end
		end
		kAction:ClearTargetList()
	end
	

	
	return true;
end

function Act_Mutisha_InAram_OnTargetListModified(actor,action,bIsBefore)
end
