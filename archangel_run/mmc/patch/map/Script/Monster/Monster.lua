function ProcessMonsterAttack(caster,target,skillno,result,arg)
	local pkGround = arg:Get(ACTARG_GROUND)
	pkGround = Ground(pkGround)
	local iOldHp = target:GetAbil(AT_HP)

	if pkGround:IsNil() == true then
		return false
	end

	caster:SkillFire(skillno,target,result,arg)

	-- 타겟의 파티원에게 알려주자
	--local iCurHp = target:GetAbil(AT_HP)
	--if iCurHp ~= iOldHp and target:IsUnitType(UT_PLAYER) then
	--	local kNfyPacket = NewPacket(PT_M_C_NFY_CHANGE_PARTY_DMG_HP)
	--	kNfyPacket:PushGuid(target:GetGuid())
	--	kNfyPacket:PushInt(iCurHp)
	--	kNfyPacket:PushInt(target:GetAbil(AT_MAX_HP))
		
	--	target:SendPacketToPartyMember(kNfyPacket,pkGround)

	--	DeletePacket(kNfyPacket)
	--end
	return true
end

--[[
function MonsterDie(monster, mon_no, ground, actarg)
	if mon_no == M_SAPHIREBOSS then
		MonsterDie_200008(monster, ground, actarg)
	elseif mon_no == M_PARELBOSS then
		MonsterDie_6000920(monster, ground, actarg)
		MonsterDie_Default(monster, mon_no, ground, actarg)
	else
		MonsterDie_Default(monster, mon_no, ground, actarg)
	end
end

function MonsterDie_Default(monster, mon_no, ground, actarg)
	packet = NewPacket(PT_M_C_NFY_STATE_CHANGE)
	packet:PushGuid(monster:GetGuid())
	packet:PushShort(AT_HP)
	packet:PushInt(0)
	local emptyGuid = GUID()
	ground:Broadcast(packet, monster, emptyGuid)
	DeletePacket(packet)
end
]]
