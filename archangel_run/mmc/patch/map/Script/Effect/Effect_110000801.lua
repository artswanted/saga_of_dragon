-- 사냥 (스플래쉬 트랩 : a_Splash Trap) : Level 1-5
function Effect_Begin110000801(unit, iEffectNo, actarg)

	return 1
end

function Effect_End110000801(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End110000801...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	
	if kEffect:GetValue() == 0 then
		-- 이전에 폭발한 적이 없으므로 없어지기 전에 폭발한다.
		Effect_Tick110000801(unit, 0, kEffect, actarg)
	end
	return 1
end

function Effect_Tick110000801(unit, elapsedtime, effect, actarg)
	local kGround = actarg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	
	local kPos = unit:GetPos()
	local kUnitArray = NewUNIT_PTR_ARRAY()
	
	local iRange = effect:GetAbil(AT_DETECT_RANGE)
	kGround:GetUnitInRangeZ(kPos, iRange, 30, UT_MONSTER, kUnitArray)
	kGround:GetUnitInRangeZ(kPos, iRange, 30, UT_PLAYER, kUnitArray)
	
	--공격력은 설치자의 물리공격력 + 트랩공격력
	local iDamage = unit:GetAbil( AT_ATTR_ATTACK ) + effect:GetAbil(AT_PHY_ATTACK_MAX)
	local caster = kGround:GetUnit( unit:GetCaller() )
	if caster:IsNil() == true then
		--unit:SetAbil(AT_HP, 0)
		unit:SetHP(0, E_SENDTYPE_NONE, effect:GetCaster())
		return ECT_DOTICK
	end
	
	local iCount = 0
	local bFired = false
	local target = kUnitArray:GetUnit(iCount)
	while target:IsNil() == false do
		if caster:IsEnemy(target, false) == true then
			DoFinalDamage( caster, target, iDamage, 0, actarg )
			bFired = true
		end
		
		iCount = iCount + 1
		if iCount > 5 then
			break
		end
		
		target = kUnitArray:GetUnit(iCount)
	end -- while
	
	DeleteUNIT_PTR_ARRAY(kUnitArray)
	if bFired == true then
		effect:SetValue(-1)
		--unit:SetAbil(AT_HP, 0) -- 지뢰개체 삭제하기
		unit:SetHP(0, E_SENDTYPE_NONE, effect:GetCaster())
		return ECT_MUSTDELETE	-- 지뢰가 터졌으므로 삭제되어야 한다.
	end
	return ECT_DOTICK
end
