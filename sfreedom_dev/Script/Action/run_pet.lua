function Act_Run_Pet_OnCheckCanEnter(actor,action)
	return true
end

function Act_Run_Pet_OnEnter(actor, action)

	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	CheckNil(action == nil);
	CheckNil(action:IsNil());

	local kMyPilot = actor:GetPilot()
	if kMyPilot:IsNil() then return true end
	local kMyUnit = kMyPilot:GetUnit()
	if kMyUnit:IsNil() then return true end

	local bPlayerPet = kMyUnit:GetCaller():IsEqual(g_pilotMan:GetPlayerPilotGuid())
	if bPlayerPet then return true end	--내 펫인 경우 코드에서 좌표를 넣어준다

	local kPacket = action:GetParamAsPacket()
	if nil==kPacket or kPacket:IsNil() then
		return false
	end
	local kGoal = kPacket:PopPoint3()
	action:SetParamAsPoint(0, kGoal)

	local kNow = actor:GetPos()
	local kDir = kGoal:_Subtract(kNow)

	action:SetParamFloat(1,kDir:Length())	--현재 남은 거리

	ODS("Act_Run_Pet_OnEnter " ..kDir:Length().."\n", false, 1509)

	return true
end

function Act_Run_Pet_OnOverridePacket(actor, action, packet)
	CheckNil(nil == action);
	CheckNil(action:IsNil());

	CheckNil(nil == packet);
	CheckNil(packet:IsNil());

	local kGoal = action:GetParamAsPoint(0)
	if kGoal:IsZero() then
		local kPos = actor:GetPos()
		kGoal:SetX(kPos:GetX())
		kGoal:SetY(kPos:GetY())	
		kGoal:SetZ(kPos:GetZ())
	end

	packet:PushPoint3(kGoal)

	local kNow = actor:GetPos()
	local kDir = kGoal:_Subtract(kNow)

	action:SetParamFloat(1,kDir:Length())	--현재 남은 거리
	ODS("Act_Run_Pet_OnOverridePacket X : "..kGoal:GetX().." Y : "..kGoal:GetY().." Z : "..kGoal:GetZ().."\n", false, 1509)
end

function Act_Run_Pet_OnUpdate(actor, accumTime, frameTime)
	CheckNil(actor == nil);
	CheckNil(actor:IsNil());
	local action = actor:GetAction()
	CheckNil(action == nil);
	CheckNil(action:IsNil());

	local kNow = actor:GetPos()
	local kGoal = action:GetParamAsPoint(0)
	local kDir = kGoal:_Subtract(kNow)

	actor:LookAt(kGoal, true, false, false)

	local len = kDir:Length()

	if len <10 then
		ODS("Act_Run_Pet_OnUpdate " ..len.."\n", false, 1509)
		return false
	end

	if len > action:GetParamFloat(1) then	--남은 거리가 늘어나면 안된다
		ODS("남은 거리가 늘어나면 안된다 : "..len.." X : "..kGoal:GetX().." Y : "..kGoal:GetY().." Z : "..kGoal:GetZ().."\n", false, 1509)
		if kGoal:IsZero() then
			kGoal = kNow
		end
		actor:SetTranslate(kGoal)
		return false
	end

	action:SetParamFloat(1,len)

	kDir:Unitize()

	local	fMoveSpeed = actor:GetAbil(AT_MOVESPEED)*(1+action:GetAbil(AT_R_MOVESPEED)/ABILITY_RATE_VALUE)*frameTime
	if 0==fMoveSpeed then
		fMoveSpeed = 100
	end

	kDir:Multiply(fMoveSpeed)
	actor:SetMovingDelta(kDir);

	return true
end

function Act_Run_Pet_OnCleanUp(actor, action)
end

function Act_Run_Pet_OnLeave(actor, action)
	return true
end
