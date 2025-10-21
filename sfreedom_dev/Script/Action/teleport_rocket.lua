
function Act_Teleport_Rocket_OnEnter(actor, action)
	
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	
	CheckNil(action == nil);
	CheckNil(action:IsNil());

	local	packet = action:GetParamAsPacket()
	if packet ~= nil and packet:IsNil() == false then
		local	ptTargetPos = packet:PopPoint3();
		local	iType = packet:PopInt()
		action:SetParamAsPoint(1,ptTargetPos);
		action:SetParamInt(100,iType)
		--ODS("Act_Teleport_Rocket_OnEnter actor:"..actor:GetPilot():GetName():GetStr().." kDestTrn : "..ptTargetPos:GetX()..","..ptTargetPos:GetY()..","..ptTargetPos:GetZ().."\n");
	end
		
	local eventType = action:GetParamInt(100)
	--ODS(actor:GetID().." /eventtype:"..eventType.."\n", false, 912)
	if eventType ~= 0 then
		if actor:IsRidingPet() then
			local kActorPet = actor:GetMountTargetPet()
			if kActorPet:IsNil() then
				return false
			end
			kActorPet:AttachParticle(289, "char_root", "e_portal")
		else
			actor:AttachParticle(289, "char_root", "e_portal")
		end
	end
	--ODS("in eventType:"..eventType.."\n" ,false, 912)
	if eventType == 1 or eventType == 3 or eventType == 4 or 6 == eventType or 7 == eventType then 
		if action:GetParam(1423) ~= "ladder" then
			if eventType == 4 then
--				action:SetSlot(3)
			elseif 6 == eventType or 7 == eventType then			
				actor:LockBidirection(false)
				actor:SeeFront(true, true)
	--			action:SetSlot(4)
			else
				if actor:IsRidingPet() then
					local kActorPet = actor:GetMountTargetPet()
					if kActorPet:IsNil() then
						return false
					end
					local kActionPet = kActorPet:GetAction()
					if kActionPet:IsNil() then
						return false
					end
					kActorPet:PlayCurrentSlot()
				end				
			end

		end
		--ODS("Net_PT_C_M_REQ_START_HYPER_MOVE\n", false, 912)
		if(actor:IsMyActor() ) then
			Net_PT_C_M_REQ_START_HYPER_MOVE(action:GetParam(914), eventType)
		end
	end
	

	 if( actor:IsMyActor() and ( 6 == eventType or 7 == eventType ) and action:GetParamInt(20110902) == 0)then	
		 if( DoPartyMemberTeleMoveIn(eventType) ) then
			 action:SetParamInt(20110902, 1)
		 end
	 end
	
	if actor:IsMyActor() and (false==action:GetDoNotBroadCast())	then
		action:BroadCast(actor:GetPilot())
		LockPlayerInput(21)
	end
	
	actor:SetCanHit(false);
	return true
end

function Act_Teleport_Rocket_OnOverridePacket(actor,action,packet)
	CheckNil(action == nil);
	CheckNil(action:IsNil());
	
	packet:PushPoint3(action:GetParamAsPoint(1));
	packet:PushInt(action:GetParamInt(100))

end
function Act_Teleport_Rocket_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local action = actor:GetAction();
	if action == nil then
		return false
	end
	
	local eventType = action:GetParamInt(100)
	
	if eventType == 0 then
		
		if actor:IsAnimationDone() == true and action:GetParamFloat(3) == 0.0 then
			action:SetParamFloat(3, accumTime)
			if actor:IsMyActor() then
				g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 0.0, 1.0, 1.0, true, false )
			end
		end
		if actor:IsAnimationDone() == true and accumTime - action:GetParamFloat(3) > 1.0 then
			action:SetParam(0,"end")
			return false
		end
		
		if actor:IsRidingPet() then
			action:SetParam(0,"end") --펫 탑승 중이면 액션 동작을 하지 않고 그냥 종료시키자
			return false
		end
	elseif eventType == 1 or eventType == 3 or eventType == 4 then
		local kDir = action:GetParamInt(915)
		if kDir <= 0 then
			kDir = DIR_UP
		end
		
		if action:GetParamFloat(3) == 0.0 then
			action:SetParamFloat(3, accumTime)
			if actor:IsMyActor() then
				g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 0.0, 1.0, 1.0, true, false )
			end
			
			if action:GetParam(1423) ~= "ladder" then
				actor:LockBidirection(false)
				actor:Stop()
				actor:Concil(true)
				if eventType ~= 4 then
					if kDir == DIR_UP then
						actor:SeeFront(false, true)
					elseif kDir == DIR_DOWN then
						actor:SeeFront(true, true)
					elseif kDir == DIR_LEFT then
						actor:ToLeft(true, true)
					elseif kDir == DIR_RIGHT then
						actor:ToLeft(false, true)
					end
				else
					actor:SeeFront(true, true)
				end
				actor:Concil(false)
			else
				actor:FreeMove(true)
			end
		end
		
		if action:GetParam(1423) ~= "ladder" and eventType ~= 4 then
			actor:Walk(kDir, 50) -- walk in direction
		end
		
		if accumTime - action:GetParamFloat(3) > 2.0 then
			action:SetParam(0,"end")
			return false
		end
	elseif eventType == 5 then
		if actor:IsAnimationDone() == true and action:GetParamFloat(3) == 0.0 then
			action:SetParamFloat(3, accumTime)
		end
	
		if actor:IsAnimationDone() == true and accumTime - action:GetParamFloat(3) > 1.0 then
			action:SetParam(0,"end")
			return false
		end
		
		if actor:IsRidingPet() then
			action:SetParam(0,"end") --펫 탑승 중이면 액션 동작을 하지 않고 그냥 종료시키자
			return false
		end
	elseif(6 == eventType or 7 == eventType) then
		actor:LockBidirection(false)
		actor:Stop()
		actor:SeeFront(true, true)
		actor:Concil(true)
		if actor:IsAnimationDone() == true and accumTime - action:GetParamFloat(3) > 1.0 then
			action:SetParam(0,"end")
			return false
		end
	else
		if actor:IsRidingPet() then
			action:SetParam(0,"end") --펫 탑승 중이면 액션 동작을 하지 않고 그냥 종료시키자
			return false
		end
		
		actor:Stop()
		action:SetParam(0,"end")
		if actor:IsMyActor() and (false==action:GetDoNotBroadCast()) then	-- damage 요청, trap코드랑 공유하고 있다.				
			NET_C_M_REQ_TRIGGER(9, actor:GetPilotGuid(), 0);
		end
		return false
	end
	
	return true 
end
function Act_Teleport_Rocket_OnCleanUp(actor, action)
	
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());

	CheckNil(action == nil);
	CheckNil(action:IsNil());
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	
	actor:SetCanHit(true);

	local curAction = actor:GetAction()
	--ODS("Teleport_OnCleanUp="..actor:GetID().." /eventtype:"..curAction:GetParamInt(100).."/netxt:"..action:GetID().."\n\n", false, 912)
	if action:GetID() ~= "a_teleport_rocket_out" then
		actor:FreeMove(false)
		actor:RestoreLockBidirection()
		actor:Concil(true)
		actor:SeeFront(true, false)
		g_world:ResetWorldFocusFilter()
	else
		if not actor:IsMyActor() then
			action:SetParamInt( curAction:GetParamInt(100) )
		end
	end
	actor:RestoreLockBidirection();
	
	if actor:IsMyActor() then
		UnLockPlayerInput(21)
	end
	return true
end
function Act_Teleport_Rocket_OnLeave(actor, action)
	
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	
	CheckNil(action == nil);
	CheckNil(action:IsNil());

	if action:GetID() ~= "a_teleport_rocket_out" then
		return false 
	end
	local curAction = actor:GetAction()

	if action:GetID() == "a_teleport_rocket_out" then
		action:SetParamAsPoint(1, curAction:GetParamAsPoint(1))
		action:SetParamInt(100, curAction:GetParamInt(100))
		action:SetParam(911, curAction:GetParam(911))
		action:SetParamInt(916, curAction:GetParamInt(916))
		if 0 ~= curAction:GetParamInt(917) then
			actor:ReserveTransitAction("a_idle")
			return false
		end
	end
	
	if actor:GetAction():GetParam(0) == "end" then
		return	true
	end

	return false
end

-- Teleport Out
function Act_Teleport_Rocket_Out_OnEnter(actor, action)
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());

	CheckNil(action == nil);
	CheckNil(action:IsNil());
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	action:SetParamInt(2,1);
	
	local	packet = action:GetParamAsPacket()
	if packet ~= nil and packet:IsNil() == false then
	
		local	ptTargetPos = packet:PopPoint3();
		local	iType = packet:PopInt()
		action:SetParamAsPoint(1,ptTargetPos);
		action:SetParamInt(100,iType)
	
		--ODS("Act_Teleport_Rocket_Out_OnEnter actor:"..actor:GetPilot():GetName():GetStr().." kDestTrn : "..ptTargetPos:GetX()..","..ptTargetPos:GetY()..","..ptTargetPos:GetZ().."\n");
	
	end
		
	local kDestTrn = action:GetParamAsPoint(1)
	local eventType = action:GetParamInt(100)
	
	--ODS("Teleport_Out="..actor:GetID().." /eventtype:"..eventType.."\n", false, 912)
	
	--ODS("Act_Teleport_Rocket_Out_OnEnter[1] actor:"..actor:GetPilot():GetName():GetStr().." kDestTrn : "..kDestTrn:GetX()..","..kDestTrn:GetY()..","..kDestTrn:GetZ().."\n");
	
	
	if kDestTrn:IsZero() == false then
		local kRayStart = Point3(kDestTrn:GetX(), kDestTrn:GetY(), kDestTrn:GetZ() + 50)
		local kFloorPos = g_world:ThrowRay(kRayStart, Point3(0,0,-1), 200)

		if kFloorPos:GetX() == -1 and kFloorPos:GetY() == -1 and kFloorPos:GetZ() == -1 then
			kFloorPos = kRayStart
		end
		--ODS("Act_Teleport_Rocket_Out_OnEnter[1] actor to floor: "..kFloorPos:GetX()..","..kFloorPos:GetY()..","..kFloorPos:GetZ().."\n");
		kFloorPos:SetZ(kFloorPos:GetZ() + 25)
		actor:SetTranslate(kFloorPos)

		if eventType ~= 5 then
			g_world:RefreshCamera()
		else
			SetCameraSpeedW(0.05)
		end
		--ODS("out eventType:"..eventType.."\n" ,false, 912)
		if eventType == 1 or eventType == 3 or eventType == 4 or eventType == 6 or 7 == eventType then
			if actor:IsRidingPet() then
				local kActorPet = actor:GetMountTargetPet()
				if kActorPet:IsNil() then
					return false
				end
				local kActionPet = kActorPet:GetAction()
				if kActionPet:IsNil() then
					return false
				end
				kActorPet:PlayCurrentSlot()
			elseif 6 == eventType or 7 == eventType then
				actor:LockBidirection(false)
				actor:Stop()
				actor:SeeFront(true, true)
				actor:Concil(true)	
			end				
			local bRealyJump = false
			local kTargetPos = g_world:GetObjectWorldPosByName(action:GetParam(911))
			kFloorPos:SetZ(kFloorPos:GetZ() - 25)
			if nil~=kTargetPos then	--GetTranslate를 하지 않는건 같은 루프라서 업데이트가 안되있음
				kTargetPos:SetZ(0)	--Z를 0으로 하는 이유는 텔레포트에서 나오면서 계단같은 곳으로 걸어나올수 있기 때문임
				local z = kFloorPos:GetZ()-25
				kFloorPos:SetZ(0)
				bRealyJump = kTargetPos:Distance(kFloorPos) <= 10	--텔레포트는 텔레점프와 달리 오차가 거의 없어야 함
				kFloorPos:SetZ(z)
			end
			if(actor:IsMyActor() ) then
				Net_PT_C_M_NFY_END_HYPER_MOVE(kFloorPos, bRealyJump)
			end
		end
		
	end
	
	if eventType ~= 2 and 
		eventType ~= 5 and 
		actor:IsMyActor() then
		g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 1.0, 0.0, 1.0, false, false )
	end

	if actor:IsMyActor() and (false==action:GetDoNotBroadCast())	then
		action:BroadCast(actor:GetPilot())
		LockPlayerInput(21)
	end
	actor:SetCanHit(false);
	return true
end
function Act_Teleport_Rocket_Out_OnOverridePacket(actor,action,packet)
		
	CheckNil(action == nil);
	CheckNil(action:IsNil());

	packet:PushPoint3(action:GetParamAsPoint(1));
	packet:PushInt(action:GetParamInt(100))

end
function Act_Teleport_Rocket_Out_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(action == nil);	
	CheckNil(action:IsNil());
	
	local iStep = action:GetParamInt(2)
	local bIsAnimationDone = actor:IsAnimationDone()
	local eventType = action:GetParamInt(100)
	
	if( actor:IsMyActor() and ( 6 == eventType or 7 == eventType ) and action:GetParamInt(20110902) == 0)then	
		 if( DoPartyMemberTeleMoveOut(eventType) ) then
			 action:SetParamInt(20110902, 1)
		 end
	end	
	
--[[
	if iStep == 1 and bIsAnimationDone == true then
		action:SetParamInt(2,2);
		action:SetParamFloat(3, accumTime)	--	다운 시작 시간 기록
	elseif iStep == 2 then
		if accumTime - action:GetParamFloat(3) > 0.3 then
			action:SetParamInt(2,3);
			actor:PlayNext();
		end
	elseif iStep == 3 and 
]]	
	if eventType ~= 1 and bIsAnimationDone == true then
		action:SetParam(0,"end")
		return false
	end
	
	if eventType == 1 or eventType == 3 or eventType == 4 then
		local kDir = action:GetParamInt(916)
		if kDir <= 0 then
			kDir = DIR_DOWN
		end

		if action:GetParamFloat(3) == 0.0 then
			action:SetParamFloat(3, accumTime)
			actor:FreeMove(false)
			actor:LockBidirection(false)			
			actor:Stop()
			actor:Concil(true)
			if kDir == DIR_UP then
				actor:SeeFront(false, true)
			elseif kDir == DIR_DOWN then
				actor:SeeFront(true, true)
			elseif kDir == DIR_LEFT then
				actor:ToLeft(true, true)
			elseif kDir == DIR_RIGHT then
				actor:ToLeft(false, true)
			end
			actor:Concil(false)
		end
		
		actor:Walk(kDir, 50) -- walk out direction
		
		if accumTime - action:GetParamFloat(3) > 1.0 then
			action:SetParam(0,"end")
			return false
		end		
	elseif (6 == eventType or 7 == eventType) then
		actor:LockBidirection(false)
		--actor:Stop()
		actor:SeeFront(true, true)
		actor:Concil(true)		
	elseif actor:IsRidingPet() then
		action:SetParam(0,"end")
		return false
	end
	return true 
end
function Act_Teleport_Rocket_Out_OnCleanUp(actor, action)
	
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	
	actor:SetCanHit(true);

	actor:RestoreLockBidirection()
	actor:Concil(true)
	
	local eventType = action:GetParamInt(100)
	local kDir = action:GetParamInt(916)
	
	if actor:IsLockBidirection() then
	
		--	if lock bidirection is true, we make this actor to see left direction
		local	kPathNormal = actor:GetPathNormal();
		local	kNewDir = kPathNormal:Cross(Point3(0,0,-1))
		if (eventType == 1 or eventType == 3 or eventType == 4 or 6 == eventType or 7 == eventType) and kDir == DIR_LEFT then
			actor:SetMovingDir(kNewDir);
			actor:ToLeft(true,true);
		else
			kNewDir = kPathNormal:Cross(Point3(0,0,1))
			actor:SetMovingDir(kNewDir);
			actor:ToLeft(false,true);
		end
		
	else
	
		actor:SeeFront(true, false)

	end
	
	SetCameraSpeedW(100.0)
	
	if actor:IsMyActor() then
		UnLockPlayerInput(21)
	end
	return true
end
function Act_Teleport_Rocket_Out_OnLeave(actor, action)

	if action:GetID() == "a_run" or  action:GetID() == "a_jump" or  action:GetID() == "a_dash" then
		--ODS("실패:"..action:GetID().."\n", false, 912)
		return false 
	end
	
	local curAction = actor:GetAction()
	if( false == curAction:IsNil() ) then
--			ODS("Act_Teleport_Rocket_Out_OnLeave:"..curAction:GetParam(0).."\n", false, 912)
		if("end" ~= curAction:GetParam(0) ) then 
			return false
		end
	end
	
	return true
end
