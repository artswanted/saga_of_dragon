-- emotion 
function Act_Single_Emotion_SingleStage_OnCheckCanEnter(actor, action)
	CheckNil(actor==nil)	
	CheckNil(actor:IsNil())
	if actor:IsMeetFloor() == false then
		return	false
	end
	if( IsJobSkill( actor:GetAction() ) ) then
		return false;
	end
	
	local actionID = action:GetID()

	if actionID=="a_Emotion_Wedding" then
		if actor:CheckStatusEffectExist("se_wedding_male") or
			actor:CheckStatusEffectExist("se_wedding_female") then
			return true
		end

		return false
	end
	
	return	true;
end
function Act_Single_Emotion_SingleStage_OnEnter(actor, action)
	CheckNil(actor==nil)
	CheckNil(actor:IsNil())
	actor:Stop()
	CheckNil(action==nil)
	CheckNil(action:IsNil())
	local actionID = action:GetID()
	local iSlotCount = action:GetSlotCount()
	action:SetSlot(math.random(0, iSlotCount))
	action:SetDoNotBroadCast(true)

	if actionID ~= "a_rush" then	-- 돌격의 경우에는 무기를 숨기지 않는다.
		local bHide = true
		if 2==actor:GetAbil(AT_GENDER) then	--여자이고
			if actionID=="a_Emotion_Wedding" then	--웨딩이모션일 경우 무기를 숨기지 않음(부케때문)
				bHide = false
			end
		end

		if bHide then
			actor:HideParts(6, true)
			action:SetParam(6,"HideParts")
		end
	end
	-- 보조캐릭터
	DoSubActorAction( actor, action:GetID(), action:GetDirection() )
	
	return true
end

function Act_Single_Emotion_SingleStage_OnUpdate(actor, accumTime, frameTime)
	CheckNil(actor==nil)
	CheckNil(actor:IsNil())
	if actor:IsAnimationDone() == true then
		return false
	end
	return true
end

function Act_Single_Emotion_SingleStage_OnLeave(actor, action)
	local actionID = action:GetID()
	if false == action:GetEnable() or "a_jump" == action:GetID() then
		return false
	end
	--if actor:IsAnimationDone() == true then
		return true
	--end

	--return false
end

function Act_Single_Emotion_SingleStage_OnCleanUp(actor, action)
	CheckNil(actor==nil)
	CheckNil(actor:IsNil())
		
	local paramValue = actor:GetAction():GetParam(6)
	local actionID = actor:GetAction():GetID()
	if actionID ~= "a_rush" and paramValue == "HideParts" then				
		actor:HideParts(6, false)
		actor:GetAction():SetParam(6,"")
	end

	DoSubActorAction( actor, "a_twin_sub_trace_ground", action:GetDirection() )	
	return true
end


function Act_Single_Emotion_ThreeStage_OnCheckCanEnter(actor, action)
	CheckNil(actor==nil)
	CheckNil(actor:IsNil())
	if actor:IsMeetFloor() == false then
		return	false
	end
	if( IsJobSkill( actor:GetAction() ) ) then
		return false;
	end
	return	true;
end
function Act_Single_Emotion_ThreeStage_OnEnter(actor, action)
	CheckNil(action==nil)
	CheckNil(action:IsNil())
	local actionID = action:GetID()
	local iSlotCount = action:GetSlotCount()
		
	if iSlotCount ~= 3  then
		return false
	end
	CheckNil(actor==nil)
	CheckNil(actor:IsNil())
	actor:HideParts(6, true)
	if actionID == "a_sleep" then
		action:SetSlot(1)
	end
	action:SetDoNotBroadCast(true)
	actor:Stop()
	
	-- 보조캐릭터	
	DoSubActorAction( actor, action:GetID(), action:GetDirection() )
	return true
end

function Act_Single_Emotion_ThreeStage_OnUpdate(actor, accumTime, frameTime)
	CheckNil(actor==nil)
	CheckNil(actor:IsNil())
	local action = actor:GetAction()
	CheckNil(action==nil)
	CheckNil(action:IsNil())
	local iCurrentSlot = action:GetCurrentSlot()
	local bIsAnimDone = actor:IsAnimationDone()

	if iCurrentSlot == 0 and
		bIsAnimDone == true then
		actor:PlayNext()
	elseif iCurrentSlot == 1 then
		return true
	elseif iCurrentSlot == 2 and
		bIsAnimDone == true then
		return false
	end

	return true
end

function Act_Single_Emotion_ThreeStage_OnLeave(actor, action)
	if false == action:GetEnable() or "a_jump" == action:GetID() then		
		return false
	end
	return true
--[[	
	local action = actor:GetAction()
	local iCurrentSlot = action:GetCurrentSlot()
	local bIsAnimDone = actor:IsAnimationDone()
	local kNextActionName = action:GetID()

	if iCurrentSlot == 1 then
		actor:PlayNext()
		return false
	elseif iCurrentSlot == 2 and 
		bIsAnimDone == true then
		return true
	end

	return false
]]	
end

function Act_Single_Emotion_ThreeStage_OnCleanUp(actor, action)
	CheckNil(actor==nil)
	CheckNil(actor:IsNil())
	actor:HideParts(6, false)
	DoSubActorAction( actor, "a_twin_sub_trace_ground", action:GetDirection() )		
end

function DoSubActorAction(kActor, kActionName, iDir)
	if( kActor==nil ) then return end
	if( kActor:IsNil() ) then return end	
	-- 격투가 보조 캐릭터에게 액션을 시킴
	if( true == kActor:IsMyActor() ) then
		local iBaseActorType = kActor:GetPilot():GetBaseClassID()
		if(iBaseActorType == CT_DOUBLE_FIGHTER) then
			local kSubActor = kActor:GetSubPlayer()
			if( false == kSubActor:IsNil() ) then
				TryMustChangeSubPlayerAction(kActor, kActionName, iDir, 0, true)
			end
		end
	end
end
	
function SubActorHide(kOwnerActor, bHide)
	if( kOwnerActor==nil ) then return end
	if( kOwnerActor:IsNil() ) then return end
	-- 격투가 보조 캐릭터에게 액션을 시킴
	local iBaseActorType = kOwnerActor:GetPilot():GetBaseClassID()
	if(iBaseActorType == CT_DOUBLE_FIGHTER) then
		local kSubActor = kOwnerActor:GetSubPlayer()
		if( false == kSubActor:IsNil() ) then
			kSubActor:SetHide(bHide)
			kSubActor:SetHideShadow(bHide)
		end
	end
end