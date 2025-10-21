-- Idle

function Act_Idle_OnEnter(actor, action)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local actorID = actor:GetID()

	--	만약 stun 상태라면, stun 액션으로 전이시킨다.
	if actor:IsStun() then
		actor:ReserveTransitAction("a_stun");
		return	false;
	end

	if actor:IsMeetFloor() == true and actor:IsMyActor() == true then
		local strClassPrefix = string.sub(actorID, 1, 1)
		if strClassPrefix == "c" or
			strClassPrefix == "p" then
			local dir = actor:GetDirection()
			--ODS("Idle's Direction (Idle OnEnter) : " .. dir .. "\n")
			if dir ~= DIR_NONE then
				actor:ReserveTransitAction("a_run", dir)
				return false
			end
		end
	end
	
	--ODS("___________________Idle's MeetFloor false!!!\n")
	
	
	if action ~= nil and 
		action:IsNil() == false and
		action:GetID() == "a_battle_idle" and
		string.sub(actorID, 1, 1) == "c" then
		action:SetSlot(1)
	end
	
	

	actor:Stop()
	local class = string.sub(actor:GetID(), 1, 1)
	
	if class == 'm' then
		
		if actor:HasTarget() then
		
			action:SetSlot(2);
			
		end
	
	end
	
	if class == "c" then
		action:SetParamFloat(2, "15")
	elseif class == "m" then
		action:SetParamFloat(2, "4")
	elseif class == "p" then
		action:SetParamFloat(2, "8")
	end

	action:SetParamInt(4,math.random(4,8));
	action:SetParamInt(5,0);
	action:SetParamInt(6,0)
	action:SetParamFloat(13, -1)

	action:SetParamFloat(14,GetAccumTime());
	action:SetParamFloat(15,math.random(1000,3000)/1000.0);
	
	return true
end

function Act_Idle_OnCastingCompleted(actor,action)
end

function Act_Idle_ChangeEquipItems(actor,action)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	local iBaseClassID = actor:GetPilot():GetBaseClassID();
	
	if iBaseClassID == CT_FIGHTER then
	
		actor:ReserveTransitAction("a_Hammer Crush");
	
	elseif iBaseClassID == CT_ARCHER then
	
		actor:ReserveTransitAction("a_Daepodong");
	
	elseif iBaseClassID == CT_MAGICIAN then
	
		actor:ReserveTransitAction("a_Fortress");
	
	elseif iBaseClassID == CT_THIEF then
	
		actor:ReserveTransitAction("a_Rocket Punch");
	end
	
end


function Act_Idle_OnUpdate(actor, accumTime, frameTime)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	local action = actor:GetAction()
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	local currentSlot = action:GetCurrentSlot()
	local param = action:GetParam(0)
	local iIdleType = action:GetParamInt(6);
	
	if action:GetParamFloat(13) == -1 then
		action:SetParamFloat(13,accumTime)
	end
	
	local class = string.sub(actor:GetID(), 1, 1)
	
	local iBaseClassID = actor:GetPilot():GetBaseClassID();
	
	if actor:IsMyActor() == false and
		(iBaseClassID>=1 and iBaseClassID<=4) and 
		action:GetParamFloat(15) ~=0 and 
		(GetAccumTime() - action:GetParamFloat(14)) > action:GetParamFloat(15) then
	
		action:SetParamFloat(15,0);
		Act_Idle_ChangeEquipItems(actor,action);
	
	end

	if actor:IsMyActor() == false then
		local dir = actor:GetDirection()
		if dir ~= DIR_NONE then
			actor:ReserveTransitAction("a_run", dir)
			return true
		end
	end
	
	Act_Idle_DoAutoFire(actor);


	if actor:IsMeetFloor() == false and actor:GetAbil(AT_MONSTER_TYPE) ~= 1 then
		ODS("actor meetfloor false "..actor:GetAbil(AT_MONSTER_TYPE).."\n")
		if action:GetParam(119) == "jump_trap" or
			actor:GetVelocity():GetZ() < 0 then			
			action:SetNextActionName("a_jump")
			action:SetParam(3, "fall_down")
			return false
		end
	end

	if actor:IsAnimationDone() == true then
		actor:ResetAnimation();
	
		if iIdleType == 0 then
		
			local iBaseIdleLoopNum = action:GetParamInt(4);
			local iBaseIdleLoopCurNum = action:GetParamInt(5);
			
			iBaseIdleLoopCurNum=iBaseIdleLoopCurNum+1;
			
			if iBaseIdleLoopCurNum>= iBaseIdleLoopNum then
				action:SetParamInt(6,1)
				actor:PlayCurrentSlot()
			else
				action:SetParamInt(5,iBaseIdleLoopCurNum);
				actor:PlayCurrentSlot(true);	
			end
		
		
			
		elseif iIdleType == 1 then
			
			action:SetParamInt(6,0)
			action:SetParamInt(4,math.random(4,8));
			action:SetParamInt(5,0);
			
			local iBaseIdleLoopNum = action:GetParamInt(4);
			
			actor:PlayCurrentSlot(true);			
		end
	

		return true
	end

	if currentSlot == 1 then
		if accumTime - action:GetParamFloat(13) > 3.0 then
			action:SetNextActionName("a_idle")
			return false
		end
	elseif actor:GetPilot():GetAbil(AT_IDLEACTION_TYPE) ~= 101 and 
		accumTime - action:GetParamFloat(13) > 3.0 then
		
		action:SetParamFloat(13,accumTime)
		
		--actor:SeeFront(true)
	end

	return true
end
function Act_Idle_OnCleanUp(actor, action)
end

function Act_Idle_OnLeave(actor, action)
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local kCurAction = actor:GetAction();
	if( CheckNil(nil==kCurAction) ) then return true end
	if( CheckNil(kCurAction:IsNil()) ) then return true end
	
	if action:GetID() == "a_jump" and 
		kCurAction:GetAction():GetParam(3) == "fall_down" then
		-- 절벽에서 떨어질 때는, ActionPacket을 보내지 않는다.
		action:SetSlot(2)
		action:SetDoNotBroadCast(true)
	end
	ODS(kCurAction:GetID() .. " Idle's NextAction : " .. action:GetID() .. "\n")
	return true
end

