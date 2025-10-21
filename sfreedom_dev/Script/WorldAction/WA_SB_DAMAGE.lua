-- 보스	데미지 모션
iBossState = 0

function WA_SB_DAMAGE_OnReceivePacket(wa_obj,Packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	BossActor:SetAnimSpeed(1)
	BossActor = g_world:FindActor(g_world:GetBossGUID())
	if iDownPuppetID4 ~= -1 then
		puppets[iDownPuppetID4]:TransitAction("up")
		iDownPuppetID4 = -1
	end
	
	local now = BossActor:GetLookingDir()
	wa_obj:SetParamFloat(9, now:GetX())
	wa_obj:SetParamFloat(10, now:GetY())

	return true
end

function WA_SB_DAMAGE_OnEnter(wa_obj,ElapsedTimeAtStart)
	if BossActor:IsNil() then
		return
	end
	QuakeCamera(1.0, 2.0)
	BossActor:ReserveTransitAction("a_sp_damage")		-- 애니	시작
--	BossActor:SetTargetAnimation("dmg_01")		-- 애니	시작
	ODS("보스 데미지 모션 \n")
	if iBossState == 0 then
--  	MessageBox("iBossState 0")
		BossActor:DetachChild("sapphire_finger_05")
		BossActor:DetachChild("sapphire_finger_06")
		BossActor:DetachChild("sapphire_finger_07")
		BossActor:DetachChild("sapphire_finger_08")
		BossActor:DetachChild("sapphire_hand02")
		BossActor:DetachChild("sapphire_arm04")
		BossActor:DetachChild("sapphire_arm02")
 		BossActor:AttachParticle(1234,	"Bip01 R Finger2", "e_boss_die_small_explode")
 		BossActor:DetachFrom(12349)
	elseif iBossState == 1 then
--		MessageBox("iBossState 1")
		BossActor:DetachChild("sapphire_finger_01")
		BossActor:DetachChild("sapphire_finger_02")
		BossActor:DetachChild("sapphire_finger_03")
		BossActor:DetachChild("sapphire_finger_04")
		BossActor:DetachChild("sapphire_hand01")
		BossActor:DetachChild("sapphire_arm03")
		BossActor:DetachChild("sapphire_arm01")
 		BossActor:AttachParticle(12147,	"Bip01 L Finger2", "e_boss_die_small_explode")
 		BossActor:DetachFrom(12348)
	elseif iBossState == 2 then
--		MessageBox("iBossState 2")
	 	BossActor:DetachChild("sapphire_head01")
	 	BossActor:DetachChild("sapphire_head02")
	 	BossActor:DetachChild("spdragon_sapphire_heart")	 	
		BossActor:AttachParticle(12345,	"sapphire_head01", "e_boss_die_small_explode")
		BossActor:DetachFrom(12346)
		BossActor:DetachFrom(12350)
	end
	
	iBossState = iBossState	+ 1
	return true
end

function WA_SB_DAMAGE_OnUpdate(wa_obj,ElapsedTime)
	if BossActor:IsNil() then
		return false
	end
	
	BossActor:LookAt(Point3( wa_obj:GetParamFloat(9), wa_obj:GetParamFloat(10), 0 ) )
	return false
end

function WA_SB_DAMAGE_OnLeave(wa_obj)
	if iBossState >= 3 then
		iBossState = 0
	end
	return true
end
