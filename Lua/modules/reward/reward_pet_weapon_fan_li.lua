PetWeaponFanLiView = PetWeaponFanLiView or DeclareMono("PetWeaponFanLiView",UIWFlushPanel)

function PetWeaponFanLiView:PetWeaponFanLiView()
    self.data = PetWeaponFanliData.Instance
    self.data_cares = {
        {data = self.data.info_data, func = self.FlushPanel, init_call = false},
        {data = self.data.new_info_data, func = self.FlushPlay, init_call = false},
    }
end

function PetWeaponFanLiView:Awake()
    UIWFlushPanel.Awake(self)
    self.data.remind_data.first_open_flag = true
    self:FlushPanel()
end

function PetWeaponFanLiView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.playing then
        self:PlayEndFunc()
    end
    if self.item_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.item_timer)
        self.item_timer = nil
    end
    PetWeaponFanliData.Instance.stage_cfg = nil
end

function PetWeaponFanLiView:FlushPanel()
    local list_data = self.data:GetItemListData()
    for i = 1,  self.ItemList:Length() do
        local mono = self.ItemList[i]
        if list_data[i] then
            mono:SetData(list_data[i])
        end
    end

    UH.SetText(self.RemainCount, self.data:GetRemainCount())
    self.PlayEffectObj:SetActive(not self.playing and self.data:GetRemainCount() > 0)
    if not self.playing and self.data:GetRemainCount() > 0 then
        self.KuangAni:Play()
    else
        self.KuangAni:Stop()
    end
    self.WanChengDi:SetActive(self.data:IsDayFull())
    UH.SetInter(self.PlayBtnInter, not self.data:IsDayFull())

    --阶段
    local cur_stage = self.data:CurStageCfg()
    UH.SetText(self.StageNeedCount, cur_stage.number)
    local flag = self.data:GetStageFlag(cur_stage)
    self.LingQuBtn:SetActive(flag == 1)
    self.StageFlag:SetActive(flag == -1)
    self.Item:SetData(Item.Create(cur_stage.reward1_item))

    --充值
    local is_cz_full = self.data:IsCZFull()
    self.MingRiChongZhi:SetActive(is_cz_full)
    self.ChongZhi:SetActive(not is_cz_full)
    local cur_cz_cfg = self.data:CurCZCfg()
    if is_cz_full then
        UH.SetText(self.CZGetTimes2, cur_cz_cfg.number .. Language.PetWeaponFanLi.Tip3)
    else
        UH.SetText(self.CZGetTimes1, cur_cz_cfg.number .. Language.PetWeaponFanLi.Tip3)
    end
    self.SpiteFee.text = cur_cz_cfg.price / 10

    local btn_text = self.data:GetRemainCount() > 0 and Language.PetWeaponFanLi.ChouJiang or Language.PetWeaponFanLi.QianWangChongZhi
    UH.SetText(self.BtnText, btn_text)
end

function PetWeaponFanLiView:FlushPlay()
    local seq = self.data.play_seq
    --LogDG("seq", seq)
    if not seq or seq < 0 then
        return
    end
    self.play_seq = seq
    self:Play(seq)
end

-- 获取那些还未被抽中的items
function PetWeaponFanLiView:GetCanPlayItemList(seq)
    local list = {}
    local t_index = 0
    for i = 1, self.ItemList:Length() do
        local data = self.ItemList[i]:GetData()
        if self.data:GetItemState(data) == 0 then
            table.insert(list, self.ItemList[i])
        end
        if data.seq == seq then
            t_index = #list
        end
    end
    return list,t_index
end

-- 转盘动效，index转到哪个item上
function PetWeaponFanLiView:Play(seq)
    local list,t_index = self:GetCanPlayItemList(seq)
    if TableIsEmpty(list) then
        return
    end
    if t_index == 0 then
        LogDG("抽奖异常 t_index == 0")
        return
    end
    --LogDG("t_index", t_index)
    if #list == 1 then
        list[1]:SetSelected(true)
        self:PlayEndFunc()
        return
    end
    self.play_item_list = list
    self.item_index = 1
    self.t_index = t_index

    self.PlayEffectObj:SetActive(false)
    self.KuangAni:Stop()
    self.playing = true
    self.speed = 0
    self.start_speed = 0            -- 开始速度
    self.min_speed = 0.08            -- 最小速度
    self.max_speed = 0.4              -- 最大速度
    self.accelerated_speed = 0.02   -- 加速度
    self.process_time = 3           -- 过程时间
    self.start_time = Time.time
    self.is_stop = false
    if self.item_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.item_timer)
    end
    self.item_timer = Runner.Instance:RunUntilTrue(BindTool.Bind(self.PlayFunc,self))
end

function PetWeaponFanLiView:PlayFunc()
    -- 减速处理
    if Time.time - self.start_time >= self.process_time and not self.is_stop and self.accelerated_speed > 0 then
        self.accelerated_speed = -self.accelerated_speed
    end
    if not self.is_stop then
        self.speed = self.speed + self.accelerated_speed
        if self.accelerated_speed < 0 then
            -- 减到最小速度后，开始锁定目标吧
            if self.speed <= self.min_speed then
                self.speed = self.min_speed
                self.is_stop = true
            end
        else
            self.speed = self.speed > self.max_speed and self.max_speed or self.speed
        end
    end
    self.start_speed = self.start_speed + self.speed
    if self.start_speed >= 1 then
        self.item_index = self.item_index + 1
        self.item_index = self.item_index > #self.play_item_list and 1 or self.item_index
        self.play_item_list[self.item_index]:SetSelected(true)
        self.start_speed = 0
        -- 转盘停止
        if self.is_stop == true and self.item_index == self.t_index then
            self:PlayEndFunc()
            return true
        end
    end
end

function PetWeaponFanLiView:PlayEndFunc()
    self.playing = false
    if not self.play_seq then
        return
    end
    local cfg = self.data:GetItemDataBySeq(self.play_seq)
    local reward_item = Item.Create(cfg.reward1_item)
    ActRewardTipData.Instance:OpenActRewardTipViewByData(ServeractivityConfig.ActRewerdTip.RewardType.Success,{reward_item})
    PublicPopupCtrl.Instance:CenterI({item_id = reward_item.item_id, num = reward_item.num,
        color_str = reward_item:ColorStr(),item = reward_item,icon_id = reward_item:IconId()})
    ChatCtrl.Instance:ChannelChatCustom{item_get = {item = reward_item, num = reward_item.num}}
    self.data:UseNewInfo()
end

function PetWeaponFanLiView:OnPlayClick()
    if self.playing then
        PublicPopupCtrl.Instance:Center(Language.PetWeaponFanLi.Drawing)
        return
    end

    local cur_stage = self.data:CurStageCfg()
    local flag = self.data:GetStageFlag(cur_stage)
    if flag == 1 then
        PublicPopupCtrl.Instance:Center(Language.PetWeaponFanLi.Tip1)
        return
    end

    local remain_num = self.data:GetRemainCount()
    if remain_num > 0 then
        -- 发送抽奖请求
        self.playing = true
        RewardCtrl.Instance:PetWeaponFanLiSend(PetWeaponFanliCtrl.OpType.Lottery)
        return
    end

    if self.data:IsCZFull() then
        PublicPopupCtrl.Instance:Center(Language.PetWeaponFanLi.Tip2)
    else
        ViewMgr:OpenViewByKey(Mod.Store.Recharge)
    end
end

function PetWeaponFanLiView:OnRewardLingQu()
    if self.playing then
        PublicPopupCtrl.Instance:Center(Language.PetWeaponFanLi.Drawing)
        return
    end
    local cur_stage = self.data:CurStageCfg()
    local flag = self.data:GetStageFlag(cur_stage)
    if flag == 1 then 
        PetWeaponFanliData.Instance.stage_cfg = cur_stage
        RewardCtrl.Instance:PetWeaponFanLiSend(PetWeaponFanliCtrl.OpType.Lottery_Count_Reward,cur_stage.seq)
    end
end

function PetWeaponFanLiView:OnClickPre()
    ViewMgr:OpenView(PetWeaponFanLiPreView)
end


PetWeaponFanLiItem = PetWeaponFanLiItem or DeclareMono("PetWeaponFanLiItem", UIWidgetBaseItem)
function PetWeaponFanLiItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data.reward1_item)
    self.Item:SetData(item)
    self.Flag:SetActive(data.mark == 1)
    local rate = ColorStr(data.show_rate * 100 .. "%", COLORSTR.Red5)
    UH.SetText(self.Rate, string.format(Language.PetWeaponFanLi.Tip4, rate))
    local flag = PetWeaponFanliData.Instance:GetItemState(data)
    self.Mask:SetActive(flag == 1)
end


PetWeaponFanLiPreView = PetWeaponFanLiPreView or DeclareView("PetWeaponFanLiPreView","reward/pet_weapon_fanli_pre")
VIEW_DECLARE_MASK(PetWeaponFanLiPreView,ViewMask.BgBlockClose)
function PetWeaponFanLiPreView:PetWeaponFanLiPreView()
    self.data = PetWeaponFanliData.Instance
end

function PetWeaponFanLiPreView:LoadCallback()
    local data_list = self.data:GetStageCfg()
    self.List:SetData(data_list)
end

PetWeaponFanLiPreItem = PetWeaponFanLiPreItem or DeclareMono("PetWeaponFanLiPreItem", UIWidgetBaseItem)
function PetWeaponFanLiPreItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Item:SetData(Item.Create(data.reward1_item))
    local times_str = ColorStr(data.number .. Language.PetWeaponFanLi.Tip3, COLORSTR.Purple4)
    UH.SetText(self.Count, string.format(Language.PetWeaponFanLi.Tip5, times_str))
end