
UIWBoard5 = UIWBoard5 or DeclareMono("UIWBoard5")

function UIWBoard5:UIWBoard5()
	self.title_str = ""
	self.view_type = nil
end

function UIWBoard5:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
	end
end

function UIWBoard5:SetData(view_type, title_str, width)
	self.view_type = view_type
	self.Title.text = title_str
	self.BgTrans.sizeDelta = Vector2.New(width, 551)
end

function UIWBoard5:OnRelease()
end
