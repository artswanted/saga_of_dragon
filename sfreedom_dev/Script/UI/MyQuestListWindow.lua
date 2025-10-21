--[[
QUEST_STATE_NONE = 0;
QUEST_STATE_BEGIN = 1;
QUEST_STATE_ING = 3;
QUEST_STATE_END = 240;
QUEST_STATE_FINISHED = 255;

COLOR_GREEN = 0x00ff00;
COLOR_WHITE = 0xffffff;

function UI_MY_QUEST_LIST_WINDOW_ON_UPDATE(wnd, my_quest_list)
	
	listctl_quest_list = wnd:GetControl("SFRM_MY_QUEST_LIST"):GetControl("QUEST_LIST");
	
	iTotalMyQuest = my_quest_list:GetCount();
	
	i = 0;
	
	while i<iTotalMyQuest do
		
		quest_info = my_quest_list:GetAt(i);
		quest_xml_info = quest_info:GetXMLInfo();

		bAlreadyExist = false;

		listitem_group_item = listctl_quest_list:ListFindItemW(quest_xml_info:GetGroupName());
		treectl_tree_item = nil;
		listitem_item = nil;

		if listitem_group_item:IsNil() == false then
		
			treectl_tree_item = listitem_group_item:GetWnd();
			listitem_item = treectl_tree_item:TreeFirstItem();
			while listitem_item:IsNil()==false do
			
				wnd_item_wnd = listitem_item:GetWnd();
				
				sQuestID = wnd_item_wnd:GetCustomDataAsInt();
				if sQuestID == quest_info:GetQuestID() then
				
					bAlreadyExist = true;
					break;
				
				end
				
				listitem_item = treectl_tree_item:TreeNextItem(listitem_item);
			
			end
		end		
		
		sQuestState = quest_info:GetQuestState();
		if bAlreadyExist == true then
		
			if  sQuestState == QUEST_STATE_ING or sQuestState == QUEST_STATE_END then
			
				ItemTitle = quest_xml_info:GetQuestTitle();
				if sQuestState == QUEST_STATE_END then
					
					listitem_item:GetWnd():SetFontColor(COLOR_GREEN);
					ItemTitle = GetAppendTextW(ItemTitle, GetTextW(20001));
				
				else
				
					listitem_item:GetWnd():SetFontColor(COLOR_WHITE);
				
				end
				
				listitem_item:GetWnd():SetStaticTextW(ItemTitle);
			
			else
			
				treectl_tree_item:TreeDeleteItem(listitem_item);
				if treectl_tree_item:TreeFirstItem():IsNil() == true then
				
					listctl_quest_list:ListDeleteItem(listitem_group_item);
				
				end
			
			end
			
		else
		
			if sQuestState == QUEST_STATE_ING or sQuestState == QUEST_STATE_END then
			
				local kGroupNameW = quest_xml_info:GetGroupName()
				if listitem_group_item:IsNil() == true then
					listitem_group_item = listctl_quest_list:AddNewListItem(kGroupNameW)
				end
				
				sQuestID = quest_info:GetQuestID();
				treectl_tree_item = listitem_group_item:GetWnd();
				treectl_tree_item:GetControl("SFRM_QUEST_GROUP"):SetStaticText(kGroupNameW:GetStr())--Set SFRM Group Name <070610 NaturalLaw>
				
				listitem_addedItem = treectl_tree_item:AddNewTreeItem(WideString(""));
				listitem_addedItem:GetWnd():SetCustomDataAsInt(sQuestID);
				
				ItemTitle = quest_xml_info:GetQuestTitle();
				if sQuestState == QUEST_STATE_END then
					
					listitem_addedItem:GetWnd():SetFontColor(COLOR_GREEN);
					ItemTitle = GetAppendTextW(ItemTitle, GetTextW(20001));
				else
					
					listitem_addedItem:GetWnd():SetFontColor(COLOR_WHITE);
				end
				
				listitem_addedItem:GetWnd():SetStaticTextW(ItemTitle);
			
				checkbtn = listitem_addedItem:GetWnd():GetControl("CHKBTN_CHK");
				checkbtn:SetCustomDataAsInt(sQuestID);
				
				treectl_tree_item:ExpandTree();
			
			end
			
		
		end
	
	
		i = i + 1;
	
	end
end

]]--

