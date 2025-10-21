-- emotion 

function Act_Single_Emotion_Picket_OnCheckCanEnter(actor, action)
	
	if actor:IsMeetFloor() == false then
		return	false
	end
	if( IsJobSkill( actor:GetAction() ) ) then
		return false;
	end
	return	true;
end
function Act_Single_Emotion_Picket_OnEnter(actor, action)
	CheckNil(actor==nil)
	CheckNil(actor:IsNil())
	CheckNil(action==nil)
	CheckNil(action:IsNil())
	actor:Stop()
	actor:HideParts(6, true)
	local actionID = action:GetID()
	local iSlotCount = action:GetSlotCount()
	action:SetSlot(math.random(0, iSlotCount))
	action:SetDoNotBroadCast(true)

	if actionID == "a_vote" then	-- 투표의 경우 카메라를 바라보도록 한다.					
		actor:LockBidirection(false)
		actor:SeeFront(true, true)
		actor:AttachParticle(500, "p_pt_r_hand", "ef_Notice_Board")
		local voteType = actor:GetParam("VOTE_TYPE")
		if "OK" == voteType then
			actor:ChangeParticleTexture(500, "ef_Notice_Board", "Cylinder01", 1, "../Data/4_Item/9_Etc/Notice_Board/Circle.dds")
		elseif "NT" == voteType then
			actor:ChangeParticleTexture(500, "ef_Notice_Board", "Cylinder01", 1, "../Data/4_Item/9_Etc/Notice_Board/triangle.dds")
		elseif "NO" == voteType then
			actor:ChangeParticleTexture(500, "ef_Notice_Board", "Cylinder01", 1, "../Data/4_Item/9_Etc/Notice_Board/x.dds")
		else
			actor:ChangeParticleTexture(500, "ef_Notice_Board", "Cylinder01", 1, "../Data/4_Item/9_Etc/Notice_Board/question.dds")
		end
		
	end
	
	local kSubActor = actor:GetSubPlayer()
	if( false == kSubActor:IsNil() ) then
		kSubActor:SetParam("VOTE_TYPE", actor:GetParam("VOTE_TYPE")) 
		DoSubActorAction( actor, action:GetID(), action:GetDirection() )
	end	
	return true
end

function Act_Single_Emotion_Picket_OnUpdate(actor, accumTime, frameTime)
	if actor:IsAnimationDone() == true then
		return false
	end
	return true
end

function Act_Single_Emotion_Picket_OnCleanUp(actor, action)	
	actor:HideParts(6, false)
	actor:DetachFrom(500)
	actor:RestoreLockBidirection()			
	DoSubActorAction( actor, "a_twin_sub_trace_ground", action:GetDirection() )
	return true
end

function Act_Single_Emotion_Picket_OnLeave(actor, action)
	return true
--[[
	if actor:IsAnimationDone() == true then
		actor:HideParts(6, false)
--		actor:DetachFrom(500)
		return true
	end
	return false
]]
end
