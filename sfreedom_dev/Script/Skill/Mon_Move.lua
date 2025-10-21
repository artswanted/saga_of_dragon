-- skill

function Act_Mon_Move_OnCheckCanEnter(actor, action)
end

function Act_Mon_Move_OnTargetListModified(actor,action,bIsBefore)
end

function Act_Mon_Move_SetState(actor,action,kState)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	actor:ResetAnimation()
	action:SetSlot(0)
	actor:PlayCurrentSlot()

	return	true

end

function Act_Mon_Move_OnEnter(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end	

	ODS("Act_Mon_Move_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n");
	
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end
	

	local	ptTargetPos = 0;

	local	kPacket = action:GetParamAsPacket();
	if kPacket == nil or kPacket:IsNil() then
		ODS("[SkillError] Act_Mon_Move_OnEnter, kPacket is not valid. actor : "..actor:GetPilotGuid():GetString().." action:"..action:GetID().."\n")
		return	false;
		
	else
		ptTargetPos = Point3(0,0,0);
		ptTargetPos:SetX(kPacket:PopFloat());
		ptTargetPos:SetY(kPacket:PopFloat());
		ptTargetPos:SetZ(kPacket:PopFloat()+25);
	end
		

	
	action:SetParamAsPoint(0,ptTargetPos);
		
	actor:ResetAnimation()
	action:SetSlot(0)
	actor:PlayCurrentSlot()
	
	--	방향 벡터 구하기
	local	kCurrentPos = actor:GetPos()
	local	kDirVector = ptTargetPos:_Subtract(kCurrentPos)
	local len = kDirVector:Length()
	kDirVector:Unitize()
	
	action:SetParamAsPoint(1,kDirVector)
	
	
	if( Act_Mon_Move_SetState(actor,action,"BATTLEIDLE_START") == false) then
		Act_Mon_Move_SetState(actor,action,"BATTLEIDLE_LOOP")
	end	


	--SetBreak();
	actor:LookAt(ptTargetPos,true,false,true);
	actor:SetMovingDir(actor:GetLookingDir());
	action:SetParamFloat(2,len)
	ODS("시작 "..len.."\n", false, 1509)

	return true
end
function Act_Mon_Move_OnCastingCompleted(actor,action)	

	Act_Mon_Move_SetState(actor,action,"FIRE")
	
	Act_Mon_Move_Fire(actor,action);
	
end

function Act_Mon_Move_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Act_Mon_Move_Fire\n");
	
	--	타겟 좌표 바라보기
	local	ptTargetPos = action:GetParamAsPoint(0)
	--actor:LookAt(ptTargetPos,true,false,true);

	--	방향 벡터 구하기
	-- local	kCurrentPos = actor:GetPos()
	-- local	kDirVector = ptTargetPos:_Subtract(kCurrentPos)
	-- kDirVector:Unitize()
	
	-- action:SetParamAsPoint(1,kDirVector)


end

function Act_Mon_Move_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	
	
	local	iState = action:GetParamInt(0)
	
	local	kLookDir = actor:GetLookingDir();

	
	local	kMovingDir = action:GetParamAsPoint(1)
	
	local	fMoveSpeed = actor:GetAbil(AT_MOVESPEED)*(1+action:GetAbil(AT_R_MOVESPEED)/ABILITY_RATE_VALUE);

	local	ptTargetPos = action:GetParamAsPoint(0)
	local	kCurrentPos = actor:GetPos()
	actor:LookAt(ptTargetPos,true,false,true);
	local	kCurrentDir = ptTargetPos:_Subtract(kCurrentPos)
	local len = kCurrentDir:Length()

	if len <10 then
		return false
	end
	
	kMovingDir:Multiply(fMoveSpeed)
	
	actor:SetMovingDelta(kMovingDir);

	action:SetParamFloat(2, len)

	return true
end

function Act_Mon_Move_OnCleanUp(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end


end

function Act_Mon_Move_OnLeave(actor, action)
	
	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
	local	kCurAction = actor:GetAction();
	
	if( CheckNil(nil==kCurAction) ) then return true end
	if( CheckNil(kCurAction:IsNil()) ) then return true end
	
	local	actionID = action:GetID()
	ODS("Act_Mon_Move_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n");
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

function Act_Mon_Move_OnEvent(actor,textKey)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	

	return true
end

