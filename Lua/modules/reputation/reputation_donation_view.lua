ReputationDonationView = ReputationDonationView or DeclareView("ReputationDonationView", "reputation/reputation_donation")

function ReputationDonationView:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Reputation.Donation, Vector2.New(1021, 572))
end

function ReputationDonationView:CloseCallback()
    PrestigeData.Instance:InitDonateData()
end

ReputationDonationPanel = ReputationDonationPanel or DeclareMono("ReputationDonationPanel", UIWFlushPanel)

function ReputationDonationPanel:ReputationDonationPanel()
    self.data = PrestigeData.Instance
    self.data_cares = {
        {data = self.data.donate_notice,func = self.FlushDonationNum},
        {data = self.data.donation_data,func = self.FlushDonationItem},
        {data = self.data.village_prestige_info,func = self.FlushDonationView,init_call = false},
    }
    GuideManager.Instance:RegisterGetGuideUi("DonationVillageBtn", function()
        local village_id = Config.function_guide_auto.pet_skill[1].village_id
        if self.List and self.List.index_item then
            local index = self:GetIndex(village_id) or 1
            if self.List.index_item[index] then
                return self.List.index_item[index].BtnGoObj
            end
		end
	end)
end

function ReputationDonationPanel:GetIndex(village_id)
    for k,v in pairs(PrestigeData.Instance:GetVillageListConfig()) do
        if v.village_id == village_id then
            return k
        end
    end
end

function ReputationDonationPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.List:SetDataList(PrestigeData.Instance:GetVillageListConfig())
    if PrestigeData.IsGuideUi then
        local village_id = Config.function_guide_auto.pet_skill[1].village_id
        local index = self:GetIndex(village_id) or 1
        if index >= 4 then
            self.List:JumpVerticalIndex(index,114)
        end
        PrestigeData.IsGuideUi = nil
    end
end

function ReputationDonationPanel:FlushDonationItem()
    if self.param_t == nil then
        local item = ItemInfoCtrl.Instance:Data():GetItemData()
        self.Call:SetData(item)
        UH.SetText(self.ItemName,item:QuaName())
        self.param_t = {
            color = self.data:GetEquipColor(item.item_id),
            level = self.data:GetEquipLevel(item.item_id),
        }
    end
    local donate = self.data:GetDonateNum(self.param_t.color,
    self.param_t.level,self.data:GetDonationId())
    UH.SetText(self.ItemNum,Format(Language.Reputation.RepuGain,donate))
    self.param_t.donate = donate
end

function ReputationDonationPanel:FlushDonationView()
    local index = self.data:GetDonationId() or 1
    if self.List.index_item[index] then
        self.List.index_item[index]:FlushVillageView()
    end
end

--刷新捐赠数量
function ReputationDonationPanel:FlushDonationNum()
    UH.SetText(self.DonationNum,Format(Language.Reputation.DonationCount,self.data:GetTodayDonateCount()))
end

--批量捐赠
function ReputationDonationPanel:OnClickBatchDonate()
    local config = self.data:GetVillageList(self.data:GetDonationId())
    SceneLogic.Instance:AutoToNpcs(config.juanzeng_npc or 0)
    ViewMgr:CloseView(ReputationDonationView)
    ViewMgr:CloseView(BagView)
end

--捐赠
function ReputationDonationPanel:OnClickDonate()
    if self.data:GetTodayDonateCount() <= 0 then
        PublicPopupCtrl.Instance:Center(Language.Reputation.PopupTips2)
        return
    end
    if self.param_t.donate == 0 then
        PublicPopupCtrl.Instance:Center(Language.Reputation.PopupTips5)
        return
    end
    local item = ItemInfoCtrl.Instance:Data():GetItemData()
    if (item:IsEquip() or item:IsJewelry()) and (item:HadStone() or item:HasSpirit()) then
        PublicPopupCtrl.Instance:AlertTip(Language.Reputation.PopupStoneTip,function()
            if not BagData.Instance:TryFindEmptyInBagByType() then
                PublicPopupCtrl.Instance:Center(ErrorText[132])
                return
            else
                if BagData.Instance:GetEmptyNumInBag() < (self:GetEmptyNum(item:Stone()) + item:GetSpiritNum()) then
                    PublicPopupCtrl.Instance:Center(ErrorText[133])
                    return
                end
            end
            item:UnStone()
            item:RemoveAllSpirit()
            RoleCtrl.Instance:CSPrestigeDonateOp(self.data:GetDonationId(),item.column_type,item.index)
            ViewMgr:CloseView(ReputationDonationView)
        end)
        return
    end
    RoleCtrl.Instance:CSPrestigeDonateOp(self.data:GetDonationId(),item.column_type,item.index)
    ViewMgr:CloseView(ReputationDonationView)
end

function ReputationDonationPanel:GetEmptyNum(stone_list)
    local list = {}
    for k,value in pairs(stone_list) do
        if value > 0 and list[value] == nil then
            list[value] = true
        end
    end
    local num = 0
    for k,v in pairs(list) do
        num = num + 1
    end
    return num
end

----------------------------ReputationDonationItem----------------------------
ReputationDonationItem = ReputationDonationItem or DeclareMono("ReputationDonationItem", UIWidgetBaseItem)

function ReputationDonationItem:SetData(data)
    UH.SpriteName(self.Icon,data.pic)
    local min_level = (data.donate_min_level - 1) * 10
    local max_level = (data.donate_max_level - 1) * 10
    min_level = min_level == 0 and 1 or min_level
    local item = ItemInfoCtrl.Instance:Data():GetItemData()
    local level = PrestigeData.Instance:GetEquipLevel(item.item_id)
    local can_donation = level >= data.donate_min_level and level <= data.donate_max_level
    local color = can_donation and COLORSTR.Green11 or COLORSTR.Red8
    UH.SetText(self.Name,Format(Language.Reputation.DonationRange,data.name,color,min_level,max_level))
    self.Mask:SetActive(not can_donation)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushVillageView()
end

function ReputationDonationItem:FlushVillageView()
    local info,config = PrestigeData.Instance:GetVillagePrestigeNextData(self.data.village_id)
    local cur_num,need_num = info.prestige,config.need_prestige
    self.Progress:SetProgress(cur_num / need_num)
    self.Progress:SetText(Format(Language.Reputation.RepuProgress,cur_num,need_num))
    
    local _,cur_config = PrestigeData.Instance:GetVillagePrestigeData(self.data.village_id)
    UH.SetText(self.Level,cur_config.name)

    local relation = info.level > 6 and 6 or info.level
    self.Outline.effectColor = PRESTIGE_LEVEL_COLOR[relation]

    if relation > 0 then
        self.Gradient.topColor = COLORS.Yellow7
        self.Gradient.buttomColor = COLORS.Yellow1
    else
        self.Gradient.topColor=COLORS.Yellow13
        self.Gradient.buttomColor = COLORS.Yellow19
    end
end

function ReputationDonationItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected then
        PrestigeData.Instance:SetDonationId(self.data.village_id)
    end
end