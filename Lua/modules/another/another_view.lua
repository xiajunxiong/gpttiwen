AnotherView = AnotherView or DeclareView("AnotherView", "another/another_view")

function AnotherView:AnotherView()
    self.data = AnotherData.Instance
    self.step_index,self.step_list = 1,{}
    AnotherCtrl.Instance:SetSelectFunc(function(index)
        if AnotherData.IsGuide() then
            self.BtnTabs[index].isOn = true
        else
            self.TabToggle[index].isOn = true
        end
    end)
end

function AnotherView:OnClickClose()
    if AnotherData.IsGuide() then
        AnotherCtrl.Instance:SendAllReq(ANOTHER_INFO_TYPE.FINISH_GUIDE)
        BattlefieldData.Instance:SetGuideData(true)
    end
    ViewMgr:CloseView(AnotherView)
end

function AnotherView:OnTipsClick()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[155].desc})
end

function AnotherView:LoadCallback(param_t)
    local level = BattlefieldData.Instance:GetOtherConfig().level_limit
    UH.SetText(self.MaskItemTip,Format(Language.Battlefield.MaskItemTip,level))

    if AnotherData.IsGuide() then
        self.Guide:SetActive(true)
        self:FlushStepView()
    end
    if param_t and param_t.taggle_index then
        if AnotherData.IsGuide() then
            self.BtnTabs[param_t.taggle_index].isOn = true
        else
            self.TabToggle[param_t.taggle_index].isOn = true
        end
    end
end

--新人引导
function AnotherView:OnClickComfirm(num)
    if num > 0 and self.data:IsHasTip(self.step_index) then
        PublicPopupCtrl.Instance:AlertTip(Language.Another.Error[self.step_index],function()
            self:FlushStepView(num)
        end)
        return
    end
    self:FlushStepView(num)
end

function AnotherView:OnClickNext()
    if self.data:IsHasTip(self.step_index) then
        PublicPopupCtrl.Instance:AlertTip(Language.Another.Error[self.step_index],function()
            self:OnClickClose()
        end)
        return
    end
    if self.data:IsHasTip(self.step_index+1) then
        PublicPopupCtrl.Instance:AlertTip(Language.Another.Error[self.step_index+1],function()
            self:OnClickClose()
        end)
        return
    end
    self:OnClickClose()
end

function AnotherView:OnClickTabs()
    local index = self.step_index
    if self.BtnTabs[index] then
        self.BtnTabs[index].isOn = true
    end
    if self.step_list[index] == nil then
        self.step_list[index] = true
        self.MaskList[index]:SetActive(true)
    end
end

function AnotherView:OnClickMask(index)
    self.MaskList[index]:SetActive(false)
end

function AnotherView:FlushStepView(num)
    self.step_index = self.step_index + (num or 0)
    if self.step_index == 2 and self.data:GetPetCount() == 0 then
        self.step_list[self.step_index] = true
    end
    self:FlushBotton()
    self:OnClickTabs()
end

function AnotherView:FlushBotton()
    self.BtnClose:SetActive(self.step_index ~= 1)
    self.BtnNext:SetActive(self.step_index ~= 2)
    self.BtnComfirm:SetActive(self.step_index == 2)
end