function Act_Rest_Exp_OnCheckCanEnter(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	--if( CheckNil(action == nil) ) then return false end
	--if( CheckNil(action:IsNil()) ) then return false end
	
	if actor:IsMeetFloor() == false then
		return	false
	end
	
	return	true;
end
function Act_Rest_Exp_OnEnter(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if( CheckNil(nil == g_world) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local actionID = action:GetID()
	local iSlotCount = action:GetSlotCount()
	
	actor:HideParts(6, true)
	action:SetParam(6,"HideParts")
	
	if actionID == "a_sleep" then
		action:SetSlot(1)
	end
	
	action:SetThrowRayStatus(true)
	actor:FreeMove(true);	
	actor:StopJump();
	actor:SetNoWalkingTarget(false);
	actor:ResetAnimation();	
	local ptTargetPos = 0;	
	ptTargetPos = actor:GetPos()

	if ptTargetPos ~= nil or ptTargetPos:IsNil() == false then
		local kKind = action:GetScriptParam("SA_CLASSNO")
		local kEftNo = action:GetScriptParam("EFT_ID")
		if kEftNo ~= nil or kEftNo ~= "null" or kEftNo ~= "" then
			local kEftNumber = tonumber(kEftNo)
			action:SetParamInt(10, kEftNumber)
		end
		if kKind ~= nil or kKind ~= "null" or kKind ~= "" then
			local TargetPos = g_world:ThrowRay(Point3(ptTargetPos:GetX(),ptTargetPos:GetY(),ptTargetPos:GetZ()+50.0),Point3(0,0,-1),500)
			
			actor:LockBidirection(false)
			actor:SeeFront(true, true)

			local kPPos = actor:GetNodeWorldPos("char_root");

			ptTargetPos = Point3(0,0,25)
			ptTargetPos = ptTargetPos:_Add(kPPos);

			actor:SetTranslate(ptTargetPos)
			TargetPos:SetZ(TargetPos:GetZ() + 25)

			local ClassNo = tonumber( kKind )
			Act_Rest_Exp_CreateElemental(actor,action,TargetPos, ClassNo)
			
			actor:AttachSound(9991,"Use_RestChair");
		end
	end		
	actor:Stop()
	SubActorHide(actor, true)
	return true
end

function Act_Rest_Exp_OnUpdate(actor, accumTime, frameTime)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	local iCurrentSlot = action:GetCurrentSlot()
	local bIsAnimDone = actor:IsAnimationDone()

	if bIsAnimDone == true then
		if iCurrentSlot == 0 then
			local iRand = math.random(100)
			local iSlot = 0
			if iRand > 70 then
				iSlot = 1
			end
			
			action:SetSlot(iSlot)
			actor:PlayCurrentSlot()
			
			if( CheckNil(nil == g_world) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			
			local guid = actor:GetAction():GetGUIDInContainer(0);
			local elemental = g_world:FindActor(guid);
			if elemental:IsNil() == false then
				elAction = elemental:GetAction()
				if elAction:IsNil() == false then
					elAction:SetSlot(iSlot)
					elemental:PlayCurrentSlot()
				end
			end
		elseif iCurrentSlot == 1 then
			action:SetSlot(0)
			actor:PlayCurrentSlot()
			
			if( CheckNil(nil == g_world) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			
			local guid = actor:GetAction():GetGUIDInContainer(0);
			local elemental = g_world:FindActor(guid);
			if elemental:IsNil() == false then
				elAction = elemental:GetAction()
				if elAction:IsNil() == false then
					elAction:SetSlot(0)
					elemental:PlayCurrentSlot()
				end
			end
		end
	end

	return true
end

function Act_Rest_Exp_OnLeave(actor, action)	
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if action:GetEnable() == false then
		return	false
	end
	
	return true
end

function Act_Rest_Exp_OnCleanUp(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end	
	
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	local EffectNo = action:GetParamInt(10)
	
	if 0 ~= EffectNo and actor:IsMyActor() then
		local	kPacket = NewPacket(12130)
		kPacket:PushGuid(actor:GetPilotGuid())
		kPacket:PushInt(EffectNo)
		Net_Send(kPacket)
		DeletePacket(kPacket)
	end
	
	local paramValue = actor:GetAction():GetParam(6)
	local actionID = actor:GetAction():GetID()
	if paramValue == "HideParts" then				
		actor:HideParts(6, false)
		actor:GetAction():SetParam(6,"")
	end			
	Act_Rest_Exp_OnClearUpRun(actor)
	SubActorHide(actor, false)
end

function Act_Rest_Exp_OnClearUpRun(actor)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end	
	
	local kCurAction = actor:GetAction()
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end	
	
	if( CheckNil(nil == g_world) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	actor:DetachFrom(99100);
	actor:FreeMove(false);
	actor:SeeFront(false,true)
	actor:RestoreLockBidirection()		
	actor:GetAction():SetThrowRayStatus(false)	
	
	local guid = actor:GetAction():GetGUIDInContainer(0);
	local elemental = g_world:FindActor(guid);
	if elemental:IsNil() == false then
		g_world:RemoveActorOnNextUpdate(elemental:GetPilotGuid());
	end
end

function Act_Rest_Exp_CreateElemental(actor,action,kTargetPos, classno)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if( CheckNil(nil == g_world) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local guid = GUID("123");
	guid:Generate();
	
	local pilot = g_pilotMan:NewPilot(guid, classno, 0);
	if pilot:IsNil() == false then
		action:AddNewGUID(guid);
		local kNewActor = pilot:GetActor();
		g_world:AddActor(guid, kNewActor, kTargetPos, 9);
		kNewActor:FreeMove(true);
		kNewActor:SetTranslate(kTargetPos);
		kNewActor:ReserveTransitAction("a_rest_expSA01");
		kNewActor:ClearActionState()
		local	kPathNormal = actor:GetPathNormal();
		kPathNormal:Multiply(-1);
		kNewActor:LookAt(kNewActor:GetTranslate():_Add(kPathNormal), true)	
		kNewActor:PlayCurrentSlot();
	end
end
