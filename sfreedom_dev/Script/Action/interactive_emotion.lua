-- emotion 

function Set_InteractiveEmotion_TargetActor(kActor, kTargetGUID, bRequestActor)
	if( CheckNil(nil==kActor) ) then return false end
	if( CheckNil(kActor:IsNil()) ) then return false end
	
	if( CheckNil(nil==kTargetGUID) ) then return false end
	if( CheckNil(kTargetGUID:IsNil()) ) then return false end
		
	kActor:SetParam(INTERACTIVE_EMOTION_TARGET_GUID_INDEX, kTargetGUID:GetString())
	if(bRequestActor) then
		kActor:SetParam(INTERACTIVE_EMOTION_TARGET_GUID_INDEX+1, "REQ_ACTOR") -- 상호 이모션 요청자
	else
		kActor:SetParam(INTERACTIVE_EMOTION_TARGET_GUID_INDEX+1, "REQED_ACTOR") -- 상호 이모션 요청 받은자
	end
end

function Get_InteractiveEmotion_TargetActor(kActor)
	if( CheckNil(nil==kActor) ) then return false end
	if( CheckNil(kActor:IsNil()) ) then return false end
	if( CheckNil(nil==g_world) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	--local kActor = g_pilotMan:GetPlayerActor()
	local StrTargetGuid = kActor:GetParam(INTERACTIVE_EMOTION_TARGET_GUID_INDEX)
	
	if("" ~= StrTargetGuid and nil ~= StrTargetGuid) then
		local kTargetGuid = GUID(StrTargetGuid)		
		if false== kTargetGuid:IsNil() then
			local kTargetActor = g_world:FindActor(kTargetGuid)
			if false==kTargetActor:IsNil() then
				return kTargetActor;
			end
		end
	end

	return nil
end

function Interactive_Emotion_SingleStage_OnCheckCanEnter(actor, action)	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if( CheckNil(nil==g_world) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	if not actor:IsUnderMyControl() then
		return true
	end

	if actor:IsMeetFloor() == false then
		return	false
	end

	local kPilot = actor:GetPilot()

	local kTargetActor = Get_InteractiveEmotion_TargetActor(actor)
	if nil==kTargetActor or kTargetActor:IsNil() then
		--커플은 있는데 같은 필드에 없음
		if action:GetID() ~= "a_Emotion_rest07" then
			AddWarnDataTT(450081)
		end
		return false
	end

	CheckNil(action==nil)
	if( CheckNil(action:IsNil()) ) then return false end
	
	if action:GetScriptParam("CHECK_SAME_GENDER") == "TRUE" then
		if(actor:GetAbil(AT_GENDER) == kTargetActor:GetAbil(AT_GENDER)) then
		-- 동성끼리는 할수 없음
			AddWarnDataTT(790904)
			return false
		end
	end

	local kMyPos = actor:GetTranslate()
	local kTargetPos = kTargetActor:GetTranslate()
	local fDist = kMyPos:Distance(kTargetPos)

	if action:GetSkillRange(action:GetActionNo(), actor) < fDist then
		--같은 필드에 있는데 거리가 너무 멈
		if action:GetID() ~= "a_Emotion_rest07" then
			AddWarnDataTT(450081)
		end		
		return false
	end

	local kResult = Interactive_Emotion_FindTargetPos(actor, kTargetActor, action)
	if nil==kResult or kResult:IsZero() then
		return false 
	end

	local fZDelta = math.abs(kResult:GetZ() - kTargetPos:GetZ())
	if 2<fZDelta then
		local fHigherZ = math.max(kResult:GetZ(), kTargetPos:GetZ())
		local fZDeltaMy = kResult:GetZ()-fHigherZ
		kResult:SetZ(fHigherZ)
		local fZDeltaTarget = kTargetPos:GetZ()-fHigherZ
		kTargetPos:SetZ(fHigherZ)

		action:SetParamFloat(14, math.abs(fZDeltaMy))	--높이 차이
		action:SetParamFloat(15, math.abs(fZDeltaTarget))	--높이 차이
	end

	local kDir = kResult:_Subtract(kTargetPos)
	local kDist = kResult:Distance(kTargetPos)+10
	kDir:Unitize()

	local kGroundPos = g_world:ThrowRay(kTargetPos, kDir, kDist, 2, 256)	--중간에 움직이는 벽 검사
	if -1~=kGroundPos:GetX() and -1~=kGroundPos:GetY() and -1~=kGroundPos:GetZ() then
		return false
	end

	kDir = kTargetPos:_Subtract(kResult)
	kDir:Unitize()
	kGroundPos = g_world:ThrowRay(kResult, kDir, kDist, 2, 256)	
	if -1~=kGroundPos:GetX() and -1~=kGroundPos:GetY() and -1~=kGroundPos:GetZ() then
		return false
	end
	
	action:SetParamAsPoint(12, kResult)	--목표위치
	action:SetParamAsPoint(13, kTargetPos)	--타겟위치

	return	true;
end

function Interactive_Emotion_SingleStage_OnOverridePacket(actor,action,packet)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	packet:PushPoint3(action:GetParamAsPoint(12))
	packet:PushPoint3(action:GetParamAsPoint(13))

	local target = actor:GetParam(INTERACTIVE_EMOTION_TARGET_GUID_INDEX)
	local targetuid = GUID(target)
	packet:PushGuid(targetuid)
end

function Interactive_Emotion_SingleStage_OnEnter(actor, action)	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if not actor:IsUnderMyControl() then
	end
	
	actor:Stop()
	
	local actionID = action:GetID()
	local iSlotCount = action:GetSlotCount()
	action:SetSlot(0)
	
	actor:HideParts(6, true)

	action:SetParamInt(0,0)	--정상이면 0

	local kPilot = actor:GetPilot()
	local ReturnValue = true
	local kTargetActor = Get_InteractiveEmotion_TargetActor(actor)
	if nil == kTargetActor or kTargetActor:IsNil() then	-- 제 3자가 봤을때 타겟이 small area를 벗어나서 얻어오지 못할수 있음
		action:SetParamInt(0,1)		
		ReturnValue = false
		--return false
	end

--	local kTargetActor1 = Get_InteractiveEmotion_TargetActor(GetMyActor())
	
	
	local	kPacket = action:GetParamAsPacket()
	local kMyPos = action:GetParamAsPoint(12)
	local kTargetPos = action:GetParamAsPoint(13)

	if kPacket == nil or kPacket:IsNil() then	--내가 캐스팅
		CheckNil(kPilot==nil)	
		CheckNil(kPilot:IsNil())
		if false==kTargetActor:IsNil() then
			actor:FreeMove(2<action:GetParamFloat(14))
			kTargetActor:FreeMove(2<action:GetParamFloat(15))
			kTargetActor:LockBidirection(false)
			kTargetActor:ReserveTransitAction(actionID.."2")
			actor:SetTranslate(kMyPos)
			kTargetActor:SetTranslate(kTargetPos) 

			actor:IncTimeToAniObj(0)
			kTargetActor:IncTimeToAniObj(0)
		else
			action:SetParamInt(0,1)	--정상이면 0
		end

		actor:LockBidirection(false)
		actor:SeeFront(true, true)
		kTargetActor:SeeFront(true, true)
	else
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local kPos = Point3(kPacket:PopFloat(),kPacket:PopFloat(),kPacket:PopFloat())
		local kMyPosbyPacket = Point3(kPacket:PopFloat(),kPacket:PopFloat(),kPacket:PopFloat())
		
		if false==ReturnValue then	--위에서 못찾았으면
			local targetguid = kPacket:PopGuid()
			if targetguid:IsNil() then
				return false
			end

			kTargetActor = g_world:FindActor(targetguid)
			Set_InteractiveEmotion_TargetActor(actor, targetguid, true)
			Set_InteractiveEmotion_TargetActor(kTargetActor, actor:GetPilotGuid(), true)
			action:SetParamInt(0,0)
			kTargetPos = kPos
			kMyPos = kMyPosbyPacket
		end
		if kPos:IsZero() or kMyPosbyPacket:IsZero() then
			action:SetParamInt(0,1)
		else

			if kTargetActor:IsNil() then
				action:SetParamInt(0,1)
			else				
				actor:FreeMove(2<action:GetParamFloat(14))
				kTargetActor:FreeMove(2<action:GetParamFloat(15))

				actor:SetTranslate(kPos)
				actor:LockBidirection(false)
				kTargetActor:LockBidirection(false)
				kTargetActor:SetTranslate(kMyPosbyPacket)
				-------------
				local kTargetAction = kTargetActor:GetAction()
				kTargetAction:ChangeToNextActionOnNextUpdate(true,true);
				-------------
				kTargetActor:ReserveTransitAction(actionID.."2")
				actor:IncTimeToAniObj(0)
				kTargetActor:IncTimeToAniObj(0)
				kTargetPos = kPos
				kMyPos = kMyPosbyPacket
			end
		end
		actor:SeeFront(true, true)
		kTargetActor:SeeFront(true, true)
	end

	local kDir = kMyPos:_Subtract(kTargetPos)
	kDir:Multiply(0.5)
	kTargetPos:Add(kDir)

--	actor:AttachParticleToPoint(774, kTargetPos, "ef_interact_emotion_start_01")
	actor:AttachParticleToPoint(775, kTargetPos, "ef_interact_emotion_idle_01");
	actor:AttachSound(778, "Emotion_Couple_Heart")

	if action:GetID() == "a_Emotion_rest07" then

		actor:SeeFront(true, true)

		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		
		local ptTargetPos = kTargetPos;
		local TargetPos = g_world:ThrowRay(Point3(ptTargetPos:GetX(),ptTargetPos:GetY(),ptTargetPos:GetZ()+50.0),Point3(0,0,-1),500)
		actor:AttachParticleToPointWithRotate(99000, TargetPos, "ef_Rest09", actor:GetRotateQuaternion(), 1.0);

		local Gender = actor:GetAbil(AT_GENDER)
		if Gender == 1 then
			kPosition = actor:GetParticleNodeWorldPos(99000, "char_man")
		else
			kPosition = actor:GetParticleNodeWorldPos(99000, "char_women")
		end

		kPosition:SetZ(kPosition:GetZ() + 25)

		actor:FreeMove(true);
		actor:StopJump();
		actor:SetNoWalkingTarget(false);
		actor:ResetAnimation();			
		actor:SetTranslate(kPosition)
		actor:Stop()
		
		actor:SetParam(99123, actor:GetStartParamID("a_Emotion_rest07"));
--[[
		local kTargetActor = Get_InteractiveEmotion_TargetActor(actor)
		if nil==kTargetActor or kTargetActor:IsNil() then
			return false
		end
	]]
		Gender = kTargetActor:GetAbil(AT_GENDER)
		if Gender == 1 then
			kPosition = actor:GetParticleNodeWorldPos(99000, "char_man")
		else
			kPosition = actor:GetParticleNodeWorldPos(99000, "char_women")
		end

		kPosition:SetZ(kPosition:GetZ() + 25)

		kTargetActor:FreeMove(true);
		kTargetActor:StopJump();
		kTargetActor:SetNoWalkingTarget(false);
		kTargetActor:ResetAnimation();	
		
		kTargetActor:SetTranslate(kPosition)

		kTargetActor:Stop()

		kTargetActor:SetParam(99123, actor:GetStartParamID("a_Emotion_rest07"));
	end
	
	SubActorHide(actor, true)
	return true
end

function Interactive_Emotion_SingleStage_OnUpdate(actor, accumTime, frameTime)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local action = actor:GetAction()
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local kParam0 = action:GetParamInt(0)
	if --[[actor:IsAnimationDone() == true or]] 0~=kParam0 then
		return false
	end
	
	local kTargetActor = Get_InteractiveEmotion_TargetActor(actor)
	if nil==kTargetActor or kTargetActor:IsNil() then
		return false
	end

	if action:GetID()=="a_Emotion_rest07"  then
		local Gender = actor:GetAbil(AT_GENDER)
		if Gender == 1 then
			kPosition = actor:GetParticleNodeWorldPos(99000, "char_man")
		else
			kPosition = actor:GetParticleNodeWorldPos(99000, "char_women")
		end

		kPosition:SetZ(kPosition:GetZ() + 25)
		actor:SetTranslate(kPosition)
		actor:SetHideShadow(true)

		--local kTargetActor = Get_InteractiveEmotion_TargetActor(actor)
		if nil==kTargetActor or kTargetActor:IsNil() then
			return false
		end

		Gender = kTargetActor:GetAbil(AT_GENDER)
		if Gender == 1 then
			kPosition = actor:GetParticleNodeWorldPos(99000, "char_man")
		else
			kPosition = actor:GetParticleNodeWorldPos(99000, "char_women")
		end

		kPosition:SetZ(kPosition:GetZ() + 25)
		kTargetActor:SetTranslate(kPosition)
		kTargetActor:SetHideShadow(true)
	else
		local kParam0 = action:GetParamInt(0)
		if --[[actor:IsAnimationDone() == true or]] 0~=kParam0 then
			return false
		end
		
		--local kTargetActor = Get_InteractiveEmotion_TargetActor(actor)
		if nil==kTargetActor or kTargetActor:IsNil() then
			return false
		end
	end
	
	local animDone = actor:IsAnimationDone()
	local iSlot = action:GetCurrentSlot()
	local iMaxActionSlot = action:GetSlotCount();
	
	if action:GetParam(1) == "end" then
		return	false;
	end
	
	if animDone == true then
		if(false == actor:PlayNext()) then	-- 더이상 애니메이션이 없으면
			local curActionParam = action:GetParam(0)
			if curActionParam == "GoNext" then
				action:SetParam(0, "null")
				action:SetParam(1, "end")
				return false;
			else
				action:SetParam(1, "end")
				return false
			end
		end		
	end	

	return true
end

function Interactive_Emotion_SingleStage_OnLeave(actor, action)
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if false == action:GetEnable() then
		return false
	end

	return true
end

function Interactive_Emotion_SingleStage_OnCleanUp(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local kcurAction = actor:GetAction()
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end	

	if nil~=kcurAction and false==kcurAction:IsNil() then
		actor:DetachFrom(99000)

		local kTargetActor = Get_InteractiveEmotion_TargetActor(actor)

		local kEffectID = actor:GetStartEffectSave("a_Emotion_rest07")
		if kEffectID ~= 0 and actor:IsMyActor() then
			local	kPacket = NewPacket(12130)
			kPacket:PushGuid(actor:GetPilotGuid())
			kPacket:PushInt(kEffectID)
			Net_Send(kPacket)
			DeletePacket(kPacket)
		end

		if actor:IsMyActor() then
			local kDeleteEffectID = tonumber(actor:GetParam(99123))

			local	kPacket = NewPacket(12130)
			kPacket:PushGuid(actor:GetPilotGuid())
			kPacket:PushInt(kDeleteEffectID)
			Net_Send(kPacket)
			DeletePacket(kPacket)
		end		
	end
		
	local paramValue = actor:GetAction():GetParam(6)
	local actionID = actor:GetAction():GetID()
	
	actor:HideParts(6, false)

	actor:FreeMove(false)

	local HeadString = "a_interactive"

	local kTargetActor = Get_InteractiveEmotion_TargetActor(actor)
	if nil~=kTargetActor and false==kTargetActor:IsNil() then
		local kTargetAction = kTargetActor:GetAction()
		if false==kcurAction:IsNil() and false==kTargetAction:IsNil() then
			if nil~=string.find(kcurAction:GetID(), HeadString) and nil~=string.find(kTargetAction:GetID(), HeadString) then
				if string.len(kcurAction:GetID()) < string.len(kTargetAction:GetID()) then
					if nil~=string.find(kTargetAction:GetID(),kcurAction:GetID()) then
						if nil==string.find(action:GetID(), HeadString) then
							kTargetActor:ReserveTransitAction("a_idle")
						end
					end
				else
					if nil~=string.find(kcurAction:GetID(),kTargetAction:GetID()) then
						if nil==string.find(action:GetID(), HeadString) then
							kTargetActor:ReserveTransitAction("a_idle")
						end
					end
				end
			end
		end
		kTargetActor:FreeMove(false)
	end

	if nil~=kTargetActor and false==kTargetActor:IsNil() then
		if actor:GetAction():GetID() == "a_Emotion_rest07" then
			kTargetActor:ReserveTransitAction("a_idle")
			kTargetActor:SetParam(INTERACTIVE_EMOTION_TARGET_GUID_INDEX, "")
			kTargetActor:SetParam(INTERACTIVE_EMOTION_TARGET_GUID_INDEX+1, "")
		end
	end
	actor:DetachFrom(775)
	actor:RestoreLockBidirection()

	actor:SetParam(INTERACTIVE_EMOTION_TARGET_GUID_INDEX, "")
	actor:SetParam(INTERACTIVE_EMOTION_TARGET_GUID_INDEX+1, "")
	
	SubActorHide(actor, false)
end

function Interactive_Emotion_SingleStage_OnEvent(actor,textKey)
end

--이모션을 당하는 쪽이 쓰는 액션
function Interactive_Emotion_SingleStage2_OnCheckCanEnter(actor, action)
	return true
end
function Interactive_Emotion_SingleStage2_OnEnter(actor, action)	
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if not actor:IsUnderMyControl() then
	end

	actor:HideParts(6, true)
	if nil~=string.find(action:GetID(), "a_emotion_rest07") then
		actor:FreeMove(true)
	end
	SubActorHide(actor, true)
	return true
end

function Interactive_Emotion_SingleStage2_OnUpdate(actor, accumTime, frameTime)	
	if not actor:IsUnderMyControl() then
	end
	return Interactive_Emotion_SingleStage_OnUpdate(actor, accumTime, frameTime)
end

function Interactive_Emotion_SingleStage2_OnLeave(actor, action)
	if not actor:IsUnderMyControl() then
	end
	return Interactive_Emotion_SingleStage_OnLeave(actor, action)
end

function Interactive_Emotion_SingleStage2_OnCleanUp(actor, action)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local kCurAction = actor:GetAction()
	if( CheckNil(nil==kCurAction) ) then return false end
	if( CheckNil(kCurAction:IsNil()) ) then return false end
		
	local actionID = actor:GetAction():GetID()

	actor:HideParts(6, false)

	actor:RestoreLockBidirection();
	actor:FreeMove(false)

	local HeadString = "a_emotion_"

	local kTargetActor = Get_InteractiveEmotion_TargetActor(actor)
	if nil~=kTargetActor and false==kTargetActor:IsNil() then
		if nil==string.find(action:GetID(), HeadString) then
			kTargetActor:ReserveTransitAction("a_idle")
		end
	end

	if actor:IsMyActor() then
		local kDeleteEffectID = tonumber(actor:GetParam(99123))

		local	kPacket = NewPacket(12130)
		kPacket:PushGuid(actor:GetPilotGuid())
		kPacket:PushInt(kDeleteEffectID)
		Net_Send(kPacket)
		DeletePacket(kPacket)
	end
	SubActorHide(actor, false)
end

function Interactive_Emotion_SingleStage2_OnEvent(actor,textKey)
	Interactive_Emotion_SingleStage_OnEvent(actor,textKey)
end

function Interactive_Emotion_ThreeStage_OnCheckCanEnter(actor, action)
	return Interactive_Emotion_SingleStage_OnCheckCanEnter(actor, action)
end
function Interactive_Emotion_ThreeStage_OnEnter(actor, action)
	return Interactive_Emotion_SingleStage_OnEnter(actor, action)
end

function Interactive_Emotion_ThreeStage_OnUpdate(actor, accumTime, frameTime)
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local action = actor:GetAction()
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local kParam0 = action:GetParamInt(0)
	local iCurrentSlot = action:GetCurrentSlot()
	local bIsAnimDone = actor:IsAnimationDone()


	if iCurrentSlot == 0 and bIsAnimDone == true then
		actor:PlayNext()
	elseif iCurrentSlot == 1 then

	elseif iCurrentSlot == 2 and bIsAnimDone == true then
		return false
	end

	local kTargetActor = Get_InteractiveEmotion_TargetActor(actor)
	if nil==kTargetActor or kTargetActor:IsNil() then
		return false
	end

	return 0==kParam0
end

function Interactive_Emotion_ThreeStage_OnLeave(actor, action)
	return Interactive_Emotion_SingleStage_OnLeave(actor, action)
end

function Interactive_Emotion_ThreeStage_OnCleanUp(actor, action)
	Interactive_Emotion_SingleStage_OnCleanUp(actor, action)
end

function Interactive_Emotion_ThreeStage_OnOverridePacket(actor,action,packet)
	Interactive_Emotion_SingleStage_OnOverridePacket(actor,action,packet)
end

function Interactive_Emotion_ThreeStage_OnEvent(actor,textKey)
	Interactive_Emotion_SingleStage_OnEvent(actor,textKey)
end

--이모션을 당하는 쪽이 쓰는 액션
function Interactive_Emotion_ThreeStage2_OnCheckCanEnter(actor, action)
	return true
end
function Interactive_Emotion_ThreeStage2_OnEnter(actor, action)
	return Interactive_Emotion_SingleStage2_OnEnter(actor, action)
end

function Interactive_Emotion_ThreeStage2_OnUpdate(actor, accumTime, frameTime)
	return Interactive_Emotion_ThreeStage_OnUpdate(actor, accumTime, frameTime)
end

function Interactive_Emotion_ThreeStage2_OnLeave(actor, action)
	return Interactive_Emotion_ThreeStage_OnLeave(actor, action)
end

function Interactive_Emotion_ThreeStage2_OnCleanUp(actor, action)
	Interactive_Emotion_SingleStage2_OnCleanUp(actor, action)
end

function Interactive_Emotion_ThreeStage2_OnEvent(actor,textKey)
	Interactive_Emotion_SingleStage2_OnEvent(actor,textKey)
end

function Interactive_Emotion_FindTargetPos(actor, kTargetActor, action)	--순간이동 할 위치를 계산해 보자
	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==kTargetActor) ) then return false end
	if( CheckNil(kTargetActor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end

	if( CheckNil(nil==g_world) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local iGender = actor:GetAbil(AT_GENDER)
	local iAdjust = 1
	if 2==iGender then
		iAdjust = -1
	end
	local iPosX = 29*iAdjust--action:GetAbil(AT_POSITION_X)*iAdjust
	if action:GetID()=="a_Emotion_Hug" then
		iPosX = 27*iAdjust
	elseif action:GetID()=="a_Emotion_Kiss" then
		iPosX = 30*iAdjust
	end

	local iPosX = action:GetAbil(AT_POSITION_X)*iAdjust
	local iPosY = action:GetAbil(AT_POSITION_Y)*iAdjust
	local iZLimit = action:GetAbil(AT_DETECT_RANGE_Z)
	if 0==iZLimit then
		iZLimit = 30
	end

	local kMyPos = actor:GetTranslate()
	local kTargetPos = kTargetActor:GetTranslate()

	local kResult = Point3(0,0,0)
	local z = math.abs(kMyPos:GetZ() - kTargetPos:GetZ())
	if iZLimit <= z then
		return nil
	end

	local kDir = kMyPos:_Subtract(kTargetPos)
	local fDist = kMyPos:Distance(kTargetPos)
	kDir:Unitize()
	
	local kGroundPos = g_world:ThrowRay(kTargetPos, kDir, fDist)
	if -1~=kGroundPos:GetX() and -1~=kGroundPos:GetY() and -1~=kGroundPos:GetZ() then	--중간에 걸리는 물체가 있다
		return nil
	end

	kTargetActor:FindPathNormal(true)
	local kRightVector = kTargetActor:GetPathNormal()
	kRightVector:Unitize()
	local kPathNormal = kTargetActor:GetPathNormal()
	kRightVector:Cross(Point3(0,0,1))	--오른쪽 벡터
	kRightVector:Multiply(iPosX)
	kPathNormal:Multiply(iPosY)

	kTargetPos:Add(kRightVector)
	kTargetPos:Add(kPathNormal)
	kTargetPos:SetZ(kTargetPos:GetZ()+20)

	kGroundPos = g_world:ThrowRay(kTargetPos, Point3(0,0,-1), 100)
	if -1==kGroundPos:GetX() and -1==kGroundPos:GetY() and -1==kGroundPos:GetZ() then	--바닥이 없다
		return nil
	end

	kGroundPos:SetZ(kGroundPos:GetZ()+25)
	kResult = kGroundPos
	return kResult
end



--[[
function InteractiveEmotion_OnCheckCanEnter(actor, action)
	CheckNil(actor==nil)	
	if( CheckNil(actor:IsNil()) ) then return false end

	local kPilot = actor:GetPilot()
	if false==kPilot:IsNil() then
		if kPilot:GetCoupleGuid():IsNil() then
			--커플없음
			if action:GetID() ~= "a_Emotion_rest07" then
				AddWarnDataTT(450080)
			end
			return false 
		end
	end
	local kTargetActor = Get_InteractiveEmotion_TargetActor(actor)
	if nil==kTargetActor or kTargetActor:IsNil() thenEjr
		--커플은 있는데 같은 필드에 없음
		if action:GetID() ~= "a_Emotion_rest07" then
			AddWarnDataTT(450081)
		end
		return false
	end

	CheckNil(action==nil)
	if( CheckNil(action:IsNil()) ) then return false end

	local kMyPos = actor:GetTranslate()
	local kTargetPos = kTargetActor:GetTranslate()
	local fDist = kMyPos:Distance(kTargetPos)

	if action:GetSkillRange(action:GetActionNo(), actor) < fDist then
		--같은 필드에 있는데 거리가 너무 멈
		if action:GetID() ~= "a_Emotion_rest07" then
			AddWarnDataTT(450081)
		end
		return false
	end

	return true
end
]]
