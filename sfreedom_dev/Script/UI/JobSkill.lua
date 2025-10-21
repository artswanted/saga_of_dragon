JOBSKILL_LEARN_COUNT = 5

function JobSkillLearn_Select(kSelect)
	local kParent = kSelect:GetParent()
	if true == kParent:IsNil() then
		return
	end
	local iIndex = kSelect:GetBuildIndex()
	for iCount = 0, (JOBSKILL_LEARN_COUNT-1) do
		local kViewSelect = kParent:GetControl("FRM_SELECT"..iCount)
		local kViewLearn = kParent:GetControl("FRM_LEARN"..iCount)
		if true == kViewSelect:IsNil() then
			return
		end
		if true == kViewLearn:IsNil() then
			return
		end
		local iJobSkillNo = kViewLearn:GetCustomDataAsInt()
		if 0 == iJobSkillNo then
			return
		end
		if iIndex == iCount then
			if true == kViewSelect:IsVisible() then
				kViewSelect:Visible(false)
				JobSkillLearn_SetPrice(0)
			else
				kViewSelect:Visible(true)
				local iSkillNo = kSelect:GetCustomDataAsInt()
				JobSkillLearn_SetPrice(iSkillNo)
			end
		else
			kViewSelect:Visible(false)
		end
	end
end

function JobSkillLearn_Select_Clear()
	local kMain = GetUIWnd("SFRM_JOB_SKILL_LEARN")
	if true == kMain:IsNil() then
		return
	end
	local kShadow = kMain:GetControl("SFRM_LEARN_SHADOW")
	if true == kShadow:IsNil() then
		return
	end
	
	for iCount = 0, (JOBSKILL_LEARN_COUNT-1) do
		local kViewSelect = kShadow:GetControl("FRM_SELECT"..iCount)
		if true == kViewSelect:IsNil() then
			return
		end
		kViewSelect:Visible(false)
	end
	JobSkillLearn_SetPrice(0)
end

function JobSkillSpotParticleName()
	return "ef_set_item_03_b"
end

g_kJobSkillGatherTimer = 0
g_kJobSkillGatherTurn = 0
function CallJobSkillGatherTimer(iTurnTime, iExpertness, iCurExhaustion, iMaxExhaustion )
	g_kJobSkillGatherTurn = iTurnTime
	local kMain = ActivateUI("FRM_JOB_GATHER_PROGRESS")
	SetJSGT_Expertness(kMain, iExpertness)
	SetJSGT_Exhaustion(kMain, iCurExhaustion, iMaxExhaustion)
end
function SetJSGT_Expertness(kMain, iExpertness)
	if true == kMain:IsNil() then
		return
	end
	local kText = kMain:GetControl("FRM_EXPERTNESS")
	if false == kText:IsNil() then
	 kText:SetStaticText( tostring(iExpertness) )
	end
end
function SetJSGT_Exhaustion(kMain, iCurExhaustion, iMaxExhaustion)
	if true == kMain:IsNil() then
		return
	end
	local kExhaustion = kMain:GetControl("FRM_EXHAUSTION")
	if true == kExhaustion:IsNil() then
		return
	end
	
	if 0 == iCurExhaustion then
		kExhaustion:Visible(false)
		return
	else
		kExhaustion:Visible(true)
	end
	
	local fExhaustionLevel = iCurExhaustion / iMaxExhaustion * 10
	local iExhaustionLevel = math.floor(fExhaustionLevel)
	ODS("ExhaustionLevel = "..(iExhaustionLevel) ..("\n"), false, 912)
	if 0 ~= iCurExhaustion and 0 == iExhaustionLevel then
		kExhaustion:SetUVIndex(1)
	else
		kExhaustion:SetUVIndex(iExhaustionLevel)
	end
end
function JobSkillGatherTimerSet()
	g_kJobSkillGatherTimer = GetAccumTime()
end
function JobSkillGatherTimerTick(UISelf)
	if true == UISelf:IsNil() then
		return
	end
	if 0 == g_kJobSkillGatherTimer 
	and 0 == g_kJobSkillGatherTurn then
		ODS( GetAccumTime() .. " Timer None \n", false, 912 )
		return
	end
	if UISelf:IsNil() == false then
		local fTime = GetAccumTime() - g_kJobSkillGatherTimer
		local iTime = math.floor(fTime)
		if g_kJobSkillGatherTurn > iTime then
			local fIndex = iTime / g_kJobSkillGatherTurn * 10
			local iIndex = math.floor(fIndex)
			UISelf:SetUVIndex(iIndex + 1)
		else
			UISelf:SetUVIndex(10)
		end
	end
end
function CloseJobSkillGatherTimer()
	CloseUI("FRM_JOB_GATHER_PROGRESS")
	g_kJobSkillGatherTimer = 0
	g_kJobSkillGatherTurn = 0
end

function SetJobSkillFilterSize(kListWnd, kList, kItem)
	if true == kListWnd:IsNil() or
	   true == kList:IsNil() or
	   true == kItem:IsNil() then
		return false
	end
	
	local kItemWnd = kItem:GetWnd()
	if true == kItemWnd:IsNil() then
		return false
	end
	
	iBuildIndex = kList:GetListItemCount()
	local ptItemSize = kItemWnd:GetSize()
	
	kList:SetSize(Point2(ptItemSize:GetX(), ptItemSize:GetY()*iBuildIndex ))
	kListWnd:SetSize(Point2(ptItemSize:GetX(), ptItemSize:GetY()*iBuildIndex ))
	return true
end


-------------------------------------------------------------------------------------

function Get_JS3ResultRateUVIdx(iRate)
	if		iRate > 3000 	then		return 6
	elseif 	iRate > 2000 	then		return 5
	elseif 	iRate > 1200 	then		return 4
	elseif 	iRate > 600 	then		return 3
	elseif 	iRate > 200 	then		return 2
	end
	return 1
end

function OnClose_SOULABIL_TRANSFER()
	CloseToolTip()
	StopSoundByID("soulpower-gauge")
	CloseUI("Inv")
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if nil~=g_NpcManu_MapNo and g_NpcManu_MapNo==g_world:GetMapNo() then
		if nil==g_NpcManu_OpenPos or g_NpcManu_OpenPos:Distance(GetMyActor():GetPos()) < 160 then
			CallUI("SFRM_CONFIRM_SOULABIL_TRANSFER")
		end
	end
end
