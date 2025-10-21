function Blizzard_Initialize(actor)

    ODS("Blizzard_Initialize\n");
	actor:FreeMove(true)
    actor:HideNode("Dummy01",true);
    local kPos = actor:GetNodeWorldPos("Dummy01");
	actor:AttachParticleToPointS(2,kPos,"ef_Blizzard_00003", 5);
	actor:AttachSound(50,"Blizzard2");
	
	local kProjectileMan = GetProjectileMan();

	local iTotalProjectile = 40;
	local i = 0;
	local j = 0;
	local fFireDelay = 0.5;
	local fRange = 30;
	while i < iTotalProjectile do
		local kNewArrow = kProjectileMan:CreateNewProjectile("PROJECTILE_Blizzard02", actor:GetAction(), actor:GetPilotGuid());
		if false == kNewArrow:IsNil() then
			local kStartPos = Point3(kPos:GetX() + math.random(-fRange,fRange),
									 kPos:GetY() + math.random(-fRange,fRange),
									 kPos:GetZ() + 150);
			local kEndPos = Point3(kStartPos:GetX(), kStartPos:GetY(), kPos:GetZ());
			kNewArrow:LoadToPosition(kStartPos);	--	장전			
			kNewArrow:SetParamValue("index", tostring(i))
			kNewArrow:SetTargetLoc(kEndPos);
			-- 한발에 랜덤값을 적용해서 틱 당 발사되는 발사체들이 덜 어색하도록
			kNewArrow:DelayFire(fFireDelay * j + (math.random(-3, 3) / 10) ,true);
			kNewArrow:SetParentPilotGuid(actor:GetPilotGuid());
			kNewArrow:SetSpeed(500);
			kNewArrow:SetScale(1.5);
			kNewArrow:SetHide(true);
		end
		i=i+1;

		-- 4발이 같은 틱에 발사되도록 
		if i % 4 == 0 then
			j=j+1;
		end
	end
end

function MeteorRain_Initialize(actor)
	actor:FreeMove(true)
	local iLifeTime = actor:GetAbil(AT_LIFETIME)
	
    local kPos = actor:GetPos();
	--actor:AttachParticleToPointS(2,kPos,"ef_Blizzard_00003", 5);
	--actor:AttachSound(50,"Blizzard2");
	
	local kProjectileMan = GetProjectileMan();

	local iTotalProjectile = 28*3;
	local i = 0;
	local j = 0;
	local fFireDelay = 0.5;
	local fRange = 60;
	while i < iTotalProjectile do
		local kNewArrow = kProjectileMan:CreateNewProjectile("PROJECTILE_MeteorRain", actor:GetAction(), actor:GetPilotGuid());
		if false == kNewArrow:IsNil() then
			local kStartPos = Point3(kPos:GetX() + math.random(-fRange,fRange),
									 kPos:GetY() + math.random(-fRange,fRange),
									 kPos:GetZ() + 150);
			local kEndPos = Point3(kStartPos:GetX(), kStartPos:GetY(), kPos:GetZ());
			kNewArrow:LoadToPosition(kStartPos);	--	장전			
			kNewArrow:SetParamValue("index", tostring(i))
			kNewArrow:SetParamValue("effect", "eff_sum_skill_smw_meteo_rain_02")
			kNewArrow:SetTargetLoc(kEndPos);
			-- 한발에 랜덤값을 적용해서 틱 당 발사되는 발사체들이 덜 어색하도록
			kNewArrow:DelayFire(fFireDelay * j + (math.random(-3, 3) / 10) ,true);
			kNewArrow:SetParentPilotGuid(actor:GetPilotGuid());
			kNewArrow:SetSpeed(500);
			kNewArrow:SetScale(1.5);
			kNewArrow:SetHide(true);
		end
		i=i+1;

		-- 4발이 같은 틱에 발사되도록 
		if i % 12 == 0 then
			j=j+1;
		end
	end
end