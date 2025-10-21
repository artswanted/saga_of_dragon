-- 용암탄.. 

function WA_METEOR_OnReceivePacket(wa_obj, Packet)

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local kPos = Point3(Packet:PopFloat(), Packet:PopFloat(), Packet:PopFloat() + 100) 
	local bMeteorExtension = Packet:PopInt()
	local kFloorPos = g_world:ThrowRay(kPos, Point3(0,0,-1), 500)

    wa_obj:SetParamInt(125, bMeteorExtension)
	wa_obj:SetParamFloat(10, kFloorPos:GetX())
	wa_obj:SetParamFloat(11, kFloorPos:GetY())
	wa_obj:SetParamFloat(12, kFloorPos:GetZ())

	return true
end

function WA_METEOR_OnEnter(wa_obj, ElapsedTimeAtStart)
	local kX = wa_obj:GetParamFloat(10)
	local kY = wa_obj:GetParamFloat(11)
	local kZ = wa_obj:GetParamFloat(12)
	local kTargetPt = Point3(kX, kY, kZ)
	kTargetPt:SetZ(kTargetPt:GetZ() + 10)

	local kMyActor = GetPlayer()
	if kMyActor:IsNil() == false then
		kMyActor:AttachParticleToPoint(10000 + math.random(100), kTargetPt, "e_ef_fossilearth_target_01")
	end
	return true
end

function WA_METEOR_OnUpdate(wa_obj, ElapsedTime)

	local iStage = wa_obj:GetCurrentStage()
	local iDuration = wa_obj:GetDuration()

    local bMeteorExtension = wa_obj:GetParamInt(125)
	local kX = wa_obj:GetParamFloat(10)
	local kY = wa_obj:GetParamFloat(11)
	local kZ = wa_obj:GetParamFloat(12)
	local kTargetPt = Point3(kX, kY, kZ + 10)
	local kMyActor = GetPlayer()
	if kMyActor:IsNil() then
		return false
	end

	if iStage == 0 and ElapsedTime > 2000 then
		kMyActor:AttachParticleToPoint(10001 + math.random(100), kTargetPt, "e_ef_fossilearth_waver")
		wa_obj:NextStage()
	elseif iStage == 1 and iDuration < ElapsedTime then
		-- Projectile 만들어서 날리자! (Damage는 서버에서 줄거임)
		local kProjectileMan = GetProjectileMan();
		local kProjectileID = "Projectile_Lavalon_Meteor"
		if bMeteorExtension == 1 then
		    kProjectileID = "Projectile_Lavalon_Meteor_Blue"
		end
		local kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID, GetDummyAction(), GUID(0))
		if kNewArrow:IsNil() == true then
			return false
		end

		kNewArrow:SetParamValue("EXTENSION", bMeteorExtension)
	
		local kInitPos = Point3(kX, kY, kZ + 400)
		kInitPos:SetX(kInitPos:GetX() + math.random(-200, 200))
		if math.random(100) > 50 then
			kInitPos:SetY(kInitPos:GetY() + math.random(-200, 200))
		end
		
		kNewArrow:LoadToPosition(kInitPos)
		kNewArrow:SetTargetLoc(Point3(kX, kY, kZ))
		kNewArrow:Fire()
		return false
	end

	return true 
end

function WA_METEOR_OnLeave(wa_obj)
	return true
end

