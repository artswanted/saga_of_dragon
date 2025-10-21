-- Entering PvP
function Act_EnteringPvP_OnCheckCanEnter( actor, action )
	
	if true == g_world:IsNil() then
		return false
	end
	
	if true == actor:IsNil() then
		return false
	end	

	-- 안전거품  막기
	if g_pilotMan:IsLockMyInput() == true 
		and actor:IsMyActor()  
		then	
		g_ChatMgrClient:Notice_Show( GetTextW(400858), 1 )
		return false
	end	
--[[		
	local kPilot = actor:GetPilot()
	if false == kPilot:IsNil() then
		if 0 < kPilot:GetAbil(AT_HP) then
			if g_world:IsHaveWorldAttr(GATTR_FLAG_NOPVP) then
				if true == actor:IsMyActor() then
					g_ChatMgrClient:Notice_Show( GetTextW(19010), 1 )
				end
			else
				local iLevelLimit = PVP_LimitLevel()
				if kPilot:GetAbil(AT_LEVEL) >= iLevelLimit then
					local kUnit = kPilot:GetUnit()
					if false == kUnit:IsNil() then
						local kPartyGuid = kUnit:GetPartyGuid()
						if true == kPartyGuid:IsNil() then
							if false == kUnit:IsEffect(402001) then
								return true
							else
								if true == actor:IsMyActor() then
									g_ChatMgrClient:Notice_Show( GetTextW(400858), 1 )
								end
							end
						else
							if true == actor:IsMyActor() then
								g_ChatMgrClient:Notice_Show( GetTextW(700035), 1 )
							end
						end
					end
				else
					if true == actor:IsMyActor() then
						g_ChatMgrClient:Notice_Show( WideString(string.format( GetTextW(200135):GetStr(), iLevelLimit )), 1 )
					end
				end
			end
		end
	end
	]]
	return true;
end

function Act_EnteringPvP_OnEnter(actor, action)
	
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if actor:IsMyActor() == true then
		local kWaitUI = GetUIWnd("FRM_PVP_WAIT")
		if false == kWaitUI:IsNil() then
			-- 이미 접속중이면 취소
			local kTextUI = kWaitUI:GetControl("SFRM_TEXT")
			if kTextUI:IsNil() == false then
				kTextUI:SetStaticText( GetTextW(400396):GetStr() )
			end
			
			local kAniBar = kWaitUI:GetControl("BAR_TIMER")
			kAniBar:SetCloseTime(1000)
			return false
		end
	
		kWaitUI = CallUI("FRM_PVP_WAIT", true )
		if false == kWaitUI:IsNil() then
			local kTextUI = kWaitUI:GetControl("SFRM_TEXT")
			if kTextUI:IsNil() == false then
				kTextUI:SetStaticText( GetTextW(400393):GetStr() )
			end
			
			local kAniBar = kWaitUI:GetControl("BAR_TIMER")
			kAniBar:SetStartTime(5000)

			-- Step
			-- 0 : Error
			-- 1 : Entering
			-- 2 : Success
			action:SetParamInt(0,1)	-- begin
			return true
		end
	end
	
	return false
end

function Act_EnteringPvP_OnUpdate(actor, accumTime, frameTime)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local action = actor:GetAction()
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if actor:IsMyActor() == true then
		local kWaitUI = GetUIWnd("FRM_PVP_WAIT")
		if true == kWaitUI:IsNil() then
			if 1 == action:GetParamInt(0) then
				action:SetParamInt(0,2)
			end
			return false
		end
	end
	return true
end

function Act_EnteringPvP_OnLeave(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local kAction = actor:GetAction()
	if( CheckNil(kAction == nil) ) then return false end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	if actor:IsMyActor() == true then		
		local kWaitUI = GetUIWnd("FRM_PVP_WAIT")
		if false == kWaitUI:IsNil() then
			local kTextUI = kWaitUI:GetControl("SFRM_TEXT")
			if kTextUI:IsNil() == false then
				kTextUI:SetStaticText( GetTextW(400396):GetStr() )
			end
			
			local kAniBar = kWaitUI:GetControl("BAR_TIMER")
			kAniBar:SetCloseTime(1000)
			kAction:SetParamInt(0,0)
		end
	end
	return true
end

function Act_EnteringPvP_OnCleanUp(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if actor:IsMyActor() == true then
		
		local kAction = actor:GetAction()
		if 2 == kAction:GetParamInt(0) then
		
			local kPacket = NewPacket(30750)--PT_C_M_REQ_PVP_SELECTOR
			Net_Send(kPacket)
			DeletePacket(kPacket)
		--	Net_JoinPvP()
		
		else
		
			local kWaitUI = GetUIWnd("FRM_PVP_WAIT")
			if false == kWaitUI:IsNil() then
				local kTextUI = kWaitUI:GetControl("SFRM_TEXT")
				if kTextUI:IsNil() == false then
					kTextUI:SetStaticText( GetTextW(400396):GetStr() )
				end
			
				local kAniBar = kWaitUI:GetControl("BAR_TIMER")
				kAniBar:SetCloseTime(1000)
			end
			
		end
	end
	return true
end

-- emotion 
function Act_PvP_Emotion_OnCheckCanEnter(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if actor:IsMeetFloor() == false then
		return	false
	end
	
	return	true;
end
function Act_PvP_Emotion_OnEnter(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	actor:Stop()
	local actionID = action:GetID()
	local iSlotCount = action:GetSlotCount()
	action:SetSlot(math.random(0, iSlotCount))

	-- 앞쪽을 보게 한다.
	actor:LockBidirection(false)
	actor:SeeFront(true,true)
	
	-- 무기를 숨긴다.
	actor:HideParts(6,true)
	return true
end

function Act_PvP_Emotion_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	-- 무한 Loop
	if actor:IsAnimationDone() then
		actor:ResetAnimation()
		actor:PlayCurrentSlot()
	end
	return true
end

function Act_PvP_Emotion_OnLeave(actor, action)
	return true
end

function Act_PvP_Emotion_OnCleanUp(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	actor:SeeFront(false,true)
	actor:RestoreLockBidirection()
	actor:HideParts(6,false)
	return true
end
