local g_MiningTime = 60 * 8
local g_MiningTimeEnd = 0

local g_MiningTime_Wnd = {}

function FRM_MINING_TIME()
    ActivateUI("FRM_MINING_TIME")
end

function FRM_MODE7_NEXTWAVE()
    ActivateUI("FRM_MODE7_NEXTWAVE")
end

function MiningStartCover()
    local kTopWnd = ActivateUI("FRM_MISSION_STAGE_START")
	if kTopWnd:IsNil() then
		return
	end

	local kTextWnd = kTopWnd:GetControl("FRM_TEXT")
	if kTextWnd:IsNil() then
		return
	end

	kTextWnd:SetStaticText(GetTT(401240):GetStr())
end

function MiningTimerCall(kSelf)
    for i = 0, 2 do
        g_MiningTime_Wnd[i] = UISelf:GetControl("FRM_CLOCK" .. i)
    end
    g_MiningTimeEnd = GetAccumTime() + g_MiningTime
    MiningTimerTick(kSelf)
end

function MiningTimerTick(kSelf)
    local kTime = math.floor(g_MiningTimeEnd - GetAccumTime())
    if kTime <= 0 then
        kSelf:Close()
        return
    end

    if kTime <= 20 then
        PlaySoundByID("Bomb_Clock")
    end

    local clocks = {}
    clocks[0] = math.floor(kTime % 1000 / 100)
    clocks[1] = math.floor(kTime % 100 / 10)
    clocks[2] = math.floor(kTime % 10)
    if clocks[2] >= 10 then
        clocks[2] = 0
    end

    for i = 0, 2 do
        local kWnd = g_MiningTime_Wnd[i]
        if false == kWnd:IsNil() and clocks[i] >= 0 then
            kWnd:Visible(true)
            kWnd:SetUVIndex(clocks[i] + 1)
        end
    end

end