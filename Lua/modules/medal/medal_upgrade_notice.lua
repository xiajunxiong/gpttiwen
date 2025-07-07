
MedalUpgradeNoticeView = MedalUpgradeNoticeView or DeclareView("MedalUpgradeNoticeView", "medal/medal_upgrade_notice")

function MedalUpgradeNoticeView:MedalUpgradeNoticeView()
    self.medal_data = MedalData.Instance
    self.close_timer = nil
end

function MedalUpgradeNoticeView:LoadCallback(param_t)
    TimeHelper:CancelTimer(self.close_timer)
    self.close_timer = TimeHelper:AddDelayTimer(function()
        self.ClickArea:SetActive(true)
        self.medal_data:UnMarkAttrInfo()
    end,3.7)
    
    self.data = self.medal_data:GetNoticeResult()

    self.BaseAttr:SetData(self.data.attr)
    self.Effect:SetActive(self.data.is_show_effect)
    self.NewIcon:SetActive(self.data.is_new_effect)
    UH.SetText(self.EffectDesc,self.data.effect_desc)

    self.SuccEff:SetActive(self.data.is_succ)
    self.FailEff:SetActive(not self.data.is_succ)

    UH.SetIcon(self.MedalIcon, self.data.item_mark:BigIcon())
    UH.SetText(self.MedalName,self.medal_data:GetMedalFullName())
    
    self.Show_Anim:SetInteger(APH("is_succ"),self.data.is_succ and 1 or 0)
end

function MedalUpgradeNoticeView:CloseCallback()
    TimeHelper:CancelTimer(self.close_timer)
    self.close_timer = nil
end

function MedalUpgradeNoticeView:OnClickClose()
    ViewMgr:CloseView(MedalUpgradeNoticeView)
end

-- function MedalUpgradeNoticeView:ChangeClick()
--     UH.SetIcon(self.MedalIcon, self.data.item_:BigIcon())
--     LogError("????",self.medal_data:GetMedalFullName())
--     UH.SetText(self.MedalName,self.medal_data:GetMedalFullName())
-- end

MedalDescItem = DeclareMono("MedalDescItem",UIWidgetBaseItem)
function MedalDescItem:MedalDescItem()
end

function MedalDescItem:SetData(data)
    local type = Language.Common.AttrList[data.attr_type]
    local color = data.change > 0 and COLORSTR.Green4 or COLORSTR.Red8
    local change_str = data.change > 0 and "+" or "-"
    local change = data.change ~= 0 and string.format( Language.Medal.MedalAttrPlus,color,change_str,math.abs(data.change)) or ""
    UH.SetText(self.text,type.."  "..data.attr_value..change)
end 