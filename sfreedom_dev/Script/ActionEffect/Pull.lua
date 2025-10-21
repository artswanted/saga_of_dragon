-- damage
--

function Act_Pull_IsSetSendBlowStatus(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local AttackActor = action:GetEffectCaster()
	
	if actor:IsMyActor() then return true end
	if true==AttackActor:IsNil() then
		if not actor:IsUnitType(UT_PLAYER) then return true end
	else
		if GetMyActor():GetPilotGuid():IsEqual( AttackActor:GetPilotGuid() ) then
			--ODS("Act_Pull_IsSetSendBlowStatus IsEqual true\n")
			return true
		end
	end

	return false
end

function Act_Pull_OnCheckCanEnter(actor,action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil())
	local CurAction = actor:GetAction()
	CheckNil(nil==CurAction);
	CheckNil(CurAction:IsNil())

	action:SetParamInt(9, 0)
	if CurAction:GetID()==action:GetID() then
		action:SetParamInt(9, 1)
		return false
	end

	return	true
	
end

function Act_Pull_OnEnter(actor, action)
	actor:StopJump();
	actor:StartGodTime(0);
	actor:SetMovingDelta(Point3(0,0,0));
	actor:Stop()
	actor:SetNoWalkingTarget(false);

	if 1==action:GetParamInt(9) then
		return true
	end

	local	fSpeed = tonumber(action:GetScriptParam("VELOCITY"));
	if fSpeed == nil then
		fSpeed = 350
	end
	
	local AttackActor = action:GetEffectCaster()
	if true==AttackActor:IsNil() then
		actor:PushActor(not actor:IsToLeft(),fDistance,fSpeed,1);
	else
		local actorPos = actor:GetTranslate()
		local casterPos = AttackActor:GetTranslate()
		local kMovingDir = casterPos:_Subtract(actorPos)
		local fDistance = kMovingDir:Length()
		kMovingDir:Unitize()
		actor:PushActorDir(kMovingDir,fDistance,fSpeed,1);		
	end

	local kEfect = action:GetScriptParam("EFFECT")
	if nil~=kEfect and ""~=kEfect then
		local kScale = action:GetScriptParamAsInt("SCALE")
		if nil==kScale or 0==kScale then
			kScale = 1
		end
		local kNode = action:GetScriptParam("EFFECT_NODE")
		if nil==kNode or ""==kNode then
			kNode = "char_root"
		end
		actor:AttachParticleS(86754, kNode, kEfect, kScale)
	end

	action:SetParamInt(7, 0)
	if Act_Pull_IsSetSendBlowStatus(actor, action) then
		action:SetParamInt(7, 1)
	end
	return true
end

function Act_Pull_OnUpdate(actor, accumTime, frameTime)
	local action = actor:GetAction()
	
	if actor:GetNowPush() == false then
		if 0==action:GetParamInt(9) then
			if Act_Pull_IsSetSendBlowStatus(actor, action) then
				actor:SetSendBlowStatus(false, true);
				--ODS("Act_Pull_OnUpdate SetSendBlowStatus\n",false,6482)
			end
		end
		return	false
	end

	return true
end

function Act_Pull_OnCleanUp(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil())
	CheckNil(nil==action);
	CheckNil(action:IsNil())
	local CurAction = actor:GetAction()
	CheckNil(nil==CurAction);
	CheckNil(CurAction:IsNil())
	if action:GetID()~=CurAction:GetID() then
		actor:DetachFrom(86754)
	end
end

function Act_Pull_OnLeave(actor, action)

	if 0==action:GetParamInt(9) then
		if action:GetID() ~= "a_die" then
			local kEffectDef = GetEffectDef(actor:GetAction():GetActionNo());
			local fGodTime = 1;
			if kEffectDef:IsNil() == false then
				fGodTime = kEffectDef:GetAbil(AT_GOD_TIME)/1000.0;
			end
			actor:StartGodTime(fGodTime);
		end
		
		actor:SetMovingDir(Point3(0, 0, 0))

		if action:GetParamInt(7) then
			actor:SetSendBlowStatus(false, false);
			--ODS("Act_Pull_OnLeave SetSendBlowStatus\n",false,6482)
		end
	end
	return true
end
