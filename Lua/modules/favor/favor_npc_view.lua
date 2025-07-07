-- FavorNpcView = FavorNpcView or DeclareView("FavorNpcView", "favor/favor_npc_view")
-- VIEW_DECLARE_MASK(FavorNpcView, ViewMask.BgBlock)
-- VIEW_DECLARE_LEVEL(FavorNpcView, ViewLevel.Lt)

-- function FavorNpcView: FavorNpcView()
-- end

-- function FavorNpcView: LoadCallback()
-- end

FavorNpcPanel = FavorNpcPanel or DeclareMono("FavorNpcPanel", UIWFlushPanel)

function FavorNpcPanel:FavorNpcPanel()
    self.data = FavorData.Instance
    self.language = Language.Favor
    self.data_cares = {{data = self.data.favor_data_change, func = self.FlushData, init_call = false, keys = {"val"}}}
    self:FlushData()
end


function  FavorNpcPanel:FlushData()
    local gift_times= RichTextHelper.ColorStr(self.data:TodayGiftTimes(), COLORSTR.Yellow12)
    UH.SetText(self.GiftTimes,self.language.TodayGiftTimes..gift_times)
    local action_times = RichTextHelper.ColorStr(self.data:ActionTimes(), COLORSTR.Yellow12)
    UH.SetText(self.ActionTimes,self.language.ActionTimes..action_times)
    local level=self.data:GetNpcFavorLevel()

    local level_data=self.data:GetNpcFavorLevelPro()
    UH.SetText(self.FavorLevel2,level_data.level)
    UH.SetText(self.FavorTitle,self.data:GetNpcFavorTitle(level_data.level)  )
    self.Progress:SetProgress(level_data.val / level_data.next_target)
    self.Progress:SetText(string.format("%s/%s", level_data.val,  level_data.next_target))    
end

function FavorNpcPanel:OpenGiftPanel()
    self.GiftPanel:SetObjActive(true);
    self.BtnPanel:SetObjActive(false);
    self.GiftPanel:FlushAll();
end

function FavorNpcPanel:OpenBanquetPanel()
    self.BanquetPanel:SetObjActive(true);
    self.BtnPanel:SetObjActive(false);
    self.BanquetPanel:FlushAll();
end

function FavorNpcPanel:ReturnPanel()
    if self.GiftPanel:GetObjActive() then
        self.GiftPanel:SetObjActive(false);
        self.BtnPanel:SetObjActive(true);
    else
        if self.BanquetPanel:GetObjActive() then
            self.BanquetPanel:SetObjActive(false);
            self.BtnPanel:SetObjActive(true);
        else
           FavorData.Instance.favor_data_change.is_show = false
        end
    end
end

function FavorNpcPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function FavorNpcPanel:OnFetchReward()
    local npc_cfg = self.data:GetCurFavorNpcConfig()
    -- if FavorData.Instance:IsCanNpcFavorReward() then
    --     local npc_index = npc_cfg.seq
    --     local progress_data = FavorData.Instance:GetNpcFavorLevelPro()
    --     FavorCtrl.Instance:SendFavorReq(FAVOR_NPC_OP_TYPE.FETCH_FAVORE_REWARD, npc_index, progress_data.level - 1)
    -- else
        ViewMgr:OpenView(FavorNpcRewardView, {cfg_npc = npc_cfg})
    -- end
end


function FavorNpcPanel:OnTipTili()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[411].desc)
end

function FavorNpcPanel:OnTip()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[410].desc)
end

--NpcBtnPanel
NpcBtnPanel = NpcBtnPanel or DeclareMono("NpcBtnPanel", UIWFlushPanel)

function NpcBtnPanel:NpcBtnPanel()
    self.data = FavorData.Instance
    self.language=Language.Favor
    self:InitData()
end

function NpcBtnPanel:InitData()
    self.data_cares = {{data = self.data.favor_data_change, func = self.FlushData, init_call = false,keys={"val"}}}
    self:FlushData()
end

function NpcBtnPanel:FlushData()
    UH.SetText(self.NpcGiftTimes, self.language.NpcGiftTimes .. self.data:NpcTodayGiftTimes())
    UH.SetText(self.NpcBanquetTimes,self.language.NpcBanquetTimes.. self.data:NpcTodayBanquetTimes())
end

--NpcGiftPanel
NpcGiftPanel = NpcGiftPanel or DeclareMono("NpcGiftPanel", UIWFlushPanel)

function NpcGiftPanel:NpcGiftPanel()
    self.data = FavorData.Instance
    self.language = Language.Favor
    self.data:InitNpcSelectGiftData()
    self:InitData()
end

function NpcGiftPanel:InitData()
    self.data_cares = {
        {data = self.data.favor_data_change, func = self.FlushData, init_call = false, keys = {"val"}},
        {data = self.data.favor_gift_select_data, func = self.FlushFavorData, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushList, init_call = false},
    }
    self:FlushAll()
end

function NpcGiftPanel:FlushAll()
    self:FlushData()
    self:FlushList()
end

function NpcGiftPanel:FlushData()
    local cfg = self.data:GetCurFavorNpcConfig()
    -- local npc_id = NpcHelper.NpcSeqToId(cfg.npc_seq)
    -- local npc_config = NpcHelper.NpcCo(npc_id)
    -- local type_desc=RichTextHelper.ColorStr(self.language.GiftType[cfg.like_type],COLORSTR.Yellow16)
    UH.SetText(self.Desc, cfg.like_desc)
    local level = self.data:GetNpcFavorLevel()
    UH.SetText(self.FavorLevel, level)
    self:FlushFavorData()
end


function  NpcGiftPanel:FlushList()
    self.List:SetData(self.data:NpcGiftList())
end

function NpcGiftPanel:FlushFavorData()
    UH.SetText(self.Favorability, "+".. self.data:GetNpcSelectGiftValue())
    local progress_data = self.data:GetNpcProGress()
    local val=progress_data.val+self.data:GetNpcSelectGiftValue()
    local target=progress_data.target
    self.Progress:SetProgress(val / target)
    self.Progress:SetText(string.format("%s/%s", val, target))
    local select_times=self.data:GetNpcSelectGiftTimes()
    UH.SetText(self.GiftTimes, self.language.LastGiftTime .. self.data:NpcTodayGiftTimes())
    local cost_tili=self.data:NpcGiftCost()*select_times
    UH.SetText(self.CostTiLi,self.language.CostTiLi .. cost_tili)
end

function NpcGiftPanel:OnClickGift()
    local select_times = self.data:GetNpcSelectGiftTimes()
    if select_times == 0 then
        PublicPopupCtrl.Instance:Center(self.language.NoGiftTip)
        return
    end
    local cur_tili = self.data:ActionTimes()
    local cost_tili = self.data:NpcGiftCost() * select_times
    if cost_tili > cur_tili then
        PublicPopupCtrl.Instance:Center(self.language.NoTiLiTip)
        return
    end
    if self.data:IsMaxLevel() then
        PublicPopupCtrl.Instance:Center(self.language.MaxLevelTip)
        return
    end
    self.data:NpcGift()
end

--item
NpcFavorGiftItem = NpcFavorGiftItem or DeclareMono("NpcFavorGiftItem", UIWidgetBaseItem)

function NpcFavorGiftItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.select_num=FavorData.Instance:NpcSelectGiftNum(data.item_id) --self.select_num and self.select_num or 0
    self:FlushNumDesc()
    -- self.Cell:SetData(Item.Create(data.item_data))
    self.Cell:SetData(Item.Create({item_id=data.item_id,num=data.num}))
    self.ImgFavor:SetActive(data.is_favor==1)
end

function NpcFavorGiftItem:AddGiftNum()
    if self.select_num < self.data.num then
        local cur_favor_value = FavorData.Instance:GetFavorInfo().favor_value
        local select_favor_value = FavorData.Instance:GetNpcSelectGiftValue()
        local next_level_value=FavorData.Instance:GetNpcProGress().target
        if select_favor_value >= next_level_value-cur_favor_value then
            PublicPopupCtrl.Instance:Center(Language.Favor.MaxFavorabilityTip)
            return
        end
        local select_times = FavorData.Instance:GetNpcSelectGiftTimes()
        local max_times = FavorData.Instance:NpcTodayGiftTimes()
        if select_times >= max_times then
            PublicPopupCtrl.Instance:Center(Language.Favor.GiftMaxTip)
            return
        end
        self.select_num = self.select_num + 1
        self:FlushNumDesc()
        FavorData.Instance:NpcSelectGiftValue(self.data.item_id,self.select_num,self.data.mood_value)
    end
end

function NpcFavorGiftItem:OnClickReduce()
    if self.select_num > 0 then
        self.select_num = self.select_num - 1
        self:FlushNumDesc()
        FavorData.Instance:NpcSelectGiftValue(self.data.item_id, self.select_num, -self.data.mood_value)
    end
end


function NpcFavorGiftItem:FlushNumDesc()
    local num_desc = self.select_num ~= 0  and self.select_num .. "/" .. self.data.num or self.data.num
    UH.SetText(self.Num, num_desc)
    self.BtnReduce:SetActive( self.select_num ~= 0)
end


--NpcBanquetPanel
NpcBanquetPanel = NpcBanquetPanel or DeclareMono("NpcBanquetPanel", UIWFlushPanel)

function NpcBanquetPanel:NpcBanquetPanel()
    self.data = FavorData.Instance
    self.language = Language.Favor
    self:InitData()
end

function  NpcBanquetPanel:InitData()
    self.banquet_list=self.data:GetNpcBanquet()
    UH.SetText(self.FeastFavorNum1,"+"..self.banquet_list[1].mood_value)
    UH.SetText(self.FeastFavorNum2,"+"..self.banquet_list[2].mood_value)
    UH.SetText(self.FeastName1,self.banquet_list[1].feast_name)
    UH.SetText(self.FeastName2,self.banquet_list[2].feast_name)
    UH.SetText(self.CostTiLi,self.language.CostTiLi..self.data: NpcBanquetTiliCost())
    self.select_type=1
    self.BtnFeast1.isOn = true
    self:FlushAll()
    self.data_cares = {
    {data = self.data.favor_data_change, func = self.FlushData, init_call = false, keys = {"val"}},
    {data = BagData.Instance.item_grid, func = self.FlushList, init_call = false},}
end

function NpcBanquetPanel:FlushAll()
    self:FlushList()
    self:FlushData()
end

function  NpcBanquetPanel:FlushData()
    UH.SetText(self.BanquetTimes,self.language.BanquetTimes..self.data:NpcTodayBanquetTimes())
end

function NpcBanquetPanel:OnChangeType(type)
    self.select_type=type
    self["BtnFeast"..type].isOn = true
    self:FlushList()
end

function NpcBanquetPanel:FlushList()
    local cfg = self.banquet_list[self.select_type]
    self.List:SetData(self.data:NpcBanquetList(cfg))
end

function NpcBanquetPanel:OnClickBanquet()
    if self.data:NpcBanquetTiliCost() > self.data:ActionTimes() then
        PublicPopupCtrl.Instance:Center(self.language.NoTiLiTip)
        return
    end
    if self.data:IsMaxLevel() then
        PublicPopupCtrl.Instance:Center(self.language.MaxLevelTip)
        return
    end
    local cfg = self.banquet_list[self.select_type]
    local list = {}
    for i = 1, 6 do
        if cfg["food_" .. i] ~= 0 and cfg["food_" .. i] ~= "" then
            local has_num = Item.GetNum(cfg["food_" .. i])
            local need_num = cfg["num_" .. i]
            if tonumber (need_num) > has_num then
                PublicPopupCtrl.Instance:Center(self.language.CaiPinBuZu)
                return
            end
        end
    end
    local npc_cfg = self.data:GetCurFavorNpcConfig()
    local npc_index = npc_cfg.seq
    self.data.old_val = self.data:GetFavorInfo().favor_value
    if self.data:NpcTodayBanquetTimes()>0 then
        FavorCtrl.Instance:SendFavorReq(FAVOR_NPC_OP_TYPE.BANQUET, npc_index, cfg.seq)
    else
        PublicPopupCtrl.Instance:Center(self.language.NoBanquetTimes)
    end
end

function NpcBanquetPanel:OnGoToCook()
    local cfg = self.banquet_list[self.select_type]
    LiveData.Instance:JumpItem(cfg.food_1, true,cfg.seq)
    ViewMgr:OpenViewByKey(Mod.Live.Main)
end

--item
NpcBanquetItem = NpcBanquetItem or DeclareMono("NpcBanquetItem", UIWidgetBaseItem)

function NpcBanquetItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local has_num=Item.GetNum(data.item_id)
    UH.SetText(self.Num,has_num.."/"..data.num)
    self.Cell:SetData(Item.Create({item_id=data.item_id}))
end


