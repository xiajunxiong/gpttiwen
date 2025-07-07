FightMirageTerritoryData = FightMirageTerritoryData or BaseClass()

local STATE = {
    Empty = -1,
    Fight = 1,
    Assist = 2,
    StandBy = 3,
    Relax = 4,
}

local PosGroup = {
    [1] = {
        [1] = { ItemPos = {x = -326, y = 62.9}, DotPos = {x = -292.6, y = 65.9} },
        [2] = { ItemPos = {x = -43.6, y = 229.9}, DotPos = {x = -85.2, y = 234.1}, },
        [3] = { ItemPos = {x = -246.1, y = -49}, DotPos = {x = -210.7, y = -40.1}, },
        [4] = { ItemPos = {x = -92.1, y = -233}, DotPos = {x = -130.3, y = -230.8}, },
        [5] = { ItemPos = {x = -26.6, y = 88}, DotPos = {x = 17.9, y = 91.2}, },
        [6] = { ItemPos = {x = 263, y = 247}, DotPos = {x = 223.7, y = 254.1}, },
        [7] = { ItemPos = {x = 361, y = 63}, DotPos = {x = 320.6, y = 65.1}, },
        [8] = { ItemPos = {x = 13, y = -70}, DotPos = {x = 49.6, y = -61.1} },
        [9] = { ItemPos = {x = 39, y = -253}, DotPos = {x = 76.6, y = -251} },
        [10] = {ItemPos = {x = 315, y = -199}, DotPos = {x = 273.9, y = -193.2} }
    },
    [2] = {
        [1] = { ItemPos = {x = -327.7, y = -197.5}, DotPos = {x = -292.6, y = -192.8} },
        [2] = { ItemPos = {x = -230, y = -21.8}, DotPos = {x = -192.2, y = -26.7}, },
        [3] = { ItemPos = {x = -10, y = 113.9}, DotPos = {x = -52.7, y = 115.1}, },
        [4] = { ItemPos = {x = -182.7, y = 273.8}, DotPos = {x = -144.3, y = 275.7}, },
        [5] = { ItemPos = {x = 300, y = 215.1}, DotPos = {x = 259.7, y = 214.8}, },
        [6] = { ItemPos = {x = 189, y = 20}, DotPos = {x = 149.4, y = 20}, },
        [7] = { ItemPos = {x = -53, y = -56}, DotPos = {x = -14.7, y = -53.8}, },
        [8] = { ItemPos = {x = -52, y = -250}, DotPos = {x = -13.9, y = -247.2} },
        [9] = { ItemPos = {x = 267, y = -262}, DotPos = {x = 227.8, y = -259.4} },
        [10] = {ItemPos = {x = 360, y = -79}, DotPos = {x = 320.7, y = -83.8} }
    },
    [3] = {
        [1] = { ItemPos = {x = -253.4, y = -76.9}, DotPos = {x = -216.7, y = -84.2} },
        [2] = { ItemPos = {x = -314.1, y = 120.8}, DotPos = {x = -276.7, y = 114.7}, },
        [3] = { ItemPos = {x = -162, y = 227.7}, DotPos = {x = -127.6, y = 220.9}, },
        [4] = { ItemPos = {x = 66.6, y = 153.6}, DotPos = {x = 26.6, y = 151}, },
        [5] = { ItemPos = {x = 248.3, y = 258.3}, DotPos = {x = 209.1, y = 249.8}, },
        [6] = { ItemPos = {x = 345.4, y = 46.1}, DotPos = {x = 305.6, y = 39.6}, },
        [7] = { ItemPos = {x = 36.3, y = -4}, DotPos = {x = 73.4, y = -4.2}, },
        [8] = { ItemPos = {x = -102.4, y = -182.2}, DotPos = {x = -62.6, y = -187.1} },
        [9] = { ItemPos = {x = 114.7, y = -282.1}, DotPos = {x = 151.9, y = -284} },
        [10] = {ItemPos = {x = 292.1, y = -198.5}, DotPos = {x = 250.7, y = -201.4} }
    },
    [4] = {
        [1] = { ItemPos = {x = -327.7, y = -197.5}, DotPos = {x = -291.8, y = -199.8} },
        [2] = { ItemPos = {x = -74, y = -147}, DotPos = {x = -112.4, y = -143.9}, },
        [3] = { ItemPos = {x = -36, y = 33}, DotPos = {x = -74.4, y = 38.8}, },
        [4] = { ItemPos = {x = -226, y = 139}, DotPos = {x = -189.5, y = 137}, },
        [5] = { ItemPos = {x = 10, y = 270}, DotPos = {x = -29.7, y = 264}, },
        [6] = { ItemPos = {x = 135, y = 134}, DotPos = {x = 94.7, y = 132}, },
        [7] = { ItemPos = {x = 289, y = 140}, DotPos = {x = 247.8, y = 138.9}, },
        [8] = { ItemPos = {x = 361, y = -149}, DotPos = {x = 322.6, y = -146.2} },
        [9] = { ItemPos = {x = 34, y = -239}, DotPos = {x = 70.5, y = -234.7} },
        [10] = {ItemPos = {x = 97, y = -86}, DotPos = {x = 133.6, y = -78.6} }
    },
    [5] = {
        [1] = { ItemPos = {x = -248, y = 189.6}, DotPos = {x = -209.6, y = 192.1} },
        [2] = { ItemPos = {x = -4.6, y = 270.6}, DotPos = {x = -43.8, y = 258.9}, },
        [3] = { ItemPos = {x = 146.7, y = 129.1}, DotPos = {x = 110.9, y = 119.9}, },
        [4] = { ItemPos = {x = -183, y = 62}, DotPos = {x = -143.3, y = 61.7}, },
        [5] = { ItemPos = {x = -328, y = -50}, DotPos = {x = -289.6, y = -51.9}, },
        [6] = { ItemPos = {x = -81, y = -83}, DotPos = {x = -117, y = -95}, },
        [7] = { ItemPos = {x = -68.9, y = -234.9}, DotPos = {x = -30.6, y = -232}, },
        [8] = { ItemPos = {x = 181, y = -134}, DotPos = {x = 146, y = -136.9} },
        [9] = { ItemPos = {x = 353.9, y = -107}, DotPos = {x = 318, y = -127.1} },
        [10] = {ItemPos = {x = 344, y = 76.9}, DotPos = {x = 308.1, y = 67.9} }
    }
}

local MirageTerritoryEffectId = {
    [0] = 3165085,
    [1] = 3165086,
    [2] = 3165087,
    [3] = 3165088,
}

local BgName = {"Chun","Xia","Qiu","Dong"}

function FightMirageTerritoryData:__init()
    if FightMirageTerritoryData.Instance ~= nil then
        Debuger.LogError("[FightMirageTerritoryData] attempt to create singleton twice!")
        return
    end
    FightMirageTerritoryData.Instance = self
    self.formation_smart = SmartData.New{
		sel_seq = 0,
        list_change = false,
		formation_change = {
			seq = 0,
		},
		pos_change = {
			arrow = 0,
			lights = {}
		},
		partner = 0,
	}
    self.dispatch_smart = SmartData.New{
		certain = true,
		partner = 0,
        pos = 0,
	}
    self.dispatch_partner_list = {}
    self.formation_info = {}
    self.formation_grid_list = {}
    self.partner_id_list = {} --存在显示列表  显示战字

    self.area_layer_data = {}
    self.cur_area_data = {seq = 0,all_num = 0}
    self.cur_layer_data = SmartData.New{  --当前关卡层数
        seq = -1,
        layer = 1,
        pass_level = 1, --解锁关卡
        dispatch_level = 1, --最后一次扫荡的关卡
        is_clear = true,
        sel_level = 0,
        is_update = true, --刷新
    }
    self.dispatch_data = SmartData.New({num = 0,all_num = 0})

    self.certain_dispatch = SmartData.New({show = true})
    self:CheckConfig()
    self.is_rotate = false --通关当前层播放选择特效
    self.is_wait = false --避免伙伴未保存数据直接进入挑战
end
function FightMirageTerritoryData:DataReset()
    self.area_layer_data = {}
    self.formation_info = {}
    self.dispatch_partner_list = {}
    self.partner_id_list = {} 
    self.formation_grid_list = {}
    for i,v in pairs(self:GetAreaData()) do
        self.formation_info[v.seq] = {}
    end
    self.dispatch_data.num = self.dispatch_data.all_num
    self.is_wait = false
    self.is_rotate = false
    self.show_sweep_reward = nil
end

function FightMirageTerritoryData:CheckConfig()
    if not self.reward_data then
        self.reward_data = {}
        for i,v in pairs(Config.mirage_territory_auto.challenge) do
            if not self.reward_data[v.area] then
                self.reward_data[v.area] = {}
            end           
            table.insert(self.reward_data[v.area],v)
        end
    end  
    if not self.challenge_reward_group then
        self.challenge_reward_group = {}
        for i,v in pairs(Config.mirage_territory_auto.reward) do
            if not self.challenge_reward_group[v.seq] then
                self.challenge_reward_group[v.seq] = {}
            end           
            self.challenge_reward_group[v.seq] = v
        end
    end
    self:GetAreaConfig()
    for i,v in pairs(self:GetAreaData()) do
        self.formation_info[v.seq] = {}
    end
    self.dispatch_data.all_num = Config.mirage_territory_auto.other[1].dispatch_time
    self.dispatch_data.num = self.dispatch_data.all_num
    if not self.partner_arm_list then
        self.partner_arm_list = {}
        for i,v in pairs(Config.mirage_territory_auto.partner_arm_chip) do
            self.partner_arm_list[v.partner_id] = v
        end 
    end
end

function FightMirageTerritoryData:GetPartnerArmsById(partner_id)
    return  self.partner_arm_list[partner_id] or {}
end

function FightMirageTerritoryData:GetPartnerDispatchShow(group_id)
    return self.challenge_reward_group[group_id].reward_show or {}
end

function FightMirageTerritoryData:GetPartnerArmsNum()
    local info = self.reward_data[self.cur_area_data.seq][self.cur_layer_data.dispatch_level]
    return self.challenge_reward_group[info.reward_id].arm_chip_num
end

function FightMirageTerritoryData:AddAreaLayer(layer)
    self.cur_layer_data.layer = self.cur_layer_data.layer + (layer or 1)
    if self.cur_layer_data.layer <=0  then
        self.cur_layer_data.layer = 1
    end
    if self.cur_layer_data.layer > self.cur_area_data.all_num then
        self.cur_layer_data.layer = self.cur_area_data.all_num
    end
end

function FightMirageTerritoryData:SetCurAreaData(data)
    self.cur_area_data = data
    -- LogError("self.cur_area_data",self.cur_area_data)
    self:UpdateCurLayerData()
end

function FightMirageTerritoryData:UpdateCurLayerData()
    self.cur_layer_data.seq = self.cur_area_data.seq
    if not self.area_layer_data[self.cur_area_data.seq] then
        return
    end
    self.cur_layer_data.layer = self.area_layer_data[self.cur_area_data.seq].top_seq + (self.area_layer_data[self.cur_area_data.seq].top_seq < self.cur_area_data.all_num and 1 or 0)
    self.cur_layer_data.pass_level = self.area_layer_data[self.cur_area_data.seq].top_seq
    self.cur_layer_data.dispatch_level = self.area_layer_data[self.cur_area_data.seq].now_seq
    self.cur_layer_data.is_clear = (self.dispatch_data.num < self.dispatch_data.all_num) and (self.cur_layer_data.pass_level > 0)
    self:SetCurLevelRange()
end

function FightMirageTerritoryData:GetRewardData(group_id)
    return self.challenge_reward_group[group_id].reward or {}
end

function FightMirageTerritoryData:GetSweepRewardData(group_id)
    return self.challenge_reward_group[group_id].reward_show or {}
end

function FightMirageTerritoryData:GetAreaConfig()
    if not self.area_list then
        self.area_list = {}
        for i,v in pairs(Config.mirage_territory_auto.area) do
            v.all_num = #self.reward_data[v.seq]
            v.area_num = 1
            table.insert(self.area_list,v)          
        end
    end
    return self.area_list
end

function FightMirageTerritoryData:SetAreaInfo(protocol)
    if not self.area_layer_data[protocol.area] then
        self.area_layer_data[protocol.area] = {}
    end

    self.dispatch_data.pass_area = protocol.pass_area
    self.dispatch_data.pass_seq = protocol.pass_seq

    -- if self.area_layer_data[protocol.area].top_seq then
    --     if self.area_layer_data[protocol.area].top_seq < protocol.top_seq then
    --         self.is_rotate = false
    --         if protocol.top_seq % 10 == 0 then
    --             self.is_rotate = true
    --         end
    --     end
    -- end
    if self.area_layer_data[protocol.area].top_seq then  --重新连接设置数据
        if self.area_layer_data[protocol.area].top_seq < protocol.top_seq then
           if not self.cur_pass_level then
                self.cur_pass_level = protocol.top_seq
                self:SetCurAreaData(self:GetAreaData()[protocol.area + 1])
           end
        end
    end
    self.dispatch_data.num = protocol.times
    self.area_layer_data[protocol.area].top_seq = protocol.top_seq
    self.area_layer_data[protocol.area].now_seq = protocol.now_seq 
    local info = self.area_list[protocol.area + 1]
    info.area_num = protocol.top_seq
    if self.reward_data[info.seq] then
        if info.area_num >= #self.reward_data[info.seq] then
            info.reward_list = self:GetRewardData(self.reward_data[info.seq][info.area_num].reward_id)
        elseif self.reward_data[info.seq][info.area_num + 1] then
            info.reward_list = self:GetRewardData(self.reward_data[info.seq][info.area_num + 1].reward_id)
        else
            BuglyReport("1 MirageTerritoryData protocol",protocol)
        end
    else
        BuglyReport("2 MirageTerritoryData protocol",protocol)
    end
    if self.show_sweep_reward then
        if self.show_sweep_reward < protocol.times then
            PublicPopupCtrl.Instance:Reward2(DataHelper.FormatItemList(FightMirageTerritoryData.Instance:GetSweepData()),Language.Love.WrongRewardTip[1])           
        end        
    end
    self.show_sweep_reward = protocol.times
    self:UpdateCurLayerData()
end

function FightMirageTerritoryData:GetAreaData()
    return self.area_list
end

function FightMirageTerritoryData:GetClearReward()
    return self.reward_data[self.cur_area_data.seq] or {}
end

function FightMirageTerritoryData:PartnerOnFight(partner_id)
    return self.partner_id_list and self.partner_id_list[partner_id]
end

function FightMirageTerritoryData:SetFormationInfo(procotol)
    self.partner_id_list = {}
    for i,v in pairs(procotol.id_list) do
        if procotol.id_list[i] ~=0 then
            self:PartnerAddBegin(procotol.pos_list[i] + 1,procotol.id_list[i])
            self:PartnerAddEnd(procotol.pos_list[i] + 1,procotol.id_list[i])
        end
    end
    if self.is_wait then --布阵完成 才就行挑战
        FightMirageTerritoryCtrl.Instance:SendMirageTerritoryInfoReq(MirageTerritoryConfig.ReqType.Battle,FightMirageTerritoryData.Instance.cur_area_data.seq)
        self.is_wait = false
    end
end

function FightMirageTerritoryData:GetPartnerList()
    local function sort_lock(a,b)
        local av = a.vo
        local bv = b.vo
        if av.is_unlocked == bv.is_unlocked then
            if self:PartnerOnFight(a.id) and self:PartnerOnFight(b.id) or (self:PartnerOnFight(a.id) == nil and self:PartnerOnFight(b.id) == nil) then
                local arp = a.acti_red_point.val and 1 or 0
                local brp = b.acti_red_point.val and 1 or 0
                if arp == brp then
                    if a.partner_quality == b.partner_quality then
                        return a.id < b.id
                    end
                    return a.partner_quality > b.partner_quality
                end
                return arp > brp
            else
                return self:PartnerOnFight(a.id) ~= nil
            end
        end
        return av.is_unlocked > bv.is_unlocked
    end
    local list = {}
    for _, info in pairs(PartnerData.Instance.partner_list) do
        if info.info.job == self.cur_area_data.job1 or info.info.job == self.cur_area_data.job2 then
            table.insert(list, info)
        end
    end
    table.sort(list, sort_lock)
    return list
end

function FightMirageTerritoryData:GetAllPartnerList()
local function sort_lock(a,b)
        local av = a.vo
        local bv = b.vo
        if av.is_unlocked == bv.is_unlocked then
            if self:PartnerOnFight(a.id) and self:PartnerOnFight(b.id) or (self:PartnerOnFight(a.id) == nil and self:PartnerOnFight(b.id) == nil) then
                local arp = a.acti_red_point.val and 1 or 0
                local brp = b.acti_red_point.val and 1 or 0
                local a_is_cur_limit = a.info.job == self.cur_area_data.job1 or a.info.job == self.cur_area_data.job2
                local b_is_cur_limit = b.info.job == self.cur_area_data.job1 or b.info.job == self.cur_area_data.job2
                if a_is_cur_limit == b_is_cur_limit then
                    if arp == brp then
                        if a.partner_quality == b.partner_quality then
                            return a.id < b.id
                        end
                        return a.partner_quality > b.partner_quality
                    end
                    return arp > brp
                else
                    return a_is_cur_limit
                end
            else
                return self:PartnerOnFight(a.id) ~= nil
            end
        end
        return av.is_unlocked > bv.is_unlocked
    end
    local list = {}
    for _, info in pairs(PartnerData.Instance.partner_list) do
        table.insert(list, info)
    end
    table.sort(list, sort_lock)
    return list, list[1].id
end

function FightMirageTerritoryData:PosChangeReset()
	self.formation_smart.pos_change = {
		arrow = 0,
		lights = {},
	}
	self.formation_smart.partner = 0
end

function FightMirageTerritoryData:PartnerAddBegin(pos,partner)
    local lights = self:PatnerAddLights()
    self.formation_smart.cur_select_state = self.formation_smart.cur_select_state == STATE.Empty and STATE.Relax or self.formation_smart.cur_select_state
    self.formation_smart.pos_change = {
        arrow = 0,
        lights = lights,
    }
end

function FightMirageTerritoryData:PartnerAddEnd(pos, partner)
    local pos_change = self.formation_smart.pos_change
    if pos_change.lights[pos] then
        self.formation_smart.partner = 0
        if self.formation_info[self.cur_area_data.seq][pos] ~= nil then
            self.partner_id_list[self.formation_info[self.cur_area_data.seq][pos].partner_id] = nil
        end
        self.formation_info[self.cur_area_data.seq][pos] = FormationInfo.PartnerParse(partner, pos)
        self.formation_grid_list[pos] = {id = partner, type = ActFormationData.FormationType.Partner, unique_id_1 = 0, unique_id_2 = 0}
        self.partner_id_list[partner] = {id = partner, type = ActFormationData.FormationType.Partner, unique_id_1 = 0, unique_id_2 = 0}

        self.formation_smart:SetDirty("pos_changed")
        self:PosChangeReset()
        self.formation_smart.cur_select_state = STATE.Empty
        --PublicPopupCtrl.Instance:Center(Language.Formation.SelNoLightTips)
    end
end

function FightMirageTerritoryData:PatnerAddLights()
    local lights = {}
    for i = 1, 3 do      
        local info_i = self.formation_info[self.cur_area_data.seq][i]
        lights[i] = info_i == nil or info_i.partner
    end
    return lights
end

function FightMirageTerritoryData:PartnerChangePos(id)
    local pos = 0
    for k,v in pairs(self.formation_grid_list) do
        if v.type == ActFormationData.FormationType.Partner and v.id == id then
            self.formation_smart.cur_select_state = STATE.Fight
            pos = k
            break
        end
    end
    if pos ~= 0 then
        self:PosChange(pos)
    end
end

function FightMirageTerritoryData:GetFormationInfo()
    return self.formation_info[self.cur_area_data.seq]
end

function FightMirageTerritoryData:PosChange(pos)
    local pos_change = self.formation_smart.pos_change

    if 0 == pos_change.arrow then
        if self.formation_info[self.cur_area_data.seq][pos] then
            local lights = self:PatnerAddLights()
            self.formation_smart.pos_change = {
                arrow = pos,
                lights = lights,
            }
        end
    elseif pos_change.arrow ~= pos then --交换数据
        if pos_change.lights[pos] then
            local info = self.formation_info[self.cur_area_data.seq][pos]
            self.formation_info[self.cur_area_data.seq][pos] = self.formation_info[self.cur_area_data.seq][pos_change.arrow]
            self.formation_info[self.cur_area_data.seq][pos_change.arrow] = info
            if self.formation_info[self.cur_area_data.seq][pos] then self.formation_info[self.cur_area_data.seq][pos].pos = pos end
            if self.formation_info[self.cur_area_data.seq][pos_change.arrow] then self.formation_info[self.cur_area_data.seq][pos_change.arrow].pos = pos_change.arrow end

            local grid_info = self.formation_grid_list[pos]
            self.formation_grid_list[pos] = self.formation_grid_list[pos_change.arrow]
            self.formation_grid_list[pos_change.arrow] = grid_info           
            self.formation_smart:SetDirty("pos_changed")
            self:PosChangeReset()
        end
    else
        self:PosChangeReset()
    end
end

function FightMirageTerritoryData:SetSelectPartner(id)
    self.formation_smart.cur_select_state = self:PartnerOnFight(id) and STATE.Fight or STATE.Relax
    if id == self.formation_smart.partner and id ~= 0 then
        self.formation_smart.partner = 0
        self:PosChangeReset()
    else
        self.formation_smart.partner = id
    end
end

function FightMirageTerritoryData:RemoveShowItem(info)
    self.formation_info[self.cur_area_data.seq][info.pos] = nil
    self.partner_id_list[info.partner_id] = nil
    self.formation_smart:SetDirty("pos_changed")
    self:PosChangeReset()
end

function FightMirageTerritoryData:SaveFormation()
    local info_list = {}
    for i = 1, 3 do      
        local info_i = self.formation_info[self.cur_area_data.seq][i]
        info_list[i] = {}
        info_list[i].id = info_i ~= nil and info_i.partner_id or 0
        info_list[i].pos = info_i ~= nil and (i - 1) or 0
    end
    FightMirageTerritoryCtrl.Instance:SendMirageTerritorySetPartnerReq(info_list,self.cur_area_data.seq)
end

function FightMirageTerritoryData:SendPartnerDispatch()
    FightMirageTerritoryCtrl.Instance:SendMirageTerritoryInfoReq(
        MirageTerritoryConfig.ReqType.OneKeyPass,
        self.cur_area_data.seq,
        self.dispatch_partner_list[1] and self.dispatch_partner_list[1].id or 0,
        self.dispatch_partner_list[2] and self.dispatch_partner_list[2].id or 0
    )
end

function FightMirageTerritoryData:SetPlaySweep()
    self.certain_dispatch.show = not self.certain_dispatch.show
    ViewMgr:CloseView(PartnerDispatchView)
end

function FightMirageTerritoryData:GetFinishData()
    --LogError("seq",self.cur_area_data.seq,"cur_pass_level",self.cur_pass_level,self.reward_data)
    if self.cur_pass_level <= 0 then
        self.cur_pass_level = 1
    end
    if self.cur_pass_level > self.cur_area_data.all_num then
        self.cur_pass_level = self.cur_area_data.all_num
    end
    if self.cur_area_data.seq then 
        if self.cur_area_data.seq >= 0 and self.cur_pass_level >= 0 and self.cur_pass_level <= self.cur_area_data.all_num then
            return self:GetRewardData(self.reward_data[self.cur_area_data.seq][self.cur_pass_level].reward_id)
        end
    end
end

function FightMirageTerritoryData:GetSweepData()--得到扫荡奖励
    local reward_list = {}
    for i = 1,2 do
        if self.dispatch_partner_list[i] then
			local info = self:GetPartnerArmsById(self.dispatch_partner_list[i].id)
			info.num = self:GetPartnerArmsNum() or 0
            info.item_id = info.arms_id
			table.insert(reward_list,info)
		end
    end
    self:DispatchReset()
    return reward_list
end

function FightMirageTerritoryData:GetDispatchRemind()
    if  self.dispatch_data.all_num - self.dispatch_data.num > 0 then
        for i,v in pairs(self.area_layer_data) do
            if v.top_seq > 0 then
                return 1
            end
        end
    end
    return 0
end

function FightMirageTerritoryData:GetDispatchTimes()
    return self.dispatch_data.all_num - self.dispatch_data.num
end

function FightMirageTerritoryData:SetDispatchSelectPartner(data)
    self.dispatch_smart.partner_info = data
    self.dispatch_smart.partner = data.id
end

function FightMirageTerritoryData:AddDispatchList()
    if self.dispatch_smart.partner > 0 then
        self.dispatch_partner_list[self.dispatch_smart.pos] = self.dispatch_smart.partner_info
    end
    self.dispatch_smart.certain = not self.dispatch_smart.certain
end

function FightMirageTerritoryData:DispatchCancelSelect()
    self.dispatch_smart.partner = 0
    self.dispatch_smart.pos = 0
end

function FightMirageTerritoryData:DispatchReset()
    self.dispatch_partner_list = {}
    self:DispatchCancelSelect()
end

function FightMirageTerritoryData:GetDispatchShow(id)
    for i,v in pairs(self.dispatch_partner_list) do
        if v.id == id then
            return false
        end
    end
    return true 
end

function FightMirageTerritoryData:GoToChallenge()
    FightMirageTerritoryData.Instance.cur_pass_level = FightMirageTerritoryData.Instance.cur_layer_data.layer + FightMirageTerritoryData.Instance.cur_layer_data.sel_level
    FightMirageTerritoryCtrl.Instance:SendMirageTerritoryInfoReq(MirageTerritoryConfig.ReqType.Battle,FightMirageTerritoryData.Instance.cur_area_data.seq)
    ViewMgr:CloseView(FightMirageTerritoryLevelView)
end

function FightMirageTerritoryData:IsNeedFormation()
    local num = 0
    for i = 1,Config.mirage_territory_auto.other[1].partner_num do
        if self.formation_info[self.cur_area_data.seq][i] then
            num = num + 1
        end
    end
    return num == 0
end

function FightMirageTerritoryData:GetFormationIsEmptyPos()
    for i = 1,Config.mirage_territory_auto.other[1].partner_num do
        if not self.formation_info[self.cur_area_data.seq][i] then
            return i
        end
    end
    return 0 
end

function FightMirageTerritoryData:IsCanFormation()
    if self:GetFormationIsEmptyPos() > 0 then
        for i,v in pairs(self:GetPartnerList()) do
            if v.vo.is_unlocked > 0 and not self:PartnerOnFight(v.id) then
                return true
            end
        end
    end
    return false
end

function FightMirageTerritoryData:SetCurLevelRange()
    local val = self.cur_layer_data.pass_level % 10
    if self.cur_layer_data.layer >= self.cur_area_data.all_num then
        val = 9       
    end
    self.cur_layer_data.layer = self.cur_layer_data.layer - val
    self.cur_layer_data.sel_level = val
    --LogError("开始层级",self.cur_layer_data.layer,val)
end

function FightMirageTerritoryData:ResetLayerSelect()
    self.cur_layer_data.sel_level = 0
    if self.cur_layer_data.pass_level >= self.cur_layer_data.layer and self.cur_layer_data.pass_level <= (self.cur_layer_data.layer + 10) then
        local val = self.cur_layer_data.pass_level % 10
        self.cur_layer_data.sel_level = val
    end
end

function FightMirageTerritoryData:GetDotState(i)
    return  self.cur_layer_data.pass_level + 1 > (self.cur_layer_data.layer + i - 1)
end

function FightMirageTerritoryData.GetPosData(seq, index)

    return PosGroup[seq][index]
end

function FightMirageTerritoryData:GetBgName()
    return BgName[self.cur_layer_data.seq + 1] or ""
end

function FightMirageTerritoryData.GetEffectId(seq)
    return MirageTerritoryEffectId[seq]
end