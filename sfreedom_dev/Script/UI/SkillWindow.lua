
g_bSkill_Window_Initialized = false;
g_UI_SKILLWND_CurrentSkillClassID = 0;
g_UI_SKILLWND_CurrentSkillTreeFormIndex = 0;
g_UI_SKILLWND_BaseWnd = nil;
g_iSkill_Window_TabNo = 0
g_iSkill_MyClass = 0

function GetGroggyAccPointLv(iPoint)
	if 1450 <= iPoint then			return 15
	elseif 1350 <= iPoint then		return 14
	elseif 1250 <= iPoint then		return 13
	elseif 1150 <= iPoint then		return 12
	elseif 1050 <= iPoint then		return 11
	elseif 950 <= iPoint then		return 10
	elseif 850 <= iPoint then		return 9
	elseif 750 <= iPoint then		return 8
	elseif 650 <= iPoint then		return 7
	elseif 550 <= iPoint then		return 6
	elseif 450 <= iPoint then		return 5
	elseif 350 <= iPoint then		return 4
	elseif 250 <= iPoint then		return 3
	elseif 150 <= iPoint then		return 2
	elseif 20 <= iPoint then		return 1
	end
	return 0
end

function GetGroggyRelaxRateLv(iRate)
	if 10000 <= iRate then			return 12
	elseif 9500 <= iRate then		return 11
	elseif 9000 <= iRate then		return 10
	elseif 8500 <= iRate then		return 9
	elseif 8000 <= iRate then		return 8
	elseif 7500 <= iRate then		return 7
	elseif 7000 <= iRate then		return 6
	elseif 6500 <= iRate then		return 5
	elseif 6000 <= iRate then		return 4
	elseif 5500 <= iRate then		return 3
	elseif 5000 <= iRate then		return 2
	elseif 4500 <= iRate then		return 1
	end
	return 0
end


function UI_SKILLWND_ON_INIT(wnd)
    ODS("UI_SKILLWND_ON_INIT\n");

	if wnd ~= nil then
		g_UI_SKILLWND_BaseWnd = wnd;
	else
		if g_UI_SKILLWND_BaseWnd == nil then
			return
		end
		wnd = g_UI_SKILLWND_BaseWnd;
	end
	
	if g_bSkill_Window_Initialized == false then
		UI_SKILLWND_ResetAll(wnd);
		UI_SKILLWND_Set_Class_Buttons(wnd);
		UI_SKILLWND_Init_Skills(wnd);
		UI_SKILLWND_SetSkillPoint(wnd);
		
		--	첫번째 계열 버튼을 선택하자.
		UI_SKILLWND_SKILL_CLASS_BUTTON_CLICK(1);
		--  뷰 방식을 아이콘방식으로 선택한다.
		UI_SKILLWND_CHANGE_VIEWTYPE_TO_LIST();
	end
	
	g_bSkill_Window_Initialized = true;
	g_iSkill_Window_TabNo = 0
	g_iSkill_MyClass = 0
end

function UI_SKILLWND_SetSkillPoint(wnd)
    ODS("UI_SKILLWND_SetSkillPoint\n");
	local	kSkillTree = GetSkillTree();

	local	kControl = wnd:GetControl("SFRM_REMAIN_SP"):GetControl("SFRM_SHADOW"):GetControl("SP");

	kControl:SetStaticTextW(WideString(""..kSkillTree:GetRemainSkillPoint().." SP"));

	if 	kSkillTree:IsTemporaryRemainSkillPoint() == true then
		kControl:SetFontColor(0xff8811);
	else
		kControl:SetFontColor(0x4D3413);
	end

end

function	UI_SKILLWND_ON_CLASS_CHANGED()		--	전직 했다. 
    ODS("UI_SKILLWND_ON_CLASS_CHANGED\n");
    
    if g_bSkill_Window_Initialized == false then
        return
    end
    
	g_bSkill_Window_Initialized = false;
	UI_SKILLWND_ON_INIT();
	
end

function UI_SKILLWND_RenameIcon(kIconTableControl)

	ODS("UI_SKILLWND_RenameIcon\n");

	local	iIconKey = 0;
	
	local	iTotalIcon = kIconTableControl:GetContCount();
	
	local	iCount = 0;
	
	while iCount<iTotalIcon do
	
		local	kIcon = kIconTableControl:GetContAt(iCount);
		if kIcon:IsIcon() then
				
			kIcon:SetIconKeyAndResNo(0, 0);
			
			local	kNewName = "SKILLICON"..iIconKey;
			ODS("kNewName : "..kNewName.."\n");
			
			kIcon:SetID(WideString(kNewName));
		
			iIconKey=iIconKey+1;
		
		end
		
		iCount = iCount + 1;		
	end

end


function UI_SKILLWND_AddNewSkillToIconFrm(kIconFrmControl,iSkill,kSkillTree)
    ODS("UI_SKILLWND_AddNewSkillToIconFrm\n");

	local iKeySkillNo = kSkillTree:GetKeySkillNoByIndex(iSkill);
	local iKeySkillRes = kSkillTree:GetKeySkillResNoByIndex(iSkill);
	if iKeySkillNo == 0 then
		return
	end
	
	
	local	iIndex = kIconFrmControl:GetCustomDataAsInt();
	ODS("UI_SKILLWND_AddNewSkillToIconFrm iIndex:"..iIndex.."\n");
	
	local	kIconTable = kIconFrmControl:GetControl("ICON_TABLE");
	
	if kIconTable:IsNil() then
		return
	end

	local	iMaxIconCount = 5*5;	
	
	if iIndex>=iMaxIconCount then
		return
	end
	
	ODS("UI_SKILLWND_AddNewSkillToIconFrm kIconTable:GetControl Index :"..iIndex.."\n");
	
	local	kIcon = kIconTable:GetControl("SKILLICON"..iIndex);
	if kIcon:IsNil() == false then
	
		ODS("UI_SKILLWND_AddNewSkillToIconFrm SetIconKey:"..iKeySkillNo.."\n");
		kIcon:SetIconKeyAndResNo(iKeySkillNo, iKeySkillRes);
	    kIcon:SetCanDrag(kSkillTree:IsActiveSkill(iKeySkillNo));		
		
	end
	
	kIconFrmControl:SetCustomDataAsInt(iIndex+1);
	
end

function UI_SKILLWND_AddNewSkillToList(kListControl,iSkillIndex,kSkillTree)
    ODS("UI_SKILLWND_AddNewSkillToList\n");

	local iKeySkillNo = kSkillTree:GetKeySkillNoByIndex(iSkillIndex);
	local iKeySkillRes = kSkillTree:GetKeySkillResNoByIndex(iSkillIndex);
	if iKeySkillNo == 0 then
		return
	end
	
	
	local	kNewListItem = kListControl:AddNewListItem(WideString("SKILL_LIST_ITEM_"..iKeySkillNo));
	if kNewListItem:IsNil() then
		return
	end
	
	local	kNewListItemWnd = kNewListItem:GetWnd();
	
	kNewListItemWnd:SetCustomDataAsInt(iKeySkillNo);

	kNewListItemWnd:SetStaticTextW(WideString(""));
	
	local	kSkillIcon = kNewListItemWnd:GetControl("SFRM_ICON_BG"):GetControl("SkillIcon");
	kSkillIcon:SetIconKeyAndResNo(iKeySkillNo, iKeySkillRes);
	
	kSkillIcon:SetCanDrag(kSkillTree:IsActiveSkill(iKeySkillNo));
	
	
end

function	UI_SKILLWND_CHANGE_VIEWTYPE_TO_LIST()
    ODS("UI_SKILLWND_CHANGE_VIEWTYPE_TO_LIST\n");

	local	i=0;
	while i<4 do
	
		local	kViewTypeFrmList = g_UI_SKILLWND_BaseWnd:GetControl("SFRM_SKILL_BG_0"..(i+1)):GetControl("SFRM_SDW"):GetControl("CBTN_VIEW_LIST");
		kViewTypeFrmList:CheckState(true);
		kViewTypeFrmList:LockClick(true);
        kViewTypeFrmList:GetParent():GetControl("CBTN_VIEW_ICON"):LockClick(false)
        kViewTypeFrmList:GetParent():GetControl("CBTN_VIEW_ICON"):CheckState(false)
        kViewTypeFrmList:GetParent():GetControl("FRM_ICON"):Visible(false)
        kViewTypeFrmList:GetParent():GetControl("LST_LIST"):Visible(true)				
	
		i=i+1;
	end
	
end
function	UI_SKILLWND_CHANGE_VIEWTYPE_TO_ICON()
    ODS("UI_SKILLWND_CHANGE_VIEWTYPE_TO_ICON\n");

	local	i=0;
	while i<4 do
	
		local	kViewTypeFrmList = g_UI_SKILLWND_BaseWnd:GetControl("SFRM_SKILL_BG_0"..(i+1)):GetControl("SFRM_SDW"):GetControl("CBTN_VIEW_ICON");
		kViewTypeFrmList:CheckState(true);
		kViewTypeFrmList:LockClick(true);
        kViewTypeFrmList:GetParent():GetControl("CBTN_VIEW_LIST"):LockClick(false)
        kViewTypeFrmList:GetParent():GetControl("CBTN_VIEW_LIST"):CheckState(false)
        kViewTypeFrmList:GetParent():GetControl("FRM_ICON"):Visible(true)
        kViewTypeFrmList:GetParent():GetControl("LST_LIST"):Visible(false)				
	
		i=i+1;
	end
	
end

function UI_SKILLWND_Init_Skills(wnd)
    ODS("UI_SKILLWND_Init_Skills\n");

	--	현재 클래스가 쓰거나 배울수 있는 모든 스킬을 윈도우에 추가하자. 
	
	local	kMyPilot = GetMyPilot()
	local	kSkillTree	=	GetSkillTree();


	local	kListControl = wnd:GetControl("SFRM_SKILL_BG_01"):GetControl("SFRM_SDW"):GetControl("LST_LIST");
	local	kIconFrmControl = wnd:GetControl("SFRM_SKILL_BG_01"):GetControl("SFRM_SDW"):GetControl("FRM_ICON");

	local	iTotalSkill = kSkillTree:GetTotalSkill();
	local	iSkill = 0;
	while iSkill<iTotalSkill do
	
		local	iKeySkillNo = kSkillTree:GetKeySkillNoByIndex(iSkill);
	
		UI_SKILLWND_AddNewSkillToList(kListControl,iSkill,kSkillTree);
		UI_SKILLWND_AddNewSkillToIconFrm(kIconFrmControl,iSkill,kSkillTree);
		UI_SKILLWND_Refresh_Skill(iKeySkillNo,kListControl,kIconFrmControl);
	
		iSkill = iSkill+1;
	
	end
		
	
end

function	UI_SKILLWND_Refresh_All()
    ODS("UI_SKILLWND_Refresh_All\n");


	if g_bSkill_Window_Initialized == false then	--	윈도우가 생성되지 않은 상태라면, 리턴!
		return	;
	end

	local	kMyPilot = GetMyPilot()
	local	kSkillTree	=	GetSkillTree();

	local	kListControl = g_UI_SKILLWND_BaseWnd:GetControl("SFRM_SKILL_BG_01"):GetControl("SFRM_SDW"):GetControl("LST_LIST");
	local	kIconFrmControl = g_UI_SKILLWND_BaseWnd:GetControl("SFRM_SKILL_BG_01"):GetControl("SFRM_SDW"):GetControl("FRM_ICON");

	local	iTotalSkill = kSkillTree:GetTotalSkill();
	local	iSkill = 0;
	while iSkill<iTotalSkill do
	
		UI_SKILLWND_Refresh_Skill(kSkillTree:GetKeySkillNoByIndex(iSkill),kListControl,kIconFrmControl);
	
		iSkill = iSkill+1;
	
	end
	
	UI_SKILLWND_SetSkillPoint(g_UI_SKILLWND_BaseWnd);
	UI_SKILLWND_SetButtonVisible();
	
end

function	UI_SKILLWND_ON_NEW_SKILL_LEARNED(iNewSkillNo)
    ODS("UI_SKILLWND_ON_NEW_SKILL_LEARNED\n");
    if g_UI_SKILLWND_BaseWnd == nil then
        return
    end

	local	kSkillTree = GetSkillTree();
	local	iKeySkillNo = kSkillTree:GetKeySkillNo(iNewSkillNo);
	
	local	kListControl = g_UI_SKILLWND_BaseWnd:GetControl("SFRM_SKILL_BG_01"):GetControl("SFRM_SDW"):GetControl("LST_LIST");
	local	kIconFrmControl = g_UI_SKILLWND_BaseWnd:GetControl("SFRM_SKILL_BG_01"):GetControl("SFRM_SDW"):GetControl("FRM_ICON");
	
	UI_SKILLWND_Refresh_Skill(iKeySkillNo,kListControl,kIconFrmControl);
	UI_SKILLWND_SetSkillPoint(g_UI_SKILLWND_BaseWnd);

	UI_SKILLWND_SetButtonVisible();
	
end

function	UI_SKILLWND_Refresh_Skill(iKeySkillNo,kListControl,kIconFrmControl)
    ODS("UI_SKILLWND_Refresh_Skill\n");

	local	kListItem = kListControl:ListFindItemW(WideString("SKILL_LIST_ITEM_"..iKeySkillNo));
	if kListItem:IsNil() then
		return
	end
	
	
	local	kListItemWnd = kListItem:GetWnd();
	
	local	kSkillTree = GetSkillTree();
	local	kSkillName = kSkillTree:GetSkillName(iKeySkillNo);
	
	if kSkillName:IsNil() then
		return
	end	
	local   bLearned = kSkillTree:IsLearnedSkill(iKeySkillNo);
	
	kListItemWnd:GetControl("SFRM_SKILL_NAME"):SetStaticTextW(kSkillName);
	if bLearned then
	    kListItemWnd:GetControl("SFRM_SKILL_NAME"):SetFontColorRGBA(0,0,0,255)
	else
	    kListItemWnd:GetControl("SFRM_SKILL_NAME"):SetFontColorRGBA(250,250,250,0)
	end
	
	if(0 < kSkillTree:GetMaxSkillLevel(iKeySkillNo)) then
		kListItemWnd:GetControl("SFRM_SKILL_LEVEL"):SetStaticTextW(GetAppendTextW(GetTextW(224),WideString(" "..kSkillTree:GetSkillLevel(iKeySkillNo).."/"..kSkillTree:GetMaxSkillLevel(iKeySkillNo))));
	else
		kListItemWnd:GetControl("SFRM_SKILL_LEVEL"):SetStaticText("");
	end

	if 	kSkillTree:IsTemporaryLevelChanged(iKeySkillNo) == true then
	
		kListItemWnd:GetControl("SFRM_SKILL_LEVEL"):SetFontColor(0xff8811);
	else
		kListItemWnd:GetControl("SFRM_SKILL_LEVEL"):SetFontColor(0x4D3413);
	
	end

	local	iNeedSP = kSkillTree:GetNeedSkillPoint(iKeySkillNo);
	if iNeedSP == -1 then
		kListItemWnd:GetControl("SFRM_NEED_SKILL_POINT"):Visible(false);
	else
		kListItemWnd:GetControl("SFRM_NEED_SKILL_POINT"):Visible(true);
		kListItemWnd:GetControl("SFRM_NEED_SKILL_POINT"):SetStaticTextW(GetAppendTextW(WideString(""..iNeedSP),GetTextW(223)));	
	end
	
	local	kPointUpBtn = kListItemWnd:GetControl("POINT_UP");
	local	kPointDownBtn = kListItemWnd:GetControl("POINT_DOWN");
	
	kPointUpBtn:Visible(kSkillTree:CanLevelUp(iKeySkillNo));
	kPointDownBtn:Visible(kSkillTree:CanLevelDown(iKeySkillNo));
	
	
	local	kSkillIcon = kListItemWnd:GetControl("SFRM_ICON_BG"):GetControl("SkillIcon");	
	if kSkillIcon:IsNil() == false then
	    kSkillIcon:SetGrayScale(  bLearned == false )
	end
	
	kSkillIcon:SetInvalidate(true)
	
	
	--  테이블 아이콘 
	local	kIconTable = kIconFrmControl:GetControl("ICON_TABLE");
	if kIconTable:IsNil() then
		return
	end
	local	iMaxIconCount = 5*5;	
	local   i=0
	while i<iMaxIconCount do
	    local	kIcon = kIconTable:GetControl("SKILLICON"..i);
	    
	    if kIcon:IsNil() == false then
	    
	        if kIcon:IconKey() == iKeySkillNo then
	            kIcon:SetGrayScale( bLearned == false )
	            break;
	        end
	    
	    end
	
	    i=i+1
	end
	
end



function	UI_SKILLWND_ON_RESET_TEMPORARY()
    ODS("UI_SKILLWND_ON_RESET_TEMPORARY\n");
	
	local	kSkillTree = GetSkillTree();
	kSkillTree:ResetTemporary();
	UI_SKILLWND_Refresh_All();
	
end

function	UI_SKILLWND_ON_CONFIRM_TEMPORARY()
    ODS("UI_SKILLWND_ON_CONFIRM_TEMPORARY\n");
	
	local	kSkillTree = GetSkillTree();
	kSkillTree:ConfirmTemporary();
	UI_SKILLWND_SetButtonVisible();	
end
function UI_SKILLWND_SetButtonVisible(wnd, iSkillNo)
    ODS("UI_SKILLWND_SetButtonVisible\n");
	if true==wnd:IsNil() then return end

	local	kSkillTree = GetSkillTree();
	local bRet = false
	if nil==iSkillNo then
		iSkillNo = 0
	end
	ODS("UI_SKILLWND_SetButtonVisible " .. iSkillNo.. "\n")
	bRet = kSkillTree:IsTemporaryRemainSkillPoint(iSkillNo)
	if bRet==false then
	end

	UI_SKILLWND_ShowResetButton(wnd, bRet);
	UI_SKILLWND_ShowConfirmButton(wnd, bRet);
end

function	UI_SKILLWND_ON_LEVELUP_CLICK(LevelUpBtn)	--	스킬 레벨 업 버튼 클릭
    ODS("UI_SKILLWND_ON_LEVELUP_CLICK\n");

	if LevelUpBtn:IsNil() == true then
		MessageBox("Nil","");
		return;
	end

	local	IconBGWnd = LevelUpBtn:GetParent();
	local	iKeySkillNo = IconBGWnd:GetCustomDataAsInt();
		
	local	kSkillTree = GetSkillTree();
	kSkillTree:LevelUpTemporary(iKeySkillNo);
	UI_SKILLWND_Refresh_All();
	


end
function	UI_SKILLWND_ON_LEVELDOWN_CLICK(LevelUpBtn)	--	스킬 레벨 다운 버튼 클릭
    ODS("UI_SKILLWND_ON_LEVELDOWN_CLICK\n");

	if LevelUpBtn:IsNil() == true then
		MessageBox("Nil","");
		return;
	end

	IconBGWnd = LevelUpBtn:GetParent();
	iKeySkillNo = IconBGWnd:GetCustomDataAsInt();
		
	kSkillTree = GetSkillTree();
	kSkillTree:LevelDownTemporary(iKeySkillNo);
	UI_SKILLWND_Refresh_All();
	
	

end
function	UI_SKILLWND_ResetAll(wnd)
    ODS("UI_SKILLWND_ResetAll\n");

	local	i = 0;
	
	while i<4 do
	
		--	리스트 클리어
		
		local	kListControl = wnd:GetControl("SFRM_SKILL_BG_0"..(i+1)):GetControl("SFRM_SDW"):GetControl("LST_LIST");
		kListControl:ClearAllListItem();
		
		--	아이콘 클리어
		local	kIconFrm = wnd:GetControl("SFRM_SKILL_BG_0"..(i+1)):GetControl("SFRM_SDW"):GetControl("FRM_ICON");
		kIconFrm:SetCustomDataAsInt(0);
			
		local	kIconTable = kIconFrm:GetControl("ICON_TABLE");
		UI_SKILLWND_RenameIcon(kIconTable);
		
		i=i+1;
	end
	
	--	초기화 버튼 숨기기
	UI_SKILLWND_ShowResetButton(wnd, false);
	--	확인 버튼 숨기기
	UI_SKILLWND_ShowConfirmButton(wnd, false);
	
end
function UI_SKILLWND_ShowResetButton(wnd, bShow)
    ODS("UI_SKILLWND_ShowResetButton\n");

	if nil==wnd or true==wnd:IsNil() then return end
	
	local	kResetBtn = wnd:GetControl("BTN_CANCEL");
	if kResetBtn:IsNil() then return end
	
	kResetBtn:Visible(bShow);
end
function UI_SKILLWND_ShowConfirmButton(wnd, bShow)
    ODS("UI_SKILLWND_ShowConfirmButton\n")

	if nil==wnd or true==wnd:IsNil() then return end
	
	local	kConfirmBtn = wnd:GetControl("BTN_CONFIRM")
	if kConfirmBtn:IsNil() then	return end
	kConfirmBtn:Visible(bShow);
end

function	UI_SKILLWND_SKILL_CLASS_BUTTON_CLICK(iClassButtonNum)

	ODS("UI_SKILLWND_SKILL_CLASS_BUTTON_CLICK iClassButtonNum:"..iClassButtonNum.."\n");
	if g_UI_SKILLWND_BaseWnd == nil then
		return
	end
	
	g_UI_SKILLWND_CurrentSkillClassID = iClassButtonNum-1;
	
	local i =0;
	while i<4 do
	
		local	kButtonControl = g_UI_SKILLWND_BaseWnd:GetControl("SFRM_TITLE"):GetControl("SFRM_COLOR"..(i+1)):GetControl("CBTN_TITLE");
		local	kForm = 	g_UI_SKILLWND_BaseWnd:GetControl("SFRM_SKILL_BG_0"..(i+1));
	
		if i == iClassButtonNum-1 then

			kButtonControl:CheckState(true);
			kButtonControl:LockClick(true);
			kForm:Visible(true);
		
		else
		
			kButtonControl:CheckState(false);
			kButtonControl:LockClick(false);
			kForm:Visible(false);
		
		end
		i=i+1;
	
	end

end

function	UI_SKILLWND_Set_Class_Buttons(wnd)
    ODS("UI_SKILLWND_Set_Class_Buttons\n");

	-- 플레이어의 클래스를 얻어온다.
	
	local	MyPilot = GetMyPilot()
	local	iClass	=	MyPilot:GetAbil(AT_CLASS);
	local	kActor = MyPilot:GetActor();
	
	local	iBaseClassID = MyPilot:GetBaseClassID();
	
	-- 클래스에 맞는 계열 이름을 각 버튼에 설정하자.
	
	local	i = 0;
	while i<4 do
	
		local	kButtonControl = g_UI_SKILLWND_BaseWnd:GetControl("SFRM_TITLE"):GetControl("SFRM_COLOR"..(i+1)):GetControl("CBTN_TITLE");
		kButtonControl:SetStaticTextW(GetTextW(201+(iBaseClassID-1)*4+i));
	
		i=i+1;
	end
	
end

--[[CT_MAGICIAN = 2	-- 매지션		0000 0000 0000 0004
CT_ARCHER = 3		-- 궁수			0000 0000 0000 0008
CT_THIEF = 4		-- 도둑			0000 0000 0000 0010
CT_KNIGHT = 5		-- 기사			0000 0000 0000 0020
CT_WARRIOR = 6		-- 투사			0000 0000 0000 0040
CT_MAGE = 7		-- 메이지		0000 0000 0000 0080
CT_BATTLEMAGE = 8	-- 전투마법사	0000 0000 0000 0100
CT_HUNTER = 9		-- 사냥꾼		0000 0000 0000 0200
CT_RANGER = 10		-- 레인저		0000 0000 0000 0400
CT_CLOWN = 11		-- 광대			0000 0000 0000 0800
CT_ASSASSIN =12	-- 암살자		0000 0000 0000 1000
CT_PALADIN = 13	-- 성기사
CT_GLADIATOR = 14	-- 검투사
CT_WIZARD = 15		-- 위자드
CT_WARMAGE = 16	-- 워메이지
CT_TRAPPER = 17	-- 트랩퍼
CT_SNIPER = 18		-- 저격수
CT_DANCER = 19		-- 댄서
CT_NINJA = 20		-- 닌자
CT_DRAOON = 21		-- 용기사
CT_DESTROYER = 22	-- 파괴자
CT_ARCHMAGE = 23	-- 아크메이지
CT_WARLORD = 24	-- 워로드
CT_SENTINEL = 25	-- 수호자
CT_LAUNCHER = 26	-- 런처
CT_FREEJACKER = 27	-- 프리재커
CT_SHADOW = 28		-- 새도우
--Hidden
CT_DUELIST = 29	-- 무도가
CT_DOCTOR = 30		-- 닥터
CT_GUNSLINGER = 31	-- 건슬링거
CT_BARD = 32		-- 음유시인
CT_BATTLEMASTER = 33	-- 패와
CT_ANATOMIST = 34	-- 분석가
CT_DESPERADO = 35	-- 무법자
CT_VOCAL = 36		-- 보컬
]]--

SKILL_BG_NAME_LIST = {}
SKILL_BG_NAME_LIST[CT_FIGHTER] = 	"FRM_FIG_SKILL_BG"
SKILL_BG_NAME_LIST[CT_MAGICIAN] = 	"FRM_MAG_SKILL_BG"
SKILL_BG_NAME_LIST[CT_ARCHER] = 	"FRM_ARC_SKILL_BG"
SKILL_BG_NAME_LIST[CT_THIEF] = 		"FRM_THI_SKILL_BG"
SKILL_BG_NAME_LIST[CT_KNIGHT] = 	"FRM_KNT_SKILL_BG"
SKILL_BG_NAME_LIST[CT_WARRIOR] = 	"FRM_WAR_SKILL_BG"
SKILL_BG_NAME_LIST[CT_MAGE] = 		"FRM_MAE_SKILL_BG"
SKILL_BG_NAME_LIST[CT_BATTLEMAGE] =	"FRM_BGE_SKILL_BG"
SKILL_BG_NAME_LIST[CT_HUNTER] = 	"FRM_HUN_SKILL_BG"
SKILL_BG_NAME_LIST[CT_RANGER] = 	"FRM_RAN_SKILL_BG"
SKILL_BG_NAME_LIST[CT_CLOWN] = 		"FRM_CLO_SKILL_BG"
SKILL_BG_NAME_LIST[CT_ASSASSIN] = 	"FRM_ASS_SKILL_BG"
SKILL_BG_NAME_LIST[CT_PALADIN] = "SFRM_PAL_SKILL_BG"
SKILL_BG_NAME_LIST[CT_GLADIATOR] = "SFRM_GLA_SKILL_BG"
SKILL_BG_NAME_LIST[CT_WIZARD] = "SFRM_WIZ_SKILL_BG"
SKILL_BG_NAME_LIST[CT_WARMAGE] = "SFRM_WMA_SKILL_BG"
SKILL_BG_NAME_LIST[CT_TRAPPER] = "SFRM_TRP_SKILL_BG"
SKILL_BG_NAME_LIST[CT_SNIPER] = "SFRM_SNP_SKILL_BG"
SKILL_BG_NAME_LIST[CT_DANCER] = "SFRM_DAN_SKILL_BG"
SKILL_BG_NAME_LIST[CT_NINJA] = "SFRM_NIN_SKILL_BG"

SKILL_BG_NAME_LIST[CT_DRAOON] = "SFRM_DRA_SKILL_BG"
SKILL_BG_NAME_LIST[CT_DESTROYER] = "SFRM_DES_SKILL_BG"
SKILL_BG_NAME_LIST[CT_ARCHMAGE] = "SFRM_AMA_SKILL_BG"
SKILL_BG_NAME_LIST[CT_WARLORD] = "SFRM_WLD_SKILL_BG"
SKILL_BG_NAME_LIST[CT_SENTINEL] = "SFRM_SEN_SKILL_BG"
SKILL_BG_NAME_LIST[CT_LAUNCHER] = "SFRM_LAN_SKILL_BG"
SKILL_BG_NAME_LIST[CT_FREEJACKER] = "SFRM_FJK_SKILL_BG"
SKILL_BG_NAME_LIST[CT_SHADOW] = "SFRM_SDW_SKILL_BG"

SKILL_BG_NAME_LIST[CT_SPECIAL_SKILL] ="FRM_SPC_SKILL_BG"

SKILL_BG_NAME_LIST[CT_SHAMAN] ="SFRM_SUM_SKILL_BG"
SKILL_BG_NAME_LIST[CT_SUMMONER] ="SFRM_SUMWIZ_SKILL_BG"
SKILL_BG_NAME_LIST[CT_NATURE_MASTER] ="SFRM_NAMAS_SKILL_BG"
SKILL_BG_NAME_LIST[CT_GAIA_MASTER] ="SFRM_GAIAMAS_SKILL_BG"

SKILL_BG_NAME_LIST[CT_DOUBLE_FIGHTER] ="SFRM_DBLFIG_SKILL_BG"
SKILL_BG_NAME_LIST[CT_TWINS] ="SFRM_TWINS_SKILL_BG"
SKILL_BG_NAME_LIST[CT_MIRAGE] ="SFRM_MRG_SKILL_BG"
SKILL_BG_NAME_LIST[CT_DRAGON_FIGHTER] ="SFRM_DRAFIG_SKILL_BG"
 
--[[
SKILL_BG_NAME_LIST[CT_DUELIST] = "SFRM_DUE_SKILL_BG"
SKILL_BG_NAME_LIST[CT_DOCTOR] = "SFRM_DOC_SKILL_BG"
SKILL_BG_NAME_LIST[CT_GUNSLINGER] = "SFRM_GSG_SKILL_BG"
SKILL_BG_NAME_LIST[CT_BARD] = "SFRM_BAD_SKILL_BG"
SKILL_BG_NAME_LIST[CT_BATTLEMASTER] = "SFRM_BMA_SKILL_BG"
SKILL_BG_NAME_LIST[CT_ANATOMIST] = "SFRM_ANA_SKILL_BG"
SKILL_BG_NAME_LIST[CT_DESPERADO] = "SFRM_DEP_SKILL_BG"
SKILL_BG_NAME_LIST[CT_VOCAL] = "SFRM_VOC_SKILL_BG"]]--

--자주 반복되는 Y값 위치
SKILL_ICON_YPOS_1 = 7  +29
SKILL_ICON_YPOS_2 = 83 +29
SKILL_ICON_YPOS_3 = 159+29
SKILL_ICON_YPOS_4 = 235+29
SKILL_ICON_YPOS_5 = 311+29
SKILL_ICON_YPOS_6 = 387+29

-- 4차 스킬 아이콘 위치
SKILL4_ICON_XPOS_1 = 7	+32
SKILL4_ICON_XPOS_2 = 87+32
SKILL4_ICON_XPOS_3 = 135+32
SKILL4_ICON_XPOS_4 = 183+32
SKILL4_ICON_XPOS_5 = 263+32

SKILL4_ICON_YPOS_1 = SKILL_ICON_YPOS_1
SKILL4_ICON_YPOS_2 = SKILL_ICON_YPOS_3
SKILL4_ICON_YPOS_3 = SKILL_ICON_YPOS_4
SKILL4_ICON_YPOS_4 = SKILL_ICON_YPOS_6


SKILL_STRUCT_CLASS = {}
SKILL_STRUCT = {}

SKILL_STRUCT[0] = { ["ResNo"] = 101300101, ["SkillNo"] = 101300101, ["Pos"] = Point2(135+35,45+29-15) }		--소드 마스터리
SKILL_STRUCT[1] = { ["ResNo"] = 101100901, ["SkillNo"] = 101100901, ["Pos"] = Point2(56+35,45+29-15) }		--찌르기
SKILL_STRUCT[2] = { ["ResNo"] = 101100701, ["SkillNo"] = 101100701, ["Pos"] = Point2(56+35,121+29-15) }		--내려찍기
SKILL_STRUCT[3] = { ["ResNo"] = 101000801, ["SkillNo"] = 101000801, ["Pos"] = Point2(56+35,197+29-15) }		--스톰 블레이드
SKILL_STRUCT[4] = { ["ResNo"] = 101001101, ["SkillNo"] = 101001101, ["Pos"] = Point2(56+35,349+29-15) }		--불타는 청춘
SKILL_STRUCT[5] = { ["ResNo"] = 101100601, ["SkillNo"] = 101100601, ["Pos"] = Point2(214+35,45+29-15) }		--띄우기
SKILL_STRUCT[6] = { ["ResNo"] = 101101801, ["SkillNo"] = 101101801, ["Pos"] = Point2(214+35,121+29-15) }		--다운조작
SKILL_STRUCT[7] = { ["ResNo"] = 101001001, ["SkillNo"] = 101001001, ["Pos"] = Point2(214+35,197+29-15) }		--소드댄스
SKILL_STRUCT[8] = { ["ResNo"] = 101001201, ["SkillNo"] = 101001201, ["Pos"] = Point2(214+35,349+29-15) }		--비정한 인생
SKILL_STRUCT[9] = { ["ResNo"] = 101000101, ["SkillNo"] = 101000101, ["Pos"] = Point2(135+35,283+29-15) }		--인생은 한방
SKILL_STRUCT[10] = { ["ResNo"] = 180000101, ["SkillNo"] = 180000101, ["Pos"] = Point2(135+35,SKILL_ICON_YPOS_6-15) }		--친교 버프
--SKILL_STRUCT[10] = { ["ResNo"] = 200001001, ["SkillNo"] = 200001001, ["Pos"] = Point2(180,70) }
--SKILL_STRUCT[10] = { ["ResNo"] = 200001101, ["SkillNo"] = 200001101, ["Pos"] = Point2(180,130) }
--SKILL_STRUCT[11] = { ["ResNo"] = 200001201, ["SkillNo"] = 200001201, ["Pos"] = Point2(180,190) }
--SKILL_STRUCT[13] = { ["ResNo"] = 200001301, ["SkillNo"] = 200001301, ["Pos"] = Point2(180,250) }
--SKILL_STRUCT[14] = { ["ResNo"] = 200001401, ["SkillNo"] = 200001401, ["Pos"] = Point2(180,310) }
--SKILL_STRUCT[15] = { ["ResNo"] = 200001501, ["SkillNo"] = 200001501, ["Pos"] = Point2(180,370) }
--SKILL_STRUCT[16] = { ["ResNo"] = 200001601, ["SkillNo"] = 200001601, ["Pos"] = Point2(225,70) }
--SKILL_STRUCT[17] = { ["ResNo"] = 200001701, ["SkillNo"] = 200001701, ["Pos"] = Point2(225,130) }
--SKILL_STRUCT[12] = { ["ResNo"] = 200001801, ["SkillNo"] = 200001801, ["Pos"] = Point2(225,190) }
--SKILL_STRUCT[19] = { ["ResNo"] = 200001901, ["SkillNo"] = 200001901, ["Pos"] = Point2(225,250) }
--SKILL_STRUCT[20] = { ["ResNo"] = 200002001, ["SkillNo"] = 200002001, ["Pos"] = Point2(225,310) }
--SKILL_STRUCT[21] = { ["ResNo"] = 200002101, ["SkillNo"] = 200002101, ["Pos"] = Point2(225,370) }
--SKILL_STRUCT[12] = { ["ResNo"] = 200002201, ["SkillNo"] = 200002201, ["Pos"] = Point2(270,70) }
--SKILL_STRUCT[23] = { ["ResNo"] = 200002301, ["SkillNo"] = 200002301, ["Pos"] = Point2(270,130) }
--SKILL_STRUCT[24] = { ["ResNo"] = 200002401, ["SkillNo"] = 200002401, ["Pos"] = Point2(270,190) }

SKILL_STRUCT_CLASS[CT_FIGHTER] = SKILL_STRUCT

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 103101201, ["SkillNo"] = 103101201, ["Pos"] = Point2(40+30,SKILL_ICON_YPOS_2) }		--다운조작
SKILL_STRUCT[1] = { ["ResNo"] = 102000101, ["SkillNo"] = 102000101, ["Pos"] = Point2(40+30,SKILL_ICON_YPOS_4) }		--포트리스
SKILL_STRUCT[2] = { ["ResNo"] = 102002201, ["SkillNo"] = 102002201, ["Pos"] = Point2(7+30,SKILL_ICON_YPOS_5) }			--포신 강선(포트리스 강화)
SKILL_STRUCT[3] = { ["ResNo"] = 102002301, ["SkillNo"] = 102002301, ["Pos"] = Point2(71+30,SKILL_ICON_YPOS_6) }		--탄환 교체(포트리스 강화)
SKILL_STRUCT[4] = { ["ResNo"] = 103100501, ["SkillNo"] = 103100501, ["Pos"] = Point2(135+30,SKILL_ICON_YPOS_1) }		--포인트 버스터
SKILL_STRUCT[5] = { ["ResNo"] = 103101401, ["SkillNo"] = 103101401, ["Pos"] = Point2(135+30,SKILL_ICON_YPOS_2) }		--공중콤보
SKILL_STRUCT[6] = { ["ResNo"] = 103100201, ["SkillNo"] = 103100201, ["Pos"] = Point2(135+30,SKILL_ICON_YPOS_3) }		--내려찍기(운석낙하)
SKILL_STRUCT[7] = { ["ResNo"] = 109001101, ["SkillNo"] = 109001101, ["Pos"] = Point2(230+30,SKILL_ICON_YPOS_1) }		--MP 포션 극대화
SKILL_STRUCT[8] = { ["ResNo"] = 102002401, ["SkillNo"] = 102002401, ["Pos"] = Point2(199+30,SKILL_ICON_YPOS_3) }		--슬로우 힐
SKILL_STRUCT[9] = { ["ResNo"] = 102000501, ["SkillNo"] = 102000501, ["Pos"] = Point2(199+30,SKILL_ICON_YPOS_5) }		--힐은 셀프
SKILL_STRUCT[10] = { ["ResNo"] = 102002101, ["SkillNo"] = 102002101, ["Pos"] = Point2(263+30,SKILL_ICON_YPOS_2) }		--마법 강화
SKILL_STRUCT[11] = { ["ResNo"] = 102000801, ["SkillNo"] = 102000801, ["Pos"] = Point2(263+30,SKILL_ICON_YPOS_3) }		--정신일도 하사불성
SKILL_STRUCT[12] = { ["ResNo"] = 103102001, ["SkillNo"] = 103102001, ["Pos"] = Point2(40+30,SKILL_ICON_YPOS_3) }		--블링크(뒤로 이동)
SKILL_STRUCT[13] = { ["ResNo"] = 180000201, ["SkillNo"] = 180000201, ["Pos"] = Point2(135+30,SKILL_ICON_YPOS_6) }		--친교 버프
SKILL_STRUCT_CLASS[CT_MAGICIAN] = SKILL_STRUCT

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 102101901, ["SkillNo"] = 102101901, ["Pos"] = Point2(7+35,SKILL_ICON_YPOS_2) }			--백스텝
SKILL_STRUCT[1] = { ["ResNo"] = 102101601, ["SkillNo"] = 102101601, ["Pos"] = Point2(7+35,SKILL_ICON_YPOS_3) }			--백스텝 샷
SKILL_STRUCT[2] = { ["ResNo"] = 102101801, ["SkillNo"] = 102101801, ["Pos"] = Point2(101+35,SKILL_ICON_YPOS_2) }		--라이징 애로우
SKILL_STRUCT[3] = { ["ResNo"] = 102101301, ["SkillNo"] = 102101301, ["Pos"] = Point2(71+35,SKILL_ICON_YPOS_3) }		--위로 쏘기
SKILL_STRUCT[4] = { ["ResNo"] = 110000301, ["SkillNo"] = 110000301, ["Pos"] = Point2(71+35,SKILL_ICON_YPOS_4) }		--더블 스트라이크
SKILL_STRUCT[5] = { ["ResNo"] = 102100301, ["SkillNo"] = 102100301, ["Pos"] = Point2(135+35,SKILL_ICON_YPOS_3) }		--점프 하단 샷
SKILL_STRUCT[6] = { ["ResNo"] = 110001901, ["SkillNo"] = 110001901, ["Pos"] = Point2(135+35,SKILL_ICON_YPOS_4) }		--멀티샷
SKILL_STRUCT[7] = { ["ResNo"] = 102102401, ["SkillNo"] = 102102401, ["Pos"] = Point2(199+35,SKILL_ICON_YPOS_4) }		--다운조작
SKILL_STRUCT[8] = { ["ResNo"] = 103300101, ["SkillNo"] = 103300101, ["Pos"] = Point2(263+35,SKILL_ICON_YPOS_2) }		--보우 마스터리
SKILL_STRUCT[9] = { ["ResNo"] = 103000501, ["SkillNo"] = 103000501, ["Pos"] = Point2(263+35,SKILL_ICON_YPOS_5) }		--래피드 스텝
SKILL_STRUCT[10] = { ["ResNo"] = 180000301, ["SkillNo"] = 180000301, ["Pos"] = Point2(135+35,SKILL_ICON_YPOS_6) }		--친교 버프
SKILL_STRUCT_CLASS[CT_ARCHER] = SKILL_STRUCT

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 103202401, ["SkillNo"] = 103202401, ["Pos"] = Point2(59+35,SKILL_ICON_YPOS_1) }		--띄우기
SKILL_STRUCT[1] = { ["ResNo"] = 103201901, ["SkillNo"] = 103201901, ["Pos"] = Point2(20+35,SKILL_ICON_YPOS_2) }		--공중 난타
SKILL_STRUCT[2] = { ["ResNo"] = 104301601, ["SkillNo"] = 104301601, ["Pos"] = Point2(20+35,SKILL_ICON_YPOS_3) }		--섬머솔트킥
SKILL_STRUCT[3] = { ["ResNo"] = 103202601, ["SkillNo"] = 103202601, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_2) }		--내려찍기
SKILL_STRUCT[4] = { ["ResNo"] = 104000401, ["SkillNo"] = 104000401, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_3) }		--로켓 펀치
SKILL_STRUCT[5] = { ["ResNo"] = 104301901, ["SkillNo"] = 104301901, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_4) }		--노즐 개조(로켓 펀치 강화)
SKILL_STRUCT[6] = { ["ResNo"] = 103201001, ["SkillNo"] = 103201001, ["Pos"] = Point2(172+35,SKILL_ICON_YPOS_3) }		--다운조작
SKILL_STRUCT[7] = { ["ResNo"] = 104000701, ["SkillNo"] = 104000701, ["Pos"] = Point2(211+35,SKILL_ICON_YPOS_1) }		--더블 어택
SKILL_STRUCT[8] = { ["ResNo"] = 104000301, ["SkillNo"] = 104000301, ["Pos"] = Point2(250+35,SKILL_ICON_YPOS_3) }		--독 살포
SKILL_STRUCT[9] = { ["ResNo"] = 104302001, ["SkillNo"] = 104302001, ["Pos"] = Point2(172+35,SKILL_ICON_YPOS_5) }		--바이탈리티
SKILL_STRUCT[10] = { ["ResNo"] = 104302101, ["SkillNo"] = 104302101, ["Pos"] = Point2(172+35,SKILL_ICON_YPOS_6) }		--덱스터리티
SKILL_STRUCT[11] = { ["ResNo"] = 180000401, ["SkillNo"] = 180000401, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_6) }		--친교 버프
SKILL_STRUCT_CLASS[CT_THIEF] = SKILL_STRUCT
--SKILL_STRUCT[1] = { ["ResNo"] = 104000801, ["SkillNo"] = 104000801, ["Pos"] = Point2(57,SKILL_ICON_YPOS_1) }		--소매치기

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 101102601, ["SkillNo"] = 101102601, ["Pos"]  = Point2(56+35 ,45 +29) }		--기사 공중대쉬 강타
SKILL_STRUCT[1] = { ["ResNo"] = 101000901, ["SkillNo"] = 101000901, ["Pos"]  = Point2(56+35 ,121+29) }		--숄더 테클
SKILL_STRUCT[2] = { ["ResNo"] = 105300401, ["SkillNo"] = 105300401, ["Pos"]  = Point2(56+35 ,197+29) }		--스피어잽
SKILL_STRUCT[3] = { ["ResNo"] = 105500701, ["SkillNo"] = 105500701, ["Pos"]  = Point2(56+35 ,273+29) }		--프레셔
SKILL_STRUCT[4] = { ["ResNo"] = 105300701, ["SkillNo"] = 105300701, ["Pos"]  = Point2(135+35,45 +29) }		--쉴드 마스터리
SKILL_STRUCT[5] = { ["ResNo"] = 105300201, ["SkillNo"] = 105300201, ["Pos"]  = Point2(135+35,121+29) }		--패링
SKILL_STRUCT[6] = { ["ResNo"] = 101000301, ["SkillNo"] = 101000301, ["Pos"]  = Point2(135+35,197+29) }		--단단한 머리
SKILL_STRUCT[7] = { ["ResNo"] = 106300601, ["SkillNo"] = 106300601, ["Pos"]  = Point2(135+35,273+29) }		--생명연장
SKILL_STRUCT[8] = { ["ResNo"] = 105400001, ["SkillNo"] = 105400001, ["Pos"]  = Point2(135+35,349+29) }		--바디 액티베이션
SKILL_STRUCT[9] = { ["ResNo"] = 101102501, ["SkillNo"] = 101102501, ["Pos"]  = Point2(214+35,45 +29) }		--공중조작 3타
SKILL_STRUCT[10] = { ["ResNo"] = 101000401, ["SkillNo"] = 101000401, ["Pos"] = Point2(214+35,121+29) }		--와신상담
SKILL_STRUCT[11] = { ["ResNo"] = 105300601, ["SkillNo"] = 105300601, ["Pos"] = Point2(214+35,273+29) }		--리벤지
SKILL_STRUCT[12] = { ["ResNo"] = 101103201, ["SkillNo"] = 101103201, ["Pos"] = Point2(56+35 ,349+29) }		--기사용 형돈다
SKILL_STRUCT_CLASS[CT_KNIGHT] = SKILL_STRUCT

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 101000601, ["SkillNo"] = 101000601, ["Pos"] = Point2(59+35,SKILL_ICON_YPOS_1) }		--곰같은 힘이여
SKILL_STRUCT[1] = { ["ResNo"] = 105300801, ["SkillNo"] = 105300801, ["Pos"] = Point2(20+35,SKILL_ICON_YPOS_2) }		--Adv 소드 마스터리
SKILL_STRUCT[2] = { ["ResNo"] = 101102701, ["SkillNo"] = 101102701, ["Pos"] = Point2(20+35,SKILL_ICON_YPOS_3) }		--대쉬 강타
SKILL_STRUCT[3] = { ["ResNo"] = 101000501, ["SkillNo"] = 101000501, ["Pos"] = Point2(20+35,SKILL_ICON_YPOS_4) }		--형 간다
SKILL_STRUCT[4] = { ["ResNo"] = 101102801, ["SkillNo"] = 101102801, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_3) }		--공중조작 3타
SKILL_STRUCT[5] = { ["ResNo"] = 101000201, ["SkillNo"] = 101000201, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_4) }		--형왔다
SKILL_STRUCT[6] = { ["ResNo"] = 106000101, ["SkillNo"] = 106000101, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_5) }		--룰렛
SKILL_STRUCT[7] = { ["ResNo"] = 106300401, ["SkillNo"] = 106300401, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_6) }		--분노의 외침
SKILL_STRUCT[8] = { ["ResNo"] = 106300301, ["SkillNo"] = 106300301, ["Pos"] = Point2(172+35,SKILL_ICON_YPOS_2) }		--섬멸
SKILL_STRUCT[9] = { ["ResNo"] = 106500401, ["SkillNo"] = 106500401, ["Pos"] = Point2(172+35,SKILL_ICON_YPOS_6) }		--대지의 분노
SKILL_STRUCT[10] = { ["ResNo"] = 106300201, ["SkillNo"] = 106300201, ["Pos"] = Point2(250+35,SKILL_ICON_YPOS_1) }		--액셀레이션
SKILL_STRUCT[11] = { ["ResNo"] = 105300901, ["SkillNo"] = 105300901, ["Pos"] = Point2(250+35,SKILL_ICON_YPOS_2) }		--HP 회복력 향상
SKILL_STRUCT_CLASS[CT_WARRIOR] = SKILL_STRUCT

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 109002901, ["SkillNo"] = 109002901, ["Pos"] = Point2(7+60,SKILL_ICON_YPOS_4) }			--캐스팅 고속화
SKILL_STRUCT[1] = { ["ResNo"] = 102001001, ["SkillNo"] = 102001001, ["Pos"] = Point2(71+60,SKILL_ICON_YPOS_1) }		--확산포
SKILL_STRUCT[2] = { ["ResNo"] = 102001101, ["SkillNo"] = 102001101, ["Pos"] = Point2(71+60,SKILL_ICON_YPOS_3) }		--라이트닝 스트라이크
SKILL_STRUCT[3] = { ["ResNo"] = 102000301, ["SkillNo"] = 102000301, ["Pos"] = Point2(135+60,SKILL_ICON_YPOS_2) }		--디텍션
SKILL_STRUCT[4] = { ["ResNo"] = 102000401, ["SkillNo"] = 102000401, ["Pos"] = Point2(135+60,SKILL_ICON_YPOS_3) }		--바바리맨
SKILL_STRUCT[5] = { ["ResNo"] = 102000201, ["SkillNo"] = 102000201, ["Pos"] = Point2(135+60,SKILL_ICON_YPOS_4) }		--마녀의 저주
SKILL_STRUCT[6] = { ["ResNo"] = 109000301, ["SkillNo"] = 109000301, ["Pos"] = Point2(199+60,SKILL_ICON_YPOS_1) }		--MP회복력 향상
SKILL_STRUCT[7] = { ["ResNo"] = 107000301, ["SkillNo"] = 107000301, ["Pos"] = Point2(199+60,SKILL_ICON_YPOS_2) }		--와이드힐
SKILL_STRUCT[8] = { ["ResNo"] = 109002801, ["SkillNo"] = 109002801, ["Pos"] = Point2(199+60,SKILL_ICON_YPOS_3) }		--생츄어리
SKILL_STRUCT[9] = { ["ResNo"] = 107000401, ["SkillNo"] = 107000401, ["Pos"] = Point2(235+60,SKILL_ICON_YPOS_4) }		--인스탄스 힐
SKILL_STRUCT[10] = { ["ResNo"] = 109001001, ["SkillNo"] = 109001001, ["Pos"] = Point2(199+60,SKILL_ICON_YPOS_5) }		--부활
SKILL_STRUCT[11] = { ["ResNo"] = 103100301, ["SkillNo"] = 103100301, ["Pos"] = Point2(135+60,SKILL_ICON_YPOS_1) }		--트리플 메테오(패시브)
SKILL_STRUCT[12] = { ["ResNo"] = 103101301, ["SkillNo"] = 103101301, ["Pos"] = Point2(7+60,SKILL_ICON_YPOS_1) }		--멀티 라이트닝(패시브)
SKILL_STRUCT[13] = { ["ResNo"] = 102003201, ["SkillNo"] = 102003201, ["Pos"] = Point2(71+60,SKILL_ICON_YPOS_4) }		--레인 오브 파이어
SKILL_STRUCT_CLASS[CT_MAGE] = SKILL_STRUCT

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 103101901, ["SkillNo"] = 103101901, ["Pos"] = Point2(7+32,SKILL_ICON_YPOS_1) }			--배틀 히트
SKILL_STRUCT[1] = { ["ResNo"] = 102300101, ["SkillNo"] = 102300101, ["Pos"] = Point2(7+32,SKILL_ICON_YPOS_2) }			--스피어 마스터리
SKILL_STRUCT[2] = { ["ResNo"] = 102003101, ["SkillNo"] = 102003101, ["Pos"] = Point2(30+32,SKILL_ICON_YPOS_4) }		--에너지 응집(에너지 볼 강화)
SKILL_STRUCT[3] = { ["ResNo"] = 109000901, ["SkillNo"] = 109000901, ["Pos"] = Point2(71+32,SKILL_ICON_YPOS_3) }		--에너지 볼
SKILL_STRUCT[4] = { ["ResNo"] = 109000501, ["SkillNo"] = 109000501, ["Pos"] = Point2(71+32,SKILL_ICON_YPOS_5) }		--매직미사일
SKILL_STRUCT[5] = { ["ResNo"] = 109000701, ["SkillNo"] = 109000701, ["Pos"] = Point2(135+32,SKILL_ICON_YPOS_4) }		--토네이도
SKILL_STRUCT[6] = { ["ResNo"] = 108000101, ["SkillNo"] = 108000101, ["Pos"] = Point2(199+32,SKILL_ICON_YPOS_5) }		--블리자드
SKILL_STRUCT[7] = { ["ResNo"] = 102000701, ["SkillNo"] = 102000701, ["Pos"] = Point2(263+32,SKILL_ICON_YPOS_1) }		--마나실드
SKILL_STRUCT[8] = { ["ResNo"] = 109002501, ["SkillNo"] = 109002501, ["Pos"] = Point2(263+32,SKILL_ICON_YPOS_4) }		--클라우드 킬
SKILL_STRUCT[9] = { ["ResNo"] = 102003301, ["SkillNo"] = 102003301, ["Pos"] = Point2(263+32,SKILL_ICON_YPOS_5) }		--맹독(클라우드킬 강화)
SKILL_STRUCT_CLASS[CT_BATTLEMAGE] = SKILL_STRUCT

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 110000501, ["SkillNo"] = 110000501, ["Pos"] = Point2(20+30,SKILL_ICON_YPOS_3) }		--Adv. 보우 마스터리
SKILL_STRUCT[1] = { ["ResNo"] = 110000401, ["SkillNo"] = 110000401, ["Pos"] = Point2(97+30,SKILL_ICON_YPOS_1) }		--모서리로 찍으세요
SKILL_STRUCT[2] = { ["ResNo"] = 110001001, ["SkillNo"] = 110001001, ["Pos"] = Point2(97+30,SKILL_ICON_YPOS_2) }		--애시드 애로우
SKILL_STRUCT[3] = { ["ResNo"] = 110001301, ["SkillNo"] = 110001301, ["Pos"] = Point2(97+30,SKILL_ICON_YPOS_3) }		--샤프슈팅
SKILL_STRUCT[4] = { ["ResNo"] = 110001801, ["SkillNo"] = 110001801, ["Pos"] = Point2(58+30,SKILL_ICON_YPOS_4) }		--넷 바인더
SKILL_STRUCT[5] = { ["ResNo"] = 110003201, ["SkillNo"] = 110003201, ["Pos"] = Point2(20+30,SKILL_ICON_YPOS_5) }		--디스트럽션 웹(넷바인드 강화)
SKILL_STRUCT[6] = { ["ResNo"] = 110000201, ["SkillNo"] = 110000201, ["Pos"] = Point2(172+30,SKILL_ICON_YPOS_2) }		--애로우 샤워
SKILL_STRUCT[7] = { ["ResNo"] = 110000101, ["SkillNo"] = 110000101, ["Pos"] = Point2(172+30,SKILL_ICON_YPOS_3) }		--로켓 런처
SKILL_STRUCT[8] = { ["ResNo"] = 110001201, ["SkillNo"] = 110001201, ["Pos"] = Point2(172+30,SKILL_ICON_YPOS_4) }		--울프 러쉬
SKILL_STRUCT[9] = { ["ResNo"] = 103000401, ["SkillNo"] = 103000401, ["Pos"] = Point2(250+30,SKILL_ICON_YPOS_1) }		--버드 워칭
SKILL_STRUCT[10] = { ["ResNo"] = 110000901, ["SkillNo"] = 110000901, ["Pos"] = Point2(250+30,SKILL_ICON_YPOS_2) }		--환각
SKILL_STRUCT[11] = { ["ResNo"] = 110003301, ["SkillNo"] = 110003301, ["Pos"] = Point2(250+30,SKILL_ICON_YPOS_3) }		--최면(환각 강화)
SKILL_STRUCT_CLASS[CT_HUNTER] = SKILL_STRUCT

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 110003001, ["SkillNo"] = 110003001, ["Pos"] = Point2(20,SKILL_ICON_YPOS_5) }		--HE 그레네이드
SKILL_STRUCT[1] = { ["ResNo"] = 150001201, ["SkillNo"] = 150001201, ["Pos"] = Point2(20,SKILL_ICON_YPOS_6) }		--콤포지트4(HE 그레네이드 강화)
SKILL_STRUCT[2] = { ["ResNo"] = 110000701, ["SkillNo"] = 110000701, ["Pos"] = Point2(97,SKILL_ICON_YPOS_2) }		--크럭스 샷
SKILL_STRUCT[3] = { ["ResNo"] = 110002001, ["SkillNo"] = 110002001, ["Pos"] = Point2(97,SKILL_ICON_YPOS_3) }		--난사
SKILL_STRUCT[4] = { ["ResNo"] = 103103001, ["SkillNo"] = 103103001, ["Pos"] = Point2(97,SKILL_ICON_YPOS_4) }		--투액션샷
SKILL_STRUCT[5] = { ["ResNo"] = 150001001, ["SkillNo"] = 150001001, ["Pos"] = Point2(97,SKILL_ICON_YPOS_5) }		--개틀링 러쉬
SKILL_STRUCT[6] = { ["ResNo"] = 150000301, ["SkillNo"] = 150000301, ["Pos"] = Point2(172,SKILL_ICON_YPOS_2) }		--바주카포
SKILL_STRUCT[7] = { ["ResNo"] = 150001401, ["SkillNo"] = 150001401, ["Pos"] = Point2(172,SKILL_ICON_YPOS_4) }		--바주카포 지상
SKILL_STRUCT[8] = { ["ResNo"] = 110001501, ["SkillNo"] = 110001501, ["Pos"] = Point2(172,SKILL_ICON_YPOS_6) }		--자동사격장치
SKILL_STRUCT[9] = { ["ResNo"] = 150001301, ["SkillNo"] = 150001301, ["Pos"] = Point2(210,SKILL_ICON_YPOS_3) }		--추진제 개조(바주카포 강화/바주카포 지상 강화)
SKILL_STRUCT[10] = { ["ResNo"] = 110002201, ["SkillNo"] = 110002201, ["Pos"] = Point2(250,SKILL_ICON_YPOS_2) }		--공중 연사
SKILL_STRUCT_CLASS[CT_RANGER] = SKILL_STRUCT

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 104000201, ["SkillNo"] = 104000201, ["Pos"] = Point2(20+35,SKILL_ICON_YPOS_1) }		--비상 탈출
SKILL_STRUCT[1] = { ["ResNo"] = 104000601, ["SkillNo"] = 104000601, ["Pos"] = Point2(20+35,SKILL_ICON_YPOS_2) }		--버로우
SKILL_STRUCT[2] = { ["ResNo"] = 104301401, ["SkillNo"] = 104301401, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_1) }		--롤링 스팅거
SKILL_STRUCT[3] = { ["ResNo"] = 104300901, ["SkillNo"] = 104300901, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_3) }		--스피닝 너클
SKILL_STRUCT[4] = { ["ResNo"] = 104300601, ["SkillNo"] = 104300601, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_4) }		--스크류 어퍼
SKILL_STRUCT[5] = { ["ResNo"] = 104301201, ["SkillNo"] = 104301201, ["Pos"] = Point2(172+35,SKILL_ICON_YPOS_2) }		--마인드 리딩
SKILL_STRUCT[6] = { ["ResNo"] = 104302401, ["SkillNo"] = 104302401, ["Pos"] = Point2(172+35,SKILL_ICON_YPOS_3) }		--모션 캡쳐
SKILL_STRUCT[7] = { ["ResNo"] = 104302201, ["SkillNo"] = 104302201, ["Pos"] = Point2(172+35,SKILL_ICON_YPOS_4) }		--하이퍼 스트렝스
SKILL_STRUCT[8] = { ["ResNo"] = 104302301, ["SkillNo"] = 104302301, ["Pos"] = Point2(172+35,SKILL_ICON_YPOS_5) }		--인텔리전스
SKILL_STRUCT[9] = { ["ResNo"] = 104300101, ["SkillNo"] = 104300101, ["Pos"] = Point2(250+35,SKILL_ICON_YPOS_1) }		--클로 마스터리
SKILL_STRUCT_CLASS[CT_CLOWN] = SKILL_STRUCT

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 104301501, ["SkillNo"] = 104301501, ["Pos"] = Point2(7+32,SKILL_ICON_YPOS_1) }			--포이즌 크래셔
SKILL_STRUCT[1] = { ["ResNo"] = 170003001, ["SkillNo"] = 170003001, ["Pos"] = Point2(7+32,SKILL_ICON_YPOS_2) }			--에이프릴 풀
SKILL_STRUCT[2] = { ["ResNo"] = 104000501, ["SkillNo"] = 104000501, ["Pos"] = Point2(71+32,SKILL_ICON_YPOS_1) }		--기습 공격
SKILL_STRUCT[3] = { ["ResNo"] = 104300501, ["SkillNo"] = 104300501, ["Pos"] = Point2(71+32,SKILL_ICON_YPOS_2) }		--마하 펀치
SKILL_STRUCT[4] = { ["ResNo"] = 104301801, ["SkillNo"] = 104301801, ["Pos"] = Point2(71+32,SKILL_ICON_YPOS_4) }		--스위프트 어택
SKILL_STRUCT[5] = { ["ResNo"] = 104301301, ["SkillNo"] = 104301301, ["Pos"] = Point2(135+32,SKILL_ICON_YPOS_3) }		--그림자 밟기
SKILL_STRUCT[6] = { ["ResNo"] = 104000601, ["SkillNo"] = 170001401, ["Pos"] = Point2(199+32,SKILL_ICON_YPOS_2) }		--버로우
SKILL_STRUCT[7] = { ["ResNo"] = 104301101, ["SkillNo"] = 104301101, ["Pos"] = Point2(199+32,SKILL_ICON_YPOS_3) }		--기믈렛
SKILL_STRUCT[8] = { ["ResNo"] = 104300301, ["SkillNo"] = 104300301, ["Pos"] = Point2(263+32,SKILL_ICON_YPOS_1) }		--카타르 블로킹
SKILL_STRUCT[9] = { ["ResNo"] = 104301001, ["SkillNo"] = 104301001, ["Pos"] = Point2(263+32,SKILL_ICON_YPOS_2) }		--쓰로잉 대거
SKILL_STRUCT[10] = { ["ResNo"] = 170000901, ["SkillNo"] = 170000901, ["Pos"] = Point2(263+32,SKILL_ICON_YPOS_3) }		--단검 능숙화(쓰로잉 대거 강화)
SKILL_STRUCT[11] = { ["ResNo"] = 104300701, ["SkillNo"] = 104300701, ["Pos"] = Point2(263+32,SKILL_ICON_YPOS_4) }		--카타르 마스터리
SKILL_STRUCT_CLASS[CT_ASSASSIN] = SKILL_STRUCT

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 105300101, ["SkillNo"] = 105300101, ["Pos"] = Point2(59+30,SKILL_ICON_YPOS_1) }		--아머 마스터리
SKILL_STRUCT[1] = { ["ResNo"] = 101000701, ["SkillNo"] = 101000701, ["Pos"] = Point2(20+30,SKILL_ICON_YPOS_2) }		--아머 브레이크
SKILL_STRUCT[2] = { ["ResNo"] = 105300501, ["SkillNo"] = 105300501, ["Pos"] = Point2(20+30,SKILL_ICON_YPOS_3) }		--조인트 브레이크
SKILL_STRUCT[3] = { ["ResNo"] = 105300301, ["SkillNo"] = 105300301, ["Pos"] = Point2(20+30,SKILL_ICON_YPOS_4) }		--웨폰 프리징
SKILL_STRUCT[4] = { ["ResNo"] = 105501501, ["SkillNo"] = 105501501, ["Pos"] = Point2(20+30,SKILL_ICON_YPOS_5) }		--웨폰 크래셔
SKILL_STRUCT[5] = { ["ResNo"] = 105500601, ["SkillNo"] = 105500601, ["Pos"] = Point2(20+30,SKILL_ICON_YPOS_6) }		--크로스 컷
SKILL_STRUCT[6] = { ["ResNo"] = 106300101, ["SkillNo"] = 106300101, ["Pos"] = Point2(97+30,SKILL_ICON_YPOS_2) }		--아이언 스킨
SKILL_STRUCT[7] = { ["ResNo"] = 105500801, ["SkillNo"] = 105500801, ["Pos"] = Point2(97+30,SKILL_ICON_YPOS_3) }		--바리케이트
SKILL_STRUCT[8] = { ["ResNo"] = 105501601, ["SkillNo"] = 105501601, ["Pos"] = Point2(97+30,SKILL_ICON_YPOS_4) }		--바퀴벌레 인생

SKILL_STRUCT[9] = { ["ResNo"] = 105500501, ["SkillNo"] = 105500501, ["Pos"] = Point2(250+30,SKILL_ICON_YPOS_3) }		--오라 발현
SKILL_STRUCT[10] = { ["ResNo"] = 105500901, ["SkillNo"] = 105500901, ["Pos"] = Point2(250+30,SKILL_ICON_YPOS_2) }		--리커버리 오라
SKILL_STRUCT[11] = { ["ResNo"] = 105501201, ["SkillNo"] = 105501201, ["Pos"] = Point2(172+30,SKILL_ICON_YPOS_4) }		--디펜시브 오라
SKILL_STRUCT[12] = { ["ResNo"] = 105501001, ["SkillNo"] = 105501001, ["Pos"] = Point2(250+30,SKILL_ICON_YPOS_4) }		--타임 리버스
SKILL_STRUCT[13] = { ["ResNo"] = 105501101, ["SkillNo"] = 105501101, ["Pos"] = Point2(250+30,SKILL_ICON_YPOS_5) }		--오펜시브 오라
SKILL_STRUCT[14] = { ["ResNo"] = 105501301, ["SkillNo"] = 105501301, ["Pos"] = Point2(211+30,SKILL_ICON_YPOS_6) }		--성스러운 보호

SKILL_STRUCT[15] = { ["ResNo"] = 105501801, ["SkillNo"] = 105501801, ["Pos"] = Point2(172+30,SKILL_ICON_YPOS_1) }		--도발
SKILL_STRUCT[16] = { ["ResNo"] = 105400501, ["SkillNo"] = 105400501, ["Pos"] = Point2(250+30,SKILL_ICON_YPOS_1) }		--필중
SKILL_STRUCT[17] = { ["ResNo"] = 105400401, ["SkillNo"] = 105400401, ["Pos"] = Point2(172+30,SKILL_ICON_YPOS_2) }		--쉴드 스트라이크
SKILL_STRUCT[18] = { ["ResNo"] = 105400101, ["SkillNo"] = 105400101, ["Pos"] = Point2(97+30,SKILL_ICON_YPOS_5) }		--체인 쉴드
SKILL_STRUCT[19] = { ["ResNo"] = 105400301, ["SkillNo"] = 105400301, ["Pos"] = Point2(97+30,SKILL_ICON_YPOS_6) }		--메가 스톰 블레이드
SKILL_STRUCT_CLASS[CT_PALADIN] = SKILL_STRUCT

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 106500101, ["SkillNo"] = 106500101, ["Pos"] = Point2(59+35,SKILL_ICON_YPOS_1) }		--샤프니스 블레이드
SKILL_STRUCT[1] = { ["ResNo"] = 106500501, ["SkillNo"] = 106500501, ["Pos"] = Point2(20+35,SKILL_ICON_YPOS_2) }		--거스트 슬래시
SKILL_STRUCT[2] = { ["ResNo"] = 106501201, ["SkillNo"] = 106501201, ["Pos"] = Point2(20+35,SKILL_ICON_YPOS_3) }		--블러디 로어
SKILL_STRUCT[3] = { ["ResNo"] = 106500601, ["SkillNo"] = 106500601, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_2) }		--브로큰 매그넘
SKILL_STRUCT[4] = { ["ResNo"] = 106500701, ["SkillNo"] = 106500701, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_3) }		--데스 바운드
SKILL_STRUCT[5] = { ["ResNo"] = 106500001, ["SkillNo"] = 106500001, ["Pos"] = Point2(211+35,SKILL_ICON_YPOS_1) }		--피지컬 트레이닝
SKILL_STRUCT[6] = { ["ResNo"] = 106500201, ["SkillNo"] = 106500201, ["Pos"] = Point2(172+35,SKILL_ICON_YPOS_2) }		--휠윈드
SKILL_STRUCT[7] = { ["ResNo"] = 106500901, ["SkillNo"] = 106500901, ["Pos"] = Point2(172+35,SKILL_ICON_YPOS_3) }		--레이징 스톰
SKILL_STRUCT[8] = { ["ResNo"] = 106500801, ["SkillNo"] = 106500801, ["Pos"] = Point2(250+35,SKILL_ICON_YPOS_2) }		--웨폰 퀴큰
SKILL_STRUCT[9] = { ["ResNo"] = 106300501, ["SkillNo"] = 106300501, ["Pos"] = Point2(250+35,SKILL_ICON_YPOS_3) }		--와일드 레이지
SKILL_STRUCT[10] = { ["ResNo"] = 106501101, ["SkillNo"] = 106501101, ["Pos"] = Point2(135+35,SKILL_ICON_YPOS_4) }		--와이번 블레이드
SKILL_STRUCT_CLASS[CT_GLADIATOR] = SKILL_STRUCT

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 102002601, ["SkillNo"] = 102002601, ["Pos"] = Point2(7+35,SKILL_ICON_YPOS_1) }			--스태프 마스터리
SKILL_STRUCT[1] = { ["ResNo"] = 109001301, ["SkillNo"] = 109001301, ["Pos"] = Point2(7+35,SKILL_ICON_YPOS_2) }			--스트라이킹
SKILL_STRUCT[2] = { ["ResNo"] = 102002501, ["SkillNo"] = 102002501, ["Pos"] = Point2(7+35,SKILL_ICON_YPOS_3) }			--블레싱
SKILL_STRUCT[3] = { ["ResNo"] = 109001501, ["SkillNo"] = 109001501, ["Pos"] = Point2(7+35,SKILL_ICON_YPOS_5) }			--체인 라이트닝
SKILL_STRUCT[4] = { ["ResNo"] = 107000501, ["SkillNo"] = 107000501, ["Pos"] = Point2(169+35,SKILL_ICON_YPOS_1) }		--큐어
SKILL_STRUCT[5] = { ["ResNo"] = 109001601, ["SkillNo"] = 109001601, ["Pos"] = Point2(135+35,SKILL_ICON_YPOS_2) }		--HP 리스토어
SKILL_STRUCT[6] = { ["ResNo"] = 109001801, ["SkillNo"] = 109001801, ["Pos"] = Point2(135+35,SKILL_ICON_YPOS_3) }		--배리어
SKILL_STRUCT[7] = { ["ResNo"] = 109001901, ["SkillNo"] = 109001901, ["Pos"] = Point2(135+35,SKILL_ICON_YPOS_5) }		--완전한 결계
SKILL_STRUCT[8] = { ["ResNo"] = 102002801, ["SkillNo"] = 102002801, ["Pos"] = Point2(71+35,SKILL_ICON_YPOS_4) }		--에너지 흡수
SKILL_STRUCT[9] = { ["ResNo"] = 109001701, ["SkillNo"] = 109001701, ["Pos"] = Point2(199+35,SKILL_ICON_YPOS_2) }		--MP 전이
SKILL_STRUCT[10] = { ["ResNo"] = 102002701, ["SkillNo"] = 102002701, ["Pos"] = Point2(199+35,SKILL_ICON_YPOS_5) }		--아크 생츄어리
SKILL_STRUCT[11] = { ["ResNo"] = 109000401, ["SkillNo"] = 109000401, ["Pos"] = Point2(263+35,SKILL_ICON_YPOS_3) }		--콰그마이어
SKILL_STRUCT[12] = { ["ResNo"] = 102002901, ["SkillNo"] = 102002901, ["Pos"] = Point2(263+35,SKILL_ICON_YPOS_4) }		--질퍽한 진흙
SKILL_STRUCT[13] = { ["ResNo"] = 103101801, ["SkillNo"] = 103101801, ["Pos"] = Point2(263+35,SKILL_ICON_YPOS_1) }		--더블샷
SKILL_STRUCT[14] = { ["ResNo"] = 102003501, ["SkillNo"] = 102003501, ["Pos"] = Point2(71+35,SKILL_ICON_YPOS_2) }		--스파이크 오브 롹
SKILL_STRUCT[15] = { ["ResNo"] = 108000201, ["SkillNo"] = 108000201, ["Pos"] = Point2(71+35,SKILL_ICON_YPOS_1) }		--후퇴
SKILL_STRUCT_CLASS[CT_WIZARD] = SKILL_STRUCT

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 102003401, ["SkillNo"] = 102003401, ["Pos"] = Point2(59+35,SKILL_ICON_YPOS_1) }		--에너지 변환
SKILL_STRUCT[1] = { ["ResNo"] = 109002101, ["SkillNo"] = 109002101, ["Pos"] = Point2(20+35,SKILL_ICON_YPOS_2) }		--리소스 컨버터
SKILL_STRUCT[2] = { ["ResNo"] = 109002001, ["SkillNo"] = 109002001, ["Pos"] = Point2(20+35,SKILL_ICON_YPOS_3) }		--미러 이미지
SKILL_STRUCT[3] = { ["ResNo"] = 107000701, ["SkillNo"] = 107000701, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_3) }		--감염
SKILL_STRUCT[4] = { ["ResNo"] = 109002201, ["SkillNo"] = 109002201, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_4) }		--문어 먹물
SKILL_STRUCT[5] = { ["ResNo"] = 109002301, ["SkillNo"] = 109002301, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_5) }		--스노우 블루스
SKILL_STRUCT[6] = { ["ResNo"] = 109001201, ["SkillNo"] = 109001201, ["Pos"] = Point2(211+35,SKILL_ICON_YPOS_1) }		--어드벤스드 스피어 마스터리
SKILL_STRUCT[7] = { ["ResNo"] = 109000601, ["SkillNo"] = 109000601, ["Pos"] = Point2(172+35,SKILL_ICON_YPOS_2) }		--스파이럴 캐논
SKILL_STRUCT[8] = { ["ResNo"] = 109003001, ["SkillNo"] = 109003001, ["Pos"] = Point2(172+35,SKILL_ICON_YPOS_3) }		--에너지 버스터
SKILL_STRUCT[9] = { ["ResNo"] = 109002401, ["SkillNo"] = 109002401, ["Pos"] = Point2(250+35,SKILL_ICON_YPOS_3) }		--백 드래프트
SKILL_STRUCT[10] = { ["ResNo"] = 109001401, ["SkillNo"] = 109001401, ["Pos"] = Point2(211+35,SKILL_ICON_YPOS_4) }		--썬더 브레이크
SKILL_STRUCT[11] = { ["ResNo"] = 109002601, ["SkillNo"] = 109002601, ["Pos"] = Point2(135+35,SKILL_ICON_YPOS_6) }		--메테오 스트라이크
SKILL_STRUCT[12] = { ["ResNo"] = 109003101, ["SkillNo"] = 109003101, ["Pos"] = Point2(20+35,SKILL_ICON_YPOS_4) }		--프로즌샷
SKILL_STRUCT_CLASS[CT_WARMAGE] = SKILL_STRUCT

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 150000401, ["SkillNo"] = 150000401, ["Pos"] = Point2(7+35,SKILL_ICON_YPOS_1) }			--관통
SKILL_STRUCT[1] = { ["ResNo"] = 150000501, ["SkillNo"] = 150000501, ["Pos"] = Point2(7+35,SKILL_ICON_YPOS_2) }			--에이밍
SKILL_STRUCT[2] = { ["ResNo"] = 110003401, ["SkillNo"] = 110003401, ["Pos"] = Point2(7+35,SKILL_ICON_YPOS_3) }			--약점 간파(에이밍강화 스킬)
SKILL_STRUCT[3] = { ["ResNo"] = 110001101, ["SkillNo"] = 110001101, ["Pos"] = Point2(71+35,SKILL_ICON_YPOS_2) }		--팔콘 비트
SKILL_STRUCT[4] = { ["ResNo"] = 150001901, ["SkillNo"] = 150001901, ["Pos"] = Point2(71+35,SKILL_ICON_YPOS_4) }		--팔콘리 마스터리(팔콘비트 강화)
SKILL_STRUCT[5] = { ["ResNo"] = 110001401, ["SkillNo"] = 110001401, ["Pos"] = Point2(135+35,SKILL_ICON_YPOS_1) }		--클레이모어
SKILL_STRUCT[6] = { ["ResNo"] = 110002801, ["SkillNo"] = 110002801, ["Pos"] = Point2(135+35,SKILL_ICON_YPOS_2) }		--바운드 트랩
SKILL_STRUCT[7] = { ["ResNo"] = 110000801, ["SkillNo"] = 110000801, ["Pos"] = Point2(135+35,SKILL_ICON_YPOS_3) }		--스플래쉬 트랩
SKILL_STRUCT[8] = { ["ResNo"] = 110002601, ["SkillNo"] = 110002601, ["Pos"] = Point2(135+35,SKILL_ICON_YPOS_4) }		--프리징 트랩
SKILL_STRUCT[9] = { ["ResNo"] = 110002701, ["SkillNo"] = 110002701, ["Pos"] = Point2(135+35,SKILL_ICON_YPOS_5) }		--사일런스 트랩
SKILL_STRUCT[10] = { ["ResNo"] = 110001601, ["SkillNo"] = 110001601, ["Pos"] = Point2(199+35,SKILL_ICON_YPOS_2) }		--디토네이트
SKILL_STRUCT[11] = { ["ResNo"] = 110002501, ["SkillNo"] = 110002501, ["Pos"] = Point2(199+35,SKILL_ICON_YPOS_6) }		--건들지마라
SKILL_STRUCT[12] = { ["ResNo"] = 150002001, ["SkillNo"] = 150002001, ["Pos"] = Point2(71+35,SKILL_ICON_YPOS_3) }		--블릿츠
SKILL_STRUCT[13] = { ["ResNo"] = 110002401, ["SkillNo"] = 110002401, ["Pos"] = Point2(263+35,SKILL_ICON_YPOS_5) }	--인비저블 트랩
SKILL_STRUCT[14] = { ["ResNo"] = 110003501, ["SkillNo"] = 110003501, ["Pos"] = Point2(263+35,SKILL_ICON_YPOS_2) }	--테슬라 코일
SKILL_STRUCT_CLASS[CT_TRAPPER] = SKILL_STRUCT

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 110002301, ["SkillNo"] = 110002301, ["Pos"] = Point2(59+35,SKILL_ICON_YPOS_1) }		--집중
SKILL_STRUCT[1] = { ["ResNo"] = 150000601, ["SkillNo"] = 150000601, ["Pos"] = Point2(59+35,SKILL_ICON_YPOS_2) }		--플래시뱅
SKILL_STRUCT[2] = { ["ResNo"] = 110002901, ["SkillNo"] = 110002901, ["Pos"] = Point2(20+35,SKILL_ICON_YPOS_3) }		--커즈드 봄버
SKILL_STRUCT[3] = { ["ResNo"] = 150000801, ["SkillNo"] = 150000801, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_3) }		--파이어 그레네이드
SKILL_STRUCT[4] = { ["ResNo"] = 150001101, ["SkillNo"] = 150001101, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_5) }		--융단폭격
SKILL_STRUCT[5] = { ["ResNo"] = 150001501, ["SkillNo"] = 150001501, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_6) }		--추가 폭격(용단폭격 강화)
SKILL_STRUCT[6] = { ["ResNo"] = 150001601, ["SkillNo"] = 150001601, ["Pos"] = Point2(172+35,SKILL_ICON_YPOS_1) }		--ASS구동원리 변경
SKILL_STRUCT[7] = { ["ResNo"] = 150001701, ["SkillNo"] = 150001701, ["Pos"] = Point2(172+35,SKILL_ICON_YPOS_3) }		--발칸300
SKILL_STRUCT[8] = { ["ResNo"] = 110001701, ["SkillNo"] = 110001701, ["Pos"] = Point2(250+35,SKILL_ICON_YPOS_2) }		--카모플라주
SKILL_STRUCT[9] = { ["ResNo"] = 150000701, ["SkillNo"] = 150000701, ["Pos"] = Point2(250+35,SKILL_ICON_YPOS_3) }		--아드레날린
SKILL_STRUCT[10] = { ["ResNo"] = 150000901, ["SkillNo"] = 150000901, ["Pos"] = Point2(250+35,SKILL_ICON_YPOS_4) }		--아트로핀
SKILL_STRUCT[11] = { ["ResNo"] = 150002101, ["SkillNo"] = 150002101, ["Pos"] = Point2(250+35,SKILL_ICON_YPOS_1) }		--스나이핑
SKILL_STRUCT_CLASS[CT_SNIPER] = SKILL_STRUCT

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 160000401, ["SkillNo"] = 160000401, ["Pos"] = Point2(59+35,SKILL_ICON_YPOS_1) }		--문워크
SKILL_STRUCT[1] = { ["ResNo"] = 160000101, ["SkillNo"] = 160000101, ["Pos"] = Point2(20+35,SKILL_ICON_YPOS_2) }		--메가 드릴 드라이브
SKILL_STRUCT[2] = { ["ResNo"] = 170000801, ["SkillNo"] = 170000801, ["Pos"] = Point2(20+35,SKILL_ICON_YPOS_3) }		--연환질풍각
SKILL_STRUCT[3] = { ["ResNo"] = 160000901, ["SkillNo"] = 160000901, ["Pos"] = Point2(20+35,SKILL_ICON_YPOS_4) }		--강타(연환질풍각 강화)
SKILL_STRUCT[4] = { ["ResNo"] = 160000201, ["SkillNo"] = 160000201, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_2) }		--윈드밀
SKILL_STRUCT[5] = { ["ResNo"] = 160000301, ["SkillNo"] = 160000301, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_4) }		--세븐
SKILL_STRUCT[6] = { ["ResNo"] = 160000601, ["SkillNo"] = 160000601, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_5) }		--나인틴 나이티
SKILL_STRUCT[7] = { ["ResNo"] = 160000001, ["SkillNo"] = 160000001, ["Pos"] = Point2(250+35,SKILL_ICON_YPOS_1) }		--스탠스
SKILL_STRUCT[8] = { ["ResNo"] = 104300401, ["SkillNo"] = 104300401, ["Pos"] = Point2(172+35,SKILL_ICON_YPOS_2) }		--소닉붐
SKILL_STRUCT[9] = { ["ResNo"] = 104301701, ["SkillNo"] = 104301701, ["Pos"] = Point2(172+35,SKILL_ICON_YPOS_3) }		--저지 드레드
SKILL_STRUCT[10] = { ["ResNo"] = 160000701, ["SkillNo"] = 160000701, ["Pos"] = Point2(172+35,SKILL_ICON_YPOS_5) }		--코사크 댄스
SKILL_STRUCT[11] = { ["ResNo"] = 160000501, ["SkillNo"] = 160000501, ["Pos"] = Point2(250+35,SKILL_ICON_YPOS_4) }		--프로보크
SKILL_STRUCT[12] = { ["ResNo"] = 159999101, ["SkillNo"] = 159999101, ["Pos"] = Point2(172+35,SKILL_ICON_YPOS_1) }		--스탭 더 비트
SKILL_STRUCT[13] = { ["ResNo"] = 159999201, ["SkillNo"] = 159999201, ["Pos"] = Point2(97+35,SKILL_ICON_YPOS_3) }		--헤드 스핀
--SKILL_STRUCT[] = { ["ResNo"] = 160001201, ["SkillNo"] = 160001201, ["Pos"] = Point2(250,SKILL_ICON_YPOS_1) }		--댄서 변신
SKILL_STRUCT_CLASS[CT_DANCER] = SKILL_STRUCT

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 170000301, ["SkillNo"] = 170000301, ["Pos"] = Point2(7+30,SKILL_ICON_YPOS_1) }			--미혼향
SKILL_STRUCT[1] = { ["ResNo"] = 170000101, ["SkillNo"] = 170000101, ["Pos"] = Point2(71+30,SKILL_ICON_YPOS_2) }		--그림자 도약
SKILL_STRUCT[2] = { ["ResNo"] = 170000701, ["SkillNo"] = 170000701, ["Pos"] = Point2(71+30,SKILL_ICON_YPOS_6) }		--격! 일섬!
SKILL_STRUCT[3] = { ["ResNo"] = 170000001, ["SkillNo"] = 170000001, ["Pos"] = Point2(135+30,SKILL_ICON_YPOS_1) }		--풍마 수리검
SKILL_STRUCT[4] = { ["ResNo"] = 170000501, ["SkillNo"] = 170000501, ["Pos"] = Point2(135+30,SKILL_ICON_YPOS_4) }		--목둔술! 통나무 굴리기
SKILL_STRUCT[5] = { ["ResNo"] = 170001301, ["SkillNo"] = 170001301, ["Pos"] = Point2(199+30,SKILL_ICON_YPOS_2) }		--날 갈기(풍마 수리검 강화)
SKILL_STRUCT[6] = { ["ResNo"] = 170002801, ["SkillNo"] = 170002801, ["Pos"] = Point2(199+30,SKILL_ICON_YPOS_6) }		--하늘 꿰뚫기
SKILL_STRUCT[7] = { ["ResNo"] = 170001101, ["SkillNo"] = 170001101, ["Pos"] = Point2(263+30,SKILL_ICON_YPOS_2) }		--닌자변신
SKILL_STRUCT[8] = { ["ResNo"] = 170002501, ["SkillNo"] = 170002501, ["Pos"] = Point2(263+30,SKILL_ICON_YPOS_3) }		--벚꽃
SKILL_STRUCT[9] = { ["ResNo"] = 170002601, ["SkillNo"] = 170002601, ["Pos"] = Point2(263+30,SKILL_ICON_YPOS_4) }		--낙엽 떨구기
SKILL_STRUCT[10] = { ["ResNo"] = 170002701, ["SkillNo"] = 170002701, ["Pos"] = Point2(263+30,SKILL_ICON_YPOS_5) }		--안개죽이기
SKILL_STRUCT[11] = { ["ResNo"] = 170003101, ["SkillNo"] = 170003101, ["Pos"] = Point2(199+30,SKILL_ICON_YPOS_3) }		--질풍답보

SKILL_STRUCT_CLASS[CT_NINJA] = SKILL_STRUCT
--SKILL_STRUCT[2] = { ["ResNo"] = 170000201, ["SkillNo"] = 170000201, ["Pos"] = Point2(71,SKILL_ICON_YPOS_3) }		--허공밟기
--SKILL_STRUCT[2] = { ["ResNo"] = 170000401, ["SkillNo"] = 170000401, ["Pos"] = Point2(71,77) }		--압정
--SKILL_STRUCT[1] = { ["ResNo"] = 103202101, ["SkillNo"] = 103202101, ["Pos"] = Point2(71,SKILL_ICON_YPOS_1) }		--슬라이딩 테클
--SKILL_STRUCT[1] = { ["ResNo"] = 103202301, ["SkillNo"] = 103202301, ["Pos"] = Point2(71,SKILL_ICON_YPOS_1) }		--사이드대쉬 어택

-- 용기사
SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 2000102001, ["SkillNo"] = 2000102001, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_1) }		--롤링 그라운드
SKILL_STRUCT[1] = { ["ResNo"] = 2000000101, ["SkillNo"] = 2000000001, ["Pos"] = Point2(SKILL4_ICON_XPOS_3, SKILL4_ICON_YPOS_1) }		--각성
SKILL_STRUCT[2] = { ["ResNo"] = 2000101001, ["SkillNo"] = 2000101001, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_1) }		--용의 비늘
SKILL_STRUCT[3] = { ["ResNo"] = 2000110101, ["SkillNo"] = 2000110101, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_2) }		--승천룡
SKILL_STRUCT[4] = { ["ResNo"] = 2009110101, ["SkillNo"] = 2009110101, ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_2) }		--궁극기-드래곤 러쉬
SKILL_STRUCT[5] = { ["ResNo"] = 2000120101, ["SkillNo"] = 2000120101, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_3) }		--드래곤 다이브
SKILL_STRUCT[6] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_4) }		--브래스 화이어 2000130101
SKILL_STRUCT[7] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_4) }		--각성의 파동 2000001001
SKILL_STRUCT[8] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_4) }		-- 극진오의 2009111101
SKILL_STRUCT[9] = { ["ResNo"] = 2000002001, ["SkillNo"] = 2000002001, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_4) }		-- 각성챠지
SKILL_STRUCT_CLASS[CT_DRAOON] = SKILL_STRUCT

-- 파괴자
SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 2000202001, ["SkillNo"] = 2000202001, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_1) }		--데스 스네치
SKILL_STRUCT[1] = { ["ResNo"] = 2000000101, ["SkillNo"] = 2000000001, ["Pos"] = Point2(SKILL4_ICON_XPOS_3, SKILL4_ICON_YPOS_1) }		--각성
SKILL_STRUCT[2] = { ["ResNo"] = 2000201001, ["SkillNo"] = 2000201001, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_1) }		--윈드 스탭
SKILL_STRUCT[3] = { ["ResNo"] = 2000210101, ["SkillNo"] = 2000210101, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_2) }		--데스 스팅거 
SKILL_STRUCT[4] = { ["ResNo"] = 2009210101, ["SkillNo"] = 2009210101, ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_2) }		--궁극기-데몰리션 봄버
SKILL_STRUCT[5] = { ["ResNo"] = 2000220101, ["SkillNo"] = 2000220101, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_3) }		--데몰리션 챠지
SKILL_STRUCT[6] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_4) }		--디 엔드 2000230101
SKILL_STRUCT[7] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_4) }		--각성의 파동 2000001001
SKILL_STRUCT[8] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_4) }		-- 극진오의 2009211101
SKILL_STRUCT[9] = { ["ResNo"] = 2000002001, ["SkillNo"] = 2000002001, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_4) }		-- 각성챠지
SKILL_STRUCT_CLASS[CT_DESTROYER] = SKILL_STRUCT	

-- 아크메이지
SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 2000302001, ["SkillNo"] = 2000302001, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_1) }		--힐링웨이브
SKILL_STRUCT[1] = { ["ResNo"] = 2000000101, ["SkillNo"] = 2000000001, ["Pos"] = Point2(SKILL4_ICON_XPOS_3, SKILL4_ICON_YPOS_1) }		--각성
SKILL_STRUCT[2] = { ["ResNo"] = 2000301001, ["SkillNo"] = 2000301001, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_1) }		--블러드 피버
SKILL_STRUCT[3] = { ["ResNo"] = 2000310101, ["SkillNo"] = 2000310101, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_2) }		--라이트닝 마그넷 
SKILL_STRUCT[4] = { ["ResNo"] = 2009310101, ["SkillNo"] = 2009310101, ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_2) }		--궁극기-라이트닝 스톰
SKILL_STRUCT[5] = { ["ResNo"] = 2000320101, ["SkillNo"] = 2000320101, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_3) }		--레이스 오브 헤븐
SKILL_STRUCT[6] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_4) }		--라이트닝 웨이브 2000330101
SKILL_STRUCT[7] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_4) }		--각성의 파동 2000001001
SKILL_STRUCT[8] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_4) }		-- 극진오의 2009311101
SKILL_STRUCT[9] = { ["ResNo"] = 2000002001, ["SkillNo"] = 2000002001, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_4) }		-- 각성챠지
SKILL_STRUCT_CLASS[CT_ARCHMAGE] = SKILL_STRUCT

-- 워로드
SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 2000402001, ["SkillNo"] = 2000402001, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_1) }		--타임 프리즈
SKILL_STRUCT[1] = { ["ResNo"] = 2000000101, ["SkillNo"] = 2000000001, ["Pos"] = Point2(SKILL4_ICON_XPOS_3, SKILL4_ICON_YPOS_1) }		--각성
SKILL_STRUCT[2] = { ["ResNo"] = 2000401001, ["SkillNo"] = 2000401001, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_1) }		--피의 각서
SKILL_STRUCT[3] = { ["ResNo"] = 2000410101, ["SkillNo"] = 2000410101, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_2) }		--마그마 피스톤
SKILL_STRUCT[4] = { ["ResNo"] = 2009410101, ["SkillNo"] = 2009410101, ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_2) }		--궁극기-리버스 그라비티
SKILL_STRUCT[5] = { ["ResNo"] = 2000420101, ["SkillNo"] = 2000420101, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_3) }		--소울 드레인
SKILL_STRUCT[6] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_4) }		--앤드 오브 데이 2000430101
SKILL_STRUCT[7] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_4) }		--각성의 파동 2000001001
SKILL_STRUCT[8] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_4) }		-- 극진오의 2009411101
SKILL_STRUCT[9] = { ["ResNo"] = 2000002001, ["SkillNo"] = 2000002001, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_4) }		-- 각성챠지
SKILL_STRUCT_CLASS[CT_WARLORD] = SKILL_STRUCT

-- 수호자
SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 2000502001, ["SkillNo"] = 2000502001, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_1) }		--와이어액션 발동
SKILL_STRUCT[1] = { ["ResNo"] = 2000000101, ["SkillNo"] = 2000000001, ["Pos"] = Point2(SKILL4_ICON_XPOS_3, SKILL4_ICON_YPOS_1) }		--각성
SKILL_STRUCT[2] = { ["ResNo"] = 2000501001, ["SkillNo"] = 2000501001, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_1) }		--바람의 노래
SKILL_STRUCT[3] = { ["ResNo"] = 2000510101, ["SkillNo"] = 2000510101, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_2) }		--아이스 샤워 
SKILL_STRUCT[4] = { ["ResNo"] = 2009610101, ["SkillNo"] = 2009610101, ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_2) }		--궁극기-멀티 스프라이트
SKILL_STRUCT[5] = { ["ResNo"] = 2000520101, ["SkillNo"] = 2000520101, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_3) }		--윈드 블레이드
SKILL_STRUCT[6] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_4) }		--프로스트 드릴 2000530101
SKILL_STRUCT[7] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_4) }		--각성의 파동 2000001001
SKILL_STRUCT[8] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_4) }		-- 극진오의 2009611101
SKILL_STRUCT[9] = { ["ResNo"] = 2000002001, ["SkillNo"] = 2000002001, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_4) }		-- 각성챠지
SKILL_STRUCT_CLASS[CT_SENTINEL] = SKILL_STRUCT

-- 스트라이커
SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 2000602001, ["SkillNo"] = 2000602001, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_1) }		--셀프 봄버
SKILL_STRUCT[1] = { ["ResNo"] = 2000000101, ["SkillNo"] = 2000000001, ["Pos"] = Point2(SKILL4_ICON_XPOS_3, SKILL4_ICON_YPOS_1) }		--각성
SKILL_STRUCT[2] = { ["ResNo"] = 2000601001, ["SkillNo"] = 2000601001, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_1) }		--N2탄약 강화
SKILL_STRUCT[3] = { ["ResNo"] = 2000620101, ["SkillNo"] = 2000620101, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_2) }		--뉴클리어
SKILL_STRUCT[4] = { ["ResNo"] = 2009510101, ["SkillNo"] = 2009510101, ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_2) }		--궁극기-그라운드 제로
SKILL_STRUCT[5] = { ["ResNo"] = 2000610101, ["SkillNo"] = 2000610101, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_3) }		--파티클 캐논
SKILL_STRUCT[6] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_4) }		--올 레인지 어택 2000630101
SKILL_STRUCT[7] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_4) }		--각성의 파동 2000001001
SKILL_STRUCT[8] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_4) }		-- 극진오의 2009511101
SKILL_STRUCT[9] = { ["ResNo"] = 2000002001, ["SkillNo"] = 2000002001, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_4) }		-- 각성챠지
SKILL_STRUCT_CLASS[CT_LAUNCHER] = SKILL_STRUCT

-- 매니악(프리재커)
SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 2000702001, ["SkillNo"] = 2000702001, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_1) }		--클로 피싱
SKILL_STRUCT[1] = { ["ResNo"] = 2000000101, ["SkillNo"] = 2000000001, ["Pos"] = Point2(SKILL4_ICON_XPOS_3, SKILL4_ICON_YPOS_1) }		--각성
SKILL_STRUCT[2] = { ["ResNo"] = 2000701001, ["SkillNo"] = 2000701001, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_1) }		--크레이지 소울
SKILL_STRUCT[3] = { ["ResNo"] = 2000710101, ["SkillNo"] = 2000710101, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_2) }		--헬 샤우팅
SKILL_STRUCT[4] = { ["ResNo"] = 2009710101, ["SkillNo"] = 2009710101, ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_2) }		--궁극기-클럽 매니아
SKILL_STRUCT[5] = { ["ResNo"] = 2000720101, ["SkillNo"] = 2000720101, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_3) }		--마리오네트
SKILL_STRUCT[6] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_4) }		--길로틴웨이브 2000730101
SKILL_STRUCT[7] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_4) }		--각성의 파동 2000001001
SKILL_STRUCT[8] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_4) }		-- 극진오의 2009711101
SKILL_STRUCT[9] = { ["ResNo"] = 2000002001, ["SkillNo"] = 2000002001, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_4) }		-- 각성챠지
SKILL_STRUCT_CLASS[CT_FREEJACKER] = SKILL_STRUCT

-- 새도우
SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 2000802001, ["SkillNo"] = 2000802001, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_1) }		--프로텍트 엣지
SKILL_STRUCT[1] = { ["ResNo"] = 2000000101, ["SkillNo"] = 2000000001, ["Pos"] = Point2(SKILL4_ICON_XPOS_3, SKILL4_ICON_YPOS_1) }		--각성
SKILL_STRUCT[2] = { ["ResNo"] = 2000801001, ["SkillNo"] = 2000801001, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_1) }		--닷지 마스터
SKILL_STRUCT[3] = { ["ResNo"] = 2000810101, ["SkillNo"] = 2000810101, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_2) }		--크로스 블레이드
SKILL_STRUCT[4] = { ["ResNo"] = 2009810101, ["SkillNo"] = 2009810101, ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_2) }		--궁극기-환영 분신술
SKILL_STRUCT[5] = { ["ResNo"] = 2000820101, ["SkillNo"] = 2000820101, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_3) }		--쉐도우 블레이드
SKILL_STRUCT[6] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_4) }		--블레이드 댄스 2000830101
SKILL_STRUCT[7] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_4) }		--각성의 파동 2000001001
SKILL_STRUCT[8] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_4) }		-- 극진오의 2009811101
SKILL_STRUCT[9] = { ["ResNo"] = 2000002001, ["SkillNo"] = 2000002001, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_4) }		-- 각성챠지
SKILL_STRUCT_CLASS[CT_SHADOW] = SKILL_STRUCT

--소환사 1차 직업
SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 300101101, ["SkillNo"] = 300101101, ["Pos"] = Point2(7+55, 8+50) }	--미스트 소환 Lv1
SKILL_STRUCT[1] = { ["ResNo"] = 300100101, ["SkillNo"] = 300100101, ["Pos"] = Point2(145+55, 8+50) }	-- 아이비 쏜즈
SKILL_STRUCT[2] = { ["ResNo"] = 300100201, ["SkillNo"] = 300100201, ["Pos"] = Point2(209+55, 38+50) }	-- 그라운드 피스톤
SKILL_STRUCT[3] = { ["ResNo"] = 300101301, ["SkillNo"] = 300101301, ["Pos"] = Point2(71+55, 98+50) }	--소환 마스터리 Lv1 (1차 직업_패시브)
SKILL_STRUCT[4] = { ["ResNo"] = 300100301, ["SkillNo"] = 300100301, ["Pos"] = Point2(145+55, 178+50) }	-- 아이비 트랩
SKILL_STRUCT[5] = { ["ResNo"] = 300100601, ["SkillNo"] = 300100601, ["Pos"] = Point2(209+55, 158+50) }	--에어 스프레쉬
SKILL_STRUCT[6] = { ["ResNo"] = 300101201, ["SkillNo"] = 300101201, ["Pos"] = Point2(7+55, 258+50) }	--다이크 소환 Lv1
SKILL_STRUCT[7] = { ["ResNo"] = 300101401, ["SkillNo"] = 300101401, ["Pos"] = Point2(71+55, 258+50) }	--소환체 강화 Lv1 (패시브)
SKILL_STRUCT[8] = { ["ResNo"] = 300100501, ["SkillNo"] = 300100501, ["Pos"] = Point2(145+55, 258+50) }	--주술 강화
SKILL_STRUCT[9] = { ["ResNo"] = 300101001, ["SkillNo"] = 300101001, ["Pos"] = Point2(7+55, 338+50) }	--리게인 (언 써몬)
SKILL_STRUCT[10] = { ["ResNo"] = 300100901, ["SkillNo"] = 300100901, ["Pos"] = Point2(103+55, 338+50) }	--자연의 숨결 (친교 스킬)
SKILL_STRUCT[11] = { ["ResNo"] = 300100701, ["SkillNo"] = 300100701, ["Pos"] = Point2(209+55, 338+50) }	--아이비 러쉬
SKILL_STRUCT[12] = { ["ResNo"] = 300102001, ["SkillNo"] = 300102001, ["Pos"] = Point2(209+55, 258+50) }	--용력 강화 (패시브)
SKILL_STRUCT_CLASS[CT_SHAMAN] = SKILL_STRUCT

--소환사 2차 직업
SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 300200101, ["SkillNo"] = 300200101, ["Pos"] = Point2(7, 8+50) }	--다이어 미스트(패시브)
SKILL_STRUCT[1] = { ["ResNo"] = 300200201, ["SkillNo"] = 300200201, ["Pos"] = Point2(71, 8+50) }	--집합
SKILL_STRUCT[2] = { ["ResNo"] = 300200301, ["SkillNo"] = 300200301, ["Pos"] = Point2(135, 8+50) }	--Adv 소환 마스터리
SKILL_STRUCT[3] = { ["ResNo"] = 300201201, ["SkillNo"] = 300201201, ["Pos"] = Point2(199, 8+50) }	--에어 스메쉬 Lv1
SKILL_STRUCT[4] = { ["ResNo"] = 300201401, ["SkillNo"] = 300201401, ["Pos"] = Point2(263, 8+50) }	--인페르노 Lv1
SKILL_STRUCT[5] = { ["ResNo"] = 300201301, ["SkillNo"] = 300201301, ["Pos"] = Point2(327, 8+50) }	--샷 마스터리 Lv1 (패시브)
SKILL_STRUCT[6] = { ["ResNo"] = 300200401, ["SkillNo"] = 300200401, ["Pos"] = Point2(7, 118+50) }	--생명의 외침 Lv1 (소환체 회복)
SKILL_STRUCT[7] = { ["ResNo"] = 300200501, ["SkillNo"] = 300200501, ["Pos"] = Point2(71, 118+50) }	--두나믹 소환 Lv1
SKILL_STRUCT[8] = { ["ResNo"] = 300200601, ["SkillNo"] = 300200601, ["Pos"] = Point2(135, 178+50) }	--강철 가죽 Lv1 (두나믹 패시브_방어)
SKILL_STRUCT[9] = { ["ResNo"] = 300201701, ["SkillNo"] = 300201701, ["Pos"] = Point2(199, 158+50) }--네이션 퓨리 Lv1 (패시브)
SKILL_STRUCT[10] = { ["ResNo"] = 300201601, ["SkillNo"] = 300201601, ["Pos"] = Point2(263, 98+50) }	--볼케이노 Lv1
SKILL_STRUCT[11] = { ["ResNo"] = 300201501, ["SkillNo"] = 300201501, ["Pos"] = Point2(327, 98+50) }	--스톤가드 
SKILL_STRUCT[12] = { ["ResNo"] = 300200901, ["SkillNo"] = 300200901, ["Pos"] = Point2(39, 258+50) }	--칼키 소환 Lv1
SKILL_STRUCT[13] = { ["ResNo"] = 300200701, ["SkillNo"] = 300200701, ["Pos"] = Point2(135, 258+50) }	--스틸 크로우 1Lv (두나믹 패시브_장풍)
SKILL_STRUCT[14] = { ["ResNo"] = 300201801, ["SkillNo"] = 300201801, ["Pos"] = Point2(263, 218+50) }	--메테오 레인 시전 Lv1
SKILL_STRUCT[15] = { ["ResNo"] = 300201001, ["SkillNo"] = 300201001, ["Pos"] = Point2(7, 338+50) }	--어웨이크 인스팅트 (칼키 패시브_속도)
SKILL_STRUCT[16] = { ["ResNo"] = 300201101, ["SkillNo"] = 300201101, ["Pos"] = Point2(71, 338+50) }	--퓨리 Lv1 (칼키 패시브_강하공격)
SKILL_STRUCT[17] = { ["ResNo"] = 300200801, ["SkillNo"] = 300200801, ["Pos"] = Point2(135, 338+50) }	--롤링 윈드 1Lv (두나믹 패시브_롤링)
SKILL_STRUCT[18] = { ["ResNo"] = 300202101, ["SkillNo"] = 300202101, ["Pos"] = Point2(263, 338+50) }	--어스 퀘이크 시전 Lv1
SKILL_STRUCT[19] = { ["ResNo"] = 300202001, ["SkillNo"] = 300202001, ["Pos"] = Point2(327, 338+50) }	--바위 감옥  Lv1 (자신에게 거는 버프)
SKILL_STRUCT[20] = { ["ResNo"] = 300300401, ["SkillNo"] = 300300401, ["Pos"] = Point2(135, 118+10) }	-- 공격
SKILL_STRUCT[21] = { ["ResNo"] = 300203901, ["SkillNo"] = 300203901, ["Pos"] = Point2(199, 338+20) }	-- 에너지 순환
SKILL_STRUCT_CLASS[CT_SUMMONER] = SKILL_STRUCT

--소환사 3차 직업
SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 300300101, ["SkillNo"] = 300300101, ["Pos"] = Point2(7, 8+50) }	-- 블레서 소환
SKILL_STRUCT[1] = { ["ResNo"] = 300302001, ["SkillNo"] = 300302001, ["Pos"] = Point2(327,  338+50) }	-- 메가 아이비 러쉬
SKILL_STRUCT[2] = { ["ResNo"] = 300300301, ["SkillNo"] = 300300301, ["Pos"] = Point2(71, 8+50) }	-- 네이쳐 블레싱
SKILL_STRUCT[3] = { ["ResNo"] = 300301301, ["SkillNo"] = 300301301, ["Pos"] = Point2(199, 8+50) }	-- 스마트 샷
SKILL_STRUCT[4] = { ["ResNo"] = 300301401, ["SkillNo"] = 300301401, ["Pos"] = Point2(263, 8+50) }	-- 포이즌 가드
SKILL_STRUCT[5] = { ["ResNo"] = 300300201, ["SkillNo"] = 300300201, ["Pos"] = Point2(7, 98+50) }	-- 호크아이
SKILL_STRUCT[6] = { ["ResNo"] = 300300501, ["SkillNo"] = 300300501, ["Pos"] = Point2(135, 98+50) }	-- 골곤 소환
SKILL_STRUCT[7] = { ["ResNo"] = 300301601, ["SkillNo"] = 300301601, ["Pos"] = Point2(199, 98+50) }	-- 마나번
SKILL_STRUCT[8] = { ["ResNo"] = 300301501, ["SkillNo"] = 300301501, ["Pos"] = Point2(327, 68+50) }	-- 톡신 플루이드
SKILL_STRUCT[9] = { ["ResNo"] = 300300901, ["SkillNo"] = 300300901, ["Pos"] = Point2(7, 178+50) }	-- 엘리멘탈 블레싱 Lv1 (소환용량 증가 패시브)
SKILL_STRUCT[10] = { ["ResNo"] = 300300601, ["SkillNo"] = 300300601, ["Pos"] = Point2(135, 178+50) }	-- 원소 변환 Lv1 (골곤 강화 패시브_모델변경)
SKILL_STRUCT[11] = { ["ResNo"] = 300301701, ["SkillNo"] = 300301701, ["Pos"] = Point2(199, 218+50) }	-- 마나 리액터
SKILL_STRUCT[12] = { ["ResNo"] = 300301801, ["SkillNo"] = 300301801, ["Pos"] = Point2(263, 158+50) }	-- 메가 인페르노
SKILL_STRUCT[13] = { ["ResNo"] = 300301101, ["SkillNo"] = 300301101, ["Pos"] = Point2(7, 258+50) }	-- 임펙트 언서몬
SKILL_STRUCT[14] = { ["ResNo"] = 300301001, ["SkillNo"] = 300301001, ["Pos"] = Point2(71, 258+50) }	-- 리게인 리프레쉬
SKILL_STRUCT[15] = { ["ResNo"] = 300300701, ["SkillNo"] = 300300701, ["Pos"] = Point2(135, 258+50) }	-- 골렘 마스터리 Lv1 (골곤 패시브 롹펀치_스킬 해제)
SKILL_STRUCT[16] = { ["ResNo"] = 300301201, ["SkillNo"] = 300301201, ["Pos"] = Point2(7, 338+50) }	-- 임펙트 마스터리 (패시브)
SKILL_STRUCT[17] = { ["ResNo"] = 300300801, ["SkillNo"] = 300300801, ["Pos"] = Point2(135, 338+50) }	-- 블러드 쉐어 (골렘 공격 이펙트 습득 패시브_스킬 해제)
SKILL_STRUCT[18] = { ["ResNo"] = 300301901, ["SkillNo"] = 300301901, ["Pos"] = Point2(199, 338+50) }	-- 임펙트 프레셔
SKILL_STRUCT[19] = { ["ResNo"] = 300302201, ["SkillNo"] = 300302201, ["Pos"] = Point2(263, 338+50) }	-- 롤링 스톤 
SKILL_STRUCT[20] = { ["ResNo"] = 300304201, ["SkillNo"] = 300304201, ["Pos"] = Point2(263, 98+35) }	-- 용력 증폭
SKILL_STRUCT[21] = { ["ResNo"] = 300400101, ["SkillNo"] = 300400101, ["Pos"] = Point2(135, 8+50) }			--프로즌 체인
SKILL_STRUCT_CLASS[CT_NATURE_MASTER] = SKILL_STRUCT

--소환사 4차 직업
SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 300400301, ["SkillNo"] = 300400301, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_1) }			--템프테이션
SKILL_STRUCT[1] = { ["ResNo"] = 2000000101, ["SkillNo"] = 2000000001, ["Pos"] = Point2(SKILL4_ICON_XPOS_3, SKILL4_ICON_YPOS_1) }		--각성
SKILL_STRUCT[2] = { ["ResNo"] = 300304301, ["SkillNo"] = 300304301, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_1) }			--고통 분담
SKILL_STRUCT[3] = { ["ResNo"] = 300400501, ["SkillNo"] = 300400501,   ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_2) }		-- 엘리멘탈 스톰
SKILL_STRUCT[4] = { ["ResNo"] = 300402001, ["SkillNo"] = 300402001, ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_2) }			-- 드래곤 레이지
SKILL_STRUCT[5] = { ["ResNo"] = 300401501, ["SkillNo"] = 300401501,   ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_3) }		--  드라군 소환
SKILL_STRUCT[6] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, 		  ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_4) }		
SKILL_STRUCT[7] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, 		  ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_4) }		
SKILL_STRUCT[8] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_4) }		
SKILL_STRUCT[9] = { ["ResNo"] = 2000002001, ["SkillNo"] = 2000002001, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_4) }		-- 각성챠지
SKILL_STRUCT_CLASS[CT_GAIA_MASTER] = SKILL_STRUCT


-- 격투가 1차 직업
SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 400100101, ["SkillNo"] = 400100101, ["Pos"] = Point2(7+65, 8+50) }		--일도 양단
SKILL_STRUCT[1] = { ["ResNo"] = 400101201, ["SkillNo"] = 400101201, ["Pos"] = Point2(101+65, 8+50) }		--강철 주먹
SKILL_STRUCT[2] = { ["ResNo"] = 400100701, ["SkillNo"] = 400100701, ["Pos"] = Point2(195+65, 8+50) }		--스핀킥
SKILL_STRUCT[3] = { ["ResNo"] = 400100601, ["SkillNo"] = 400100601, ["Pos"] = Point2(7+65, 88+50) }		--파워피스트
SKILL_STRUCT[4] = { ["ResNo"] = 400101001, ["SkillNo"] = 400101001, ["Pos"] = Point2(195+65, 338+50) }		--백열각
SKILL_STRUCT[5] = { ["ResNo"] = 400100901, ["SkillNo"] = 400100901, ["Pos"] = Point2(195+65, 103+50) }		--로켓블로
SKILL_STRUCT[6] = { ["ResNo"] = 400100201, ["SkillNo"] = 400100201, ["Pos"] = Point2(7+65, 168+50) }		--토네이도 스핀
SKILL_STRUCT[7] = { ["ResNo"] = 400100301, ["SkillNo"] = 400100301, ["Pos"] = Point2(101+65, 248+50) }		--위빙
SKILL_STRUCT[8] = { ["ResNo"] = 400100801, ["SkillNo"] = 400100801, ["Pos"] = Point2(195+65, 223+50) }		--드래곤킥
SKILL_STRUCT[9] = { ["ResNo"] = 400300301, ["SkillNo"] = 400300301, ["Pos"] = Point2(7+65, 298+50) }		--벙커 버스터 Lv1
SKILL_STRUCT[10] = { ["ResNo"] = 400100501, ["SkillNo"] = 400100501, ["Pos"] = Point2(101+65, 338+50) }		--레피드 스텐스 (친교스킬)
SKILL_STRUCT_CLASS[CT_DOUBLE_FIGHTER] = SKILL_STRUCT

-- 격투가 2차 직업
SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 400200201, ["SkillNo"] = 400200201, ["Pos"] = Point2(7, 8+50) }		--무한 돌려차기 Lv1
SKILL_STRUCT[1] = { ["ResNo"] = 400200101, ["SkillNo"] = 400200101, ["Pos"] = Point2(71, 8+50) }		--퓨전 Lv1
SKILL_STRUCT[2] = { ["ResNo"] = 400301901, ["SkillNo"] = 400301901, ["Pos"] = Point2(327, 278+50) }		--헤비 블로우 Lv1
SKILL_STRUCT[3] = { ["ResNo"] = 400201301, ["SkillNo"] = 400201301, ["Pos"] = Point2(71,88+50) }		--레이지 Lv1
SKILL_STRUCT[4] = { ["ResNo"] = 400201801, ["SkillNo"] = 400201801, ["Pos"] = Point2(155, 88+50) }		--[FU]절대방어
SKILL_STRUCT[5] = { ["ResNo"] = 400200301, ["SkillNo"] = 400200301, ["Pos"] = Point2(7, 128+50) }		--하이퍼 니킥 Lv1
SKILL_STRUCT[6] = { ["ResNo"] = 400200701, ["SkillNo"] = 400200701, ["Pos"] = Point2(71, 178+50) }		--파워 위빙 Lv1
SKILL_STRUCT[7] = { ["ResNo"] = 400201901, ["SkillNo"] = 400201901, ["Pos"] = Point2(155, 178+50) }		--[FU]임펙트 블로우 Lv1
SKILL_STRUCT[8] = { ["ResNo"] = 400200401, ["SkillNo"] = 400200401, ["Pos"] = Point2(7, 258+50) }		--드래곤 어퍼 Lv1
SKILL_STRUCT[9] = { ["ResNo"] = 400201401, ["SkillNo"] = 400201401, ["Pos"] = Point2(71, 258+50) }		--닷지 마스터리 Lv1
SKILL_STRUCT[10] = { ["ResNo"] = 400202001, ["SkillNo"] = 400202001, ["Pos"] = Point2(155, 258+50) }		--[FU]투기 발산 Lv1
SKILL_STRUCT[11] = { ["ResNo"] = 400200501, ["SkillNo"] = 400200501, ["Pos"] = Point2(7, 338+50) }		--지옥의 링 소환 Lv1
SKILL_STRUCT[12] = { ["ResNo"] = 400201701, ["SkillNo"] = 400201701, ["Pos"] = Point2(71, 338+50) }		--호흡조절 Lv1
SKILL_STRUCT[13] = { ["ResNo"] = 400300701, ["SkillNo"] = 400300701, ["Pos"] = Point2(278, 8+50) }		--[SC]스핀 붐 Lv1
SKILL_STRUCT[14] = { ["ResNo"] = 400301001, ["SkillNo"] = 400301001, ["Pos"] = Point2(229, 248+50) }		--[SC]슬라이딩 난타 Lv1
SKILL_STRUCT[15] = { ["ResNo"] = 400300801, ["SkillNo"] = 400300801, ["Pos"] = Point2(229, 158+50) }		--[SC]스피릿 스피어 Lv1
SKILL_STRUCT[16] = { ["ResNo"] = 400300901, ["SkillNo"] = 400300901, ["Pos"] = Point2(327, 98+50) }		--[SC]롤링 그래블 Lv1
SKILL_STRUCT[17] = { ["ResNo"] = 400301101, ["SkillNo"] = 400301101, ["Pos"] = Point2(327, 188+50) }		--[SC]스매쉬 킥 Lv1

SKILL_STRUCT_CLASS[CT_TWINS] = SKILL_STRUCT

-- 격투가 3차 직업
SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 400100401, ["SkillNo"] = 400100401, ["Pos"] = Point2(7, 8+50) }		--스크류 드랍킥
SKILL_STRUCT[1] = { ["ResNo"] = 400301601, ["SkillNo"] = 400301601, ["Pos"] = Point2(71, 8+50) }		--철권 단련 Lv1
SKILL_STRUCT[2] = { ["ResNo"] = 400301201, ["SkillNo"] = 400301201, ["Pos"] = Point2(160, 43+50) }		--[FU]퓨전 부스트 Lv1
SKILL_STRUCT[3] = { ["ResNo"] = 400300401, ["SkillNo"] = 400300401, ["Pos"] = Point2(7, 108+50) }		--사자후
SKILL_STRUCT[4] = { ["ResNo"] = 400301701, ["SkillNo"] = 400301701, ["Pos"] = Point2(71, 108+50) }		--피스트 하모니 Lv1
SKILL_STRUCT[5] = { ["ResNo"] = 400301301, ["SkillNo"] = 400301301, ["Pos"] = Point2(160, 158+50) }		--[FU]피니쉬 무브 Lv1
SKILL_STRUCT[6] = { ["ResNo"] = 400300501, ["SkillNo"] = 400300501, ["Pos"] = Point2(7, 223+50) }		--토네이도 어퍼 Lv1
SKILL_STRUCT[7] = { ["ResNo"] = 400301801, ["SkillNo"] = 400301801, ["Pos"] = Point2(71, 187+50) }		--하모니 마스터리 Lv1
SKILL_STRUCT[8] = { ["ResNo"] = 400301401, ["SkillNo"] = 400301401, ["Pos"] = Point2(160, 248+50) }		--[FU]피니쉬 임펙트 Lv1
SKILL_STRUCT[9] = { ["ResNo"] = 400300101, ["SkillNo"] = 400300101, ["Pos"] = Point2(7, 338+50) }		--운명의 수레바퀴 시전 Lv1
SKILL_STRUCT[10] = { ["ResNo"] = 400300601, ["SkillNo"] = 400300601, ["Pos"] = Point2(71, 278+50) }		--전광 석화 Lv1
SKILL_STRUCT[11] = { ["ResNo"] = 400301501, ["SkillNo"] = 400301501, ["Pos"] = Point2(160, 338+50) }		--[FU]고스트 파이터 Lv1
SKILL_STRUCT[12] = { ["ResNo"] = 400200801, ["SkillNo"] = 400200801, ["Pos"] = Point2(263, 8+50) }		--[SC]스템퍼 Lv1
SKILL_STRUCT[13] = { ["ResNo"] = 400201501, ["SkillNo"] = 400201501, ["Pos"] = Point2(327,8+50) }		--트윈 이펙트 Lv1
SKILL_STRUCT[14] = { ["ResNo"] = 400200901, ["SkillNo"] = 400200901, ["Pos"] = Point2(219, 88+50) }		--[SC]스핀 블레이드 Lv1
SKILL_STRUCT[15] = { ["ResNo"] = 400201101, ["SkillNo"] = 400201101, ["Pos"] = Point2(219, 218+50) }		--[SC]백열진각 Lv1
SKILL_STRUCT[16] = { ["ResNo"] = 400201001, ["SkillNo"] = 400201001, ["Pos"] = Point2(327, 218+50) }		--[SC]스위시 킥 Lv1
SKILL_STRUCT[17] = { ["ResNo"] = 400201201, ["SkillNo"] = 400201201, ["Pos"] = Point2(263,298+50) }		--[SC]라이징 드롭킥 Lv1
SKILL_STRUCT_CLASS[CT_MIRAGE] = SKILL_STRUCT

-- 격투가 4차 직업
SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] =  400400101, ["SkillNo"] = 400400101, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_1) }			-- 트레이닝 타임
SKILL_STRUCT[1] = { ["ResNo"] = 2000000101, ["SkillNo"] = 2000000001, ["Pos"] = Point2(SKILL4_ICON_XPOS_3, SKILL4_ICON_YPOS_1) }		-- 각성
--SKILL_STRUCT[2] = { ["ResNo"] =  400400201, ["SkillNo"] =  400400201, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_1) }		-- 일심 동체
SKILL_STRUCT[2] = { ["ResNo"] =  2010000001, ["SkillNo"] =  -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_1) }
SKILL_STRUCT[3] = { ["ResNo"] =  400202101, ["SkillNo"] =  400202101, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_2) }		-- 무한 난투
SKILL_STRUCT[4] = { ["ResNo"] =  400401001, ["SkillNo"] =  400401001, ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_2) }		-- 크로스 피스트
SKILL_STRUCT[5] = { ["ResNo"] =  400400501, ["SkillNo"] =  400400501, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_3) }		-- 연계강타
SKILL_STRUCT[6] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_2, SKILL4_ICON_YPOS_4) }		
SKILL_STRUCT[7] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_4, SKILL4_ICON_YPOS_4) }		
SKILL_STRUCT[8] = { ["ResNo"] = 2010000001, ["SkillNo"] = -1, ["Pos"] = Point2(SKILL4_ICON_XPOS_5, SKILL4_ICON_YPOS_4) }		
SKILL_STRUCT[9] = { ["ResNo"] = 2000002001, ["SkillNo"] = 2000002001, ["Pos"] = Point2(SKILL4_ICON_XPOS_1, SKILL4_ICON_YPOS_4) }		-- 각성챠지
SKILL_STRUCT_CLASS[CT_DRAGON_FIGHTER] = SKILL_STRUCT

------------------------------------------------------------------------------------------------------------------------------------------------------
SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 95000201, ["SkillNo"] = 95000201, ["Pos"] = Point2(56,45+29) }		-- 형 돈다
SKILL_STRUCT[1] = { ["ResNo"] = 95000001, ["SkillNo"] = 95000001, ["Pos"] = Point2(214,45+29) }		-- 형 쏜다
SKILL_STRUCT_CLASS[CT_SPECIAL_SKILL] = SKILL_STRUCT      

SKILL_STRUCT = {}
SKILL_STRUCT[0] = { ["ResNo"] = 100006301, ["SkillNo"] = 100006301, ["Pos"] = Point2(12,282) }	-- 손잡기
SKILL_STRUCT[1] = { ["ResNo"] = 100006401, ["SkillNo"] = 100006401, ["Pos"] = Point2(56,282) }	-- 껴안기
SKILL_STRUCT[2] = { ["ResNo"] = 100006501, ["SkillNo"] = 100006501, ["Pos"] = Point2(100,282) }	-- 키스
SKILL_STRUCT_CLASS[CT_COUPLE_EMOTION] = SKILL_STRUCT

ARROW_STRUCT = {}

function UI_CalcSkillPoint(wnd)
	if true == wnd:IsNil() then return end
	local	kSkillTree	=	GetSkillTree();
	local wndBg = wnd:GetControl("SFRM_SKILL")
	if wndBg:IsNil() == false then
		wndBg:SetStaticText(GetTextW(219):GetStr().. " " .. GetTextW(1304):GetStr() ..kSkillTree:GetRemainSkillPoint())
	end	
end

function UI_SKILLWND_REFRESH(wnd)
    ODS("UI_SKILLWND_REFRESH Start\n")

	if true == wnd:IsNil() then return end
	-- Check Basic Combo
	local MyActor = GetMyActor()
	if nil ~= MyActor and false==MyActor:IsNil() then
		local kComboTab = wnd:GetControl("CBTN_COMBO_TAB")
		if nil~=kComboTab and false==kComboTab:IsNil() then
			local	kSkillTree = GetSkillTree();
			local iWeaponType = MyActor:GetEquippedWeaponType();
			if 0 == kSkillTree:GetMaxComboCount(iWeaponType) then
				kComboTab:Visible(false)
				kComboTab:CheckState(false);
				local kSkillTab = wnd:GetControl("CBTN_SKILL_TAB")
				if nil~=kSkillTab and false==kSkillTab:IsNil() then
					kSkillTab:CheckState(true);
				end
				local kComboSkillTab = wnd:GetControl("CBTN_COMBO_SKILL_TAB")
				if nil~=kComboSkillTab and false==kComboSkillTab:IsNil() then
					kComboSkillTab:CheckState(false);
				end
			else
				kComboTab:Visible(true)
			end
		end
	end
	
	local	MyPilot = GetMyPilot()
	if MyPilot:IsNil() == true then
		return
	end

	UI_CalcSkillPoint(wnd)
	UI_StrategySkillTab(wnd)

	local	iClass	=	MyPilot:GetAbil(AT_CLASS);
	local iClassTableCount = table.getn(SKILL_BG_NAME_LIST)

	UI_InitClassTab(wnd, iClass, true)
	
	local Btn = wnd:GetControl("CBTN_CLASS0")
	local iClassNum = Btn:GetCustomDataAsInt()	
	local WndFullName = wnd:GetControl("SFRM_CLASS_FULLNAME")
	if( nil ~= WndFullName ) then 
		if( false == WndFullName:IsNil() ) then 
			WndFullName:SetStaticTextW(GetTT(30000+iClassNum))
		end
	end
				
--[[	local List = wnd:GetControl("LST_LIST")
	if false == List:IsNil() then
		local Item = List:ListFirstItem()
		if false == Item:IsNil() then
			UI_SkillArrowRefresh(Item:GetWnd())
		end
	end]]--
	UI_SKILLWND_SetButtonVisible(wnd)
    ODS("UI_SKILLWND_REFRESH End\n")
end

function UI_RefreshSkillIcon(wnd, iSkillNo, iResNo)
	--ODS("UI_RefreshSkillIcon Start " .. iSkillNo .. "  " .. iResNo .. "\n", false, 1509)
	if wnd:IsNil() == true or iSkillNo == 0 or iResNo == 0 then
		return
	end

	local icon = wnd:GetControl("ICN_SKILL")
	if icon:IsNil() == true then 
		return 
	end

	wnd:SetCustomDataAsInt(iSkillNo)

	icon:RegistScript("ON_DISPLAY", "UISelf:DrawIcon()")
	icon:RegistScript("ON_R_BTN_DOWN", "UISelf:IconDoAction()")
	icon:RegistScript("ON_MOUSE_OVER", "UISelf:CallIconToolTip()")
	icon:RegistScript("ON_MOUSE_OUT", "CloseToolTip()")
	icon:RegistScript("ON_L_BTN_DOWN", "UISelf:IconToCursor()")

	icon:SetIconKeyAndResNo(iSkillNo, iResNo)
	local	kSkillTree = GetSkillTree();
	local   bLearned = kSkillTree:IsLearnedSkill(iSkillNo);
	local 	bCheck = kSkillTree:CheckAllNeedSkill(iSkillNo);

	local bCanLvUp = kSkillTree:CanLevelUp(iSkillNo)
	local Button = wnd:GetControl("BTN_UP")
	if Button:IsNil() == false then
		Button:Visible(bCanLvUp and bCheck);
		Button:RegistScript("ON_L_BTN_UP", "UI_Skill_ON_LEVELUP_CLICK(UISelf)")
	end

	local bCanLvDown = kSkillTree:CanLevelDown(iSkillNo)
	local DButton = wnd:GetControl("BTN_DOWN")
	if DButton:IsNil() == false then
		DButton:Visible(bCanLvDown and bCheck);
		DButton:RegistScript("ON_L_BTN_UP", "UI_Skill_ON_LEVELDOWN_CLICK(UISelf)")
	end
	
	-- 스킬 봉인 시스템 : 더이상 사용하지 않는다 (롤백)
	local COpButton = wnd:GetControl("CBTN_CMD_OPTION");
	if COpButton:IsNil() == false then
		-- local bHaveCommand = kSkillTree:IsCommandSkill(iSkillNo)
		COpButton:Visible(false);
		-- COpButton:Visible(bLearned and bHaveCommand);
		-- if true == bHaveCommand then
			-- COpButton:CheckState(not GetSkillOption(kSkillTree:GetKeySkillNo(iSkillNo)))
		-- end
		-- COpButton:RegistScript("ON_L_BTN_UP", "UI_Skill_ON_COMMAND_SEAL_CLICK(UISelf)");
	end

	local iSkillLv = kSkillTree:GetSkillLevel(iSkillNo) -- 스킬의 레벨
	local iOverLv = kSkillTree:GetOverSkillLevel(iSkillNo) -- 스킬의 오버된 레벨

	if 0 < iSkillLv and 0 < iOverLv then
		iSkillLv = iSkillLv + iOverLv;
		wnd:SetFontColorRGBA(75,233,255,255);
	else
		wnd:SetFontColor(0xFFFFF200)
	end

	if(0 < kSkillTree:GetMaxSkillLevel(iSkillNo)) then
		icon:SetStaticText(tostring(kSkillTree:GetMaxSkillLevel(iSkillNo)))
		wnd:SetStaticText(tostring(iSkillLv))
	else
		icon:SetStaticText("")
		wnd:SetStaticText("")
	end

    icon:SetGrayScale(  bLearned == false and iSkillLv <= 0 or bCheck == false)

	--ODS("UI_RefreshSkillIcon End\n", false, 1509)
end

function UI_RefreshSkillIconByBuild(wnd, iClassNo)
	ODS("UI_RefreshSkillIconByBuild Start "..iClassNo.."\n", false, 912)
	if true==wnd:IsNil() then return end
	if (iClassNo ~= CT_SPECIAL_SKILL) and (iClassNo ~= CT_COUPLE_EMOTION) then
		--if 50 >= iClassNo then			
			-- if table.getn(SKILL_STRUCT_CLASS) <	iClassNo then return end	-- 기조 클래스 체크
		-- else		
			if table.getn( SKILL_STRUCT_CLASS[iClassNo] ) <= 0 then return end
		--end
	end

	local struct = SKILL_STRUCT_CLASS[iClassNo]
	local iIdx = wnd:GetBuildIndex()
	local iSkillNo = struct[iIdx]["SkillNo"]
	local iResNo = struct[iIdx]["ResNo"]
	ODS("classno".. iClassNo .. "  resno:" .. iResNo .. "\n" , false, 912)
	
	if (iClassNo == CT_SPECIAL_SKILL) then
	-- 스페셜 스킬 아이콘은
		local kSkillTree = GetSkillTree()
		local bLearned = kSkillTree:IsLearnedSkill(iSkillNo)
		-- 배우기 전까지 표시 하지 않는다
		wnd:Visible(bLearned)
		if(true == bLearned) then 
			UI_RefreshSkillIcon(wnd, iSkillNo, iResNo)
			wnd:SetLocation(struct[iIdx]["Pos"])
		end
	else
	-- 일반 스킬 아이콘 위치 설정
		ODS("일반 스킬 아이콘 위치 설정\n", false, 912)
		UI_RefreshSkillIcon(wnd, iSkillNo, iResNo)
		wnd:SetLocation(struct[iIdx]["Pos"])
		if iClassNo == CT_COUPLE_EMOTION then
			local icon = wnd:GetControl("ICN_SKILL")
			if false==icon:IsNil() and false==icon:IsGrayScale() then
				local kPilot = GetMyPilot()
				if false==kPilot:IsNil() then
					icon:SetGrayScale(kPilot:GetCoupleGuid():IsNil())
				end
			end
		end
	end	
end

function UI_Skill_ON_LEVELUP_CLICK(wnd)	--	스킬 레벨 업 버튼 클릭
    --ODS("UI_Skill_ON_LEVELUP_CLICK Start\n", false, 1509);

	if wnd:IsNil() == true then
		MessageBox("Nil","");
		return;
	end

	local	IconBGWnd = wnd:GetParent();
	if IconBGWnd:IsNil() == true then
		return
	end
	local icon = IconBGWnd:GetControl("ICN_SKILL")
	if icon:IsNil() == true then
		return
	end

	local	iKeySkillNo = IconBGWnd:GetCustomDataAsInt();
	--ODS("UI_Skill_ON_LEVELUP_CLICK " .. iKeySkillNo .. "\n", false, 1509)
		
	local	kSkillTree = GetSkillTree();
	kSkillTree:LevelUpTemporary(iKeySkillNo);

	UI_SKILLWND_Refresh_All_New(iKeySkillNo);
    --ODS("UI_Skill_ON_LEVELUP_CLICK End\n", false, 1509);	
end

function UI_Skill_ON_LEVELDOWN_CLICK(wnd)	--	스킬 레벨 업 버튼 클릭
    ODS("UI_Skill_ON_LEVELDOWN_CLICK Start\n");

	if wnd:IsNil() == true then
		MessageBox("Nil","");
		return;
	end

	local	IconBGWnd = wnd:GetParent();
	if IconBGWnd:IsNil() == true then
		return
	end
	local icon = IconBGWnd:GetControl("ICN_SKILL")
	if icon:IsNil() == true then
		return
	end

	local	iKeySkillNo = IconBGWnd:GetCustomDataAsInt();
	ODS("UI_Skill_ON_LEVELUP_CLICK " .. iKeySkillNo .. "\n")
		
	local	kSkillTree = GetSkillTree();
	kSkillTree:LevelDownTemporary(iKeySkillNo);

	UI_SKILLWND_Refresh_All_New(iKeySkillNo);
    ODS("UI_Skill_ON_LEVELDOWN_CLICK End\n");	
end

function UI_Skill_ON_COMMAND_SEAL_CLICK(wnd)
    ODS("UI_Skill_ON_COMMAND_SEAL_CLICK Start\n");

	if wnd:IsNil() == true then
		MessageBox("Nil","");
		return;
	end

	local	IconBGWnd = wnd:GetParent();
	if IconBGWnd:IsNil() == true then
		return
	end
	local icon = IconBGWnd:GetControl("ICN_SKILL")
	if icon:IsNil() == true then
		return
	end

	local	iKeySkillNo = IconBGWnd:GetCustomDataAsInt();
	ODS("UI_Skill_ON_LEVELUP_CLICK " .. iKeySkillNo .. "\n")
		
	local	kSkillTree = GetSkillTree();
	
	SetSkillOption(iKeySkillNo, not wnd:GetCheckState())
    ODS("UI_Skill_ON_COMMAND_SEAL_CLICK End\n");	
end

function UI_SKILLWND_Refresh_All_New(iSkillNo)
	ODS("UI_SKILLWND_Refresh_All_New Start \n")
	if nil~=iSkillNo then
		--ODS("iSkillNo " .. iSkillNo .. "\n", false, 1509)
	else
		iSkillNo = 0
		--ODS("iSkillNo nil\n", false, 1509)
	end
	local SkillWnd = GetUIWnd("SFRM_SKILL_WINDOW")
	if SkillWnd:IsNil() == true then
		return
	end

	local list = SkillWnd:GetControl("LST_LIST")
	if list:IsNil() == true then
		return
	end

	local iEleNum = list:GetListItemCount()
	local kItem = list:ListFirstItem()
	local	kSkillTree	=	GetSkillTree();

	if 0>=iEleNum then return end

	local kEle = kItem:GetWnd()
	if kEle:IsNil() == false then
		local iCount = kEle:GetContCount()
		for i=0,iCount-1 do 
			local kChild = kEle:GetContAt(i)
			if kChild:IsNil() == false then
				local iKey = kSkillTree:GetKeySkillNo(kChild:GetCustomDataAsInt())
				local iResNo = kSkillTree:GetKeySkillResNoBySkillNo(iKey)
				UI_RefreshSkillIcon(kChild, iKey, iResNo)
			end
		end
		UI_SkillArrowRefresh(kEle)
	end

	UI_CalcSkillPoint(SkillWnd)
	UI_StrategySkillTab(SkillWnd)
	UI_SKILLWND_SetButtonVisible(SkillWnd, iSkillNo)
	ODS("UI_SKILLWND_Refresh_All_New End \n")
end

function	UI_SKILLWND_ON_NEW_SKILL_LEARNED_NEW(iNewSkillNo)
    ODS("UI_SKILLWND_ON_NEW_SKILL_LEARNED : " .. iNewSkillNo .. "\n");
	UI_SKILLWND_Refresh_All_New()
end

function UI_ChangeClassMark(wnd)
	if wnd:IsNil() == true then
		return
	end

	local	MyPilot = GetMyPilot()
	if MyPilot:IsNil() == true then
		return
	end
	local	iClass	=	MyPilot:GetAbil(AT_CLASS)
	if(50 < iClass) then -- "../Data/6_ui/skill/skClassMark.tga" 의 UVIndex에 iClass가 매칭됐었다. 하지만 용족이 생기면서 29번 UVIndex가 용족 시작 클래스가 되었음
		iClass = iClass - 22
	end
	wnd:SetUVIndex(iClass)
end

function UI_ChangeSkillBG(wnd)
	if wnd:IsNil() == true then
		return
	end

	local	MyPilot = GetMyPilot()
	if MyPilot:IsNil() == true then
		return
	end
	
	local	iClass	=	MyPilot:GetBaseClassID(AT_BASE_CLASS)
	--ODS("GetBaseClassID:"..iClass.."\n",false, 912)
	if(10 > iClass) then 
		wnd:ChangeImage("../Data/6_ui/skill/skBg0" .. iClass .. ".tga")
	else
		wnd:ChangeImage("../Data/6_ui/skill/skBg" .. iClass .. ".tga")
	end
end

function UI_SkillCallDisplayInit(wnd)
	if wnd:IsNil() == true then return end
	local MyPilot = GetMyPilot()
	if MyPilot:IsNil() == true then return end
	
	local iMyClass = MyPilot:GetAbil(AT_CLASS)
	if iMyClass==g_iSkill_MyClass then
		local Btn = wnd:GetControl("CBTN_CLASS"..(g_iSkill_Window_TabNo+0))
		if Btn:IsNil() == false then
			UI_Click_Class_Tab(Btn)
			return
		end
	end
	
	for i=0,3 do
		local Btn = wnd:GetControl("CBTN_CLASS"..i)
		if Btn:IsNil() == false then
			local iClassNum = Btn:GetCustomDataAsInt()
			if iMyClass == iClassNum then
				UI_Click_Class_Tab(Btn)		
				g_iSkill_MyClass = iMyClass
				break
			end
		end
	end
end

function UI_Click_Class_Tab(wnd)
	g_iSkill_Window_TabNo = 0
	if wnd:IsNil() == true then return end
	wnd:LockClick(true)
	if wnd:GetCheckState() == true then --선택된걸 또 선택
		return 
	else
		wnd:CheckState(true)
		local kLvWnd = wnd:GetControl("FRM_LV") -- LV 글자색 노랗게
		if(false == kLvWnd:IsNil()) then
			kLvWnd:SetFontColor(0xFFFFF568)
		end
	end 
	local Parent = wnd:GetParent()
	if Parent:IsNil() == true then return end
	
	local iLast = tonumber(string.sub(wnd:GetID():GetStr(), -1))
	-- 우측 탭 버튼 초기화
		-- 스페셜 스킬 (대만의 경우에만)
		local kSpcWnd = Parent:GetControl("CBTN_SPECIAL")
		if nil~=kSpcWnd and false==kSpcWnd:IsNil() then
			if LOCALE.NC_TAIWAN == GetLocale()
			or LOCALE.NC_DEVELOP == GetLocale() then
				kSpcWnd:CheckState(false)
				kSpcWnd:LockClick(true)
			else
				kSpcWnd:Visible(false)
			end
		end	
	-- 상단 탭
	for i=0,3 do
		local Btn = Parent:GetControl("CBTN_CLASS"..i)
		if Btn:IsNil() == false then
			ODS(Btn:GetID():GetStr() .. iLast .."\n")
			if i~=iLast then
				Btn:CheckState(false)
				Btn:LockClick(true)
				local kLvWnd = Btn:GetControl("FRM_LV")	-- LV 글자색 다시 하얗게 
				if(false == kLvWnd:IsNil()) then	
					kLvWnd:SetFontColor(0xFFB7B7B7)
				end
			else
				local iClassNum = Btn:GetCustomDataAsInt()
				CheckSkillWnd(Parent, iClassNum)
				
				local WndFullName = Parent:GetControl("SFRM_CLASS_FULLNAME")
				if( nil ~= WndFullName ) then 
					if( false == WndFullName:IsNil() ) then 
						WndFullName:SetStaticTextW(GetTT(30000+iClassNum))
					end
				end
			end
		end
	end
	
	g_iSkill_Window_TabNo = iLast
end

function UI_Click_Special_Tab(wnd)
	if wnd:IsNil() == true then return end
	wnd:LockClick(true)
	if wnd:GetCheckState() == true then --선택된걸 또 선택
		return 
	else
		wnd:CheckState(true)
	end 
	local Parent = wnd:GetParent()
	if Parent:IsNil() == true then return end
	
	-- 우측 탭
	
	--상단 탭 초기화
	for i=0,3 do
		local Btn = Parent:GetControl("CBTN_CLASS"..i)
		if Btn:IsNil() == false then
			Btn:CheckState(false)
			Btn:LockClick(true)			
		end
	end
	
	local iNum = wnd:GetCustomDataAsInt()
	CheckSkillWnd(Parent, iNum)
	
	--풀네임 넣어야함
	local WndFullName = Parent:GetControl("SFRM_CLASS_FULLNAME")
	if( nil ~= WndFullName ) then 
		if( false == WndFullName:IsNil() ) then 
			WndFullName:SetStaticTextW(GetTT(30000+iNum))
		end
	end
end
--------- Basic Skill Set ---------
function SetLearnBasicSkill(iSetNo, bLearned)
	g_bLearnedBasicSkillSet[iSetNo] = bLearned
end

function GetLearnBasicSkill(iSetNo)
	return g_bLearnedBasicSkillSet[iSetNo]
end

function SetReadyBasicSkill(iSetNo, bReady)
	g_bReadyBasicSkillSet[iSetNo] = bReady
end

function GetReadyBasicSkill(iSetNo)
	return g_bReadyBasicSkillSet[iSetNo]
end

function InputBasicSkillSet(iClassNo, iSkillSet)
	if nil == g_BasicSkillSet[iClassNo] then
		ODS("Not Found g_BasicSkillSet[iClassNo] \n")
		ODS("[iClassNo] = "..iClassNo.."\n")
		return false
	end
	if nil == g_BasicSkillSet[iClassNo][iSkillSet] then
		ODS("Not Found g_BasicSkillSet[iClassNo][iSkillSet]\n")
		ODS("[iClassNo] = "..iClassNo.."\n")
		ODS("[iSkillSet] = "..iSkillSet.."\n")
		return false
	end
	local iSkillSetIndex = 1
	while nil ~= g_BasicSkillSet[iClassNo][iSkillSet][iSkillSetIndex] do

		local iSkillNo = g_BasicSkillSet[iClassNo][iSkillSet][iSkillSetIndex]["SKILL"]	--스킬 번호
		local iCheckSkillNo = g_BasicSkillSet[iClassNo][iSkillSet][iSkillSetIndex]["CHECK_SKILL"]	--배웠는지 체크할 스킬 번호
		local iDelay = g_BasicSkillSet[iClassNo][iSkillSet][iSkillSetIndex]["DELAY"]	--1/10초 단위
		local iCount = g_BasicSkillSet[iClassNo][iSkillSet][iSkillSetIndex]["COUNT"]	--스킬 반복 횟수
		
		SetBasicSkillSet(iSkillSet+30, iSkillNo, iCheckSkillNo, iDelay, iCount)
		
		iSkillSetIndex = iSkillSetIndex + 1
	end
	return true
end

function InitBasicSkillSet()
	local	MyPilot = GetMyPilot()
	if true == MyPilot:IsNil() == true then 
		ODS("Falid SetBasicSkillSet - Not Fount Player\n", false, 912)
		return false
	end
	--Init
	ClearBasicSkillSet()
	SetLearnBasicSkill(31, false)
	SetLearnBasicSkill(32, false)
	SetReadyBasicSkill(31, false)
	SetReadyBasicSkill(32, false)
	
	local iClass = MyPilot:GetAbil(AT_BASE_CLASS)
	if(iClass == CT_FIGHTER) or 
	  (iClass == CT_MAGICIAN) or
	  (iClass == CT_ARCHER) or
	  (iClass == CT_THIEF) or
	  (iClass == CT_SHAMAN) or
	  (iClass == CT_DOUBLE_FIGHTER) then
		if false == InputBasicSkillSet(iClass, 1) then
			ODS("Falid SetBasicSkillSet1 - InputBasicSkillSet"..iClass.."\n")
			return false
		end
		if false == InputBasicSkillSet(iClass, 2) then
			ODS("Falid SetBasicSkillSet2 -  - InputBasicSkillSet"..iClass.."\n")
			return false
		end	
		
		ApplyBasicSkillSetToSkillSet()
		
		local bLearned = CheckUseSkillSetNo(31, true)
		SetLearnBasicSkill(31, bLearned)
		bLearned = CheckUseSkillSetNo(32, true)
		SetLearnBasicSkill(32, bLearned)
		
		local bReady = CheckUseSkillSetNo(31, false)
		SetReadyBasicSkill(31, bReady)
		local bReady = CheckUseSkillSetNo(32, false)
		SetReadyBasicSkill(32, bReady)
		
	else
		ODS("Not Fount Player Class\n")
		return false
	end	
	return true
end
------------------------------------
function UI_GetClassOrder()
	local	MyPilot = GetMyPilot()
	if MyPilot:IsNil() == true then return 0 end
	local	iClass	=	MyPilot:GetAbil(AT_CLASS);
	local iClassTableCount = table.getn(SKILL_BG_NAME_LIST)
	local iOrder = 0
	
	-- 전직하지 않았는가
	if(iClass == CT_FIGHTER
	or iClass == CT_MAGICIAN
	or iClass == CT_ARCHER
	or iClass == CT_THIEF
	--or iClass == CT_SHAMAN
	--or iClass == CT_DOUBLE_FIGHTER
	) then
		iOrder = 2
	else
	-- 전직 했다면, 용족은 클래스 라인이 한개이므로 전직 여부를 판단할 필요가 없음
		iOrder = 3
		--[[
		if(CT_KNIGHT  or CT_PALADIN or CT_DRAOON) then 
		elseif(CT_WARRIOR or CT_GLADIATOR or CT_DESTROYER) then
		
		elseif(CT_MAGE	or CT_WIZARD or CT_ARCHMAGE) then
		elseif(CT_BATTLEMAGE or CT_WARMAGE or CT_WARLORD) then
		
		elseif(CT_HUNTER or CT_TRAPPER or CT_SENTINEL) then
		elseif(CT_RANGER or CT_SNIPER or CT_LAUNCHER) then
		
		elseif(CT_CLOWN or CT_DANCER or CT_FREEJACKER) then
		elseif(CT_ASSASSIN or CT_NINJA or CT_SHADOW) then
		end
		]]
	end
	--[[
	for i=1,iClassTableCount do
		if MyPilot:IsCorrectClass(i, false) then
			--ODS("IsCorrectClass : " .. i .. "\n")
			iOrder = iOrder + 1
		end
	end
	]]
	return iOrder
end

function UI_InitClassTab(wnd, iClassNo, bInit)
	ODS("UI_InitClassTab Start\n", false, 912)
	if wnd:IsNil() == true then return 0 end
	local iOrder = UI_GetClassOrder()
	ODS("iOrder:"..iOrder.."\n", false, 912)
	if 0==iOrder then return 0 end

	local iLast = 0
	if false == bInit then
		iLast = math.max(iOrder, 1) - 1
	end

	ODS("iLast : " .. iLast .. "\n", false, 912)
	
	local NoTb = UI_GetClassNoTable(iClassNo)
	local iStringTbCount = table.getn(NoTb)

	for i=0,table.getn(NoTb) do
		ODS("TB " .. NoTb[i] .. "\n")
	end
	local kTopLine = wnd:GetControl("IMG_TOP_LINE")
	
	for i=0,3 do
		local Btn = wnd:GetControl("CBTN_CLASS"..i)
		if Btn:IsNil() == false then
			Btn:SetStaticText("")
			Btn:ClearCustomData()
			Btn:CheckState(i==iLast)
			Btn:LockClick(true)
			Btn:SetButtonTextColor(2,0xFFF7EA96)
			Btn:SetEnable(true)
			Btn:SetEnable(i <= iStringTbCount)
			---------------------------------------------------------------------------
			local kLvWnd = Btn:GetControl("FRM_LV") -- LV 글자
			if(false == kLvWnd:IsNil()) then
				kLvWnd:SetStaticText("")
			end
			---------------------------------------------------------------------------
			if(i == 3 and iOrder == 2) then 	--상단에 한줄 나오게 하기
				Btn:Visible(false)
				if(false == kTopLine:IsNil()) then 
					kTopLine:Visible(true)
				end
			else
				if(false == kTopLine:IsNil()) then 
					kTopLine:Visible(false)
				end
				Btn:Visible(true) 
			end
			---------------------------------------------------------------------------------
			if i <= iStringTbCount then
				local CutStr = WideString("..."); 
				SetCutedTextLimitLength(Btn,GetTT(30000+NoTb[i]),CutStr,Btn:GetWidth())
				Btn:SetCustomDataAsInt(NoTb[i])
								
				if(false == kLvWnd:IsNil()) then
					local iLvValue = 1
					if(2 == iOrder) then	-- 1차 직업이라면
						if(i == 1 or i == 2) then iLvValue = 20 end						
					else					-- 1차 직업이 아니라면
						if(i == 1) 	   then	iLvValue = 20	-- 2차 직업 레벨
						elseif(i == 2) then	iLvValue = 40	-- 3차 직업 레벨
						elseif(i == 3) then	iLvValue = 60	-- 4차 직업 레벨
							
						end						
					end
					kLvWnd:SetStaticText(tostring(iLvValue)..GetTT(450254):GetStr())
				end
			end
			if i == iLast then	-- 현재 보여지는 놈은 업데이트
				CheckSkillWnd(Btn:GetParent(), NoTb[i])
			end
		end
	end

	-- 대만 로컬에서만 유저가 스페셜 스킬을 가지고 있때만 스페셜 스킬 텝을 보여줌
	local kSpcWnd = wnd:GetControl("CBTN_SPECIAL")
	if nil~=kSpcWnd and false==kSpcWnd:IsNil() then
		if LOCALE.NC_TAIWAN == GetLocale()
		or LOCALE.NC_DEVELOP == GetLocale() then
			if(false == IsMyPlayerLearnedAnySpecialSkill()) then
				kSpcWnd:Visible(false)
			else
				kSpcWnd:Visible(true)
				kSpcWnd:CheckState(false)
				kSpcWnd:LockClick(true)
				kSpcWnd:SetCustomDataAsInt(CT_SPECIAL_SKILL)
			end
		else
			kSpcWnd:Visible(false)
		end
	end
	ODS("UI_InitClassTab End\n")
end

function UI_GetClassNoTable(iClassNo)
	local NoTb = {}
	if iClassNo <=0 then return NoTb end

	local	MyPilot = GetMyPilot()
	if MyPilot:IsNil() == true then return NoTb end
--[[
	local iClassTableCount = table.getn(SKILL_BG_NAME_LIST)
	local iIdx = 0
	for i=1,iClassTableCount do
		if MyPilot:IsCorrectClass(i, false) then
			ODS("IsCorrectClass : " .. i .. "\n")
			NoTb[iIdx] = i--GetTT(30000+i):GetStr()
			iIdx = iIdx + 1
		end
	end
]]
	local iClass = MyPilot:GetAbil(AT_CLASS)	
	if(iClass == CT_FIGHTER) then
		NoTb[0] = CT_FIGHTER
		NoTb[1] = CT_KNIGHT
		NoTb[2] = CT_WARRIOR
	elseif(iClass == CT_MAGICIAN) then
		NoTb[0] = CT_MAGICIAN
		NoTb[1] = CT_MAGE
		NoTb[2] = CT_BATTLEMAGE
	elseif(iClass == CT_ARCHER) then
		NoTb[0] = CT_ARCHER
		NoTb[1] = CT_HUNTER
		NoTb[2] = CT_RANGER
	elseif(iClass == CT_THIEF) then
		NoTb[0] = CT_THIEF
		NoTb[1] = CT_CLOWN
		NoTb[2] = CT_ASSASSIN
	else
		if(iClass == CT_KNIGHT or iClass == CT_PALADIN or iClass == CT_DRAOON) then
			NoTb[0] = CT_FIGHTER
			NoTb[1] = CT_KNIGHT
			NoTb[2] = CT_PALADIN
			NoTb[3] = CT_DRAOON
		elseif(iClass ==CT_WARRIOR or iClass ==CT_GLADIATOR or iClass ==CT_DESTROYER) then
			NoTb[0] = CT_FIGHTER
			NoTb[1] = CT_WARRIOR
			NoTb[2] = CT_GLADIATOR
			NoTb[3] = CT_DESTROYER
		elseif(iClass ==CT_MAGE	or iClass ==CT_WIZARD or iClass ==CT_ARCHMAGE) then
			NoTb[0] = CT_MAGICIAN
			NoTb[1] = CT_MAGE
			NoTb[2] = CT_WIZARD
			NoTb[3] = CT_ARCHMAGE
		elseif(iClass ==CT_BATTLEMAGE or iClass ==CT_WARMAGE or iClass ==CT_WARLORD) then
			NoTb[0] = CT_MAGICIAN
			NoTb[1] = CT_BATTLEMAGE
			NoTb[2] = CT_WARMAGE
			NoTb[3] = CT_WARLORD
		elseif(iClass ==CT_HUNTER or iClass ==CT_TRAPPER or iClass ==CT_SENTINEL) then
			NoTb[0] = CT_ARCHER
			NoTb[1] = CT_HUNTER
			NoTb[2] = CT_TRAPPER
			NoTb[3] = CT_SENTINEL
		elseif(iClass ==CT_RANGER or iClass ==CT_SNIPER or iClass ==CT_LAUNCHER) then
			NoTb[0] = CT_ARCHER
			NoTb[1] = CT_RANGER
			NoTb[2] = CT_SNIPER
			NoTb[3] = CT_LAUNCHER
		elseif(iClass ==CT_CLOWN or iClass ==CT_DANCER or iClass ==CT_FREEJACKER) then
			NoTb[0] = CT_THIEF
			NoTb[1] = CT_CLOWN
			NoTb[2] = CT_DANCER
			NoTb[3] = CT_FREEJACKER
		elseif(iClass ==CT_ASSASSIN or iClass ==CT_NINJA or iClass ==CT_SHADOW) then
			NoTb[0] = CT_THIEF
			NoTb[1] = CT_ASSASSIN
			NoTb[2] = CT_NINJA
			NoTb[3] = CT_SHADOW
		
		elseif(iClass ==CT_SHAMAN or iClass ==CT_SUMMONER or iClass ==CT_NATURE_MASTER or iClass == CT_GAIA_MASTER) then
			--ODS("소환사\n", false, 912)
			NoTb[0] = CT_SHAMAN
			NoTb[1] = CT_SUMMONER
			NoTb[2] = CT_NATURE_MASTER
			NoTb[3] = CT_GAIA_MASTER		
		elseif(iClass ==CT_DOUBLE_FIGHTER or iClass ==CT_TWINS or iClass ==CT_MIRAGE or iClass == CT_DRAGON_FIGHTER) then
			--ODS("격투가\n", false, 912)
			NoTb[0] = CT_DOUBLE_FIGHTER
			NoTb[1] = CT_TWINS
			NoTb[2] = CT_MIRAGE
			NoTb[3] = CT_DRAGON_FIGHTER			
		end
	end
	return NoTb
end

function UI_SetListElement(wnd, i)
	ODS("UI_SetListElement Start\n")
	if wnd:IsNil() == true then return end
	local Lst = wnd:GetControl("LST_LIST")
	if Lst:IsNil() == true then return end

	Lst:ClearAllListItem()
	Lst:ElementID(SKILL_BG_NAME_LIST[i])
	local Ele = Lst:AddNewListItem(GetTT(30000+i))
	if false == Ele:IsNil() then
		local wndEle = Ele:GetWnd()
		if false == wndEle:IsNil() then
			wndEle:SetStaticText("")
			UI_SkillArrowRefresh(wndEle)
		end
	end
	ODS("UI_SetListElement End\n")
end

function UI_SkillArrowRefresh(wnd)
	ODS("UI_SkillArrowRefresh \n")
	if wnd:IsNil() == true then return end

	local iCount = table.getn(ARROW_STRUCT)
	if 0>=iCount then return end

	local	kSkillTree	=	GetSkillTree();

	for i=1,iCount do
		local wnd = ARROW_STRUCT[i]["WND"]
		if wnd:IsNil() == false then
			local iFrom = ARROW_STRUCT[i]["FROM"]
			local iTo = ARROW_STRUCT[i]["TO"]
			local bCan = kSkillTree:GetCanLearn(iTo, iFrom)
			local iColor = 9078383
			local Loc = wnd:GetLocation3()
			Loc:SetZ(0)
			if true == bCan then
				iColor = 15542569
				Loc:SetZ(2)
			end
			wnd:SetColor(iColor)
			wnd:SetLocation3(Loc)
		end
	end
end

function UI_SkillArrowInit(wnd, iFrom, iTo)
	ODS("UI_SkillArrowInit  FROM : " .. iFrom .. " TO : " .. iTo .. "\n")
	local ID = string.sub(wnd:GetID():GetStr(), 1,9)
	if "IMG_ARROW" == ID then
		local iNo = tonumber(string.sub(wnd:GetID():GetStr(), 10))
		ARROW_STRUCT[iNo] = { ["WND"] = wnd, ["FROM"] = iFrom, ["TO"] = iTo }
		local iCount = table.getn(ARROW_STRUCT)
		ODS(wnd:GetID():GetStr() .. " " .. iNo .. " total : " .. iCount .. "\n")
	end
end

function UI_SkillBG_OnClose(wnd)
	ARROW_STRUCT = {}
end

function IsActivitySkill()
	local wnd = GetUIWnd("SFRM_POST")
	if false==wnd:IsNil() then return false end
	
	return true
end

function OnUp_SkillSetDelayTime(kWnd)
	if nil==kWnd or kWnd:IsNil() then return end
	
	local iMinTime = kWnd:GetCustomDataAsInt()
	
	local kTime = kWnd:GetParent():GetControl("IMG_TIME")
	local value = kTime:GetCustomDataAsInt() + 1
	value = value % 11
	if 1 > value then
		value = iMinTime
	end
	kTime:SetCustomDataAsInt(value)
	kTime:SetStaticText(tonumber(string.format("%0.1f", value/10)))
	
	ChangeSaveState(true)
end

function OnDown_SkillSetDelayTime(kWnd)
	if nil==kWnd or kWnd:IsNil() then return end
	
	local iMinTime = kWnd:GetCustomDataAsInt()
	
	local kTime = kWnd:GetParent():GetControl("IMG_TIME")
	local value = kTime:GetCustomDataAsInt() - 1
	if value < iMinTime then
		value = 10
	end
	value = value % 11
	kTime:SetCustomDataAsInt(value)
	kTime:SetStaticText(tonumber(string.format("%0.1f", value/10)))
	
	ChangeSaveState(true)
end

function UI_StrategySkillTab(kWnd)
	if nil==kWnd or kWnd:IsNil() then return end

	local kTab1 = kWnd:GetControl("BTN_STRATEGYSKILL0")
	local kTab2 = kWnd:GetControl("BTN_STRATEGYSKILL1")
	if kTab1:IsNil() or kTab2:IsNil() then return end

	local kImg1 = kTab1:GetControl("IMG_SELECT")
	local kImg2 = kTab2:GetControl("IMG_SELECT")
	if false==kImg1:IsNil() and false==kImg2:IsNil() then 
		if 1 == GetStrategySkillTabNo() then
			kImg1:Visible(false)
			kImg2:Visible(true)
		else
			kImg1:Visible(true)
			kImg2:Visible(false)
		end
	end

	kTab2:SetEnable(IsOpenStrategySkillSecond())
end

function StrategySkillTabDown(kWnd)
	if nil==kWnd or kWnd:IsNil() then return end
	local iTabNo = kWnd:GetBuildIndex()
	if iTabNo==GetStrategySkillTabNo() then return end
	if 1==iTabNo and false==IsOpenStrategySkillSecond() then return end

	local kOwner = kWnd:GetOwnerGuid()
	CallYesNoMsgBox(GetTT(401628), kOwner, 60)--MBT_STRATEGYSKILL_CHANGE
end

---------------------------- New Combo UI -----------------------------------
function CheckSkillWnd(kParentWnd, iClassNo)
	if nil==kParentWnd or true==kParentWnd:IsNil() then
		return
	end
	local kSkillTab = kParentWnd:GetControl("CBTN_SKILL_TAB")
	local kComboTab = kParentWnd:GetControl("CBTN_COMBO_TAB")
	local kComboSkillTab = kParentWnd:GetControl("CBTN_COMBO_SKILL_TAB")
	if nil~=kSkillTab and false==kSkillTab:IsNil() then
		if true == kSkillTab:GetCheckState() then
			UI_Click_Skill_Tab(kSkillTab, iClassNo)
			kSkillTab:CheckState(true)
		end
	end
	if nil~=kComboTab and false==kComboTab:IsNil() then
		if true == kComboTab:GetCheckState() then
			UI_Click_Combo_Tab(kComboTab)
			kComboTab:CheckState(true)
		end
	end
	if nil~=kComboSkillTab and false==kComboSkillTab:IsNil() then
		if true == kComboSkillTab:GetCheckState() then
			UI_Click_ComboSkill_Tab(kComboSkillTab)
			kComboSkillTab:CheckState(true)
		end
	end
end

function CheckGuidMsg(kParentWnd)
	if nil==kParentWnd or true==kParentWnd:IsNil() then
		return
	end
	
	local kSkillTab = kParentWnd:GetControl("CBTN_SKILL_TAB")
	local kComboTab = kParentWnd:GetControl("CBTN_COMBO_TAB")
	local kComboSkillTab = kParentWnd:GetControl("CBTN_COMBO_SKILL_TAB")
	local kBtn = kParentWnd:GetControl("BTN_CONFIRM")
	local kGuidMsg = kParentWnd:GetControl("FRM_GUIDE_MSG")
	if nil==kBtn or true==kBtn:IsNil() then
		return
	end
	if nil==kGuidMsg or true==kGuidMsg:IsNil() then
		return
	end

	if nil~=kSkillTab and false==kSkillTab:IsNil() then
		if true == kSkillTab:GetCheckState() then			
			if( true == kBtn:IsVisible() ) then 
				kGuidMsg:Visible(false)
			else
				kGuidMsg:Visible(true)
			end
		end
	end
	if nil~=kComboTab and false==kComboTab:IsNil() then
		if true == kComboTab:GetCheckState() then
			kGuidMsg:Visible(false)
		end
	end
	if nil~=kComboSkillTab and false==kComboSkillTab:IsNil() then
		if true == kComboSkillTab:GetCheckState() then
			kGuidMsg:Visible(false)
		end
	end
end

function SelectSkillTab( kParentWnd, iTab )
	if nil==kParentWnd or true==kParentWnd:IsNil() then
		return
	end
	local kSkillTab = kParentWnd:GetControl("CBTN_SKILL_TAB")
	local kComboTab = kParentWnd:GetControl("CBTN_COMBO_TAB")
	local kComboSkillTab = kParentWnd:GetControl("CBTN_COMBO_SKILL_TAB")
	
	if 0 == iTab then
		if nil~=kSkillTab and false==kSkillTab:IsNil() then
			if true == kSkillTab:GetCheckState() then
				kSkillTab:CheckState(false)
				return
			end
		end
		if nil~=kComboTab and false==kComboTab:IsNil() then
			kComboTab:CheckState(false)
		end
		if nil~=kComboSkillTab and false==kComboSkillTab:IsNil() then
			kComboSkillTab:CheckState(false)
		end
		UI_Click_Skill_Tab(kSkillTab, -1)
	elseif 1 == iTab then
		if nil~=kComboTab and false==kComboTab:IsNil() then
			if true == kComboTab:GetCheckState() then
				kComboTab:CheckState(false)
				return
			end
		end
		if nil~=kSkillTab and false==kSkillTab:IsNil() then
			kSkillTab:CheckState(false)
		end
		if nil~=kComboSkillTab and false==kComboSkillTab:IsNil() then
			kComboSkillTab:CheckState(false)
		end
		UI_Click_Combo_Tab(kComboTab)
	elseif 2 == iTab then
		if nil~=kComboSkillTab and false==kComboSkillTab:IsNil() then
			if true == kComboSkillTab:GetCheckState() then
				kComboSkillTab:CheckState(false)
				return
			end
		end
		if nil~=kSkillTab and false==kSkillTab:IsNil() then
			kSkillTab:CheckState(false)
		end
		if nil~=kComboTab and false==kComboTab:IsNil() then
			kComboTab:CheckState(false)
		end
		UI_Click_ComboSkill_Tab(kComboSkillTab)
	end
end

function UI_Click_Skill_Tab(wnd, iClassNo)
	if nil==wnd or true==wnd:IsNil() then
		return false;
	end
	local kParent = wnd:GetParent();
	if nil==kParent or true==kParent:IsNil() then
		return false;
	end
	if -1 ~= iClassNo then
		UI_SetListElement( kParent, iClassNo )
	else
		for i=0,3 do
			local Btn = kParent:GetControl("CBTN_CLASS"..i)
			if true == Btn:GetCheckState() then
				UI_SetListElement( kParent, Btn:GetCustomDataAsInt() )

				local	kSkillTree = GetSkillTree();
				if nil~=kSkillTree and false==kSkillTree:IsNil() then
					if true == kSkillTree:IsTemporaryRemainSkillPoint() then
						local kSkillWnd = GetUIWnd("SFRM_SKILL_WINDOW")
						if nil~=kSkillWnd and false==kSkillWnd:IsNil() then
							--	초기화 버튼 숨기기
							UI_SKILLWND_ShowResetButton(kSkillWnd, true);
							--	확인 버튼 숨기기
							UI_SKILLWND_ShowConfirmButton(kSkillWnd, true);
						end
					end
				end
			end
		end
	end
	return false
end
 
function UI_Click_Combo_Tab(wnd)
	if nil==wnd or true==wnd:IsNil() then
		return false;
	end
	local kParent = wnd:GetParent();
	if nil==kParent or true==kParent:IsNil() then
		return false;
	end
	local Lst = kParent:GetControl("LST_LIST")
	if nil==Lst or Lst:IsNil()==true then 
		return false
	end
	Lst:ClearAllListItem()
	Lst:ElementID("FRM_COMBO_BG")
	local kItem = Lst:AddNewListItem(WideString("COMBO"))
	if nil~=kItem and false==kItem:IsNil() then
		local	kSkillTree = GetSkillTree();
		if nil~=kSkillTree and false==kSkillTree:IsNil() then
			kSkillTree:CallComboUI(kItem)
			
			if true == kSkillTree:IsTemporaryRemainSkillPoint() then
				UI_SKILLWND_ON_RESET_TEMPORARY()
				local kSkillWnd = GetUIWnd("SFRM_SKILL_WINDOW")
				if nil~=kSkillWnd and false==kSkillWnd:IsNil() then
					--	초기화 버튼 숨기기
					UI_SKILLWND_ShowResetButton(kSkillWnd, false);
					--	확인 버튼 숨기기
					UI_SKILLWND_ShowConfirmButton(kSkillWnd, false);
				end
			end
		end
	end
end

function UI_Click_ComboSkill_Tab(wnd)
	if nil==wnd or true==wnd:IsNil() then
		return false;
	end
	local kParent = wnd:GetParent();
	if nil==kParent or true==kParent:IsNil() then
		return false;
	end
	
	local Lst = kParent:GetControl("LST_LIST")
	if nil==Lst or Lst:IsNil()==true then 
		return false
	end
	Lst:ClearAllListItem()
	Lst:ElementID("FRM_COMBO_SKILL_BG")
	local kItem = Lst:AddNewListItem(WideString("COMBO_SKILL"))
	if nil~=kItem and false==kItem:IsNil() then
	end
end

function CallComboAlramUI( iWeaponType, iComboID )
	local kWnd = CallUI("FRM_ALARM_NEW_COMBO")
	if nil==kWnd or true==kWnd:IsNil() then
		return
	end
	local kSkillTree = GetSkillTree()
	if nil==kSkillTree or true==kSkillTree:IsNil() then
		return
	end
	kSkillTree:SetComboAlramUI( kWnd, iWeaponType, iComboID )
end