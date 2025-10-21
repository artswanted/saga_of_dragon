
-- 투사 (와일드레이지 : a_Wild Rage) : Level 1-10
function Skill_Begin106300501(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire106300501(caster, kTargetArray, skillnum, result, arg)
	--local bReturn = GetSkillResult106300501(caster, kTargetArray, skillnum, result)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire106300501....SkillDef is NIl" ..skillnum)
		return -1
	end
	
	local kPartyGuid = caster:GetPartyGuid()
	if kPartyGuid:IsNil() == true then
		caster:AddEffect(kSkillDef:GetEffectNo(), 0, arg, caster)
		return -1
	end
	
	-- Party Member를 얻어온다.
	local kGround = arg:Get(ACTARG_GROUND)
	if kGround == nil then
		InfoLog(5, "Effect_Tick105500901 Ground is NILL")
		return -1
	end
	
	kGround = Ground(kGround)
	if kGround:IsNil() then
		InfoLog(5, "Skill_Fire106300501 Ground is NILL")
		return -1
	end
	local iEffectNo = kSkillDef:GetEffectNo()
	InfoLog(9, "Skill_Fire106300501 30")
	local kPartyMgr = GetLocalPartyMgr()
	InfoLog(9, "Skill_Fire106300501 31")
	local kGuidVector = NewVecGuid()
	InfoLog(9, "Skill_Fire106300501 32")
	kPartyMgr:GetPartyMember(kPartyGuid, kGuidVector)
	InfoLog(9, "Skill_Fire106300501 33")
	local iIndex = 0
	local kMemberGuid = kGuidVector:Get(iIndex)
	while kMemberGuid:IsNil() == false do
		InfoLog(9, "Skill_Fire106300501 40")
		local kMember = kGround:GetUnit(kMemberGuid)
		if kMember:IsNil() == false then
			InfoLog(9, "Skill_Fire106300501 41")
			kMember:AddEffect(iEffectNo, 0, arg, caster)
		end
		iIndex = iIndex + 1
		InfoLog(9, "Skill_Fire106300501 42")
		kMemberGuid = kGuidVector:Get(iIndex)
	end
	DeleteVecGuid(kGuidVector)
	
	return iIndex
end

function Skill_Fail106300501(caster, kTargetArray, skillnum, result, arg)
	return false
end
