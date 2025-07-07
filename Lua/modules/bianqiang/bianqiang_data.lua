BianQiangData = BianQiangData or BaseClass()
--主界面变强提醒类型
BianQiangData.ListType = {
        JueSeKeFeiSheng = 1, --"角色可飞升"
        JueSeQianNengDian = 2,-- "角色潜能点"
        ZhiYeJinJie = 3,--"职业晋阶"
        XiuLianJiNeng =4,--"修炼技能"
        ZhuangBeiKeTiHuang = 5,--"装备可替换"
        BaoShiKeXiangQian = 6,--"宝石可镶嵌",
        JiNengKeJieSuo = 7,--"技能可解锁",
        JiNengKeShengJi =8,--"技能可升级",
        JueZhaoKeShengJi = 9,--"绝招可升级",
        XiuWeiKeTiSheng=10,--"修为可提升",
        ChongWuKeFeiSheng = 11,--"宠物可飞升",
        ChongWuKeShengJi=12,--"宠物升级",
        ChongWuQianNengDian= 13,--"宠物潜能点",
        ChongWuTuPo=14,--"宠物突破",
        HuoBanKeFeiSheng=15,--"伙伴可飞升",
        HuoBanKeShengJi=16,--"伙伴可升级",
        HuoBanKeJinSheng=17,--"伙伴可晋升",
        HuoBanKeJueXing=18,--"伙伴可觉醒",
        --JinRiTongJi=19,--"今日统计",
        WoYaoBianQiang=19,--"我要变强"
}
--结算显示按钮类型
BianQiangData.FinishBtnType = {
    JueSeKeFeiSheng = 1, --"角色可飞升"
    JueSeQianNengDian = 2,-- "角色潜能点"
    ZhiYeJinJie = 3,--"职业晋阶"
    XiuLianJiNeng = 4,--"修炼技能"
    ZhuangBeiKeTiHuang = 5,--"装备可替换"
    BaoShiKeXiangQian = 6,--"宝石可镶嵌",
    JiNengKeJieSuo = 7,--"技能可解锁",
    JiNengKeShengJi = 8,--"技能可升级",
    JueZhaoKeShengJi = 9,--"绝招可升级",
    XiuWeiKeTiSheng = 10,--"修为可提升",
    ChongWuKeFeiSheng = 11,--"宠物可飞升",
    ChongWuKeShengJi = 12,--"宠物升级",
    ChongWuQianNengDian = 13,--"宠物潜能点",
    ChongWuTuPo = 14,--"宠物突破",
    HuoBanKeFeiSheng=15,--"伙伴可飞升",
    HuoBanKeShengJi=16,--"伙伴可升级",
    HuoBanKeJinSheng=17,--"伙伴可晋升",
    HuoBanKeJueXing=18,--"伙伴可觉醒",
    LingQiTiSheng=19,--"灵骑提升",
    WoYaoBianQiang=20,--"我要变强"
    XinFaTiSheng = 21, --心法提升
    JingJieTiSheng = 22, --境界提升
    ZhuangBeiQiangHua = 23,--装备强化
    ShenZhuangPeiYang = 24,--神装培养
    ShengQiPeiYang = 25,--圣器
    ZaoHuaZhuangBei = 26,--造化装备
}
--变强条件类型
BianQiangType = {
    TYPE_NONE = 0,
    -- X-> param1  Y->param2
    TYPE_ROLE_SKILL_LV_1 = 1,   --玩家X个职业技能达到Y级(Y=玩家等级可升最高级)
    TYPE_ROLE_SKILL_LV_2 = 2,   --玩家X个修炼技能达到Y级(Y=玩家等级可升最高级)
    TYPE_ROLE_SKILL_LV_3 = 3,   --玩家X个辅助技能达到Y级(Y=玩家等级可升最高级)
    TYPE_ROLE_EQUIP_COLOR_1 = 4,--玩家穿戴X件Y色装备
    TYPE_ROLE_EQUIP_COLOR_2 = 5,--玩家穿戴X件Y色灵饰
    TYPE_FABAO_LV = 6,          --X件法宝达到Y级
    TYPE_TUJIAN_ACTIVED = 7,    --激活x组图鉴
    TYPE_USED_TOKEN_LV = 8,     --穿戴X件Y阶令牌
    TYPE_PET_LV = 9,            --携带X个Y级宠物（Y等级玩家自身等级）
    TYPE_PET_SKILL_NUM = 10,    --X只宠物打满了高级技能书
    TYPE_PET_COLOR_NUM = 11,    --拥有X只稀有宠物
    TYPE_PARTNER_NUM = 12,      --激活X个伙伴
    TYPE_PARTNER_COLOR_NUM = 13,--X个Y色伙伴
    TYPE_PARTNER_LV = 14,       --X个伙伴等级达到Y级(Y=玩家等级)
    TYPE_PARTNER_UP_LV = 15,    --X个伙伴晋升到Y级(Y=玩家等级可晋升最高级)
    TYPE_ROLE_EQUIP_STONE = 16, --镶嵌X颗Y级宝石
    TYPE_ROLE_EQUIP_INTENSIFY = 17, --X个装备强化等级达到Y级
    TYPE_PET_STAMP_INTENSIFY = 18,  --X个神印升到Y级
    TYPE_ACTIVE_DAILY = 19,     --活跃日常，p1:活跃里的条件类型
    TYPE_ChallengeTest = 20,    --轮回忘川
    TYPE_DeepDarkFantasy = 21,  --幽冥炼狱
    TYPE_ROLE_PROMOTE = 22,     --角色转职 p1:阶数
    TYPE_ACTIVE_TYPE = 23,      --活跃限时活动类型，p1:活动类型
    TYPE_CONSPIRACY = 24,       --远征魔冢

    TYPE_JUE_ZHAO = 25,         --绝招X级
    TYPE_ZHAO_HUA = 26,         --穿戴X件Y装等造化装备
    TYPE_EQUIP_INTENSIFY = 27,  --X件装备强化到Y
    TYPE_ROLE_TITLE = 28,       --角色进阶到X称号
    TYPE_PEI_SHI= 29,           --穿戴X件Y品质配饰
    TYPE_PARTNER_INTENSIFY = 30,--X个伙伴强化到Y

    TYPE_EQUIP_DUAN_ZHAO = 31,  --装备新增装备锻造，X件Y级Z品质装备
    TYPE_PET_LEVEL = 32,        --携带X个Y级宠物
    TYPE_PET_QUA = 33,          --获得X个Y品质宠物
}

BianQiangGroupId = {
    TYPE_ROLE_UP = 1, --角色提升
    TYPE_PET_UP = 2, --宠物提升
    TYPE_PARTNER_UP = 3, --伙伴提升
    TYPE_GET_EXP = 4, --获得经验
    TYPE_GET_LINGYU = 5, --获得灵玉
    TYPE_GET_COIN = 6, --获得铜币
    TYPE_GET_XIAYI = 7 --获得侠义
}

--不显示状态的任务(急需提升那个)
BianQiangNoShowState = {
    [BianQiangType.TYPE_NONE] = 1,
    [BianQiangType.TYPE_ACTIVE_DAILY] = 1,
    [BianQiangType.TYPE_ChallengeTest] = 1,
    [BianQiangType.TYPE_DeepDarkFantasy] = 1,
    [BianQiangType.TYPE_ACTIVE_TYPE] = 1,
    [BianQiangType.TYPE_CONSPIRACY] = 1,
}
--不显示进度条的任务
BianQiangNoShowProg = {
    [BianQiangType.TYPE_NONE] = 1,
    [BianQiangType.TYPE_ACTIVE_TYPE] = 1,
}

--任务进度，return 最大值，当前值
local BQProgressFunc = {
    [BianQiangType.TYPE_ROLE_SKILL_LV_1] = function(task_cfg)
        local max_num, match_num = RoleSkillData.Instance:GetBianQiangProfSkillLevel(task_cfg.param1)
        return max_num, match_num
    end,
    [BianQiangType.TYPE_ROLE_SKILL_LV_2] = function(task_cfg)
        local max_num, match_num = RoleSkillData.Instance:GetBianQiangShowSkillLevel(task_cfg.param2)
        return task_cfg.param1, match_num
    end,
    [BianQiangType.TYPE_ROLE_SKILL_LV_3] = function(task_cfg)
        local max_num, match_num = RoleSkillData.Instance:GetBianQiangAssistSkillLevel(task_cfg.param2)
        return task_cfg.param1, match_num
    end,
    [BianQiangType.TYPE_ROLE_EQUIP_COLOR_1] = function(task_cfg)
        -- p1 数量 p2 普装品质 p3 神装品质 p4 神装阶数
        local match_num = 0
        for i, v in pairs(BagData.Instance:GetEquipList()) do
            if v:IsStarGod() then
                if v:Color() >= (task_cfg.param3 or 1) and v:StrengthenLevel() >= (task_cfg.param4 or 7) then
                    match_num = match_num + 1
                end
            else
                if v:Color() >= task_cfg.param2 and v:Level() >= task_cfg.level_min then
                    match_num = match_num + 1
                end
            end
        end
        return task_cfg.param1, match_num
    end,
    [BianQiangType.TYPE_ROLE_EQUIP_COLOR_2] = function(task_cfg)
        local match_num = 0
        for i, v in pairs(BagData.Instance:GetJewelryList()) do
            if v:IsStarGod() then
                if v:Color() >= (task_cfg.param3 or 1) and v:StrengthenLevel() >= (task_cfg.param4 or 7) then
                    match_num = match_num + 1
                end
            else
                if v:Color() >= task_cfg.param2 and v:Level() >= task_cfg.level_min then
                    match_num = match_num + 1
                end
            end
        end
        return task_cfg.param1, match_num
    end,
    [BianQiangType.TYPE_FABAO_LV] = function(task_cfg)
        local max_num, match_num = FabaoData.Instance:GetBianQiangFaBaoLevel(task_cfg.param2)
        return task_cfg.param1, match_num
    end,
    [BianQiangType.TYPE_TUJIAN_ACTIVED] = function(task_cfg)
        local max_num, match_num = IllustrateData.Instance:GetBianQiangIllustrateActived()
        return task_cfg.param1, match_num
    end,
    [BianQiangType.TYPE_USED_TOKEN_LV] = function(task_cfg)
        local max_level = 0
        for i, v in pairs(BagData.Instance:GetMedalList()) do
            if v.item_id ~= 0 then
                local item_cfg = MedalData.Instance:GetMedalSpItemCfg(v.item_id)
                max_level = math.max(max_level, v.param.grade)
            end
        end
        return task_cfg.param1, max_level
    end,
    [BianQiangType.TYPE_PET_LV] = function(task_cfg)
        local match_num = 0
        for i, v in pairs(PetData.Instance:GetPetList()) do
            if v:Level() >= RoleData.Instance:GetRoleLevel() then
                match_num = match_num + 1
            end
        end
        return task_cfg.param1, match_num
    end,
    [BianQiangType.TYPE_PET_SKILL_NUM] = function(task_cfg)
        local match_num = 0
        local flags = {[2] = true, [3] = true}
        for i, v in pairs(PetData.Instance:GetPetList()) do
            if v:GiftMax() == v:GetSkillNumByFlags(flags) then
                match_num = match_num + 1
            end
        end
        for i, v in pairs(PetData.Instance:GetStorageList()) do
            if v:GiftMax() == v:GetSkillNumByFlags(flags) then
                match_num = match_num + 1
            end
        end
        return task_cfg.param1, match_num
    end,
    [BianQiangType.TYPE_PET_COLOR_NUM] = function(task_cfg)
        local match_num = 0
        for i, v in pairs(PetData.Instance:GetPetList()) do
            if v:Quality() >= 4 then
                match_num = match_num + 1
            end
        end
        for i, v in pairs(PetData.Instance:GetStorageList()) do
            if v:Quality() >= 4 then
                match_num = match_num + 1
            end
        end
        return task_cfg.param1, match_num
    end,
    [BianQiangType.TYPE_PARTNER_NUM] = function(task_cfg)
        local match_num = 0
        for i, v in pairs(PartnerData.Instance:GetPartnerAllInfoLockSort()) do
            if v:IsUnlocked() then
                match_num = match_num + 1
            end
        end
        return task_cfg.param1, match_num
    end,
    [BianQiangType.TYPE_PARTNER_COLOR_NUM] = function(task_cfg)
        local match_num = 0
        for i, v in pairs(PartnerData.Instance:GetPartnerAllInfoLockSort()) do
            if v:IsUnlocked() and v:Quality() >= task_cfg.param2 then
                match_num = match_num + 1
            end
        end
        return task_cfg.param1, match_num
    end,
    [BianQiangType.TYPE_PARTNER_LV] = function(task_cfg)
        local match_num = 0
        for i, v in pairs(PartnerData.Instance:GetPartnerAllInfoLockSort()) do
            if v:IsUnlocked() and v:Level() >= RoleData.Instance:GetRoleLevel() then
                match_num = match_num + 1
            end
        end
        return task_cfg.param1, match_num
    end,
    [BianQiangType.TYPE_PARTNER_UP_LV] = function(task_cfg)
        local match_num = 0
        for i, v in pairs(PartnerData.Instance:GetPartnerAllInfoLockSort()) do
            if v:IsUnlocked() and v.vo.up_level >= task_cfg.param2 then
                match_num = match_num + 1
            end
        end
        return task_cfg.param1, match_num
    end,
    [BianQiangType.TYPE_ROLE_EQUIP_STONE] = function(task_cfg)
        local match_num = 0
        for i, v in pairs(BagData.Instance:GetEquipList()) do
            local stone = v:Stone()
            for m = 1, EQUIP_STONE_COUNT_MAX do
                local stone_info = WorkshopData.Instance:GetStoneInfo(stone[m])
                if stone_info ~= nil and stone_info.level >= task_cfg.param2 then
                    match_num = match_num + 1
                end
            end
        end
        for i, v in pairs(BagData.Instance:GetJewelryList()) do
            local stone = v:Stone()
            for m = 1, EQUIP_STONE_COUNT_MAX do
                local stone_info = WorkshopData.Instance:GetStoneInfo(stone[m])
                if stone_info ~= nil and stone_info.level >= task_cfg.param2 then
                    match_num = match_num + 1
                end
            end
        end
        return task_cfg.param1, match_num
    end,
    [BianQiangType.TYPE_ROLE_EQUIP_INTENSIFY] = function(task_cfg)
        local match_num = 0
        for i, v in pairs(BagData.Instance:GetEquipList()) do
            if BagData.Instance:GetEquipSortUpgrade(v:EquipType()) >= task_cfg.param2 then
                match_num = match_num + 1
            end
        end
        for i, v in pairs(BagData.Instance:GetJewelryList()) do
            if BagData.Instance:GetEquipSortUpgrade(v:EquipType()) >= task_cfg.param2 then
                match_num = match_num + 1
            end
        end
        return task_cfg.param1, match_num
    end,
    [BianQiangType.TYPE_PET_STAMP_INTENSIFY] = function(task_cfg)
        local match_num = 0
        for i, v in pairs(PetStampData.Instance:GetPetStampList()) do
            if v:Level() >= task_cfg.param2 then
                match_num = match_num + 1
            end
        end
        return task_cfg.param1, match_num
    end,
    [BianQiangType.TYPE_ACTIVE_DAILY] = function(task_cfg)
        for i, v in pairs(ActiveData.Instance:CheckDailyData()) do
            if v.act_type == task_cfg.param1 then
                if v.need_count == 0 then
                    local act_info = ActivityData.Instance:GetActivityInfo(v.act_type)
                    if act_info ~= nil then
                        local cur_val, max_val = act_info:GetTimesProg()
                        return max_val or 1, cur_val or 1
                    end
                else
                    return ActiveData.Instance:GetDailyNeedCount(v), ActivityData.Instance:GetProgressNum(v.type)
                end
            end
        end
        return 1, 1
    end,
    [BianQiangType.TYPE_ChallengeTest] = function(task_cfg)
        local num = ChallengeTestCtrl.Instance:Data():GetChallengeTestPassLayer()
        local total = ChallengeTestCtrl.Instance:Data():GetChallengeTestMaxLayer()
        return total, num
    end,
    [BianQiangType.TYPE_DeepDarkFantasy] = function(task_cfg)
        local num = DeepDarkFantasyData.Instance.challenge_data.pass
        local total = DeepDarkFantasyData.Instance:CheckUnlockNum()
        return total, num
    end,

    [BianQiangType.TYPE_JUE_ZHAO] = function (task_cfg)
        local num = FlyUpData.Instance:GetSkillLevel()
        local skill_list = FlyUpData.Instance:GetSkillList() or {}
        --local total = #skill_list
        local total = task_cfg.param1
        return total,num
    end,
    [BianQiangType.TYPE_ZHAO_HUA] = function (task_cfg)
        local num = 0
        local t_level = task_cfg.param2
        for k,v in pairs(MarbasData.Instance.equip_list) do
            if v.item_id and v.item_id > 0 and v:LevelShow() >= t_level then
                num = num + 1
            end
        end 
        return task_cfg.param1,num
    end,
    [BianQiangType.TYPE_EQUIP_INTENSIFY] = function (task_cfg)
        local t_level = task_cfg.param2
        local num = 0
        for i = 0, 7 do
            local level = BagData.Instance:GetEquipSortUpgrade(i)
            if level >= t_level then
                num = num + 1
            end
        end
        return task_cfg.param1,num
    end,
    [BianQiangType.TYPE_ROLE_TITLE] = function (task_cfg)
        return task_cfg.param1,RolePromoteData.Instance:GetProfGrade()
    end,
    [BianQiangType.TYPE_PEI_SHI] = function (task_cfg)
        local num = 0
        local t_qua = task_cfg.param2
        for i = 1, 5 do
            local item = AccrssoryData.Instance.accrssory_equip_data.info[i] or {}
            if item.item_id and item.item_id > 0 and item:Color() >= t_qua then
                num = num + 1
            end
        end
        return task_cfg.param1,num
    end,
    [BianQiangType.TYPE_PARTNER_INTENSIFY] = function (task_cfg)
        local num = 0
        local t_level = task_cfg.param2
        for i,v in pairs(PartnerData.Instance.partner_list) do
            if v.vo.intensify_level and v.vo.intensify_level >= t_level then
                num = num + 1
            end
        end
        return task_cfg.param1,num
    end,

    [BianQiangType.TYPE_EQUIP_DUAN_ZHAO] = function (task_cfg)
        local num = 0
        local t_level = task_cfg.param2
        local t_qua = task_cfg.param3
        for i = 1, 7 do
            local equip = BagData.Instance:GetEquipByType(i)
            if equip and equip:Level() >= t_level and equip:Color() >= t_qua then
                num = num + 1
            end
        end
        return task_cfg.param1,num
    end,
    [BianQiangType.TYPE_PET_LEVEL] = function (task_cfg)
        local pet_list = PetData.Instance:GetPetList()
        local num = 0
        local t_level = task_cfg.param2
        for i,v in pairs(pet_list) do
            local state = v:State()
            if (state == PetState.Fight or state == PetState.StandBy) and v:Level() >= t_level then
                num = num + 1
            end
        end
        return task_cfg.param1,num
    end,
    [BianQiangType.TYPE_PET_QUA] = function (task_cfg)
        local pet_list = PetData.Instance:GetPetList()
        local num = 0
        local t_qua = task_cfg.param2
        for i,v in pairs(pet_list) do
            local state = v:State()
            if v:Quality() >= t_qua then
                num = num + 1
            end
        end
        for i, v in pairs(PetData.Instance:GetStorageList()) do
            if v:Quality() >= t_qua then
                num = num + 1
            end
        end
        return task_cfg.param1,num
    end,
}
BQProgressFunc[BianQiangType.TYPE_ROLE_PROMOTE] = function(task_cfg)
    return 1, 0
end
BQProgressFunc[BianQiangType.TYPE_ACTIVE_TYPE] = function(task_cfg)
    --已参与过并且活动结束了，后置
    if ActiveData.Instance:GetSignUpFlag(task_cfg.param1) or ActiveData.Instance:IsHadSignUpFlag(task_cfg.param1) then
        if not ActivityData.IsOpen(task_cfg.param1) then
            return 1, 1 
        end
    end
    return 1, 0
end
BQProgressFunc[BianQiangType.TYPE_CONSPIRACY] = function(task_cfg)
    local num = ConspiracyData.Instance:WeeklySurplus()
    local total = ConspiracyData.Instance:WeeklyTotal()
    return total, total - num
end

function BianQiangData:__init()
    if BianQiangData.Instance ~= nil then
        Debuger.LogError("[BianQiangData] attempt to create singleton twice!")
        return
    end
    BianQiangData.Instance = self

    self.auto_cfg = Config.bianqiang_cfg_auto
    self:InitCfg()
    self.list_data = SmartData.New({
        JueSeKeFeiSheng = false,
        JueSeQianNengDian = false,
        ZhiYeJinJie = false,
        XiuLianJiNeng = false,
        ZhuangBeiKeTiHuang = false,
        BaoShiKeXiangQian = false,
        JiNengKeJieSuo = false,
        JiNengKeShengJi = false,
        JueZhaoKeShengJi = false,
        XiuWeiKeTiSheng = false,
        ChongWuKeFeiSheng = false,
        ChongWuKeShengJi = false,
        ChongWuQianNengDian = false,
        ChongWuTuPo = false,
        HuoBanKeFeiSheng = false,
        HuoBanKeShengJi = false,
        HuoBanKeJinSheng = false,
        HuoBanKeJueXing = false,
        JinRiTongJi = false,
        WoYaoBianQiang = false,
    })
    self.income_info = SmartData.New({})

    self.selected_task = nil
end

function BianQiangData:__delete()
    if BianQiangData.Instance == self then
        BianQiangData.Instance = nil
    end
end
function BianQiangData:ListInfo()
    return self.list_info
end
function BianQiangData:ClearData()
end

function BianQiangData:InitCfg()
    --标签组
    self.total_tab_list = self.auto_cfg.tabs
    self.group_tab_list = {}
    for i, v in ipairs(self.total_tab_list) do
        self.group_tab_list[v.tab_index] = self.group_tab_list[v.tab_index] or {}
        table.insert(self.group_tab_list[v.tab_index], v)
    end

    --等级组 group_task_list[group_id][type] = {lv1, lv2}
    self.group_task_list = {}
    for _, group in pairs(self.total_tab_list) do
        local page_list = self.auto_cfg["group_" .. group.group_id]
        if page_list ~= nil then
            local new_list = {}
            for i, v in pairs(page_list) do
                new_list[v.type] = new_list[v.type] or {}
                table.insert(new_list[v.type], v)
            end
            for i, list in pairs(new_list) do
                table.sort(list, DataSorter.KeyLowerSorter("level_min"))
            end
            self.group_task_list[group.group_id] = new_list
        end
    end
    self.list_info = self.auto_cfg.list
    
    self.list_keys = {}
    for key, value in pairs(BianQiangData.ListType) do
        self.list_keys[value] = key
    end

    self.finish_btn_keys = {}
    for key, value in pairs(BianQiangData.FinishBtnType) do
        self.finish_btn_keys[value] = key
    end
end

function BianQiangData:OnInit()
    self.DescFunc = {
        [BianQiangType.TYPE_ROLE_SKILL_LV_1] = function(task_cfg)
            return string.format(task_cfg.desc, RoleData.Instance:GetRoleLevel())
        end,
        [BianQiangType.TYPE_PET_LV] = function(task_cfg)
            return string.format(task_cfg.desc, RoleData.Instance:GetRoleLevel())
        end,
        [BianQiangType.TYPE_PARTNER_LV] = function(task_cfg)
            return string.format(task_cfg.desc, RoleData.Instance:GetRoleLevel())
        end,
        [BianQiangType.TYPE_PARTNER_UP_LV] = function(task_cfg)
            return string.format(task_cfg.desc, RoleData.Instance:GetRoleLevel())
        end
    }
    
    BagData.Instance.item_grid:Care(self:BindFlush(BianQiangData.ListType.JueSeKeFeiSheng))
    FlyUpData.Instance:CareInfoDataFlyFlagChange(self:BindFlush(BianQiangData.ListType.JueSeKeFeiSheng))
    BagData.Instance.item_grid:Care(self:BindFlush(BianQiangData.ListType.BaoShiKeXiangQian))
    -- RoleData.Instance.base_data:Care(self:BindFlush(BianQiangData.ListType.BaoShiKeXiangQian))
    SkillData.Instance.change_data:Care(self:BindFlush(BianQiangData.ListType.JiNengKeJieSuo))
    SkillData.Instance.change_data:Care(self:BindFlush(BianQiangData.ListType.JiNengKeShengJi))
    BagData.Instance.item_grid:Care(self:BindFlush(BianQiangData.ListType.JueZhaoKeShengJi))
    RoleData.Instance.base_data:Care(self:BindFlush(BianQiangData.ListType.JueZhaoKeShengJi), "level")
    FlyUpData.Instance.red_piont_data:Care(self:BindFlush(BianQiangData.ListType.JueZhaoKeShengJi))
    BagData.Instance.item_grid:Care(self:BindFlush(BianQiangData.ListType.XiuWeiKeTiSheng))
    RoleData.Instance.base_data:Care(self:BindFlush(BianQiangData.ListType.XiuWeiKeTiSheng), "level")
    PetData.Instance:GetPetList():Care(self:BindFlush(BianQiangData.ListType.ChongWuQianNengDian))
    PetData.Instance:GetPetList():Care(self:BindFlush(BianQiangData.ListType.ChongWuKeFeiSheng))
    PetData.Instance:GetPetList():Care(self:BindFlush(BianQiangData.ListType.ChongWuKeShengJi))
    BagData.Instance:ListenItems({27577},self:BindFlush(BianQiangData.ListType.ChongWuTuPo))
    PetData.Instance:GetPetList():Care(self:BindFlush(BianQiangData.ListType.ChongWuTuPo))
    -- 注册所有列表
    self:RegisterUpgradeRemind(BianQiangData.ListType.JueSeKeFeiSheng, false)
    self:RegisterUpgradeRemind(BianQiangData.ListType.JueSeQianNengDian, false)
    self:RegisterUpgradeRemind(BianQiangData.ListType.ZhiYeJinJie, false)
    self:RegisterUpgradeRemind(BianQiangData.ListType.XiuLianJiNeng, false)
    self:RegisterUpgradeRemind(BianQiangData.ListType.ZhuangBeiKeTiHuang, false)
    self:RegisterUpgradeRemind(BianQiangData.ListType.BaoShiKeXiangQian, false)
    self:RegisterUpgradeRemind(BianQiangData.ListType.JiNengKeJieSuo, false)
    self:RegisterUpgradeRemind(BianQiangData.ListType.JiNengKeShengJi, false)
    self:RegisterUpgradeRemind(BianQiangData.ListType.JueZhaoKeShengJi, false)
    self:RegisterUpgradeRemind(BianQiangData.ListType.XiuWeiKeTiSheng, false)
    --宠物
    self:RegisterUpgradeRemind(BianQiangData.ListType.ChongWuKeFeiSheng, false)
    self:RegisterUpgradeRemind(BianQiangData.ListType.ChongWuKeShengJi, false)
    self:RegisterUpgradeRemind(BianQiangData.ListType.ChongWuQianNengDian, false)
    self:RegisterUpgradeRemind(BianQiangData.ListType.ChongWuTuPo, false)
    --伙伴
    self:RegisterUpgradeRemind(BianQiangData.ListType.HuoBanKeFeiSheng, false)
    self:RegisterUpgradeRemind(BianQiangData.ListType.HuoBanKeShengJi, false)
    self:RegisterUpgradeRemind(BianQiangData.ListType.HuoBanKeJinSheng, false)
    self:RegisterUpgradeRemind(BianQiangData.ListType.HuoBanKeJueXing, false)

    --self:RegisterUpgradeRemind(BianQiangData.ListType.JinRiTongJi, true)
    self:RegisterUpgradeRemind(BianQiangData.ListType.WoYaoBianQiang, true)
end
function BianQiangData:BindFlush(type)
    return BindTool.Bind(self.FlushListRemind, self, type)
end
function BianQiangData:FlushListRemind(listType)
    self.list_data:SetDirty(self.list_keys[listType])
end
function BianQiangData:RegisterUpgradeRemind(type, init_call)
    MainViewData.Instance:RegisterUpgradeRemind(type, self.list_data, BindTool.Bind(self[self.list_keys[type]], self), BindTool.Bind(self.ListButtonClick,self, type), init_call, self.list_keys[type])
end
function BianQiangData:ListButtonClick(type)
    -- LogDG(type)
    local config = self:ListInfo()[type]
    local func_name = (self.list_keys[type] or "") .. "RemindClick"
    if self[func_name] ~= nil then
        self[func_name]()
    end
    if config and config.open_panel ~= "" and config.open_panel ~= 0 then
        ViewMgr:OpenViewByKey(config.open_panel)
    else
        ViewMgr:OpenViewByKey(Mod.BianQiang.Main)
    end
end
function BianQiangData:FinishButtonClick(type)
    local func_name = (self.finish_btn_keys[type] or "").. "RemindClick"
    if self[func_name] ~= nil then
        self[func_name]()
    end
end
function BianQiangData:GetRemindFinishBtnType(type)
    local func_name = self.finish_btn_keys[type]
    if func_name and self[func_name] ~= nil then
        return self[func_name]()
    end
    return MainViewUpgradeRemindType.Remove
end
function BianQiangData:JueSeKeFeiSheng()
    return FlyUpData.Instance:GetRedPiontNum() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end

function BianQiangData:JueSeQianNengDian()
    return RoleAddPointData.Instance:GetRedAddPiontNum() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end
function BianQiangData:ZhiYeJinJie()
    return RolePromoteData.Instance:GetPromoteNum() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end

function BianQiangData:XiuLianJiNeng()
    return RoleSkillData.Instance:GetXiulianSkillRemind() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end

function BianQiangData:ZhuangBeiKeTiHuang()
    return BagData.Instance.better_equip.is_better == true and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end

function BianQiangData:BaoShiKeXiangQian()
    return WorkshopData.Instance:CheckWorkInset() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end
function BianQiangData:JiNengKeJieSuo()
    return SkillData.Instance:CacuRemindCount() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end
function BianQiangData:JiNengKeShengJi()
    return SkillData.Instance:SkillCanUpgradeRemind() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end
function BianQiangData:JueZhaoKeShengJi()
    return FlyUpData.Instance:GetRoleFlyUpNum() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end
function BianQiangData:XiuWeiKeTiSheng()
    return RoleXiuWeiData.Instance:GetRemind() > 0  and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end

function BianQiangData:ChongWuKeFeiSheng()
    return PetData.Instance:CanFlyup(true) > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end
function BianQiangData:ChongWuKeFeiShengRemindClick()
    local num, pet = PetData.Instance:CanFlyup(true)
    if num > 0 and pet ~= nil then
        PetData.Instance:SetSelect(pet.index) 
    end
end

function BianQiangData:ChongWuKeShengJi()
    return PetData.Instance:CheckAllUpgradePointRemind() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end
function BianQiangData:ChongWuKeShengJiRemindClick()
    local num, pet = PetData.Instance:CheckAllUpgradePointRemind()
    if num > 0 and pet ~= nil then
        PetData.Instance:SetSelect(pet.index) 
    end
end

function BianQiangData:ChongWuQianNengDian()
    return PetData.Instance:CheckPetPointRemind() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end
function BianQiangData:ChongWuQianNengDianRemindClick()
    local num, pet = PetData.Instance:CheckPetPointRemind()
    if num > 0 and pet ~= nil then
        PetData.Instance:SetSelect(pet.index) 
    end
end

function BianQiangData:ChongWuTuPo()
    local num = PetData.Instance:CheckBreachRemind(true, true)
    return num > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end
function BianQiangData:ChongWuTuPoRemindClick()
    local num, pet = PetData.Instance:CheckBreachRemind(true, true)
    if num > 0 and pet ~= nil then
        PetData.Instance:SetSelect(pet.index) 
    end
end

function BianQiangData:HuoBanKeFeiSheng()
    return PartnerData.Instance:FightPartnerSoarRedPoint() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end
function BianQiangData:HuoBanKeFeiShengRemindClick()
    local num, partner = PartnerData.Instance:FightPartnerSoarRedPoint()
    if num > 0 and partner ~= nil then
        PartnerData.Instance:PartnerShowListJump(true)
        PartnerData.Instance.guide_partner = partner:Id()
    end
end

function BianQiangData:HuoBanKeShengJi()
    return PartnerData.Instance:FightPartnerAttrLevelRedPoint() >  0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end
function BianQiangData:HuoBanKeShengJiRemindClick()
    local num, partner = PartnerData.Instance:FightPartnerAttrLevelRedPoint()
    if num > 0 and partner ~= nil then
        PartnerData.Instance:PartnerShowListJump(true)
        PartnerData.Instance.guide_partner = partner:Id()
    end
end

function BianQiangData:HuoBanKeJinSheng()
    return PartnerData.Instance:FightPartnerUpRedPoint() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end
function BianQiangData:HuoBanKeJinShengRemindClick()
    local num, partner = PartnerData.Instance:FightPartnerUpRedPoint()
    if num > 0 and partner ~= nil then
        PartnerData.Instance:PartnerShowListJump(true)
        PartnerData.Instance.guide_partner = partner:Id()
    end
end

function BianQiangData:HuoBanKeJueXing()
    return PartnerData.Instance:FightPartnerQuaRedPoint() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end
function BianQiangData:HuoBanKeJueXingRemindClick()
    local num, partner = PartnerData.Instance:FightPartnerQuaRedPoint()
    if num > 0 and partner ~= nil then
        PartnerData.Instance:PartnerShowListJump(true)
        PartnerData.Instance.guide_partner = partner:Id()
    end
end

function BianQiangData:JinRiTongJi()
    return MainViewUpgradeRemindType.WithoutRedPoint
end

function BianQiangData:WoYaoBianQiang()
    return MainViewUpgradeRemindType.WithoutRedPoint
end
--注意GetGroupNum 和 GetRemindTotalNum会导致sm_data刷新，因为结算按钮不需要即时刷新
--所以直接使用，如果变强要用的就不能这么使用,变强是带刷新的，可能导致死循环
function BianQiangData:XinFaTiSheng()
    return Remind.Instance:GetRemindNum(Mod.Role.RoleHeartSkill) > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end
function BianQiangData:LingQiTiSheng()
    return Remind.Instance:GetGroupNum(Mod.Bicycle) > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end
function BianQiangData:JingJieTiSheng()
    return RoleData.Instance:BreakRemind() == true and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end
function BianQiangData:ZhuangBeiQiangHua()
    return WorkshopData.Instance:CheckUpgradeRemind() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end
function BianQiangData:ShenZhuangPeiYang()
    return Remind.Instance:GetGroupNum(Mod.DivineCostume) > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end
function BianQiangData:ShengQiPeiYang()
    return ShengQiData.Instance:Remind() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end
function BianQiangData:ZaoHuaZhuangBei()
    return Remind.Instance:GetGroupNum(Mod.Marbas) > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
end

function BianQiangData:GetGroupList(tab_index)
    return self.group_tab_list[tab_index] or {}
end

--打开界面自动选中对应标签页
function BianQiangData:SetGroupId(group_id)
    self.selected_group_id = group_id
end
function BianQiangData:GetGroupId()
    return self.selected_group_id
end
function BianQiangData:GetSelecteTab()
    if self.selected_group_id ~= nil then
        for i, v in pairs(self.total_tab_list) do
            if v.group_id == self.selected_group_id then
                return v.tab_index
            end
        end
    end
    return 1
end

function BianQiangData:GetTaskProgress(task_cfg)
    if BQProgressFunc[task_cfg.condition] ~= nil then
        return BQProgressFunc[task_cfg.condition](task_cfg)
    end
    return 1, 1
end
function BianQiangData:GetTaskProgressVal(task_cfg, max_val, cur_val)
    if task_cfg.score_4 == 0 then --防止意外置后
        return cur_val >= max_val and 4 or 1
    end
    local index = 1
    for i = 1, 4 do
        if cur_val < task_cfg["score_" .. i] then
            break
        end
        index = i
    end
    return index
end

function BianQiangData:GetTaskDesc(task_cfg)
    if self.DescFunc[task_cfg.condition] ~= nil then
        return self.DescFunc[task_cfg.condition](task_cfg)
    end
    return task_cfg.desc
end

function BianQiangData:OnRoleDataChange()
    self.cur_group_task_list = {}
end
function BianQiangData:OnDayChange()
    self.cur_group_task_list = {}
    self.income_info:Clear()
end

--找到某项任务当前等级段配置  当为true放入该任务
local BQFuncPush = {
    [BianQiangType.TYPE_NONE] = function(task_cfg)
        local fly_level = FlyUpData.Instance:GetStage() or 0
        if task_cfg.is_feisheng and fly_level < task_cfg.is_feisheng then
            return false
        end
        local role_level = RoleData.Instance:GetRoleLevel()
        if task_cfg.level_min <= role_level and role_level <= task_cfg.level_max then
            return true
        end
    end,
    [BianQiangType.TYPE_ROLE_PROMOTE] = function(task_cfg)
        local role_level = RoleData.Instance:GetRoleLevel()
        if task_cfg.level_min <= role_level and role_level <= task_cfg.level_max then
            local prof = RoleData.Instance:ProfStage()
            local order = RolePromoteData.Instance:GetProfGrade()
            if (prof < task_cfg.param1) or (prof == task_cfg.param1 and order < task_cfg.param2) then
                return true
            end
        end
    end,
    [BianQiangType.TYPE_ACTIVE_TYPE] = function(task_cfg)
        local role_level = RoleData.Instance:GetRoleLevel()
        if task_cfg.level_min <= role_level and role_level <= task_cfg.level_max then
            local act_info = ActivityData.Instance:GetActivityInfo(task_cfg.param1)
            return act_info and act_info:IsToday() or false
        end
    end
}
function BianQiangData:GetLevelTask(task_list)
    if task_list == nil or task_list[1] == nil then
        return nil
    end
    local func_check = BQFuncPush[task_list[1].condition] or BQFuncPush[BianQiangType.TYPE_NONE]
    for i, v in ipairs(task_list) do
        if func_check(v) then
            return v
        end
    end
end

function BianQiangData:GetCurGroupTaskList(group_id)
    -- return CacheList(self, "cur_group_task_list", group_id, function()
    --     local show_list = {}
    --     for i, level_list in pairs(self.group_task_list[group_id] or {}) do
    --         local task = self:GetLevelTask(level_list)
    --         if task ~= nil then
    --             table.insert(show_list, task)
    --         end
    --     end
    --     return show_list
    -- end)

    -- 等级会变化，不缓存
    local show_list = {}
    for i, level_list in pairs(self.group_task_list[group_id] or {}) do
        local task = self:GetLevelTask(level_list)
        if task ~= nil then
            table.insert(show_list, task)
        end
    end
    return show_list
end
--获取当前等级任务列表
function BianQiangData:GetTaskList(group_id)
    local task_list = self:GetCurGroupTaskList(group_id)
    table.sort(task_list, DataHelper.WeightSort("type", function(cfg)
        local add_val = 0
        if cfg.condition ~= 0 then
            local max_val, cur_val = self:GetTaskProgress(cfg)
            if self:GetTaskProgressVal(cfg, max_val, cur_val) == 4 then    --登峰造极显示最后
                add_val = add_val + 1000
            end
        end
        return add_val
    end))
    return task_list
end

--今日统计
function BianQiangData:SetIncomeRoleInfo(protocol)
    self.income_info.role_count = protocol.role_count
    self.income_info.role_param_list = protocol.role_param_list
    self.income_info.global_count = protocol.global_count
    self.income_info.global_param_list = protocol.global_param_list
end

function BianQiangData:GetIncomeStageCfg(cur_val, max_val)
    local contrast = cur_val / max_val * 100
    for i,v in ipairs(self.auto_cfg.statistics) do
        if contrast <= v.contrast then
            return v
        end
    end
    return self.auto_cfg.statistics[#self.auto_cfg.statistics]
end

function BianQiangData:GetIncomeInfo()
    -- return {role_count = 3, global_count = 3,
    --     role_param_list = {[0] = 0, [1] = 100, [2] = 100},
    --     global_param_list = {[0] = 150, [1] = 200, [2] = 3000},
    -- }
    return self.income_info
end

--今日统计的获取
function BianQiangData:GetGainTypeList(gain_type)
    return CacheList(self, "gain_all_list", gain_type, function()
        local show_list = {}
        for i, v in ipairs(self.auto_cfg.goods_gain) do
            if v.gain_type == gain_type then
                table.insert(show_list, v)
            end
        end
        return show_list
    end)
end

function BianQiangData:GetGainShowList(gain_type)
    local base_list = self:GetGainTypeList(gain_type)
    local show_list = {}
    --判断显示
    for i, v in pairs(base_list) do
        local is_push = true
        if v.open_type == 1 then
            local mod_key = tonumber(v.open_param) or 0
            if mod_key > 0 and not FunOpen.Instance:GetFunIsOpened(mod_key) then
                is_push = false
            end
        elseif v.open_type == 2 then
            local act_type = tonumber(v.open_param) or 0
            if not ActivityData.IsEnoughLevel(act_type) then
                is_push = false
            end
        end
        if is_push and v.condition == BianQiangType.TYPE_ACTIVE_TYPE then
            local act_info = ActivityData.Instance:GetActivityInfo(v.param1)
            if act_info == nil or not act_info:IsToday() or not ActivityData.IsEnoughLevel(v.param1) then
                is_push = false
            end
        end
        if is_push then
            table.insert(show_list, v)
        end
    end
    --排序
    table.sort(show_list, DataHelper.WeightSort("order", function(cfg)
        local add_val = 0
        if cfg.condition ~= 0 then
            local max_val, cur_val = self:GetTaskProgress(cfg)
            if cur_val >= max_val then
                add_val = add_val + 1000
            end
        end
        return add_val
    end))
    return show_list
end
