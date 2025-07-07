BanquetCfg = {}
BanquetCfg.Oper = {
    Info = 0,   -- 下发信息
    Put = 1,    -- 提交道具  p1:index
    Get = 2,    -- 领取奖励  p1:seq
    Eat = 3,    -- 吃席
}
BanquetCfg.Status = {
    Black = 0,  -- 空白期  24:00 -> 筹办
    Ready = 1,  -- 准备期
    Banquet = 2,-- 宴会期
    Stuff = 3,  -- 筹办期
}

-- Gm命令加进度：qionglouyuyan:1 100
-- Gm命令进入下一进度：qionglouyuyan:2

BanquetView = BanquetView or DeclareView("BanquetView", "serveractivity/summer_first/banquet_view", Mod.SummerFirst.Banquet)

function BanquetView:BanquetView()
    self.data = ServerActivityData.Instance
    self.ht = self.data.banquet_data:Care(BindTool.Bind(self.FlushPanel, self))
end

function BanquetView:LoadCallback()
    self.ReadPanelObj:SetActive(true)
    self.EatPanelObj:SetActive(false)
    self.TestObj:SetActive(DEBUG_MODE)
end

function BanquetView:CloseCallback()
    self.data.banquet_data:Uncare(self.ht)
end

function BanquetView:FlushPanel()
    if self.data.banquet_data.base_info.status ~= BanquetCfg.Status.Banquet then
        if self.EatPanelObj.activeSelf == true then
            self.ReadPanelObj:SetActive(true)
            self.EatPanelObj:SetActive(false)
            PublicPopupCtrl.Instance:Error(Language.Banquet.Error6)
        end
    end
end

function BanquetView:OnTestOrderClick(int_p)
    if int_p == 1 then
        GMCmdCtrl.Instance:SendGMCommand("qionglouyuyan","2 0 0 0 0 0")
    elseif int_p == 2 then
        GMCmdCtrl.Instance:SendGMCommand("qionglouyuyan","1 100 0 0 0 0")
    end
end

function BanquetView:OnCloseClick()
    ViewMgr:CloseView(BanquetView)
end

function BanquetView:OnEnterClick(data)
    if data.status == BanquetCfg.Status.Banquet then
        self.ReadPanelObj:SetActive(false)
        self.EatPanelObj:SetActive(true)
    else
        local npc_id = Config.ambassador_activity_auto.other[1].npc_id
        ViewMgr:CloseView(BanquetView)
        ViewMgr:CloseView(SummerFirstView)
        SceneLogic.Instance:AutoToNpcs(npc_id)
    end
end

BanquetReadyPanel = BanquetReadyPanel or DeclareMono("BanquetReadyPanel",UIWFlushPanel)
function BanquetReadyPanel:BanquetReadyPanel()
    self.data = ServerActivityData.Instance
    self.act_info = self.data.banquet_data
    self.data_cares = {
        {data = self.act_info, func = self.FlushPanel, init_call = false},
    }
end

function BanquetReadyPanel:Awake()
    UIWFlushPanel.Awake(self)

    local server_open_day = TimeCtrl.Instance:GetCurOpenServerDay()
    ActivityRandData.SetRemind(ACTIVITY_TYPE.RAND_BANQUET, server_open_day)

    ServerActivityCtrl.Instance:BanquetReq(BanquetCfg.Oper.Info)
    local p1_num = self.data:GetBanquetRewardCfg(1).prep_down
    local p2_num = self.data:GetBanquetRewardCfg(2).prep_down
    self.max_progress_num = self.data:GetBanquetRewardCfg(3).prep_down
    UH.SetText(self.P1, p1_num)
    UH.SetText(self.P2, p2_num)
    UH.SetText(self.P3, self.max_progress_num)
    self.P1Obj:SetLocalPosition(Vector3.New((826 * (p1_num / self.max_progress_num) - 418), 60, 0))
    self.P2Obj:SetLocalPosition(Vector3.New((826 * (p2_num / self.max_progress_num) - 418), 60, 0))
    self.P3Obj:SetLocalPosition(Vector3.New(826 - 418, 60, 0))
    self.ActTime:CloseCountDownTime()
    self.ActTime:CreateActTime(ACTIVITY_TYPE.RAND_BANQUET,TimeType.Type_Special_4,Language.Banquet.ActTime)
    --self:FlushPanel()
end

function BanquetReadyPanel:FlushPanel()
    self:FlushFrame()
    self:FlushProgress()
end

function BanquetReadyPanel:FlushFrame()
    self.cur_reward_data = self.data:GetBanquetRewardCfg()
    local frame_data_l, frane_data_r = self:GetFrameData()
    self.FrameL:SetData(frame_data_l)
    self.FrameR:SetData(frane_data_r)
    if self.act_info.base_info.status == BanquetCfg.Status.Stuff then
        UH.SpriteName(self.StateImg, "_LocChouBeiZhong")
    elseif self.act_info.base_info.status == BanquetCfg.Status.Black then
        UH.SpriteName(self.StateImg, "")
    else
        if self.cur_reward_data.seq < 1 then
            UH.SpriteName(self.StateImg, "_LocChouBeiShiBai")
        else
            UH.SpriteName(self.StateImg, "_LocChouBeiChengGong")
        end
    end
    
    for i = 1, 3 do
        if self.cur_reward_data.seq == 0 or self.cur_reward_data.seq ~= i then
            self.Foods[i]:SetActive(false)
        else
            self.Foods[i]:SetActive(true)
        end
    end
end

function BanquetReadyPanel:FlushProgress()
    local progress_va = self.act_info.base_info.progress_num / self.max_progress_num
    progress_va = progress_va >= 1 and 1 or progress_va
    self.Progress:SetProgress(progress_va)
    self.Progress:SetText(self.act_info.base_info.progress_num)
    local data = self.data:GetBanquetRewardCfg(3)
    local flag = self.data:GetBanquetBoxFlag(data)
    self.BoxInteractor.Interactable = flag > 0
    self.BoxEffect:SetActive(flag == 1)
    for i = 1, 3 do
        local num = self.data:GetBanquetRewardCfg(i).prep_down
        self.PEffects[i]:SetActive(self.act_info.base_info.progress_num >= num)
        LogError(num, self.act_info.base_info.progress_num)
    end
end

function BanquetReadyPanel:GetFrameData()
    local time_l = 0
    local time_r = 0
    if self.act_info.base_info.status == BanquetCfg.Status.Stuff or self.act_info.base_info.status == BanquetCfg.Status.Black then
        time_l = self.act_info.base_info.next_status_time
    end
    if self.act_info.base_info.status == BanquetCfg.Status.Stuff then
        time_r = self.act_info.base_info.next_status_time + Config.burning_summer_auto.other[1].prepare_times
    elseif self.act_info.base_info.status == BanquetCfg.Status.Banquet or self.act_info.base_info.status == BanquetCfg.Status.Ready then
        time_r = self.act_info.base_info.next_status_time
    end
    local frame_data_l = {
        status = BanquetCfg.Status.Stuff,
        is_unlock = self.act_info.base_info.status == BanquetCfg.Status.Stuff,
        time = time_l,
        reward_list = self.data:GetBanquetRewardListByGroupId(self.data:GetBanquetStuffRewardGroupId()),
    }
    local frame_data_r = {
        status = BanquetCfg.Status.Banquet,
        is_unlock = self.act_info.base_info.status == BanquetCfg.Status.Banquet and self.cur_reward_data.seq > 0,
        time = time_r,
        reward_list = self.data:GetBanquetRewardListByGroupId(self.data:GetBanquetRewardCfg().reward_group),
    }
    return frame_data_l,frame_data_r
end

function BanquetReadyPanel:OnDestroy()
    self.ActTime:CloseCountDownTime()
    UIWFlushPanel.OnDestroy(self)
end

function BanquetReadyPanel:OnGetBoxClick(seq)
    local data = self.data:GetBanquetRewardCfg(seq)
    local flag = self.data:GetBanquetBoxFlag(data)
    if flag == 1 then
        ServerActivityCtrl.Instance:BanquetReq(BanquetCfg.Oper.Get, seq)
    elseif flag == 0 then
        PublicPopupCtrl.Instance:Error(Language.Banquet.Error1)
    elseif flag == -1 then
        PublicPopupCtrl.Instance:Error(Language.Banquet.Error2)
    end
end

function BanquetReadyPanel:OnHelpClick()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[133].desc})
end

BanquetEatPanel = BanquetEatPanel or DeclareMono("BanquetEatPanel", UIWFlushPanel)
function BanquetEatPanel:BanquetEatPanel()
    self.data = ServerActivityData.Instance
    self.act_info = self.data.banquet_data
    self.update_time = Config.burning_summer_auto.other[1].refresh
    self.data_cares = {
        {data = self.act_info, func = self.FlushPanel, init_call = false},
    }
    self.max_eat_count = Config.burning_summer_auto.other[1].phase_times
end

function BanquetEatPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.data.banquet_look_flag = true
    self.Time:CloseCountDownTime()
    self.Time:StampTime(self.act_info.base_info.next_status_time, ItemType.Type_Special_0)
    self:FlushPanel()
end

function BanquetEatPanel:OnDestroy()
    if self.run_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_timer)
    end
    self.Time:CloseCountDownTime()
    UIWFlushPanel.OnDestroy(self)
end

function BanquetEatPanel:FlushPanel()
    UH.SetText(self.EatCount, string.format(Language.Banquet.EatDesc, (self.act_info.reward_info.eat_food_count or 0), self.max_eat_count))
    if self.act_info.reward_info.eat_food_count >= self.max_eat_count then
        self.Progress:SetProgress(1)
        return
    end
    local server_time = TimeCtrl.Instance:GetServerTime()
    self.down_count_time = (self.act_info.reward_info.next_eat_tamp or 0) - server_time
    if self.run_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_timer)
    end
    self.EatBtnObj:SetActive(false)
    self.run_timer = Runner.Instance:RunUntilTrue(BindTool.Bind(self.Run,self))
end

function BanquetEatPanel:Run()
    local last_time = math.ceil(self.down_count_time)
    self.down_count_time = self.down_count_time - Time.deltaTime
    local now_time = math.ceil(self.down_count_time)
    if last_time > now_time then
        now_time = now_time <= 0 and 0 or now_time
        self.Progress:SetText(now_time)
    end
    local num = self.update_time - self.down_count_time
    num = num > self.update_time and self.update_time or num
    local progress = num / self.update_time
    self.Progress:SetProgress(progress)
    if num == self.update_time then
        self.EatBtnObj:SetActive(true)
        return true
    end
end

function BanquetEatPanel:OnEatClick()
    if self.act_info.reward_info.eat_food_count >= self.max_eat_count then
        PublicPopupCtrl.Instance:Error(Language.Banquet.Error3)
        return
    end
    local server_time = TimeCtrl.Instance:GetServerTime()
    if (self.act_info.reward_info.next_eat_tamp or 0) > server_time then
        PublicPopupCtrl.Instance:Error(Language.Banquet.Error4)
        return
    end
    ServerActivityCtrl.Instance:BanquetReq(BanquetCfg.Oper.Eat)
end

BanquetMainFrame = BanquetMainFrame or DeclareMono("BanquetMainFrame",UIWidgetBaseItem)
-- @ data : {status:BanquetCfg.Status, is_unlock:bool, reward_list:{}, time:number}
function BanquetMainFrame:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TitleText, Language.Banquet.StatusDesc[data.status])
    local item_list = DataHelper.FormatItemList(data.reward_list)
    self.RewardList:SetData(item_list)
    self:HandleShow(data.is_unlock)
    self:HandleTime(data.time)
end

function BanquetMainFrame:HandleTime(time)
    local cur_reward_data = ServerActivityData.Instance:GetBanquetRewardCfg()
    local cur_status = ServerActivityData.Instance.banquet_data.base_info.status
    local other_flag = self.data.status == BanquetCfg.Status.Banquet and
    (cur_status == BanquetCfg.Status.Banquet or cur_status == BanquetCfg.Status.Ready) and cur_reward_data.seq < 1
    local other_flag_2 = self.data.status == BanquetCfg.Status.Banquet and cur_status == BanquetCfg.Status.Stuff
    self.EndTime:SetObjActive(time == 0 or other_flag or other_flag_2)
    self.Time:SetObjActive(time ~= 0 and not other_flag and not other_flag_2)
    if time > 0 and not other_flag and not other_flag_2 then
        self.Time:CloseCountDownTime()
        local now_time = TimeCtrl.Instance:GetServerTime()
        local time_type
        if time - now_time > 3600 then
            time_type = TimeType.Type_Time_2
        else
            time_type = TimeType.Type_Special_6
        end
	    self.Time:StampTime(time, time_type)
        if cur_status == self.data.status then
            UH.SetText(self.TimeDesc, Language.Banquet.TimeStr2)
        else
            UH.SetText(self.TimeDesc, Language.Banquet.TimeStr1)
        end
    elseif other_flag then
        UH.SetText(self.EndTime, Language.Banquet.Tip2)
    elseif other_flag_2 then
        UH.SetText(self.EndTime, Language.Banquet.Tip3)
    else
        UH.SetText(self.EndTime, Language.Banquet.Tip1)
    end
end

function BanquetMainFrame:HandleShow(is_unlock)
    self.Interactor.Interactable = is_unlock
    self.GoBtnObj:SetActive(is_unlock)
    self.LockObj:SetActive(not is_unlock)
end

function BanquetMainFrame:OnDestroy()
    self.Time:CloseCountDownTime()
    UIWidgetBaseItem.OnDestroy(self)
end

function BanquetMainFrame:OnClick()
    if self.data.func then
        self.data.func()
    end
end

BanquetRewardItem = BanquetRewardItem or DeclareMono("BanquetRewardItem", UIWidgetBaseItem)
function BanquetRewardItem:SetData(data)
    LogError(data)
    self.Cell:SetData(data)
end


BanquetPutItemsView = BanquetPutItemsView or DeclareView("BanquetPutItemsView","serveractivity/summer_first/banquet_put_items")

function BanquetPutItemsView:BanquetPutItemsView()
    self.Board:SetData(self:GetType(),string.format(Language.Banquet.PutViewName, ActivityRandData.GetName(ACTIVITY_TYPE.RAND_BANQUET)), Vector2.New(551, 565))
    self.selected_index = 1
    self.care_ht = BagData.Instance:Care(BindTool.Bind(self.FlushPanel,self))
    self.need_num = Config.burning_summer_auto.other[1].prep_num
end

function BanquetPutItemsView:LoadCallback()
    UH.SetText(self.BtnText, string.format(Language.Banquet.Put))
    self:FlushPanel()
end

function BanquetPutItemsView:CloseCallback()
    BagData.Instance:UnCare(self.care_ht)
end

function BanquetPutItemsView:FlushPanel()
    local item_list = ServerActivityData.Instance:GetBanquetStuffItemList() or {}
    if next(item_list) then
        self.NotItemTipObj:SetActive(false)
        self.Interactor.Interactable = true
    else
        self.NotItemTipObj:SetActive(true)
        self.Interactor.Interactable = false
    end
    self.ItemList:SetData(item_list)
    if not item_list[self.selected_index] then
        self.selected_index = 1
        self.selected_data = nil
    end
    self.ItemList:SetectItem(self.selected_index,true)
end

function BanquetPutItemsView:OnPutClick()
    if self.selected_data then
        local item_num = Item.GetNum(self.selected_data.item.item_id)
        if item_num >= self.need_num then
            ServerActivityCtrl.Instance:BanquetReq(BanquetCfg.Oper.Put,self.selected_data.item.item_id)
        else
            PublicPopupCtrl.Instance:Error(string.format(Language.Banquet.Error5,self.need_num))
        end
    end
end

function BanquetPutItemsView:OnToggleClick(value,mono)
    if value then
        local data = mono:GetData()
        self.selected_index = self.ItemList:GetSelectedIndex(data)
        self.selected_data = data
    end
end

BanquetPutItem = BanquetPutItem or DeclareMono("BanquetPutItem", UIWidgetBaseItem)
function BanquetPutItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Item:SetData(data.item)
    UH.SetText(self.ItemName, data.item:Name())
end
