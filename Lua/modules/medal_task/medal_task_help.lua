MedalTaskHelpView = MedalTaskHelpView or DeclareView("MedalTaskHelpView", "medal/medal_task_help")

VIEW_DECLARE_MASK(MedalTaskHelpView, ViewMask.BgBlockClose)

function MedalTaskHelpView:MedalTaskHelpView()
    self.data = MedalTaskData.Instance
    self.care_handle = self.data.medal_task_help_data:Care(BindTool.Bind(self.FlushHelpView,self))    
end

function MedalTaskHelpView:LoadCallback()
    self.BtnList:SetDataList(Config.medaltask_list_auto.strategy_title)
    self:FlushHelpView()
end

function MedalTaskHelpView:CloseCallback()
    self.data.medal_task_help_data:Uncare(self.care_handle)
    self.data.medal_task_help_data.type = 1
	self.care_handle = nil
end

function MedalTaskHelpView:FlushHelpView()
    self.List:SetData(self.data:GetMedalTaskHelpData())
end

--========================MedalTaskHelpBtnItem========================
MedalTaskHelpBtnItem = MedalTaskHelpBtnItem or DeclareMono("MedalTaskHelpBtnItem", UIWidgetBaseItem)

function MedalTaskHelpBtnItem:SetData(data)
    UH.SetText(self.Name,data.title)
    -- UH.SetText(self.Label,Format(Language.Role.PromoteHelpBtn,DataHelper.GetDaXie(data.type)))
    UIWidgetBaseItem.SetData(self, data)
end

function MedalTaskHelpBtnItem:OnClickItem()
	MedalTaskData.Instance.medal_task_help_data.type = self.data.type
end

function MedalTaskHelpBtnItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

--========================MedalTaskHelpItem========================
MedalTaskHelpItem = MedalTaskHelpItem or DeclareMono("MedalTaskHelpItem", UIWidgetBaseItem)

function MedalTaskHelpItem:SetData(data)
    UH.SetText(self.Desc,data.desc or "")
    UH.SetText(self.SubDesc,data.sub_desc or "")
    self.SubDesc:SetObjActive(data.sub_desc ~= "")
    self.None:SetActive(data.sub_desc ~= "")
end