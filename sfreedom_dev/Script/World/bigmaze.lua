function Moving_Block_OnEnter(trigger, actor)
end

function Moving_Block_OnUpdate(trigger, actor)
end

function Moving_Block_OnLeave(trigger, actor)
end

function Trap_StoneHand_OnEnter(trigger, actor)
	actor:AttachParticle(938, "char_root", "e_dmg_msg_04")
	actor:ReserveTransitAction("a_knockdown")
	return true
end

function Trap_StoneHand_OnUpdate(trigger, actor)
end

function Trap_StoneHand_OnLeave(trigger, actor)
end

function Trap_Jump_OnEnter(trigger, actor)
	actor:ReserveTransitAction("a_idle")
	actor:StartJump(700)
	return true
end

function Trap_Jump_OnUpdate(trigger, actor)
	action = actor:GetAction()
	if action:IsNil() == false then
		action:SetParam(119, "jump_trap")
	end
	return true
end

function Trap_Jump_OnLeave(trigger, actor)
end

function Trap_Jump_a_OnEnter(trigger, actor)
	actor:ReserveTransitAction("a_idle")
	actor:StartJump(700)
	return true
end

function Trap_Jump_a_OnUpdate(trigger, actor)
	action = actor:GetAction()
	if action:IsNil() == false then
		action:SetParam(119, "jump_trap")
	end
	return true
end

function Trap_Jump_a_OnLeave(trigger, actor)
end


function Trap_Jump_b_OnEnter(trigger, actor)
	actor:ReserveTransitAction("a_idle")
	actor:StartJump(2700)
	return true
end

function Trap_Jump_b_OnUpdate(trigger, actor)
	action = actor:GetAction()
	if action:IsNil() == false then
		action:SetParam(119, "jump_trap")
	end
	return true
end

function Trap_Jump_b_OnLeave(trigger, actor)
end

-------------------* 카메라 *-------------------

-- 17. 보조형 (5)
function underCamera_bz01_17_OnEnter(trigger, actor)
	return true
end

function underCamera_bz01_17_OnUpdate(trigger, actor)
	SetCameraSetting(Point3(0, 596, 170), Point3(0, 0, 64))
	return true
end

function underCamera_bz01_17_OnLeave(trigger, actor)
	SetCameraSetting(Point3(0, 596, 170), Point3(0, 0, 148))
	return true
end

-- 16. 보조형 (4)
function underCamera_bz01_16_OnEnter(trigger, actor)
	return true
end

function underCamera_bz01_16_OnUpdate(trigger, actor)
	SetCameraSetting(Point3(0, 388, 138), Point3(0, 0, 12))
	return true
end

function underCamera_bz01_16_OnLeave(trigger, actor)
	SetCameraSetting(Point3(0, 388, 138), Point3(0, 0, 80))
	return true
end

-- 15. 보조형 (3)
function underCamera_bz01_15_OnEnter(trigger, actor)
	return true
end

function underCamera_bz01_15_OnUpdate(trigger, actor)
	SetCameraSetting(Point3(0, 584, 342), Point3(0, 0, 72))
	return true
end

function underCamera_bz01_15_OnLeave(trigger, actor)
	return true
end

-- 14. 보조형 (2)
function underCamera_bz01_14_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	SetCameraSetting(Point3(0, 596, 170), Point3(0, 0, 100))
	g_world:SetCameraMode(1, actor)
	return true
end

function underCamera_bz01_14_OnUpdate(trigger, actor)
	return true
end

function underCamera_bz01_14_OnLeave(trigger, actor)
	return true
end


-- 13. 고정 카메라(2)
function underCamera_bz01_13_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraPose(Point3(1151, -679, 3854), Quaternion(0.418546, 0.569929, 0.424071, 0.565830))
	g_world:SetCameraMode(0, actor)
	-- SetCameraSetting(Point3(0, 636, 86), Point3(0, 0, 40))
	return true
end

function underCamera_bz01_13_OnUpdate(trigger, actor)
	return true
end

function underCamera_bz01_13_OnLeave(trigger, actor)
	return true
end

-- 12. 고정 카메라(1)
function underCamera_bz01_12_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetCameraPose(Point3(1799, 279, 317), Quaternion(0.468, 0.533, 0.500, 0.505))
	g_world:SetCameraMode(0, actor)

	--if actor:GetPilotGuid():IsEqual(g_playerInfo.guidPilot) == true then
	--	SetCameraSetting(Point3(0, 504, 166), Point3(0, 0, 140))
	--end
	return true
end

function underCamera_bz01_12_OnUpdate(trigger, actor)
	return true
end

function underCamera_bz01_12_OnLeave(trigger, actor)
	return true
end

-- 11. 무지큰 카메라
function underCamera_bz01_11_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	SetCameraSetting(Point3(0, 504, 166), Point3(0, 0, 140))
	g_world:SetCameraMode(1, actor)
	return true
end

function underCamera_bz01_11_OnUpdate(trigger, actor)
	return true
end

function underCamera_bz01_11_OnLeave(trigger, actor)
	return true
end

-- 10. 상, 하 주먹 바위
function underCamera_bz01_10_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	SetCameraSetting(Point3(28, 404, 110), Point3(36, 0, 76))
	g_world:SetCameraMode(1, actor)
	return true
end

function underCamera_bz01_10_OnUpdate(trigger, actor)
	return true
end

function underCamera_bz01_10_OnLeave(trigger, actor)
	return true
end

-- 9. 7:3 상, 하 비율 카메라
function underCamera_bz01_09_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	SetCameraSetting(Point3(0, 428, 198), Point3(0, 0, 8))
	g_world:SetCameraMode(1, actor)
	return true
end

function underCamera_bz01_09_OnUpdate(trigger, actor)
	return true
end

function underCamera_bz01_09_OnLeave(trigger, actor)
	return true
end

-- 8. 9:1.5 상, 하 비율 카메라
function underCamera_bz01_08_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	SetCameraSetting(Point3(0, 564, 186), Point3(0, 0, 164))
	g_world:SetCameraMode(1, actor)
	return true
end

function underCamera_bz01_08_OnUpdate(trigger, actor)
	return true
end

function underCamera_bz01_08_OnLeave(trigger, actor)
	return true
end

-- 7. 9:1 상, 하 비율 카메라
function underCamera_bz01_07_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	SetCameraSetting(Point3(0, 596, 170), Point3(0, 0, 148))
	g_world:SetCameraMode(1, actor)
	return true
end

function underCamera_bz01_07_OnUpdate(trigger, actor)
	return true
end

function underCamera_bz01_07_OnLeave(trigger, actor)
	return true
end

-- 6. Zoom In 카메라
function underCamera_bz01_06_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	SetCameraSetting(Point3(0, 308, 154), Point3(0, 0, 56))
	g_world:SetCameraMode(1, actor)
	return true
end

function underCamera_bz01_06_OnUpdate(trigger, actor)
	return true
end

function underCamera_bz01_06_OnLeave(trigger, actor)
	return true
end

-- 5. 아래로 떨어지는 카메라
function underCamera_bz01_05_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	SetCameraSetting(Point3(0, 564, 138), Point3(0, 0, -84))
	g_world:SetCameraMode(1, actor)
	return true
end

function underCamera_bz01_05_OnUpdate(trigger, actor)
	return true
end

function underCamera_bz01_05_OnLeave(trigger, actor)
	return true
end

-- 4. 앞 뒤로 나오는 사각벽 카메라
function underCamera_bz01_04_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	SetCameraSetting(Point3(-84, 324, 142), Point3(32, 0, 48))
	g_world:SetCameraMode(1, actor)
	return true
end

function underCamera_bz01_04_OnUpdate(trigger, actor)
	return true
end

function underCamera_bz01_04_OnLeave(trigger, actor)
	return true
end

-- 2. 앞, 뒤 주먹 바이킹 카메라
function underCamera_bz01_02_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	SetCameraSetting(Point3(-56, 356, 178), Point3(48, 0, 68))
	g_world:SetCameraMode(1, actor)
	return true
end

function underCamera_bz01_02_OnUpdate(trigger, actor)
	return true
end

function underCamera_bz01_02_OnLeave(trigger, actor)
	return true
end

-- 1. 기본 카메라
function underCamera_bz01_01_OnEnter(trigger, actor)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	SetCameraSetting(Point3(0, 388, 138), Point3(0, 0, 80))
	g_world:SetCameraMode(1, actor)
	return true
end

function underCamera_bz01_01_OnUpdate(trigger, actor)
	return true
end

function underCamera_bz01_01_OnLeave(trigger, actor)
	return true
end

function NormalCamera200()
	ODS("NormalCamera200\n")
	if bMovieCamera200 == nil then
		return true
	end

	if bMovieCamera200 == true then	-- 무비 카메라가 켜졌으면
		bMovieCamera200 = false
	else
		return
	end

	pilot = g_pilotMan:FindPilot(g_playerInfo.guidPilot)
	if pilot:IsNil() == true then
		return
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	ODS("NormalCamera200 In\n")
	g_world:SetCameraMode(1, pilot:GetActor())
	CallUI("FRM_MOVIE_OUT")
end
