-- ©К╬ое╨.. 
function Act_LAVALON_HEART_METEOR_OnEnter(actor,action)

	local kPacket = action:GetParamAsPacket()
	if kPacket:IsNil() then
		return true
	end
	
	local kPos = Point3(kPacket:PopInt(), kPacket:PopInt(), kPacket:PopInt() + 100) 
	local bMeteorExtension = kPacket:PopInt()
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local kTargetPt = g_world:ThrowRay(kPos, Point3(0,0,-1), 500)
	
	actor:SetAbil(AT_SKILL_CUSTOM_DATA_01, 	kTargetPt:GetX())
	actor:SetAbil(AT_SKILL_CUSTOM_DATA_01+1, 	kTargetPt:GetY())
	actor:SetAbil(AT_SKILL_CUSTOM_DATA_01+2, 	kTargetPt:GetZ())
	actor:SetAbil(AT_SKILL_CUSTOM_DATA_01+3, 	bMeteorExtension)
	
	kTargetPt:SetZ(kTargetPt:GetZ() + 10)
	
	local kMyPilot = g_pilotMan:GetPlayerActor()
	if kMyPilot:IsNil() == false then
		kMyPilot:AttachParticleToPoint(10000 + math.random(100), kTargetPt, "e_ef_fossilearth_target_01")
	end
	
	return true
end

function Act_LAVALON_HEART_METEOR_OnUpdate(actor, accumTime, frameTime)
	return true
end

function Act_LAVALON_HEART_METEOR_OnCleanUp(actor, action)
	return true
end

function Act_LAVALON_HEART_METEOR_OnLeave(actor, action)
	return true
end


function Act_LAVALON_HEART_METEOR_OnCastingCompleted(actor, action)
	ODS("____________Lavalon Heart Meteor OnCastingComplete \n")
	action:SetSlot(2)
	actor:PlayCurrentSlot()

	if actor:IsNil() then
		return false
	end

	local kX = actor:GetAbil(AT_SKILL_CUSTOM_DATA_01)
	local kY = actor:GetAbil(AT_SKILL_CUSTOM_DATA_01+1)
	local kZ = actor:GetAbil(AT_SKILL_CUSTOM_DATA_01+2)
	local bMeteorExtension = actor:GetAbil(AT_SKILL_CUSTOM_DATA_01+3)
	local kTargetPt = Point3(kX, kY, kZ + 10)

	actor:AttachParticleToPoint(10001 + math.random(100), kTargetPt, "e_ef_fossilearth_waver")

	local kProjectileMan = GetProjectileMan();
	local kProjectileID = "Projectile_Lavalon_Meteor"
	if bMeteorExtension == 1 then
	    kProjectileID = "Projectile_Lavalon_Meteor_Blue"
	end
	
	local kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID, action, actor:GetPilotGuid())
	if kNewArrow:IsNil() == true then
		return false
	end

	kNewArrow:SetParentActionInfo(action:GetActionNo(),action:GetActionInstanceID(),action:GetTimeStamp())
	kNewArrow:SetParamValue("EXTENSION", bMeteorExtension)

	local kInitPos = Point3(kX, kY, kZ + 400)
	--kInitPos:SetX(kInitPos:GetX() + math.random(-200, 200))
	--if math.random(100) > 50 then
	--	kInitPos:SetY(kInitPos:GetY() + math.random(-200, 200))
	--end
	
	kNewArrow:LoadToPosition(kInitPos)
	kNewArrow:SetTargetLoc(Point3(kX, kY, kZ))
	kNewArrow:Fire(true)

	return true
end

function Act_LAVALON_HEART_METEOR_OnEvent(actor, textKey, seqID)

	if textKey == "hit" then
		ODS("____________Lavalon Heart Meteor OnEvent : hit " .. textKey .. "\n")
		local kAction = actor:GetAction()
		kAction:NextStage()
		QuakeCamera(1,1,1,1,1)
	end
	
	return	true;
end
