-- 저격수 ( WP그레네이드 : a_WP Granade) : Level 1-5
function Effect_Begin150000801(unit, iEffectNo, actarg)
	return 1
end

function Effect_End150000801(unit, iEffectNo, actarg)
	return 1
end

function Effect_Tick150000801(unit, elapsedtime, effect, actarg)
	return ECT_DOTICK
end

-- 저격수 ( WP그레네이드 설치: a_WP Granade Fire) : Level 1-5

function Effect_Begin1500008001(unit, iEffectNo, actarg)	
	return 1
end

function Effect_End1500008001(unit, iEffectNo, actarg)
	return 1
end

function Effect_Tick1500008001(unit, elapsedtime, effect, actarg)
	
	local iTickDamage = effect:GetAbil(AT_HP);
	local iAddEffect = effect:GetAbil(AT_EFFECTNUM1);
	
	local iHP = unit:GetAbil(AT_HP);
	local iNewHP = math.max(1, iHP+iTickDamage);
	unit:SetHP(iNewHP, E_SENDTYPE_NONE, effect:GetCaster());

	unit:AddEffect(iAddEffect, 0, actarg, unit);

	local kPacket = NewPacket(PT_M_C_NFY_ABILCHANGED)
	kPacket:PushGuid(unit:GetGuid())
	kPacket:PushShort(AT_HP)
	kPacket:PushInt(iNewHP)
	kPacket:PushGuid(effect:GetCaster())
	kPacket:PushInt(effect:GetEffectNo())
	unit:Send(kPacket, E_SENDTYPE_BROADALL)
	DeletePacket(kPacket)	

	return ECT_DOTICK
end