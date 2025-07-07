GodBeastGraceView = GodBeastGraceView or DeclareMono("GodBeastGraceView", UIWFlushPanel)

function GodBeastGraceView:GodBeastGraceView()
    self.Data = RewardData.Instance
    self.data = self.Data.god_beast_grace_data
    self.language = Language.Reward.GodBeastGrace
    self.Data:GodBeastGraceDaySelectPetSeq(0)
    self.consume_id = Config.shenshouenze_auto.other[1].consume_item
    self.one_num = Config.shenshouenze_auto.other[1].num
    self.has_num = 0
    self.data_cares = {
        {data = self.Data.god_beast_grace_data, func = self.FlushAll, keys = {"info"}, init_call = false},
        {data = self.Data.god_beast_grace_data, func = self.FlushItem, keys = {"remind_flag"}, init_call = false},
        {data = self.Data.god_beast_grace_data, func = self.StartRoller, keys = {"result"}, init_call = false},
    }
    self.is_first = true

    self.min_times = 5
    self.max_times = 10
    self.now_ten_times = 10
end

function GodBeastGraceView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    if  self.is_first_enable then
        self.AnimList:Play("pet_list_end")
    end
    if not self.is_first_enable then
        self.is_first_enable = true
    end
end

function GodBeastGraceView:Awake()
    UIWFlushPanel.Awake(self)
    self.base_cfg = self.Data:GodBeastGraceBaseCfg()
    self.pet_cfg = self.Data:GodBeastGracePetCfg()
    local icon_id = Item.GetIconId(self.consume_id)
    UH.SetIcon(self.Icon1, icon_id, ICON_TYPE.ITEM)
    UH.SetIcon(self.Icon2, icon_id, ICON_TYPE.ITEM)
    UH.SetIcon(self.Icon3, icon_id, ICON_TYPE.ITEM)
    UH.SetText(self.TxtSpendOne, "x" .. self.one_num)
    self.Data:GodBeastGraceClearRemind(PrefKeys.GodBeastGraceAct())
    self.SelectList:SetCompleteCallBack(function()
        self.AnimList:Play("pet_list")
    end)
    self:FlushAll()
end

function GodBeastGraceView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    ViewMgr:CloseView(GodBeastGraceSpendView)
    ViewMgr:CloseView(GodBeastGraceRecordView)
end

function GodBeastGraceView:FlushAll()
    self.ObjSelectPet:SetActive(self.data.info.is_select == 0)
    if self.data.info.is_select == 0 then
        self.SelectList:SetData({})
        self.SelectList:SetDataList(self.pet_cfg)
    else
        self.Data:GodBeastGraceClearRemind(PrefKeys.GodBeastGraceSelected())
        if self.is_first then
            self:SetRollerData()
            self.is_first = false
        end
        self.pet_info = self.pet_cfg[self.data.info.select_seq + 1]
        self.Cell:SetData(Item.Create(self.pet_info.reward_item))
        self:FlushItem()
    end
end

function GodBeastGraceView:FlushItem()
    if TableIsEmpty(self.pet_info) then
        return
    end
    self.has_num = Item.GetNum(self.consume_id)
    UH.SetText(self.TxtHas, self.has_num)
    local can_lottery_count = self.pet_info.times - self.data.lottery_times

    self.now_ten_times = math.min(can_lottery_count, math.floor(self.has_num / self.one_num))
    self.now_ten_times = self.now_ten_times > self.max_times and self.max_times or self.now_ten_times
    self.now_ten_times = self.now_ten_times < self.min_times and self.min_times or self.now_ten_times

    self.ObjBtnOne:SetActive(true)
    self.ObjBtnTen:SetActive(can_lottery_count >= self.min_times)
    UH.SetText(self.TxtTen, Format(self.language.Lottery, DataHelper.GetDaXie(self.now_ten_times)))
    UH.SetText(self.TxtSpendTen, "x" .. self.one_num * self.now_ten_times)

    self.RemindOne:SetNum(can_lottery_count >= 1 and self.has_num >= self.one_num and 1 or 0)
    self.RemindTen:SetNum(can_lottery_count >= self.min_times and self.has_num >= (self.one_num * self.min_times) and 1 or 0)
    self.RemindPet:SetNum(can_lottery_count == 0 and 1 or 0)
    UH.SetText(self.TxtTimes, can_lottery_count)
end

function GodBeastGraceView:OnClickSelect()
    RewardCtrl.Instance:GodBeastGraceSend(GodBeastGraceReq.SetPet, self.Data:GodBeastGraceDaySelectPetSeq())
end

function GodBeastGraceView:OnResetClick()
    local can_lottery_count = self.pet_info.times - self.data.lottery_times
    if can_lottery_count > 0 then
        self:SetTipActive(true)
    else
        PublicPopupCtrl.Instance:Center(self.language.LotteryTip)
    end
end

function GodBeastGraceView:SetTipActive(value)
    self.ObjTip:SetActive(value)
end

function GodBeastGraceView:OnConfirmClick()
    RewardCtrl.Instance:GodBeastGraceSend(GodBeastGraceReq.Reset)
    self:SetTipActive(false)
end

function GodBeastGraceView:SetRollerData()
    local roller_data = {}
    roller_data.config = {}
    for k, v in pairs(self.base_cfg) do
        roller_data.config[k] = {}
        roller_data.config[k].info = v
        roller_data.config[k].reward = v.reward_item
    end
    roller_data.end_func = BindTool.Bind(self.RollerEnd, self)
    roller_data.rolling_func = BindTool.Bind(self.Rolling, self)
    roller_data.now_select = 1
    self.Roller:SetData(roller_data)
end

function GodBeastGraceView:OnLotteryClick(index)
    local has_num = Item.GetNum(self.consume_id)
    local need_num = index * self.one_num
    if index == 10 then
        need_num = self.one_num * self.now_ten_times
    end
    if has_num >= need_num then
        local can_lottery_count = self.pet_info.times - self.data.lottery_times
        if can_lottery_count > 0 then
            if index == 1 then
                self.Data:SetRewardBlock(true)
                RewardCtrl.Instance:GodBeastGraceSend(GodBeastGraceReq.Lottery, index)
            else
                RewardCtrl.Instance:GodBeastGraceSend(GodBeastGraceReq.Lottery, self.now_ten_times)
            end
        else
            PublicPopupCtrl.Instance:Center(self.language.LotteryTip)
        end
    else
        self:OnSpendClick()
    end
end

function GodBeastGraceView:StartRoller()
    if self.data.now_lottery_times == 1 then
        self.Data:SetRewardBlock(true)
        self.Roller:PlayRoller(self.data.result[1].seq + 1)
    elseif self.data.now_lottery_times > 1 then
        for i = 1, self.data.now_lottery_times do
            if not TableIsEmpty(self.data.result[i]) then
                if self.data.result[i].is_mall == 0 then
                    local index = self.data.result[i].seq + 1
                    PublicPopupCtrl.Instance:ShowCustom({self.base_cfg[index].reward_item})
                end
            end
        end
        self:FlushItem()
    end
end

function GodBeastGraceView:RollerEnd()
    self.Data:SetRewardBlock(false)
    if self.data.result[1].is_mall == 0 then
        local index = self.data.result[1].seq + 1
        PublicPopupCtrl.Instance:ShowCustom({self.base_cfg[index].reward_item})
    end
    self:FlushItem()
end

function GodBeastGraceView:Rolling()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function GodBeastGraceView:OpenGetWay()
    ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = self.consume_id})})
end

function GodBeastGraceView:OnPetClick()
    if self.data.info.is_select == 1 and self.data.lottery_times >= self.pet_info.times then
        RewardCtrl.Instance:GodBeastGraceSend(GodBeastGraceReq.PetReward)
    else
        PetCtrl.Instance:OpenPetDetailView(self.pet_info.reward_item.item_id)
    end
end

function GodBeastGraceView:OnSpendClick()
    ViewMgr:OpenView(GodBeastGraceSpendView)
end

function GodBeastGraceView:OnRecordClick()
    ViewMgr:OpenView(GodBeastGraceRecordView)
end

----------------------------GodBeastGraceSelectItem----------------------------
GodBeastGraceSelectItem = GodBeastGraceSelectItem or DeclareMono("GodBeastGraceSelectItem", UIWidgetBaseItem)
function GodBeastGraceSelectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data.reward_item)
    UH.SetText(self.TxtName, item:Name())
    self.Cell:SetData(item)
    self.ObjRoot.name = Format("GodBeastGraceSelectItem%s", data.seq + 1)
end

function GodBeastGraceSelectItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        RewardData.Instance:GodBeastGraceDaySelectPetSeq(self.data.seq)
    end
end

function GodBeastGraceSelectItem:Click()
    self.ToggleEvent:Trigger()
end

function GodBeastGraceSelectItem:OnLookClick()
    PetCtrl.Instance:OpenPetDetailView(self.data.reward_item.item_id)
end

----------------------------GodBeastGraceRollerItem----------------------------
GodBeastGraceRollerItem = GodBeastGraceRollerItem or DeclareMono("GodBeastGraceRollerItem", UIWRollerItem)
function GodBeastGraceRollerItem:SetData(data)
    UIWRollerItem.SetData(self, data)
    UH.SetText(self.TxtRate, data.info.show_rate)
    self.ObjRace:SetActive(data.info.mark == 1)
end

------------------------------------------------------------------------------
----------------------------GodBeastGraceSpendView----------------------------
GodBeastGraceSpendView = GodBeastGraceSpendView or DeclareView("GodBeastGraceSpendView", "reward/god_beast_grace_spend")
VIEW_DECLARE_MASK(GodBeastGraceSpendView, ViewMask.BgBlock)
function GodBeastGraceSpendView:GodBeastGraceSpendView()
end

function GodBeastGraceSpendView:OnCloseClick()
    ViewMgr:CloseView(GodBeastGraceSpendView)
end

----------------------------GodBeastGraceSpendPanel----------------------------
GodBeastGraceSpendPanel = GodBeastGraceSpendPanel or DeclareMono("GodBeastGraceSpendPanel", UIWFlushPanel)
function GodBeastGraceSpendPanel:GodBeastGraceSpendPanel()
    self.Data = RewardData.Instance
    self.data = self.Data.god_beast_grace_data
    self.language = Language.Reward.GodBeastGrace
    self.buy_cfg = self.Data:GodBeastGraceBuyCfg()
    self.data_cares = {
        {data = self.Data.god_beast_grace_data, func = self.FlushAll, keys = {"info"}},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end

function GodBeastGraceSpendPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function GodBeastGraceSpendPanel:FlushAll()
    table.sort(self.buy_cfg, DataHelper.WeightSort("seq", function(data)
        return self.Data:GodBeastGraceLimitTimes(data) == 0 and 100 or 0
    end))
    self.List:SetData(self.buy_cfg)
end

function GodBeastGraceSpendPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function GodBeastGraceSpendPanel:OnBuyClick(data)
    if self.Data:GodBeastGraceLimitTimes(data) == 0 then
        PublicPopupCtrl.Instance:Center(self.language.BuyTip)
        return
    end
    -- 防止手速快过协议
    if data.price_type == 3 and self.now_buy_item then
        return
    end
    -- 3是直购
    if data.price_type == 3 then
        -- 记录当前购买道具，用于校验
        self.now_buy_item = data
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_GOD_BEAST_GRACE, data.price / 10, ACTIVITY_TYPE.RAND_GOD_BEAST_GRACE, GodBeastGraceReq.RMBBuy, data.seq)
    else
        if MallCtrl.IsNotCurrency(data.price_type, data.price) then
            RewardCtrl.Instance:GodBeastGraceSend(GodBeastGraceReq.OtherBuy, data.seq)
        end
    end
end

function GodBeastGraceSpendPanel:SendBuy()
    if self.now_buy_item == nil then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_GOD_BEAST_GRACE, self.now_buy_item.price)
    self.now_buy_item = nil
end

----------------------------GodBeastGraceSpendItem----------------------------
GodBeastGraceSpendItem = GodBeastGraceSpendItem or DeclareMono("GodBeastGraceSpendItem", UIWidgetBaseItem)
function GodBeastGraceSpendItem:GodBeastGraceSpendItem()
    self.Data = RewardData.Instance
    self.language = Language.Reward.GodBeastGrace
end

function GodBeastGraceSpendItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data.reward_item)
    UH.SetText(self.TxtName, item:Name())
    local limit_times = self.Data:GodBeastGraceLimitTimes(data)
    local color = limit_times == 0 and COLORSTR.Red1 or COLORSTR.Green9
    UH.SetText(self.TxtLimit, Format(self.language.limit[data.limit_type], color, limit_times, data.limit_times))
    UH.SetText(self.TxtValue, data.value)
    UH.SetActGold(self.Icon, data.price_type, data.cfg_or)
    UH.SetText(self.TxtPrice, data.price_type == 3 and data.price / 10 or data.price)
    self.Cell:SetData(item)
    self.IHandler.Interactable = limit_times > 0
end

------------------------------------------------------------------------------
----------------------------GodBeastGraceRecordView----------------------------
GodBeastGraceRecordView = GodBeastGraceRecordView or DeclareView("GodBeastGraceRecordView", "reward/god_beast_grace_record")
VIEW_DECLARE_MASK(GodBeastGraceRecordView, ViewMask.BgBlock)
function GodBeastGraceRecordView:GodBeastGraceRecordView()
end

function GodBeastGraceRecordView:OnCloseClick()
    ViewMgr:CloseView(GodBeastGraceRecordView)
end

----------------------------GodBeastGraceRecordPanel----------------------------
GodBeastGraceRecordPanel = GodBeastGraceRecordPanel or DeclareMono("GodBeastGraceRecordPanel", UIWFlushPanel)
function GodBeastGraceRecordPanel:GodBeastGraceRecordPanel()
    self.Data = RewardData.Instance
    self.data = self.Data.god_beast_grace_data
    self.language = Language.Reward.GodBeastGrace
    self.data_cares = {
        {data = self.Data.god_beast_grace_data, func = self.FlushAll, keys = {"record"}, init_call = false},
    }
end

function GodBeastGraceRecordPanel:Awake()
    UIWFlushPanel.Awake(self)
    RewardCtrl.Instance:GodBeastGraceSend(GodBeastGraceReq.Record)
end

function GodBeastGraceRecordPanel:FlushAll()
    if not TableIsEmpty(self.data.record) then
        local record = {}
        for k, v in pairs(self.data.record) do
            if v.item_id > 0 then
                table.insert(record, v)
            end
        end
        table.sort(record, DataHelper.WeightSort("time", function(data)
            return data.times
        end))
        local record2 = DataHelper.ReverseList(record)
        self.List:SetData(record2)
    else
        self.List:SetData({})
    end
end

----------------------------GodBeastGraceRecordItem----------------------------
GodBeastGraceRecordItem = GodBeastGraceRecordItem or DeclareMono("GodBeastGraceRecordItem", UIWidgetBaseItem)
function GodBeastGraceRecordItem:GodBeastGraceRecordItem()
    self.Data = RewardData.Instance
    self.language = Language.Reward.GodBeastGrace
end

function GodBeastGraceRecordItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local add_zero = function(num)
        num = tonumber(num)
        return num < 10 and "0" .. num or num
    end
    local t_time = os.date("*t", data.time)
    local year = add_zero(t_time.year % 100)
    local month = add_zero(t_time.month)
    local day = add_zero(t_time.day)
    local hour = add_zero(t_time.hour)
    local sec = add_zero(t_time.sec)
    local color = Item.GetColor(data.item_id)
    local name = Item.GetName(data.item_id)
    local str = Format(self.language.Record, year, month, day, hour, sec, QualityColorStr[color], name, data.num)
    UH.SetText(self.Txt, str)
end