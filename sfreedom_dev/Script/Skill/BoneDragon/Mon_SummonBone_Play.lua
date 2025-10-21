-- Mon_SummonThorn for skill actor

function Act_Mon_SummonBone_Play_OnCheckCanEnter(actor, action) 
	return true
end

function Act_Mon_SummonBone_Play_SetState(actor,action,kState)
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	local	iNewState = -1	--이놈은 애니를 안한다는 가정
	if kState == "BATTLEIDLE_START" then
		--action:SetSlot(0)
		iNewState = 0
	elseif kState == "BATTLEIDLE_LOOP" then
		--action:SetSlot(0)
		iNewState = 1
	elseif kState == "FIRE" then
		--action:SetSlot(1)
		iNewState = 2
	elseif kState == "RETURN" then
		--action:SetSlot(0)
		iNewState = 3
	end
	return	true
end

function Act_Mon_SummonBone_Play_OnEnter(actor, action)
--	ODS("Act_Mon_SummonBone_Play_OnEnter\n", false, 1509)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	action:SetSlot(0)
	actor:ResetAnimation();
	actor:PlayCurrentSlot();
	
	actor:FreeMove(true)
	local	kTargetList = action:GetTargetList()
	local	iTargetCount = kTargetList:size()
	local	kPacket = action:GetParamAsPacket()
	if kPacket == nil or kPacket:IsNil() then
		if iTargetCount>0 then
			local	kTargetInfo = kTargetList:GetTargetInfo(0)
			if kTargetInfo:IsNil() == false then
				local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID())
				if kTargetPilot:IsNil() == false then			
					ptTargetPos = kTargetPilot:GetPos()
				end
			end
		else
			ptTargetPos = actor:GetPos()
		end
	else
		ptTargetPos = Point3(kPacket:PopFloat(),kPacket:PopFloat(),kPacket:PopFloat())
	end

	action:SetParamFloat(2, 0.0)--시간 저장
	action:SetParamAsPoint(3, actor:GetPos())--중앙 위치 저장
	action:SetParamInt(4,0)
	
	local fireStartNode	= actor:GetAnimationInfo("FIRE_EFFECT_NODE")	
	if fireStartNode == nil then
		fireStartNode = "char_root"
	end
	local fireEffect = actor:GetAnimationInfo("FIRE_EFFECT")	
	if fireEffect == nil then
		fireEffect = "ef_deathmaster_skill_03_02_char_root"
	end					

	--actor:AttachParticleToPoint(7231,actor:GetPos(),fireEffect);

	return true
end
function Act_Mon_SummonBone_Play_OnCastingCompleted(actor,action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	Act_Mon_SummonBone_Play_SetState(actor,action,"FIRE")
	
	Act_Mon_SummonBone_Play_Fire(actor,action);
end

function Act_Mon_SummonBone_Play_Fire(actor,action)
	if action:GetTargetList():IsNil() == false then
		action:GetTargetList():ApplyActionEffects();
	end
	action:ClearTargetList()
end

function Act_Mon_SummonBone_Play_OnCleanUp(actor, action)

end

function Act_Mon_SummonBone_Play_OnLeave(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	local	kCurAction = actor:GetAction()
	CheckNil(nil==kCurAction)
	CheckNil(kCurAction:IsNil())
	local	actionID = action:GetID()
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

function Act_Mon_SummonBone_Play_OnEvent(actor,textKey)
	ODS("Act_Mon_SummonBone_Play_OnEvent:"..textKey.."\n",false,987)
	return true
end

function Act_Mon_SummonBone_Play_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local animDone = actor:IsAnimationDone()
--	local iCurrentSlot = action:GetCurrentSlot();
	-- local iState = action:GetParamInt(3);
	
	-- --local   kMoveDir = Point3(action:GetParamFloat(4),action:GetParamFloat(5),action:GetParamFloat(6));
	-- local   kMoveDir = Point3(-1,0,0);
	-- local   fMoveSpeed = actor:GetAbil(AT_C_MOVESPEED);
	-- if fMoveSpeed == 0 then
	    -- fMoveSpeed = 100
    -- end
	-- ODS("SP:"..fMoveSpeed.."\n",false,987)
	-- ODS("X:"..kMoveDir:GetX().." Y:"..kMoveDir:GetY().." Z:"..kMoveDir:GetZ().."\n",false,987)
	
	-- kMoveDir:Multiply(fMoveSpeed);	
	-- actor:SetMovingDelta(kMoveDir);

	-- if iState == 0 then
		-- ODS("aa\n",false,987)
		-- local fElapsedTIme = accumTime - action:GetParamFloat(1);
		-- if actor:IsUnderMyControl() then
		-- ODS("bb\n",false,987)
			-- local fTickTime = action:GetParamFloat(100);

			-- if fTickTime == nil then
				-- fTickTime = 1.0
			-- end
			
			-- if fElapsedTIme > fTickTime then

				-- action:SetParamFloat(1,accumTime);
				-- action:CreateActionTargetList(actor);
			    
				-- if IsSingleMode() or action:GetTargetCount() == 0 then
					-- Act_Mon_SummonBone_Play_OnTargetListModified(actor,action,false);	    
				-- else		    
					-- action:BroadCastTargetListModify(actor:GetPilot());
					-- action:ClearTargetList();
				-- end
			-- end
		-- end
	-- end

	-- local   fLifeTime = actor:GetAbil(AT_LIFETIME)/1000.0;
	-- if fLifeTime == 0 then
	    -- fLifeTime = 2.0;
    -- end	

    -- local   fTotalElapsedTime = accumTime - action:GetParamFloat(0);

	-- if iState == 0 then
		-- if fLifeTime - fTotalElapsedTime < 1.5 then
			-- actor:SetTargetAlpha(actor:GetAlpha(),0, 0.5);
			-- action:SetParamInt(3, 1);
			-- actor:SetHideShadow(true);
		-- end
	-- end    

    -- if fTotalElapsedTime > fLifeTime then
        -- return  false
    -- end	

	return true
end

function Act_Mon_SummonBone_Play_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
        Actor_Mob_Base_DoDefaultMeleeAttack(actor,action);
        action:GetTargetList():ApplyActionEffects();    
    end

end