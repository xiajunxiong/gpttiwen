LBDragonPluseData = LBDragonPluseData or BaseClass()

function LBDragonPluseData:__init()
    if LBDragonPluseData.Instance ~= nil then
        Debuger.LogError("[LBDragonPluseData] attempt to create singleton twice!")
        return
    end
    LBDragonPluseData.Instance = self

    self.base_info = SmartData.New({flush = 0}) -- 5518同步
    self.my_info = SmartData.New({flush = 0}) -- 5519同步
    self.position_info = SmartData.New() -- 5521同步
    self.operate_info = SmartData.New() -- 联动smart_data
    self.dragon_cfg = Config.divine_domain_system_auto.dragon_vein
    self.dragon_monster_cfg = Config.divine_domain_system_auto.dragon_vein_monster
    self.dragon_vein_concerted_war_cfg = Config.divine_domain_system_auto.dragon_vein_concerted_war
    self.dragon_camp_cfg = Config.divine_domain_system_auto.dragon_vein_camp

    self.max_boss_num = 3
    self.max_monster_num = 5
    self:InitConfig()

    self.vein_str = {
        [0] = "_Loc_ShenYu_0005_ShuiZhiChengLongMai",
        [1] = "_Loc_ShenYu_0004_HuoZhiChengLongMai",
        [2] = "_Loc_ShenYu_0003_FengZhiChengLongMai",
        [3] = "_Loc_ShenYu_0002_DiZhiChengLongMai",
    }

    self.call_friend_cd = 0
end 

function LBDragonPluseData:InitConfig()
    self.dragon_fix_cfg = {}
    for i = 1,#self.dragon_monster_cfg do 
        local vo = {}
        vo.monster_list = {}

        for j = 1,5 do 
            local eo = {}
            eo.monster_group_id = self.dragon_monster_cfg[i]["camp_monster_group_id_"..j]
            eo.seq = self.dragon_monster_cfg[i]["seq_"..j]
            eo.score = self.dragon_monster_cfg[i]["score_"..j]
            eo.war_name = self.dragon_monster_cfg[i]["concerted_war_name_"..j]

            eo.concerted = {}
            local split_list = Split(eo.seq)
            for k,v in pairs(split_list) do 
                for seq,cfg in pairs(self.dragon_vein_concerted_war_cfg) do 
                    if v == cfg.seq then 
                        local zo = {}
                        zo.name = cfg.way_explain
                        zo.type = cfg.param1
                        zo.id = cfg.param2
                        zo.quality = cfg.param3
                        table.insert(eo.concerted,zo)
                    end 
                end 
            end 

            eo.rewards = {}
            local temp_list = BagData.Instance:ItemListBuild(self.dragon_monster_cfg[i]["camp_monster_kill_reward_"..j])
            for k,v in pairs(temp_list) do 
                local vo = {}
                vo.item = v
                vo.is_extra = false
                table.insert(eo.rewards,vo)
            end 
            for k,v in pairs(self.dragon_monster_cfg[i]["camp_monster_concerted_war_reward_"..j]) do 
                local item = BagData.Instance:ItemBuild(v)
                local vo = {}
                vo.is_extra = true
                vo.item = item 
                table.insert(eo.rewards,vo)
                
            end 

            table.insert(vo.monster_list,eo)
        end 

        vo.boss_list = {}
        for j = 1,3 do 
            local eo = {}
            eo.monster_group_id = self.dragon_monster_cfg[i]["elite_monster_group_id_"..j]
            table.insert(vo.boss_list,eo)
        end 

        self.dragon_fix_cfg[i] = vo
    end 
end

-- 获取龙脉的怪物配置
-- 取法是1~5的位置对应group_id,然后取三个组中对应的monsetr_group_id
function LBDragonPluseData:GetMonsterCfg(index,group_id)
    for k,v in pairs(self.dragon_fix_cfg) do 
        if v.monster_list[index].monster_group_id == group_id then 
            return v.monster_list[index]
        end 
    end 

    return nil
end

-- 
function LBDragonPluseData:SetOperateBoss(data)
    self.operate_info.cache = data
end

function LBDragonPluseData:SetOperateSendInfo(data)
    self.operate_info.send_data = data
end

-- 5518 龙脉信息
function LBDragonPluseData:SetBaseInfo(protocol)
    self.base_info.status = protocol.status
    self.base_info.anger_percent = protocol.anger_percent
    self.base_info.boss_hp_percent_list = protocol.boss_hp_percent_list
    self.base_info.inspire_lv = protocol.inspire_lv
    self.base_info.dragon_vein_close_timestamp = protocol.dragon_vein_close_timestamp
    self.base_info.boss_group_list = protocol.boss_group_list

    if self.base_info.status == 0 then 
        if ViewMgr:IsOpen(LBDragonPluseDetailView) then 
            ViewMgr:CloseView(LBDragonPluseDetailView)
        end 

        if ViewMgr:IsOpen(LBDragonPluseView) then 
            ViewMgr:CloseView(LBDragonPluseView)
        end 

        if ViewMgr:IsOpen(LBDragonPluseInspireView) then 
            ViewMgr:CloseView(LBDragonPluseInspireView)
        end 
    end 

    if self.operate_info.cache ~= nil and self.operate_info.cache.type == 1 then 
        local boss_list = self:GetBossList()
        self:SetOperateBoss(boss_list.boss[self.operate_info.cache.index])
    end 

    self.base_info.flush = self.base_info.flush +1
end

-- 5519 龙脉个人信息
function LBDragonPluseData:SetMyInfo(protocol)
    self.my_info.kill_monster_num = protocol.kill_monster_num
    self.my_info.monster_reward_times = protocol.monster_reward_times
    self.my_info.boss_participate_reward_times = protocol.boss_participate_reward_times
    self.my_info.boss_kill_reward_times = protocol.boss_kill_reward_times
    self.my_info.boss_remain_fight_times = protocol.boss_remain_fight_times
    self.my_info.helper_seq_1 = protocol.helper_seq_1
    self.my_info.helper_seq_2 = protocol.helper_seq_2
    self.my_info.monster_group_list = protocol.monster_group_list

    if self.operate_info.cache ~= nil and self.operate_info.cache.type == 2 then 
        local boss_list = self:GetBossList()
        self:SetOperateBoss(boss_list.monster[self.operate_info.cache.index])
    end 

    self.my_info.flush = self.my_info.flush +1
end

-- 5521 龙脉位置信息
function LBDragonPluseData:SetPostionInfo(protocol)
    for k,v in pairs(self.position_info.pos_list or {}) do
        if  v.row > 0 and v.column > 0 then 
            local camp_cfg = self.dragon_camp_cfg[k+1]
            local fix_pos = LandsBetweenData.FixLink(camp_cfg.dragon_vein_center)
            local center_link_mark = LandsBetweenData.CfgLink({[1] = v.row + fix_pos[1] -1 ,[2] = v.column + fix_pos[2] -1})    
            
            LandsBetweenData.Instance:SetResOfSpPicByLinkMark(nil,center_link_mark,true)
        end 
    end 

    self.position_info.pos_list = protocol.pos_list

    -- 警告目前龙脉模型的基准生成位置依然是习惯中心点而不是生成点
    -- 若后续出现修正需要调整
    -- 简单来讲就是这里有坑
    self.position_info.fix_list = {}
    for k,v in pairs(self.position_info.pos_list or {})  do
        if  v.row > 0 and v.column > 0 then 
            local camp_cfg = self.dragon_camp_cfg[k+1]
            local fix_pos = LandsBetweenData.FixLink(camp_cfg.dragon_vein_center)

            local around = {}
            for i = v.row ,v.row + camp_cfg.dragon_vein_x - 1 do 
                for j = v.column ,v.column  + camp_cfg.dragon_vein_y - 1 do 
                    local link_mark = LandsBetweenData.CfgLink({[1] = i ,[2] = j})    
                    table.insert(around,link_mark)
                end 
            end 
            
            local center_link_mark = LandsBetweenData.CfgLink({[1] = v.row + fix_pos[1] -1 ,[2] = v.column + fix_pos[2] -1})    
            
            local res_name = LandsBetweenData.Instance.map_resources_list[camp_cfg.dragon_vein_number].corresponding_model
            LandsBetweenData.Instance:SetResOfSpPicByLinkMark({
                res_path = string.format(GoldLaw.ResPath,res_name),
                name = "DragonVein",
                link_mark = center_link_mark,
                ui_show_sp_sub = "dragon_vein_sub",
                around = around,
                center_link_mark = center_link_mark,
                big_size = false,
                extral_info = self:ExtralInfo(),
                enable_func = function()
                    return true
                end,
                faction = k,
            },center_link_mark,true)

            self.position_info.fix_list[k] = {row = v.row + fix_pos[1] -1,column = v.column + fix_pos[2] -1}
        end 
    end 
end

function LBDragonPluseData:ExtralInfo()
    return {
        name = Language.LBDragonPluse.OperateName,
        func = function()
            LBDragonPluseCtrl.Instance:OpenDragonPluseView()
        end,
        tips_func = function()
            PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[378].desc})
        end,
        share_func = function()
            LandsBetweenData.Instance:ExtralShareClick(ActType.DragonPluse)
        end
    }
end

-- 获取boss们的位置（注意两个list分开包装且统一输出
function LBDragonPluseData:GetBossList()
    local boss_list = {}
    for i = 1, #self.base_info.boss_group_list do 
        local vo = {}
        local monster_cfg = Config.monster_group_auto.monster_group[self.base_info.boss_group_list[i]] 

        if monster_cfg == nil then
            return {fail = true}
        end

        local monster_id = monster_cfg.show_monster
        vo.model_path = DrawerHelper.GetNpcPath(ResHelper.MonsterResId(monster_id))
        vo.name = monster_cfg.name
        vo.life = self.base_info.boss_hp_percent_list[i]
        vo.times = self.my_info.boss_remain_fight_times[i]
        vo.index = i
        
        vo.type = 1
        table.insert(boss_list,vo)
    end 

    local monster_list = {}
    for i = 1,#self.my_info.monster_group_list do
        local vo = {}
        local monster_cfg = Config.monster_group_auto.monster_group[self.my_info.monster_group_list[i]] 

        if monster_cfg == nil then
            return {fail = true}
        end
        
        local monster_id = monster_cfg.show_monster
        vo.model_path = DrawerHelper.GetNpcPath(ResHelper.MonsterResId(monster_id))
        vo.name = monster_cfg.name


        local dragon_cfg = self:GetMonsterCfg(i,self.my_info.monster_group_list[i])
        -- vo.concerted = dragon_cfg.concerted
        vo.score = dragon_cfg.score
        vo.war_name = dragon_cfg.war_name
        vo.seq_1 = self.my_info.helper_seq_1[i]
        vo.seq_2 = self.my_info.helper_seq_2[i]
        vo.index = i

        vo.rewards = dragon_cfg.rewards
        -- table.insert()

        vo.type = 2
        table.insert(monster_list,vo)
    end 

    return {boss = boss_list, monster = monster_list}
end

function LBDragonPluseData:GetMyInfo()
    local boss_num = 0 
    for k,v in pairs(self.base_info.boss_hp_percent_list) do 
        if v > 0 then 
            boss_num = boss_num + 1
        end 
    end 

    local monster_num = 0
    for k,v in pairs(self.my_info.monster_group_list) do
        if v > 0 then 
            monster_num = monster_num + 1
        end 
    end 

    local cfg = self.dragon_cfg[1]
    return {
        monster_num = monster_num - self.my_info.kill_monster_num,
        boss_num = boss_num,
        monster_reward = cfg.camp_monster_kill_reward_number -  self.my_info.monster_reward_times,
        boss_reward_times = cfg.elite_daily_reward_number - self.my_info.boss_participate_reward_times,
        boss_remain_times = cfg.elite_kill_reward_number - self.my_info.boss_kill_reward_times,
        stamp_time = self.base_info.dragon_vein_close_timestamp,
        inspire_time = self.base_info.inspire_lv > 0 and self.base_info.dragon_vein_close_timestamp or 0,
    }
end

function LBDragonPluseData:GethelpInfoBySeq(seq)
    for k,v in pairs(self.dragon_vein_concerted_war_cfg) do 
        if v.seq == seq then 
            return v
        end 
    end 
end

function LBDragonPluseData:GetInspireShow()
    -- self.base_info.inspire_lv = 1
    local param = {}

    param.eff_cur = self.base_info.inspire_lv == 0 and Language.LBDragonPluse.InspireZero or Config.divine_domain_system_auto.inspire[self.base_info.inspire_lv].inspire_buff_describe
    param.eff_next = self.base_info.inspire_lv == self.dragon_cfg[1].inspire_level_max and Language.LBDragonPluse.MaxInspire or Config.divine_domain_system_auto.inspire[self.base_info.inspire_lv + 1].inspire_buff_describe

    local cur_cfg = self.base_info.inspire_lv == self.dragon_cfg[1].inspire_level_max and Config.divine_domain_system_auto.inspire[self.dragon_cfg[1].inspire_level_max]  or Config.divine_domain_system_auto.inspire[self.base_info.inspire_lv+1] 
    param.cost = cur_cfg ~= nil and cur_cfg.yuanbao_consume or 0
    param.costcoin = cur_cfg ~= nil and cur_cfg.lingyu_consume or 0
    param.is_max =  self.base_info.inspire_lv == self.dragon_cfg[1].inspire_level_max
    return param
end

-- 活动红点显隐
function LBDragonPluseData:GetActRemind()
    return self.my_info.kill_monster_num == 0 and 1 or 0
end

-- 活动倒计时
function LBDragonPluseData:GetActCurTimeStamp()
    return self.base_info.dragon_vein_close_timestamp
end

-- 活动图标显示与否
function LBDragonPluseData:GetActIconShow()
    return true -- self.base_info.status > 0
end

-- 活动状态显示
function LBDragonPluseData:GetActIconStatusShow()
    if self.base_info.status == 1 then 
        return GoldLaw.ActWait
    elseif self.base_info.status == 2 then 
        return  GoldLaw.ActOpen
    end 
    return 3
end

-- 获取boss的奖励
function LBDragonPluseData:GetBossRewardList()
    local total_cfg = Config.divine_domain_system_auto.dragon_vein[1]
    local list = {
        [1] = {title = Language.LBDragonPluse.RewardTitle[1],item_list ={}},
        [2] = {title = Language.LBDragonPluse.RewardTitle[2],item_list ={}},
    }

    if total_cfg.military_exploits_1 > 0 then 
        local item_military_exploits = Item.Init(CommonItemId.GodSpaceExploits,total_cfg.military_exploits_1)
        table.insert(list[2].item_list,item_military_exploits)      
    end     
    if total_cfg.quintessence_1 > 0 then 
        local item_quintessence = Item.Init(CommonItemId.GodSpaceExploits,total_cfg.quintessence_1)
        table.insert(list[2].item_list,item_quintessence)      
    end   
    
    if total_cfg.military_exploits_2 > 0 then 
        local item_military_exploits = Item.Init(CommonItemId.GodSpaceExploits,total_cfg.military_exploits_2)
        table.insert(list[1].item_list,item_military_exploits)      
    end     
    if total_cfg.quintessence_2 > 0 then 
        local item_quintessence = Item.Init(CommonItemId.GodSpaceExploits,total_cfg.quintessence_2)
        table.insert(list[1].item_list,item_quintessence)      
    end   

    -- local list_1 = BagData.Instance:ItemListBuild(total_cfg.elite_daily_reward)
    for i = 0, table.nums(total_cfg.elite_daily_reward)-1 do
        -- LogError("ityem ?",v.item_id)
        table.insert(list[1].item_list,BagData.Instance:ItemBuild(total_cfg.elite_daily_reward[i])) 
    end 

    -- local list_2 = BagData.Instance:ItemListBuild(total_cfg.elite_kill_reward)
    -- for k,v in pairs(total_cfg.elite_kill_reward) do 
        
    for i = 0, table.nums(total_cfg.elite_kill_reward)-1 do
        table.insert(list[2].item_list,BagData.Instance:ItemBuild(total_cfg.elite_kill_reward[i])) 
    end 


    return list
end

function LBDragonPluseData:GetDragonVienNameByLinkMark(link_mark)
    local fix = LandsBetweenData.FixLink(link_mark)
    -- LogError("?fix",fix,self.position_info.fix_list)
    for k,v in pairs(self.position_info.fix_list) do 
        if v.row == fix[1] and v.column == fix[2] then 
            return self.vein_str[k]
        end 
    end 
    return ""
end

function LBDragonPluseData:JumpToMyDragonVein()
    local my_faction = LandsBetweenData.Instance:GetMyFaction()
    local my_vein_pos = self.position_info.fix_list[my_faction]
    LandsBetweenData.Instance:SetMapClick({x = my_vein_pos.row,y = my_vein_pos.column})

    -- TimeHelper:AddDelayTimer(function()
        LandsBetweenData.Instance:SetMapJumpClick({x = my_vein_pos.row,y = my_vein_pos.column})
    -- end,1)
end

function LBDragonPluseData:GetCallFriendCD()
    return math.floor(self.call_friend_cd - TimeCtrl.Instance:GetServerTime())
end

function LBDragonPluseData:SetCallFriendCD()
    self.call_friend_cd = TimeCtrl.Instance:GetServerTime() + 30
end