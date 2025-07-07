MountainSeaMineCView = MountainSeaMineCView or DeclareView("MountainSeaMineCView", "serveractivity/mountain_sea/mountain_sea_minec", Mod.MountainSea.MineCollect)
-- VIEW_DECLARE_LEVEL(MountainSeaMineCView, ViewLevel.L0)

function MountainSeaMineCView:MountainSeaMineCView()
    
end

function MountainSeaMineCView:SetTimerCloseView()

end

function MountainSeaMineCView:OnClickBack()
    ViewMgr:CloseView(MountainSeaMineCView)
end

MountainSeaMineCPanel = MountainSeaMineCPanel or DeclareMono("MountainSeaMineCPanel", UIWFlushPanel)
function MountainSeaMineCPanel:MountainSeaMineCPanel()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        -- {data = self.data.mountain_sea_minec_data, func = self.UpdateMaster, init_call = false},
        {data = self.data.mountain_sea_minec_data, func = self.FlushRollPart},
        -- {data = BagData.Instance.item_grid, func = self.OnFlushShowView, init_call = false},
    }
    self:UpdateMaster()
    self:InitMaster()

    self:OnFlushShowView()

    self.roll_show_anim:ListenEvent("idle",
    function (eve_param,anim_st_info)
        if eve_param == "enter" then 
            self.block:SetActive(false)
        end 
    end)
    self.is_init = false

    UH.SetIcon(self.key_icon, Item.GetIconId(Config.shanhaijubao_auto.other[1].consume_item), ICON_TYPE.ITEM)

    self.act_time:SetShowCallBack(function (t)
        local time_s = ""
        if t.day > 0 then 
            time_s = string.format(Language.UiTimeMeter.TimeStr3, t.day, t.hour ,t.min)
        else 
			time_s = string.format(Language.UiTimeMeter.TimeStr1, t.hour, t.min ,t.s)
        end 
        return time_s
    end)

end

function MountainSeaMineCPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    self.data:SetMinecAnimMark(false)
    TimeHelper:CancelTimer(self.show_timer)
    TimeHelper:CancelTimer(self.anim_complete)
end 

function MountainSeaMineCPanel:OnClickAddItem()
    local key =  Item.Create({item_id = Config.shanhaijubao_auto.other[1].consume_item})
    ViewMgr:OpenView(GetWayView, {item = key})
end

function MountainSeaMineCPanel:UpdateMaster()
    self.master_data = self.data:GetMountainSeaMineCollectCfg()
    self.type_group_list = {}
    for i = 1,3 do 
        self.type_group_list[i] = KeyList(self.master_data.group_list[i],"type")
    end 

    UH.SetText(self.key_num, Item.GetNum(Config.shanhaijubao_auto.other[1].consume_item))
end

function MountainSeaMineCPanel:InitMaster()
    local cur_phase = 0
    for i = 1,#self.master_data.group_btn_list do 
        if self.master_data.group_btn_list[i].inter == true then 
            cur_phase = i
        end 
    end 
    self.send_data = {}
    self.send_data.phase = cur_phase
    self.send_data.jump = false
    self.send_data.inter = false
end

-- 总刷新
function MountainSeaMineCPanel:OnFlushShowView()
    self:FlushStaticInfo()
    self:FlushRewardShow()
end

-- 刷新转盘填充
function MountainSeaMineCPanel:FlushRollPart()
    self:UpdateMaster()
    for i = 1,8 do 
        self.roll_shows[i]:SetData(self.master_data.group_list[self.send_data.phase][i])
    end 

    for i = 1,3 do 
        self.roll_groups[i]:SetData(self.master_data.group_btn_list[i])
    end 

    if not self.is_init then 
        for i = 1,8 do 
            self.roll_shows[i]:ShowReward(false)
            self.roll_shows[i]:ShowEffect(false)
            self.roll_shows[i]:ShowSelect(false)
            self.roll_shows[i]:ShowRate(true)
        end 

        if self.roll_groups[self.send_data.phase].toggle then 
            self.roll_groups[self.send_data.phase].toggle.isOn = true

            self.send_data.phase = self.master_data.group_btn_list[self.send_data.phase].phase
            self.send_data.inter = self.master_data.group_btn_list[self.send_data.phase].inter 
            self.send_data.act_time = self.master_data.group_btn_list[self.send_data.phase].phase_time
        end 
        self.is_init = true
    end 

    local extra_info = self.data.mountain_sea_minec_data.info[self.send_data.phase]
    local extra_cfg = self.master_data.extra_info[self.send_data.phase]

    UH.SetText(self.extra_num, extra_info.total_draw_times.."/"..extra_cfg.phase_reward.lucky_draw_num)
    self.time_show:SetActive(not extra_cfg.is_all_in)
    self.time_complete:SetActive(extra_cfg.is_all_in)

    local flag_can = extra_info.total_draw_times >= extra_cfg.phase_reward.lucky_draw_num
    local flag_complete = extra_cfg.is_all_in
    self.extra_show:SetActive(flag_complete or not flag_can)
    self.extra_eff:SetActive(flag_can and not flag_complete)
    self.inter_extra.Interactable = not extra_cfg.is_all_in

    self.btn_eff:SetActive(Item.GetNum(Config.shanhaijubao_auto.other[1].consume_item) >= Config.shanhaijubao_auto.other[1].consume_num)
    self.btn_inter.Interactable = (Item.GetNum(Config.shanhaijubao_auto.other[1].consume_item) >= Config.shanhaijubao_auto.other[1].consume_num and self.send_data.inter)
    if self.send_data.mark then 
        self:ShowAnimShow()
    end 
end

function MountainSeaMineCPanel:ShowAnimShow()
    if self.data.mountain_sea_minec_data.draw_ret == nil then return end 
    local show_type = self.data.mountain_sea_minec_data.draw_ret.show_type
    local operate = self.type_group_list[self.send_data.phase][show_type]
    self.send_data.mark = false

    if self.anim_complete ~= nil then 
        TimeHelper:CancelTimer(self.anim_complete)
    end 
    for i = 1,8 do 
        self.roll_shows[i]:ShowRate(false)
    end 

    if self.send_data.jump then
        for i = 1,8 do 
            self.roll_shows[i]:ShowSelect(operate.seq + 1 == i)
            self.roll_shows[i]:ShowReward(operate.seq + 1 == i)
            self.roll_shows[i]:ShowEffect(operate.seq + 1 == i)
        end
        
        self:ShowRollRewardPoP(self.roll_shows[operate.seq + 1]:GetData().reward_item)

        self.anim_complete = TimeHelper:AddDelayTimer(function ()
            if not ViewMgr:IsOpen(MountainSeaMineCView) then 
                return 
            end 
            for i = 1,8 do 
                self.roll_shows[i]:ShowSelect(false)
                self.roll_shows[i]:ShowReward(false)
                self.roll_shows[i]:ShowEffect(false)
                self.roll_shows[i]:ShowRate(true)
            end 
        end, 3)
        return 
    end 
    self.block:SetActive(true)
    local cur_index = 0
    local roll_show_list = self:SimulateAnim(operate.seq + 1)
    self.show_timer = TimeHelper:AddCountDownTT(function()
        if self.roll_shows ~= nil then 
            cur_index = cur_index + 1

            if not ViewMgr:IsOpen(MountainSeaMineCView) then 
                return 
            end 
            for i = 1,8 do 
                self.roll_shows[i]:ShowSelect(roll_show_list[cur_index] == i)  
            end 
        end 
    end,
    function ()
        if self.roll_shows ~= nil then 
            if not ViewMgr:IsOpen(MountainSeaMineCView) then 
                return 
            end 

            self.roll_show_anim:SetTrigger(APH("show"))
            for i = 1,8 do 
                self.roll_shows[i]:ShowSelect(operate.seq + 1 == i)
                self.roll_shows[i]:ShowReward(operate.seq + 1 == i)
                self.roll_shows[i]:ShowEffect(operate.seq + 1 == i)
            end 
            self.data:SetMinecAnimMark(false)

            self:ShowRollRewardPoP(self.roll_shows[operate.seq + 1]:GetData().reward_item)


            self.anim_complete = TimeHelper:AddDelayTimer(function ()
                if not ViewMgr:IsOpen(MountainSeaMineCView) then 
                    return 
                end 
                for i = 1,8 do 
                    self.roll_shows[i]:ShowSelect(false)
                    self.roll_shows[i]:ShowReward(false)
                    self.roll_shows[i]:ShowEffect(false)
                    self.roll_shows[i]:ShowRate(true)
                end 
			end, 3)
        end 
    end,0.1*#roll_show_list,0.1)
end

function MountainSeaMineCPanel:SimulateAnim(show_seq)
    local simulate_list = {}
    -- 正常旋转两圈
    for j = 1,2 do 
        for i = 1,8 do 
            table.insert(simulate_list,i)
        end 
    end 
    -- 减速至内圈
    if show_seq >= 4 then 
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
        for i = 1,5 do
            table.insert(simulate_list,i)
        end 
        for i = 6,8 do 
            for j = 1,2 do 
               table.insert(simulate_list,i)
            end 
        end 
        for i = 1,show_seq do 
            for j = 1,3 do 
                table.insert(simulate_list,i)
             end 
        end 
    end 
    return simulate_list
end

function MountainSeaMineCPanel:ShowRollRewardPoP(data)
    local item = Item.Create(data)
    PublicPopupCtrl.Instance:CenterI({item_id = item.item_id, num = data.num,
        color_str = item:ColorStr(),item = item,icon_id = item:IconId()})
end

-- 刷新奖励预览
function MountainSeaMineCPanel:FlushRewardShow()
    local show_mode = #self.master_data.show_group_list
    self.reward_show:SetData(self.master_data.show_group_list[self.send_data.phase])
end

-- 刷新静态内容
function MountainSeaMineCPanel:FlushStaticInfo()
    self.act_time:CreateActTime(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_MINEC,TimeType.Type_Time_3,Language.HappyLink.ActTime)
end

-- 点击跳过动画
function MountainSeaMineCPanel:OnClickJumpAnim(flag)
    self.send_data.jump = flag
end

-- 点击抽奖
function MountainSeaMineCPanel:OnClickDraw()
    -- if  LoginData.Instance:IsOnCrossSever() then
    --     PublicPopupCtrl.Instance:Center(Language.MountainSeaMineCollect.Crossing)
    --     return 
    -- end 

    if not self.send_data.inter then 
        local timer = TimeHelper.FormatDHMS(self.send_data.act_time - TimeHelper.GetServerTime())

        if timer.day > 0 then 
            PublicPopupCtrl.Instance:Center(string.format(Language.MountainSeaMineCollect.AfterDayStart,timer.day,timer.hour))
        else 
            PublicPopupCtrl.Instance:Center(string.format(Language.MountainSeaMineCollect.AfterHourStart,timer.hour,timer.min))
        end 
        return 
    end 
    local key =  Item.Create({item_id = Config.shanhaijubao_auto.other[1].consume_item})
    if key:Num() < Config.shanhaijubao_auto.other[1].consume_num then 
        PublicPopupCtrl.Instance:Center(string.format(Language.MountainSeaMineCollect.LackItem,key:QuaName()))
        ViewMgr:OpenView(GetWayView, {item = key})
        return 
    end 

    self.send_data.mark = true
    if self.send_data.mark and self.send_data.jump then 
        self.data:SetMinecAnimMark(true)
    end 
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_MINEC,0,self.send_data.phase)
end

-- 点击额外礼包
function MountainSeaMineCPanel:OnClickExtraGift()
    local flag_can = self.data.mountain_sea_minec_data.info[self.send_data.phase].total_draw_times 
        >= self.master_data.extra_info[self.send_data.phase].phase_reward.lucky_draw_num
    local flag_complete = self.master_data.extra_info[self.send_data.phase].is_all_in

    if flag_complete or not flag_can then 
        local reward_list = {}
        for k,v in pairs(self.master_data.extra_info[self.send_data.phase].phase_reward.reward_item) do 
            table.insert(reward_list,BagData.Instance:ItemBuild(v))
        end 
        ViewMgr:OpenView(SilkRoadPreview,{reward_list = reward_list,pos = Vector3.New(0,-110,0)})
        return 
    end 

    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_MINEC,1,self.send_data.phase,self.master_data.extra_info[self.send_data.phase].cur_index - 1)
end

function MountainSeaMineCPanel:OnClickRewardGroup(data)
    self.send_data.phase = data.phase
    self.send_data.inter = data.inter 
    self.send_data.act_time = data.phase_time
    self:FlushRewardShow()
    self:FlushRollPart()
end

MountainSeaMineCRollGroup = MountainSeaMineCRollGroup or DeclareMono("MountainSeaMineCRollGroup", UIWidgetBaseItem)
function MountainSeaMineCRollGroup:MountainSeaMineCRollGroup() 
    self.pop_time:SetShowCallBack(function (t)
        if t.day > 0 then 
            return string.format(Language.MountainSeaMineCollect.AfterDayStart,t.day,t.hour)
        else 
            return string.format(Language.MountainSeaMineCollect.AfterHourStart,t.hour,t.min)
        end
    end)
end

function MountainSeaMineCRollGroup:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.name, data.name)
    self.inter.Interactable = data.inter
    self.gray_btns:SetActive(not data.inter)
    self.nomal_btns:SetActive(data.inter)
    self.pop:SetActive(data.show_next > 0 and data.phase == data.show_next)
    -- self.pop_anim:SetBool(APH("is_show"),data.show_next > 0 and data.phase == data.show_next)
    -- UH.SetText(self.pop_time, data.name)
    self.pop_time:StampTime(data.phase_time,TimeType.Type_Time_6)
end

MountainSeaMineCRollShow = MountainSeaMineCRollShow or  DeclareMono("MountainSeaMineCRollShow", UIWidgetBaseItem)
function MountainSeaMineCRollShow:MountainSeaMineCRollShow() end
function MountainSeaMineCRollShow:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local item = Item.Create(data.reward_item)
    self.item_call:SetData(item)
    -- UH.SetIcon(self.item_icon, item:IconId(), ICON_TYPE.ITEM)

    UH.SetText(self.rate_num, (data.show_rate/100).."%")
end

function MountainSeaMineCRollShow:ShowRate(flag)
    self.rate_obj:SetActive(flag)
end

function MountainSeaMineCRollShow:ShowSelect(flag)
    self.select:SetActive(flag)
end

function MountainSeaMineCRollShow:ShowEffect(flag)
    self.effect:SetActive(flag)
end

function MountainSeaMineCRollShow:ShowReward(flag)
    self.item_icon.gameObject:SetActive(flag)
    if flag then 
        local item = Item.Create(self.data.reward_item)
        UH.SetIcon(self.item_icon, item:IconId(), ICON_TYPE.ITEM)
        UH.SetText(self.item_num, self.data.reward_item.num)
    end 
end

MountainSeaMineCRewardShow = MountainSeaMineCRewardShow or DeclareMono("MountainSeaMineCRewardShow", UIWidgetBaseItem)
function MountainSeaMineCRewardShow:MountainSeaMineCRewardShow() end
function MountainSeaMineCRewardShow:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.reward_model:SetActive(#data == 1)
    self.item_show:SetActive(#data > 1)

    if #data > 1  then 
        for i = 1,5 do 
            local show_flag = data[i] ~= nil 
            self.rewardobjs[i].gameObject:SetActive(show_flag)
            if show_flag then 
                self.rewards[i]:SetData(Item.Create(data[i].reward_item))
            end 
        end 
    else

    end 
end


-- roll_show