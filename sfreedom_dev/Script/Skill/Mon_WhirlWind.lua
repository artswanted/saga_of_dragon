-- skill

function Skill_Mon_WhirlWind_OnCheckCanEnter(actor, action)
	return true
end

function Skill_Mon_WhirlWind_OnEnter(actor, action)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end	

	ODS("Skill_Mon_WhirlWind_OnEnter ActionParam : "..action:GetActionParam().."\n");
	
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end
	actor:ResetAnimation()
	
	if action:GetActionParam() == AP_FIRE then
		Skill_Mon_WhirlWind_OnCastingCompleted(actor,action)
	else
	
		if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then
			action:SetSlot(1);	
		end
		
		actor:ResetAnimation();
		actor:PlayCurrentSlot();
		
	end
	
	--	돌면서 이동해야한다면
	if action:GetScriptParam("MOVE")=="TRUE" then
	
		local	kTargetPos =  nil;
		
		if IsSingleMode() then
			kTargetPos = GetMyActor():GetPos();
		else
			local	kPacket = action:GetParamAsPacket();
			if kPacket == nil or kPacket:IsNil() then
				ODS("[SkillError] Skill_Mon_WhirlWind_OnEnter, kPacket is not valid. actor : "..actor:GetPilotGuid():GetString().." action:"..action:GetID().."\n");
				return	false;
			else
				kTargetPos = Point3(0,0,0);
				kTargetPos:SetX(kPacket:PopFloat());
				kTargetPos:SetY(kPacket:PopFloat());
				kTargetPos:SetZ(kPacket:PopFloat());
			end
			
		end
		
		action:SetParamAsPoint(0,kTargetPos);
		
		local	kMoveDir = kTargetPos:_Subtract(actor:GetPos());
		kMoveDir:SetZ(0);
		kMoveDir:Unitize();
		
		action:SetParamAsPoint(1,kMoveDir);
	
	end	

	action:SetParamInt(10,0)

	return true
end
function Skill_Mon_WhirlWind_OnCastingCompleted(actor,action)	
	
	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	
	ODS("Skill_Mon_WhirlWind_OnCastingCompleted\n");

	action:SetSlot(2);
	actor:ResetAnimation();
	actor:PlayCurrentSlot();
	
	Skill_Mon_WhirlWind_Fire(actor,action);
	
end
function  Skill_Mon_WhirlWind_OnTargetListModified(actor,action,bIsBefore)
	
	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	
    ODS("Skill_Mon_WhirlWind_OnTargetListModified\n");
    
    if bIsBefore == false then
        --  때린다.
        Actor_Mob_Base_DoDefaultMeleeAttack(actor,action);   --  타격 효과
    end
end

function Skill_Mon_WhirlWind_Fire(actor,action)
	
	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	if( CheckNil(nil==action) ) then return end
	if( CheckNil(action:IsNil()) ) then return end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	--	시작 시간 기록
	action:SetParamFloat(0,g_world:GetAccumTime())	--	Rotation Start Time
	action:SetParamFloat(1,actor:GetLookingDir():GetX());
	action:SetParamFloat(2,actor:GetLookingDir():GetY());
	action:SetParamFloat(3,actor:GetLookingDir():GetZ());
	
	action:SetParamInt(4,0);

	local kRotSpeed	= actor:GetAnimationInfo("ROT_SPEED")	
	if nil==kRotSpeed or ""==kRotSpeed then
		kRotSpeed = 3.0
	else
		kRotSpeed = tonumber(kRotSpeed)
	end

	action:SetParamFloat(6,kRotSpeed)
	
	local fireStartNode	= actor:GetAnimationInfo("FIRE_EFFECT_NODE")	
	if fireStartNode == nil then
		fireStartNode = "char_root"
	elseif fireStartNode == "NONE" then
		return
	end
	local fireEffect = actor:GetAnimationInfo("FIRE_EFFECT")	
	if fireEffect == nil then
		fireEffect = "e_m_Mahalka_skill_01_char_root" 
	elseif fireEffect == "NONE" then
		return
	end					

	actor:AttachParticle(581,fireStartNode,fireEffect);
	
--	ODS("Skill_Mon_WhirlWind_Fire OK. NODE : " .. fireStartNode .. " EFF : " .. fireEffect ..  "\n");
end

function Skill_Mon_WhirlWind_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	local	action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	
	ODS("Skill_Mon_WhirlWind_OnUpdate accumTime:"..accumTime.."\n");
	
	if action:GetActionParam() == AP_FIRE then
	
		local	bIsMove = action:GetScriptParam("MOVE") == "TRUE";
		local	fElapsedTime = accumTime - action:GetParamFloat(0);

		if 0 ~= action:GetParamInt(10) then
			local anilen = actor:GetAnimationLength(action:GetSlotAnimName(3))
			if 0 < anilen then	--리턴이 있으면		
				if anilen < accumTime - action:GetParamFloat(11) then
					return false
				end
			else
				return false
			end
			return true
		end

		if bIsMove == false then
		
			local	fTotalTime = action:GetAbil(AT_MAINTENANCE_TIME)/1000.0;
			
			ODS("Skill_Mon_WhirlWind_OnUpdate fTotalTime:"..fTotalTime.."\n");
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
					actor:DetachFrom(581)
				if 0 < actor:GetAnimationLength(action:GetSlotAnimName(3)) then
					action:SetParamInt(10,1)
					action:SetParamFloat(11,accumTime)
					action:SetSlot(3)
					actor:ResetAnimation()
					actor:PlayCurrentSlot()
					return	true
				else
					return false
				end
			end
		end

		local kRotSpeed = action:GetParamFloat(6)	
		actor:IncRotate(frameTime*kRotSpeed*460.0*3.141592/180.0);
		
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
				Skill_Mon_WhirlWind_OnCastingCompleted(actor,action)
			
			end
		end
		
	
	end
	
	ODS("Skill_Mon_WhirlWind_OnUpdate OK\n")
	return true
end

function Skill_Mon_WhirlWind_OnCleanUp(actor, action)
	
	if( CheckNil(nil==actor) ) then return end
	if( CheckNil(actor:IsNil()) ) then return end
	
	actor:DetachFrom(581)
end

function Skill_Mon_WhirlWind_OnLeave(actor, action)
	return	true
end
