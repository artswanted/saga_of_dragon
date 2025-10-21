function Effect_ChaosRun_Begin(actor, iEffectID, iEffectKey)

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	if false == actor:IsMyActor() then
		return
	end
	
	LockPlayerInput(100);--EPLT_StatusEffect1
		
	local kDirActor = nil
	local kUnit = actor:GetPilot():GetUnit();
	if false == kUnit:IsNil() then
		local kEffect = kUnit:FindEffect( iEffectID );
		if false == kEffect:IsNil() then
			kDirActor = g_world:FindActor(kEffect:GetCaster())
		end
	end
	
	local kTargetPos = nil
	if nil~=kDirActor and false==kDirActor:IsNil() then
		kTargetPos = actor:GetLookingDir()
	else
		kTargetPos = actor:GetLookingDir():_Multiply(-1)
	end

	kTargetPos:Multiply(1000)
	kTargetPos:Add(actor:GetPos())
	
	local kNowAction = actor:GetAction()
	local action = nil
	if false==kNowAction:IsNil() then
		actor:CancelAction(kNowAction:GetActionNo(),kNowAction:GetActionInstanceID(),"a_run");
		action = actor:GetAction()
	end
	
	if nil==action then
		action = actor:ReserveTransitAction("a_run");
	end

	action:SetParamAsPoint(0,kTargetPos);
	action:SetIgnoreNotActionShift(true)
	
	--Effect_ChaosRun_UpdateDirection(actor, action)
	local fade = actor:GetStatusEffectParam(iEffectID, "FADEIN")
	if nil~=fade and "TRUE"==fade:GetStr() then
		g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 0.0, 0.5, 0.5, true, false )
	end
end

function Effect_ChaosRun_End(actor, iEffectID, iEffectKey)	
	if false == actor:IsMyActor() then
		return
	end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	actor:SetDirection(0)
	local kNextAction = actor:ReserveTransitAction("a_idle");
	kNextAction:SetIgnoreNotActionShift(true)
	UnLockPlayerInput(100);--EPLT_StatusEffect1

	local fade = actor:GetStatusEffectParam(iEffectID, "FADEIN")
	if nil~=fade and "TRUE"==fade:GetStr() then
		g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 0.5, 0.0, 0.5, true, false )
	end
end

function Effect_ChaosRun_OnTick(actor, iEffectID, fAccumTime)
	if false == actor:IsMyActor() then
		return
	end
	
	local action = actor:GetAction()
	
	local iDirection = 0
	if 0==math.random(0,1) then
		iDirection = DIR_LEFT
	else
		iDirection = DIR_RIGHT
	end
	if 0==math.random(0,1) then
		if 0==math.random(0,1) then
			iDirection = iDirection + DIR_UP
		else
			iDirection = iDirection + DIR_DOWN
		 end
	end
	--ODS(math.random(0,1)..", "..iDirection.."\n")
	local kMoveDirection = actor:GetDirectionVector(iDirection)
	local kTargetPos = Point3(kMoveDirection:GetX(), kMoveDirection:GetY(), kMoveDirection:GetZ())
	kTargetPos:Multiply(1000)
	kTargetPos:Add(actor:GetPos())
	
	action:SetParamAsPoint(0,kTargetPos);
	action:SetParamAsPoint(1,kMoveDirection);
	action:SetParamAsPoint(2,actor:GetPos());
	actor:SetMovingDir(kMoveDirection);
	actor:LookAt(kTargetPos,true,actor:IsLockBidirection(),false);
	actor:SetDirection(iDirection)

	action:BroadCast(actor:GetPilot())
end

--[[
function Effect_ChaosRun_UpdateDirection(actor, action)
	actor:FindPathNormal()
	local kTargetPos = action:GetParamAsPoint(0);
	local kSubtract = kTargetPos:_Subtract(actor:GetPos())
	kSubtract:Unitize()

	local byDir = 0
	if actor:IsLockBidirection() then
		byDir = actor:GetDirFromMovingVector(kSubtract);
	else
		byDir = actor:GetDirFromMovingVector8Way(kSubtract)
	end
	--ODS("Effect_ChaosRun_UpdateDirection DIR: "..byDir.."\n", false, 4682)
	
	actor:SetDirection(byDir)
end
]]
