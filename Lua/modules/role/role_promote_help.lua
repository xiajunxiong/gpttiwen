RolePromoteHelpView = RolePromoteHelpView or DeclareView("RolePromoteHelpView", "role/role_promote_help")

VIEW_DECLARE_MASK(RolePromoteHelpView, ViewMask.BgBlockClose)

function RolePromoteHelpView:RolePromoteHelpView()
    self.data = RolePromoteData.Instance
    self.care_handle = self.data.promote_help_data:Care(BindTool.Bind(self.FlushHelpView,self))    
end

function RolePromoteHelpView:LoadCallback()
    self.BtnList:SetDataList(Config.career_advancements_auto.strategy_title)
    self:FlushHelpView()
end

function RolePromoteHelpView:CloseCallback()
    self.data.promote_help_data:Uncare(self.care_handle)
    self.data.promote_help_data.type = 1
	self.care_handle = nil
end

function RolePromoteHelpView:FlushHelpView()
    self.List:SetData(self.data:GetPromoteHelpData())
end

--========================RolePromoteHelpBtnItem========================
RolePromoteHelpBtnItem = RolePromoteHelpBtnItem or DeclareMono("RolePromoteHelpBtnItem", UIWidgetBaseItem)

function RolePromoteHelpBtnItem:SetData(data)
    UH.SetText(self.Name,data.title)
    UH.SetText(self.Label,Format(Language.Role.PromoteHelpBtn,DataHelper.GetDaXie(data.type)))
    UIWidgetBaseItem.SetData(self, data)
end

function RolePromoteHelpBtnItem:OnClickItem()
	RolePromoteData.Instance.promote_help_data.type = self.data.type
end

function RolePromoteHelpBtnItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

--========================RolePromoteHelpItem========================
RolePromoteHelpItem = RolePromoteHelpItem or DeclareMono("RolePromoteHelpItem", UIWidgetBaseItem)

function RolePromoteHelpItem:SetData(data)
    UH.SetText(self.Desc,data.desc or "")
    UH.SetText(self.SubDesc,data.sub_desc or "")
    self.SubDesc:SetObjActive(data.sub_desc ~= "")
    self.None:SetActive(data.sub_desc ~= "")
end