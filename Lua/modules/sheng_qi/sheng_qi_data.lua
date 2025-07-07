ShengQiData = ShengQiData or BaseClass()

function ShengQiData:__init()
    if ShengQiData.Instance ~= nil then
        Debuger.LogError("[ShengQiData] attempt to create singleton twice!")
        return
    end
    ShengQiData.Instance = self
    self:InitCfg()

    self.sheng_qi_list = SmartData.New({})        -- 全部的圣器

    self.sel_sheng_qi = SmartData.New({})         -- 当前选择的圣器

    self.sel_book_data = SmartData.New({})        -- 图鉴选择的圣器

    self.study_skill_data = SmartData.New({sheng_qi = nil, skill_index = 0})     -- 领悟技能

    self.sel_stuff_list = SmartData.New({})       -- 选中的升级圣器的材料列表     -- 切换圣器和关闭界面的时候清空

    self.sel_tab_data = SmartData.New()         -- 选中的标签页

    self.remind_data = SmartData.New({v = false})

    self.follow_data = SmartData.New({index = -1})

    self.target_book = nil      -- 图鉴的目标，用于打开的时候选中哪一个
end

function ShengQiData:ClearData()
    
end

function ShengQiData:InitCfg()
    self.item_cfg = Config.shengqi_item_auto
    self.cfg = Config.shengqi_auto

    Cfg.Register("ShengQiCfg",Cfg.RealTimeSearch(self.cfg.shengqi_type,"item_id"))
    Cfg.Register("ShengQiCfgById",Cfg.RealTimeSearch(self.cfg.shengqi_type,"shengqi_id"))
    -- 圣器升级配置
    Cfg.Register("ShengQiUpCfg",Cfg.RealTimeSearch2Key(self.cfg.shengqi_upgrade, "shengqi_quality", "level"))
    -- 圣器天赋技能
    Cfg.Register("ShengQiTianFuSkill",Cfg.RealTimeSearch(self.cfg.tianfu_skill,"skill_id"))
    -- 圣器主动技能
    Cfg.Register("ShengQiZhuDongSkill",Cfg.RealTimeSearch(self.cfg.zhudong_skill,"skill_id"))
    -- 圣器擅长技能
    Cfg.Register("ShengQiShanChangSkill",Cfg.RealTimeSearch(self.cfg.zhudong_skill,"shanchang_id"))
    -- 技能书配置
    Cfg.Register("ShengQiSkillBook",Cfg.RealTimeSearch(self.cfg.skill_obtain,"skill_id"))

    self.skill_cfg = Config.hallow_skill_auto
    -- 战斗技能配置
    Cfg.Register("ShengQiSkillCfg",Cfg.RealTimeSearch2Key(self.skill_cfg.skill_cfg, "skill_id", "skill_level"))

    -- 经验丹配置
    self.exp_stuff_cfg = KeyList(self.cfg.exp_mat,"item_id")
end

-- ========================  基础 ==============================
function ShengQiData:IsOpen()
    local role_level = RoleData.Instance:GetRoleLevel()
    local t_level = self.cfg.other[1].open_level
    if role_level < t_level then
        return false
    end
 
    if not self:IsFly() then
        return false
    end
    return true
end

function ShengQiData:IsFly()
    local t_fly_level = self.cfg.other[1].feisheng_type
    local fly_level = FlyUpData.Instance:GetStage()
    if fly_level < t_fly_level then
        return false
    end
    return true
end

function ShengQiData:NeedFlyLevel()
    return self.cfg.other[1].feisheng_type
end

-- 跟随的圣器，为空则没有
function ShengQiData:GetFollowShengQi()
    if not self.follow_data.index or self.follow_data.index < 0 then
        return
    end
    return self:GetShengQiByIndex(self.follow_data.index)
end


-- 获取圣器配置
function ShengQiData:GetShengQiCfg(item_id)
    local cfg = Cfg.ShengQiCfg(item_id)
    if TableIsEmpty(cfg) then
        LogDG("【获取圣器配置】异常 id == ", item_id)
    end
    return cfg
end

function ShengQiData:GetCfgById(id)
    local cfg = Cfg.ShengQiCfgById(id)
    if TableIsEmpty(cfg) then
        LogDG("【获取圣器配置】异常 id == ", item_id)
    end
    return cfg
end

-- 当前选中的圣器
function ShengQiData:GetSelShengQi()
    return self.sel_sheng_qi
end
function ShengQiData:SetSelShengQi(sheng_qi)
    self:ClearSelStuff()
    self.sel_sheng_qi:Set(sheng_qi or {})
end

-- 当前选中的标签页
function ShengQiData:GetSelTab()
    return self.sel_tab_data
end
function ShengQiData:SetSelTab(tab_data)
    self.sel_tab_data:Set(tab_data or {})
end

-- 当前出战的圣器
function ShengQiData:GetFightShengQi()
    for i,v in pairs(self.sheng_qi_list) do
        if v:State() == ShengQiCfg.State.Fight then
            return v
        end
    end
end

-- 获取背包的圣器
function ShengQiData:GetShengQiByIndex(bag_index)
    for i,v in pairs(self.sheng_qi_list) do
        if v.index == bag_index then
            return v
        end
    end
end

function ShengQiData:ItemChange(protocol)
    local bag_type = protocol.bag_type
    local reason = protocol.reason
    local item = protocol.item
    ShengQiCtrl.Log("ShengQiData:ItemChange:",reason,item.item_id)
    if bag_type == ShengQiCfg.BagType.ShengQi then
        self:ShengQiChange(item)
    elseif bag_type == ShengQiCfg.BagType.FuShi then
        FuShiData.Instance:FuShiChange(item)
    end

    if item.item_id == 0 then
        return
    end
    if SHENGQI_NOT_NOTICE_REASON[reason] then
        PublicPopupCtrl.Instance:CenterI({item_id = item.item_id, num = item.num,
            color_str = item:ColorStr(),item = item,icon_id = item:IconId()})
        ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item, num = num}}
    end

    if reason == ShengQiCfg.SendReason.FuShiReset then
        if item:Type() == ItemType.FuShi then
            FuShiData.Instance:SetResetFushi(item)
        end
        if item:Type() == ItemType.ShengQi then
            local fushi = FuShiData.Instance:GetResetFushi()
            for i,v in pairs(item:GetAllFushi()) do
                if v.item_id == fushi.item_id then
                    FuShiData.Instance:SetResetFushi(v)
                    break
                end
            end
        end
    elseif reason == ShengQiCfg.SendReason.FuShiFusion then
        ViewMgr:OpenView(FuShiGetShow, {item = item})
    elseif reason == ShengQiCfg.SendReason.SkillUpdate then
        local update_skill = item:GetStudyInitativeSkill()
        if TableIsEmpty(update_skill) then
            ViewMgr:CloseView(ShengQiSkillUpdateResult)
        end
        --self:CheckSkillUpdate(item)
    end
end


-- 插入/变化单个圣器
function ShengQiData:ShengQiChange(sheng_qi)
    ShengQiCtrl.Log("圣器变化",sheng_qi.item_id,sheng_qi.state,sheng_qi)
    local sel_sheng_qi = self:GetSelShengQi()
    if not TableIsEmpty(sel_sheng_qi) and sel_sheng_qi.index == sheng_qi.index then
        if sheng_qi.item_id == 0 or sheng_qi:State() == ShengQiCfg.State.Rest then
            self:SetSelShengQi({})
        else
            self:SetSelShengQi(sheng_qi)
        end
    end
    if TableIsEmpty(sel_sheng_qi) and sheng_qi:State() == ShengQiCfg.State.Fight then
        self:SetSelShengQi(sheng_qi)
    end

    local index = sheng_qi.index
    local old_sheng_qi = self.sheng_qi_list[index]
    if old_sheng_qi and sheng_qi.item_id ~= 0 and sheng_qi:State() ~= ShengQiCfg.State.Rest then
        -- 评分变化
        local old_score = old_sheng_qi:Score()
        local new_score = sheng_qi:Score()
        if old_score ~= new_score then
            self:SetScoreChange(old_score,new_score)
            ViewMgr:OpenView(ShengQiScore)

            -- 属性变化
            local change_list = {}
            for i = 0, SHENG_QI_ATTRI_TYPE.MAX do
                local info = {}
                info.from_type = AttrChangeData.ChangeFromType.ShengQi
                info.old_val = old_sheng_qi:GetAttriInfo(i).attri_value or 0
                info.change_val = sheng_qi:GetAttriInfo(i).attri_value or 0
                info.change_val = info.change_val - info.old_val
                if info.change_val ~= 0 then
                    change_list[i] = info
                end
            end

            local title_info = {fromType = AttrChangeData.ChangeFromType.ShengQi, showName = sheng_qi:Name()}
            MainOtherCtrl.Instance:OpenAttrChange(change_list, title_info)
        end
    end

    if sheng_qi.item_id == 0 then
        self.sheng_qi_list[index] = nil
    else
        self.sheng_qi_list[index] = sheng_qi
    end

end

-- 移除圣器
function ShengQiData:RemoveShengQi(bag_index)
    local t_index
    for i,v in pairs(self.sheng_qi_list) do
        if v.index == bag_index then
            t_index = i
            break
        end
    end
    if t_index then
        self.sheng_qi_list[t_index] = nil
    end
end

-- 背包排序整理 t_qua  品质排序
function ShengQiData:BagSort(sheng_qi_list, t_qua)
    sheng_qi_list = sheng_qi_list or table.rearrange(self.sheng_qi_list)
    table.sort(sheng_qi_list, function (a, b)
        if a:State() == b:State() then
            if a:Qua() == b:Qua() then
                return a:Score() > b:Score()
            end
            if a:Qua() == t_qua then
                return true
            end
            if b:Qua() == t_qua then
                return false
            end
            return a:Qua() > b:Qua() 
        end
        return a:State() > b:State()
    end)
    return sheng_qi_list
end

-- 背包排序整理 t_qua  评分
function ShengQiData:BagSortScore(sheng_qi_list)
    sheng_qi_list = sheng_qi_list or table.rearrange(self.sheng_qi_list)
    table.sort(sheng_qi_list, function (a, b)
        if a:State() == b:State() then
            if a:Score() == b:Score() then
                return a:Qua() > b:Qua() 
            end
            return a:Score() > b:Score()
        end
        return a:State() > b:State()
    end)
    return sheng_qi_list
end

-- 获取左侧显示的圣器（出战以及待机的 最多 5 个）
function ShengQiData:GetShowList()
    local max_count = 5
    local show_list = {}
    for i,v in pairs(self.sheng_qi_list) do
        local state = v:State()
        if state == ShengQiCfg.State.Ready or state == ShengQiCfg.State.Fight then
            if #show_list < max_count then
                table.insert(show_list,v)
            else
                LogDG("圣器异常 出战以及待机的数量大于5")
            end
        end
    end
    self:BagSort(show_list)
    return show_list
end

-- 获取背包显示的圣器 （只显示休息的）-- 筛选的品质和类型
function ShengQiData:GetBagShowList(t_type,t_qua)
    local show_list = {}
    for i,v in pairs(self.sheng_qi_list) do
        local sheng_qi_type = v:ShengQiType()
        if not t_type or t_type == SHENG_QI_TYPE.COMMON or t_type == sheng_qi_type  then
            if v:State() == ShengQiCfg.State.Rest then
                table.insert(show_list, v)
            end
        end
    end

    self:BagSort(show_list,t_qua)
    return show_list
end


function ShengQiData:SetScoreChange(old_score, new_score)
    self.old_score = old_score
    self.new_score = new_score
end
function ShengQiData:GetScoreChange()
    return self.new_score or 0, self.old_score or 0
end

-- 属性评分
function ShengQiData:GetAttriScore(AttriType)
    local cfg = Config.score_cfg_auto.shengqi_score_count[1]
    local filed = SHENG_QI_ATTRI_SCORE_NAME[AttriType]
    if filed then
        return cfg[filed] or 0
    end
    return 0
end

-- =============================================================





-- ========================  属性 ==============================

-- 是否千分比
function ShengQiData:AttriTypeIsRate(attri_type)
    local t = {
        [SHENG_QI_ATTRI_TYPE.ATTACK] = true,
        [SHENG_QI_ATTRI_TYPE.REPLY] = true,
        [SHENG_QI_ATTRI_TYPE.HELP] = true,
        [SHENG_QI_ATTRI_TYPE.FALL] = true,
        [SHENG_QI_ATTRI_TYPE.COPE_FALL] = true,
    }
    return t[attri_type] or false
end

function ShengQiData:FormatAttriInfo(attri_type, attri_value)
    local t = {}
    attri_type = attri_type or -1
    attri_value = attri_value or 0
    t.is_nothing = false
    t.name = Language.ShengQi.AttriName[attri_type] or ""
    t.name2 = t.name .. ":"
    t.attri_type = attri_type
    t.value = attri_value or 0
    if self:AttriTypeIsRate(attri_type) then
        t.value_str = t.value / 10 .. "%"
    else
        t.value_str = t.value .. ""
    end
    t.tip = ""
    t.null_tip = ""
    return t
end

-- 获取评分等级
function ShengQiData:GetScoreLevel(score)
    local role_level = RoleData.Instance:GetRoleLevel()
    local cfg = Config.score_cfg_auto.shengqi_score[role_level]
    if TableIsEmpty(cfg) then
        return 0
    end
    if score >= cfg.score_s then
        return 3
    elseif score >= cfg.score_b then
        return 2
    elseif score >= cfg.score_a then
        return 1
    else
        return 0
    end
end

-- 属性描述配置
function ShengQiData:GetAttriDescCfg(attri_type)
    for i,v in pairs(self.cfg.att_dec) do
        if v.att_id == attri_type then
            return v
        end
    end
end

-- =============================================================





-- ========================  升级 ==============================

function ShengQiData:AllStuffItemIdList()
    local list = {}
    for i,v in pairs(self.cfg.exp_mat) do
        table.insert(list, v.item_id)
    end
    -- 符石孔解锁
    local fushi_stuff_id = self.cfg.fushi_hole[1].unlock_cost
    table.insert(list, fushi_stuff_id)
    return list
end

-- 经验丹加多少经验
function ShengQiData:GetStuffExp(item_id, num)
    num = num or 1
    if self.exp_stuff_cfg[item_id] then
        return self.exp_stuff_cfg[item_id].item_exp * num
    end
    return 0
end

-- 全部经验丹
function ShengQiData:GetAllExpItem()
    local item_list = BagData.Instance:GetColumnList(ItemColumnType.Material)
    local stuff_list = {}
    for i,v in pairs(item_list) do
        if self:GetStuffExp(v.item_id) > 0 then
            table.insert(stuff_list, v)
        end
    end

    item_list = BagData.Instance:GetColumnList(ItemColumnType.Item)
    for i,v in pairs(item_list) do
        if self:GetStuffExp(v.item_id) > 0 then
            table.insert(stuff_list, v)
        end
    end
    return stuff_list
end

-- 获取全部材料
function ShengQiData:GetAllStuff(t_qua)
    local stuff_list = self:GetAllExpItem()
    table.sort(stuff_list, function (a,b)
        return a:Color() > b:Color()
    end)
    local sheng_qi_list = self:GetBagShowList(SHENG_QI_TYPE.COMMON,t_qua)
    stuff_list = AndList(stuff_list,sheng_qi_list)
    return stuff_list
end

-- 添加材料 减去就 -1   结果 1 添加成功，0经验满了，-1，圣器数量上限, -2 ，-3 异常,-4数量不足
function ShengQiData:AddSelStuff(item, num, sheng_qi)
    sheng_qi = sheng_qi or self:GetSelShengQi()
    if TableIsEmpty(sheng_qi) then
        return -2
    end

    if sheng_qi:Level() < 1 then
        return -2
    end

    if item.item_id < 1 then
        return -3
    end

    if num > 0 and item:Type() == ItemType.ShengQi then
        if self:GetSelStuffShengQiCount() >= ShengQiCfg.MAX_SHENG_QI_STUFF_COUNT then
            return -1
        end
    end

    local cur_stuff_exp = self:GetSelStuffExp()
    local max_need_exp = sheng_qi:FullExp()
    if num > 0 and cur_stuff_exp >= max_need_exp then
        return 0
    end

    local cur_num = self.sel_stuff_list[item] or 0
    num = num or 0
    cur_num = cur_num + num
    if cur_num > item:Num() then
        return -4
    end
    if cur_num <= 0 then
        self.sel_stuff_list[item] = nil
    else
        self.sel_stuff_list[item] = cur_num
    end
    return 1
end

function ShengQiData:GetSelStuff(item)
    return self.sel_stuff_list[item] or 0
end

function ShengQiData:GetAllSelStuff()
    return self.sel_stuff_list
end


-- 清空
function ShengQiData:ClearSelStuff()
    self.sel_stuff_list:Set({})
end

-- 选中的材料值多少经验
function ShengQiData:GetSelStuffExp()
    local exp = 0
    for item,num in pairs(self.sel_stuff_list) do
        if item:Type() == ItemType.ShengQi then
            exp = exp + item:StuffExp()
        else
            exp = exp + self:GetStuffExp(item.item_id, num)
        end
    end
    return exp
end

-- 选中的材料有多少圣器
function ShengQiData:GetSelStuffShengQiCount()
    local count = 0
    for i,num in pairs(self.sel_stuff_list) do
        if i:Type() == ItemType.ShengQi then
            count = count + 1
        end
    end
    return count
end

-- 升级预览，放入x经验，能升至多少级,需要多少铜币
function ShengQiData:PreviewUpLevel(sheng_qi, add_exp)
    sheng_qi = sheng_qi or self:GetSelShengQi()
    add_exp = add_exp or 0
    if TableIsEmpty(sheng_qi) then
        return 1,0
    end
    local exp = sheng_qi:Exp() + add_exp
    local level = sheng_qi:Level()
    if level < 1 then
        BuglyReport("圣器净值异常，等级 == ", level, " item_id == ", sheng_qi.item_id)
        return 1,0
    end
    local qua = sheng_qi:Qua()
    local max_level = sheng_qi:MaxLevel()
    local need_coin = 0
    for i = level, max_level do
        local cfg = Cfg.ShengQiUpCfg(qua, i)
        if exp < cfg.upgrade_exp then
            if i == 1 then
                LogDG("!!!!!圣器经验表配错了，第一级经验应该为0")
            end
            return i - 1,need_coin
        end
        if i > level then
            need_coin = need_coin + cfg.coin_cost
        end
    end
    return max_level,need_coin
end

-- 材料池里面有多少种材料和多少个圣器
function ShengQiData:GetSelStuffTypeInfo(list)
    list = list or self.sel_stuff_list
    local stuff_type_num = 0
    local sheng_qi_num = 0
    for i,v in pairs(list) do
        if i:Type() == ItemType.ShengQi then
            sheng_qi_num = sheng_qi_num + 1
        else
            stuff_type_num = stuff_type_num + 1
        end
    end
    return stuff_type_num,sheng_qi_num
end


-- 宠物是否被圣佑
function ShengQiData:GetBlessShengQi(pet)
    local list = self:GetShowList()
    local pet_info = pet.info
    for i,v in pairs(list) do
        local uid_t = v:PetUid()
        if pet_info.pet_unique_id1 == uid_t.id_1 and pet_info.pet_unique_id2 == uid_t.id_2 then
            return v
        end
    end
    return nil
end

function ShengQiData:GetPetList(sheng_qi)
    sheng_qi = sheng_qi or self:GetSelShengQi()
    if TableIsEmpty(sheng_qi) then
        return {}
    end
    local pet_list = PetData.Instance:GetPetList()
    local list = {}
    for i,v in pairs(pet_list) do
        local pet_sheng_qi = v:ShengQi()
        if (pet_sheng_qi and pet_sheng_qi.index == sheng_qi.index) or pet_sheng_qi == nil then
            table.insert(list, v)
        end
    end
    return list
end

-- =============================================================





-- ========================  技能 ==============================

-- 天赋技能库
function ShengQiData:GetTalentSkillPool(talent_type)
    if not self.talent_skill_pool then
        self.talent_skill_pool = {}
        for i,v in pairs(self.cfg.tianfu_skill) do
            if not self.talent_skill_pool[v.skill_type] then
                self.talent_skill_pool[v.skill_type] = {}
            end
            local skill = ShengQiSkill.New({skill_id = v.skill_id, skill_type = SHENG_QI_SKILL_TYPE.TALENT})
            table.insert(self.talent_skill_pool[v.skill_type], skill)
        end
    end
    if not talent_type then
        return self.talent_skill_pool
    end
    return self.talent_skill_pool[talent_type] or {}
end

-- 主动技能库
function ShengQiData:GetInitativeSkillPool(initative_type)
    if not self.initative_skill_pool then
        self.initative_skill_pool = {}
        for i,v in pairs(self.cfg.zhudong_skill) do
            if not self.initative_skill_pool[v.skill_type] then
                self.initative_skill_pool[v.skill_type] = {}
            end
            local skill = ShengQiSkill.New({skill_id = v.skill_id, skill_type = SHENG_QI_SKILL_TYPE.INITIATIVE, level = v.combo_level})
            table.insert(self.initative_skill_pool[v.skill_type], skill)
        end
    end
    if not initative_type then
        return self.initative_skill_pool
    end
    return self.initative_skill_pool[initative_type] or {}
end

-- 技能标签页的替换技能用
function ShengQiData:SetStudySkillData(sheng_qi,skill_index)
    self.study_skill_data.sheng_qi = sheng_qi
    self.study_skill_data.skill_index = skill_index
end

function ShengQiData:GetStudySkillData()
    return self.study_skill_data
end

-- 图鉴的技能配置
function ShengQiData:GetSkillBookCfg(skill_id)
    for i,v in pairs(self.cfg.tujian_skill) do
        if v.skill_id == skill_id then
            return v
        end
    end
end


-- 可学习技能列表 : 是主动技能，不包含天赋
function ShengQiData:GetStudySkillGroupByQua(qua, sheng_qi_type)
    local cfg = self.cfg.skill_obtain
    if qua then
        local type = qua - 3
        if not self.update_skill_group_cfg then
            self.update_skill_group_cfg = DataHelper.ConvertGroup(cfg, "skill_type")
        end
        cfg = self.update_skill_group_cfg[type] or {}
    end
    if sheng_qi_type then
        local list = {}
        for i,v in pairs(cfg) do
            if v.shengqi_type == sheng_qi_type then
                table.insert(list, v)
            end
        end
        return list
    end
    return cfg
end

-- 技能孔解锁配置
function ShengQiData:GetSkillUnlockCfg(hole)
    for i,v in pairs(self.cfg.skill_hole) do
        if hole == v.hole then
            return v
        end
    end
end


-- 检查学习的技能
function ShengQiData:CheckSkillUpdate(sheng_qi)
    local update_skill = sheng_qi:GetStudyInitativeSkill()
    if TableIsEmpty(update_skill) then
        ViewMgr:CloseView(ShengQiSkillUpdateResult)
    else
        ViewMgr:OpenView(ShengQiSkillUpdateResult, {sheng_qi = sheng_qi})
    end
end

-- 通过品质获取技能组
function ShengQiData:GetSkillGroupByQua(qua)
    local cfg = self.cfg.zhudong_skill
    if qua then
        if not self.zhudong_skill_group_cfg then
            self.zhudong_skill_group_cfg = DataHelper.ConvertGroup(cfg, "skill_quality")
        end
        cfg = self.zhudong_skill_group_cfg[qua] or {}
    end
    return cfg
end

-- =============================================================




-- ========================  符石 ==============================

-- ========== 在 fu_shi_data.lua ========= 

-- =============================================================





-- ========================  图鉴 ==============================

function ShengQiData:GetBookList(_type)
    _type = _type or SHENG_QI_TYPE.COMMON

    if not self.book_cfg then
        self.book_cfg = DataHelper.ConvertGroup(self.cfg.tujian, "shengqi_type")
    end

    local list_data = {}
    if _type == SHENG_QI_TYPE.COMMON then
        for i,v in pairs(self.book_cfg) do
            local data = {
                type = i,
                list = v,
            }
            table.insert(list_data, data)
        end
    else
        local info = self.book_cfg[_type] or {}
        local data = {
            type = _type,
            list = info,
        }
        table.insert(list_data, data)
    end
    return list_data
end

function ShengQiData:SetBookSelData(data)
    self.sel_book_data:Set(data)
end

function ShengQiData:GetBookSelData(data)
    return self.sel_book_data
end

function ShengQiData:SetTargetBook(item_id)
    self.target_book = item_id
    if item_id then
        self:SetBookSelData({})
    end
end

function ShengQiData:GetTargetBook()
    return self.target_book
end

-- =============================================================

-- 红点通知
function ShengQiData:RemindNotice()
    self.remind_data.v = not self.remind_data.v
end

-- ================= 红点逻辑
function ShengQiData:Remind()
    if self:ShengQiUseRemind() > 0 then
        return 1
    end
    if self:UpLevelTabRemind() > 0 then
        return 1
    end
    if self:FuShiTabRemind() > 0 then
        return 1
    end
    return 0
end

-- 圣器穿戴红点
function ShengQiData:ShengQiUseRemind()
    local show_list = self:GetShowList()
    if #show_list == 5 then
        return 0
    end
    local bag_list = self:GetBagShowList()
    if TableIsEmpty(bag_list) then
        return 0
    end

    return 1
end

-- 升级标签页红点
function ShengQiData:UpLevelTabRemind()
    local sheng_qi_list = self:GetShowList()
    for i,v in pairs(sheng_qi_list) do
        if self:UplevelRemind(v) > 0 then
            return 1
        end
        if self:HelpPetRemind(v) > 0 then
            return 1
        end
    end
    return 0
end

-- 单个圣器升级红点
function ShengQiData:UplevelRemind(sheng_qi)
    if sheng_qi:State() == ShengQiCfg.State.Rest then
        return 0
    end
    if sheng_qi:IsFullLevel() then
        return 0
    end
    -- 下一级需要多久经验
    local need_exp = sheng_qi:NeedExpToNextLevel()
    local all_stuff = self:GetAllExpItem()
    local exp = 0
    for i,v in pairs(all_stuff or {}) do
        exp = exp + self:GetStuffExp(v.item_id, v.num)
    end
    if exp < need_exp then
        return 0
    end
    return 1
end

-- 单个圣器圣佑宠物红点
function ShengQiData:HelpPetRemind(sheng_qi)
    if sheng_qi:State() == ShengQiCfg.State.Rest then
        return 0
    end
    local uid_t = sheng_qi:PetUid()
    if TableIsEmpty(uid_t) or uid_t.id_1 == 0 then
        return 1
    end
    return 0
end


-- 符石标签页红点
function ShengQiData:FuShiTabRemind()
    local sheng_qi_list = self:GetShowList()
    for i,v in pairs(sheng_qi_list) do
        if self:FuShiRemind(v) > 0 then
            return 1
        end
    end
    return 0
end

-- 单个圣器符石红点
function ShengQiData:FuShiRemind(sheng_qi)
    if sheng_qi:State() == ShengQiCfg.State.Rest then
        return 0
    end

    -- 检查下有无解锁的符石孔
    for i = 0, 4 do
        local flag = FuShiData.Instance:FuShiCheckLock(sheng_qi, i, true)
        if flag == 1 then
            return 1
        end
    end

    local all_fushi = FuShiData.Instance:GetBagShowList()
    for i,v in pairs(all_fushi) do
       if FuShiData.Instance:Remind(v,sheng_qi) > 0 then
            return 1
       end
    end
    return 0
end







-- ==================== 其它 ===================

-- 刷新圣器跟随
function ShengQiData:FlushFollow()
    -- if TeamData.Instance:InTeam() then
    --     MainPlayer:SetShengQi(nil)
    --     return
    -- end
    -- MainPlayer:SetShengQi(self:GetFollowShengQi())

    self:ClearFollow()
    if not TeamData.Instance:InTeam() then
        local sheng_qi = self:GetFollowShengQi()
        if not sheng_qi then
            return
        end
        MainPlayer.follow_shengqi = SceneObj.CreateObj(
		{
			obj_type = SceneObjType.ShengQi,
			res_id = sheng_qi:ResId(),
			name = sheng_qi:Name(),
        })

        local ch_queue = {}
        if MainPlayer.follow_queue_id == nil then
            table.insert(ch_queue, MainPlayer)
        end
        
        MainPlayer.follow_shengqi:Vo().move_speed = MainPlayer.vo.move_speed
        MainPlayer.follow_shengqi:SetMoveSpeed(MainPlayer.vo.move_speed)
        table.insert(ch_queue, MainPlayer.follow_shengqi)

        if MainPlayer.follow_queue_id == nil then
            MainPlayer.follow_queue_id = ChsFollow.Instance:CreateOrAddQueue(MainPlayer.follow_queue_id, ch_queue, nil, MainPlayer.follow_pet)
        else
            for i,v in ipairs(ch_queue) do
                ChsFollow.Instance:AddChToQueue(MainPlayer.follow_queue_id, v, nil, MainPlayer.follow_pet)
            end
        end
    end
end

function ShengQiData:ClearFollow()
    if MainPlayer and MainPlayer.follow_shengqi then
        if MainPlayer.follow_shengqi ~= nil then
            ChsFollow.Instance:OnChDelete(MainPlayer.follow_shengqi)
        end
        MainPlayer.follow_shengqi:DeleteMe()
        MainPlayer.follow_shengqi = nil
    end
end