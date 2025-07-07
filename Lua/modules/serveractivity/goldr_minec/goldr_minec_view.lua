GoldRMinecView = GoldRMinecView or DeclareView("GoldRMinecView", "serveractivity/goldr_minec/goldr_minec", Mod.GoldRMineC.Main)
VIEW_DECLARE_LEVEL(GoldRMinecView, ViewLevel.L0)

local AnimType = {
    Hide = 0,
    Show = 1,
    Get = 2,
}

function GoldRMinecView:GoldRMinecView()
	self.AudioOpen = CommonAudio.ViewOpenL1
    self.AudioClose = CommonAudio.ViewCloseL1
end

function GoldRMinecView:LoadCallback(param_t)
    GoldRMinecCtrl.Instance:SendInfo()
end

function GoldRMinecView:OnClickClose()
    ViewMgr:CloseView(GoldRMinecView)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end


--===========================GoldRMinecPanel===========================
GoldRMinecPanel = GoldRMinecPanel or DeclareMono("GoldRMinecPanel", UIWFlushPanel)
function GoldRMinecPanel:GoldRMinecPanel()
    self.data = GoldRMinecData.Instance
    self.data_cares = {
        {data = self.data.act_info, func = self.MainFlush, keys = {"flush"},init_call = false},
        {data = self.data.view_data, func = self.StartShield, keys = {"start_shield"},init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushBagChange},
    }

    self.act_timer:CreateActTime(ACTIVITY_TYPE.RAND_JIN_LONG_JU_BAO,TimeType.Type_Special_4,Language.FlowerTouch.ActTime)

    -- if not self.data:GetSelectComplete() then 
    --     ViewMgr:OpenView(GoldrMinecRareSelectView)
    -- end
    self.activity_sp = {
        [0] = "_Loc_QiTianDaSheng",
        "_LocTianShuiLingLongZi",
        "_LocTianWuJinLongZi"
    }
end 

function GoldRMinecPanel:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
    
end

function GoldRMinecPanel:OnFlush()
end

function GoldRMinecPanel:MainFlush()
    local item_id = self.data:GetOtherCfg().consume_item
    UH.SetIcon( self.icon, Item.GetIconId(item_id))
    UH.SetText( self.item_num, "x"..Item.GetNum(item_id))
    local flag = self.data:GetSelectComplete() 
    self.oper_eff:SetActive(Item.GetNum(item_id) > 0 or not flag)
    UH.SetText( self.btn_text, flag and Language.GoldRMinec.BtnOperateDraw or Language.GoldRMinec.BtnOperateSelect)

    self.red_point = self.red_point or UiRedPoint.New(self.red_pos, Vector3.New(0,0,0))
	self.red_point:SetNum(flag and 0 or 1)
    if flag == true then
        self.red_point:SetNum(Item.GetNum(item_id)> 0 and 1 or 0)
    end

    local a_type = self.data:ActivityType()
    UH.SpriteName(self.TitleSp, self.activity_sp[a_type])
    for i = 1, 3, 1 do
        self.EffectList[i]:SetActive((a_type + 1) == i)
    end
    for i = 1, 3, 1 do
        self.TitleList[i]:SetActive((a_type + 1) == i)
    end
end

function GoldRMinecPanel:FlushBagChange()
    self:MainFlush()
end 

function GoldRMinecPanel:OnClickRate()
    ViewMgr:OpenView(GoldRMinecPreView)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function GoldRMinecPanel:OnClickShop()
    ViewMgr:OpenView(GoldrMinecShopView)    
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function GoldRMinecPanel:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[332].desc})
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function GoldRMinecPanel:OnClickRecord()
    ViewMgr:OpenView(GoldrMinecRecordView)    
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function GoldRMinecPanel:OnClickDraw()

    -- 选奖
    if not self.data:GetSelectComplete() then 
        PublicPopupCtrl.Instance:Center(Language.GoldRMinec.DrawErrorSelect)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)

        ViewMgr:OpenView(GoldrMinecRareSelectView)
        return 
    end 

    if Item.GetNum(self.data:GetOtherCfg().consume_item) == 0 then 
        PublicPopupCtrl.Instance:Center(string.format(Language.GoldRMinec.DrawErrorLack,
            Item.GetName(self.data:GetOtherCfg().consume_item)))
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)

        self:OnClickShop()
        
        return
    end 

    -- 是否展示
    if self.data:ShouldSendShow() then 
        ViewMgr:OpenView(GoldrMinecCountView)
        return 
    end

    -- LogError("?!@~",self.data.send_type)

    -- 是十连 但不够十连
    if self.data:IsDirectShow() and not self.data:CheckEnoughDirect() then 
        local item_show = ColorStr(string.format(Language.GoldRMinec.ItemShow
        ,Item.GetName(self.data:GetOtherCfg().consume_item))
        ,Item.GetColorStr(self.data:GetOtherCfg().consume_item))

        local info = {            
            content = string.format(Language.GoldRMinec.LackTen,item_show),
            cancel = {
                name = nil,
                func = function ()
                   self.data:SetSendType(1) -- 变回1连
                   ViewMgr:CloseView(DialogTipsView)
                end
            },
            confirm = {
                name = nil,
                func = function()
                    self:OnClickShop() -- 不变化 打开商店
                    ViewMgr:CloseView(DialogTipsView)
                end
            }
        }
        PublicPopupCtrl.Instance:DialogTips(info)

        return 
    end 

    if self.is_drawing then 
        return 
    end 

    GoldRMinecCtrl.Instance:SendDraw()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    self:StartShield()
end 

function GoldRMinecPanel:OnClickJump()
    self.jump:SetActive(not self.jump.activeSelf)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end 

-- 上锁
function GoldRMinecPanel:StartShield()
    self.is_drawing = true
    SocietyData.Instance.is_mail_shield_tip = true
    AppearanceCtrl.Instance.is_shield_appear_show = true
    RoleCtrl.Instance.is_shield_title_show = true
    RoleCtrl.Instance.is_shield_currency = true
end

-- 解除屏蔽
function GoldRMinecPanel:ClearShield()
    self.is_drawing = false
    SocietyData.Instance.is_mail_shield_tip = false
    if SocietyData.Instance.mail_shield_tip_list and next(SocietyData.Instance.mail_shield_tip_list) then
        SocietyData.Instance:ReleaseMailShieldTip()
    end
    AppearanceCtrl.Instance:ReleaseShieldParam()
    RoleCtrl.Instance:ReleaseShieldTitle()
    RoleCtrl.Instance.is_shield_currency = false
end

function GoldRMinecPanel:OnClickDrawIcon()
    local other_cfg = Config.jinlongjubao_auto.other[1]
    local item = Item.Init(other_cfg.consume_item)
    local info = {
		item_list = {item}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
end

--=======================GoldRMinecOperatePart============================
GoldRMinecOperatePart = GoldRMinecOperatePart or DeclareMono("GoldRMinecOperatePart", UIWFlushPanel)
function GoldRMinecOperatePart:GoldRMinecOperatePart()
    self.data = GoldRMinecData.Instance

    self.data_cares = {
        {data = self.data.act_info, func = self.MainFlush, keys = {"flush"},init_call = false},
        {data = self.data.view_data, func = self.MainFlush, keys = {"flush"},init_call = false},
        {data = self.data.view_data, func = self.ShowAnim, keys = {"show_anim"},init_call = false},
    }

    self.cards[1]:FlushCorn(1)
    self.cards[5]:FlushCorn(2)
    self.cards[7]:FlushCorn(3)
    self.cards[11]:FlushCorn(4)
end 

function GoldRMinecOperatePart:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
    self.data:ClearDrawRet()
    TimeHelper:CancelTimer(self.handle_flush)
    TimeHelper:CancelTimer(self.handle_jump)
end

function GoldRMinecOperatePart:MainFlush()
    if self.data:WithDrawRet() then 
        if self.data:IsDirectShow() then 
            self:DirectShow()
            return 
        end 

        if self.panel.jump.activeSelf then 
            self:JumpAnim()
        else 
            self:PlayAnim()
        end  
        return 
    end 
    local list = self.data:GetCardList()
    local show_flag = self.data:GetSelectComplete()
    for i = 1,12 do 
        self.cards[i]:SetData(list[i]) 
        self.cards[i]:PlayAnim(show_flag and AnimType.Get or AnimType.Hide)
        -- self.cards[i]:SetSelect(false)
        self.cards[i]:ShowComplete()
    end 

    if not show_flag then 
        ViewMgr:OpenView(GoldrMinecRareSelectView)

        for i = 1,12 do 
            self.cards[i]:SetSelect(false)
        end 
    end 

    self.progress:MainFlush()
end

function GoldRMinecOperatePart:ShowAnim()
    if not self.data.view_data.show_anim then 
        return 
    end 
    for i = 1,12 do 
        self.cards[i]:PlayAnim(AnimType.Show)
    end 
end

function GoldRMinecOperatePart:FlushView()
    for i = 1,12 do 
        -- self.cards[i]:SetSelect(false)
    end 

    for i = 1,12 do 
        self.cards[i]:ShowComplete()
    end 
end

function GoldRMinecOperatePart:JumpAnim()
    if self.is_animing then 
        return 
    end 
    self.is_animing = true
    local target = self.data:GetDrawRet()
    for i= 1,12 do 
        self.cards[i]:SetSelect(target == i)
    end 
    
    self.block:SetActive(true)
    if self.handle_jump ~= nil then
        TimeHelper:CancelTimer(self.handle_jump)
        self.handle_jump = nil
    end  
    self.handle_jump = TimeHelper:AddDelayTimer(function ()
        self:FlushView()
        self:FlyText()
    end, 1)

    if self.handle_flush ~= nil then
        TimeHelper:CancelTimer(self.handle_flush)
        self.handle_flush = nil
    end  
    self.handle_flush = TimeHelper:AddDelayTimer(function ()
        self.is_animing = false
        self.block:SetActive(false)
    end,1.5)
end

function GoldRMinecOperatePart:PlayAnim()
    if self.is_animing then 
        return 
    end
    
    self.is_animing = true
    local target = self.data:GetDrawRet()
    local roll_show_list = self:SimulateAnim(target)
    self.block:SetActive(true)
    local cur_index = 0
    local time_step = 0.1
    local total_time = time_step*#roll_show_list
    local pass_time = 0
    local pass_auido = false
    self.show_timer = TimeHelper:AddCountDownTT(function()
        if self.cards ~= nil then 
            cur_index = cur_index + 1
            for i = 1,12 do 
                self.cards[i]:SetSelect(roll_show_list[cur_index] == i)  
            end 
            pass_time = time_step*cur_index

            if (total_time - pass_time) <= 3 and not pass_auido then 
                AudioMgr:PlayEffect(AudioType.UI, "gongfangzhizao")
                pass_auido = true
            end 
        end 
    end,
    function ()
        if self.cards ~= nil then 
            for i = 1,12 do 
                self.cards[i]:SetSelect(target == i)
            end 

            if self.handle_jump ~= nil then
                TimeHelper:CancelTimer(self.handle_jump)
                self.handle_jump = nil
            end  
            self.handle_jump = TimeHelper:AddDelayTimer(function ()
                self:FlushView()
                self:FlyText()
            end, 1)

            if self.handle_flush ~= nil then
                TimeHelper:CancelTimer(self.handle_flush)
                self.handle_flush = nil
            end  
            self.handle_flush = TimeHelper:AddDelayTimer(function ()
                
                self.is_animing = false
                self.block:SetActive(false)
            end,1.5)
        end 
    end,total_time,time_step)
end

function GoldRMinecOperatePart:SimulateAnim(show_seq)
    local simulate_list = {}
    -- 正常旋转两圈
    for j = 1,2 do 
        for i = 1,12 do 
            table.insert(simulate_list,i)
        end 
    end 
    -- 减速
    -- 前半圈
    if show_seq <= 6 then 
        for i = 1,show_seq-2 do
            for j = 1,2 do 
                table.insert(simulate_list,i)
            end 
        end 
        for i = show_seq-2,show_seq do
            for j = 1,3 do 
                table.insert(simulate_list,i)
            end 
        end 
    else
        for i = 1,6 do
            table.insert(simulate_list,i)
        end 
        -- 满足双减速
        if show_seq - 2 >= 6 then 
            for i = 6,show_seq -2 do 
                for j = 1,2 do 
                   table.insert(simulate_list,i)
                end 
            end 
            for i = show_seq-2,show_seq do
                for j = 1,3 do 
                    table.insert(simulate_list,i)
                 end 
            end 
        else 
            for i = 6,show_seq do 
                for j = 1,3 do 
                    table.insert(simulate_list,i)
                end 
            end 
        end 
    end 
    -- LogError("?模拟结果！",simulate_list)
    return simulate_list
end

function GoldRMinecOperatePart:FlyText()

    local target = self.data:GetDrawRetCfg()
    if target== nil then 
        return
    end 

    local target_index = self.data:GetDrawRet()
    if target_index == 1 or target_index == 5 or target_index == 7 or target_index == 11 then 
        local item_info = Item.Create(target.reward_item)
        ViewMgr:OpenView(GoldrMinecRewardView, {item_list = {[1]= target.reward_item},close_func = BindTool.Bind1(self.MainFlush,self)})

    else 
        local item = Item.Create(target.reward_item)
        ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item, num = target.reward_item.num}}
        PublicPopupCtrl.Instance:CenterI({item_id = target.reward_item.item_id, num = target.reward_item.num,
            color_str = item:ColorStr(),item = item,icon_id = item:IconId()})

        self.data:ClearDrawRet()
        self:MainFlush()
    end 

    -- ViewMgr:OpenView(GiftOpenView,{item = item_info,gift_type = GIFT_OPEN_TYPE.CONGRATULATIONS,
    --     close_func = BindTool.Bind1(self.MainFlush,self)})
    
    self.panel:ClearShield()
end

function GoldRMinecOperatePart:DirectShow()
    
    local flag_show = false
    
    for k,v in pairs(self.data.view_data.direct_list) do 
        if self.data:IsSelectType(v) then 
            flag_show = true
        end 
    end 
    

    if flag_show then 
        local list = {}
        for k,v in pairs(self.data.view_data.direct_list) do 
            if self.data:IsSelectType(v) then 
                table.insert(list,v.reward_item)
            end 
        end 

        ViewMgr:OpenView(GoldrMinecRewardView, {item_list = list,close_func = BindTool.Bind1(self.DirectWait,self)})
    end 

    local list = {}
    for k,v in pairs(self.data.view_data.direct_list) do 
        if not self.data:IsSelectType(v) then 
            table.insert(list,v.reward_item)
        end 
    end 

    local commit_list = {}
    for k,v in pairs(list) do 
        local flag_with = false 
        for i,j in pairs(commit_list) do 
            if j.item_id == v.item_id then 
                flag_with = true
                j.num = v.num + j.num
                break
            end 
        end 
        if not flag_with then 
            table.insert(commit_list,v)
        end 
    end 
    for k,v in pairs(commit_list) do 
        local item = Item.Create(v)
        ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item, num = item:Num()}}
        PublicPopupCtrl.Instance:CenterI({item_id = item.item_id, num = item:Num(),
            color_str = item:ColorStr(),item = item,icon_id = item:IconId()})
    end 

    if not flag_show then 
        self:DirectWait()
    end 
end

function GoldRMinecOperatePart:DirectWait()

    self.data:ClearDrawRet()
    self.panel:ClearShield()    

    self:MainFlush()
end

GoldrMinecMainCardCell = GoldrMinecMainCardCell or DeclareMono("GoldrMinecMainCardCell", UIWidgetBaseItem)
function GoldrMinecMainCardCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local cfg = GoldRMinecData.Instance:GetRewardCfg(data.reward_group_id,data.reward_seq)
    if cfg == nil then 
        return 
    end 

    local cell = Item.Create(cfg.reward_item)

    self.call:SetData(cell)
end 

function GoldrMinecMainCardCell:FlushCorn(pos)
    if self.corn ~= nil then 
        UH.SetText(self.corn_text,Config.jinlongjubao_auto.draw_label[pos].desc)
    end 
end

function GoldrMinecMainCardCell:ShowComplete()
    if self.complete == nil then return end 
    -- LogError("!?@~~~",self.data)
    self.complete:SetActive(self.data.is_select == 1 and self.data.is_get == 1)
    -- self.inter.Interactable = self.data.is_select == 1 and self.data.is_get == 0
end

function GoldrMinecMainCardCell:PlayAnim(status)
    if self.corn ~= nil then 
        self.corn:SetActive(status == 2)
    end 
    if status ~= nil then 
        self.anim:SetInteger(APH("status"), status)
    end 
end

function GoldrMinecMainCardCell:SetSelect(flag)
    self.select:SetActive(flag)
end

--========================GoldRMinecRewardPart====================================
GoldRMinecRewardPart = GoldRMinecRewardPart or DeclareMono("GoldRMinecRewardPart", UIWFlushPanel)
function GoldRMinecRewardPart:GoldRMinecRewardPart()
    self.data = GoldRMinecData.Instance

    self.data_cares = {
        -- {data = self.data.act_info, func = self.MainFlush, keys = {"flush"},init_call = false},
        -- {data = BagData.Instance.item_grid, func = self.FlushBagChange},
    }

    self.progress_rate = {
        [1] = {start = 0, done = 0.08},
        [2] = {start = 0.08, done = 0.16},
        [3] = {start = 0.26, done = 0.35},
        [4] = {start = 0.45, done = 0.54},
        [5] = {start = 0.64, done = 0.72},
        [6] = {start = 0.83, done = 0.91},
    }

end 

function GoldRMinecRewardPart:InitProgress()
    if self.init then return end 
    self.progress_times = {}
    for i = 1,self.rewards:Length() do 
        self.progress_times[i] =  self.rewards[i]:GetData().times
    end 
    self.init = true
end

function GoldRMinecRewardPart:MainFlush()
    local list = self.data:GetRoundRewardList()

    for i = 1,self.rewards:Length() do 
        local flag = list[i] ~= nil 
        if flag then 
            self.rewards[i]:SetData(list[i])
        end 
    end

    self:InitProgress()

    UH.SetText(self.round,self.data:GetCurRound())
    local rate = self:GetProgressRound() -- self.data:GetCurRound() >= 12 and 1 or self.progress_rate[self.data:GetCurRound()]
    self.progress:SetProgress(rate)
end

function GoldRMinecRewardPart:GetProgressRound()
    local cur_time = self.data:GetCurRound()

    local mark = 0
    for i = 1,self.rewards:Length() do 
        if self.progress_times[i] > cur_time then 
            mark = i
            break
        end 
    end 

    if mark == 0 then return 1 end 
    
    local last = self.progress_times[mark - 1] ~= nil and self.progress_times[mark - 1] or 0
    local total = (cur_time -last)/(self.progress_times[mark]- last) * (self.progress_rate[mark].done - self.progress_rate[mark].start)

    return total + self.progress_rate[mark].start
end

function GoldRMinecRewardPart:OnClickRound(data)
    -- if not self.data:IsCanRoundReward(data) then 
    --     PublicPopupCtrl.Instance:Center(Language.GoldRMinec.ProgressErrorTimes)
    --     AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
    --     return 
    -- end 

    -- if self.data:IsPassRoundReward(data) then 
    --     PublicPopupCtrl.Instance:Center(Language.GoldRMinec.ProgressErrorPass)
    --     AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
    --     return 
    -- end 
    if not self.data:IsCanRoundReward(data) or self.data:IsPassRoundReward(data) then 
        local item = Item.Create(data.reward_item)
        if item:ShowType() == ShowType.Pet then
            PetCtrl.Instance:OpenActPetDetailView(Pet.New({item_id = item.item_id}))
            return
        end
        local info = {
            item_list = {item}
        }
        ItemInfoCtrl.Instance:ItemInfoView(info)
        return 
    end


    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    GoldRMinecCtrl.Instance:SendRoundReward(data.seq)
end

GoldrMinecMainRewardCell = GoldrMinecMainRewardCell or DeclareMono("GoldrMinecMainRewardCell",UIWidgetBaseItem)
function GoldrMinecMainRewardCell:GoldrMinecMainRewardCell() end
function GoldrMinecMainRewardCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.call:SetData(Item.Create(data.reward_item))
    UH.SetText(self.text,data.times..Language.GoldRMinec.Ci)
    UH.SetText(self.text_s,data.times..Language.GoldRMinec.Ci)
    self.select:SetActive(GoldRMinecData.Instance:IsCanRoundReward(data))
    self.complete:SetActive(GoldRMinecData.Instance:IsPassRoundReward(data))
    self.wait_eff:SetActive(not GoldRMinecData.Instance:IsPassRoundReward(data) and GoldRMinecData.Instance:IsCanRoundReward(data))
end 
