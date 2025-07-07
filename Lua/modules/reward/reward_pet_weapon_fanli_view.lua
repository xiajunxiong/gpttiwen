-- =================================== 废弃 废弃 暂时留着 =============================

PetWeaponFanLiView = PetWeaponFanLiView or DeclareMono("PetWeaponFanLiView",UIWFlushPanel)

local result = 4;
local Yi_Chou = 0;

function PetWeaponFanLiView:PetWeaponFanLiView()
    self.Data = RewardData.Instance
    self.data = self.Data.pet_weapon_fanli_data
    RewardCtrl.Instance:PetWeaponFanLiSend(HunQiFanLiReq.Info)
    self.data_cares = {
        {data = self.data, func = self.Drawing, keys={"draw_flag"},init_call = false},
        {data = self.data, func = self.FlushRewardView, init_call = false},
        {data = self.data, func = self.RewardFresh, keys={"lingqu_flag"},init_call = false},
    }
end

function PetWeaponFanLiView:Awake()
    UIWFlushPanel.Awake(self)
    self.Data:HunQiFanLiInitCofig()
    self.Data:PetWeaponFanLiClearRemind(PrefKeys.HunQiFanLiFirst())
    self.data.on_draw = false
    self.is_drawing = false
    --今日抽奖是否完成
    self.JInRIWanChengDi:SetActive(false)
    self.DrawBtnInter.Interactable = true
    if self.data.last_lottery_times < 1 and self.data.taday_recharge >= 120 then
        self.JInRIWanChengDi:SetActive(true)
        self.DrawBtnInter.Interactable = false
    end
    --设置抽奖按钮状态
    if self.data.last_lottery_times < 1 and self.data.taday_recharge < 120 then
        UH.SetText(self.DrawBtnTxt,Language.PetWeaponFanLi.QianWangChongZhi)
        self.DrawEffect:SetActive(false)
    elseif self.data.last_lottery_times < 1 and self.data.taday_recharge >= 120 then
        UH.SetText(self.DrawBtnTxt,Language.PetWeaponFanLi.ChouJiang)
        self.DrawEffect:SetActive(false)
    else
        UH.SetText(self.DrawBtnTxt,Language.PetWeaponFanLi.ChouJiang)
        self.DrawEffect:SetActive(true)
    end
    UH.SetText(self.HuoDeCountTxt,self.data.last_lottery_times)
    --生成物品
    for i=1 , 8 do
        local data = self.Data.show_reward_group_list[i]["item"]
        local item = Item.Create(data);
        self.ItemList[i]:SetData(item);
        local gl = (self.Data.show_reward_group_list[i]["show_rate"] * 100).."%"
        UH.SetText(self.GaiLvList[i],gl)
        if self.Data.show_reward_group_list[i]["mark"] == 1 then
            self.MaskList[i]:SetActive(true)
        else
            self.MaskList[i]:SetActive(false)
        end
    end
    --加载轮盘状态
    local table_state = self.Data:GetPetWeaponFanLiTableState()
    for i = 0, 7 do
        if table_state[i] == 1 then
            self.ToggleList[i+1].isOn = true
            self.TickList[i+1]:SetActive(true);
        else
            self.ToggleList[i+1].isOn = false
            self.TickList[i+1]:SetActive(false);
        end
    end
    --保存轮盘状态到本地，抽奖时用于判断
    self.cur_rotary_table_state = {}
    for i = 1, 8 do
        table.insert(self.cur_rotary_table_state,table_state[i-1])
    end
    --抽奖次数界面设置
    local target_ = self.Data:GetPetWeaponFanLiNumber()
    self.reward_count = target_ - 1

    self.CountEff:SetActive(false)
    local target
    if target_ == 0 then
        self.YiLingQu:SetActive(true)
        local reward_item = self.Data.show_reward_item_list[4]["item"]
        target = 64
        local item = Item.Create(reward_item)
        self.RewardItem:SetData(item)
        CellFlushs.MakeGrey(self.RewardItem, true)
    else
        local reward_item = self.Data.show_reward_item_list[target_]["item"]
        local item = Item.Create(reward_item);
        self.RewardItem:SetData(item)
        CellFlushs.MakeGrey(self.RewardItem, false)
        target = self.Data.show_reward_item_list[target_]["number"]
    end
    UH.SetText(self.TargetTxt,target)
    for i = 1, 4 do
        if self.data.lottery_times >= self.data.lottery_reward_times[i] and self.data.lottery_reward_flag[i-1] ~= 1 then
            self.CountEff:SetActive(true)
        end
    end
    BagData.Instance:SetHideCurrencyCenterFlag(true)
end

function PetWeaponFanLiView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.drawing)
    TimeHelper:CancelTimer(self.one_round)
end

function PetWeaponFanLiView:FlushRewardView()
    self.Data:GetPetWeaponFanLiRemind()
    if self.data.last_lottery_times < 1 and self.data.taday_recharge < 120 then
        UH.SetText(self.DrawBtnTxt,Language.PetWeaponFanLi.QianWangChongZhi)
        self.DrawEffect:SetActive(false)
        self.KuangAni:Stop()
        self.DrawBtnInter.Interactable = true
        self.JInRIWanChengDi:SetActive(false)
    elseif self.data.last_lottery_times < 1 and self.data.taday_recharge >= 120 then
        UH.SetText(self.DrawBtnTxt,Language.PetWeaponFanLi.ChouJiang)
        self.DrawEffect:SetActive(false)
        self.DrawBtnInter.Interactable = false
        self.KuangAni:Stop()
        self.JInRIWanChengDi:SetActive(true)
    else
        UH.SetText(self.DrawBtnTxt,Language.PetWeaponFanLi.ChouJiang)
        self.DrawEffect:SetActive(true)
        self.KuangAni:Play()
    end
    UH.SetText(self.HuoDeCountTxt,self.data.last_lottery_times)
    self:RefreshJinImage()
end


--刷新需要充值金额
function PetWeaponFanLiView:RefreshJinImage()
    self.JinRiChongZhiPanel:SetActive(true)
    self.MingRiChongZhiPanel:SetActive(false)
    if self.data.taday_recharge >= 120 then
        self.JinRiChongZhiPanel:SetActive(false)
        self.MingRiChongZhiPanel:SetActive(true)
        self.JinL:SetActive(false)
        self.JinR:SetActive(false)
        self.JinRD:SetActive(true)
        UH.SpriteName(self.JinRDImg,"6")
        return
    end
    if self.data.taday_recharge >= 80 then
        self.JinR:SetActive(true)
        self.JinL:SetActive(true)
        self.JinRD:SetActive(false)
        UH.SpriteName(self.JinLImg,"1")
        UH.SpriteName(self.JinRImg,"2")
        return
    end
    if self.data.taday_recharge >= 60 then
        self.JinL:SetActive(false)
        self.JinR:SetActive(false)
        self.JinRD:SetActive(true)
        UH.SpriteName(self.JinRDImg,"8")
        return
    end
    self.JinL:SetActive(false)
    self.JinR:SetActive(false)
    self.JinRD:SetActive(true)
    UH.SpriteName(self.JinRDImg,"6")
end


function PetWeaponFanLiView:OnClickPre()
    ViewMgr:OpenView(PetWeaponFanLiPreView)
end
--点击获取满足抽奖次数后物品
function PetWeaponFanLiView:OnRewardLingQu()
    if self.reward_count < 0 then
        return
    end
    if self.is_drawing then
        PublicPopupCtrl.Instance:Center(Language.PetWeaponFanLi.Drawing)
        return
    end
    for i = 1, 4 do
        if self.data.lottery_times >= self.data.lottery_reward_times[i] and self.data.lottery_reward_flag[i-1] ~= 1 then
            RewardCtrl.Instance:PetWeaponFanLiSend(HunQiFanLiReq.Lottery_Count_Reward,self.reward_count)
            self.data:SetDirty("lingqu_flag")
            if self.reward_count == 3 then
                self.YiLingQu:SetActive(true)
                local reward_item = self.Data.show_reward_item_list[self.reward_count+1]["item"]
                local item = Item.Create(reward_item);
                self.RewardItem:SetData(item)
                CellFlushs.MakeGrey(self.RewardItem, true)
                self.CountEff:SetActive(false)
            end
            return
        end
    end
    RewardCtrl.Instance:PetWeaponFanLiSend(HunQiFanLiReq.Lottery_Count_Reward,self.reward_count)
end
--次数足够时领取奖励后刷新
function PetWeaponFanLiView:RewardFresh()
    local reward_item = self.Data.show_reward_item_list[self.reward_count+1]["item"]
    ActRewardTipData.Instance:OpenActRewardTipViewByData(ServeractivityConfig.ActRewerdTip.RewardType.Success,{reward_item})
    PublicPopupCtrl.Instance:CenterI({item_id = reward_item.item_id, num = reward_item.num,
        color_str = reward_item:ColorStr(),item = reward_item,icon_id = reward_item:IconId()})
    ChatCtrl.Instance:ChannelChatCustom{item_get = {item = reward_item, num = reward_item.num}}
    if self.reward_count == 3 then
        return
    end
    local reward_item = self.Data.show_reward_item_list[self.reward_count+2]["item"]
    local item = Item.Create(reward_item);
    self.RewardItem:SetData(item)
    CellFlushs.MakeGrey(self.RewardItem, false)
    local target = self.Data.show_reward_item_list[self.reward_count+2]["number"]
    UH.SetText(self.TargetTxt,target)
    self.CountEff:SetActive(false)
    local target_ = self.Data:GetPetWeaponFanLiNumber()
    self.reward_count = target_ - 1
end


--抽奖事件
function PetWeaponFanLiView:OnClickDraw()
    if self.is_drawing then
        PublicPopupCtrl.Instance:Center(Language.PetWeaponFanLi.Drawing)
        return
    end
    for i = 1, 4 do
        if self.data.lottery_times >= self.data.lottery_reward_times[i] and self.data.lottery_reward_flag[i-1] ~= 1 then
            PublicPopupCtrl.Instance:Center(Language.PetWeaponFanLi.Tip1)
            self.reward_count = i-1
            return
        end
    end
    if self.data.last_lottery_times < 1 and self.data.taday_recharge < 120 then
        ViewMgr:OpenViewByKey(Mod.Store.Recharge)
        return
    elseif self.data.last_lottery_times < 1 and self.data.taday_recharge >= 120 then
        PublicPopupCtrl.Instance:Center(Language.PetWeaponFanLi.Tip2)
        return
    end
    --条件满足发送抽奖请求
    if self.item_eff ~= nil then
        self.ItemEffList[self.resIndex]:Stop(self.item_eff[self.resIndex])
    end
    RewardCtrl.Instance:PetWeaponFanLiSend(HunQiFanLiReq.Lottery)
    self.is_drawing = true
    self.data.on_draw = true
end


function PetWeaponFanLiView:Drawing()
    local tLast = self.Data:GetPetWeaponFanLiLastLottery()
    UH.SetText(self.HuoDeCountTxt,tLast)
    --获取抽到的物品
    local reset = true
    local table_state = self.data.table_state
    for i = 1, 8 do
        if table_state[i-1] == 1 then
            reset = false
            break
        end
    end
    if reset then
        for i = 1, 8 do
            if self.cur_rotary_table_state[i] == 0 then
                self.resIndex = i
            end
        end
    else
        for i = 1, 8 do
            if self.cur_rotary_table_state[i] ~= table_state[i-1] then
                self.resIndex = i
                break
            end
        end
    end
    self.cur_rotary_table_state = {}
    for i = 1, 8 do
        table.insert(self.cur_rotary_table_state,table_state[i-1])
    end

    --LOG(self.resIndex,"本次抽奖的奖励")

    local data = self.Data.show_reward_group_list[self.resIndex]["item"]
    local reward_item = Item.Create(data);

    
    self.curIndex = self.resIndex - 4 <= 0 and (8 - (4-self.resIndex)) or self.resIndex - 4


    for i = 1, 4 do
        if self.data.lottery_times >= self.data.lottery_reward_times[i] and self.data.lottery_reward_flag[i-1] ~= 1 then
            self.CountEff:SetActive(true)
        end
    end
    if self.data.last_lottery_times < 1 and self.data.taday_recharge < 120 then
        UH.SetText(self.DrawBtnTxt,Language.PetWeaponFanLi.QianWangChongZhi)
        self.DrawEffect:SetActive(false)
    elseif self.data.last_lottery_times < 1 and self.data.taday_recharge >= 120 then
        UH.SetText(self.DrawBtnTxt,Language.PetWeaponFanLi.ChouJiang)
        self.DrawEffect:SetActive(false)
        self.DrawBtnInter.Interactable = false
        self.JInRIWanChengDi:SetActive(true)
    else
        UH.SetText(self.DrawBtnTxt,Language.PetWeaponFanLi.ChouJiang)
        self.DrawEffect:SetActive(true)
    end


    --还没抽奖的格子
    local canrunIndex = {}
    local ti = 1
    for i = 1, 8 do
        if self.ToggleList[i].isOn == false then
            canrunIndex[ti] = i
            ti = ti + 1
        end
    end
    self.item_eff = {}
    --只剩下一个没抽时，直接获取当前物品并对轮盘状态进行重置
    if #canrunIndex == 1 then
        self.ToggleList[self.resIndex].isOn = true;
        self.item_eff[self.resIndex] = self.ItemEffList[self.resIndex]:Play("6165232")
        self.TickList[self.resIndex]:SetActive(true);

        TimeHelper:CancelTimer(self.one_round)
        self.one_round = TimeHelper:AddDelayTimer(function ()
        ActRewardTipData.Instance:OpenActRewardTipViewByData(ServeractivityConfig.ActRewerdTip.RewardType.Success,{reward_item})
        PublicPopupCtrl.Instance:CenterI({item_id = reward_item.item_id, num = reward_item.num,
        color_str = reward_item:ColorStr(),item = reward_item,icon_id = reward_item:IconId()})
        ChatCtrl.Instance:ChannelChatCustom{item_get = {item = reward_item, num = reward_item.num}}
        for i = 1, 8 do
            self.ToggleList[i].isOn = false;
            self.TickList[i]:SetActive(false);
        end
        Yi_Chou = 0;
        self.is_drawing = false
        end,0.5,nil,false)
        return;
    end
    --找到和起点最近的还没抽到的格子（包括起点本身）
    while self.ToggleList[self.curIndex].isOn do
        self.curIndex = self.curIndex + 1 > 8 and 1 or self.curIndex + 1
    end
    --记录当前这个格子到还没抽奖的格子的距离，将动画播放次数设置为两个格子的距离加上还可以抽到的物品的数量（也就是轮一圈）
    local count = 0;
    --起点小于终点，顺序记录  终点小于起点的需要进行过圈记录
    if self.curIndex < self.resIndex then
        for i = self.curIndex, self.resIndex do
            if self.ToggleList[i].isOn == false then
                count = count + 1
            end
        end
        count = count + #canrunIndex
    else
        for i = self.curIndex, 8 do
            if self.ToggleList[i].isOn == false then
                count = count + 1
            end
        end
        for i = 1, self.resIndex do
            if self.ToggleList[i].isOn == false then
                count = count + 1
            end
        end
        count = count + #canrunIndex
    end

    --增加抽奖动画，显示完之后过0.1s消失    快速动画
    local fast_draw = (count-1)/2
    fast_draw = math.floor(fast_draw)
    local slow_draw = (count-1) - fast_draw
    -- LOG(fast_draw,"快和慢次数",slow_draw)
    -- LOG(count,"总次数")
    local res = fast_draw * 0.2 + slow_draw * 0.3 + 0.4
    TimeHelper:CancelTimer(self.drawing)
    self.drawing = TimeHelper:AddRunTimer(function ()
        --判断当前这个格子的奖励是否已经拿到，跳过已经获得的格子播放动画
        while self.ToggleList[self.curIndex].isOn do
            self.curIndex = self.curIndex + 1 > 8 and 1 or self.curIndex + 1
        end
        self.item_eff[self.curIndex] = self.ItemEffList[self.curIndex]:Play("6165232")
        local t = self.curIndex
        TimeHelper:AddDelayTimer(function ()
            --这里直接用self.curIndex会导致闭包
            self.ItemEffList[t]:Stop(self.item_eff[t])
        end,0.1 ,nil,false)
        --更新下一个动画位置
        self.curIndex = self.curIndex + 1 > 8 and 1 or self.curIndex + 1
    end,0.2,fast_draw,false)

    TimeHelper:AddDelayTimer(function ()
        --增加抽奖动画，显示完之后过0.1s消失    慢速动画
        TimeHelper:CancelTimer(self.drawing)
        self.drawing = TimeHelper:AddRunTimer(function ()
            --判断当前这个格子的奖励是否已经拿到，跳过已经获得的格子播放动画
            while self.ToggleList[self.curIndex].isOn do
                self.curIndex = self.curIndex + 1 > 8 and 1 or self.curIndex + 1
            end
            self.item_eff[self.curIndex] = self.ItemEffList[self.curIndex]:Play("6165232")
            local t = self.curIndex
            TimeHelper:AddDelayTimer(function ()
                --这里直接用self.curIndex会导致闭包
                self.ItemEffList[t]:Stop(self.item_eff[t])
            end,0.2 ,nil,false)
            --更新下一个动画位置
            self.curIndex = self.curIndex + 1 > 8 and 1 or self.curIndex + 1
        end,0.3,slow_draw,false)
    end,fast_draw*0.2 ,nil,false)


    --执行动画后设置抽到的奖品格为已抽奖
    TimeHelper:AddDelayTimer(function ()
        self.ToggleList[self.resIndex].isOn = true;
        self.item_eff[self.resIndex] = self.ItemEffList[self.resIndex]:Play("6165232")
        self.TickList[self.resIndex]:SetActive(true);
        Yi_Chou = Yi_Chou + 1;
        self.is_drawing = false
        ActRewardTipData.Instance:OpenActRewardTipViewByData(ServeractivityConfig.ActRewerdTip.RewardType.Success,{reward_item})
        PublicPopupCtrl.Instance:CenterI({item_id = reward_item.item_id, num = reward_item.num,
        color_str = reward_item:ColorStr(),item = reward_item,icon_id = reward_item:IconId()})
        ChatCtrl.Instance:ChannelChatCustom{item_get = {item = reward_item, num = reward_item.num}}
    end,res ,nil,false)
end



PetWeaponFanLiPreView = PetWeaponFanLiPreView or DeclareView("PetWeaponFanLiPreView","reward/pet_weapon_fanli_pre")
VIEW_DECLARE_MASK(PetWeaponFanLiPreView,ViewMask.BgBlockClose)
function PetWeaponFanLiPreView:PetWeaponFanLiPreView()
    self.data = RewardData.Instance.show_reward_item_list
end

function PetWeaponFanLiPreView:LoadCallback()
    for i=1 , #self.data do
        local Itemdata = self.data[i]["item"]
        local item = Item.Create(Itemdata);
        self.ItemList[i]:SetData(item);
        UH.SetText(self.CountList[i],self.data[i]["number"] .. Language.PetWeaponFanLi.Times)
    end
end