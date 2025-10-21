-- skill

function Act_Mon_Teleport_OnCheckCanEnter(actor, action)
	--if actor:GetAbil(AT_ATTACK_RANGE_C)
end

function Act_Mon_Teleport_SetState(actor,action,kState)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
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
		
			ODS("Act_Mon_Teleport_SetState SlotName["..action:GetSlotAnimName(1).."] Not Exist -> SetSlot To Next\n");
		
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

function Act_Mon_Teleport_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Act_Mon_Teleport_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n");
	
	local kPacket = action:GetParamAsPacket()
	if kPacket ~= nil and false == kPacket:IsNil() then
		action:SetParamAsPoint(0, kPacket:PopPoint3() )
	end
	
	local	prevAction = actor:GetAction()
		if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end
	
	actor:ResetAnimation()
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_Teleport_OnCastingCompleted(actor,action)
	else
	
		if( Act_Mon_Teleport_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_Teleport_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
		
	    ODS("Act_Mon_Teleport_OnEnter Start Casting\n");
		
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
function Act_Mon_Teleport_OnCastingCompleted(actor,action)	

	Act_Mon_Teleport_SetState(actor,action,"FIRE")
	
	Act_Mon_Teleport_Fire(actor,action);
	
end

function Act_Mon_Teleport_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	
	--GetActionResultCS(action, actor)

	-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
	local	kTargetList = action:GetTargetList();
	local	iTargetCount = kTargetList:size();
	
	ODS("Act_Mon_Teleport_Fire iTargetCount:"..iTargetCount.."\n");
	
	
	if iTargetCount>0 then
		local	kTargetInfo = kTargetList:GetTargetInfo(0);
		if kTargetInfo:IsNil() == false then
		
			local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID());
			if kTargetPilot:IsNil() == false then
			
				local	kTargetActor = kTargetPilot:GetActor();
				if kTargetActor:IsNil() == false then
				
					ODS("Act_Mon_Teleport_Fire LookAtBidirection Attacker:"..actor:GetID().." Target:"..kTargetActor:GetID().."\n");
					--actor:LookAtBidirection(kTargetActor:GetPos());
					actor:LookAt(kTargetActor:GetPos(),true,true);
				end
			end
		end
	end
	
end

function Act_Mon_Teleport_OnUpdate(actor, accumTime, frameTime)

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
				Act_Mon_Teleport_SetState(actor,action,"BATTLEIDLE_LOOP")
				
				local eff = actor:GetAnimationInfo("CASTING_EFFECT") -- 캐스팅 이팩트 붙이기
				if nil~=eff then
					local TargetPos = action:GetParamAsPoint(0)
					if false==TargetPos:IsZero() then
						actor:AttachParticleToPointWithRotate(230, TargetPos, eff, actor:GetRotateQuaternion(), 1.0);
					else
						local node = actor:GetAnimationInfo("CASTING_EFFECT_NODE")
						if nil==node then
							node = "char_root"
						end
						actor:AttachParticle(230, node, eff)
					end
				end
			end
		end
	
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_Teleport_OnCastingCompleted(actor,action)
			
			end
		end
	
	end
	return true
end
function Act_Mon_Teleport_OnCleanUp(actor, action)
	actor:DetachFrom(230)	--캐스팅 이펙트 제거
end

function Act_Mon_Teleport_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
	local	kCurAction = actor:GetAction();
	local	actionID = action:GetID()
	ODS("Act_Mon_Teleport_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n");
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

function Act_Mon_Teleport_LoadToWeapon(kActor, kAction)
	
	if( CheckNil(nil==kActor) ) then return nil end
	if( CheckNil(kActor:IsNil()) ) then return nil end
	
	if( CheckNil(nil==kAction) ) then return nil end
	if( CheckNil(kAction:IsNil()) ) then return nil end
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
	kNewArrow:LoadToPosition(kPos)
	--ODS(" x:"..kPos:GetX().." y:"..kPos:GetY().." z:"..kPos:GetZ().." ID:"..kAction:GetID().."\n",false,987)
	return kNewArrow
end

function Act_Mon_Teleport_OnEvent(actor,textKey)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
    ODS("Act_Mon_Teleport_OnEvent actor:"..actor:GetID().." textKey:"..textKey.."\n");
	   
      
	if textKey == "hit" or textKey == "fire" then
		local action = actor:GetAction()
		Act_Mon_Melee_DoDamage(actor,action)
		-- ODS("aa:"..kAction:GetParamFloat(10).."\n",false,987)		
		-- local	kProjectileMan = GetProjectileMan()
		-- local	kArrow = Act_Mon_Teleport_LoadToWeapon(actor,kAction)
		-- if kArrow ~= nil then
			-- if not kArrow:IsNil() then
				-- local	iTargetCount = kAction:GetTargetCount()
				-- local	iTargetABVShapeIndex = 0
				-- local	kActionTargetInfo = nil
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
				
				-- kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp())
				-- kArrow:Fire()
			-- end
		-- end
		-- kAction:ClearTargetList()
	end
	

	
	return true;
end

function Act_Mon_Teleport_OnTargetListModified(actor,action,bIsBefore)
end
