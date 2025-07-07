RolePrestigeView = RolePrestigeView or DeclareMono("RolePrestigeView", UIWFlushPanel)

function RolePrestigeView:RolePrestigeView()
    self.data = PrestigeData.Instance
    self.data_cares = {
        {data = self.data.village_data, func = self.FlushTitleView, init_call = true},
        {data = self.data.village_prestige_info, func = self.FlushDataFlush, init_call = true},
    }
end

function RolePrestigeView:FlushDataFlush()
    self.List:SetDataList(self.data:GetVillageListConfig())
end

function RolePrestigeView:FlushTitleView()
    local village_id = self.data:GetVillageID()
    self.TitleList:SetData(PrestigeData.GetUpgradeData(village_id))
    local config = self.data:GetVillageList(village_id)
    UH.SetText(self.Title,config.name)
end

function RolePrestigeView:OnClickShop()
    local village_id = self.data:GetVillageID()
    local npc_id = self.data:GetNpcIdByVillageId(village_id)
    SceneLogic.Instance:AutoToNpcs(npc_id)
    ViewMgr:CloseView(RoleView)
end

function RolePrestigeView:OnClickDonation()
    local village_id = self.data:GetVillageID()
    local config = self.data:GetVillageList(village_id)
    SceneLogic.Instance:AutoToNpcs(config.juanzeng_npc)
    ViewMgr:CloseView(RoleView)
end

--===========================RolePrestigeItem===========================
RolePrestigeItem = RolePrestigeItem or DeclareMono("RolePrestigeItem", UIWidgetBaseItem)

function RolePrestigeItem:SetData(data)
    UH.SetText(self.Name,data.name)
    UH.SpriteName(self.Icon, data.pic)

    local info,config = PrestigeData.Instance:GetVillagePrestigeNextData(data.village_id)
    local cur_num,need_num = info.prestige,config.need_prestige
    self.Progress:SetProgress(cur_num / need_num)
    self.Progress:SetText(Language.Role.Reputation ..
    Format(Language.Common.Progress2TxtColorBFormat, cur_num, need_num))

    local _,cur_config = PrestigeData.Instance:GetVillagePrestigeData(data.village_id)
    UH.SetText(self.Relation,cur_config.name)

    local relation = info.level > 6 and 6 or info.level
    self.Outline.effectColor = PRESTIGE_LEVEL_COLOR[relation]

    if relation > 0 then
        self.Gradient.topColor = COLORS.Yellow7
        self.Gradient.buttomColor = COLORS.Yellow1
    else
        self.Gradient.topColor=COLORS.Yellow13
        self.Gradient.buttomColor = COLORS.Yellow19
    end
    UIWidgetBaseItem.SetData(self, data)
end

function RolePrestigeItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
        PrestigeData.Instance:SetVillageID(self.data.village_id)
    end
end

function RolePrestigeItem:Click()
    self.ToggleClick:Trigger()
end

--===========================RolePrestigeTitleItem===========================
RolePrestigeTitleItem = RolePrestigeTitleItem or DeclareMono("RolePrestigeTitleItem", UIWidgetBaseItem)

function RolePrestigeTitleItem:SetData(data)
    UH.SetText(self.Title,data.name)
    UH.SetText(self.Desc,data.desc)
end

--===========================PrestigePopupView===========================
PrestigePopupView = PrestigePopupView or DeclareView("PrestigePopupView","main_other/prestige_popup",0,ViewLayer.Top - 1)

VIEW_DECLARE_MASK(PrestigePopupView,ViewMask.None)

function PrestigePopupView:PrestigePopupView()
    self.data = PrestigeData.Instance
end

function PrestigePopupView:OnFlush()
    TimeHelper:CancelTimer(self.show_time)
    local param_t = self.data:GetPopupParam()
    local add_num = self.data:GetPopupParamValue()
    self.need_num = self:FlushPopupView(param_t.old_data,param_t.village_id)
    local is_change_level = param_t.old_data.level ~= param_t.new_data.level

    if add_num ~= 0 then
        UH.SetText(self.AddNum,"+" .. add_num)
    end

    self.progress_time = TimeHelper:AddDelayTimer(function()
        if is_change_level then
            self.Effect:Play("10010857")
            self:FlushPopupView(param_t.new_data,param_t.village_id)
        else
            self:FlushPopupView(param_t.new_data,param_t.village_id)
            self.ProgressEx:AddToProgress(param_t.new_data.prestige / self.need_num,function()
                -- self.Effect:Play("10010856")
            end)
        end
    end, 1)
    self.show_time = TimeHelper:AddDelayTimer(function()
		ViewMgr:CloseView(PrestigePopupView)
    end, 4)
end

function PrestigePopupView:FlushPopupView(data,village_id)
    local config = self.data:GetVillageList(village_id)
    local cur_config = self.data:GetPrestigeUpgradeData(data.level,village_id)
    local next_config = self.data:GetPrestigeUpgradeData(data.level + 1,village_id) or cur_config

    UH.SpriteName(self.Icon, config.pic)
    UH.SetText(self.Name,Format(Language.Role.VillageNameTip,config.name))
    UH.SetText(self.Relation,cur_config.name)

    self.ProgressEx:SetProgress(data.prestige / next_config.need_prestige)
    UH.SetText(self.Num,Format("%s/%s",data.prestige,next_config.need_prestige))
    
    return next_config.need_prestige
end

function PrestigePopupView:CloseCallback()
    TimeHelper:CancelTimer(self.show_time)
    TimeHelper:CancelTimer(self.progress_time)
    PrestigeData.IsPopupNotice = nil
end