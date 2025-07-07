FairyDragonData = FairyDragonData or BaseClass()

function FairyDragonData:__init()
    if FairyDragonData.Instance ~= nil then
        Debuger.LogError("[FairyDragonData] attempt to create singleton twice!")
        return
    end
    FairyDragonData.Instance = self
    self.dragon = FairyDragon.New()
    self.dragon = FairyDragon.Instance
    
    self:InitCfg()

    self.sel_tab_data = SmartData.New()         -- 选中的标签页
    
    self.soul_item_list = SmartData.New()       -- 龙魂道具列表

    self.sel_star_clicp = nil                   -- 选中的要激活星星点

    self.pray_result_info = SmartData.New()     -- 祈神结果
end

function FairyDragonData:ClearData()

end

function FairyDragonData:InitCfg()
    self.cfg = Config.god_dragon_auto
    self.other_cfg = Config.god_dragon_auto.other[1]

    -- 培养配置
    Cfg.Register("FairyDragonFosterCfg",Cfg.RealTimeSearch2Key(self.cfg.dragon_up,"prof","dragon_level"))
    -- 升星配置
    Cfg.Register("FairyDragonStarCfg",Cfg.RealTimeSearch2Key(self.cfg.dragon_star_break,"prof","dragon_star"))
    -- 升星突破组
    self.dragon_star_break_group_cfg = DataHelper.ConvertGroup(self.cfg.dragon_star_break, "prof")
    -- 升星属性配置
    Cfg.Register("FairyDragonAttriStarCfg",Cfg.RealTimeSearch3Key(self.cfg.dragon_star,"prof","dragon_star", "point"))
    -- 升星技能配置
    --Cfg.Register("FairyDragonStarSkillCfg",Cfg.RealTimeSearch2Key(self.cfg.star_skill,"dragon_star","point"))

    -- 皮肤升星加成配置
    Cfg.Register("FairyDragonSkinAttriCfg",Cfg.RealTimeSearch3Key(self.cfg.dragon_skin_up_star, "prof", "skin_group", "star_level"))
    
    -- 技能配置
    Cfg.Register("FairyDragonSkillCfg",Cfg.RealTimeSearch(self.cfg.skill_dec,"skill_id"))
    -- 技能配置2
    Cfg.Register("FairyDragonSkillCfg2",Cfg.RealTimeSearch2Key(self.cfg.skill_dec,"group", "skill_level"))

    local dragon_star_group = DataHelper.ConvertGroup(self.cfg.dragon_star, "prof")
    self.star_gorup_cfg = {}
    for i,v in pairs(dragon_star_group) do
        self.star_gorup_cfg[i] = DataHelper.ConvertGroup(v, "dragon_star")
    end
    self.skin_star_group_cfg = DataHelper.ConvertGroup(self.cfg.dragon_skin_up_star, "skin_group")


    -- 神龙战斗龙魂技能配置
    Cfg.Register("FairyDragonBattleSkillCfg",Cfg.RealTimeSearch(Config.dragon_soul_skill_auto.skill_cfg,"skill_id"))

    -- 随机属性配置
    Cfg.Register("FairyDragonRandAttriCfg",Cfg.RealTimeSearch3Key(self.cfg.random_group, "attri_group", "attri_affix", "attri_level"))
end

-- 处理服务端的通知逻辑
function FairyDragonData:HandleNotice(protocol)
    local info = protocol.info
    local notice_type = info.notice_type
    local type_cfg = FairyDragonCfg.NOTICE_TYPE
    if notice_type == type_cfg.UNLOCK then
        self.dragon.base_data.level = info.p1
    elseif notice_type == type_cfg.EXP_CHANGE then
        -- 经验变动，升级提示
        local old_level = self.dragon:Level()
        local old_attri_list = self.dragon:AttriList()
        self.dragon.base_data.level = info.p1
        self.dragon.base_data.is_break_flag = info.p2
        self.dragon.base_data.exp = info.p3
        if info.p1 > old_level and not ViewMgr:IsOpen(FairyDragonBreachTip) then
            local new_sttri_list = self.dragon:AttriList()
            local attri_list = self:FormatAttriListCpmpare(old_attri_list, new_sttri_list)
            local item = Item.Create({item_id = FairyDragon.Instance:ItemId()})
            local param_t = {
                last_level = old_level,
                cur_level = info.p1,
                level_text = Language.FairyDragon.Level,
                attri_list = attri_list,
                is_show_skill = false,
                --item = item
            }
            ViewMgr:OpenView(FairyDragonUpTip,param_t)
        end
    elseif notice_type == type_cfg.BREAK then
        self.dragon.base_data.level = info.p1
        self.dragon.base_data.is_break_flag = info.p2
        ViewMgr:OpenView(FairyDragonBreachTip)
    elseif notice_type == type_cfg.ACTIVE_STAR_NODE then
        local old_skill_list = self.dragon:StarSkillList()
        local old_attri_list = self:StarAllAttriList()

        local node_index = info.p1 + 1
        self.dragon.base_data.star_node_flag[node_index] = 1
        self.dragon.base_data:SetDirty("star_node_flag")

        local new_skill_list = self.dragon:StarSkillList()
        local old_skill
        local change_skill
        for i, v in pairs(new_skill_list) do
            old_skill = old_skill_list[i]
            if not old_skill or v.skill_id ~= old_skill.skill_id then
                change_skill = v
                break
            end
        end

        --local prof = RoleData.Instance:ProfType()
        --local node_cfg = Cfg.FairyDragonAttriStarCfg(prof, self.dragon:Star(), node_index)
        --local node_attri = self:GetStarClipAttri(node_cfg)
        --local attri_list = self:FormatAttriListCpmpare({}, node_attri)
        local attri_list = self:FormatAttriListCpmpare(old_attri_list, self:StarAllAttriList())
        local param_t = {
            last_level = self.dragon:Star(),
            cur_level = self.dragon:Star(),
            level_text = Language.FairyDragon.Star,
            attri_list = attri_list,
            is_show_skill = change_skill ~= nil,
            skill_item = change_skill,
        }
        if change_skill then
            param_t.cur_skill_desc = change_skill:Desc()
            if old_skill then
                param_t.last_skill_desc = old_skill:Desc()
            end
        end
        --ViewMgr:OpenView(FairyDragonUpTip,param_t)

        -- 逻辑换了。上面代码需真理
        if change_skill then
            ViewMgr:OpenView(FairyDragonStarSkillUp, {skill = change_skill})
        end
    elseif notice_type == type_cfg.STAR_BREAK then
        local old_skill_list = self.dragon:StarSkillList()
        local old_attri_list = self:StarAllAttriList()

        local old_level = self.dragon:Star()
        local new_level = info.p1
        for i,v in pairs(self.dragon.base_data.star_node_flag or {}) do
            self.dragon.base_data.star_node_flag[i] = 0
        end
        self.dragon.base_data.star_level = new_level

        local new_skill_list = self.dragon:StarSkillList()
        local old_skill
        local change_skill
        for i, v in pairs(new_skill_list) do
            old_skill = old_skill_list[i]
            if not old_skill or v.skill_id ~= old_skill.skill_id then
                change_skill = v
                break
            end
        end

        --local star_attri = self:GetStarCfgAttri(new_level)
        --local attri_list = self:FormatAttriListCpmpare({}, star_attri)
        local attri_list = self:FormatAttriListCpmpare(old_attri_list, self:StarAllAttriList())
        local param_t = {
            last_level = old_level,
            cur_level = new_level,
            level_text = Language.FairyDragon.Star,
            attri_list = attri_list,
            is_show_skill = change_skill ~= nil,
            skill_item = change_skill,
        }
        if change_skill then
            param_t.cur_skill_desc = change_skill:Desc()
            if old_skill then
                param_t.last_skill_desc = old_skill:Desc()
            end
        end
        ViewMgr:OpenView(FairyDragonStarUpTip,param_t)

        if change_skill then
            ViewMgr:OpenView(FairyDragonStarSkillUp, {skill = change_skill})
        end
    elseif notice_type == type_cfg.SKIN_ACTIVE then
        skin_info = {}
        skin_info.seq = info.p1
        skin_info.is_unlock = 1
        skin_info.star_level = info.p2
        self.dragon.base_data.skin_info_list[info.p1] = skin_info
        self.dragon.base_data:SetDirty("skin_info_list")
    elseif notice_type == type_cfg.SKIN_STAR_LEVEL_UP then
        local last_skin = self.dragon:GetSkin(info.p1)
        local last_level = last_skin:StarLevel()
        local last_attri_list = last_skin:AttriList()

        skin_info = {}
        skin_info.seq = info.p1
        skin_info.is_unlock = 1
        skin_info.star_level = info.p2
        self.dragon.base_data.skin_info_list[info.p1] = skin_info
        self.dragon.base_data:SetDirty("skin_info_list")
 
        local cur_skin = self.dragon:GetSkin(info.p1)
        local star_list = cur_skin:GetStarUIList()
        -- 皮肤升星成功提示
        local attri_list = self:FormatAttriListCpmpare(last_attri_list, cur_skin:AttriList())
        local param_t = {
            last_level = last_level,
            cur_level = skin_info.star_level,
            level_text = Language.FairyDragon.Star,
            attri_list = attri_list,
            is_show_skill = false,
            star_list = star_list
        }
        --ViewMgr:OpenView(FairyDragonUpTip,param_t)
        ViewMgr:OpenView(FairyDragonStarUpTip,param_t)
    elseif notice_type == type_cfg.WEAR_DRAGON_SOUL then
        local soul_type = info.p1
        local index = info.p2
        local soul_info = {}
        soul_info.is_unlock = 1
        soul_info.index = index
        self.dragon.base_data.soul_info_list[soul_type] = soul_info
        self.dragon.base_data:SetDirty("soul_info_list")
    elseif notice_type == type_cfg.SKIN_CHANGE then
        self.dragon.base_data.use_skin_seq = info.p1
    elseif notice_type == type_cfg.SOUL_GRID_UNLOCK then
        local soul_type = info.p1
        local soul_info = {}
        soul_info.is_unlock = 1
        soul_info.index = -1
        self.dragon.base_data.soul_info_list[soul_type] = soul_info
        self.dragon.base_data:SetDirty("soul_info_list")
    elseif notice_type == type_cfg.SOUL_RESOLVE then
        -- local num = info.p3
        -- local item_id = Config.god_dragon_auto.other[1].dragon_soul_chips
        -- LogDG("龙魂分解获得的碎片", num)
    elseif notice_type == type_cfg.SYSTEM_OPEN then
        local is_open = info.p1 or 0
        self.dragon.base_data.is_sys_open = is_open
        ViewMgr:OpenView(FairyDragonOpenView)
        LandsBetweenData.Instance:ForceFlushExtra()
        MainViewData.Instance:FlushRbButton()
    elseif notice_type == type_cfg.UNFIX_SOUL then
        local soul_type = info.p1
        local info = self.dragon:GetSoulInfo(soul_type)
        local new_info = TableCopy(info)
        new_info.index = -1
        FairyDragon.Instance.base_data.soul_info_list[soul_type] = new_info
        FairyDragon.Instance.base_data:SetDirty("soul_info_list")
    end
end


-- 属性转评分
function FairyDragonData:GetScoreByAttriList(attri_list)
    if TableIsEmpty(attri_list) then
        return 0
    end
    local job = RoleData.Instance:ProfType()
    local co = Cfg.ScaleScoreCfgByProf(job * 100)
    local score = 0
    for k, v in pairs(attri_list) do
        local filed_name = AttrNameForType[v.attri_type]
        if co[filed_name] then 
            score = score + co[filed_name] * v.attri_value
        end 
    end
    
    return score
end

-- 两条属性转对比
function FairyDragonData:FormatAttriListCpmpare(attri_list_1, attri_list_2)
    local list = {}
    for i,v in pairs(attri_list_2) do
        local attri = {}
        attri.attri_type = v.attri_type
        attri.attri_value_2 = v.attri_value
        for _,v1 in pairs(attri_list_1) do
            if v1.attri_type == v.attri_type then
                attri.attri_value_1 = v1.attri_value
            end
        end
        attri.attri_value_1 = attri.attri_value_1 or 0
        table.insert(list, attri)
    end
    return list
end

-- 》》》》》》》》》》》》》》》》 基础 《《《《《《《《《《《《《《《
-- 神龙功能开启
function FairyDragonData:IsOpen()
    local is_sys_open = FairyDragon.Instance.base_data.is_sys_open or 0
    if is_sys_open == 0 then
        return false
    end
    return true
end


-- 当前选中的标签页
function FairyDragonData:GetSelTab()
    return self.sel_tab_data
end
function FairyDragonData:SetSelTab(tab_data)
    self.sel_tab_data:Set(tab_data or {})
end


-- 》》》》》》》》》》》》》》》》 培养 《《《《《《《《《《《《《《《
function FairyDragonData:GetFosterCfg(level)
    level = level or self.dragon:Level()
    if not level or level < 1 then
        return {}
    end
    local prof = RoleData.Instance:ProfType()
    local cfg = Cfg.FairyDragonFosterCfg(prof,level)
    return cfg
end

function FairyDragonData:NextFosterCfg()
    local next_cfg
    local level = self.dragon:Level()
    if self.dragon:IsMaxLevel() then
        next_cfg = self:GetFosterCfg(level)
    else
        next_cfg = self:GetFosterCfg(level + 1)
    end
    return next_cfg
end

function FairyDragonData:MaxLevel()
    return self.cfg.dragon_up[#self.cfg.dragon_up].dragon_level
end

-- 返回当前培养属性 + 下级培养属性列表。满级下级培养属性 == 当前培养属性
function FairyDragonData:FosterAttriList()
    local attri_list = {}
    local level = self.dragon:Level()
    local cfg = self:GetFosterCfg(level) or {}
    local next_cfg
    if self.dragon:IsMaxLevel() or self.dragon:IsNeedBreak() then
        next_cfg = cfg
    else
        next_cfg = self:GetFosterCfg(level + 1)
    end
    for i = 1, 3 do
        local attri = {}
        attri.attri_type = next_cfg["attri_" .. i]
        attri.cur_attri_value = cfg["attri_num_" .. i] or 0
        attri.next_attri_value = next_cfg["attri_num_" .. i] or 0
        table.insert(attri_list, attri)
    end
    return attri_list
end

-- 培养材料配置
function FairyDragonData:FosterStuffCfg()
    if not self.foster_stuff_cfg then
        self.foster_stuff_cfg = {}
        for i,v in pairs(self.cfg.item_type) do
            if v.item_type == 2 then
                self.foster_stuff_cfg[v.item_id] = v.exp_num
            end
        end
    end
    return self.foster_stuff_cfg
end


-- 》》》》》》》》》》》》》》》》 升星 《《《《《《《《《《《《《《《
function FairyDragonData:GetStarGroupCfg(star_level)
    local prof = RoleData.Instance:ProfType()
    return self.star_gorup_cfg[prof][star_level] or {}
end

function FairyDragonData:GetStarCfg(star_level)
    local prof = RoleData.Instance:ProfType()
    return Cfg.FairyDragonStarCfg(prof,star_level)
end

function FairyDragonData:GetStarCfgAttri(star_level)
    local star_cfg = self:GetStarCfg(star_level)
    local attri_list = {}
    for j = 1, star_cfg.break_attri_num do
        local attri_type = star_cfg["break_attri_" .. j]
        local attri_value = star_cfg["break_attri_num_" .. j]
        if not attri_list[attri_type] then
            attri_list[attri_type] = {attri_type = attri_type, attri_value = 0}
        end
        attri_list[attri_type].attri_value = attri_list[attri_type].attri_value + attri_value
    end
    return attri_list
end

function FairyDragonData:MaxStar()
    local prof = RoleData.Instance:ProfType()
    local cfg = self.dragon_star_break_group_cfg[prof] or {}
    return cfg[#cfg].dragon_star
end

-- data == Config.god_dragon_auto.dragon_star
function FairyDragonData:GetStarClicpState(data)
    if TableIsEmpty(self.dragon.base_data.star_node_flag) then
        return 0
    end
    if data.dragon_star < self.dragon:Star() then
        return 1
    end
    return self.dragon.base_data.star_node_flag[data.point]
end

function FairyDragonData:GetSelStarClip()
    return self.sel_star_clicp
end
function FairyDragonData:SetSelStarClip(data)
    self.sel_star_clicp = data
end

function FairyDragonData:GetStarClipAttri(data)
    local attri_list = {}
    for i = 1, data.affix_num do
        local attri = {}
        attri.attri_type = data["attri_" ..  i]
        attri.attri_value = data["attri_num_" .. i]
        table.insert(attri_list, attri)
    end
    return attri_list
end

-- 升星属性加成总览
function FairyDragonData:StarAllAttriList()
    local attri_list = {}
    local star = self.dragon:Star()
    local max_star = self:MaxStar()
    local t_star = star >= max_star and max_star - 1 or star
    for i = 0, t_star do
        local group_cfg = self:GetStarGroupCfg(i)
        for _, v in pairs(group_cfg) do
            local state = self:GetStarClicpState(v)
            if state == 1 then
                local list = self:GetStarClipAttri(v)
                for _,attri in pairs(list) do
                    if not attri_list[attri.attri_type] then
                        attri_list[attri.attri_type] = {attri_type = attri.attri_type, attri_value = 0}
                    end
                    attri_list[attri.attri_type].attri_value = attri_list[attri.attri_type].attri_value + attri.attri_value
                end
            end
        end
    end

    star = self.dragon:Star()
    if star > 0 then
        for i = 1, star do
            local star_cfg = self:GetStarCfg(i)
            for j = 1, star_cfg.break_attri_num do
                local attri_type = star_cfg["break_attri_" .. j]
                local attri_value = star_cfg["break_attri_num_" .. j]
                if not attri_list[attri_type] then
                    attri_list[attri_type] = {attri_type = attri_type, attri_value = 0}
                end
                attri_list[attri_type].attri_value = attri_list[attri_type].attri_value + attri_value
            end
        end
    end

    return attri_list
end

function FairyDragonData:IsMaxStar()
    local star = self.dragon:Star()
    if star >= self:MaxStar() then
        return true
    end
    return false
end

-- 是否需要升星
function FairyDragonData:IsNeedUpStar()
    local star = self.dragon:Star()
    if star >= self:MaxStar() then
        return false
    end
    local cfg = self:GetStarGroupCfg(star)
    for i,v in pairs(cfg) do
        local state = self:GetStarClicpState(v)
        if state == 0 then
            return false
        end
    end
    return true
end

-- 1可激活节点，0材料不足 -1，已激活,-2,未激活神龙
function FairyDragonData:GetCtrlStarNodeFlag(node_cfg)
    if not self.dragon:IsActive() then
        return -2
    end
    local state = self:GetStarClicpState(node_cfg)
    if state == 1 then
        return -1
    end
    local has_num = Item.GetNum(node_cfg.expend_item)
    local need_num = node_cfg.expend_num
    if has_num >= need_num then
        return 1
    else
        return 0
    end
end

-- 1可升星，0材料不足，-1，已满级
function FairyDragonData:GetCtrlStarBreakFlag(star_break_cfg)
    if self:IsMaxStar() then
        return -1
    end
    local has_num = Item.GetNum(star_break_cfg.break_expend_item)
    local need_num = star_break_cfg.break_expend_num
    if has_num >= need_num then
        return 1
    else
        return 0
    end
end

-- 获取星级技能在哪个配置
function FairyDragonData:GetStarAttriCfgBySkillId(skill_id)
    -- for i,v in pairs(self.cfg.dragon_star) do
    --     for n = 1, 3 do
    --         local t_skill_id = v["skill_" .. n]
    --         if t_skill_id and skill_id == t_skill_id then
    --             return v
    --         end
    --     end
    -- end

    for i,v in pairs(self.cfg.star_skill) do
        for n = 1, 3 do
            local t_skill_id = v["skill_" .. n]
            if t_skill_id and skill_id == t_skill_id then
                return v
            end
        end
    end
end


-- 根据星级和星级节点数量拿到星级技能配置
function FairyDragonData:GetStarSkillCfg(star_level, star_num)
    local t_num = star_level * FairyDragon.MaxStarNodeCount + star_num
    for i = #self.cfg.star_skill, 1, -1 do
        local v = self.cfg.star_skill[i]
        local cfg_num = v.dragon_star * FairyDragon.MaxStarNodeCount + v.point
        if t_num >= cfg_num then
            return v
        end
    end
end


-- 》》》》》》》》》》》》》》》》 皮肤 《《《《《《《《《《《《《《《

function FairyDragonData:GetSkinCfgBySeq(seq)
    local cfg = self.cfg.dragon_skin
    return cfg[seq + 1]
end

function FairyDragonData:GetSkinCfgByItem(item_id)
    local cfg = self.cfg.dragon_skin
    for i,v in pairs(cfg) do
        if v.skin_item_id == item_id then
            return v 
        end
    end
end

-- 根据品质获取皮肤配置
function FairyDragonData:GetSkinList(qua)
    if not self.skin_group_list then
        self.skin_group_list = {}
        local cfg = self.cfg.dragon_skin
        local list = {}
        for i, v in pairs(cfg) do
            local qua = Item.GetColor(v.skin_item_id)
            if not self.skin_group_list[qua] then
                self.skin_group_list[qua] = {}
            end
            local skin = FairyDragonSkin.New(v)
            table.insert(self.skin_group_list[qua], skin)
        end
    end
    
    return self.skin_group_list[qua] or {}
end

function FairyDragonData:GetSkinUpStarCfg(group_id, level)
    local prof = RoleData.Instance:ProfType()
    local cfg = Cfg.FairyDragonSkinAttriCfg(prof, group_id, level)
    return cfg
end

-- 获取皮肤升星加成
function FairyDragonData:GetSkinStarAttri(group_id, level)
    local cfg = self:GetSkinUpStarCfg(group_id, level)
    if TableIsEmpty(cfg) then
        return {}
    end
    local attri_list = {}
    for i = 1, cfg.affix_num do
        local attri = {}
        attri.attri_type = cfg["attri_type_" .. i]
        attri.attri_value = cfg["attri_num_" .. i]
        table.insert(attri_list, attri)
    end
    return attri_list
end

-- 皮肤是否能操作，2可以激活，1可以升星，0材料不足，-1已满星 , -2 神龙未激活
function FairyDragonData:GetSkinCtrlFlag(skin)
    if not self.dragon:IsActive() then
        return -2
    end
    if skin:IsMaxStar() then
        return -1
    end
    local star_cfg = skin:UpStarCfg()
    if not star_cfg then
        return 0
    end
    local has_num = Item.GetNum(star_cfg.up_expend)
    if has_num <= 0 then
        return 0
    end
    if not skin:IsActive() then
        return 2
    else
        return 1
    end
end

-- 》》》》》》》》》》》》》》》》 龙魂 《《《《《《《《《《《《《《《

-- 龙魂逻辑在 >>> FairyDragonSoulData

-- 》》》》》》》》》》》》》》》》 祈愿 《《《《《《《《《《《《《《《

-- 获取展示的配置
function FairyDragonData:GetPrayShowList()
    local cfg = self.cfg.dragon_pray
    local list = {}
    for i,v in pairs(cfg) do
        if v.is_show == 1 then
            table.insert(list, v)
        end
    end
    return list
end

function FairyDragonData:GetPrayItemCfg(sn)
    return self.cfg.dragon_pray[sn + 1]
end

-- 单抽材料
function FairyDragonData:GetOnePrayStuff()
    return self.other_cfg.one_expend
end

-- 十连材料
function FairyDragonData:GetTenPrayStuff()
    return self.other_cfg.ten_expend
end

-- 》》》》》》》》》》》》》》》》 红点 《《《《《《《《《《《《《《《

-- =================== 培养红点
-- 培养标签红点
function FairyDragonData:RemindFosterTab()
    local prof = RoleData.Instance:ProfType()
    if prof == nil or prof == 0 then
        return 0
    end
    local remind = self:RemindUnlockDragon()
    if remind > 0 then
        return 1
    end
    remind = self:RemindFoster()
    if remind > 0 then
        return 1
    end
    return 0
end

-- 解锁神龙红点
function FairyDragonData:RemindUnlockDragon()
    if not self.dragon:IsActive() then
        return 1
    end
    return 0
end

-- 培养审计突破红点
function FairyDragonData:RemindFoster()
    if not self.dragon:IsActive() then
        return -2
    end
    if self.dragon:IsMaxLevel() then
        return 0
    end

    local is_need_break = self.dragon:IsNeedBreak()
    if is_need_break then
        local cur_foster_cfg = self:GetFosterCfg()
        local num = Item.GetNum(cur_foster_cfg.break_item_id)
        local need_num = cur_foster_cfg.break_item_num
        if num >= need_num then
            return 1
        end
    else
        -- 背包材料值多少经验
        local exp = 0
        local stuff_list = self:FosterStuffCfg()
        for i,v in pairs(stuff_list) do
            local num = Item.GetNum(i)
            exp = exp + v * num
        end
        local cur_exp = self.dragon:CurExp()
        local next_exp = self.dragon:NextExp()
        local need_exp = next_exp - cur_exp
        if exp >= need_exp then
            return 1
        end
    end
    return 0
end

-- =================== 升星红点
-- 升星标签红点
function FairyDragonData:RemindStarTab()
    local prof = RoleData.Instance:ProfType()
    if prof == nil or prof == 0 then
        return 0
    end
    local remind = self:RemindStarUp()
    if remind > 0 then
        return 1
    end
    remind = self:RemindAllStarNode()
    if remind > 0 then
        return 1
    end
    return 0
end


-- 升星红点
function FairyDragonData:RemindStarUp()
    local star_level = self.dragon:Star()
    local max_star = self:MaxStar()
    if star_level >= max_star then
        return 0
    end

    local is_need_star = self:IsNeedUpStar()
    local item_id
    local need_num
    if is_need_star then
        local star_cfg = self:GetStarCfg(self.dragon:Star() + 1)
        if not star_cfg then
            return 0
        end
        item_id = star_cfg.break_expend_item
        need_num = star_cfg.break_expend_num
        local num = Item.GetNum(item_id)
        if num >= need_num then
            return 1
        end  
    end
    return 0
end

-- 星级节点红点
function FairyDragonData:RemindAllStarNode()
    local star_level = self.dragon:Star()
    local max_star = self:MaxStar()
    if star_level >= max_star then
        return 0
    end
    local cfg = self:GetStarGroupCfg(star_level)
    for i,v in pairs(cfg) do
        local remind = self:GetCtrlStarNodeFlag(v)
        if remind > 0 then
            return 1
        end
    end
    return 0
end

-- function FairyDragonData:RemindStarNode(cfg)
--     local item_id = cfg.expend_item
--     local need_num = cfg.expend_num
--     local num = Item.GetNum(item_id)
--     if num >= need_num then
--         return 1
--     end
--     return 0
-- end

-- =================== 升星红点
-- 皮肤标签红点
function FairyDragonData:RemindSkinTab()
    if not self:IsOpen() then
        return 0
    end
    local prof = RoleData.Instance:ProfType()
    if prof == nil or prof == 0 then
        return 0
    end
    local skin_list = self.dragon:GetSkinList()
    for i, v in pairs(skin_list) do
        local remind = self:RemindSkin(v)
        if remind > 0 then
            return 1
        end
    end
    return 0
end

function FairyDragonData:RemindSkin(skin)
    if not self:IsOpen() then
        return 0
    end
    -- local item_id = skin.item_id
    -- local num = Item.GetNum(item_id)
    -- if skin:IsMaxStar() then
    --     return 0
    -- end
    -- if num > 0 then
    --     return 1
    -- end
    -- return 0
    local flag = self:GetSkinCtrlFlag(skin)
    if flag > 0 then
        return 1
    end
    return 0
end

-- =================== 龙魂红点
-- 龙魂标签红点
function FairyDragonData:RemindSoulTab()
    if not self:IsOpen() then
        return 0
    end
    local prof = RoleData.Instance:ProfType()
    if prof == nil or prof == 0 then
        return 0
    end
    for i = 0, FairyDragonCfg.SOUL_TYPE.Max do
        local remind = self:RemindBySoulType(i)
        if remind > 0 then
            return 1
        end
    end
    return 0
end


-- 龙魂格子红点
function FairyDragonData:RemindBySoulType(soul_type)
    if not self:IsOpen() then
        return 0
    end
    if not self.dragon:IsActive() then
        return 0
    end
    local info = self.dragon:GetSoulInfo(soul_type) or {}
    local is_unlock = info.is_unlock == 1
    if not is_unlock then
        return 0
    end
    if info.index < 0 then
        local item_list = FairyDragonSoulData.Instance:GetItemList()
        for i,v in pairs(item_list) do
            if v:SoulType() == soul_type then
                return 1
            end
        end
    else
        local item = FairyDragonSoulData.Instance:GetItem(info.index)
        local flag = self:RemindBySoulItem(item)
        if flag > 0 then
            return 1
        end
    end
    return 0
end

-- 龙魂全部红点
function FairyDragonData:RemindBySoulItem(soul_item)
    if not self:IsOpen() then
        return 0
    end
    local rep_remind = self:RemindSoulRep(soul_item)
    if rep_remind > 0 then
        return 0
    end
    return 0
end

-- 龙魂替换红点
function FairyDragonData:RemindSoulRep(soul_item)
    if not self:IsOpen() then
        return 0
    end
    if not soul_item then
        return 0
    end
    local soul_type = soul_item:SoulType()
    local score = soul_item:Score()
    local item_list = FairyDragonSoulData.Instance:GetItemList()
    for i,v in pairs(item_list) do
        if v:SoulType() == soul_type and v:Score() > score then
            return 1
        end
    end
    return 0
end

-- 龙魂升级红点 :: 用不到 ，用经验升级已废弃
function FairyDragonData:RemindSoulUp(soul_item)
    -- if soul_item:IsFullLevel() then
    --     return 0
    -- end
    -- local need_exp = soul_item:FullExp()
    -- local exp = 0
    -- for i,v in pairs(self.cfg.item_type) do
    --     if v.item_type == 2 then
    --         local num = Item.GetNum(v.item_id)
    --         exp = exp + v.exp_num
    --     end
    -- end
    -- if exp >= need_exp then
    --     return 1
    -- end
    return 0
end

-- =================== 祈愿红点
-- 标签红点
function FairyDragonData:RemindPrayTab()
    if not self:IsOpen() then
        return 0
    end
    local remind = self:RemindPrayOne()
    if remind > 0 then
        return 1
    end
    remind = self:RemindPrayTen()
    if remind > 0 then
        return 1
    end
    return 0
end

-- 单抽红点
function FairyDragonData:RemindPrayOne()
    local cfg = self:GetOnePrayStuff()
    local has_num = Item.GetNum(cfg.item_id)
    if has_num >= cfg.num then
        return 1
    end
    return 0
end

-- 十抽红点
function FairyDragonData:RemindPrayTen()
    local cfg = self:GetTenPrayStuff()
    local has_num = Item.GetNum(cfg.item_id)
    if has_num >= cfg.num then
        return 1
    end
    return 0
end


-- 所有红点用到的item_id
function FairyDragonData:GetRemindItemList()
    if not self.remind_item_list then
        local list = {}
        local func = function (item_id)
            if not list[item_id] then
                list[item_id] = true
            end
        end

        -- 龙魂经宴和龙魄
        for i,v in pairs(self.cfg.item_type) do
            func(v.item_id)
        end

        -- 突破
        func(56320)

        -- 皮肤
        local skin_list = self.dragon:GetSkinList()
        for i,v in pairs(skin_list) do
            func(v.item_id)
        end

        -- 升星突破
        func(56309)
        -- 升星
        for i,v in pairs(self.cfg.dragon_star) do
            func(v.expend_item)
        end

        -- 祈愿材料
        local cfg = self:GetOnePrayStuff()
        func(cfg.item_id)
        cfg = self:GetTenPrayStuff()
        func(cfg.item_id)

        self.remind_item_list = {}
        for i,v in pairs(list) do
            table.insert(self.remind_item_list, i)
        end
    end
    return self.remind_item_list
end

function FairyDragonData:RemindNotice()
    self.dragon.base_data:Notify()
end
