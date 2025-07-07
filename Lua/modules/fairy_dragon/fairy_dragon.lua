-- 神龙 ：方便拿神龙信息
FairyDragon = FairyDragon or BaseClass()
FairyDragon.MaxStarNodeCount = 10 -- 升星最多有多少个节点
function FairyDragon.DecodeBaseInfo()
    local t = {}
    t.is_break_flag = MsgAdapter.ReadChar()     --该等级是否突破过（培养）
    t.is_sys_open = MsgAdapter.ReadChar()       --神龙玩法是否开启
    t.use_skin_seq = MsgAdapter.ReadShort()     --//幻化皮肤skin_seq -1:没有幻化
    t.level = MsgAdapter.ReadInt()
    t.exp = MsgAdapter.ReadLL()                 --当前经验
    t.star_level = MsgAdapter.ReadInt()         --神龙星级

    local count = MsgAdapter.ReadShort()
    t.star_node_flag = MsgAdapter.IntFlag(1,true)   --//神龙升星节点激活标识

    count = MsgAdapter.ReadShort()
    t.soul_info_list = {}    --//龙魂佩戴信息
    for i = 0, count - 1 do  -- i == FairyDragonCfg.SOUL_TYPE
        local info = {}
        info.is_unlock = MsgAdapter.ReadChar()  --//是否解锁
        info.index = MsgAdapter.ReadShort()     --//对应背包格子序号
        t.soul_info_list[i] = info
    end

    count = MsgAdapter.ReadShort()
    t.skin_info_list = {}    --//神龙皮肤信息列表
    for i = 0,count - 1 do
        local info = {}
        info.seq = MsgAdapter.ReadChar()
        info.is_unlock = MsgAdapter.ReadChar()  --//是否解锁
        info.star_level = MsgAdapter.ReadShort()--//皮肤星级等级
        t.skin_info_list[info.seq] = info
    end
    return t
end

function FairyDragon:__init()
    if FairyDragon.Instance ~= nil then
        Debuger.LogError("[FairyDragon] attempt to create singleton twice!")
        return
    end
    FairyDragon.Instance = self
    self.ctrl_data = FairyDragonData.Instance
    self.item_id = Config.god_dragon_auto.other[1].jihuo_icon
    self.base_data = SmartData.New()
    self.skin_list = {}

    self.show_skill_list = SmartData.New()      -- 需要展示的技能
end

function FairyDragon:IsActive()
    return self:Level() ~= 0
end

function FairyDragon:GetBaseData()
    return self.base_data
end
function FairyDragon:SetBaseData(data)
    data = data or {}
    self.base_data:Set(data)
end

function FairyDragon:ItemId()
    local skin = self:Skin()
    if skin then
        return skin.item_id
    end
    return self.item_id
end

-- 有皮肤则显示皮肤名字，默认名字''神龙'
function FairyDragon:Name()
    local skin = self:Skin()
    if skin then
        return skin:Name()
    else
        return Language.FairyDragon.Name
    end
end

function FairyDragon:ResId()
    local skin = self:Skin()
    if skin then
        return skin:ResId()
    else
        return FairyDragonData.Instance.other_cfg.res
    end
end

--有皮肤就拿皮肤品质，没有默认紫色
-- function FairyDragon:Qua()
--     return ItemColor.Purple
-- end

function FairyDragon:Score()
    local score = 0
    local attri_list = self:AttriList()
    score = self.ctrl_data:GetScoreByAttriList(attri_list)
    local skin = self:Skin()
    if skin then
        score = score + skin:Score()
    end
    local soul_info_list = self.base_data.soul_info_list or {}
    for i,v in pairs(soul_info_list) do
        if v.index >= 0 then
            local soul_item = FairyDragonSoulData.Instance:GetItem(v.index)
            if soul_item and soul_item.item_id ~= 0 then
                score = score + soul_item:Score()
            end
        end
    end
    -- 星级属性
    local star_attri_list = self.ctrl_data:StarAllAttriList()
    score = score + self.ctrl_data:GetScoreByAttriList(star_attri_list)
    return score
end

function FairyDragon:Level()
    return self.base_data.level or 0
end

function FairyDragon:MaxLevel()
    return self.ctrl_data:MaxLevel()
end

function FairyDragon:IsMaxLevel()
    return self:Level() >= self:MaxLevel()
end


-- 等级上限
function FairyDragon:LimitLevel()
    if self:IsMaxLevel() then
        return self:Level()
    end
    if self:IsNeedBreak() then
        return self:Level()
    end
    local s_level = self:Level() + 1
    local max_level = self:MaxLevel()
    local level = self:Level()
    for i = s_level, max_level do
        local cfg = self.ctrl_data:GetFosterCfg(i)
        level = cfg.dragon_level
        if cfg.is_break == 1 then
            break
        end
    end
    return level
end

function FairyDragon:Exp()
    return self.base_data.exp or 0
end

function FairyDragon:CurExp()
    -- local level = self:Level()
    -- if level <= 1 then
    --     return self:Exp()
    -- end
    -- local cfg = FairyDragonData.Instance:GetFosterCfg(level - 1)
    -- return self:Exp() - cfg.need_exp
    return self:Exp()
end

function FairyDragon:NextExp()
    local level = self:Level()
    if level < 1 then
        return 0
    end
    -- local cur_cfg = FairyDragonData.Instance:GetFosterCfg(level)
    -- local last_cfg = FairyDragonData.Instance:GetFosterCfg(level - 1)
    -- if TableIsEmpty(last_cfg) then
    --     return cur_cfg.need_exp
    -- end
    -- return cur_cfg.need_exp - last_cfg.need_exp

    local cur_cfg = FairyDragonData.Instance:GetFosterCfg(level)
    return cur_cfg.need_exp
end

-- 阶
function FairyDragon:Stage()
    local level = self:Level()
    if level < 1 then
        return 0
    end
    local cfg = FairyDragonData.Instance:GetFosterCfg(level)
    return cfg.stage
end

-- 升星等级
function FairyDragon:Star()
    return self.base_data.star_level or 0
end

-- 升星节点数量
function FairyDragon:StarNum()
    if TableIsEmpty(self.base_data) then
        return 0
    end
    local num = 0
    for i = 1, FairyDragon.MaxStarNodeCount do
        if self.base_data.star_node_flag[i] == 1 then
            num = num + 1
        end
    end
    return num
end

function FairyDragon:GetStarUIList()
    local star_list = {}
    local max_star = self.ctrl_data:MaxStar()
    local cur_star = self:Star()
    for i = 1, max_star do
        local data = {}
        data.active = cur_star >= i
        table.insert(star_list, data)
    end
    return star_list
end

-- 是否突破过了
function FairyDragon:IsBreaked()
    return self.base_data.is_break_flag == 1
end

-- 是否需要突破
function FairyDragon:IsNeedBreak()
    local cfg = self.ctrl_data:GetFosterCfg()
    local exp = self:Exp()
    --if cfg.is_break == 1 and not self:IsBreaked() and exp >= cfg.need_exp then
    if cfg.is_break == 1 and not self:IsBreaked() then
        return true
    end
    return false
end

function FairyDragon:GetSoulInfo(soul_type)
    if TableIsEmpty(self.base_data.soul_info_list) then
        return {}
    end
    local info = self.base_data.soul_info_list[soul_type] or {}
    return info
end

function FairyDragon:GetSoulList()
    return self.base_data.soul_info_list or {}
end

function FairyDragon:Skin()
    local skin_seq = self.base_data.use_skin_seq or -1
    if skin_seq >= 0 then
        return self:GetSkin(skin_seq)
    end
end

function FairyDragon:GetSkin(seq)
    if not self.skin_list[seq] then
        local cfg = FairyDragonData.Instance:GetSkinCfgBySeq(seq)
        local skin = FairyDragonSkin.New(cfg)
        self.skin_list[seq] = skin
    end
    return self.skin_list[seq]
end

function FairyDragon:GetSkinList()
    local list = {}
    local cfg = self.ctrl_data.cfg.dragon_skin
    for i,v in pairs(cfg) do
        local skin = self:GetSkin(v.seq)
        table.insert(list, skin)
    end
    return list
end

function FairyDragon:IsSkin()
    local skin_seq = self.base_data.use_skin_seq or -1
    return skin_seq >= 0
end

function FairyDragon:GetSkinInfo(seq)
    if TableIsEmpty(self.base_data.skin_info_list) or TableIsEmpty(self.base_data.skin_info_list[seq]) then
        return {
            is_unlock = 0,
            star_level = 0,
        }
    end 
    return self.base_data.skin_info_list[seq]
end

function FairyDragon:Skill()
    local skill_id = Config.god_dragon_auto.other[1].initial_skill
    local skill = FairyDragonSkill.New({skill_id = skill_id, show_type = 1})
    return skill
end

--星级技能
function FairyDragon:StarSkillList() 
    local skill_list = {}
    local max_star = self:MaxLevel()
    local star = self:Star() >= max_star and max_star - 1 or self:Star()
    local star_num = self:StarNum()
    local cfg = FairyDragonData.Instance:GetStarSkillCfg(star, star_num)
    if cfg then
        for i = 1, 3 do
            local skill_id = cfg["skill_" .. i ] or 0
            if skill_id > 0 then
                local skill = FairyDragonSkill.New({skill_id = skill_id, show_type = 2})
                table.insert(skill_list, skill)
            end
        end
    end

    if table.nums(skill_list) >= 3 then
        return skill_list
    end

    if table.nums(skill_list) < 3 then
        for t_star = star, max_star do
            for i = star_num + 1, FairyDragon.MaxStarNodeCount do
                local cfg = FairyDragonData.Instance:GetStarSkillCfg(t_star, i)
                if cfg then
                    for n = #skill_list + 1, 3 do
                        if not skill_list[n] then
                            local skill_id = cfg["skill_" .. n]
                            if skill_id and skill_id > 0 then
                                local skill = FairyDragonSkill.New({skill_id = skill_id, show_type = 2, is_gray = true, lock_tip = string.format(Language.FairyDragon.Text25,cfg.dragon_star,cfg.point)})
                                skill_list[n] = skill
                            end
                        end
                    end
                    if table.nums(skill_list) >= 3 then
                        return skill_list
                    end
                end
            end
        end
    end

    return skill_list


    -- for g_id = star, 0, -1 do
    --     local group_cfg = self.ctrl_data:GetStarGroupCfg(g_id)
    --     if not TableIsEmpty(group_cfg) then
    --         for i = #group_cfg, 1, -1 do
    --             local cfg = group_cfg[i]
    --             local node_state = self.ctrl_data:GetStarClicpState(cfg)
    --             if node_state == 1 then
    --                 for n = 1, 3 do
    --                     local skill_id = cfg["skill_" .. n]
    --                     if skill_id and skill_id > 0 then
    --                         local skill = FairyDragonSkill.New({skill_id = skill_id, show_type = 2})
    --                         table.insert(skill_list, skill)
    --                     end
    --                 end
    --                 if table.nums(skill_list) >= 3 then
    --                     return skill_list
    --                 end
    --             end
    --         end
    --     end
    -- end

    -- if table.nums(skill_list) < 3 then
    --     local group_cfg = self.ctrl_data:GetStarGroupCfg(0)
    --     for i,cfg in pairs(group_cfg) do
    --         if table.nums(skill_list) >= 3 then
    --             break
    --         end
    --         for n = #skill_list + 1, 3 do
    --             if not skill_list[n] then
    --                 local skill_id = cfg["skill_" .. n]
    --                 if skill_id and skill_id > 0 then
    --                     local skill = FairyDragonSkill.New({skill_id = skill_id, show_type = 2, is_gray = true, lock_tip = string.format("需激活%s星%s节点解锁技能",cfg.dragon_star,cfg.point)})
    --                     skill_list[n] = skill
    --                 end
    --             end
    --         end
    --     end
    -- end
    -- return skill_list
end

function FairyDragon:SkillList()
    local skill = self:Skill()
    local list = {skill}
    local skin = self:Skin()
    if skin then
        local skin_skill_list = skin:SkillList() or {}
        for i,v in pairs(skin_skill_list) do
            table.insert(list, v)
        end
    end
    return list
end

function FairyDragon:SetShowSkillList(list)
    self.show_skill_list:Set(list)
end

function FairyDragon:GetShowSkillList()
    return self.show_skill_list:Val()
end

function FairyDragon:AttriList()
    local level = self:Level()
    if level == 0 then
        return {}
    end
    local attri_list = {}
    local cfg = self.ctrl_data:GetFosterCfg(level) or {}
    for i = 1, 4 do
        local attri = {}
        attri.attri_type = cfg["attri_" .. i]
        if attri.attri_type then
            attri.attri_value = cfg["attri_num_" .. i] or 0
            table.insert(attri_list, attri)
        end
    end
    return attri_list
end



-- 神龙皮肤
FairyDragonSkin = FairyDragonSkin or BaseClass()
-- vo == Config.god_dragon_auto.dragon_skin
function FairyDragonSkin:__init(vo)
    self.vo = vo
    self.item_id = vo.skin_item_id
    self.res_id = vo.res_is
    self.attri_group = vo.attri_group
    self.seq = vo.seq
    self.name = vo.skin_name
end

function FairyDragonSkin:IsActive()
    local info = FairyDragon.Instance:GetSkinInfo(self.seq) or {}
    return info.is_unlock == 1
end

-- 是否使用中
function FairyDragonSkin:IsInUse()
    if not self:IsActive() then
        return false
    end
    return self.seq == FairyDragon.Instance.base_data.use_skin_seq
end

function FairyDragonSkin:Name()
    return self.name or ""
end

function FairyDragonSkin:Score()
    local attri_list = self:AttriList()
    local score =  FairyDragonData.Instance:GetScoreByAttriList(attri_list)
    return score
end

function FairyDragonSkin:ResId()
    return self.res_id
end

function FairyDragonSkin:StarLevel()
    local info = FairyDragon.Instance:GetSkinInfo(self.seq) or {}
    return info.star_level
end

function FairyDragonSkin:MaxStar()
    local group_id = self.attri_group
    local group_cfg = FairyDragonData.Instance.skin_star_group_cfg[group_id]
    return group_cfg[#group_cfg].star_level
end

function FairyDragonSkin:IsMaxStar()
    local level = self:StarLevel()
    local max_level = self:MaxStar()
    return level >= max_level
end

function FairyDragonSkin:AttriList()
    local group_id = self.attri_group
    local level = self:StarLevel()
    local attri_list = FairyDragonData.Instance:GetSkinStarAttri(group_id, level)
    return attri_list
end

function FairyDragonSkin:UpStarCfg()
    local group_id = self.attri_group
    local level = self:StarLevel()
    local cfg = FairyDragonData:GetSkinUpStarCfg(group_id, level)
    return cfg
end

function FairyDragonSkin:SkillList()
    local list = {}
    for i = 1, 3 do
        local skill_id = self.vo["skill_" .. i]
        if skill_id and skill_id > 0 then
            local skill = FairyDragonSkill.New({skill_id = skill_id, show_type = 1})
            table.insert(list, skill)
        end
    end
    return list
end

function FairyDragonSkin:AllSkillDesc()
    local skill_list = self:SkillList()
    local desc = ""
    for i,v in pairs(skill_list) do
        local name = v:Name()
        desc = desc .. ColorStr(name .. ": ", COLORSTR.Yellow6)
        desc = desc .. v:Desc()
        if i ~= #skill_list then
            desc = desc .. "\n"
        end
    end
    return desc
end

function FairyDragonSkin:GetStarUIList()
    local star_list = {}
    local max_star = self:MaxStar()
    local cur_star = self:StarLevel()
    for i = 1, max_star do
        local data = {}
        data.active = cur_star >= i
        table.insert(star_list, data)
    end
    return star_list
end



-- 龙魂
FairyDragonSoul = FairyDragonSoul or BaseClass(Item)
function FairyDragonSoul.DecodeParam()
    local p = {}
    p.has_net_value = MsgAdapter.ReadShort()    -- 是否已随机净值
    p.level = MsgAdapter.ReadShort()
    p.exp = MsgAdapter.ReadLL()
    MsgAdapter.ReadShort()
    
    p.attri_count = MsgAdapter.ReadShort()      -- 当前已有的随机属性词条数量
    p.attri_list = {}
    for i = 1, 12 do
        local attri = {}
        attri.index = MsgAdapter.ReadShort()        -- 词条下标
        attri.attri_type = MsgAdapter.ReadShort()
        attri.attri_value = MsgAdapter.ReadShort()
        attri.level = MsgAdapter.ReadShort()        -- 词条等级
        if attri.attri_value > 0 then
            table.insert(p.attri_list, attri)
        end
    end
    return p
end

function FairyDragonSoul:__init(vo)
    self.vo = vo
    self.index = vo.index or -1             -- 背包的index
    self.item_id = vo.item_id or 0
    self.is_bind = vo.is_bind or false
    self.num = vo.num or 1
    self.param = vo.param or {}
    self.is_grey = vo.is_grey
    self.level = self.param.level or 1
    self.exp = self.param.exp or 0
end

-- 是否使用中
function FairyDragonSoul:IsInUse()
    if self.index == -1 then
        return false
    end
    local soul_list = FairyDragon.Instance:GetSoulList()
    for i,v in pairs(soul_list) do
        if v.index == self.index then
            return true
        end
    end
    return false
end

function FairyDragonSoul:SoulType()
    local item_cfg = Item.GetConfig(self.item_id)
    return item_cfg.dragon_soul_type
end

function FairyDragonSoul:Level()
    return self.level or 1
end

function FairyDragonSoul:LevelDesc()
    return string.format(Language.FairyDragon.Text1, self:Level())
end

function FairyDragonSoul:MaxLevel()
    local prof = RoleData.Instance:ProfType()
    local g_id = self:GetGroupId()
    local group_cfg = FairyDragonSoulData.Instance.base_group_attri_cfg[g_id][prof]
    return group_cfg[#group_cfg].soul_level
end

function FairyDragonSoul:IsFullLevel()
    return self:Level() >= self:MaxLevel()
end

-- 这个是累计的经验
function FairyDragonSoul:Exp()
    return self.exp or 0
end

-- 当前等级经验进度
function FairyDragonSoul:CurExp()
    -- local level = self:Level()
    -- if level < 1 then
    --     return self:Exp()
    -- end
    -- local g_id = self:GetGroupId()
    -- local last_cfg = FairyDragonSoulData:GetBaseAttriCfg(g_id, level - 1)
    -- local exp = self:Exp() - last_cfg.exe_up
    -- return exp
    
    return self.exp or 0
end

-- 距离升满还差多少经验  >>废弃
function FairyDragonSoul:FullExp()
    local g_id = self:GetGroupId()
    local exp = 0
    local level = self:Level()
    for i = level, self:MaxLevel() - 1 do
        local cfg = FairyDragonSoulData.Instance:GetBaseAttriCfg(g_id, i)
        exp = exp + cfg.exe_up
    end
    exp = exp - self.exp
    return exp
end

-- 距离下级升满还差多少经验 >>废弃
function FairyDragonSoul:NextExp()
    local attri_cfg = self:AttriCfg()
    return attri_cfg.exe_up
end

function FairyDragonSoul:BaseCfg()
    local cfg = FairyDragonSoulData.Instance:GetBaseCfg(self.item_id)
    return cfg
end

function FairyDragonSoul:GetGroupId()
    local base_cfg = self:BaseCfg()
    local g_id = base_cfg.primary_group
    return g_id
end

function FairyDragonSoul:Score()
    if nil == self.score then
        local attri_list = self:BaseAttriList()
        local rand_attri_list = self:RandAttriList()
        self.score =  FairyDragonData.Instance:GetScoreByAttriList(attri_list)
        self.score = self.score + FairyDragonData.Instance:GetScoreByAttriList(attri_list)
	end
	return self.score
end

-- 基础属性
function FairyDragonSoul:BaseAttriList()
    local g_id = self:GetGroupId()
    local level = self:Level()
    local attri_list = FairyDragonSoulData.Instance:GetBaseAttri(g_id, level)
    return attri_list
end

function FairyDragonSoul:AttriCfg()
    local g_id = self:GetGroupId()
    level = self:Level()
    local cfg = FairyDragonSoulData:GetBaseAttriCfg(g_id, level)
    return cfg
end

function FairyDragonSoul:BaseAndNextAttriList(next_level)
    local g_id = self:GetGroupId()
    local level = self:Level()
    next_level = next_level or level
    local attri_list = FairyDragonSoulData.Instance:GetBaseAttriAndNext(g_id, level, next_level)
    return attri_list
end

function FairyDragonSoul:AllAttriList()
    local base_list = TableCopy(self:BaseAttriList())
    local rand_list = TableCopy(self:RandAttriList())
    local list = AndList(base_list, rand_list)
    return list
end

-- 随机属性
function FairyDragonSoul:RandAttriList()
    return self.param.attri_list or {}
end

function FairyDragonSoul:RandAttriPreviewList()
    local attri_list = self:RandAttriList()
    if TableIsEmpty(attri_list) then
        return {}
    end
    local attri = attri_list[1]
    local attri_type = attri.attri_type
    local group_id = self:RandAttriGroupId()
    local list = {}
    local cfg = Config.god_dragon_auto.random_group
    for i,v in pairs(cfg) do
        if group_id == v.attri_group and v.attri_type == attri_type and v.attri_level == 1 then
            if not list[v.attri_color] then
                local t = {}
                list[v.attri_color] = t
                t.qua = v.attri_color
                t.attri_type = v.attri_type
                t.attri_value = v.attri_num
                local next_cfg = cfg[i + 1]
                t.next_attri_value = next_cfg.attri_num
            end
        end
    end
    
    local new_list = {}
    for i,v in pairs(list) do
        table.insert(new_list, v)
    end
    
    table.sort(new_list, function (a,b)
        return a.qua < b.qua
    end)
    return new_list
end

-- 随机属性组
function FairyDragonSoul:RandAttriGroupId()
    local base_cfg = self:BaseCfg()
    return base_cfg.random_group
end

function FairyDragonSoul:MaxRandCount()
    local base_cfg = self:BaseCfg()
    return base_cfg.random_num or 0
end

-- 下一个随机新词条或升级随机属性在哪个配置
function FairyDragonSoul:GetNextSpInCfg()
    local level = self:Level()
    local max_level = self:MaxLevel()
    local g_id = self:GetGroupId()
    for i = level + 1, max_level do
        local cfg = FairyDragonSoulData:GetBaseAttriCfg(g_id, i)
        if cfg.is_random == 1 then
            return cfg
        end
    end
end

-- 当前技能id
function FairyDragonSoul:GetSpId(level)
    local g_id = self:GetGroupId()
    level = level or self:Level()
    local max_level = self:MaxLevel()
    if level > max_level then
        level = max_level
    end
    local cfg = FairyDragonSoulData:GetBaseAttriCfg(g_id, level)
    return cfg.sp_id
end

-- 当前技能
function FairyDragonSoul:Skill()
    local skill_id = self:GetSpId()
    local skill = FairyDragonSkill.New({skill_id = skill_id, show_type = 1})
    return skill
end

-- 下一个特效在哪个配置
function FairyDragonSoul:GetNextSpInCfg()
    local sp_id = self:GetSpId()
    local level = self:Level()
    local max_level = self:MaxLevel()
    local g_id = self:GetGroupId()
    for i = level + 1, max_level do
        local cfg = FairyDragonSoulData.Instance:GetBaseAttriCfg(g_id, i)
        if cfg.sp_id ~= sp_id then
            return cfg
        end
    end
end

-- 特效触发概率 返回百分比，配置千分比
function FairyDragonSoul:SpTriggerRate()
    local cfg = self:BaseCfg()
    return cfg.sp_rate / 10
end

-- 可以分解多少个碎片
function FairyDragonSoul:ResolveClipCount()
    local cfg = self:Config()
    return cfg.dragon_soul_chips or 0
end

-- 所有的特效列表
function FairyDragonSoul:AllEffectList()
    local sp_id = nil
    local level = self:Level()
    local max_level = self:MaxLevel()
    local g_id = self:GetGroupId()
    local list = {}
    for i = 1, max_level do
        local cfg = FairyDragonSoulData.Instance:GetBaseAttriCfg(g_id, i)
        if cfg.sp_id ~= sp_id then
            local skill_cfg = Cfg.FairyDragonSkillCfg(cfg.sp_id) or {}
            local desc = skill_cfg.skill_dec or ""
            table.insert(list, {soul_level = i, sp_id = cfg.sp_id, desc = desc})
            sp_id = cfg.sp_id
        end
    end
    return list
end

-- 分解可以获得多少个龙魂材料，等同于升级过程中消耗了多少个龙魂
function FairyDragonSoul:ResolveSoulCount()
    local level = self:Level()
    if level == 1 then
        return 0
    end
    local num = 0
    local g_id = self:GetGroupId()
    for i = 1, level - 1 do
        local cfg = FairyDragonSoulData.Instance:GetBaseAttriCfg(g_id, i)
        num = num + cfg.cost_itself
    end
    -- 策划说需要+1
    return num + 1
end



-- =================== 神龙技能 ==========
FairyDragonSkill = FairyDragonSkill or BaseClass()
function FairyDragonSkill:__init(vo)
    self.vo = vo
    self.skill_id = vo.skill_id
    self.show_type = vo.show_type or 1      -- 1 展示FairyDragonSkillTip1  2：展示：FairyDragonSkillTip2
    self.is_gray = vo.is_gray or false
    self.lock_tip = vo.lock_tip
end

function FairyDragonSkill:Cfg()
    local cfg = Cfg.FairyDragonSkillCfg(self.skill_id)
    return cfg
end

function FairyDragonSkill:Name()
    return self:Cfg().skill_name
end

function FairyDragonSkill:Desc()
    return self:Cfg().skill_dec
end

function FairyDragonSkill:Level()
    return self:Cfg().skill_level
end

function FairyDragonSkill:Icon()
    return self:Cfg().skill_icon
end

function FairyDragonSkill:GroupId()
    return self:Cfg().group
end

-- 下一级技能
function FairyDragonSkill:NextSkill()
    if self.show_type == 1 then
        return
    end
    local level = self:Level()
    local group_id = self:GroupId()
    local cfg = Cfg.FairyDragonSkillCfg2(group_id, level + 1, false)
    if cfg then
        local skill = FairyDragonSkill.New({skill_id = cfg.skill_id, show_type = self.show_type})
        -- 解锁配置
        local unlock_cfg = FairyDragonData.Instance:GetStarAttriCfgBySkillId(cfg.skill_id)
        return skill,unlock_cfg
    end
end