-- skill

function Act_Aram_Backward_OnCheckCanEnter(actor, action)
	--if actor:GetAbil(AT_ATTACK_RANGE_C)
end

function Act_Aram_Backward_SetState(actor,action,kState)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iNewState = -1;
	if kState == "FIRE" then
		action:SetSlot(1)
		iNewState = 1;
	elseif kState == "RETURN1" then
		action:SetSlot(2)
		iNewState = 2;
	elseif kState == "RETURN2" then
		action:SetSlot(3)
		iNewState = 3;
	else
		return false
	end
	ODS("Backward State:"..iNewState.."\n",false,987)
	actor:ResetAnimation();
	actor:PlayCurrentSlot();
	action:SetParamInt(0,iNewState)
	
	return	true;

end

function Act_Aram_Backward_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetSlot(0)
	actor:ResetAnimation();
	actor:PlayCurrentSlot();
	action:SetParamInt(0,0)

	actor:DetachFrom(985)
	actor:AttachParticle(986, "p_ef_Head", "ef_Aram_end_01_p_ef_head")
	
	if action:GetActionParam() == AP_FIRE then
		Act_Aram_Backward_OnCastingCompleted(actor,action)
	end

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:RecoverCameraAdjustInfo()

	return true
end
function Act_Aram_Backward_OnCastingCompleted(actor,action)	

	Act_Aram_Backward_SetState(actor,action,"FIRE")
	
	--Act_Aram_Backward_Fire(actor,action);
	
end

function Act_Aram_Backward_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--GetActionResultCS(action, actor)

	-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
	local	kTargetList = action:GetTargetList();
	local	iTargetCount = kTargetList:size();
	
	ODS("Act_Aram_Backward_Fire iTargetCount:"..iTargetCount.."\n");
	
	
	if iTargetCount>0 then
		local	kTargetInfo = kTargetList:GetTargetInfo(0);
		if kTargetInfo:IsNil() == false then
		
			local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID());
			if kTargetPilot:IsNil() == false then
			
				local	kTargetActor = kTargetPilot:GetActor();
				if kTargetActor:IsNil() == false then
				
					ODS("Act_Aram_Backward_Fire LookAtBidirection Attacker:"..actor:GetID().." Target:"..kTargetActor:GetID().."\n");
					--actor:LookAtBidirection(kTargetActor:GetPos());
					--actor:LookAt(kTargetActor:GetPos(),true,true);
				end
			end
		end
	end
	
end

function Act_Aram_Backward_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then
		if actor:IsAnimationDone() == true then
			if iState == 3 then
				return true
			end
			if iState >= 1 then
				local bRet = Act_Aram_Backward_SetState(actor,action,"RETURN"..iState)
				return bRet
			end
			return true
		end
	end
	
	return true
end
function Act_Aram_Backward_OnCleanUp(actor, action)
end

function Act_Aram_Backward_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local	kCurAction = actor:GetAction();
	local	actionID = action:GetID()
	
	actor:DetachFrom(986)
	
	--ODS("Act_Aram_Backward_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n");
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

function Act_Aram_Backward_LoadToWeapon(kActor, kAction)
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
	kNewArrow:LoadToHelper(kActor,kTargetNodeID)

	return kNewArrow
end

function Act_Aram_Backward_OnEvent(actor,textKey)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	ODS("Act_Aram_Backward_OnEvent actor:"..actor:GetID().." textKey:"..textKey.."\n");
		
	-- if textKey == "hit" or textKey == "fire" or textKey == "walk02" then
		-- local kAction = actor:GetAction()
		-- local	kProjectileMan = GetProjectileMan()
		-- local	kArrow = Act_Aram_Backward_LoadToWeapon(actor,kAction)
		-- if kArrow ~= nil then
			-- if not kArrow:IsNil() then
				
				-- local	iTargetCount = kAction:GetTargetCount()
				-- local	iTargetABVShapeIndex = 0
				-- local	kActionTargetInfo = nil
				
				-- local kTargetActor = g_pilotMan:FindActorByClassNo(6000940)
				-- if kTargetActor ~= nil and kTargetActor:IsNil() == false then
					-- kArrow:SetTargetLoc( kTargetActor:GetNodeTranslate("p_ef_head") )
				-- end
				-- -- if iTargetCount>0 then
					-- -- local kTargetGUID = kAction:GetTargetGUID(0)
					-- -- iTargetABVShapeIndex = kAction:GetTargetABVShapeIndex(0)
					-- -- kActionTargetInfo = kAction:GetTargetInfo(0)
					-- -- local kTargetobject = g_pilotMan:FindPilot(kTargetGUID)
					-- -- if kTargetobject:IsNil() == false then
						-- -- local kTargetActor = kTargetobject:GetActor()
						-- -- if not kTargetActor:IsNil() then
							-- -- local kTargetLoc = kTargetActor:GetABVShapeWorldPos(iTargetABVShapeIndex)
							-- -- --kActor:LookAt(kTargetLoc,true,false)
							-- -- --kTargetLoc:Add( Point3(0,-100,150) )
							-- -- --kArrow:SetTargetLoc(kTargetLoc)
							-- -- --kArrow:SetTargetLoc( Point3(-9,125,225) )
							-- -- kArrow:SetTargetLoc( kTargetActor:GetNodeTranslate("p_ef_head") )
							-- -- --kArrow:SetTargetObject( kActionTargetInfo )
							
							-- -- actor:ReleaseAllParticles()
							-- -- kArrow:SetParamValue("exValue", tostring(-150));
						-- -- end
					-- -- end
				-- -- end
				
				-- kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp())
				-- kArrow:Fire()
			-- end
		-- end
		-- kAction:ClearTargetList()
	-- end
	

	
	return true;
end

function Act_Aram_Backward_OnTargetListModified(actor,action,bIsBefore)
end
