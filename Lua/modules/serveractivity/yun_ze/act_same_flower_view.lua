ActSameFlowerView = ActSameFlowerView or DeclareView("ActSameFlowerView", "serveractivity/yun_ze/act_same_flower_view", Mod.YunZe.SameFlower)
VIEW_DECLARE_LEVEL(ActSameFlowerView, ViewLevel.L0)
ActSameFlowerView.Oper = {
    Info = 0,       -- 请求信息
    Play = 1,       -- 请求抽奖
    Select = 2,     -- 请求锁定奖励  p1 奖励编号
    Buy = 3,        -- 请求购买
}
function ActSameFlowerView:OnCloseClick()
    ViewMgr:CloseView(ActSameFlowerView)
end


ActSameFlowerPanel = ActSameFlowerPanel or DeclareMono("ActSameFlowerPanel", UIWFlushPanel)
function ActSameFlowerPanel:ActSameFlowerPanel()
    self.data = YunZeData.Instance
    self.data_cares = {
        {data = self.data.same_flower_data, func = self.FlushReward, init_call = false, keys = {"info"}},
        {data = self.data.same_flower_data, func = self.FlushPlay, init_call = false, keys = {"result"}},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushKey, init_call = false}
    }
    self.cfg = Config.fenxiangtonghuali_auto
    self.other_cfg = self.cfg.other[1]
    self.model_index = 1
    self.key_item_id = self.other_cfg.reward_item[0].item_id
    self.key_item_name = Item.GetName(self.key_item_id)
    self.act_type = ACTIVITY_TYPE.RAND_SAME_FLOWER
end

function ActSameFlowerPanel:Awake()
    UIWFlushPanel.Awake(self)

    -- 红点
    local key = PrefKeys.ActivityIsRemind(RoleData.Instance:GetRoleId(), self.act_type)
    local t0 = TimeManager.FormatUnixTime2Date(TimeCtrl.Instance:GetServerTime())
    UnityPlayerPrefs.SetString(key, Format(Language.SameFlower.RemindValue, ActivityData.Instance:GetActivityStatusInfo(self.act_type).param_1, t0.year, t0.month, t0.day))
    self.data.same_flower_data.remind = not self.data.same_flower_data.remind
   -- self.data:RemindNotify()

    self.TestBtn:SetActive(false) --DEBUG_MODE

    for i = 1, self.Items:Length() do
        local list_data = self:GetItemListData()
        local data = {list_data = list_data}
        self.Items[i]:SetData(data)
    end
    UH.SetText(self.Desc, self.other_cfg.desc)
    local key_num = ColorStr(tostring(self.other_cfg.item_num), COLORSTR.Purple5)
    local key_name = ColorStr(self.key_item_name, COLORSTR.Purple5)
    UH.SetText(self.KeyTip, string.format(Language.SameFlower.Txt1, key_num, key_name))
    UH.SetIcon(self.KeyIcon, Item.GetIconId(self.key_item_id))
    self.Timer:CloseCountDownTime()
    self.Timer:CreateActTime(ACTIVITY_TYPE.RAND_SAME_FLOWER, TimeType.Type_Time_3, Language.SameFlower.Txt2)

    local act_name = ActivityRandData.GetName(ACTIVITY_TYPE.RAND_SAME_FLOWER)
    local char_list = RichTextHelper.ParseUTF8Str(act_name)
    local title1 = char_list[1]
    local title2 = ""
    for i = 2, #char_list do
        title2 = title2 .. char_list[i]
    end
    UH.SetText(self.Title1, title1)
    UH.SetText(self.Title2, title2)
    self:FlushPanel()
    self.time_ht = TimeHelper:AddRunTimer(function ()
        self:FlushModel()
    end, 3)
end

function ActSameFlowerPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.Timer:CloseCountDownTime()
    self:ShowResult()
    ActSameFlowerPanel.playing = false
    self.data:SetSameFlowerResult(-1)
    TimeHelper:CancelTimer(self.time_ht)
end

function ActSameFlowerPanel:FlushPanel()
    self:FlushSel()
    self:FlushReward()
    self:FlushKey()
end

function ActSameFlowerPanel:FlushKey()
    local num = Item.GetNum(self.key_item_id)
    UH.SetText(self.KeyNum, num)
    local btn_txt = num < self.other_cfg.item_num and string.format(Language.SameFlower.Txt11, self.key_item_name) or Language.SameFlower.Txt12
    UH.SetText(self.PlayBtnText, btn_txt)
end

function ActSameFlowerPanel:FlushModel()
    local model_list = self:GetModelList()
    local item = model_list[self.model_index]
    self.model_index = self.model_index > #model_list and 1 or self.model_index
    if item then
        UH.SetText(self.ModelTip, string.format(Language.SameFlower.Txt3,item:QuaName()))
        self.UIModel:SetData({item_id = item})
        self.model_index = self.model_index + 1
    end
end

function ActSameFlowerPanel:GetModelList()
    if not self.model_list then
        self.model_list = {}
        local cfg = self.data:GetSameFlowerRewardCfg()
        for i,v in pairs(cfg) do
            if v.show3 == 1 then
                local item_id = v.reward_item.item_id
                if Item.GetType(item_id) == ItemType.Gift then
                    local item_list,config = BagData.Instance:GiftItemList(item_id)
                    for _,value in pairs(item_list or {}) do
                        local item = Item.Create({item_id = value.item_id})
                        table.insert(self.model_list, item)
                    end
                else
                    local item = Item.Create(v.reward_item)
                    table.insert(self.model_list, item)
                end
            end
        end
    end
    return self.model_list
end

function ActSameFlowerPanel:FlushSel()
    if self.sel_flower_index then
        --self.SelEffect:SetActive(false)
        UH.SpriteName(self.SelFlower, string.format("Hua%s", self.sel_flower_index))
    else
        --self.SelEffect:SetActive(true)
        UH.SpriteName(self.SelFlower, string.format("JiaHaoAnNiu"))
    end
end

function ActSameFlowerPanel:FlushReward()
    for i = 1, self.RewardList:Length() do
        self.RewardList[i]:SetData({reward_type = i})
    end
end

function ActSameFlowerPanel:GetItemListData()
    local list_data = {}
    for i = 1,3 do
        list_data = AndList(list_data, DataHelper.GetRamdonList(1,3))
    end
    return list_data
end

function ActSameFlowerPanel:OnPlayClick()
    if ActSameFlowerPanel.playing then
        PublicPopupCtrl.Instance:Center(Language.SameFlower.Txt4)
        return
    end
    local key_num = Item.GetNum(self.key_item_id)
    if key_num < self.other_cfg.item_num then
        PublicPopupCtrl.Instance:Error(string.format(Language.SameFlower.Txt5,self.other_cfg.item_num, Item.GetQuaName(self.key_item_id)))
        ViewMgr:OpenView(ActSameFlowerBuyKeyView)
        return
    end
    if not self.sel_flower_index then
        PublicPopupCtrl.Instance:Error(Language.SameFlower.Txt6)
        return
    end
    if not self:IsAllSel() then
        PublicPopupCtrl.Instance:Error(Language.SameFlower.Txt13)
        return
    end
    ActSameFlowerPanel.playing = true
    AppearanceCtrl.Instance.is_shield_appear_show = true    -- 拦截外观显示
    AchieveCtrl.Instance:DeLayCourseRemind()                -- 拦截成就显示
    self:CheckFashionItem()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SAME_FLOWER, ActSameFlowerView.Oper.Play)
end

function ActSameFlowerPanel:IsAllSel()
    local num = 0
    for i,v in pairs(self.data.same_flower_data.info.lock_rewards or {}) do
        if i > 0 and v > 0 then
            num = num + 1
        end
    end
    return num >= 3
end

-- 抽奖测试
function ActSameFlowerPanel:OnTestPlayClick()
    if not self.sel_flower_index then
        PublicPopupCtrl.Instance:Error(Language.SameFlower.Txt6)
        return
    end
    self:Play(2)
end

function ActSameFlowerPanel:FlushPlay()
    self:Play()
end

function ActSameFlowerPanel:Play(result_reward)
    local result_reward = result_reward or self.data:GetSameFlowerResult()
    if result_reward < 1 then
        return
    end
    self.rusult_list = self:GetResultList(result_reward)
    ActSameFlowerPanel.playing = true
    self:PlayFunc()
end

-- index == 1 则是依次抽奖
function ActSameFlowerPanel:PlayFunc(index)
    if not index then
        for i = 1, 3 do
            if i == 3 then
                self.Items[i]:Play(self.rusult_list[i], i, BindTool.Bind(self.EndFunc, self))
            else
                self.Items[i]:Play(self.rusult_list[i], i)
            end
        end
        return
    end
    if index > 3 then
        self:EndFunc()
        return
    end
    self.Items[index]:Play(self.rusult_list[index], index, BindTool.Bind(self.PlayFunc, self, index + 1))
end

function ActSameFlowerPanel:EndFunc()
    -- if self.sel_flower_mono then
    --     self.sel_flower_mono.Selector.isOn = false
    -- end
    -- self.sel_flower_index = nil
    ActSameFlowerPanel.playing = false
    self:FlushSel()
    self:ShowResult()
    AppearanceCtrl.Instance:ReleaseShieldParam()
    AchieveCtrl.Instance:CloseDelayCourseRemind()
    self.data:SetSameFlowerResult(-1)
end

-- 抽之前判断下时装奖励是否激活了
function ActSameFlowerPanel:CheckFashionItem()
    local cfg = self.data:GetSameFlowerRewardCfg() or {}
    self.check_list = {}
    for i,v in pairs(cfg) do
        local item_id = v.reward_item.item_id
        if Item.GetShowType(item_id) == ShowType.Fashion then
            local item_cfg = Item.GetConfig(item_id)
            local fashion_id = item_cfg.param1
            local info = IllusionData.Instance:GetFashionInfoById(fashion_id) or {}
            if info.level and info.level >= 1 then
                self.check_list[item_id] = info
            end
        end
    end
end

function ActSameFlowerPanel:ShowResult()
    if self.data:GetSameFlowerResult() > 0 then
        local item_list = self:GetResultReward()
        ViewMgr:OpenView(GiftOpenView,{item_list = item_list,gift_type = GIFT_OPEN_TYPE.SPECIAL})
        for i,v in pairs(item_list) do
            if self.check_list[v.item_id] then
                local info = self.check_list[v.item_id]
                PublicPopupCtrl.Instance:Center(Language.SameFlower.Txt14)
                local item = Item.Create({item_id = info.info.decompose_item, num = info.info.decompose_num})
                PublicPopupCtrl.Instance:ShowCustom({item})
                ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item, num = info.info.decompose_num}}
            end
        end
    end
end

function ActSameFlowerPanel:GetResultReward(result_reward)
    result_reward = result_reward or self.data:GetSameFlowerResult()
    if result_reward < 1 then
        return {}
    end
    local item_list = {}
    local len = (result_reward - 1) * 3 + 1
    local cfg = self.data:GetSameFlowerRewardCfg()
    for i = len, #cfg do
        if self.data:GetSameFlowerRewardLock(i) or len == 10 then
            local v = cfg[i]
            table.insert(item_list, Item.Create(v.reward_item))
        end
    end
    return item_list
end

function ActSameFlowerPanel:GetResultList(result_reward)
    result_reward = result_reward or 4
    local sel_index = self.sel_flower_index
 
    local T = function (count)
        local list = {}
        local rand_list = DataHelper.GetRamdonList(1,3)
        for i,v in ipairs(rand_list) do
            if i <= count then
                list[v] = sel_index
            else
                local rand_list = DataHelper.GetRamdonList(1,3)
                for m,n in pairs(rand_list) do
                    if n ~= sel_index then
                        list[v] = n
                    end
                end
            end
        end
        return list
    end
    local list = T(4 - result_reward)
    return list
end

function ActSameFlowerPanel:OnSelClick(value, mono)
    if value then
        self.sel_flower_mono = mono
        self.sel_flower_index = mono:GetData().index
        self:OnCloseSelFlowerClick()
    else
        self.sel_flower_mono = nil
        self.sel_flower_index = nil
    end
end

function ActSameFlowerPanel:OnOpenSelFlowerClick()
    if ActSameFlowerPanel.playing then
        PublicPopupCtrl.Instance:Center(Language.SameFlower.Txt4)
        return
    end
    self.SelFlowerPanel:SetActive(true)
    self.Flowers:SetData({
        {index = 1},{index = 2},{index = 3}
    })
end

function ActSameFlowerPanel:OnCloseSelFlowerClick()
    self.SelFlowerPanel:SetActive(false)
    self:FlushSel()
end

ActSameFlowerNode = ActSameFlowerNode or DeclareMono("ActSameFlowerNode", UIWidgetBaseItem)
-- data : {list_data} #list_data >= 3
function ActSameFlowerNode:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.list_len = #data.list_data
    self.cell_h = 60    -- 格子宽
    self.spacing = 4    -- 格子内边距
    self.loop_height = self.list_len * (self.cell_h + self.spacing)     -- 循环的高 4 是格子内边距 60 是格子高
    local list_data = TableCopy(data.list_data)
    self:SetList(list_data)
    self.first_index = 1
    self.run_pos_y = self.first_index * 64 - 34
    self.ListObj:SetLocalPosition(Vector3.New(0,-(self.loop_height + self.run_pos_y), 0))
end

function ActSameFlowerNode:SetList(list_data)
    local data_1 = TableCopy(list_data)
    local data_2 = TableCopy(list_data)
    list_data = AndList(list_data, data_1)
    list_data = AndList(list_data, data_2)
    self.List:SetData(list_data)
end

function ActSameFlowerNode:OnDestroy()
    if self.run_ht ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_ht)
    end
end

function ActSameFlowerNode:Reset()
    self.SelImg:SetActive(false)
end

function ActSameFlowerNode:Play(target_index, index,callback_func)
    index = index or 1
    self.SelImg:SetActive(false)
    local t_i = 0
    for i,v in pairs(self.data.list_data) do
        if v == target_index then
            t_i = #self.data.list_data + 1 - i
            break
        end
    end
    self.callback_func = callback_func
    self.run_h = self.loop_height * (2 + index)
    self.end_y = self.run_h + (t_i * (64)) - 30 - 4
    self.start_y = 0
    self.max_speed = 20
    self.min_speed = 5
    self.speed = 2

    if self.run_ht ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_ht)
    end
    self.run_ht = Runner.Instance:RunUntilTrue(BindTool.Bind(self.RunEvent,self))
end

function ActSameFlowerNode:RunEvent()
    local pos = self.ListObj:GetLocalPosition()
    if self.end_y - self.start_y <= self.loop_height then
        self.speed = self.speed - (self.speed) / 8
        self.speed = self.speed <= self.min_speed and self.min_speed or self.speed
    else
        self.speed = self.speed + (self.speed) / 2
        self.speed = self.speed >= self.max_speed and self.max_speed or self.speed
    end
    self.start_y = self.start_y + self.speed
    if self.start_y >= (self.end_y - self.run_pos_y) then
        self.run_pos_y = self.end_y - self.run_h
        pos.y = -(self.loop_height + self.run_pos_y)
        self.ListObj:SetLocalPosition(pos)
        self:RunCallback()
        return true
    else
        pos.y = pos.y - self.speed
        if pos.y <= -self.loop_height * 2 then
            pos.y = pos.y + self.loop_height
        end
        self.ListObj:SetLocalPosition(pos)
    end
end

function ActSameFlowerNode:RunCallback()
    self.SelImg:SetActive(true)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.FabaoLianZhiGuangQiu)
    if self.callback_func then
        self.callback_func()
        self.callback_func = nil
    end
end

ActSameFlowerElement = ActSameFlowerElement or DeclareMono("ActSameFlowerElement" ,UIWidgetBaseItem)
-- data : int
function ActSameFlowerElement:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SpriteName(self.Img, string.format("Hua%s", data))
end

ActSameFlowerSelItem = ActSameFlowerSelItem or DeclareMono("ActSameFlowerSelItem", UIWidgetBaseItem)
function ActSameFlowerSelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SpriteName(self.Img, string.format("Hua%s", data.index))
end


-- ========== 奖励组
ActSameFlowerRewardGroup = ActSameFlowerRewardGroup or DeclareMono("ActSameFlowerRewardGroup", UIWidgetBaseItem)
-- data:{reward_type:int}
function ActSameFlowerRewardGroup:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Title, Language.SameFlower.RewardTitle[data.reward_type])
    local reward_list_data = YunZeData.Instance:GetSameFlowerRewardList(data.reward_type)
    for i = 1, self.Rewards:Length() do
        local v = reward_list_data[i]
        if v then
            self.Rewards[i]:SetData(v)
        end
    end
end

ActSameFlowerRewardItem = ActSameFlowerRewardItem or DeclareMono("ActSameFlowerRewardItem", UIWidgetBaseItem)
function ActSameFlowerRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data.reward_item)
    self.ItemCell:SetData(item)
    local lock = YunZeData.Instance:GetSameFlowerRewardLock(self.data.item_id)
    self.Selector.isOn = lock
end

function ActSameFlowerRewardItem:OnSelClick()
    if ActSameFlowerPanel.playing then
        PublicPopupCtrl.Instance:Center(Language.SameFlower.Txt4)
        return
    end
    if YunZeData.Instance:GetSameFlowerRewardLock(self.data.item_id) then
        return
    else
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
        local func = function (  )
            ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SAME_FLOWER, ActSameFlowerView.Oper.Select, self.data.item_id)
        end
        if self:IsSel() then
            local dialog_info = {
                content = Language.SameFlower.Txt10,
                confirm = {
                    func = function ()
                        func()
                        ViewMgr:CloseView(DialogTipsView)
                    end
                },
                cancel = {
                    func = function()
                        ViewMgr:CloseView(DialogTipsView)
                    end
                },
                new_tnr = DTNewTodayNotRemind.SameFlowerSelTip,
            }
            PublicPopupCtrl.Instance:DialogTips(dialog_info)
        else
            func()
        end
    end
end

function ActSameFlowerRewardItem:IsSel()
    local reward_type = self.data.item_pool
    local list = YunZeData.Instance:GetSameFlowerRewardList(reward_type)
    for i,v in pairs(list) do
        if v.item_id ~= self.data.item_id and YunZeData.Instance:GetSameFlowerRewardLock(v.item_id) then
            return true
        end
    end
    return false
end


ActSameFlowerBuyKeyView = ActSameFlowerBuyKeyView or DeclareView("ActSameFlowerBuyKeyView", "serveractivity/yun_ze/act_same_flower_dialog")
function ActSameFlowerBuyKeyView:OnCloseClick()
    ViewMgr:CloseView(ActSameFlowerBuyKeyView)
end

ActSameFlowerBuyKeyPanel = ActSameFlowerBuyKeyPanel or DeclareMono("ActSameFlowerBuyKeyPanel", UIWFlushPanel)
function ActSameFlowerBuyKeyPanel:ActSameFlowerBuyKeyPanel()
    self.data = YunZeData.Instance
    self.cfg = Config.fenxiangtonghuali_auto.other[1]
    self.data_cares = {
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
        {data = self.data.same_flower_data, func = self.FlushPanel, init_call = false},
    }
end

function ActSameFlowerBuyKeyPanel:Awake()
    UIWFlushPanel.Awake(self)
    local item = Item.Create(self.cfg.reward_item[0])
    self.Cell:SetData(item)
    UH.SetText(self.CellName, item:Name())
    UH.SetText(self.Price, self.cfg.price)
    local img_name = ""
    if self.cfg.price_type == 0 then
        img_name = HuoBi[CurrencyType.Gold]
    elseif self.cfg.price_type == 1 then
        img_name = "99999"
    else
        img_name = HuoBi[CurrencyType.ImmortalCoin]
    end
    UH.SpriteName(self.Icon, img_name)
    self:FlushPanel()
end

function ActSameFlowerBuyKeyPanel:FlushPanel()
    local str = self.cfg.limit_type == 1 and Language.SameFlower.Txt7 or Language.SameFlower.Txt8
    local num = self.data.same_flower_data.info.buy_count or 0
    local max_num = self.cfg.today_times
    num = ColorStr(num, COLORSTR.Red10)
    UH.SetText(self.LimitText, string.format(str, num, max_num))
end

function ActSameFlowerBuyKeyPanel:OnCancelClick()
    ViewMgr:CloseView(ActSameFlowerBuyKeyView)
end

function ActSameFlowerBuyKeyPanel:OnBuyClick()
    local num = self.data.same_flower_data.info.buy_count or 0
    local max_num = self.cfg.today_times
    if num >= max_num then
        PublicPopupCtrl.Instance:Error(Language.SameFlower.Txt9)
        return
    end
    if self.cfg.price_type ~= 1 then
        local price = lock_price + stuff_price
        local curr_type = self.cfg.price_type == 0 and CurrencyType.Gold or CurrencyType.ImmortalCoin
        if MallCtrl.IsNotCurrency(curr_type, self.cfg.price) then
            ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SAME_FLOWER, ActSameFlowerView.Oper.Buy)
        end
    else
        local fee = self.cfg.price
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_ZHAOCAIJINBAO, fee, ACTIVITY_TYPE.RAND_SAME_FLOWER, ActSameFlowerView.Oper.Buy)
    end
end

function ActSameFlowerBuyKeyPanel:SendBuy()
    if self.cfg == nil then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_SCAME_FLOWER, self.cfg.price)
end