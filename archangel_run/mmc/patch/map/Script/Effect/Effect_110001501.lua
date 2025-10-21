-- 레인저 (자동 사격 장치 : a_Auto Shoot System) : Level 1-5
function Effect_Begin110001501(unit, iEffectNo, actarg)
	InfoLog(5, "Do not call me DELETED EFFECT : Effect_Begin110001501")
	return 1
end

function Effect_End110001501(unit, iEffectNo, actarg)
	InfoLog(5, "Do not call me DELETED EFFECT : Effect_End110001501")
	--local kEffectMgr = unit:GetEffectMgr()
	--local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	--if kEffect:IsNil() == true then
	--	InfoLog(5, "Effect_End110001501...Cannot GetEffect(" .. iEffectNo ..")");
	--	return 0
	--end

	return 1
end

function Effect_Tick110001501(unit, elapsedtime, effect, actarg)
	InfoLog(5, "Do not call me DELETED EFFECT : Effect_Tick110001501")
	--[[
	local iDelay = math.max(unit:GetDelay() - elapsedtime, 0)
	unit:SetDelay(iDelay)
	if iDelay > 0 then
		-- 너무 빠르게 연속해서 공격하는 것을 방지하기 위함
		return ECT_DOTICK
	end
	local kGround = actarg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	
	local kPos = unit:GetPos()
	local kUnitArray = NewUNIT_PTR_ARRAY()
	
	local iRange = effect:GetAbil(AT_DETECT_RANGE)
	kGround:GetUnitInRangeZ(kPos, iRange, 30, UT_MONSTER, kUnitArray)
	kGround:GetUnitInRangeZ(kPos, iRange, 30, UT_PLAYER, kUnitArray)
	
	--공격력은 설치자의 물리공격력 + 자동사격장치공격력
	local iDamage = unit:GetAbil( AT_ATTR_ATTACK ) + effect:GetAbil(AT_PHY_ATTACK_MAX)
	local caster = kGround:GetUnit( unit:GetCaller() )
	if caster:IsNil() == true then
		--unit:SetAbil(AT_HP, 0)
		unit:SetHP(0, E_SENDTYPE_NONE, effect:GetCaster())
		return ECT_DOTICK
	end
	
	local iCount = 0
	local target = kUnitArray:GetUnit(iCount)
	while target:IsNil() == false do
		if caster:IsEnemy(target, true) == true then
			DoFinalDamage( caster, target, iDamage, 0, actarg )
			unit:SetDelay(500)	-- 한번 공격을 했으니 좀 쉬어주자.
		end
		
		iCount = iCount + 1
		target = kUnitArray:GetUnit(iCount)
	end -- while
	
	DeleteUNIT_PTR_ARRAY(kUnitArray)
	]]
	return ECT_DOTICK
end
