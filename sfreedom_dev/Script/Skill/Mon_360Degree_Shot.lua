-- Mon_360Degree_Shot
function Skill_Mon_360Degree_Shot_OnCheckCanEnter(actor, action)

end

function Skill_Mon_360Degree_Shot_OnEnter(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end
	
	actor:ResetAnimation()
	
	if action:GetActionParam() == AP_FIRE then
		Skill_Mon_360Degree_Shot_OnCastingCompleted(actor,action)
	else
		if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then
			action:SetSlot(1)
		end
		
		actor:ResetAnimation()
		actor:PlayCurrentSlot()
	end
	
	--	돌면서 이동해야한다면
	if action:GetScriptParam("MOVE")=="TRUE" then
		local	kTargetPos =  nil
		if IsSingleMode() then
			kTargetPos = GetMyActor():GetPos()
		else
			local	kPacket = action:GetParamAsPacket();
			if kPacket == nil or kPacket:IsNil() then
				ODS("[SkillError] Skill_Mon_360Degree_Shot_OnEnter, kPacket is not valid. actor : "..actor:GetPilotGuid():GetString().." action:"..action:GetID().."\n")
				return	false
			else
				kTargetPos = Point3(0,0,0)
				kTargetPos:SetX(kPacket:PopFloat())
				kTargetPos:SetY(kPacket:PopFloat())
				kTargetPos:SetZ(kPacket:PopFloat())
			end
			
		end
		
		action:SetParamAsPoint(0,kTargetPos);
		
		local	kMoveDir = kTargetPos:_Subtract(actor:GetPos())
		kMoveDir:SetZ(0)
		kMoveDir:Unitize()
		
		action:SetParamAsPoint(1,kMoveDir)
	
	end	

	return true
end
function Skill_Mon_360Degree_Shot_OnCastingCompleted(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	action:SetSlot(2)
	actor:ResetAnimation()
	actor:PlayCurrentSlot()
	
	Skill_Mon_360Degree_Shot_Fire(actor,action)	
end
function  Skill_Mon_360Degree_Shot_OnTargetListModified(actor,action,bIsBefore)
--    ODS("Skill_Mon_360Degree_Shot_OnTargetListModified\n", false, 150);
    
    if bIsBefore == false then
        --  때린다.
        Actor_Mob_Base_DoDefaultMeleeAttack(actor,action);   --  타격 효과
        action:GetTargetList():ApplyActionEffects();    
    end
end

function Skill_Mon_360Degree_Shot_Fire(actor,action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	--	시작 시간 기록
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(0,g_world:GetAccumTime())	--	Rotation Start Time
	action:SetParamFloat(1,actor:GetLookingDir():GetX());
	action:SetParamFloat(2,actor:GetLookingDir():GetY());
	action:SetParamFloat(3,actor:GetLookingDir():GetZ());
	
	action:SetParamInt(4,0);
	
	local fireStartNode	= actor:GetAnimationInfo("FIRE_EFFECT_NODE")	
	if fireStartNode == nil then
		fireStartNode = "char_root"
	end
	local fireEffect = actor:GetAnimationInfo("FIRE_EFFECT")	
	if fireEffect ~= nil then
		actor:AttachParticle(581,fireStartNode,fireEffect)
--		fireEffect = "e_m_Mahalka_skill_01_char_root"
	end					

	action:SetParamInt(4,0)	--회전하는데 쓸것임. 초기화
	action:SetParamInt(5,0)	--프로젝틸 아이로 쓸 것임. 초기화
end

function Skill_Mon_360Degree_Shot_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local	action = actor:GetAction()
		
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if action:GetActionParam() == AP_FIRE then
	
		local	bIsMove = action:GetScriptParam("MOVE") == "TRUE"
		local	fElapsedTime = accumTime - action:GetParamFloat(0)
		local	fTotalTime = action:GetAbil(AT_MAINTENANCE_TIME)/1000.0

		if bIsMove == false then
			
			if fElapsedTime > fTotalTime then
				return false
			end
			
		else
		
			local	kMovingDir = action:GetParamAsPoint(1)
			local	kMovingTarget = action:GetParamAsPoint(0)
			local	fMoveSpeed = actor:GetAbil(AT_MOVESPEED)*(1+action:GetAbil(AT_R_MOVESPEED)/10000.0);
			
			kMovingDir:Multiply(fMoveSpeed)
			
			actor:SetMovingDelta(kMovingDir);		
			
			local	kCurrentPos = actor:GetPos()
			local	kCurrentDir = kMovingTarget:_Subtract(kCurrentPos)
			kCurrentDir:Unitize()
			
			local	fDot = kCurrentDir:Dot(kMovingDir)
			if fDot<0 then
				return	false
			end			
		
		end
		
		--actor:IncRotate(frameTime*3*460.0*3.141592/180.0);

		local iAngle = 30

		local iDegree = fElapsedTime/fTotalTime*360/iAngle--*3.141592/180.0 /30
		iDegree = math.floor(iDegree * iAngle)
		local Delta = iDegree - action:GetParamInt(4)
		if Delta > iAngle then
			actor:IncRotate(math.rad(Delta))
			action:SetParamInt(4, iDegree)
			actor:ResetAnimation()
			actor:PlayCurrentSlot()
		end
		
		if IsSingleMode() then
		
			if action:GetParamInt(4) == 0 and fElapsedTime>0.5 then
				Actor_Mob_Base_DoDefaultMeleeAttack(actor,action);
				action:SetParamInt(4,1);
			end
			
		end
		
	elseif action:GetActionParam() == AP_CASTING then
		if action:GetCurrentSlot() == 0 then
			if actor:IsAnimationDone() then
				action:SetSlot(1)
				actor:ResetAnimation();
				actor:PlayCurrentSlot();				
			end
		end
	
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
				action:SetActionParam(AP_FIRE)
				Skill_Mon_360Degree_Shot_OnCastingCompleted(actor,action)
			end
		end
	end
	
	return true
end

function Skill_Mon_360Degree_Shot_OnCleanUp(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	actor:DetachFrom(581) 
	local kAction = actor:GetAction();
	
	if kAction:IsNil() == false then
		kAction:ClearTargetList();
	end
end

function Skill_Mon_360Degree_Shot_OnLeave(actor, action)
	return	true
end

function Skill_Mon_360Degree_Shot_OnEvent(actor,textKey)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local kAction = actor:GetAction();
	
	if( CheckNil(nil==kAction) ) then return end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	if textKey == "hit" or textKey == "fire" then
		-- 발사 사운드 붙이기
		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end

		--발사 이펙트 붙이기
		local fireEffect = actor:GetAnimationInfo("FIRE_EFFECT")	
		if fireEffect ~= nil then
			local fireStartNode	= actor:GetAnimationInfo("FIRE_EFFECT_NODE")	
			
			if fireStartNode == nil then
				fireStartNode = "p_ef_heart"
			end
			
			actor:AttachParticle(231, fireStartNode, fireEffect)
		end

		--거리 얻어오기
		local	iAttackRange = kAction:GetSkillRange(0,actor);
		
		if iAttackRange == 0 then
			iAttackRange = 200
		end
		
--		local	kProjectileMan = GetProjectileMan();

		local	iProjectileIndex = kAction:GetParamInt(5)

			local	kArrow = Act_Mon_RangeAttack_LoadToWeapon(actor,kAction,iProjectileIndex);
			if kArrow == nil or kArrow:IsNil() then
				return
			end

			local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
			local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치
			--WriteToConsole("________MovingDir : " .. kMovingDir:GetX() .. ", " .. kMovingDir:GetY() .. ", " .. kMovingDir:GetZ() .. "\n")
			kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp())
			kMovingDir:Multiply(iAttackRange)
			kMovingDir:Add(arrow_worldpos)
			kArrow:SetTargetLoc(kMovingDir)
			kArrow:Fire()
			iProjectileIndex=iProjectileIndex+1;
--			kAction:SetParamInt(5,iProjectileIndex)
--		end
--		kAction:ClearTargetList();
	end

	return	true
end

function	Skill_Mon_360Degree_Shot_LoadToWeapon(actor,action,iProjectileIndex)
	return Act_Mon_RangeAttack_LoadToWeapon(actor,action,iProjectileIndex)
end
