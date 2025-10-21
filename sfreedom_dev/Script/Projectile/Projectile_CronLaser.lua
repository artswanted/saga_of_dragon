
function PROJECTILE_CRON_LASER_OnCollision(kProjectile,kCollideActor,kTargets)
	return	PROJECTILE_ARROW_OnCollision(kProjectile,kCollideActor,kTargets);
end

function PROJECTILE_CRON_LASER_OnTargetListModified(kProjectile)
	return	true
end

function PROJECTILE_CRON_LASER_OnArrivedAtTarget(kProjectile)
	return	PROJECTILE_ARROW_OnArrivedAtTarget(kProjectile)
end

function PROJECTILE_CRON_LASER_OnUpdate(kProjectile)

	if 3 == kProjectile:GetState() then	--S_FLYING

		local fNowAcc = g_world:GetAccumTime()
		local fOldAcc = tonumber(kProjectile:GetParamValue('Time'))
		if nil == fOldAcc then
			fOldAcc = 0.0
		end
	
		if fNowAcc-fOldAcc < 0.04 then
			return true
		end

		kProjectile:SetParamValue('Time', tostring(fNowAcc))

		local actor = g_world:FindActor(kProjectile:GetParentPilotGuid())
		if false == actor:IsNil() then
			local kFirePos = actor:GetNodeWorldPos( kProjectile:GetFireNode() )
			local kEndPos = actor:GetNodeWorldPos( kProjectile:GetTargetEndNode() )
			
			local kDir = kEndPos:_Subtract(kFirePos)
			kDir:Unitize()

			local kGroundPos = g_world:ThrowRay(kFirePos, kDir, 1000)	--중간에 움직이는 벽 검사
			if kGroundPos:GetX() == -1 and kGroundPos:GetY() == -1 and kGroundPos:GetZ() == -1 then
				--바닥을 찾지못하면 이펙트를 붙이지 않는다.
				return true;
			end
		
			g_world:AttachParticle("ef_antares_skill_02_02_char_root", kGroundPos)
		end
	
	end
	
	return	true
end
