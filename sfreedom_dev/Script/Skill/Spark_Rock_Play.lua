-- [INSTALL_EFFECT_TARGET_NODE] : Install Target에 이펙트를 붙일때, 붙일 노드
-- [INSTALL_EFFECT_ID] : Install Target에 붙일 이펙트 ID
-- [FIRE_CASTER_SCALE_NORMALIZE] : 스킬이 실제 발동될때 캐스터의 크기를 실제 범위와 맞추기 위해 보정하는값
-- [ENTER_TIMER_TIME] : OnEnter에서 타이머의 유지시간을 세팅
-- [ENTER_TIMER_TICK] : OnEnter에서 타이머의 Tick을 세팅
function Skill_Spark_Rock_Play_OnEnter(actor, action)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local actorID = actor:GetID()
	local actionID = action:GetID()
	
	action:SetDoNotBroadCast(true)
	
	--	시작 시간 기록
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(0,g_world:GetAccumTime())
	action:SetParamFloat(1,g_world:GetAccumTime())
	

	--local iSkillLevel = actor:GetAbil(AT_LEVEL)-1
	--actor:SetTargetScale(1+(0.1*iSkillLevel))
	action:SetParam(1, "start")
	action:SetParam(2, "EnterPassed")
	
	local iAttackRange = action:GetSkillRange(0,actor);
	local fCasterScaleNomalize = action:GetScriptParamAsFloat("FIRE_CASTER_SCALE_NORMALIZE")
	if(0 ~= fCasterScaleNomalize) then
		fCasterScaleNomalize = tonumber(fCasterScaleNomalize)
		fCasterScaleNomalize = iAttackRange/fCasterScaleNomalize
		actor:SetTargetScale(fCasterScaleNomalize)
	end
	
	local iFollowPlayerDir = 1
	if("FALSE" == action:GetScriptParam("FOLLOW_PLAYER_DIRECTION")) then
		iFollowPlayerDir = 0
	end
		
	if(1 == iFollowPlayerDir) then
		-- 플레이어의 방향과 오브젝트의 방향을 맞추기 위함
		local kQuat = Quaternion(-90*DEGTORAD, Point3(0,0,1))
		local kMyPilot = actor:GetPilot()
		if not kMyPilot:IsNil() then 	
			local kMyUnit = kMyPilot:GetUnit()
			if not kMyUnit:IsNil() then 
				local kCallerPilot = g_pilotMan:FindPilot(kMyUnit:GetCaller())
				if not kCallerPilot:IsNil() then 
					local kCallerActor = kCallerPilot:GetActor()
					if not kCallerActor:IsNil() then
						local kCallerRot= kCallerActor:GetRotateQuaternion()
						kQuat = kCallerRot:Multiply(kQuat)
						actor:SetRotationQuat(kQuat)		
					end	
				end
			end
		end	
		action:SetParamFloat(10, kQuat:GetX())
		action:SetParamFloat(11, kQuat:GetY())
		action:SetParamFloat(12, kQuat:GetZ())
		action:SetParamFloat(13, kQuat:GetW())
	end
	
	action:SetParamInt(14, iFollowPlayerDir)
	
	local kTime = actor:GetAbil(AT_LIFETIME)
	if 0==kTime then
		kTime = 4000
	else
		kTime = kTime + 1000	--있으면 1초 더 주자(서버 클라 동기화 위해 확실히 끝났을 시간을 넣어야함. 오차범위로 1초 정도줌)
	end
	action:SetParamFloat(20, action:GetParamFloat(0)+kTime/1000)
	--ODS("라이프:"..actor:GetAbil(AT_LIFETIME).."\n현시간:"..action:GetParamFloat(0).."시간:"..action:GetParamFloat(20).."\n", false, 912)
	--	ODS("스파크 롹 엔터 통과:"..fCasterScaleNomalize.."\n", false, 912)
	
	local fTimerTotalTime = action:GetScriptParamAsFloat("ENTER_TIMER_TIME")
	if(0 == fTimerTotalTime) then -- 값이 없으면 라이프 타임을 사용
		fTimerTotalTime = actor:GetAbil(AT_LIFETIME)/1000
	end
	
	local fTimeTick = action:GetScriptParamAsFloat("ENTER_TIMER_TICK")
	action:StartTimer(fTimerTotalTime, fTimeTick, 0)
	actor:FreeMove(true)
	
	return true
end

function Skill_Spark_Rock_Play_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
    local   kParam = FindTargetParam()

	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir())
	kParam:SetParam_2(0,0,action:GetSkillRange(0,actor),0)
	kParam:SetParam_3(true,FTO_NORMAL+FTO_DOWN+FTO_BLOWUP)
	
	return action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets)
end

function Skill_Spark_Rock_Play_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
    if bIsBefore == false then
		SkillHelpFunc_DefaultHitOneTime(actor,action, true);
    end

end

function Skill_Spark_Rock_Play_OnUpdate(actor, accumTime, frameTime)
	--ODS("스파크 롹 업데이트\n", false, 912)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	--ODS("accumTime:"..accumTime.."\n", false, 912)
	if accumTime > action:GetParamFloat(20) then
		action:SetParam(1, "end")
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())	
		--ODS("강제로 지움\n", false, 912)
		return false
	end
	
	local animDone = actor:IsAnimationDone()
	if animDone == true then
		--ODS("animDone\n", false, 912)
		actor:PlayNext()
	end
	
	
	-- 매 업데이트마다 세팅해주지 않으면 다른 클라이언트에서 볼때 방향이 바뀌지 않는다
	local iFollowPlayerDir = action:GetParamInt(14)
	if(1 == iFollowPlayerDir) then
		--ODS("여기", false, 912)
		local kQuat = Quaternion(0*DEGTORAD, Point3(0,0,1))
		kQuat:SetX(action:GetParamFloat(10))	
		kQuat:SetY(action:GetParamFloat(11))	
		kQuat:SetZ(action:GetParamFloat(12))	
		kQuat:SetW(action:GetParamFloat(13))
		actor:SetRotationQuat(kQuat)
	end

	return true
end

function Skill_Spark_Rock_Play_OnCleanUp(actor, action)
	--SetBreak()
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	local	kCurAction = actor:GetAction()	
	CheckNil(nil==kCurAction)
	CheckNil(kCurAction:IsNil())
	
	if(kCurAction:GetParam(2) == "EnterPassed") then 
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
		actor:DetachFrom(7212)
		actor:DetachFrom(7213)
		--ODS("스파크 롹 액터 삭제함\n", false, 912)
	end
	actor:FreeMove(false)
	--ODS("스파크 롹 클린업 true\n", false, 912)
	return true
end

function Skill_Spark_Rock_Play_OnLeave(actor, action)
	--ODS("스파크 롹 Leave진입 \n", false, 912)
--	SetBreak();
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	local	kCurAction = actor:GetAction()	
	CheckNil(nil==kCurAction)
	CheckNil(kCurAction:IsNil())
	
	if("end" == kCurAction:GetParam(1)) then
		--ODS("스파크 롹 Leave 성공\n", false, 912)
		return true
	end
	--ODS("스파크 롹 Leave 실패\n", false, 912)
	return false
end


function Skill_Spark_Rock_Play_Fire(actor,action)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
    --  시작 시간
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
    action:SetParamFloat(2,g_world:GetAccumTime())
    action:SetParamFloat(3,g_world:GetAccumTime())

	local   kFireEffectID = action:GetScriptParam("INSTALL_EFFECT_ID")
    local   kFireEffectTargetNodeID = action:GetScriptParam("INSTALL_EFFECT_TARGET_NODE")
    
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" then
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            local   kDir = actor:GetLookingDir()
            local   fDistance = 30
            kDir:Multiply(fDistance)
            kDir:Add(actor:GetPos())
            kDir:SetZ(kDir:GetZ()-28)
            actor:AttachParticleToPoint(7212,kDir,kFireEffectID)
        else
            actor:AttachParticle(7212,kFireEffectTargetNodeID,kFireEffectID)
        end
    end	

	local   kFireEffectID2 = action:GetScriptParam("INSTALL_EFFECT_ID2")
    local   kFireEffectTargetNodeID2 = action:GetScriptParam("INSTALL_EFFECT_TARGET_NODE2")
    
    if kFireEffectID2 ~= "" and kFireEffectTargetNodeID2 ~= "" then
    
        if kFireEffectTargetNodeID2 == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir()
            local   fDistance = 30
            kDir:Multiply(fDistance)
            kDir:Add(actor:GetPos())
            kDir:SetZ(kDir:GetZ()-28)
            
            actor:AttachParticleToPoint(7213,kDir,kFireEffectID2)
            
        else
            actor:AttachParticle(7213,kFireEffectTargetNodeID2,kFireEffectID2)
        end
    
    end	
end

function Skill_Spark_Rock_Play_OnCastingCompleted(actor, action)
end

function Skill_Spark_Rock_Play_OnEvent(actor,textKey)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local kAction = actor:GetAction();
		
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then
	
		if actor:IsUnderMyControl() then
			
			local fTimeTick = kAction:GetScriptParamAsFloat("HIT_TIMER_TICK")
			if(0 ~= fTimeTick) then
				local fTimerTotalTime = kAction:GetScriptParamAsFloat("HIT_TIMER_TIME")
				if(0 == fTimerTotalTime) then -- 값이 없으면 라이프 타임을 사용
					fTimerTotalTime = actor:GetAbil(AT_LIFETIME)/1000
				end
				kAction:StartTimer(fTimerTotalTime, fTimeTick, 1);
			end
		end
		
	end
end

function Skill_Spark_Rock_Play_OnTimer(actor,fAccumTime,action,iTimerID)	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	if 0 == iTimerID or 1 == iTimerID then
		Skill_Spark_Rock_Play_Fire(actor, action)
		--action:SetParamFloat(1,g_world:GetAccumTime())
		action:CreateActionTargetList(actor)
		
		if IsSingleMode() or action:GetTargetCount() == 0 then	    
			Skill_Spark_Rock_Play_OnTargetListModified(actor,action,false)
		elseif actor:IsUnderMyControl() then
			action:BroadCastTargetListModify(actor:GetPilot())
			action:ClearTargetList()
		end
	end	
	return true
end