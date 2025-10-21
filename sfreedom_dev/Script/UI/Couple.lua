g_kSweetHeartMoney = 10
g_fMarryColseTime = 0

function CallCouplePopup(kSelf)
	if nil == kSelf or kSelf:IsNil() then return end
	
	local kGuid = kSelf:GetOwnerGuid()
	local kMyGuid = g_pilotMan:GetPlayerPilotGuid()
	local kCursorPos = GetCursorPos()
	
	local kCmdArray = {}
	--kCmdArray[1] = "REQ_COUPLE"
	kCmdArray[2] = "REQ_INSTANCE_COUPLE"
	kCmdArray[3] = "INVITE_PARTY"
	kCmdArray[4] = "ADD_FRIEND"
	
	CallCommonPopup( kCmdArray, kGuid, nil, kCursorPos)
end

function CallFindCouplePopup(kSelf)
	if nil == kSelf or kSelf:IsNil() then return end
	
	local kGuid = kSelf:GetOwnerGuid()
	local kMyGuid = g_pilotMan:GetPlayerPilotGuid()
	local kCursorPos = GetCursorPos()
	
	local kCmdArray = {}
	kCmdArray[1] = "REQ_COUPLE"
	kCmdArray[2] = "REQ_INSTANCE_COUPLE"
	kCmdArray[3] = "INVITE_PARTY"
	kCmdArray[4] = "ADD_FRIEND"
	
	CallCommonPopup( kCmdArray, kGuid, nil, kCursorPos)
end

function UpdateSweetHeartPlayUI(kSelf)
	if nil == kSelf or kSelf:IsNil() then return end
	
	local text1 = kSelf:GetControl("FRM_SWEETHEART_TEXT1")
	if text1:IsNil() == false then
		local loc = text1:GetLocation()
		if loc:GetX() <= 362 then
			text1:SetLocation( Point2(loc:GetX()+26, loc:GetY()) )
		end		
	end
	local text2 = kSelf:GetControl("FRM_SWEETHEART_TEXT2")
	if text2:IsNil() == false then
		local loc = text2:GetLocation()
		if loc:GetX() >= 662 then
			text2:SetLocation( Point2(loc:GetX()-26, loc:GetY()) )	
		end
	end	
	local text3 = kSelf:GetControl("FRM_SWEETHEART_TEXT3")
	if text3:IsNil() == false then
		local loc = text3:GetLocation()
		if loc:GetY() >= 160 then
			text3:SetLocation( Point2(loc:GetX(), loc:GetY()-26) )
		end
	end
	
	local cha1 = kSelf:GetControl("IMG_CHA1")
	if cha1:IsNil() == false then
		local loc = cha1:GetLocation()		
		if 0 == cha1:GetCustomDataAsInt() then
			if 1 ~= cha1:GetAlpha() then
				if loc:GetY() >= -20 then
					cha1:SetLocation( Point2(loc:GetX(), loc:GetY()-40) )
					cha1:SetMaxAlpha(1)
					cha1:SetAlpha((loc:GetY()*0.001))
				else
					cha1:SetMaxAlpha(1)
					cha1:SetAlpha(1)				
				end
			else
				if loc:GetY() <= -10 then
					cha1:SetLocation( Point2(loc:GetX(), loc:GetY()+20) )
				else
					cha1:SetCustomDataAsInt(1)
				end
			end
		elseif 1 == cha1:GetCustomDataAsInt() then
			if loc:GetY() >= 0 then
				cha1:SetLocation( Point2(loc:GetX(), loc:GetY()-20) )
			else
				cha1:SetCustomDataAsInt(2)
			end
		elseif 2 == cha1:GetCustomDataAsInt() then
			if loc:GetX() <= 390 then
				cha1:SetLocation( Point2(loc:GetX()+10, loc:GetY()) )
			else
				cha1:SetCustomDataAsInt(3)				
			end
		elseif 3 == cha1:GetCustomDataAsInt() then	
			if loc:GetX() >= 390 then
				cha1:SetLocation( Point2(383, loc:GetY()) )
			else
				cha1:SetCustomDataAsInt(4)								
			end
		elseif 4 == cha1:GetCustomDataAsInt() then	
			local heart = kSelf:GetControl("IMG_HEART")
			heart:Visible(true)
			if heart:IsNil() == false then
				if heart:GetSizeScale() <= 10.0 then
					heart:SetSizeScale(heart:GetSizeScale()+0.2, true)
					heart:SetAlpha(4.0-(heart:GetSizeScale()+0.2))
				else
					heart:SetAlpha(0)
					heart:Visible(false)
					cha1:SetCustomDataAsInt(5)
				end
			end
		elseif 5 == cha1:GetCustomDataAsInt() then
			--CloseUI("FRM_SWEETHEART_PLAY")
		end
	end
	local cha2 = kSelf:GetControl("IMG_CHA2")
	if cha2:IsNil() == false then
		local loc = cha2:GetLocation()		
		if 0 == cha2:GetCustomDataAsInt() then
			if 1 ~= cha2:GetAlpha() then
				if loc:GetY() >= -20 then
					cha2:SetLocation( Point2(loc:GetX(), loc:GetY()-40) )
					cha2:SetMaxAlpha(1)
					cha2:SetAlpha((loc:GetY()*0.001))
				else
					cha2:SetMaxAlpha(1)
					cha2:SetAlpha(1)						
				end		
			else
				if loc:GetY() <= -10 then
					cha2:SetLocation( Point2(loc:GetX(), loc:GetY()+20) )
				else
					cha2:SetCustomDataAsInt(1)
				end
			end
		elseif 1 == cha2:GetCustomDataAsInt() then
			if loc:GetY() >= 0 then
				cha2:SetLocation( Point2(loc:GetX(), loc:GetY()-20) )
			else
				cha2:SetCustomDataAsInt(2)	
			end
		elseif 2 == cha2:GetCustomDataAsInt() then
			if loc:GetX() >= 500 then
				cha2:SetLocation( Point2(loc:GetX()-10, loc:GetY()) )
			else
				cha2:SetCustomDataAsInt(3)					
			end
		elseif 3 == cha2:GetCustomDataAsInt() then	
			if loc:GetX() <= 500 then
				cha2:SetLocation( Point2(512, loc:GetY()) )
			else
				cha2:SetCustomDataAsInt(4)					
			end	
		elseif 4 == cha1:GetCustomDataAsInt() then	
		end
	end
end

--//////////////////////////////// OnSelectParty_Level ////////////////////////////////
function OnSelectSweetHeart_Money(kVal)
	g_kSweetHeartMoney = kVal
	GetUIWnd("SFRM_SWEETHEART_REQ"):GetControl("SFRM_BG_MONEY"):SetStaticText(tostring(kVal))
end

function SweetHeart_TotalMoney(iTotalMoney)
	local mainform = CallUI("SFRM_SWEETHEART_MONEY")
	
	if mainform:IsNil() == true then
		return true
	end 

	totalform = mainform:GetControl("FRM_TOTAL_MONEY"):GetControl("SFRM_BG_TOTAL_MONEY")
	if totalform:IsNil() == true then
		return true
	end
	local money = tostring(iTotalMoney)
	totalform:SetStaticText(money)
end

function OnDisplayer_CoupleTime(kSelf)
	CheckSweetHeartQuestTime()
	local iCouplePlayTime = math.floor( GetMyCouplePlayTime() )
	
	local iSec1 = math.floor(iCouplePlayTime%10)
	local iSec10 = math.floor(iCouplePlayTime%60)/10
	local iMin1 = math.floor(iCouplePlayTime/60)%10
	local iMin10 = math.floor((iCouplePlayTime/60)%60)/10
	local iHour1 = math.floor(iCouplePlayTime/3600)%10
	local iHour10 = math.floor(iCouplePlayTime/3600)/10
	
	kSelf:GetControl("FRM_HOUR10"):SetUVIndex(iHour10+1)			
	kSelf:GetControl("FRM_HOUR1"):SetUVIndex(iHour1+1)
	kSelf:GetControl("FRM_MIN10"):SetUVIndex(iMin10+1)
	kSelf:GetControl("FRM_MIN1"):SetUVIndex(iMin1+1)
	kSelf:GetControl("FRM_SEC10"):SetUVIndex(iSec10+1)
	kSelf:GetControl("FRM_SEC1"):SetUVIndex(iSec1+1)
	
	kSelf:GetControl("FRM_HOUR10"):SetInvalidate(true)
	kSelf:GetControl("FRM_HOUR1"):SetInvalidate(true)
	kSelf:GetControl("FRM_MIN10"):SetInvalidate(true)
	kSelf:GetControl("FRM_MIN1"):SetInvalidate(true)
	kSelf:GetControl("FRM_SEC10"):SetInvalidate(true)
	kSelf:GetControl("FRM_SEC1"):SetInvalidate(true)
	
	local iMinLocationY = 600
	local kPos = kSelf:GetLocation()
	if iMinLocationY < kPos:GetY() then
		kPos:SetY(iMinLocationY)
		kSelf:SetLocation( kPos )
	end
end

function SweetHeartNPC_Talk(iTextNo)
	SweetHeartNPC_TalkCall(iTextNo, "84A2C16C-47D9-4595-947A-90471CE887EE")
end

function SweetHeartVillageNPC_Talk(kCharName, kCoupleName)
	SweetHeartVillageNPC_TalkCall(kCharName, kCoupleName, "C00688BF-C60B-4e43-B373-6A9070612D2E")
end

