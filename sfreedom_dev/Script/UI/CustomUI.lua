TBL_CUSTOMUI = {}
TBL_CUSTOMUI[1]	= { DIR=DIR_UP,		WND="FMA_RECALL",	ACTION_NAME="a_Recall" }
TBL_CUSTOMUI[2]	= { DIR=DIR_LEFT,	WND="FMA_REGAIN",	ACTION_NAME="a_Regain" }	
TBL_CUSTOMUI[3]	= { DIR=DIR_RIGHT,	WND="FMA_PROTECT",	ACTION_NAME="a_summon_attack" }	
TBL_CUSTOMUI[4]	= { DIR=DIR_DOWN,	WND="FMA_IMPECT",	ACTION_NAME="a_Impact_Unsummon" }
g_eCustomUISelectDir = DIR_NONE

function CustomUISelect(kWnd, eDir)
	if nil==kWnd or kWnd:IsNil() then
		return
	end
	
	if DIR_NONE~=eDir and g_eCustomUISelectDir==eDir then
		ODS("SELECT\n")
		for i=1, table.getn(TBL_CUSTOMUI) do
			if TBL_CUSTOMUI[i].DIR==eDir then
				CustomUISummoner_DoAction(TBL_CUSTOMUI[i].ACTION_NAME)
				RegistESCScript('')
				break
			end
		end

		return
	end
	
	--배웠는지 체크
	for i=1, table.getn(TBL_CUSTOMUI) do
		if TBL_CUSTOMUI[i].DIR==eDir then
			local iSkillNo = GetMyActor():GetAction():GetActionNoByActionID(TBL_CUSTOMUI[i].ACTION_NAME)
			if GetMyActor():CheckSkillExist(iSkillNo) == 0 then
				CustomUISelect(kWnd, 0)
				return
			end
		end
	end

	g_eCustomUISelectDir = eDir
	for i=1, table.getn(TBL_CUSTOMUI) do
		local kChild = kWnd:GetControl(TBL_CUSTOMUI[i].WND)
		if false==kChild:IsNil() then
			kChild:GetControl("IMG_SELECT"):Visible(TBL_CUSTOMUI[i].DIR==eDir)
		end
	end
	
	PlaySoundByID( "sum_menu" )
end

function CheckCustomUISkill(kWnd)
	if nil==kWnd or kWnd:IsNil() then
		return
	end

	for i=1, table.getn(TBL_CUSTOMUI) do
		local kChild = kWnd:GetControl(TBL_CUSTOMUI[i].WND)
		if false==kChild:IsNil() then
			local iSkillNo = GetMyActor():GetAction():GetActionNoByActionID(TBL_CUSTOMUI[i].ACTION_NAME)
			local bGrayScale = GetMyActor():CheckSkillExist(iSkillNo) == 0
			kChild:GetControl("IMG_SKILL"):SetGrayScale(bGrayScale)
		end
	end
end

function empty_fun()
	return true
end

g_kPickupActorGuid = nil
function SetPickupActorGuid(kGuid)
	g_kPickupActorGuid = kGuid
end

function GetPickupActorGuid()
	if nil == g_kPickupActorGuid then
		return GUID('')
	end
	return g_kPickupActorGuid
end

function ShowEvadeUI_Tick(kSelf)
	if nil==kSelf or true==kSelf:IsNil() then
		return false
	end
	local kEvade1 = kSelf:GetControl("FRM_EVADE_1")
	local kEvade2 = kSelf:GetControl("FRM_EVADE_2")
	if nil==kEvade1 or true==kEvade1:IsNil() then
		return false
	end
	if nil==kEvade2 or true==kEvade2:IsNil() then
		return false
	end
	local fTickTimer = GetCurrentTimeInSec() - kSelf:GetCustomDataAsFloat()
	if 0.1 < fTickTimer then
		local bShowEvade2 = kEvade1:IsVisible()
		kEvade1:Visible( not bShowEvade2 )
		kEvade2:Visible( bShowEvade2 )
		kSelf:SetCustomDataAsFloat( GetCurrentTimeInSec() )
	end
end
