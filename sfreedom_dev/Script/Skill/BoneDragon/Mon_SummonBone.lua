local MAX_SUMMON_BONE_LINE_COUNT = 15;
local MAX_SUMMON_BONE_SELECT_COUNT = 3;

function Act_Mon_SummonBone_OnCheckCanEnter(actor, action)
end

function Act_Mon_SummonBone_SetState(actor,action,kState)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iNewState = -1;
	if kState == "BATTLEIDLE_START" then
		if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then
			return	false	
		end
				
		action:SetSlot(0)
		iNewState = 0;
	
	elseif kState == "BATTLEIDLE_LOOP" then
		action:SetSlot(1)
		if actor:GetAnimationLength(action:GetSlotAnimName(1)) == 0 then
		
			action:SetSlot(3);	--	배틀 Idle 모션이 없을 때는 그냥 Idle 모션을 하자.
		end
		iNewState = 1;
		
	elseif kState == "FIRE" then
	
		action:SetSlot(2)
		iNewState = 2;
	elseif kState == "RETURN" then
	
		action:SetSlot(4)
		iNewState = 3;

	end
	
	actor:ResetAnimation();
	actor:PlayCurrentSlot();
	action:SetParamInt(0,iNewState)
	
	return	true;

end

function Act_Mon_SummonBone_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local kPacket = action:GetParamAsPacket()
	if kPacket:IsNil() then
		return true
	end
	
	action:SetParamInt(820, 0)
	action:SetParamInt(821, 0)
	actor:ResetAnimation()
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_SummonBone_OnCastingCompleted(actor,action)
	else
		if( Act_Mon_SummonBone_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_SummonBone_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
	end
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local kTopLeft = g_world:GetObjectPosByName("breath_L_top")		-- left top
	local kTopRight = g_world:GetObjectPosByName("breath_R_top")	-- right top
	local kBottomLeft = g_world:GetObjectPosByName("breath_L_bottom")	-- left bottom
	local kBottomRight = g_world:GetObjectPosByName("breath_R_bottom")	-- right bottom

	local kUpperBase = Point3((kTopRight:GetX()+kTopLeft:GetX())/2, (kTopRight:GetY()+kTopLeft:GetY())/2, (kTopRight:GetZ()+kTopLeft:GetZ())/2)
	local kRightBase = Point3((kTopRight:GetX()+kBottomRight:GetX())/2, (kTopRight:GetY()+kBottomRight:GetY())/2, (kTopRight:GetZ()+kBottomRight:GetZ())/2)
	local kBottomBase = Point3((kBottomRight:GetX()+kBottomLeft:GetX())/2, (kBottomRight:GetY()+kBottomLeft:GetY())/2, (kBottomRight:GetZ()+kBottomLeft:GetZ())/2)
	local kLeftBase = Point3((kTopLeft:GetX()+kBottomLeft:GetX())/2, (kTopLeft:GetY()+kBottomLeft:GetY())/2, (kTopLeft:GetZ()+kBottomLeft:GetZ())/2)
	
	local fUnitLength = (kUpperBase:GetY() - kBottomBase:GetY()) / MAX_SUMMON_BONE_SELECT_COUNT
	action:SetParamFloat(200, fUnitLength)
	
	local kStartPoint = Point3(kRightBase:GetX(), kUpperBase:GetY(), kUpperBase:GetZ())
	action:SetParamAsPoint(300, kStartPoint)
	
	local ptStartPos = g_world:GetObjectPosByName(kStartPos)
	
	action:SetParamAsPoint(1, ptStartPos)
	
	local it = 0
	while it < MAX_SUMMON_BONE_LINE_COUNT do
		local jt = 0
		while jt < MAX_SUMMON_BONE_SELECT_COUNT-1 do
			local iPos = kPacket:PopInt()
			ODS("aa   "..iPos.."\n",false,987)
			action:SetParamInt(100 + (it*(MAX_SUMMON_BONE_SELECT_COUNT-1) + jt), iPos)
			jt = jt + 1
		end
		it = it + 1
	end
	
	action:DeleteParamPacket()
	
	Act_Mon_SummonBone_CastingEffect(actor, action)
	
	return true
end
function Act_Mon_SummonBone_OnCastingCompleted(actor,action)	

	Act_Mon_SummonBone_SetState(actor,action,"FIRE")
	
	local kUseKey = actor:GetAnimationInfo("USE_EVENT_KEY")
	if nil~=kUseKey then
		kUseKey = tonumber(kUseKey)
		action:SetParamInt(800, kUseKey)
	end

	if nil==kUseKey and 0==kUseKey then
		Act_Mon_SummonBone_Fire(actor,action);
	end
	
end

function Act_Mon_SummonBone_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParamInt(820, 1)
	--Act_Mon_SummonBone_FireEffect(actor, action)
	--GetActionResultCS(action, actor)

end

function Act_Mon_SummonBone_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then
	
		if actor:IsAnimationDone() == true then
			return false
		end
		
	elseif action:GetActionParam() == AP_CASTING then
		
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_SummonBone_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
	
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_SummonBone_OnCastingCompleted(actor,action)
			
			end
		end
	
	end
	
	if action:GetParamInt(820) == 1 then
		local ipos = action:GetParamInt(821)
		for i=0,1 do
			Act_Mon_SummonBone_FireEffect_Each(actor, action, ipos, i)
		end
		ipos = ipos + 1
		action:SetParamInt(821, ipos)
		if ipos >= MAX_SUMMON_BONE_LINE_COUNT then
			action:SetParamInt(820, 0)
		end
	end
	
	return true
end
function Act_Mon_SummonBone_OnCleanUp(actor, action)

end

function Act_Mon_SummonBone_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kCurAction = actor:GetAction();
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

function Act_Mon_SummonBone_OnTargetListModified(actor,action,bIsBefore)
	Act_Melee_Default_OnTargetListModified(actor,action,bIsBefore)
end

function Act_Mon_SummonBone_CastingEffect(actor, action)
	local fUnitLen = action:GetParamFloat(200)
	local fHalfLen = fUnitLen / 2
	local kStartPoint = action:GetParamAsPoint(300)

	local kCastEff = actor:GetAnimationInfo("CAST_EFFECT")
	if nil==kCastEff or ""==kCastEff then
		kCastEff = "EF_Pattern_BoneDragon_skill_01_char_root"
	end

	local kScale = actor:GetAnimationInfo("CAST_EFFECT_SCALE")
	if nil==kScale or ""==kScale then
		kScale = 0.55
	end

	kScale = tonumber(kScale)
	
	local it = 0
	while it < MAX_SUMMON_BONE_LINE_COUNT do
		local jt = 0
		while jt < MAX_SUMMON_BONE_SELECT_COUNT-1 do
			local iIndex = it*(MAX_SUMMON_BONE_SELECT_COUNT-1) + jt
			local iPos = action:GetParamInt(100 + iIndex)
			ODS(" "..iPos.."\n",false,987)
			local EffectPos = Point3(0,0,0)
			EffectPos:SetX(kStartPoint:GetX() - (it * fUnitLen) - fHalfLen)
			EffectPos:SetY(kStartPoint:GetY() - (iPos * fUnitLen) - fHalfLen)
			EffectPos:SetZ(kStartPoint:GetZ())
			GetPlayer():AttachParticleToPointS(80000 + iIndex, EffectPos, kCastEff, kScale)
			jt = jt + 1
		end
		it = it + 1
	end
end

function Act_Mon_SummonBone_FireEffect(actor, action)
	local fUnitLen = action:GetParamFloat(200)
	local fHalfLen = fUnitLen / 2
	local kStartPoint = action:GetParamAsPoint(300)

	local kGroundEff = actor:GetAnimationInfo("GROUND_EFFECT")
	if nil==kGroundEff or ""==kGroundEff then
		kGroundEff = "ef_BoneDragon_skill_01_02_char_root"
	end

	local kScale = actor:GetAnimationInfo("GROUND_EFFECT_SCALE")
	if nil==kScale or ""==kScale then
		kScale = 1.0
	end
	kScale = tonumber(kScale)
	
	local it = 0
	while it < MAX_SUMMON_BONE_LINE_COUNT do
		local jt = 0
		while jt < MAX_SUMMON_BONE_SELECT_COUNT-1 do
			local iIndex = it*(MAX_SUMMON_BONE_SELECT_COUNT-1) + jt
			local iPos = action:GetParamInt(100 + iIndex)
			local EffectPos = Point3(0,0,0)
			EffectPos:SetX(kStartPoint:GetX() - (it * fUnitLen) - fHalfLen)
			EffectPos:SetY(kStartPoint:GetY() - (iPos * fUnitLen) - fHalfLen)
			EffectPos:SetZ(kStartPoint:GetZ())
			GetPlayer():AttachParticleToPointS(90000 + iIndex, EffectPos, kGroundEff, kScale)
			jt = jt + 1
		end
		it = it + 1
	end
end

function Act_Mon_SummonBone_FireEffect_Each(actor, action, it, jt)
	local fUnitLen = action:GetParamFloat(200)
	local fHalfLen = fUnitLen / 2
	local kStartPoint = action:GetParamAsPoint(300)

	local kGroundEff = actor:GetAnimationInfo("GROUND_EFFECT")
	if nil==kGroundEff or ""==kGroundEff then
		kGroundEff = "ef_BoneDragon_skill_01_02_char_root"
	end

	local kScale = actor:GetAnimationInfo("GROUND_EFFECT_SCALE")
	if nil==kScale or ""==kScale then
		kScale = 1.0
	end
	kScale = tonumber(kScale)
	
	local iIndex = it*(MAX_SUMMON_BONE_SELECT_COUNT-1) + jt
	local iPos = action:GetParamInt(100 + iIndex)
	local EffectPos = Point3(0,0,0)
	EffectPos:SetX(kStartPoint:GetX() - (it * fUnitLen) - fHalfLen)
	EffectPos:SetY(kStartPoint:GetY() - (iPos * fUnitLen) - fHalfLen)
	EffectPos:SetZ(kStartPoint:GetZ())
	GetPlayer():AttachParticleToPointS(90000 + iIndex, EffectPos, kGroundEff, kScale)
	
end

function Act_Mon_SummonBone_OnEvent(actor,textKey)

    if textKey == "hit" or textKey == "fire" then
		
		CheckNil(nil==actor);
		CheckNil(actor:IsNil());		
	
		local FireEffect = actor:GetAnimationInfo("FIRE_EFFECT2")
		if nil~=FireEffect and ""~=FireEffect then
			local FireEffectNode = actor:GetAnimationInfo("FIRE_EFFECT_NODE2")
			if nil==FireEffectNode or ""==FireEffectNode then
				FireEffectNode = "char_root"
			end
			--actor:AttachParticle(3252, FireEffectNode ,FireEffect)
		end
		
		Act_Mon_QuakeCamera(actor)
		
        local   action = actor:GetAction();
		
		CheckNil(nil==action);
		CheckNil(action:IsNil());
		
		Act_Mon_Melee_DoDamage(actor,action)

		local kUseKey = action:GetParamInt(800)
		if 0~=kUseKey then
			Act_Mon_SummonBone_Fire(actor,action);
		end
	end
	
	return true;
end
