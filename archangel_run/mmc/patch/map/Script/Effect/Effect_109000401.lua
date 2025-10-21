
-- 메이지 (콰그마이어 : a_Quagmire) : Level 1-10
-- 해당 Effect는 두가지 형태로 unit에 Add된다.
--	1. 콰그마이어를 생성시키는 UT_ENITY 개체에 생성
--	2. 콰그마이어에 걸려든 unit에게 생성
function Effect_Begin109000401(unit, iEffectNo, actarg)
	if unit:IsUnitType(UT_ENTITY) == true then
		-- UT_ENTITY 이므로 콰그마이어를 생성시킨 개체이다.
		return 1
	end
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin109000401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 명중율 저하
	local iAdd = kEffect:GetAbil(AT_R_HITRATE)
	kEffectMgr:AddAbil(AT_R_HITRATE, iAdd)
	unit:NftChangedAbil(AT_R_HITRATE, E_SENDTYPE_NONE)
	-- 이동속도 저하
	iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_HITRATE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_MOVESPEED)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)			
	return 1
end

function Effect_End109000401(unit, iEffectNo, actarg)
	if unit:IsUnitType(UT_ENTITY) == true then
		-- UT_ENTITY 이므로 콰그마이어를 생성시킨 개체이다.
		return 1
	end
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End109000401...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 명중율 저하
	local iAdd = kEffect:GetAbil(AT_R_HITRATE)
	kEffectMgr:AddAbil(AT_R_HITRATE, 0-iAdd)
	unit:NftChangedAbil(AT_R_HITRATE, E_SENDTYPE_NONE)
	-- 이동속도 저하
	iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, 0-iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_NONE)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_R_HITRATE)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_R_MOVESPEED)
	unit:SendAbiles(2, kActarg, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	DeleteActArg(kActarg)
	return 1
end

function Effect_Tick109000401(unit, elapsedtime, effect, actarg)
	if unit:IsUnitType(UT_ENTITY) == true then
		-- UT_ENTITY 이므로 콰그마이어를 생성시킨 개체이다.
		local kGround = actarg:Get(ACTARG_GROUND)
		kGround = Ground(kGround)
		local kPos = unit:GetPos()
		local kUnitArray = NewUNIT_PTR_ARRAY()
		local iRange = effect:GetAbil(AT_ATTACK_RANGE)
		kGround:GetUnitInRangeZ(kPos, iRange, 30, UT_MONSTER, kUnitArray)
		kGround:GetUnitInRangeZ(kPos, iRange, 30, UT_PLAYER, kUnitArray)
		local caster = kGround:GetUnit( unit:GetCaller() )
		if caster:IsNil() == true then
			--unit:SetAbil(AT_HP, 0)
			unit:SetHP(0, E_SENDTYPE_NONE, effect:GetCaster())
			return ECT_DOTICK
		end
	
		local iCount = 0
		local target = kUnitArray:GetUnit(iCount)
		while target:IsNil() == false do
			if caster:IsEnemy(target, false) == true then
				target:AddEffect(effect:GetAbil(AT_EFFECTNUM1), 0, arg, unit)
			end

			iCount = iCount + 1
			if ( iCount > 5 )	then break end
			target = kUnitArray:GetUnit(iCount)
		end -- while
		DeleteUNIT_PTR_ARRAY(kUnitArray)
	else
		InfoLog(6, "Don't call me Effect_Tick109000401 Effect=" .. effect:GetEffectNo())
	end
	return ECT_DOTICK
end
