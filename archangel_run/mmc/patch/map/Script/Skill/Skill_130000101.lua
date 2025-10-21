
-- War Mage (리소스 컨버터 : a_Resource Converter ) : Level 1-10
function Skill_Begin130000101(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire130000101(caster, kTargetArray, skillnum, result, arg)
	local kTarget0 = kTargetArray:GetUnit(0)
	if kTarget0:IsNil() == true then -- Target이 없으면 처리 안함.
		return 1
	end
	--GetSkillResult130000101(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire130000101....SkillDef is NIl" ..skillnum)
		return -1
	end
		
	local iDecHP = kSkillDef:GetAbil(AT_HP)
	local iMinHP = math.max(kSkillDef:GetAbil(AT_AMOUNT), iDecHP)
	local iHP = caster:GetAbil(AT_HP)
	if iHP <= iMinHP then
		return -1
	end
	caster:SetAbil(AT_HP, iHP+iDecHP, false, false)
	InfoLog(9, "Skill_Fire130000101 CurHP:" .. iHP .. ", DecHP:" .. iDecHP)
	local iRate = kSkillDef:GetAbil(AT_PERCENTAGE)
	local iAdd = iDecHP * iRate / ABILITY_RATE_VALUE
	InfoLog(9, "Skill_Fire130000101 MPRate:" .. iRate .. ", MP_Add:" .. iAdd)
	local iMaxMP = caster:GetAbil(AT_C_MAX_MP)
	local iMP = caster:GetAbil(AT_MP)
	caster:SetAbil(AT_MP, math.min(iMaxMP, iMP+iAdd), true, false)
	
	-- HP 줄어드는것을 보여주자
	local kPacket = NewPacket(PT_M_C_NFY_ABILCHANGED)
	kPacket:PushGuid(caster:GetGuid())
	kPacket:PushShort(AT_HP)
	kPacket:PushInt(iHP+iDecHP)
	kPacket:PushGuid(caster:GetGuid())
	kPacket:PushInt(skillnum)
	caster:Send(kPacket, E_SENDTYPE_BROADALL)
	DeletePacket(kPacket)

	return 1
end

function Skill_Fail130000101(caster, kTargetArray, skillnum, result, arg)
	return false
end
