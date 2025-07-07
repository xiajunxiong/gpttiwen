FILE_REQ("modules/serveractivity/four_season/four_season_data")
VIEW_REQ("modules/serveractivity/four_season/four_season_card_view")
VIEW_REQ("modules/serveractivity/four_season/four_season_box_view")
VIEW_REQ("modules/serveractivity/four_season/four_season_chest_view")
-- 四季花牌，四季宝箱，四季宝盒
FourSeasonCtrl = FourSeasonCtrl or BaseClass(BaseCtrl)

function FourSeasonCtrl:__init()
    if FourSeasonCtrl.Instance ~= nil then
        Debuger.LogError("[FourSeasonCtrl] attempt to create singleton twice!")
        return
    end
    FourSeasonCtrl.Instance = self
    self.data = FourSeasonData.New()
end

function FourSeasonCtrl:OnInit()
    self:RegisterRemind()
    self:RegisterAllProtocol()
end

function FourSeasonCtrl:RegisterRemind()
    -- 四季花牌
    local card_remind_func = BindTool.Bind(self.CardRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.PlanActivity.FourSeasonCard,
        self.data.card_data,
        card_remind_func
    )
    -- 四季宝盒
    local box_remind_func = BindTool.Bind(self.BoxRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.PlanActivity.FourSeasonBox,
        self.data.box_data,
        box_remind_func
    )
    -- 四季宝箱
    local chest_remind_func = BindTool.Bind(self.ChestRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.PlanActivity.FourSeasonChest,
        self.data.chest_data,
        chest_remind_func
    )
end

function FourSeasonCtrl:RegisterAllProtocol()
    self:RegisterProtocol(SCRAFourSeasonCardInfo, "OnSCRAFourSeasonCardInfo")
    self:RegisterProtocol(SCRAFourSeasonBoxInfo, "OnSCRAFourSeasonBoxInfo")
    self:RegisterProtocol(SCRAFourSeasonChestInfo, "OnSCRAFourSeasonChestInfo")
end

function FourSeasonCtrl:__delete()
    if FourSeasonCtrl.Instance == self then
        FourSeasonCtrl.Instance = nil
    end
end

function FourSeasonCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function FourSeasonCtrl:CardRemind()
    local num = self.data:CardRemind()
    return num
end

function FourSeasonCtrl:OnSCRAFourSeasonCardInfo(protocol)
    self.data:SetCardInfo(protocol)
end

function FourSeasonCtrl:BoxRemind()
    local num = self.data:BoxRemind()
    return num
end

function FourSeasonCtrl:OnSCRAFourSeasonBoxInfo(protocol)
    self.data:SetBoxInfo(protocol)
end

function FourSeasonCtrl:OnSCRAFourSeasonCardInfo(protocol)
    self.data:SetCardInfo(protocol)
end

function FourSeasonCtrl:ChestRemind()
    local num = self.data:ChestRemind()
    return num
end

function FourSeasonCtrl:OnSCRAFourSeasonChestInfo(protocol)
    self.data:SetChestInfo(protocol)
end