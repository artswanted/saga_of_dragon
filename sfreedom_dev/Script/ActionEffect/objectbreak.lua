
function Act_Object_Break_OnEnter(actor, action)
	
	ODS("Act_Object_Break_OnEnter actor : "..actor:GetID().." GUID :"..actor:GetPilotGuid():GetString().."\n");
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	action:SetDoNotBroadCast(true)
	actor:Stop()
	actor:SetCanHit(false)

	local	kPilot = actor:GetPilot();
	local	kBreakable = kPilot:GetObject();
	
	if kBreakable:IsNil() == false then
		kBreakable:Break();
	end
	
	if action:GetParam(1) == "DIE_IDLE" then
		action:SetSlot(1)
		return	true
	end
		
	action:SetParamInt(4,0)	

	local actorID = actor:GetID()	
		
	if actor:GetExistDieAnimation() == true then
		actor:SetUseSubAnimation(true)
		actor:AttachSound(2000,"mon_die");
	end

	local heartPos = actor:GetNodeTranslate("p_ef_heart")
	
	local	die_particle_id = actor:GetDieParticleID();
	if die_particle_id ~= "" then
		actor:AttachParticleToPoint(436, heartPos, die_particle_id);
	end
	
	actor:SetHideShadow(true)
	
	action:SetParamInt(0, 1)
	
	if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then		--	죽기 애니가 없다면
		action:SetParamInt(4,2)
		actor:HideNode("Scene Root",true);	--	모습을 감추자
	end
				
	actor:ResetAnimation()
	actor:PlayCurrentSlot()
	action:SetParamFloat(1, g_world:GetAccumTime())
	action:SetParamInt(3,0)
	action:SetParamFloat(2, g_dyingTime)	--	알파를 빼는 시간
	action:SetParamFloat(0,g_dyingAlphaDelayTime);	--	죽기 애니가 끝난 후, 알파를 빼기전까지의 시간
	
	return true
end

function Act_Object_Break_OnUpdate(actor, accumTime, frameTime)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local action = actor:GetAction()		
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local startTime = action:GetParamFloat(1)
	local totalalphafadeoutTime = action:GetParamFloat(2)	
	local elapsedTime = accumTime - startTime
	local	iDieState = action:GetParamInt(4)
	local	alphadelaytime = action:GetParamFloat(0);
	
	if action:GetCurrentSlot() == 1 then
		return	true
	end
	
	actor:DoDropItems();

		
	if iDieState == 0 then	--	죽기 모션이 끝나기를 기다린다
		
		if actor:IsAnimationDone() == true then				
			action:SetParamFloat(1,accumTime);
			action:SetParamInt(4,1);
		end
	
		return	true;

	elseif iDieState == 1 then	--	알파를 빼기전에 일정시간동안 죽은 모션을 보여준다
	
		if (startTime + alphadelaytime) < accumTime then
			actor:SetTargetAlpha(actor:GetAlpha(), 0.0, totalalphafadeoutTime);
			action:SetParamInt(4,2);
		else
			BreakObject(actor)
		end
		return	true;
		
	elseif iDieState == 2 then	--	알파가 다 빠지기를 기다린다
	
		if elapsedTime > totalalphafadeoutTime + alphadelaytime then
			g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
			action:SetParamInt(3,1)
			BreakObject(actor)
			return false
		end
	end


	return true
end

function Act_Object_Break_OnCleanUp(actor, action)
	BreakObject(actor)
end

function Act_Object_Break_OnLeave(actor, action)
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	local kCurAction = actor:GetAction()
	if( CheckNil(kCurAction == nil) ) then return true end
	if( CheckNil(kCurAction:IsNil()) ) then return true end
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	-- 몬스터라면 제거한다.
	actor:DetachFrom(436);

	if kCurAction:GetParamInt(3)==0 then
		g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
	end

	return false
end

function BreakObject(actor)
	if nil==actor or actor:IsNil() then
		return
	end
	local	kPilot = actor:GetPilot();
	if kPilot:IsNil() then
		return
	end
	local	kBreakable = kPilot:GetObject();
	
	if kBreakable:IsNil() == false then
		kBreakable:Break();
	end
end
