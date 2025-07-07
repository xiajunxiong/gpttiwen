HuanShouData = HuanShouData or BaseClass()

HuanShouData.SkillType = {
    Exclusive = 0,  -- 专属技能
    Book = 1,       -- 打书技能
    Star = 2,       -- 天赋技能
}

HuanShouData.BlockPutReason = {
    [PUT_REASON_TYPE.PUT_REASON_GIFT_RAND_ITEM] = 1,
    [PUT_REASON_TYPE.PUT_REASON_OPTIONAL_GIFT] = 1,
    [PUT_REASON_TYPE.PUT_REASON_DEF_GIFT] = 1,
    [PUT_REASON_TYPE.PUT_REASON_RAND_GIFT] = 1,
    [PUT_REASON_TYPE.PUT_REASON_GIFT_OPTIONAL_ITEM] = 1,
}

function HuanShouData:__init()
    if HuanShouData.Instance ~= nil then
        Debuger.LogError("[HuanShouData] attempt to create singleton twice!")
        return
    end
    HuanShouData.Instance = self
    self.huanshou_list = SmartData.New({})
    self.sel_huanshou = SmartData.New({})   -- 当前选中的幻兽
    self.sel_tab = SmartData.New({})
    self.egg_record_data = SmartData.New({}) -- 开蛋记录

    self.bag_item_list = SmartData.New({})   -- 幻兽背包

    self.other_data = SmartData.New({})

    self.resolve_select_items = SmartData.New({})  -- 幻兽灵魄分解选择的

    self.remind_data = SmartData.New({v = false})   -- 用于红点通知


    self.jump_huanshou_data = {}     -- 跳转到哪个幻兽

    self.open_egg_times_list = SmartData.New({})    -- 幻兽开蛋次数记录 服务端是累加的，用的时候记得求模

    self.select_equip_type = 0      -- 用于打开幻兽装备界面时记录选择的装备类型

    self:InitCfg()
    self:ResetHuanShou()
end

function HuanShouData:InitCfg()
    self.cfg = Config.huanshou_auto

    self.huanshou_qua_group_cfg = DataHelper.ConvertGroup(self.cfg.huanshou, "huanshou_quality")
    self.huanshou_level_group_cfg = DataHelper.ConvertGroup(self.cfg.att_group, "group")
    self.huanshou_book_group_cfg = DataHelper.ConvertGroup(self.cfg.huanshou_skill, "skill_quality")

    Cfg.Register("HuanShouBaseCfg",Cfg.RealTimeSearch(self.cfg.huanshou,"huanshou_id"))
    Cfg.Register("HuanShouBaseCfgByItemId",Cfg.RealTimeSearch(self.cfg.huanshou,"item_id"))
    Cfg.Register("HuanShouLevelCfg",Cfg.RealTimeSearch2Key(self.cfg.att_group, "group", "level"))

    -- 技能配置
    Cfg.Register("HuanShouSkillCfg",Cfg.RealTimeSearch3Key(self.cfg.skill_dec, "skill_type", "skill_id", "skill_level"))
    Cfg.Register("HuanShouSkillBookCfg",Cfg.RealTimeSearch(self.cfg.huanshou_skill,"skill_id"))

    -- 天赋配置
    Cfg.Register("HuanShouStarCfg",Cfg.RealTimeSearch(self.cfg.potential_point,"potential_point"))
    Cfg.Register("HuanShouStarLevelCfg",Cfg.RealTimeSearch3Key(self.cfg.huanshou_potential, "group", "potential_point", "qiannen_level"))

    -- 幻兽装备
    Cfg.Register("HuanShouEquipCfg",Cfg.RealTimeSearch2Key(Config.huanshou_auto.huanshou_equip, "type", "level"))
end

function HuanShouData:ClearData()
    self:ResetHuanShou()
    self.other_data:Set({})
end

function HuanShouData:IsOpen()
    local is_open = FunOpen.Instance:IsFunOpen(Mod.HuanShou.Main)
    return is_open
end

function HuanShouData:CheckRemind()
    self.remind_data.v = not self.remind_data.v
end

function HuanShouData:SelTab(sel_tab)
    if TableIsEmpty(sel_tab) then
        self.sel_tab:Set({})
    else
        self.sel_tab:Set(sel_tab)
    end
    return self.sel_tab
end

function HuanShouData:GetSelTab()
    return self.sel_tab
end

function HuanShouData:ResetHuanShou()
    for i,v in pairs(self.cfg.huanshou) do
        local id = v.huanshou_id
        local huanshou = HuanShou.New({id = id})
        self:SetHuanShou(huanshou)
    end
end

function HuanShouData:SetHuanShou(huanshou)
    self.huanshou_list[huanshou.id] = huanshou
    local sel_huan_shou = self:GetSelHuanShou()
    if not TableIsEmpty(sel_huan_shou) and sel_huan_shou.id == huanshou.id then
        self:SetSelHuanShou(huanshou)
    end
end

function HuanShouData:GetHuanShou(id)
    return self.huanshou_list[id]
end

-- 获取幻兽列表
function HuanShouData:GetHuanShouList(qua, is_sort)
    if is_sort == nil then
        is_sort = true
    end
    if qua then
        local cfg_list = self.huanshou_qua_group_cfg[qua] or {}
        local list = {}
        for i,v in pairs(cfg_list) do
            local huan_shou = self:GetHuanShou(v.huanshou_id)
            table.insert(list, huan_shou)
        end
        if is_sort then
            table.sort(list, function (a,b)
                if a:Level() == b:Level() then
                    return a.id < b.id
                end
                return a:Level() > b:Level()
            end)
        end
        return list
    end
    return self.huanshou_list
end


-- 获取单个幻兽配置
function HuanShouData:GetCfgById(id)
    return Cfg.HuanShouBaseCfg(id)
end

-- 获取单个幻兽属性配置
function HuanShouData:GetLevelCfg(id, level)
    return Cfg.HuanShouLevelCfg(id, level)
end

-- 获取一组幻兽属性配置
function HuanShouData:GetLevelGroupById(huan_shou_id)
    return self.huanshou_level_group_cfg[huan_shou_id] or {}
end

-- 获取技能配置
function HuanShouData:GetSkillCfg(skill_type, skill_id, skill_level)
    skill_level = skill_level or 1
    local cfg = Cfg.HuanShouSkillCfg(skill_type, skill_id, skill_level)
    return cfg
end

-- 提取属性配置里面的属性
function HuanShouData:GetAttriListByLevelCfg(level_cfg)
    local attri_list = {}
    for i = 1, level_cfg.att_num or 3 do
        local attri_value = level_cfg["attribute_value_" .. i]
        if attri_value and attri_value > 0 then
            local attri_type = level_cfg["attr_type_" .. i]
            local t = {}
            t.attri_type = attri_type
            t.attri_value = attri_value
            table.insert(attri_list, t)
        end
    end
    return attri_list
end


function HuanShouData:SetSelHuanShou(huanshou)
    self.sel_huanshou:Set(huanshou)
end

function HuanShouData:GetSelHuanShou()
    return self.sel_huanshou
end

function HuanShouData:IsShowFuShen()
    local flag = self.other_data.is_use_huanshou or 0
    return flag == 1
end

-- 角色附身的幻兽
function HuanShouData:RoleFuShenId()
    return self.fushen_id.is_use_huanshou or -1
end

-- 附身显示的模型id，不显示返回空
function HuanShouData:FuShenResId()
    if not self:IsShowFuShen() then
        return nil
    end
    if self:RoleFuShenId() < 0 then
        return nil
    end
    local id = self:RoleFuShenId()
    local huanshou = self:GetHuanShou(id)
    return huanshou:ResId()
end

-- 是否使用幻兽形象，是的话返回模型id
-- 注意，替代老接口：LiveData:CardModel(appearance)
function HuanShouData:GetFuShenModelByApp(appearance)
    if appearance == nil then
        return
    end
    if appearance.is_use_huanshou ~= 1 or appearance.huanshou_id < 0 then
        return
    end
    local huanshou = self:GetHuanShou(appearance.huanshou_id)
    return huanshou:ResId()
end

-- ================= 背包专区 ======================
function HuanShouData:SetBagItem(protocol)
    local reason = protocol.reason
    local put_reason = protocol.put_reason
    local item = protocol.item
    local index = item.index
    local bag_item = self:GetBagItem(index)
    local show_item
    local item_id = item.item_id
    if bag_item then
        local num = item.num - bag_item.num
        show_item = Item.Create({item_id = item_id, num = num})
    else
        show_item = item
    end
    if reason == BAGITEM_CHANGEREASON.OUTSIDE_IN and HuanShouData.BlockPutReason[put_reason] == nil then
        PublicPopupCtrl.Instance:CenterI({item_id = item_id, num = show_item.num,color_str = show_item:ColorStr(),item = show_item,icon_id = show_item:IconId()})
        ChatCtrl.Instance:ChannelChatCustom{item_get = {item = show_item, num = show_item.num}}
    end
    self.bag_item_list[item.index] = item
end

function HuanShouData:GetBagItem(index)
    return self.bag_item_list[index]
end

function HuanShouData:GetBagItemByItemId(item_id)
    for i,v in pairs(self.bag_item_list) do
        if v.item_id == item_id then
            return v
        end
    end
end

function HuanShouData:GetBagItemNum(item_id)
    local num = 0
    for i,v in pairs(self.bag_item_list) do
        if v.item_id == item_id then
            num = num + v.num
        end
    end
    return num
end

-- 获取全部灵魄
function HuanShouData:GetAllSoulItem()
    --local items = self.bag_item_list
    local items = BagData.Instance.item_list[ItemColumnType.Item]
    local list = {}
    for i,v in pairs(items) do
        if v:ShowType() == ShowType.HuanShouSoul then
            table.insert(list, v)
        end
    end
    return list
end

-- ==================

-- ================= 打书专区 ======================
function HuanShouData:GetBookCfgList(qua)
    return self.huanshou_book_group_cfg[qua]
end
-- ==================================================


-- ================= 天赋点专区 ======================
function HuanShouData:GetStarAttriListByCfg(level_cfg)
    if level_cfg.tianfu_type == 1 then
        return {}
    end
    local attri = {}
    attri.attri_type = level_cfg.param1
    attri.attri_value = level_cfg.param2
    return attri
end
-- ==================================================



-- ================= 蛋蛋专区 ======================
function HuanShouData:SetEggRecord(list)
    self.egg_record_data:Set(list)
end
function HuanShouData:GetEggRecord()
    return self.egg_record_data
end
-- 获取蛋累计抽了多少次
function HuanShouData:GetOpenEggTimes(egg_id)
    return self.open_egg_times_list[egg_id] or 0
end
-- ==================================================


-- ================= 分解专区 ======================
--获取选择的items
function HuanShouData:GetResolveSelectItems()
    return self.resolve_select_items
end

--设置选中的itms
function HuanShouData:SetResolveSelectItems(index,item)
    local index = item:Type() * 10000 + index
    if self.resolve_select_items[index] == nil then
        self.resolve_select_items[index] = item
    else
        self.resolve_select_items[index] = nil
    end
end

--清空选中的items
function HuanShouData:ClearResolveSelectItems()
    self.resolve_select_items:Set({})
end

--获取可以分解多少道具
function HuanShouData:SetResolveSelectItemsExportItem()
    local count = 0
    local item_list = {}
    for i,v in pairs(self.resolve_select_items:Val()) do
        if v:ShowType() == ShowType.HuanShouSoul then
            local cfg = Cfg.HuanShouBaseCfgByItemId(v.item_id)
            local item_id = cfg.fenjie_item.item_id
            local num = cfg.fenjie_item.num
            if not item_list[item_id] then
                item_list[item_id] = 0
            end
            item_list[item_id] = item_list[item_id] + num
        end
    end
    local list = {}
    for i,v in pairs(item_list) do
        local item = Item.Create({item_id = i, num = v})
        table.insert(list, item)
    end
    return list
end

function HuanShouData:SetResolveSelectItemsExportNum()
    local count = 0
    for i,v in pairs(self.resolve_select_items:Val()) do
        if v:ShowType() == ShowType.HuanShouSoul then
            local cfg = Cfg.HuanShouBaseCfgByItemId(v.item_id)
            local item_id = cfg.fenjie_item.item_id
            local num = cfg.fenjie_item[0].num
            local item_num = v.num
            count = count + num * item_num
        end
    end
    return count
end

-- ==================================================


-- ======================= 逻辑区 =======================

-- 跳到指定幻兽
function HuanShouData:JumpHuanShou(id)
    if not id then
        self.jump_huanshou_data = {}
        return
    end
    local cfg = self:GetCfgById(id)
    local qua = cfg.huanshou_quality
    local t = {}
    t.jump_qua = qua
    local list_data = self:GetHuanShouList(qua)
    for i,v in pairs(list_data) do
        if v.id == id then
            t.jump_index = i
        end
    end
    if t.jump_index then
        self.jump_huanshou_data = t
        ViewMgr:OpenViewByKey(Mod.HuanShou.Attri)
    end
end

function HuanShouData:GetJumpHuanShou()
    return self.jump_huanshou_data or {}
end



-- 处理通知
function HuanShouData:HandleNotice(protocol)
    local info = protocol.info
    local notice_type = info.notice_type
    if notice_type == HuanShouNoticeType.NOTICE_TYPE_ACTIVE then
        local huanshou_id = info.p1
        local vo = {id = huanshou_id, level = 1}
        vo.star_level_list = {}
        vo.book_skill_id_list = {}
        local huanshou = HuanShou.New(vo)
        self:SetHuanShou(huanshou)
        ViewMgr:OpenView(HuanShouGainView,{huanshou = huanshou})
    elseif notice_type == HuanShouNoticeType.NOTICE_TYPE_ACTIVE_CONVERT then
        local huanshou_id = info.p1
        local vo = {id = huanshou_id, level = 1}
        vo.star_level_list = {}
        vo.book_skill_id_list = {}
        local huanshou = HuanShou.New(vo)
        ViewMgr:OpenView(HuanShouGainView,{huanshou = huanshou})
        PublicPopupCtrl.Instance:Center(Language.HuanShou.Tip4)
    elseif notice_type == HuanShouNoticeType.NOTICE_TYPE_STR_LEVEL then
        local huanshou_id = info.p1
        local level = info.p2
        local huanshou = self:GetHuanShou(huanshou_id)
        ViewMgr:OpenView(HuanShouAttriUpSucc, {huanshou = huanshou})
        local vo = TableCopy(huanshou.vo)
        vo.level = level
        huanshou = HuanShou.New(vo)
        self:SetHuanShou(huanshou)
    elseif notice_type == HuanShouNoticeType.NOTICE_TYPE_LEARN_POINT then
        local huanshou_id = info.p1
        local star_id = info.p2
        local star_level = info.p3
        local huanshou = self:GetHuanShou(huanshou_id)
        local vo = TableCopy(huanshou.vo)
        vo.star_level_list[star_id] = star_level
        huanshou = HuanShou.New(vo)
        self:SetHuanShou(huanshou)
    elseif notice_type == HuanShouNoticeType.NOTICE_TYPE_LEARN_SKILL then
        local huanshou_id = info.p1
        local skill_index = info.p2
        local skill_id = info.p3
        local huanshou = self:GetHuanShou(huanshou_id)
        local vo = TableCopy(huanshou.vo)
        vo.new_skill_grid_idx = skill_index
        vo.new_skill_id = skill_id
        huanshou = HuanShou.New(vo)
        self:SetHuanShou(huanshou)
        --ViewMgr:OpenView(HuanShouSkillStudyResult)
    elseif notice_type == HuanShouNoticeType.NOTICE_TYPE_CLEAR_LEARN_SKILL then
        local huanshou_id = info.p1
        local skill_index = info.p2
        local skill_id = info.p3
        local huanshou = self:GetHuanShou(huanshou_id)
        local vo = TableCopy(huanshou.vo)
        vo.new_skill_grid_idx = -1
        vo.new_skill_id = 0
        huanshou = HuanShou.New(vo)
        self:SetHuanShou(huanshou)
    elseif notice_type == HuanShouNoticeType.NOTICE_TYPE_SAVE_LEARN_SKILL then
        local huanshou_id = info.p1
        local skill_index = info.p2
        local skill_id = info.p3
        local huanshou = self:GetHuanShou(huanshou_id)
        local vo = TableCopy(huanshou.vo)
        vo.new_skill_grid_idx = -1
        vo.new_skill_id = 0
        vo.book_skill_id_list[skill_index] = skill_id
        huanshou = HuanShou.New(vo)
        self:SetHuanShou(huanshou)
        SkillData.Instance:PushNewSkillPopup({skill_id = skill_id, 
            is_huanshou = true, skill_level = 1, skill_type = HuanShouData.SkillType.Book})
        ViewMgr:OpenView(NewSkillPopupView)
    elseif notice_type == HuanShouNoticeType.NOTICE_TYPE_EGG_OPEN_COUNT then
        self.open_egg_times_list[info.p1] = info.p2
    end
end







-- 附身
function HuanShouData:CtrlFuShen(huanshou)
    if not huanshou:IsActive() then
        PublicPopupCtrl.Instance:Error(Language.HuanShou.NotActive)
        return
    end
    if not huanshou:IsFuShen() then
        HuanShouCtrl.Instance:SendReq(HuanShouReqType.FuShen, huanshou.id)
        HuanShouCtrl.Instance:SendReq(HuanShouReqType.ShowModel, 1)
    else
        HuanShouCtrl.Instance:SendReq(HuanShouReqType.CancelFuShen)
    end
end

-- 附身显示
function HuanShouData:ShowFuShenView(huanshou)
    ViewMgr:OpenView(HuanShouFuShenShow, {huanshou = huanshou})
    local ex_skill = huanshou:ExSkill()
    if ex_skill then
        SkillData.Instance:PushNewSkillPopup({skill_id = ex_skill.skill_id, 
        is_huanshou = true, skill_level = ex_skill.level})
        ViewMgr:OpenView(NewSkillPopupView)
    end
end

-- 升级幻兽
function HuanShouData:UpHuanShou(huanshou)
    HuanShouCtrl.Instance:SendReq(HuanShouReqType.UpLevel, huanshou.id)
end
-- 幻兽是否可以升级 flag == 1 可以， 0未激活，-1材料不足，-2满级了
function HuanShouData:IsCanUpLevel(huanshou)
    if not huanshou:IsActive() then
        return 0
    end
    if huanshou:IsMaxLevel() then
        return -2
    end
    local next_level_cfg = huanshou:NextLevelCfg()
    local stuff_list = next_level_cfg.cost_item
    for i,v in pairs(stuff_list) do
        if Item.GetNum(v.item_id) < v.num then
            return -1, v.item_id
        end
    end
    return 1
end


-- 升级天赋
function HuanShouData:UpStar(star)

end
-- 天赋：1可升级，0材料不足，-1已满级, -2 不可激活
function HuanShouData:GetStarUpFlag(star)
    if star:IsMax() then
        return -1
    end

    if not star:IsCanActive() then
        return -2
    end

    local stuff_list = star:StuffList()
    for i,v in pairs(stuff_list) do
        local has_num = Item.GetNum(v.item_id)
        if has_num < v.num then
            return 0, v.item_id
        end
    end

    return 1
end


-- 角色变身卡技能信息 is_accord:true则筛选主动技能出来
function HuanShouData:GetBattleSkillInfo(skill_info)
    if TableIsEmpty(skill_info) then
        return
    end
    if skill_info.skill_id < 1 or skill_info.skill_level < 1 then
        return
    end

    local skill_type = SkillData.GetSkillType(skill_info.skill_id)
    -- 是被动技能不返回
    if skill_type >= 7 and skill_type <= 9 then
        return
    end
    local sk_info = TableCopy(skill_info)
    sk_info.is_huanshou_skill = true
    sk_info.type_text = ColorStr(Language.Fight.SkillTitle[9], COLORSTR.Purple2)
    sk_info.type_bg = "JiNengJiaoBiao3"
    local skill_cfg = HuanShouData.Instance:GetSkillCfg(HuanShouData.SkillType.Exclusive, skill_info.skill_id, skill_info.skill_level) or {}
    sk_info.skillDesc = skill_cfg.skill_dec or ""
    sk_info.mask_tip = string.format(Language.Role.HuanHuaCDTip3, skill_cfg.skill_cd or 1)
    local cd = skill_info.skill_cd or 0
    if cd == 0 then
        local color = COLORSTR.Green4
        sk_info.is_huanshou_mask = false
    else
        sk_info.is_huanshou_mask = true
    end
    return sk_info
end


-- ================================== 红点逻辑啊 =========================

function HuanShouData:RemindItemList()
    if not self.remind_item_list then
        self.remind_item_list = {}

        for i,v in pairs(Config.huanshou_auto.egg_item) do
            table.insert(self.remind_item_list, v.item_id)
        end
        local item_id = self.cfg.other[1].potential_item
        table.insert(self.remind_item_list, item_id)
        item_id = self.cfg.other[1].jinghua_item
        table.insert(self.remind_item_list, item_id)
        for i,v in pairs(self.cfg.huanshou) do
            table.insert(self.remind_item_list, v.item_id)
        end

        local equip_cfg = Cfg.HuanShouEquipCfg(0,0)
        table.insert(self.remind_item_list, equip_cfg.item_id)
        equip_cfg = Cfg.HuanShouEquipCfg(1,0)
        table.insert(self.remind_item_list, equip_cfg.item_id)
    end
    return self.remind_item_list
end

-- 属性标签红点
function HuanShouData:GetAttriTabRemind()
    local huanshou_list = self.huanshou_list
    for i,v in pairs(huanshou_list) do
        if self:GetAttriOneRemind(v) > 0 then
            return 1
        end
    end
    return 0
end

-- 幻兽属性单个红点
function HuanShouData:GetAttriOneRemind(huanshou)
    local flag,item_id= self:IsCanUpLevel(huanshou)
    if flag > 0 then
        return 1
    end
    local equip_list = huanshou:EquipList()
    for i,v in pairs(equip_list) do
        if v:CtrlFlag() > 0 then
            return 1
        end
    end
    return 0
end

-- 天赋标签红点
function HuanShouData:GetStarTabRemind()
    local huanshou_list = self.huanshou_list
    for i,v in pairs(huanshou_list) do
        if self:GetStarOneRemind(v) > 0 then
            return 1
        end
    end
    return 0
end


-- 幻兽天赋单个红点
function HuanShouData:GetStarOneRemind(huanshou)
    local node_list = huanshou:StarList()
    for i,v in pairs(node_list) do
        local flag = HuanShouData.Instance:GetStarUpFlag(v)
        if flag > 0 then
            return 1
        end
    end
    return 0
end

-- 蛋蛋标签红点
function HuanShouData:GetEggTabRemind()
    local list_data = Config.huanshou_auto.egg_item
    for i,v in pairs(list_data) do
        local item_id = v.item_id
        if Item.GetNum(item_id) > 0 then
            return 1
        end
    end
    return 0
end