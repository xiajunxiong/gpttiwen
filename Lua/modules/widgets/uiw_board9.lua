
UIWBoard9 = UIWBoard9 or DeclareMono("UIWBoard9")

function UIWBoard9:UIWBoard9()

end

function UIWBoard9:OnClickClose()
    if self.view_type ~= nil then
        ViewMgr:CloseView(self.view_type)
    end
end

function UIWBoard9:SetData(view_type, title)
    self.view_type = view_type
    self.Title.text = title
end

function UIWBoard9:OnRelease()

end

