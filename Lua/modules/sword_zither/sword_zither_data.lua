
SwordZitherData = SwordZitherData or BaseClass()

function SwordZitherData:__init()
    self:SwordZitherData()

    self.sz_info = SmartData.New{}
    self.cur_sel_type = SmartData.New{val = 0}
end

function SwordZitherData:SwordZitherData()
    self.sword_zither_target_id = Config.jiandanqinxin_auto.organize[1].target_id
end

function SwordZitherData:GetCurCfgList()
    local server_start = TimeCtrl.Instance:GetActOpenServerDay(ACTIVITY_TYPE.RAND_SWORD_ZITHER)
	local info_list = {}
    for i, v in pairs(Config.jiandanqinxin_auto.copy_copyreward) do
		if v.section_start <= server_start and v.section_end >= server_start then
			table.insert(info_list, v)
		elseif v.section_start <= server_start and v.section_end == 0 then
			table.insert(info_list, v)
		end
    end
    table.sort(info_list, function(a, b) return a.seq < b.seq end)
    return info_list
end

function SwordZitherData:ResetActCfg()
    local cur_cfg_list = self:GetCurCfgList()
    self.sword_zither_list = {}
    self.layer_list = {}
    for _, info in pairs(cur_cfg_list) do
        self.sword_zither_list[info.checkpoint_layer] = self.sword_zither_list[info.checkpoint_layer] or {
            day = info.checkpoint_layer,
            day_desc = string.format(Language.SwordZither.Day, DataHelper.GetDaXie(info.checkpoint_layer)),
        }
        local t = {seq = info.seq, seq1 = info.seq1, attr = info.characteristic, strategy = info.strategy, skill_icon = info.battle_icon, rewards = {}}
        t.monster_group_id = info.monster_group_id
        for i = 0, #info.reward_item do
            local item = info.reward_item[i]
            table.insert(t.rewards, Item.Init(item.item_id, item.num, item.is_bind))
        end
        local group_c = Cfg.MonsterGroupById(info.monster_group_id)
        local monster_c = Cfg.MonsterById(group_c.show_monster)
        t.boss_icon = monster_c.icon_id
        t.boss_name = monster_c.name
        t.res_id = monster_c.res_id
        self.sword_zither_list[info.checkpoint_layer][info.type] = t

        if self.layer_list[info.type] == nil then
            self.layer_list[info.type] = {}
        end
        table.insert(self.layer_list[info.type],info)
    end

    -- 0全开，1只显示高级，2只显示初级
    local open_cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SWORD_ZITHER, Config.jiandanqinxin_auto.is_open, "seq")
    if open_cfg[1] ~= nil then
        self.cur_open_type = open_cfg[1].is_open
    else
        self.cur_open_type = 0
    end
end

function SwordZitherData:GetCurOpenType()
    return self.cur_open_type or 0
end

function SwordZitherData:SetRAJianDanQinXinInfo(protocol)
    self.sz_info:Set(protocol.info)
    self:ResetActCfg()
    for _, info in pairs(self.sword_zither_list) do
        info.is_open, info.pass = self:GetPassState(info)
    end
end

function SwordZitherData:GetSwordZitherList()
    return self.sword_zither_list
end

function SwordZitherData:GetPassState(info)
    return info.day <= self.sz_info.open_day, {
        [0] = 1 == bit:_rshift(self.sz_info.flag, info[0].seq1) % 2,
        [1] = 1 == bit:_rshift(self.sz_info.flag, info[1].seq1) % 2,
    }
end

function SwordZitherData:GetPassLayerNum(flag)
    local curr_num = {}
    for k,list in pairs(self.layer_list) do
        local num = 0
        for _,v in pairs(list) do
            num = num + (flag[32 - v.seq1] or 0)
        end
        curr_num[k] = num >= #list and 1 or 0
    end
    return curr_num
end