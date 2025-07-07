MarbasBloodTips = MarbasBloodTips or DeclareView("MarbasBloodTips", "marbas/marbas_blood_tips")
function MarbasBloodTips:MarbasBloodTips()
    -- self.board
end
function MarbasBloodTips:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), Language.Marbas.BloodTips, Vector2.New(930, 655))
end

MarbasBloodTipsPanel = MarbasBloodTipsPanel or DeclareMono("MarbasBloodTipsPanel", UIWFlushPanel)
function MarbasBloodTipsPanel:MarbasBloodTipsPanel()
    self.data = MarbasData.Instance
    self:InitPanel()


end 

function MarbasBloodTipsPanel:onFlush() 
    self.part_1_click:Trigger()
end 

function MarbasBloodTipsPanel:OnClickColumn(index)
    self.part_1:SetActive(index == 1)
    self.part_2:SetActive(index == 2)
end

function MarbasBloodTipsPanel:InitPanel()
    self.part_1_list:SetData(self.data:GetBloodPlusTipsList())
    self.part_2_list:SetData(self.data:GetBloodPlusGemTipsList())
end

MarbasBloodTipsForceCell = MarbasBloodTipsForceCell or DeclareMono("MarbasBloodTipsForceCell",UIWidgetBaseItem)
function MarbasBloodTipsForceCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.level,data.level)
    UH.SetText(self.force,data.force)
    UH.SetText(self.plus_attr,data.attr_str)
end 

-- MarbasBloodTipsItem
-- MarbasBloodTipsParentItem
MarbasBloodTipsParentItem = MarbasBloodTipsParentItem or DeclareMono("MarbasBloodTipsParentItem",UIWidgetBaseItem)
function MarbasBloodTipsParentItem:MarbasBloodTipsParentItem() end
function MarbasBloodTipsParentItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.LblTitle, data.name)
    UH.SetText(self.LblPlus, data.plus)
    UH.SetText(self.LblDesc, data.desc)
end 

function MarbasBloodTipsParentItem:OnSelect(selected)
    if self.Arrow then
        self.Arrow:SetActive(not selected)
    end

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

MarbasBloodTipsItem = MarbasBloodTipsItem or DeclareMono("MarbasBloodTipsItem",UIWidgetBaseItem)
function MarbasBloodTipsItem:MarbasBloodTipsItem() end
function MarbasBloodTipsItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.LblLevel,data.level)
    UH.SetText(self.LblDesc,data.desc)
end 