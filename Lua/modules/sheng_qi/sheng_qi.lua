-- >>>>>> 圣器物品
ShengQi = ShengQi or BaseClass(Item)

-- 圣器净值解析
function ShengQi.DecodeParam()
    local p = {}
    p.has_net_value = MsgAdapter.ReadInt()  -- //!< 是否已随机净值
    p.level = MsgAdapter.ReadUInt()
    p.exp = MsgAdapter.ReadUInt()
    p.score = MsgAdapter.ReadUInt()         -- 圣器评分
    p.active_max = MsgAdapter.ReadUInt()    -- //!< 圣器当前等级活跃度上限
    p.state = MsgAdapter.ReadShort()    -- 圣器状态 ShengQiCfg.State


    p.item_attri_list = {}
    local add_attri_num = MsgAdapter.ReadShort()				-- //!< 属性额外提升条数(目前固定三条)
    for i = 1,3 do
        local attri = {}
        attri.attri_type = MsgAdapter.ReadShort()
        attri.attri_value = MsgAdapter.ReadShort()
        if attri.attri_value > 0 and i <= add_attri_num then
            table.insert(p.item_attri_list, attri)
        end
    end
    MsgAdapter.ReadInt()
    MsgAdapter.ReadInt()

    p.pet_uid_t = {}
    p.pet_uid_t.id_1 = MsgAdapter.ReadUInt()
    p.pet_uid_t.id_2 = MsgAdapter.ReadUInt()
    p.pet_id = MsgAdapter.ReadInt()


    -- 圣器基础属性汇总，指的是包括天赋技能或其他渠道加成的汇总。客户端目前显示的是这个
    p.attri_list = {}
    for i = 1,ShengQiCfg.MAX_HOLY_EQUIP_BASE_VOC_NUM do
        local attri = {}
        attri.attri_type = MsgAdapter.ReadShort()
        attri.attri_value = MsgAdapter.ReadShort()
        table.insert(p.attri_list,attri)
        if attri.attri_type == SHENG_QI_ATTRI_TYPE.ACTIVENESS then
            p.active_num = attri.attri_value    -- 圣器活跃
        end
    end

    -- 圣器原生基础属性 目前没用到
    p.base_attri_list = {}
    for i = 1,ShengQiCfg.MAX_HOLY_EQUIP_BASE_VOC_NUM do
        local attri = {}
        attri.attri_type = MsgAdapter.ReadShort()
        attri.attri_value = MsgAdapter.ReadShort()
        table.insert(p.base_attri_list,attri)
        if attri.attri_type == SHENG_QI_ATTRI_TYPE.ACTIVENESS then
            --p.active_num = attri.attri_value    -- 圣器活跃 原生属性暂时用不到
        end
    end


    p.unsaved_id_info = {}  --//!< 学习了但还未决定是否替换的技能信息, [0]-技能id, [1]-主动技能索引(如果有技能id这里才合法)
    p.unsaved_id_info.skill_id = MsgAdapter.ReadShort()
    p.unsaved_id_info.skill_index = MsgAdapter.ReadShort()

    p.initative_id_list = {}     -- //!< 主动技能 初始化为-1,解锁之后变成0(或许永远不会变成0) \see SKILL_HOLE_STATUS
    for i = 0,ShengQiCfg.MAX_ACTIVE_SKILLS_NUM - 1 do
        local skill_id = MsgAdapter.ReadInt()
        p.initative_id_list[i] = skill_id
    end

    p.talent_id_list = {}        -- //!< 天赋技能
    for i = 0,ShengQiCfg.MAX_TALENT_SKILLS_NUM - 1 do
        local skill_id = MsgAdapter.ReadInt()
        p.talent_id_list[i] = skill_id
    end


    p.fu_shi_hole_state_list = {}    -- //!< 符石孔位开启状态  0 未解锁，1已解锁
    for i = 0, ShengQiCfg.MAX_FU_SHI_NUM - 1 do
        local flag = MsgAdapter.ReadChar() -- 服务端是 0 解锁，1有符石，-1未解锁
        p.fu_shi_hole_state_list[i] = flag >= 0 and 1 or 0
    end
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()

    -- 身上的符石数据
    p.fu_shi_param_list = {}
    for i = 0, ShengQiCfg.MAX_FU_SHI_NUM - 1 do
        local fu_shi_p = FuShi.DecodeParam()
        p.fu_shi_param_list[i] = fu_shi_p
    end
    --LOG("圣器净值",p)
    return p
end

function ShengQi:__init(vo)
    vo = vo or RO_EMTPY_TAB
    self.index = vo.index or -1
    self.item_id = vo.item_id or 0
    self.is_bind = vo.is_bind or 1
    self.num = vo.num or 1
    self.param = vo.param-- or {}
    self.is_grey = vo.is_grey
    self.state = self.param and self.param.state or ShengQiCfg.State.Rest
    self.level = self.param and self.param.level or 1
    self.score = self.param and self.param.score or 0

    if self.item_id ~= 0 then
        self.id = self:Id()
    end

    -- 符石
    self.fu_shi_param_list = self.param and self.param.fu_shi_param_list or {}
    self.fu_shi_list = {}
    for i = 0, ShengQiCfg.MAX_FU_SHI_NUM - 1 do
        local param = self.fu_shi_param_list[i] or {item_id = 0}
        local vo = {}
        vo.param = param
        vo.item_id = param.item_id
        vo.sheng_qi = self
        vo.hole_index = i
        local fu_shi = FuShi.New(vo)
        self.fu_shi_list[i] = fu_shi
    end


    --天赋技能
    self.talent_skill_list = nil
    if self.param and self.param.talent_id_list then
        for i,v in pairs(self.param.talent_id_list) do
            local skill = ShengQiSkill.New({skill_id = v, index = i, skill_type = SHENG_QI_SKILL_TYPE.TALENT, sheng_qi = self})
            self.talent_skill_list = self.talent_skill_list or {}
            self.talent_skill_list[i] = skill
        end
    end

    --主动技能
    self.initative_skill_list = nil
    if self.param and self.param.initative_id_list then
        for i,v in pairs(self.param.initative_id_list) do
            local skill = ShengQiSkill.New({skill_id = v, index = i, skill_type = SHENG_QI_SKILL_TYPE.INITIATIVE, sheng_qi = self})
            self.initative_skill_list = self.initative_skill_list or {}
            self.initative_skill_list[i] = skill
        end
    end

    --未保存的主动技能
    self.unsaved_skill = nil
    if self.param and not TableIsEmpty(self.param.unsaved_id_info) and self.param.unsaved_id_info.skill_id > 0 then
        local skill_id = self.param.unsaved_id_info.skill_id
        local skill_index = self.param.unsaved_id_info.skill_index
        local skill = ShengQiSkill.New({skill_id = skill_id, index = skill_index, skill_type = SHENG_QI_SKILL_TYPE.INITIATIVE, sheng_qi = self, is_new = true})
        self.unsaved_skill = skill
    end
end

function ShengQi:ShengQiType()
    local cfg = self:Cfg()
    return cfg.shengqi_type
end

function ShengQi:TypeName()
    local type = self:ShengQiType()
    return Language.ShengQi.TypeName[type] or ""
end

function ShengQi:IsInBag()
    if ShengQiData.Instance:GetShengQiByIndex(self.index) then
        return true
    end
    return false
end

function ShengQi:HasParam()
    if TableIsEmpty(self.param) or TableIsEmpty(self.param.pet_uid_t) then
        return false
    end
    if not TableIsEmpty(self.param.pet_uid_t) then
        return true
    end
    if self.index >= 0 then
        return true
    end

    return false
end

function ShengQi:Id()
    local cfg = self:Cfg()
    self.id = cfg.shengqi_id or -1
    return self.id
end

-- 状态
function ShengQi:State()
    return self.state or ShengQiCfg.State.Rest
end

function ShengQi:Score()
    return self.score
end

function ShengQi:ScoreLevel()
    return ShengQiData.Instance:GetScoreLevel(self.score)
end

-- 模型资源
function ShengQi:ResId()
    local cfg = self:Cfg()
    return cfg.res_id
end

function ShengQi:ResPath()
    local res_id = self:ResId()
    local path = DrawerHelper.GetWeaponPath(res_id)
    return path
end

function ShengQi:Name()
    local cfg = self:Cfg()
    return cfg.shengqi_name
end

function ShengQi:Cfg()
    if not self.cfg then
        self.cfg = ShengQiData.Instance:GetShengQiCfg(self.item_id)
    end
    return self.cfg
end

function ShengQi:TypeDesc()
    if not self:HasParam() then
        return Item.TypeDesc(self)
    end
    return nil
end

function ShengQi:LevelDesc()
    return nil
end

function ShengQi:OneselfBaseIndex()
    if not self:HasParam() then
        return
    end
    return 5
end

-- 圣器活跃
function ShengQi:ActiveNum()
    return self.param and self.param.active_num or 0
end

function ShengQi:MaxActiveNum()
    return self.param and self.param.active_max or 1
end

-- 圣器激活概率=圣器活跃/圣器活跃上限
function ShengQi:ActiveRate()
    local active_num = self:ActiveNum()
    local active_max = self:MaxActiveNum()
    local num = active_num / active_max
    --LogError(active_num,active_max,num)
    return num
end

-- 全部的天赋技能 下标0开始
function ShengQi:GetTalentSkillList()
    return self.talent_skill_list or {}
end
-- 单个的天赋技能
function ShengQi:GetTalentSkill(index)
    local skill_list = self:GetTalentSkillList()
    return skill_list[index] or {}
end

function ShengQi:TalenSkillNames(is_dark)
    local talen_names = ""
    local talen_list = self:GetTalentSkillList()
    for i,v in pairs(talen_list) do
        if v.skill_id > 0 then
            talen_names = talen_names .. string.format("%s  ",v:QuaName(is_dark))
        end
    end
    return talen_names
end

-- 全部的主动技能
function ShengQi:GetInitativeSkillList()
    return self.initative_skill_list or {}
end

-- 全部解锁的主动技能  is_update 是否把已学习的技能覆盖上去
function ShengQi:GetUnlockInitativeSkillList(is_update)
    local skill_list = self:GetInitativeSkillList()
    if is_update then
        skill_list = TableCopy(skill_list)
        local update_skill = self:GetStudyInitativeSkill()
        if not TableIsEmpty(update_skill) and update_skill.index >= 0 then
            skill_list[update_skill.index] = update_skill
        end
    end
    local list = {}
    for i,v in pairs(skill_list) do
        if v.skill_id > 0 then
            table.insert(list, v)
        end
    end
    return list
end

-- 单个主动技能
function ShengQi:GetInitativeSkill(index)
    local skill_list = self:GetInitativeSkillList()
    return skill_list[index] or {}
end

-- 单个可学习主动技能
function ShengQi:GetStudyInitativeSkill()
    return self.unsaved_skill
end

-- 获取技能需要解锁的孔位
function ShengQi:GetSkillUnlockHole()
    for i = 0 , 4 do
        local skill = self:GetInitativeSkill(i)
        if skill and skill.skill_id <= 0 then
            return i
        end
    end
end

function ShengQi:AttriList()
    return self.param and self.param.attri_list or {}--{{attri_type = 0, attri_value = 50},{attri_type = 1, attri_value = 850},{attri_type = 2, attri_value = 950}}
end

function ShengQi:GetAttriInfo(attri_type)
    local attri_list = self:AttriList()
    for i,v in pairs(attri_list) do
        if v.attri_type == attri_type then
            return v
        end
    end
    return {}
end

function ShengQi:AttriShowList()
    local attri_list = self:AttriList()
    local list = {}
    local active_attri
    if TableIsEmpty(attri_list) then
        local info = ShengQiData.Instance:FormatAttriInfo()
        info.null_tip = Language.ShengQi.Tip4
        info.is_nothing = true
        active_attri = info
    else
        for i,v in pairs(attri_list) do
            local info = ShengQiData.Instance:FormatAttriInfo(v.attri_type, v.attri_value)
            if v.attri_type == SHENG_QI_ATTRI_TYPE.ACTIVENESS then
                local active_rate = math.floor(self:ActiveRate() * 10000) / 100
                info.tip = string.format(Language.ShengQi.Tip5, active_rate)
                active_attri = info
            else
                table.insert(list, info)
            end
        end
    end
    active_attri = active_attri or ShengQiData.Instance:FormatAttriInfo()
    return active_attri,list
end

-- 初始圣器属性预览，取最大的
function ShengQi:AttriPreviewList()
    local attri_list = {}
    local cfg = self:Cfg()
    for i = 0, SHENG_QI_ATTRI_TYPE.MAX do
        local attri = {}
        local flied_name = SHENG_QI_ATTRI_SCORE_NAME[i] .. "_max"
        attri.attri_value = cfg[flied_name] or 0
        attri.attri_type = i
        if attri.attri_value > 0 then
            local info = ShengQiData.Instance:FormatAttriInfo(attri.attri_type, attri.attri_value)
            table.insert(attri_list, info)
        end
    end
    return attri_list
end

function ShengQi:Qua()
    local cfg = self:Cfg()
    return cfg.shengqi_quality
end

function ShengQi:Level()
    return self.level
end

function ShengQi:MaxLevel()
    return Config.shengqi_auto.other[1].maxlevel
end

function ShengQi:IsFullLevel()
    return self:Level() >= self:MaxLevel()
end

-- 增加的宠物属性
function ShengQi:PetAttriList()
    if self.param == nil then
        self.param = {}
    end
    if TableIsEmpty(self.param.item_attri_list) then
        self.param.item_attri_list = {}
        local cfg = self:Cfg()
        local attri_type_list = Split(cfg.att_type, "|")
        local level_cfg = Cfg.ShengQiUpCfg(self:Qua(),1)
        for i,v in pairs(attri_type_list) do
            local attri = {}
            local attri_type = tonumber(v)
            local field_name = ShengQiCfg.AttriFieldName[attri_type] or ""
            attri.attri_type = attri_type
            attri.attri_value = level_cfg[field_name] or 0
            table.insert(self.param.item_attri_list,attri)
        end
    end
    return self.param.item_attri_list
end

-- 预览最高等级时宠物属性
function ShengQi:MaxPreviewPetAttriList()
    local max_level = self:MaxLevel()
    local attri_list = self:ItemAttriList(max_level)
    return attri_list
end

-- 增加的宠物属性 preview_level预览等级
function ShengQi:ItemAttriList(preview_level)
    local item_attri_list = self:PetAttriList()
    preview_level = preview_level or self:Level()
    local cfg = Cfg.ShengQiUpCfg(self:Qua(),preview_level)
    local attri_list = {}
    for i,v in pairs(item_attri_list) do
        local attri = TableCopy(v)
        local field_name = ShengQiCfg.AttriFieldName[v.attri_type] or ""
        attri.preview_value = cfg[field_name] or 0
        table.insert(attri_list,attri)
        attri.index = #attri_list
    end
    return attri_list
end


-- 当前等级配置
function ShengQi:LevelCfg()
    local qua = self:Qua()
    local level = self:Level()
    local cfg = Cfg.ShengQiUpCfg(qua,level)
    return cfg
end

-- 下级等级配置
function ShengQi:NextLevelCfg()
    local qua = self:Qua()
    local level = self:Level() + 1
    if level > self:MaxLevel() then
        level = self:MaxLevel()
    end
    local cfg = Cfg.ShengQiUpCfg(qua,level)
    return cfg
end

-- 当前经验
function ShengQi:Exp()
    return self.param and self.param.exp or 0
end

-- 当前升级进度
function ShengQi:UpProgress()
    local cur_cfg = self:LevelCfg()
    if TableIsEmpty(cur_cfg) then
        BuglyReport("圣器等级异常",self.item_id,self.level)
        return 0,0
    end
    local start_exp = self:Exp() - cur_cfg.upgrade_exp
    local next_cfg = self:NextLevelCfg()
    local end_exp = 0
    if not TableIsEmpty(next_cfg) then
        end_exp = next_cfg.upgrade_exp - cur_cfg.upgrade_exp
    end
    return start_exp,end_exp
end

-- 需要多少经验升至下一级
function ShengQi:NeedExpToNextLevel()
    local next_cfg = self:NextLevelCfg()
    local exp = next_cfg.upgrade_exp - self:Exp()
    return exp
end

-- 作为材料值多少经验
function ShengQi:StuffExp()
    local cfg = self:Cfg()
    local rate = Config.shengqi_auto.other[1].exp_back / 1000
    local exp = cfg.fenjie_exp + self:Exp() * rate
    return exp
end

-- 距离升满还差多少经验
function ShengQi:FullExp()
    local qua = self:Qua()
    local max_level = self:MaxLevel()
    local cfg = Cfg.ShengQiUpCfg(qua,max_level)
    local exp = cfg.upgrade_exp - self:Exp()
    return exp
end

function ShengQi:GetAllFushi()
    return self.fu_shi_list or {}
end

-- 是否有符石
function ShengQi:IsHasFushi()
    for i,v in pairs(self:GetAllFushi()) do
        if v.item_id > 0 then
            return true
        end
    end
    return false
end

-- 是否存在一样的类型的符石
function ShengQi:GetEqualityFuShi(fushi)
    for i,v in pairs(self:GetAllFushi()) do
        if v.item_id > 0 then
            if v:FuShiType() == fushi:FuShiType() then
                return v
            end
        end
    end
end

-- 获取身上的符石 0 - 4
function ShengQi:GetFuShi(hole_index)
    return self.fu_shi_list[hole_index]
end

-- 符石是否已经解锁
function ShengQi:FuShiIsUnlock(hole_index)
    local flag_t = self.param and self.param.fu_shi_hole_state_list or {}
    local flag = flag_t[hole_index] or 0
    return flag > 0
end

-- 符石显示成圆点
function ShengQi:QuaShowPoint()
    if self.fushi_str == nil then
        local str = ""
        for i,v in pairs(self:GetAllFushi()) do
            if v.item_id ~= 0 then
                str = str .. v:Level()
            end
        end
        self.fushi_str = str
    end
    return self.fushi_str
end

-- 按照顺序镶嵌, 获取能镶嵌的位置 -1没有镶嵌的位置, -2符石类型重复
-- 必须是不同种类的
function ShengQi:GetFuShiInsertIndex(fushi)
    for i = 0, 4 do
        local cur_fushi = self:GetFuShi(i)
        if cur_fushi.item_id ~= 0 and cur_fushi:FuShiType() == fushi:FuShiType() then
            return i
        end
    end
    for i = 0, 4 do
        if self:FuShiIsUnlock(i) and self:GetFuShi(i).item_id == 0 then
            return i
        end
    end
    return -1
end

-- 获取全部符石属性的总合
function ShengQi:AllFuShiAttriList()
    local all_attri_list = {}
    local _list = {[0]=0,0,0,0,0,0,0}
    for i = 0, 4 do
        local fushi = self:GetFuShi(i)
        if fushi.item_id ~= 0 then
            local attri_list = fushi:AttriList()
            for j,v in pairs(attri_list) do
                _list[v.attri_type] = _list[v.attri_type] or 0
                _list[v.attri_type] = _list[v.attri_type] + v.attri_value
            end
        end
    end
    for i,v in pairs(_list) do
        table.insert(all_attri_list, {attri_type = i,attri_value = v})
    end
    return all_attri_list
end

-- 获取全部符石角色属性的总合
function ShengQi:AllFuShiRoleAttriList(is_show)
    local all_attri_list = {}
    for i = 0, 4 do
        local fushi = self:GetFuShi(i)
        if fushi.item_id ~= 0 then
            local attri = fushi:RoleAttri()
            table.insert(all_attri_list, attri)
        end
    end
    if is_show then
        local list = {}
        for i,v in pairs(all_attri_list) do
            local info = CommonStruct.AttrStruct(v.attri_type, v.attri_value)
            table.insert(list, info)
        end
        return list
    end
    return all_attri_list
end

function ShengQi:AllShowFuShiAttriList()
    local attri_list = self:AllFuShiAttriList()
    local list = {}
    for i,v in pairs(attri_list) do
        local info = ShengQiData.Instance:FormatAttriInfo(v.attri_type, v.attri_value)
        table.insert(list, info)
    end
    return list
end

function ShengQi:PetUid()
    return self.param.pet_uid_t or {id_1 = 0, id_2 = 0}
end

function ShengQi:BlessPet()
    local pet_list = PetData.Instance:GetPetList()
    local uid_t = self:PetUid()
    for i,v in pairs(pet_list) do
        local pet_info = v.info
        if pet_info.pet_unique_id1 == uid_t.id_1 and pet_info.pet_unique_id2 == uid_t.id_2 then
            return v
        end
    end
end

-- 推荐职业描述
function ShengQi:ProfDesc()
    local desc = ""
    local cfg = self:Cfg()
    local prof_list = Split(cfg.recommend_prof, "|")
    for i,v in pairs(prof_list) do
        local prof = tonumber(v)
        local prof_text = Language.Common.Prof[prof] or ""
        if prof == RoleData.Instance:ProfType() then
            prof_text = ColorStr(prof_text, COLORSTR.Green3)
        end
        desc = desc .. prof_text
        if i ~= #prof_list then
            desc = desc .. "  "
        end
    end
    return desc
end



-- >>>>> 圣器技能
ShengQiSkill = ShengQiSkill or BaseClass()
function ShengQiSkill:__init(vo)
    --LOG("圣器技能",vo)
    vo = vo or RO_EMTPY_TAB
    self.skill_type = vo.skill_type or SHENG_QI_SKILL_TYPE.TALENT
    self.index = vo.index or -1
    self.skill_id = vo.skill_id or 0
    self.param = vo.param-- or {}
    self.sheng_qi = vo.sheng_qi
    self.is_book = vo.is_book   -- 图鉴的
    self.is_new = vo.is_new     -- 用于显示“新”标记
end

function ShengQiSkill:IsLock()
    return self.skill_id < 1
end

-- 可为空
function ShengQiSkill:ShengQi()
    return self.sheng_qi
end

-- 是否是擅长的技能
function ShengQiSkill:IsNice()
    if TableIsEmpty(self.sheng_qi) then
        return false
    end
    local sheng_qi_id = self.sheng_qi.id
    local cfg = self:Cfg()
    if not cfg.shanchang_shengqi then
        return false
    end
    local skill_id_list = Split(cfg.shanchang_shengqi, "|")
    for i,v in pairs(skill_id_list or {}) do
        if tonumber(v) == sheng_qi_id then
            return true
        end
    end
    return false
end

function ShengQiSkill:GetNiceShengQiList()
    local cfg = self:Cfg()
    local list = Split(cfg.shanchang_shengqi, "|")
    local s_list = {}
    for i,v in pairs(list) do
        table.insert(s_list, tonumber(v))
    end
    return s_list
end

function ShengQiSkill:GetNiceShengQiName()
    local list = self:GetNiceShengQiList()
    local name = ""
    for i,v in pairs(list) do
        local cfg = ShengQiData.Instance:GetCfgById(v)
        name = name .. cfg.shengqi_name
        if i ~= #list then
            name = name .. "、"
        end
    end
    return name
end

function ShengQiSkill:Cfg()
    if not self.cfg then
        local type = self:Type()
        if type == SHENG_QI_SKILL_TYPE.INITIATIVE then
            if self.is_book then
                self.cfg = ShengQiData.Instance:GetSkillBookCfg(self.skill_id)

            else
                self.cfg = Cfg.ShengQiZhuDongSkill(self.skill_id,false)   
                if TableIsEmpty(self.cfg) then -- 技能id可能是擅长的
                    self.cfg = Cfg.ShengQiShanChangSkill(self.skill_id)
                end
            end           
        elseif type == SHENG_QI_SKILL_TYPE.TALENT then
            self.cfg = Cfg.ShengQiTianFuSkill(self.skill_id)
        end
    end
    return self.cfg or {}
end

function ShengQiSkill:Type()
    return self.skill_type
end

function ShengQiSkill:SkillId()
    return self.skill_id
end

function ShengQiSkill:Icon()
    local cfg = self:Cfg()
    --return "s940"
    return cfg.skill_icon
end

-- 只有主动技能有合击等级
function ShengQiSkill:ComLevel()
    if self.skill_type == SHENG_QI_SKILL_TYPE.INITIATIVE then
        local cfg = self:Cfg()
        return cfg.combo_level or 1
    else
        return 0
    end
end

function ShengQiSkill:Name()
    local cfg = self:Cfg()
    -- if self.skill_type == SHENG_QI_SKILL_TYPE.INITIATIVE then
    --     local level = self:ComLevel()
    --     local name = cfg.skill_name or ""
    --     if self.is_book then
    --         return cfg.skill_name
    --     end
    --     return name .. Language.ShengQi.SLevel[level]
    -- else
    --     return cfg.skill_name
    -- end

    return cfg.skill_name
end

function ShengQiSkill:QuaName(shallow)
    local qua = self:Qua()
    local name = self:Name()
    local color_t = shallow and QualityColorStrShallow or QualityColorStr
    local color = color_t[qua]
    return ColorStr(name, color)
end

function ShengQiSkill:Desc()
    local cfg = self:Cfg()
    -- if self:IsNice() then
    --     return string.format("%s（%s）",cfg.skill_dec,cfg.buff_dec)
    -- else
    --     return cfg.skill_dec
    -- end
    return cfg.skill_dec
end

function ShengQiSkill:SkillTypeDesc()
    local cfg = self:Cfg()
    local str
    if cfg.combo_level == 1 then
        str = Language.ShengQi.Text1
    else
        str = string.format(Language.ShengQi.Text2, cfg.combo_level)
    end
    return str
end

function ShengQiSkill:TriggerDesc()
    local cfg = self:Cfg()
    -- local level = cfg.combo_level
    -- if not level then
    --     return ""
    -- end
    -- local desc = level == 1 and Language.ShengQi.Tip20 or string.format(Language.ShengQi.Tip21, level - 1)
    -- return desc
    return cfg.combo_dec
end

function ShengQiSkill:CDDesc()
    if self.skill_type == SHENG_QI_SKILL_TYPE.INITIATIVE then
        local cfg = self:Cfg()
        return cfg.skill_cd
    else
        return ""
    end
end

function ShengQiSkill:Qua()
    local cfg = self:Cfg()
    return cfg.skill_quality or 0
end
