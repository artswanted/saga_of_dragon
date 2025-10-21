-- 레인저 (클레이모어 : a_Claymore) : Level 1-10
function Effect_Begin110001401(unit, iEffectNo, actarg)

	return 1
end

function Effect_End110001401(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End110001401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	local kGround = actarg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	
	local kPos = unit:GetPos()
	local kUnitArray = NewUNIT_PTR_ARRAY()
	
	local iRange = effect:GetAbil(AT_DETECT_RANGE)
	InfoLog(9, "Effect_End110001401 Range:" .. iRange)
	kGround:GetUnitInRangeZ(kPos, iRange, 30, UT_MONSTER, kUnitArray)
	kGround:GetUnitInRangeZ(kPos, iRange, 30, UT_PLAYER, kUnitArray)
	
	--공격력은 설치자의 물리공격력 + 크레이모아 공격력
	local iDamage = unit:GetAbil( AT_ATTR_ATTACK ) + effect:GetAbil(AT_PHY_ATTACK_MAX)
	local caster = kGround:GetUnit( unit:GetCaller() )
	if caster:IsNil() == true then
		--unit:SetAbil(AT_HP, 0)
		unit:SetHP(0, E_SENDTYPE_NONE, kEffect:GetCaster())
		return ECT_DOTICK
	end
	
	local iCount = 0
	local target = kUnitArray:GetUnit(iCount)
	while target:IsNil() == false do
		--if caster:IsEnemy(target, false) == true then
			DoFinalDamage( caster, target, iDamage, 0, actarg )
			InfoLog(9, "Effect_End110001401 Damage:" .. iDamage)
		--end
		
		iCount = iCount + 1
		if iCount > 5 then	-- 최대 5 개체에게 공격 가능
			break
		end
		
		target = kUnitArray:GetUnit(iCount)
	end -- while
	
	DeleteUNIT_PTR_ARRAY(kUnitArray)
	--unit:SetAbil(AT_HP, 0) -- ClayMore 삭제
	unit:SetHP(0, E_SENDTYPE_NONE, kEffect:GetCaster())

	return 1
end

function Effect_Tick110001401(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick110001401 Effect=" .. effect:GetEffectNo())
	return ECT_DOTICK
end
