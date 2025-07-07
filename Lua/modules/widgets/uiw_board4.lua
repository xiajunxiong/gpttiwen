------Title====UI.Text
------BG====UI.Rect
------Tabbar===UIWidgetList

UIWBoard4 = UIWBoard4 or DeclareMono("UIWBoard4")

function UIWBoard4:UIWBoard4()
	self.title_str = ""
	self.view_type = nil
end

function UIWBoard4:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
	end
end

function UIWBoard4:SetTabbarVisible(visible)
	self.Tabbar:SetObjActive(visible)
end

function UIWBoard4:SetData(view_type, title_str, v2)
	self.view_type = view_type
end

function UIWBoard4:ChangeTitle(title_str)
	self.Title.text = title_str
end

function UIWBoard4:OnRelease()
	self.Tabbar:SetData({})
	self.Tabbar:SetObjActive(false)
end
