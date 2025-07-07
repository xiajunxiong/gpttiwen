ServiceBoard1 = ServiceBoard1 or DeclareMono("ServiceBoard1")

function ServiceBoard1:ServiceBoard1()
	self.title_str = ""
	self.view_type = nil
end

function ServiceBoard1:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
	end
end
function ServiceBoard1:SetData(view_type, title)
	self.view_type = view_type
	UH.SetText(self.Title, title)
end

function ServiceBoard1:OnRelease()
end

ServiceBoard2 = ServiceBoard2 or DeclareMono("ServiceBoard2")

function ServiceBoard2:ServiceBoard2()
	self.title_str = ""
	self.view_type = nil
end

function ServiceBoard2:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
	end
end
function ServiceBoard2:SetData(view_type, title)
	self.view_type = view_type
	UH.SetText(self.Title, title)
end

function ServiceBoard2:OnRelease()
end