ReputationShopHelpView = ReputationShopHelpView or DeclareView("ReputationShopHelpView", "reputation/reputation_shop_help")

function ReputationShopHelpView:ReputationShopHelpView()
    self.Board:SetData(self:GetType(), Language.Reputation.ReputationPreview, Vector2.New(755, 480))
end

function ReputationShopHelpView:LoadCallback(param_t)
    local config = PrestigeData.Instance:GetVillageList(param_t.village_id) or {}
    local info = PrestigeData.Instance:GetVillagePrestigeData(param_t.village_id)

    UH.SpriteName(self.Icon, config.pic)
    UH.SetText(self.Name,config.name .. Language.Reputation.ReputationTitle)
    
    local village_list = PrestigeData.GetUpgradeConfig(param_t.village_id)
    
    self.List:SetData(village_list)
    self.TopList:SetData(village_list)

    local list,cur_prestige,max_prestige = self:GetFillAmountList(village_list,info.level)
    self.Progress:SetProgress(DataHelper.GetFillAmount(cur_prestige + info.prestige,list))
    -- UH.SetText(self.Num,Format("%s/%s",cur_prestige + info.prestige,max_prestige))
    UH.SetText(self.Num,"")

    local count = #village_list
    local spacing = (621 - 42 * count) / (count - 1)
    self.GridLayout.spacing = Vector2.New(spacing,0)
end

function ReputationShopHelpView:GetFillAmountList(config,relation)
    local list,cur_num,max_num = {0},0,0
    for i = 2, #config do
        local num = config[i].need_prestige
        max_num = max_num + num
        if relation >= config[i].relation then
            cur_num = cur_num + num
        end
        list[i] = max_num
    end
    return list,cur_num,max_num
end

----------------------------ReputationShopHelpTopItem----------------------------
ReputationShopHelpTopItem = ReputationShopHelpTopItem or DeclareMono("ReputationShopHelpTopItem", UIWidgetBaseItem)

function ReputationShopHelpTopItem:SetData(data)
    local info = PrestigeData.Instance:GetVillagePrestigeData()
    self.Image:SetActive(info.level >= data.relation)
    self.Panel:SetActive(data.relation ~= 0)
end

----------------------------ReputationShopHelpItem----------------------------
ReputationShopHelpItem = ReputationShopHelpItem or DeclareMono("ReputationShopHelpItem", UIWidgetBaseItem)

function ReputationShopHelpItem:SetData(data)
    UH.SetText(self.Name,data.name)
    local info = PrestigeData.Instance:GetVillagePrestigeData()
    local is_reach = info.level >= data.relation
    local prestige = is_reach and data.need_prestige or info.prestige
    local COLOR = is_reach and COLORSTR.Green3 or COLORSTR.Red19
    UH.SetText(self.Num,ColorStr(Format("%s/%s",prestige,data.need_prestige),COLOR))
    UH.SetText(self.Reach,ColorStr(is_reach and Language.Achieve.Reach or Language.Achieve.NotReach,COLOR))
    if info.level + 1 < data.relation or data.need_prestige == 0 then
        UH.SetText(self.Num,ColorStr(data.need_prestige,COLOR))
    end
end