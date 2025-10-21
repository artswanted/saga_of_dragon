function Act_Mon_CrossAttack_OnEnter(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Act_Mon_CrossAttack_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n")
	
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end
	
	actor:ResetAnimation()
	actor:FindPathNormal(true)
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_CrossAttack_OnCastingCompleted(actor,action)
	else	
		if( Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
		
	    ODS("Act_Mon_CrossAttack_OnEnter Start Casting\n");
		
		-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
		local	kTargetList = action:GetTargetList();
		local	iTargetCount = kTargetList:size();
	
		ODS("Act_Mon_CrossAttack Casting Start iTargetCount :"..iTargetCount.."\n");

		
		if iTargetCount>0 then
			local	kTargetInfo = kTargetList:GetTargetInfo(0);
			if kTargetInfo:IsNil() == false then
			
				local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID());
				if kTargetPilot:IsNil() == false then
				
					local	kTargetActor = kTargetPilot:GetActor();
					if kTargetActor:IsNil() == false then
					
						actor:LookAtBidirection(kTargetActor:GetPos());
						actor:LookAt(kTargetActor:GetPos(),true,true);
					
					end
				end
			end
		end
	end
	

	return true
end
function Act_Mon_CrossAttack_OnCastingCompleted(actor,action)	

	Act_Mon_Melee_SetState(actor,action,"FIRE")
	
	Act_Mon_CrossAttack_Fire(actor,action);
	
end

function Act_Mon_CrossAttack_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	--GetActionResultCS(action, actor)

	-- 타겟리스트의 첫번째 놈을 바라보도록 한다.
	-- local	kTargetList = action:GetTargetList();
	-- local	iTargetCount = kTargetList:size();
	
	-- ODS("Act_Mon_CrossAttack_Fire iTargetCount:"..iTargetCount.."\n");
	
	
	-- if iTargetCount>0 then
		-- local	kTargetInfo = kTargetList:GetTargetInfo(0);
		-- if kTargetInfo:IsNil() == false then
		
			-- local	kTargetPilot = g_pilotMan:FindPilot(kTargetInfo:GetTargetGUID());
			-- if kTargetPilot:IsNil() == false then
			
				-- local	kTargetActor = kTargetPilot:GetActor();
				-- if kTargetActor:IsNil() == false then
				
					-- ODS("Act_Mon_CrossAttack_Fire LookAtBidirection Attacker:"..actor:GetID().." Target:"..kTargetActor:GetID().."\n");
					-- --actor:LookAtBidirection(kTargetActor:GetPos());
					-- --actor:LookAt(kTargetActor:GetPos(),true,true);
				-- end
			-- end
		-- end
	-- end
	
end

function Act_Mon_CrossAttack_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then
	
		if actor:IsAnimationDone() == true then
			if 2==iState then
				Act_Mon_Melee_SetState(actor,action,"RETURN")
			else
				return false
			end
		end
		
	elseif action:GetActionParam() == AP_CASTING then
		
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
	
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_CrossAttack_OnCastingCompleted(actor,action)
			
			end
		end
	
	end
	return true
end
function Act_Mon_CrossAttack_OnCleanUp(actor, action)
end

function Act_Mon_CrossAttack_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
	local	kCurAction = actor:GetAction();
	local	actionID = action:GetID()
	ODS("Act_Mon_CrossAttack_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n");
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

function Act_Mon_CrossAttack_OnEvent(actor,textKey)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
    ODS("Act_Mon_CrossAttack_OnEvent actor:"..actor:GetID().." textKey:"..textKey.."\n");
	
	--Act_Mon_Cast_Effect(actor,textKey)
	--Act_Mon_CrossAttack_SetEffectPos(actor)
	Act_Mon_CrossAttack_PlayEffect(actor, textKey)
	
    if textKey == "hit" or textKey == "fire" then
        local   action = actor:GetAction();
		
		if( CheckNil(nil==action) ) then return false end
		if( CheckNil(action:IsNil()) ) then return false end
	
		Act_Mon_Melee_DoDamage(actor,action);

		local QuakeTime = actor:GetAnimationInfo("QUAKE_TIME")
		if nil ~= QuakeTime and "" ~= QuakeTime then
			local QuakeFactor = actor:GetAnimationInfo("QUAKE_FACTOR")	
			if  nil == QuakeFactor then
				QuakeFactor = 1.0
			else
				QuakeFactor = tonumber(QuakeFactor)
			end
			QuakeCamera(QuakeTime, QuakeFactor)
		end
	end
	
	return true;
end

function Act_Mon_CrossAttack_OnTargetListModified(actor,action,bIsBefore)
end

function Act_Mon_CrossAttack_SetEffectPos(actor)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	local	action = actor:GetAction()
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	local kSkillDef = GetSkillDef(action:GetActionNo())
	local iDist = kSkillDef:GetAbil(AT_2ND_AREA_PARAM1)
	local iCount = kSkillDef:GetAbil(AT_COUNT)
	if(0==iCount) then
		iCount = 2	--적어도 2방향
	end

	action:SetParamInt(4, iCount)

	local kActorPos = actor:GetPos()
	local kPathNormal = actor:GetPathNormal();

	local iAngle = kSkillDef:GetAbil(AT_MON_CROSS_ATTACK_ANGLE) --시작각도
	local fAngle = math.rad(iAngle)

	local fInterAngle = math.rad(360/iCount)

	local fCos = math.cos(fAngle)
	local fSin = math.sin(fAngle)

	local x = kPathNormal:GetX()
	local y = kPathNormal:GetY()

	local x_ = x*fCos + y*fSin--기본 각도로 일단 회전 한 값
	local y_ = y*fCos - x*fSin

	kPathNormal:SetX(x_)
	kPathNormal:SetY(y_)
	kPathNormal:SetZ(0)			--Z값은 사용할때 넣자

	kPathNormal:Unitize()
	--kPathNormal:Multiply(iDist)	--공격거리만큼 늘리자
	action:SetParamAsPoint(5, kPathNormal) --여기가 인덱스 0번

	for i=1,iCount-1 do
		fAngle = fInterAngle
		fCos = math.cos(fAngle)
		fSin = math.sin(fAngle)
		x = kPathNormal:GetX()
		y = kPathNormal:GetY()
		x_ = x*fCos + y*fSin
		y_ = y*fCos - x*fSin
		kPathNormal:SetX(x_)
		kPathNormal:SetY(y_)
		kPathNormal:SetZ(0)			--Z값은 사용할때 넣자
		kPathNormal:Unitize()
		action:SetParamAsPoint(5+i, kPathNormal)
	end
end

function Act_Mon_CrossAttack_PlayEffect(actor, textKey)
	if actor == nil or textKey == nil then
		return
	end
	
	if textKey ~= "start" and textKey ~= "fire" and textKey ~= "hit" then
		return
	end
	
	local	action = actor:GetAction()
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if textKey == "start" then
		local iFirst = action:GetParamInt(88)
		if iFirst == 1 then
			return
		end
		action:SetParamInt(88, 1)
		Act_Mon_CrossAttack_SetEffectPos(actor)
	end
	
	local EffectNode = nil
	local Effect = nil
	local EffectScale = nil
	
	if textKey == "start" then
		EffectNode = actor:GetAnimationInfo("CAST_EFFECT_NODE")
		if EffectNode == nil then
			EffectNode = "char_root"
		end
		Effect = actor:GetAnimationInfo("CAST_EFFECT")
		EffectScale = actor:GetAnimationInfo("CAST_EFFECT_SCALE")
		if nil==EffectScale or ""==EffectScale then
			EffectScale = 1
		else
			EffectScale = tonumber(EffectScale)
		end
	end
	
	if textKey == "fire" or textKey == "hit" then
		EffectNode = actor:GetAnimationInfo("FIRE_EFFECT_NODE")
		if EffectNode == nil then
			EffectNode = "char_root"
		end
		Effect = actor:GetAnimationInfo("FIRE_EFFECT")
		EffectScale = actor:GetAnimationInfo("FIRE_EFFECT_SCALE")
		if nil==EffectScale or ""==EffectScale then
			EffectScale = 1
		else
			EffectScale = tonumber(EffectScale)
		end	
	end
	
	if nil==Effect then
		return
	end
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	local	action = actor:GetAction()
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	local kSkillDef = GetSkillDef(action:GetActionNo())
	local iDist = kSkillDef:GetAbil(AT_2ND_AREA_PARAM1)
	local iWidth = kSkillDef:GetAbil(AT_2ND_AREA_PARAM2)*2
	
	if iWidth == 0 then
		return
	end

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local kActorPos = actor:GetPos()
	local kRayPos = Point3(kActorPos:GetX(), kActorPos:GetY(), kActorPos:GetZ()+100)
	local kFloorPos = g_world:ThrowRay(kRayPos, Point3(0,0,-1), 200)

	local iPacticleIdx = 43000
	actor:AttachParticleToPointS(iPacticleIdx, kFloorPos, Effect, EffectScale)	--일단 캐릭터 위치에 하나 찍자
	iPacticleIdx = iPacticleIdx + 1

	local iCount = action:GetParamInt(4)	--갯수
	local iDrawCount = iDist / iWidth
	local fRestLen = iDist - iWidth * iDrawCount + iWidth*0.5 --나머지 길이

	for i=0,iCount-1 do	--방향 갯수
		local kRot1 = action:GetParamAsPoint(5+i)	--노말라이즈 되어있는 상태
		if textKey ~= "start" then
			local kTemp = Point3(kRot1:GetX(), kRot1:GetY(), kRot1:GetZ())
			kTemp:Multiply(iDist)
			kTemp:Add(kActorPos)
		end
		for k=0,iDrawCount-1 do	-- 이펙트 갯수
			local fLen = fRestLen + k * iWidth
			local kPPos = kRot1:_Multiply(fLen)		--파티클이 붙을 위치
			kPPos:Add(kActorPos)
			actor:AttachParticleToPointS(iPacticleIdx, kPPos, Effect, EffectScale)
			iPacticleIdx = iPacticleIdx + 1
		end
				
	end
end
