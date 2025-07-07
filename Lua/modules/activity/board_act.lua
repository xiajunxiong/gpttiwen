
BoardAct = BoardAct or DeclareMono("BoardAct")

function BoardAct:BoardAct()
	self.title_str = ""
	self.view_type = nil
end

function BoardAct:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
	end
end

function BoardAct:SetData(view_type,title_str)
	self.view_type = view_type
	self.title_str = title_str
	self.Title.text = self.title_str
end

function BoardAct:SetTabbarVisible(visible)
	self.Tabbar:SetObjActive(visible)
end

function BoardAct:OnRelease()
	self.Tabbar:SetObjActive(false)
end

function BoardAct:SetRootName(name)
	self.Root.name = name
end