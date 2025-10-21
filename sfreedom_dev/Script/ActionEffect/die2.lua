-- slot 1 : ani 시작 시간
-- slot 2 : total ani time
-- slot 3 : CleanUp flag
-- TODO : blink가 점점 빨라지게 하자.
-- 죽는 ani/type을 좀 더 다양하게 만들자. (knockback이 된다던지.. 뒤로 쭉 밀린다던지..)

-- 죽었을 때 날아가는 시간

function Act_Die2_OnCheckCanEnter(actor,action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local kCurAction = actor:GetAction()
	if( CheckNil(kCurAction == nil) ) then return false end
	if( CheckNil(kCurAction:IsNil()) ) then return false end
	if kCurAction:GetID() == "a_telejump" then	--	텔레점프 중에 죽었을 경우, 텔레점프가 끝난 후에 죽도록 한다.
	
		if kCurAction:GetParam(10) =="Telejump_Die_OK" then
			return	true
		end
		kCurAction:SetParam(10,"Next_Die")
		return	false;
	end
	
	return	true;

end

function Act_Die2_OnEnter(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	--ODS("Act_Die2_OnEnter actor : "..actor:GetID().." GUID :"..actor:GetPilotGuid():GetString().."\n");

	action:SetDoNotBroadCast(true)
	actor:Stop()
	actor:SetCanHit(false)
	
	if action:GetParam(1) == "DIE_IDLE" then
		action:SetSlot(1)
		ChangeDieAnimation(actor, action)
		return	true
	end
		
	action:SetParamInt(4,0)	

	local actorID = actor:GetID()	
	if actorID ~= nil then
		-- 몬스터 이면..
		if string.sub(actorID, 1, 1) == "m" then
		
			-- 20%의 확률로 카메라 쪽으로 날아간다.
			if actor:IsMeetFloor() and 0==actor:GetAbil(AT_SKILL_ON_DIE) and 
				0==actor:GetAbil(AT_NOT_FLY_TO_CAMERA) and
				actor:GetAbil(AT_GRADE) ~= 3 and
				actor:GetAbil(AT_GRADE) ~= 4 then 
				
				if math.random(1000)<200 then
				
					action:SetNextActionName("a_fly_to_camera");	--	카메라쪽으로 날려보내자
					action:ChangeToNextActionOnNextUpdate(true, true)
					return	true;
				
				end
			end

			-- if actor:GetExistDieAnimation() == true then
				-- actor:SetUseSubAnimation(true)
				-- actor:AttachSound(2000,"mon_die");
			-- end
		
			local heartPos = actor:GetNodeTranslate("p_ef_heart")
			local DieParticleNode = actor:GetDieParticleNode()
			if nil~=DieParticleNode and 0<string.len(DieParticleNode) then
				heartPos = actor:GetNodeTranslate(DieParticleNode)
			end

			
			local	die_particle_id = actor:GetDieParticleID();
			if die_particle_id == "" then
				die_particle_id = "e_transition_01"
			end

			actor:SetHideShadow(true)
			
			action:SetParamInt(0, 1)
			
			if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then		--	죽기 애니가 없다면
				action:SetParamInt(4,2)
				actor:HideNode("Scene Root",true);	--	모습을 감추자
				actor:ReleaseAllParticles();
				actor:AttachSound(2784,"die_poof")	-- 펑소리
			end
			actor:AttachParticleToPointS(436, heartPos, die_particle_id, actor:GetDieParticleScale())
		end
	end
	
	action:SetSlot(0)
	ChangeDieAnimation(actor, action)
	actor:ResetAnimation()
	actor:PlayCurrentSlot()
	action:SetParamFloat(1, g_world:GetAccumTime())
	action:SetParamInt(3,0)
	action:SetParamFloat(2, g_dyingTime)	--	알파를 빼는 시간
	action:SetParamFloat(0,g_dyingAlphaDelayTime);	--	죽기 애니가 끝난 후, 알파를 빼기전까지의 시간
	
	if action:GetParamInt(4) == 0 then
	
		if actor:IsMeetFloor() == false then	--	공중에서 죽었다면, 일단 땅으로 떨어뜨리자
		
			action:SetParamInt(4,3)
			actor:FreeMove(true)
		
		end
	
	end
	return true
end

function Act_Die2_OnUpdate(actor, accumTime, frameTime)
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
	
	actor:DoDropItems();
	
--	ODS("Act_Die2_OnUpdate " .. iDieState .. " accumTime: " .. accumTime .. "\n", false, 1509)
		
	if iDieState == 0 then	--	죽기 모션이 끝나기를 기다린다
		
		if actor:GetPilot():IsPlayerPilot() == false then
			if actor:IsAnimationDone() == true then				
				action:SetParamFloat(1,accumTime);
				action:SetParamInt(4,1);
				actor:ReleaseAllParticles();
				if actor:GetAnimationLength(action:GetSlotAnimName(1)) ~= 0 then
					action:SetSlot(1)
					actor:ResetAnimation()
					actor:PlayCurrentSlot()
--					ODS("다음애니 하기\n", false, 1509)
				end

			end
		
			return	true;
		end
	elseif iDieState == 1 then	--	알파를 빼기전에 일정시간동안 죽은 모션을 보여준다
	
		if actor:IsMyActor() == false then
--			if (startTime + alphadelaytime) < accumTime then
				actor:SetTargetAlpha(actor:GetAlpha(), 0.0, totalalphafadeoutTime);
				action:SetParamInt(4,2);				
--			end
			return	true;
		end	
		
	elseif iDieState == 2 then	--	알파가 다 빠지기를 기다린다
	
		if elapsedTime > totalalphafadeoutTime + alphadelaytime then
			if actor:GetPilot():IsPlayerPilot() == false then
				g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
				action:SetParamInt(3,1);
			end		
			return false
		end
	
	elseif iDieState == 3 then	-- 땅으로 떨어지기
	
		if actor:IsMeetFloor() then
			actor:FreeMove(false);
			action:SetParamInt(4,0)
			return	true
		end
		
		local	fZ_Velocity_Init = -0;
		local	fTotalElapsedTime = accumTime - action:GetActionEnterTime();
		
		local	fSpeed =g_fGravity*(0.5*frameTime+fTotalElapsedTime)+fZ_Velocity_Init;
	
		actor:SetMovingDelta(Point3(0,0,fSpeed));	
	
	end


	return true
end

function Act_Die2_OnCleanUp(actor, action)
	if( CheckNil(action == nil) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	
	actor:FreeMove(false);
	if actor:GetAbil(AT_GRADE) == EMGRADE_BOSS then
		g_world:OnBossDie()
	end
end

function Act_Die2_OnLeave(actor, action)
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(action == nil) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	if action:GetID() == "a_fly_to_camera" then
		return	true;
	end
	
	-- 몬스터라면 제거한다.
	actor:DetachFrom(436);

	if actor:GetPilot():IsPlayerPilot() == false and actor:GetAction():GetParamInt(3)==0 then
		g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
	end
	
	if action:GetID() == "a_revive" or action:GetID() == "a_Resurrection_01" then
		return	true;
	end

	return false
end

