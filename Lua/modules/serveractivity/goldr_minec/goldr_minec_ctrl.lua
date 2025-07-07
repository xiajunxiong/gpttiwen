VIEW_REQ("modules/serveractivity/goldr_minec/goldr_minec_view")
VIEW_REQ("modules/serveractivity/goldr_minec/goldr_minec_pre_view")
VIEW_REQ("modules/serveractivity/goldr_minec/goldr_minec_select_view")
VIEW_REQ("modules/serveractivity/goldr_minec/goldr_minec_shop_view")
VIEW_REQ("modules/serveractivity/goldr_minec/goldr_minec_reward")
VIEW_REQ("modules/serveractivity/goldr_minec/goldr_minec_record")
VIEW_REQ("modules/serveractivity/goldr_minec/goldr_minec_count")

GoldRMinecCtrl = GoldRMinecCtrl or BaseClass(BaseCtrl)
function GoldRMinecCtrl:__init()
    if GoldRMinecCtrl.Instance ~= nil then
        Debuger.LogError("[GoldRMinecCtrl] attempt to create singleton twice!")
        return
    end
    GoldRMinecCtrl.Instance = self

    self.data = GoldRMinecData.New()
end 

function GoldRMinecCtrl:__delete()
    if GoldRMinecCtrl.Instance == self then
        GoldRMinecCtrl.Instance = nil
    end
end

function GoldRMinecCtrl:OnInit()
    self:RegisterAllProtocols()
    ActivityRandData.Instance:CustomBaseItemData(Mod.GoldRMineC.Main,function(data)
        local text_list = Split(data.text,"|")
        local sprite_list = Split(data.sprite,"|")
        local type = self.data:ActivityType()
        return {sprite = sprite_list[type + 1],text = text_list[type + 1]}
	end)
    local update_remind_func = BindTool.Bind(self.GetRemind, self)
    -- ActivityData.Instance:RegisterRemind(ACTIVITY_TYPE.RAND_JIN_LONG_JU_BAO, update_remind_func)
    Remind.Instance:Register(Mod.GoldRMineC.Main, self.data.act_info, update_remind_func)
    Remind.Instance:Register(Mod.GoldRMineC.Main, BagData.Instance.item_grid, update_remind_func)
end 
function GoldRMinecCtrl:GetRemind()
    return self.data:Remind()
end

function GoldRMinecCtrl:OnUnloadGameLogic()
    --切换角色操作
    self.data.time_seq = 0
end

function GoldRMinecCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAJinLongJuBao,"OnSCRAJinLongJuBao")
    self:RegisterProtocol(SCRAJinLongJuBaoDrawRet,"OnSCRAJinLongJuBaoDrawRet")
end

function GoldRMinecCtrl:OnSCRAJinLongJuBao(protocol)
    -- LogDG("平哥哥@这是金龙聚宝的下发哦？5135", protocol.reason ,protocol)
    self.data:SetActInfo(protocol)

    if protocol.reason == 4 then 
        -- ViewMgr:OpenView(GoldrMinecRareSelectView)
    end
    --检查并设置当前开服时间，得出奖励seq
end

function GoldRMinecCtrl:OnSCRAJinLongJuBaoDrawRet(protocol)
    -- LogDG("平哥哥@这是金龙聚宝的获奖下发哦？5136",protocol)
    self.data:SetActDrawRet(protocol)
end

function GoldRMinecCtrl:SendInfo()
	-- LogDG("平哥哥@这是请求信息哦")
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_JIN_LONG_JU_BAO, 0)
end

-- param_1 seq
function GoldRMinecCtrl:SendBuy(param_1)
	-- LogDG("平哥哥@这是请求直购哦",param_1)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_JIN_LONG_JU_BAO, 1,param_1)
end

function GoldRMinecCtrl:SendRoundReward(param_1)
	-- LogDG("平哥哥@这是请求轮次奖励哦",param_1)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_JIN_LONG_JU_BAO, 2,param_1)
end

-- param_1 格子
-- param_2 奖励seq
function GoldRMinecCtrl:SendSetReward(param_1,param_2)
	-- LogDG("平哥哥@这是请求设置自选奖励哦",param_1,param_2)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_JIN_LONG_JU_BAO, 3,param_1,param_2)
end

function GoldRMinecCtrl:SendDraw()
	-- LogDG("平哥哥@这是请求抽奖哦")
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_JIN_LONG_JU_BAO, 4,self.data:GetSendType())
end


GoldRMinecData = GoldRMinecData or BaseClass()
function GoldRMinecData:__init()
    if GoldRMinecData.Instance ~= nil then
        Debuger.LogError("[GoldRMinecData] attempt to create singleton twice!")
        return
    end
    GoldRMinecData.Instance = self
    self.act_info = SmartData.New({flush = false})
    self.view_data = SmartData.New({flush = false,show_anim = false,start_shield = 0})
    self.send_type = SmartData.New({num = 1,show = true,stren = false})
    self.config = Config.jinlongjubao_auto
    self.activity_interface = self.config.activity_interface
    self.time_config = self.config.time
    self.time_seq = 0
    self.show_enum = {
        [0] = {
            [0] = 1,
            [1] = 5,
            [2] = 7,
            [3] = 11,
        },
        [1] = {
            [0] = 2,
            [1] = 3,
            [2] = 4,
            [3] = 6,
            [4] = 8,
            [5] = 9,
            [6] = 10,
            [7] = 12,
        },
    }
end

function GoldRMinecData:__delete()
    if GoldRMinecData.Instance == self then
        GoldRMinecData.Instance = nil
    end
end

function GoldRMinecData:SetActInfo(protocol)
    self.act_info.ra_day_buy_times = protocol.ra_day_buy_times
    self.act_info.ra_buy_times = protocol.ra_buy_times

    self.act_info.get_all_reward_times = protocol.get_all_reward_times
    self.act_info.times_reward_fetch_flag =  bit:d2b(protocol.times_reward_fetch_flag)

    self.act_info.item_list_select = protocol.item_list_select
    self.act_info.item_list_rand = protocol.item_list_rand

    self.act_info.record_list = protocol.record_list

    self.act_info.record_cyc_num = 0
    for i = 1,20 do 
        if self.act_info.record_list[i].time == 0 then 
            self.act_info.record_cyc_num = i
            break
        end 
    end 

    if protocol.reason == 1 then -- 设置自选奖励完成
        self.view_data.show_anim = true
        if self.act_info.item_list_select[3].is_select == 0 then 
            -- 未完成全部的刷新则不刷新
            return 
        end 
    elseif protocol.reason == 2 then  -- 领取次数奖励完成
        
    elseif protocol.reason == 3 then -- 直购下发

    elseif protocol.reason == 4 then -- 刷新下发
        return 
    end 

    self.act_info.flush = not self.act_info.flush
end 

function GoldRMinecData:SetSendType(num)
    -- PublicPopupCtrl.Instance:Center(self.language.ItemChangeReason)
    self.send_type.num = num
end

function GoldRMinecData:GetSendType()
    return self.send_type.num
end

function GoldRMinecData:CheckEnoughDirect()
    local item_num = Item.GetNum(self.config.other[1].consume_item)

    -- 若数量不足，则初始化
    return item_num >= self:GetSendType()
end

function GoldRMinecData:IsDirectShow()
    return self.send_type.num > 1
end

function GoldRMinecData:SetSendShow(flag)
    self.send_type.show = flag
end

function GoldRMinecData:GetSendShow()
    return self.send_type.show
end

-- 设定为强制单抽
function GoldRMinecData:SetSendStren(flag)
    self.send_type.stren = flag
end

-- 展示 且超过10个
function GoldRMinecData:ShouldSendShow()
    local item_num = Item.GetNum(self.config.other[1].consume_item) 

    -- 需要展示+够十连
    local flag_init = self.send_type.show and item_num >= 10

    -- 不需要展示+单抽+够十连+强制单抽
    local flag_change = (not self.send_type.show) and self.send_type.num == 1 and item_num >= 10 and not self.send_type.stren 

    return flag_init or flag_change
end

function GoldRMinecData:StartToShield()
    self.view_data.start_shield = self.view_data.start_shield +1
end

-- 将类型+位置转换为1~12的位置
function GoldRMinecData:ChangeIndex(type,index)
    return self.show_enum[type][index]
end

function GoldRMinecData:SetActDrawRet(protocol)
    -- 单抽的场合
    local list = self:GetCardList()
    if self:GetSendType() == 1 then 
        local list = self:GetCardList()
        self.view_data.result = self.show_enum[protocol.item_list[1].type][protocol.item_list[1].index]
        self.view_data.result_cfg = self:GetRewardCfg(list[self.view_data.result].reward_group_id,list[self.view_data.result].reward_seq)
    
        if protocol.item_list[1].type == 0 then 
            self.act_info.item_list_select[protocol.item_list[1].index].is_get = 1
        else 
            self.act_info.item_list_rand[protocol.item_list[1].index].is_get = 1
        end 
        self.view_data.flush = not self.view_data.flush
        self.act_info.get_all_reward_times = self.act_info.get_all_reward_times + 1
        -- LogError("???",self.act_info.record_cyc_num)
        if self.act_info.record_cyc_num > 0 then 
            self.act_info.record_list[self.act_info.record_cyc_num] = {
                time = protocol.item_list[1].time,
                list_index = protocol.item_list[1].list_index,
                item_id = self.view_data.result_cfg.reward_item.item_id,
                num = self.view_data.result_cfg.reward_item.num
            }
        else 
            local list = {}
            local time_info = nil
            for k,v in pairs(self.act_info.record_list) do
                if time_info == nil then 
                    time_info = {
                        time = v.time,
                        list_index = v.list_index,
                        pos = k,   
                    }
                elseif time_info.time > v.time or (time_info.time == v.time and time_info.list_index > v.list_index) then 
                    time_info = {
                        time = v.time,
                        list_index = v.list_index,
                        pos = k,   
                    }
                end 
            end 
    
            self.act_info.record_list[time_info.pos] = {
                time = protocol.item_list[1].time,
                item_id = self.view_data.result_cfg.reward_item.item_id,
                num = self.view_data.result_cfg.reward_item.num}
        end 
    
        
        self.act_info.record_cyc_num = 0
        for i = 1,20 do 
            if self.act_info.record_list[i].time == 0 then 
                self.act_info.record_cyc_num = i
                break
            end 
        end 
    else
        self.view_data.direct_list = {}

        for k,v in pairs(protocol.item_list) do 
            if v.type == 0 then 
                self.act_info.item_list_select[v.index].is_get = 1
            else 
                self.act_info.item_list_rand[v.index].is_get = 1
            end 
            self.act_info.get_all_reward_times = self.act_info.get_all_reward_times + 1

            local show_pos = self.show_enum[v.type][v.index]
            local result = self:GetRewardCfg(list[show_pos].reward_group_id,list[show_pos].reward_seq)
            local vo = {
                reward_item = {
                    item_id = result.reward_item.item_id,
                    num = result.reward_item.num,
                },
                reward_group = result.reward_group,
            }
            table.insert(self.view_data.direct_list,vo)
            
            local list = {}
            local time_info = nil
            for k,v in pairs(self.act_info.record_list) do
                if time_info == nil then 
                    time_info = {
                        time = v.time,
                        list_index = v.list_index,
                        pos = k,   
                    }
                elseif time_info.time > v.time or (time_info.time == v.time and time_info.list_index > v.list_index) then 
                    time_info = {
                        time = v.time,
                        list_index = v.list_index,
                        pos = k,   
                    }
                end 
            end 

            self.act_info.record_list[time_info.pos] = {
                time = v.time,
                list_index = v.list_index,
                item_id = result.reward_item.item_id,
                num = result.reward_item.num}
    
            self.act_info.record_cyc_num = 0
            for i = 1,20 do 
                if self.act_info.record_list[i].time == 0 then 
                    self.act_info.record_cyc_num = i
                    break
                end 
            end 
        end

        self.view_data.flush = not self.view_data.flush
    end 
end

function GoldRMinecData:GetRecordList()
    local list = {}
    for i = 1,20 do
        if self.act_info.record_list[i].time ~= 0 then 
            local vo = {}
            local t = TimeManager.FormatUnixTime2Date(self.act_info.record_list[i].time) 
            local time_show = string.format(Language.GoldRMinec.TimeShow,t.month,t.day,t.hour,t.minute,t.second)
            local item_show = ColorStr(string.format(Language.GoldRMinec.ItemShow,Item.GetName(self.act_info.record_list[i].item_id))
                ,Item.GetColorStr(self.act_info.record_list[i].item_id,true))

            vo.time_temp = self.act_info.record_list[i].time
            vo.desc = string.format(Language.GoldRMinec.RecordDesc,time_show,item_show,self.act_info.record_list[i].num)
            table.insert(list,vo)
        end 
    end
    table.sort(list, DataSorter.KeyUpperSorter("time_temp"))
    return list
end

function GoldRMinecData:ClearDrawRet()
    self.view_data.result = nil
    self.view_data.show_anim = false
    self.view_data.result_cfg = nil
    self.view_data.direct_list = nil
end

function GoldRMinecData:WithDrawRet()
    return self.view_data.result ~= nil or self.view_data.direct_list ~= nil
end

function GoldRMinecData:GetDrawRet()
    return self.view_data.result
end

function GoldRMinecData:GetDrawRetCfg()
    return self.view_data.result_cfg
end
function GoldRMinecData:CheckTimeSeq()
    if self.time_seq ~= 0 then return end
    local time = TimeCtrl.Instance:ServerStartTS()
    local time_seq = 0
    for i, v in ipairs(self.time_config) do
        if time >= v.time then
            time_seq = math.max(time_seq, v.time_seq)
        end
    end
    self.time_seq = time_seq
end

function GoldRMinecData:GetGroupCfg()
    self:CheckTimeSeq()
    local temp = {}
    for i, v in ipairs(self.config.basis_configuration) do
        if v.time_seq == self.time_seq then
            table.insert(temp, v)
        end
    end
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_JIN_LONG_JU_BAO, temp,"type")
end

function GoldRMinecData:GetShopCfg()
    self:CheckTimeSeq()
    local temp = {}
    for i, v in ipairs(self.config.shop_configuration) do
        if v.time_seq == self.time_seq then
            table.insert(temp, v)
        end
    end
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_JIN_LONG_JU_BAO, temp,"type")
end

function GoldRMinecData:GetShopList()
    local list = self:GetShopCfg()

    table.sort(list, function (a,b)
        local limit_a =  a.limit_type == 1 and a.limit_times or a.day_times
        local sorce_a =  (limit_a - self:GetShopLimit(a.limit_type,a.seq) > 0 ) and 100 or 0
        
        local limit_b =  b.limit_type == 1 and b.limit_times or b.day_times
        local sorce_b =  (limit_b - self:GetShopLimit(b.limit_type,b.seq) > 0 ) and 100 or 0

        return sorce_a > sorce_b
    end)
    return list
end

function GoldRMinecData:GetCardList()
    local list = {}

    for k,v in pairs(self.act_info.item_list_select) do 
        list[self.show_enum[0][k]] =v
    end 

    for k,v in pairs(self.act_info.item_list_rand) do 
        list[self.show_enum[1][k]] =v
    end 

    return list
end

function GoldRMinecData:GetShopLimit(type,seq)
    if self.act_info.ra_buy_times == nil then 
        return 0
    end 
    if type == 1 then 
        return self.act_info.ra_buy_times[seq + 1]
    elseif type == 2 then 
        return self.act_info.ra_day_buy_times[seq + 1]
    end 
end

-- 获取预览列表
function GoldRMinecData:GetItemPreViewList()
    self:CheckTimeSeq()
    local list = self:GetGroupCfg()
    local result = {}
    for index,cfg in pairs(list) do 
        for i,j in pairs(self.config.reward_group) do 
            if j.reward_group == cfg.reward_group and j.time_seq == self.time_seq then 
                table.insert(result,j)
            end 
        end 
    end 

    return result
end

-- 获取自选列表
function GoldRMinecData:GetSelectRewardList()
    self:CheckTimeSeq()
    local list = self:GetGroupCfg()

    local result = {}
    for index,cfg in pairs(list) do 
        if cfg.jackpot_type == 0 then
            for i,j in pairs(self.config.reward_group) do 
                if j.reward_group == cfg.reward_group and j.time_seq == self.time_seq  then 
                    table.insert(result,j)
                end 
            end 
        end 
    end 

    return result
end

-- 是否被选择类型
function GoldRMinecData:IsSelectType(data)
    self:CheckTimeSeq()
    local list = self:GetGroupCfg()
    for index,cfg in pairs(list) do 
        if cfg.jackpot_type == 0 then
            if data.reward_group == cfg.reward_group and data.time_seq == self.time_seq then 
                return true
            end 
        end      
    end 
    return false
end

function GoldRMinecData:GetRoundRewardList()
    self:CheckTimeSeq()
    local temp = {}
    for i, v in ipairs(self.config.reward_configuration) do
        if v.time_seq == self.time_seq then
            table.insert(temp, v)
        end
    end
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_JIN_LONG_JU_BAO, temp,"type") 
end

function GoldRMinecData:GetCurRound()
    if self.act_info.get_all_reward_times == nil then 
        return 0
    end 

    return self.act_info.get_all_reward_times 
end

-- 轮数能拿吗
function GoldRMinecData:IsCanRoundReward(data)
    if self.act_info.get_all_reward_times == nil then 
        return 0
    end 

    return self.act_info.get_all_reward_times >= data.times
end

-- 轮数拿了吗
function GoldRMinecData:IsPassRoundReward(data)
    if self.act_info.times_reward_fetch_flag == nil then 
        return false
    end 
    -- LogError("!@?~~~", self.act_info.times_reward_fetch_flag)
    return self.act_info.times_reward_fetch_flag[32- data.seq] == 1
end

function GoldRMinecData:GetOtherCfg()
    return self.config.other[1]
end

function GoldRMinecData:GetSelectComplete()
    if self.act_info.item_list_select == nil then 
        return false
    end 
    local flag = true
    for k,v in pairs(self.act_info.item_list_select) do 
        flag = flag and v.is_select == 1
    end  

    return flag
end

-- 获取具体的某个奖励配置
function GoldRMinecData:GetRewardCfg(group_id,seq)
    self:CheckTimeSeq()
    for k,v in pairs(self.config.reward_group) do 
        if group_id == v.reward_group and seq == v.seq and v.time_seq == self.time_seq then 
            return v
        end 
    end 
    return nil
end
function GoldRMinecData:ActivityType()
    self:CheckTimeSeq()
    for i, v in pairs(self.activity_interface) do
        if v.is_show == 1 and v.time_seq == self.time_seq then
            return v.interface
        end
    end
    return 0
end
function GoldRMinecData:Remind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_JIN_LONG_JU_BAO) then
        return 0
    end
    --是否选择了
    if not self:GetSelectComplete() then
        return 1
    end
    local item_id = self:GetOtherCfg().consume_item
    if Item.GetNum(item_id) > 0 then
        return 1
    end
    local list = self:GetRoundRewardList()
    for k, v in pairs(list) do
        if not self:IsPassRoundReward(v) and self:IsCanRoundReward(v) then
            return 1
        end
    end
    return 0
end