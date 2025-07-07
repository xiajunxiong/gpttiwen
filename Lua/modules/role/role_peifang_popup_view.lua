RolePeiFangPopupView = RolePeiFangPopupView or DeclareView("RolePeiFangPopupView","main_other/peifang_popup",0,ViewLayer.Top - 1)
VIEW_DECLARE_MASK(RolePeiFangPopupView,ViewMask.None)

function RolePeiFangPopupView:RolePeiFangPopupView()	
    self.close_timer = nil
	self.show_id = 0
end

function RolePeiFangPopupView:LoadCallback(data)
	self.show_id = data.show_id
end

function RolePeiFangPopupView:OpenCallback()
	local item = Item.New({item_id = self.show_id})
	
	self.ItemShow:SetData(item)
	UH.SetText(self.ItemName,item:QuaName(true))
	UH.SetText(self.Desc,item:Desc())
	
   self.close_timer = TimeHelper:AddDelayTimer(function() 
        self.close_timer = nil
        ViewMgr:CloseView(RolePeiFangPopupView)
    end,self.ShowTime)
end

function RolePeiFangPopupView:CloseCallback()
    if self.close_timer ~= nil then
        TimeHelper:CancelTimer(self.close_timer)
        self.close_timer = nil
    end
	if WorkshopCtrl.Instance.popup_list[1] then
		table.remove(WorkshopCtrl.Instance.popup_list,1)
	end
	if #WorkshopCtrl.Instance.popup_list ~= 0 then
        ViewMgr:OpenView(RolePeiFangPopupView,{show_id = WorkshopCtrl.Instance.popup_list[1]})
    end
end

function RolePeiFangPopupView:OnClickClose()
	ViewMgr:CloseView(RolePeiFangPopupView)
end