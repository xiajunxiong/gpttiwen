DivinationView = DivinationView or DeclareView("DivinationView", "serveractivity/divination_view",Mod.RandActivity.Divination)
VIEW_DECLARE_LEVEL(DivinationView, ViewLevel.L1)
--Title=============Text
function DivinationView:LoadCallback()
    self.Currency:DefaultSet()
    self.Title.text = ActivityRandData.GetName(ACTIVITY_TYPE.RAND_DIVINATION)
end

function DivinationView:OnClickReset()
    local dialog_param = {
        content = Language.Divination.ResetAddLotTips,
        confirm = {
            func =  function()
                local param_t = {}
                param_t.rand_activity_type = ACTIVITY_TYPE.RAND_DIVINATION
                param_t.opera_type = DivinationData.ServerOperate.ResetLotTimes
                ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t) 
                ViewMgr:CloseView(DialogTipsView)
            end
        },
        cancel = {
            func =  function()
                ViewMgr:CloseView(DialogTipsView)
            end 
        },
        tnr = DTTodayNotRemind.DivinationReset
    }
    PublicPopupCtrl.Instance:DialogTips(dialog_param)
end

function DivinationView:OnClickClose()
    ViewMgr:CloseView(DivinationView)
end

function DivinationView:OnClickSkipAnim(tog_val)
    DivinationData.Instance:ViewData().anim_skip = tog_val
end

function DivinationView:OnClickInfo()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[57].desc)
end

DivinationPanel = DivinationPanel or DeclareMono("DivinationPanel",UIWFlushPanel)

function DivinationPanel:DivinationPanel()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_DIVINATION,TimeType.Type_Special_4)
    self.seq_to_addlot_idx = {}
    local reward_pool = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_DIVINATION,Config.divination_activity_auto.tianming_jiangchi)
    local add_lot_item_idx = 1
    for _,v in ipairs(reward_pool) do
        local item_cell = self.ItemList[v.seq]
        if item_cell ~= nil then
            local show_rate = math.floor(v.rate * 10000 + 0.5) / 100
            local reward_item_vo = TableCopy(v.reward_item)
            reward_item_vo.top_right_str = string.format(Language.Common.Percent,tostring(show_rate))
            local item_data = Item.Create(reward_item_vo)
            item_cell:SetData(item_data)
            
            if v.can_add_lot ~= 0 then  --显示在下面加注的列表里
                local add_lot_item = self.ResetItems[add_lot_item_idx]
                if add_lot_item ~= nil then
                    add_lot_item.gameObject.transform.parent.gameObject:SetActive(true)
                    add_lot_item:SetData(Item.Create(v.reward_item))
                    self.seq_to_addlot_idx[v.seq] = add_lot_item_idx
                    add_lot_item_idx = add_lot_item_idx + 1
                end
            end
        end
    end
    self.data_cares = {
        {data = DivinationData.Instance:RewardHistory(), func = self.FlushRewardHistory,init_call = true },
        {data = DivinationData.Instance:AddLotInfo(), func = self.FlushAddLotNum,init_call = true },
        {data = DivinationData.Instance:ViewData(), func = self.OnAnimSkipChange,init_call = true,keys = {"anim_skip"} }, 
        {data = DivinationData.Instance:RewardData(), func = self.OnRewardData,init_call = false},
        {data = DivinationData.Instance:GetUseTimeData(), func = self.FulshUseTimes,init_call = true},
    }
    self.item_cur_i = 1
    self.item_timer = nil
    self.need_flush_history = false --滚动转盘的时候屏蔽刷新界面获奖列表，防止剧透
    self.get_item_cache = nil
    --定时请求获奖名单
    self.require_reward_history_timer = TimeHelper:AddRunTimer(function () 
        local param_t = {}
        param_t.rand_activity_type = ACTIVITY_TYPE.RAND_DIVINATION
        param_t.opera_type = DivinationData.ServerOperate.RewardPoolInfo
        ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t) 
    end, 10, -1, true)
end

local start_interval = 0.1      --开始转的时候滚动间隔
local end_interval = 0.6        --结束转的时候的滚动间隔
local total_time_min = 4        --快速滚动的最小时间
local total_time_max = 5        --快速滚动的最大时间

function DivinationPanel:BeginZhuanPan(stop_idx,end_func)
    if self.item_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.item_timer)
    end
    local cur_stay_time = 0
    local cur_interval = start_interval
    local item_count = self.ItemList:Length()
    local total_roll_time = GameMath.FloatRamdon(total_time_min, total_time_max)
    local start_roll_time = Time.time
    local speed_down_per_interval = nil
    self.item_timer = Runner.Instance:RunUntilTrue(function() 
        cur_stay_time = cur_stay_time + Time.deltaTime
        if cur_stay_time >= cur_interval then
            self.item_cur_i = self.item_cur_i % item_count + 1
            cur_stay_time = cur_stay_time - cur_interval
            self.SelectImg:SetPosition(self.ItemList[self.item_cur_i].gameObject:GetPosition())
            if Time.time - start_roll_time > total_roll_time then
                if speed_down_per_interval == nil then
                    local idx_off = (stop_idx < self.item_cur_i and (stop_idx + item_count) or stop_idx) -self.item_cur_i
                    speed_down_per_interval = (idx_off ~= 0) and ((end_interval - start_interval) / idx_off) or end_interval
                end
                cur_interval = cur_interval + speed_down_per_interval
            end
        end
        local stop = (Time.time - start_roll_time >= total_roll_time) and (stop_idx == self.item_cur_i)
        if stop then
            self.item_timer = nil
            end_func()
        end
        return stop 
    end)
end
function DivinationPanel:FulshUseTimes()
    UH.SetText(self.UseTimes,string.format(Language.Divination.UseTimes,DivinationData.Instance:GetUseTime()))
end
function DivinationPanel:OnRewardData()
    local idx = DivinationData.Instance:RewardData().reward_index
    self.get_item_cache = nil
    local reward_pool = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_DIVINATION,Config.divination_activity_auto.tianming_jiangchi)
    for _,v in ipairs(reward_pool) do
        if v.seq == idx then
            local add_lot_idx = self.seq_to_addlot_idx[v.seq]
            local add_lot_count = add_lot_idx and DivinationData.Instance:AddLotInfo()[add_lot_idx] or 1
            self.get_item_cache = Item.Init(v.reward_item.item_id,v.reward_item.num * add_lot_count)
            break
        end
    end
    local popup_item_get_center = function ()
        if self.get_item_cache ~= nil then
            if DirectUseItemID[self.get_item_cache.item_id] == nil then
                PublicPopupCtrl.Instance:CenterI(self.get_item_cache)
                ChatCtrl.Instance:ChannelChatCustom({item_get={item=self.get_item_cache,num = self.get_item_cache.num}})
            end
            self.get_item_cache = nil
        end       
    end
    if DivinationData.Instance:ViewData().anim_skip then
        self.item_cur_i = idx
        self.SelectImg:SetPosition(self.ItemList[self.item_cur_i].gameObject:GetPosition())
        popup_item_get_center()
    else
        self.LotteryInter.Interactable = false
        self.ResetInter.Interactable = false
       
        self:BeginZhuanPan(idx,
        function() 
            self.LotteryInter.Interactable = true
            self.ResetInter.Interactable = true
            if self.need_flush_history then
                self.need_flush_history = false
                self:FlushRewardHistory()
            end
            popup_item_get_center()
        end) 
    end
end

function DivinationPanel:IsRolling()
    return self.item_timer ~= nil
end

function DivinationPanel:FlushRewardHistory()
    if self:IsRolling() then
        self.need_flush_history = true
        return
    end
    self.RewardList:SetData(DivinationData.Instance:RewardHistory())
end

function DivinationPanel:FlushAddLotNum()
    local add_lot_info = DivinationData.Instance:AddLotInfo()
    local add_lot_count = 0
    for i,v in ipairs(add_lot_info) do
        local txt = self.ResetNums[i]
        if txt ~= nil then
            txt.text = tostring(v)
        end
        add_lot_count = add_lot_count + v - 1   --未加注的时候值为1，所以要减1
    end
    local price_cfg = Cfg.GetDivinationAddLotPriceCfg(add_lot_count)
    self.GoldNum.text = price_cfg.chou_consume_gold
    self.consume_gold = price_cfg.chou_consume_gold
end

function DivinationPanel:OnClickBuGua()
    if not MallCtrl.IsNotCurrency(CurrencyType.Gold,self.consume_gold) then
        return
    end
    if DivinationData.Instance:GetUseTime() <= 0 then
        PublicPopupCtrl.Instance:Center(Language.Divination.TipTimes)
        return
    end
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_DIVINATION,DivinationData.ServerOperate.StartChou)
end

function DivinationPanel:OnClickAddLot(idx)
    if self:IsRolling() then
        PublicPopupCtrl.Instance:Center(Language.Divination.RollingTips)
        return
    end
    local add_lot_count = 0
    local add_lot_info = DivinationData.Instance:AddLotInfo()
    for i,v in ipairs(add_lot_info) do
        add_lot_count = add_lot_count + v - 1   --未加注的时候值为1，所以要减1
    end
    if add_lot_count >= DivinationData.MAX_ADD_LOT_NUM then
        PublicPopupCtrl.Instance:Center(Language.Divination.AddLotMax)
        return
    end
    local price_cfg = Cfg.GetDivinationAddLotPriceCfg(add_lot_count+1)
    local add_lot_price = price_cfg.add_consume_gold
    if not MallCtrl.IsNotCurrency(CurrencyType.Gold,add_lot_price) then
        return
    end
    local item_cell_data = self.ResetItems[idx]:GetData()
    local dialog_param = {
        content = string.format(
            Language.Divination.AddLotTips,add_lot_price,item_cell_data:QuaName()),
        confirm = {
            func =  function()
                local param_t = {}
                param_t.rand_activity_type = ACTIVITY_TYPE.RAND_DIVINATION
                param_t.opera_type = DivinationData.ServerOperate.AddLotTimes
                param_t.param_1 = idx-1 --服务器从0开始算的
                ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t) 
                ViewMgr:CloseView(DialogTipsView)
            end
        },
        cancel = {
            func =  function()
                ViewMgr:CloseView(DialogTipsView)
            end 
        },
        tnr = DTTodayNotRemind.DivinationAddLot
    }
    PublicPopupCtrl.Instance:DialogTips(dialog_param)
end

function DivinationPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.item_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.item_timer)
        self.item_timer = nil
    end
    if self.get_item_cache ~= nil then
        if DirectUseItemID[self.get_item_cache.item_id] == nil then
            PublicPopupCtrl.Instance:CenterI(self.get_item_cache)
            ChatCtrl.Instance:ChannelChatCustom({item_get={item=self.get_item_cache,num = self.get_item_cache.num}})
        end
        self.get_item_cache = nil
    end   
    if self.require_reward_history_timer ~= nil then
        TimeHelper:CancelTimer(self.require_reward_history_timer)
        self.require_reward_history_timer = nil
    end
end

function DivinationPanel:OnAnimSkipChange()
    self.Toggle.isOn = DivinationData.Instance:ViewData().anim_skip
end

DivinationRewardItem = DivinationRewardItem or DeclareMono("DivinationRewardItem",UIWidgetBaseItem)

function DivinationRewardItem:SetData(data)
    local item_name = Item.GetName(data.rate_item_id)
    local item_color = Item.GetColorStr(data.rate_item_id,true)
    self.InfoTxt.text = string.format(Language.Divination.RewardHistory,COLORSTR.Blue9,data.role_name,item_color,item_name,COLORSTR.Blue9,data.item_nun)        
end