-- Act_Pet_Base 

function Act_Pet_Base(actor, keyName)
	-- 진화 모션에서 어떤 이벤트에 대해 이펙트를 뿌려주면 좋을 듯 하다.

end

function Act_Pet_Pickup()
	charform = CallUI("CharInfo")
	if charform:IsNil() == true then
		return true
	end

	petform = charform:GetControl("Pet")
	characterForm = charform:GetControl("Charactor")
	characterForm:Visible(false)
	petform:Visible(true)

	GetPetInfo()
	CallPetUI()
end

local g_PrevPetPos = nil
local g_PetMoveSpeed = nil

local g_PetAction = nil
local g_PetMasterGuid = nil
local g_PetMasterPilot = nil
local g_PetMasterActor = nil
local g_CurAction = nil
local g_PetDistance = 80.0

function Act_Pet_Init(actor, accumTime, frameTime)
	g_PetAction = actor:GetAction()
	if g_PetAction:IsNil() then
		actor:ReserveTransitAction("a_idle")
		return false
	end

	g_PetMasterGuid = actor:GetPetMaster()
	g_PetMasterPilot = g_pilotMan:FindPilot(g_PetMasterGuid)
	if g_PetMasterPilot:IsNil() == true then
		ODS("주인을 버린 배은 망덕한 펫인가.....!!!\n")
		return false
	end
	g_PetMasterActor = g_PetMasterPilot:GetActor()

	-- TODO : 일정한 시간이 흐름으로써 겟 어빌 하자.
	if g_PetMoveSpeed == nil then
		g_PetMoveSpeed = actor:GetAbil(AT_C_MOVESPEED)
	end
	g_CurAction = g_PetAction:GetID()
 	if g_CurAction == nil then
		g_CurAction = "a_idle"
	end

	return true
end

function Act_Pet_Idle(actor, accumTime, frameTime)
	-- 펫이 가만히 있으면(이모션 상태 포함)
	if g_CurAction == "a_idle" or 
		g_CurAction == "a_btidle" then
		if actor:OutOfSight(g_PetMasterGuid, g_PetDistance * 1.3, false) == true then
			actor:FollowActor(g_PetMasterGuid, 50)
			actor:SetSpeedScale(1.0)
		end
	end
end

function Act_Pet_Jump(actor, accumTime, frameTime)
	-- 펫이 점프 상태일 때
	if g_CurAction == "a_jump" then
		actor:FollowActor(g_PetMasterGuid, 50)
		if actor:IsMeetFloor() == true then
		end
	end
end

local g_Act_Pet_Run_Time = 0
function Act_Pet_Run(actor, accumTime, frameTime)
	if accumTime-g_Act_Pet_Run_Time < 0.2 then
		return false
	end
	g_Act_Pet_Run_Time = accumTime

	local NowPetPos = actor:GetTranslate()
	-- 펫이 따라오고 있을때
	if g_CurAction == "a_run" then
		-- 만약 이동 할 수 없으면 점프!
		local fStep = 0.0
		if 	g_PrevPetPos ~= nil then
			fStep = NowPetPos:Distance(g_PrevPetPos)
		end
		if 	g_PrevPetPos ~= nil and
			fStep ~= 0.0 and
			fStep < (g_PetMoveSpeed*0.01) then
			actor:ReserveTransitAction("a_jump")
		else
			if actor:OutOfSight(g_PetMasterGuid, g_PetDistance * 0.5, false) == true then
				-- 캐릭터랑 가까워 질때까지 가자!
				actor:FollowActor(g_PetMasterGuid, 50)
			else
				-- 캐릭터 근처 왔을땐 멈춰!
				actor:ReserveTransitAction("a_idle")
			end
		end
	end
end

function Act_Pet_Post(actor, accumTime, frameTime)
	-- 그 외의 행동(걷기, 뛰기 등)
	if actor:OutOfSight(g_PetMasterGuid, g_PetDistance, false) == false then
		actor:UntransitAction(g_CurAction)
	else
		--[[
		if actor:WillBeFall(true, 5.0) == true or
			actor:WillBeFall(false, 30.0) == true then
			if actor:CompareActorPosition(g_PetMasterGuid, Point3(0,0,1), 10.0) > 0 then
				actor:ReserveTransitAction("a_jump")
			end
		end
		]]--
	end

	-- 너무 멀리 떨어지면, 텔레포트 한다.
	if actor:OutOfSight(g_PetMasterGuid, g_PetDistance * 3.8, true) == true then 
		if actor:IsMeetFloor() == true then
			if g_PetMasterActor:IsMeetFloor() == true then
				local target_pos = g_PetMasterActor:GetTranslate()
				actor:AttachParticle(289, "char_root", "e_special_transform")
				target_pos:SetZ(target_pos:GetZ()+1)
				actor:SetTranslate(target_pos)
				actor:ReserveTransitAction("a_idle")
			end
		end
	end
end

function Act_Pet_Follow(actor, accumTime, frameTime)
	-- 남의 펫은 그냥 패킷을 받아서 움직이면 된다.
	if actor:IsMyPet() == false then
		return false
	end

	-- init
	local bRet = Act_Pet_Init(actor, accumTime, frameTime)
	if bRet == false then
		return false
	end
	
	-- 펫 등급(몇 차 펫인지..)
	-- 아직 등급 적용 안됨.
	--[[
	local petPilot = g_pilotMan:FindPilot(actor:GetPilotGuid())
	if petPilot:GetAbil(AT_GRADE) == 1 then
		if actor:OutOfSight(g_PetMasterGuid, g_PetDistance * 1.7, false) == true then 
			-- 1차 펫의 경우 주인과 멀리 떨어지면 소환해제된다.
			--Net_ReqPetState(0)
			return true
		end
	end
	]]--

	-- 아직 타는건 없다 ㅡ_ㅡ/..
	--[[
	if g_PetMasterActor:IsRiding() == true then
		return true
	end
	]]--

	Act_Pet_Idle(actor, accumTime, frameTime)
	Act_Pet_Jump(actor, accumTime, frameTime)
	Act_Pet_Run(actor, accumTime, frameTime)
	Act_Pet_Post(actor, accumTime, frameTime)
	
	g_PrevPetPos = actor:GetTranslate()
	return true
end
