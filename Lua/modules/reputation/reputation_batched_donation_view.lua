ReputationBatchedDonationView = ReputationBatchedDonationView or DeclareView("ReputationBatchedDonationView", "reputation/reputatoin_batched_donation")

VIEW_DECLARE_LEVEL(ReputationBatchedDonationView,ViewLevel.L1)

function ReputationBatchedDonationView:ReputationBatchedDonationView()
    self.Board:SetData(self:GetType(), Language.Reputation.Donation, Vector2.New(1012, 549))
end

function ReputationBatchedDonationView:CloseCallback()
    PrestigeData.Instance:InitDonateData()
end

ReputationBatchedDonationPanel = ReputationBatchedDonationPanel or DeclareMono("ReputationBatchedDonationPanel", UIWFlushPanel)

function ReputationBatchedDonationPanel:ReputationBatchedDonationPanel()
    self.data = PrestigeData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid,func = self.FlushGridList},
        {data = BagData.Instance.item_grid,func = self.FlushDonationNum},
        {data = self.data.village_prestige_info,func = self.FlushDonation},
        {data = self.data.donation_data, func = self.FlushGridList, init_call = false},
        {data = self.data.donation_point,func = self.FlushPointView,init_call = false},
    }
end

function ReputationBatchedDonationPanel:Awake()
    UIWFlushPanel.Awake(self)
    UH.SetText(self.LeftBtnName,Language.Reputation.SelectLevelTip)
    UH.SetText(self.RightBtnName,Language.Reputation.SelectColorTip)
    self.LeftList:SetData(self.data:GetLevelListData())
    self.RightList:SetData(self.data:GetColorListData())
end

function ReputationBatchedDonationPanel:OnClickColorItem()
    self.ColorParent:SetActive(not self.ColorParent.activeSelf)
    if self.LevelParent.activeSelf then
        self.LevelParent:SetActive(false)
    end
end

function ReputationBatchedDonationPanel:OnClickLevelItem()
    self.LevelParent:SetActive(not self.LevelParent.activeSelf)
    if self.ColorParent.activeSelf then
        self.ColorParent:SetActive(false)
    end
end

function ReputationBatchedDonationPanel:OnClickColorBtn(data)
    self.OutLine.enabled = data.color ~= 0
    UH.SetText(self.RightBtnName,data.name)
    self.data:SetDonationColor(data.color)
    self.ColorParent:SetActive(false)
    if data.color == 0 then
        self:FlushGridList()
    end
end

function ReputationBatchedDonationPanel:OnClickLevelBtn(data)
    UH.SetText(self.LeftBtnName,data.level == 0 and self.LimitRange or data.name)
    self.data:SetDonationLevel(data.level)
    self.LevelParent:SetActive(false)
    if data.level == 0 then
        self:FlushGridList()
    end
end

function ReputationBatchedDonationPanel:SetParentActive()
    if self.LevelParent.activeSelf then
        self.LevelParent:SetActive(false)
    end
    if self.ColorParent.activeSelf then
        self.ColorParent:SetActive(false)
    end
end

--点击捐赠
function ReputationBatchedDonationPanel:OnClickDonate()
    if #self.GridList:GetSelectGroupData() == 0 then
        PublicPopupCtrl.Instance:Center(Language.Reputation.PopupTips4)
        return
    end
    if #self.GridList:GetSelectGroupData() > self.data:GetTodayDonateCount() then
        PublicPopupCtrl.Instance:Center(Language.Reputation.PopupTips2)
        return
    end
    if self.IsMaxLevel then
        PublicPopupCtrl.Instance:Center(Language.Reputation.PopupTips6)
        return
    end
    for k, v in pairs(self.GridList:GetSelectGroupData()) do
        if v:HadStone() or v:HasSpirit() then
            PublicPopupCtrl.Instance:Center(Language.Reputation.PopupStoneTip2)
            return
        end
    end
    self:SendDonationSeq()
    self:SetParentActive()
end

function ReputationBatchedDonationPanel:GetStoneList()
    local list = {}
    for k,v in pairs(self.GridList:GetSelectGroupData()) do
        if v:HadStone() then
            table.insert(list,v)
        end
    end
    return list
end

function ReputationBatchedDonationPanel:FlushGridList()
    self.GridList:SetData(self.data:GetEquipList())
    self.GridList:CancelSelect()
    local data = self.data:GetDonationData()
    if data.level == -1 and data.color == -1 then
        self:FlushPointView()
        return
    end
    if data.level <= 0 and data.color > 0 then
        for k,item in pairs(self.GridList.index_item or {}) do
            if item and item:GetColor() == data.color then
                self.GridList:SetectItem(k,true)
            end
        end
    elseif data.level > 0 and data.color <= 0 then
        for k,item in pairs(self.GridList.index_item or {}) do
            if item and item:GetLevel() == data.level then
                self.GridList:SetectItem(k,true)
            end
        end
    elseif data.level > 0 and data.color > 0 then
        for k,item in pairs(self.GridList.index_item or {}) do
            if item and item:GetLevel() == data.level and item:GetColor() == data.color then
                self.GridList:SetectItem(k,true)
            end
        end
    else
        self.GridList:SelectAll(true)
    end
    self:FlushPointView()
end

function ReputationBatchedDonationPanel:FlushDonation()
    self:FlushProgress()
    self:FlushVillageName()
end

--刷新进度条
function ReputationBatchedDonationPanel:FlushProgress()
    local info,config = self.data:GetVillagePrestigeNextData()
    local cur_num,need_num = info.prestige,config.need_prestige
    self.Progress:SetProgress(cur_num / need_num)
    self.Progress:SetText(Format(Language.Reputation.RepuProgress,cur_num,need_num))
    self.IsMaxLevel = info.level >= 11
end

--刷新捐赠数量
function ReputationBatchedDonationPanel:FlushDonationNum()
    UH.SetText(self.DonationNum,Format(Language.Reputation.DonationCount,self.data:GetTodayDonateCount()))
end

--刷新当前捐赠等级范围
function ReputationBatchedDonationPanel:FlushVillageName()
    local _,config = PrestigeData.Instance:GetVillagePrestigeData()
    local min_level,max_level,village_name = self:GetVillageLevel()
    UH.SetText(self.VillageName,Format(Language.Reputation.DonationTip,
    village_name,config.name,min_level,max_level))
end

--刷新当前可获得声望
function ReputationBatchedDonationPanel:FlushPointView()
    local num = 0
    for k,v in pairs(self.GridList:GetSelectGroupData() or {}) do
        num = num + self.data:GetEquipCacheNum(v.index)
    end
    UH.SetText(self.Reputation,Format(Language.Reputation.RepuGain, num))
end

function ReputationBatchedDonationPanel:SendDonationSeq()
    for k, v in pairs(self.GridList:GetSelectGroupData() or {})do
        RoleCtrl.Instance:CSPrestigeDonateOp(self.data:GetVillageID(),v.column_type, v.index)
    end
    self.GridList:CancelSelect()
end

function ReputationBatchedDonationPanel:GetVillageLevel()
    local data = self.data:GetVillageList()
    local min_level = (data.donate_min_level - 1) * 10
    local max_level = (data.donate_max_level - 1) * 10
    min_level = min_level == 0 and 1 or min_level
    return min_level,max_level,data.name
end

---------------------------------ReputationBatchedDonationBtn--------------------------------------------
ReputationBatchedDonationBtn = ReputationBatchedDonationBtn or DeclareMono("ReputationBatchedDonationBtn", UIWidgetBaseItem)

function ReputationBatchedDonationBtn:SetData(data)
    UH.SetText(self.Name,data.name)
    UIWidgetBaseItem.SetData(self, data)
end

----------------------------ReputationBatchedDonationItem----------------------------
ReputationBatchedDonationItem = ReputationBatchedDonationItem or DeclareMono("ReputationBatchedDonationItem", UIWidgetBaseItem)

function ReputationBatchedDonationItem:SetData(data)
    self.Call:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
end

function ReputationBatchedDonationItem:OnClickItem()
    local val = PrestigeData.Instance.donation_point.val
    PrestigeData.Instance.donation_point.val = not val
end

function ReputationBatchedDonationItem:GetColor()
    return self.data.color
end

function ReputationBatchedDonationItem:GetLevel()
    return self.data.level
end

function ReputationBatchedDonationItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected and self.data.item_id == 0 then
        self.Selector.isOn = false
    end
end