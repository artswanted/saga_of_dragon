function nop()
end

function lavalon_start()
--	local packet = NewPacket(30101)
--	packet:PushInt(1)
--	packet:PushBool(true)

--	Net_Send(packet)
--	DeletePacket(packet)
end


function Act_LAVALON_Appear_OnEnter(actor, action)

    Common_ManualOpening_Enter(actor,action)
--	CallUI("FRM_MOVIE_IN")
--	CallUI("FRM_Lavalon")
--	CloseUI("FRM_MINIMAP")
--	CloseUI("FRM_SHINESTONE_MAIN")

	g_lavalon = actor
	actor:FreeMove(false)
	actor:LockBidirection(false)
	actor:SeeFront(true, true)
	actor:FindPathNormal()

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraModeMovie(2, 1000000, "nop()")
	QuakeCamera(15,0.5,1)

--	LockPlayerInput()
	
	-- Esc Script Regist
--	RegistESCScript("OnEsc_LavalonOpening")
	return true
end

function Act_LAVALON_Appear_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		return false
	end

	actor:SeeFront(true)

	local action = actor:GetAction()		-- 지금 엑션 객체를 반환
	local curSlot = action:GetCurrentSlot()	-- 엑션이 플레이하고 있는 에니 순번

	if 0 == action:GetParamFloat(124) then
		action:SetParamFloat(124, accumTime)
	end
	local fCurAccumTime = (accumTime - action:GetParamFloat(124)) * 2000
	
	local kDropTime = action:GetParamFloat(123) + frameTime
	action:SetParamFloat(123, kDropTime)
	
	if 14000 > fCurAccumTime and kDropTime * 1000 > math.random() * fCurAccumTime + 330 then
		Meteor_Client_Only_Count(1, 2)
		action:SetParamFloat(123, 0)
	end
	
	if action:GetParamInt(81) == 1 then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
	    if true == g_world:IsEndShowWorldFocusFilter() then
	        return false
	    end
	    return true
	end

	if curSlot == 0 and actor:IsAnimationDone() then
  		Common_ManualOpening_End(actor,action)
	end
	
	if KeyIsDown(KEY_ESC) then
        Common_ManualOpening_End(actor,action)
	end

	return true
end

function Act_LAVALON_Appear_OnCleanUp(actor, action)
	-- 애니를 다 봤으면.
	CloseUI("FRM_Lavalon")
	Common_ManualOpening_Leave(actor,action)
	
--	local kPacket = NewPacket(13201) -- PT_C_M_REQ_INDUN_START
--	Net_Send(kPacket)
--	DeletePacket(kPacket)
end

function Act_LAVALON_Appear_OnLeave(actor, action)

--	SetBossGUIDForHPBar(actor:GetPilotGuid())
--	UnLockPlayerInput()
	return true
end

function Act_LAVALON_Appear_OnEvent(actor, textKey, seqID)

	--ODS("_________________Lavalon Appear Event_________________\n")

	if textKey == "quake_01" or
		textKey == "quake_02" then
		QuakeCamera(0.7, 2, 1, 1, 1)
		Meteor_Client_Only_Count(2, 4)
	elseif textKey == "quake_03" or
		textKey == "quake_04" then
		--ODS("_________Quake Camera 3,4__________\n")
		Meteor_Client_Only_Count(3, 5)
		QuakeCamera(0.7, 5, 1, 1, 1)
	end
	ODS( GetAccumTime() .. " " .. textKey .. "\n", false, 998)
	return true
end

function Meteor_Client_Only_Count(iMin, iMax)
	local iCount = math.random(iMin, iMax)
	local iCurCount = 0
	while iCount > iCurCount do
		Meteor_Client_Only()
		iCurCount = iCurCount + 1
	end
end

function Meteor_Client_Only()
	-- Meteor가 뿌려질 좌표..
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local kTopLeft = g_world:GetObjectPosByName("pt_blaze_01")		-- Top Left
	local kTopRight = g_world:GetObjectPosByName("pt_blaze_02")	-- Top Right
	local kBottomLeft = g_world:GetObjectPosByName("pt_blaze_03")	-- Bottom Left
	local kBottomRight = g_world:GetObjectPosByName("pt_blaze_04")	-- Bottm Right

	-- 위 좌표는 반드시 의미에 부합해야 한다.
	local firstPos = kTopRight:_Subtract(kTopLeft)
	firstPos = firstPos:_Multiply(math.random())
	firstPos = firstPos:_Add(kTopLeft)
	--ODS( GetAccumTime() .. " 1. Meteor_Client_Only ......... X:" .. firstPos:GetX() .. ", Y:" .. firstPos:GetY() .. ", Z:" .. firstPos:GetZ() .. "\n", false, 998)

	local secondPos = kBottomRight:_Subtract(kBottomLeft)
	secondPos = secondPos:_Multiply(math.random())
	secondPos = secondPos:_Add(kBottomLeft)
	--ODS( GetAccumTime() .. " 2. Meteor_Client_Only ......... X:" .. secondPos:GetX() .. ", Y:" .. secondPos:GetY() .. ", Z:" .. secondPos:GetZ() .. "\n", false, 998)

	local finalPos = firstPos:_Subtract(secondPos)
	finalPos = finalPos:_Multiply(math.random())
	finalPos = finalPos:_Add(secondPos)
	--ODS( GetAccumTime() .. " f. Meteor_Client_Only ......... X:" .. finalPos:GetX() .. ", Y:" .. finalPos:GetY() .. ", Z:" .. finalPos:GetZ() .. "\n", false, 998)

	local kFinalPos = g_world:ThrowRay(finalPos, Point3(0,0,-1), 500)
	
	--ODS( GetAccumTime() .. " F. Meteor_Client_Only ......... X:" .. kFinalPos:GetX() .. ", Y:" .. kFinalPos:GetY() .. ", Z:" .. kFinalPos:GetZ() .. "\n", false, 998)
	
	local kProjectileMan = GetProjectileMan()
	local kNewArrow = kProjectileMan:CreateNewProjectile("Projectile_Lavalon_Meteor", GetDummyAction(), GUID(0))

	if kNewArrow:IsNil() == true then
		return false
	end

	kNewArrow:SetParamValue("EXTENSION", 0)

	local kInitPos = Point3(kFinalPos:GetX(), kFinalPos:GetY(), kFinalPos:GetZ() + 400)
	kInitPos:SetX(kInitPos:GetX() + math.random(-20, 20))
	if math.random(100) > 50 then
		kInitPos:SetY(kInitPos:GetY() + math.random(-20, 20))
	end
	kInitPos:SetZ(kInitPos:GetZ() + math.random(0, 400))
	kNewArrow:LoadToPosition(kInitPos)

	kNewArrow:SetTargetLoc(kFinalPos)
	kNewArrow:Fire()
end
