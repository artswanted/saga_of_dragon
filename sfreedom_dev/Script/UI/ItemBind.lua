
function CheckTimeItemBindReq()	
	if(  UISelf:GetBarNow() >0 and UISelf:GetBarNow() >= UISelf:GetBarMax() ) then		
		if(true == ItemBindCheckOK() ) then 
			ItemBindSendReq()
			CloseUI("SFRM_ITEM_BINDING")
		end		
	end	
end

function CheckTimeItemUnbindReq()	
	if(  UISelf:GetBarNow() > 0 and UISelf:GetBarNow() >= UISelf:GetBarMax() ) then
		if(true == ItemUnbindSendReq() ) then 			
			ItemBindSendReq()
			CloseUI("SFRM_ITEM_UNBINDING")
		end		
	end	
end
