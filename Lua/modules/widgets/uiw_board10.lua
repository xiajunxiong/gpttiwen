UIWBoard10 = UIWBoard10 or DeclareMono("UIWBoard10")

function UIWBoard10:UIWBoard10()
	self.view_type = nil
	self.close_func = nil

end

function UIWBoard10:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
		if self.close_func then
			self.close_func()
		end
	end
end

function UIWBoard10:SetData(view_type, title_str, size_x)
	self.view_type = view_type
	self:SetTitle(title_str)
	local v_x = size_x or 780
	self.BG.sizeDelta = Vector2.New(v_x,585)
end

function UIWBoard10:OnRelease()
	self.close_func = nil
end

function UIWBoard10:SetCloseFunc(func)
	self.close_func = func
end

function UIWBoard10:SetTitle(title_str)
	self.TitleM.text = title_str
end
