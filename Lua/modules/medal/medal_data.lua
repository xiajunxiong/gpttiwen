MedalData = MedalData or BaseClass()

MedalData.OperaType = {
    Bag = "bag",
    Wear = "wear",
}

function MedalData:__init()
    if MedalData.Instance ~= nil then
        Debuger.LogError("[MedalData] attempt to create singleton twice!")
        return
    end
    MedalData.Instance = self
    -- upgrade
    self.cost_medal =  SmartData.New()
    self.NoticeInfo = SmartData.New({is_show = false ,info = nil})
    -- build
    self.medal_redtips = SmartData.New({mark_point = ""})
    self.medal_lottrey_info = {}
    self.medal_lottrey_flag = SmartData.New({is_show = false})
    -- smelt
    self.smelt_oper = SmartData.New({select = {}})

    self.upgrading_medal = {}
    self.effect_show_base_list = {}
    self.effect_type_all_list = {}
    self.build_reward_show_cfg = {}
    self.sp_change = SmartData.New({select_data = nil, target_data = nil,target_data_2 = nil})
    self.sp_change_notice = SmartData.New({is_show = false, info = nil})
    self.mix_change = SmartData.New({main_data = nil,select_data = nil, plus_data = nil})
    self:InitConfig()

    self.spmedal_marquee = {}
    self.cache = nil

    PublicPopupCtrl.Instance:AddInterceptRollingFunc(function(protocol)
        local list = Split(protocol.msg,";")
        if ViewMgr:IsOpen(MedalAtelierView) == true then
            if self.cache == nil then 
                self.cache = protocol.msg
            elseif self.cache == protocol.msg then 
                self.cache = nil 
                return true
            end 

            local role_id = RoleData.Instance:GetRoleId()

            if string.find(protocol.msg,tostring(role_id),1) ~= nil then
                MedalData.Instance:SetSpMedalMarqueeMsg(
                    {
                        str = RichTextHelper.ParseAll(protocol.msg, true, RTParseType.TopLayerMarquee),
                        delay_time = PublicPopupCtrl.Instance.top_layer_marquee_data:NeedDelay(protocol.msg),
                        msg = protocol.msg
                    }
                )
                return true
            end
        elseif string.find(protocol.msg,"medal_lottery",1) ~= nil then 
            -- 因为两者需要区分下划线故需要全部拦截
            -- 防止连续两次判断故做保险措施
            if self.cache == nil then 
                self.cache = protocol.msg
            elseif self.cache == protocol.msg then 
                self.cache = nil 
                return true
            end 
            local show = Split(RichTextHelper.ParseAll(protocol.msg, true, RTParseType.TopLayerMarquee)," ")
            PublicPopupCtrl.Instance:Marquee(show[1],PublicPopupCtrl.Instance.top_layer_marquee_data:NeedDelay(protocol.msg))
            ChatCtrl.Instance:ChannelChatCustom{content = protocol.msg}
            return true
        end
    end)
end 

function MedalData:InitTypeInfo(info)
    local vo = {}
    vo.sp_type = info.sp_type
    vo.icon_id = info.icon_id
    vo.icon_type = ICON_TYPE.BUFF
    vo.reward_name = info.reward_name
    vo.role_id = info.role_id
    vo.is_special = info.is_special
    return vo
end

--------------初始化-----------------------------
function MedalData:InitConfig()
    for k,v in pairs(Config.special_effect_auto.spe_list) do 
        if self.effect_show_base_list[v.sp_type] == nil then 
            if v.is_medal_effect == 1 then 
                local vo = self:InitTypeInfo(v)
                self.effect_show_base_list[v.sp_type] = vo
            end 
        end 

        if self.effect_type_all_list[v.sp_type] == nil then 
            self.effect_type_all_list[v.sp_type] = {}
        end  

        table.insert( self.effect_type_all_list[v.sp_type], v)
    end 

    self.medal_sp_show_cfg = Config.medal_cfg_auto.sp_show

    self.medal_build_level_config = KeyList(Config.medal_cfg_auto.medal_exchange,"seq")
    self.sp_group_cfg = KeyList(Config.medal_cfg_auto.sp_group,"sp_group")
    self.medal_rank_up = KeyList(Config.medal_cfg_auto.rank_up,"rank")
    self.medal_sp_item = KeyList(Config.medal_cfg_auto.sp_item_list,"item_id")
    self.medal_market_config = KeyList(Config.medal_cfg_auto.market_medal,"rank")
    
    FunOpen.Instance:Register(Mod.Medal.Build, BindTool.Bind(self.FlushMedalFunOpen, self))

    self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.RoleNoticeComplete, self))

    for k,v in pairs(Config.medal_cfg_auto.reward_show) do 
        if self.build_reward_show_cfg[v.job_type] == nil then 
            self.build_reward_show_cfg[v.job_type] = {}
        end 
        table.insert( self.build_reward_show_cfg[v.job_type],v )
    end

    self.sp_low_change_effect_list = {}
    self.sp_max_change_effect_list = {}
    self.sp_all_change_effect_list = {}
    for k,v in pairs(Config.special_effect_auto.spe_list) do 
        if v.is_medal_effect == 1 then 
            if v.sp_rank == 15 then
                if self.sp_low_change_effect_list[v.role_id] == nil then 
                    self.sp_low_change_effect_list[v.role_id] = {}
                end 
                table.insert(self.sp_low_change_effect_list[v.role_id],v)
            end 
            if v.sp_rank == 24 then 
                if self.sp_max_change_effect_list[v.role_id] == nil then 
                    self.sp_max_change_effect_list[v.role_id] = {}
                end 
                table.insert(self.sp_max_change_effect_list[v.role_id],v)
            end                 
            if self.sp_all_change_effect_list[v.role_id] == nil then 
                self.sp_all_change_effect_list[v.role_id] = {}
            end 
            if self.sp_all_change_effect_list[v.role_id][v.sp_rank] == nil then 
                self.sp_all_change_effect_list[v.role_id][v.sp_rank] = {}
            end 
            table.insert(self.sp_all_change_effect_list[v.role_id][v.sp_rank],v)
        end
    end 
end
--============================升华===============================
-- paramt bag_pos = {column , index} 背包位置
-- paramt wear_pos 身上穿着
function MedalData:SetOperateMedal(paramt)
    if paramt.wear_pos == nil then
        self.upgrading_medal = {
            oper_type = MedalData.OperaType.Bag,
            bag_pos = paramt.bag_pos
        }
    else 
        self.upgrading_medal = {
            oper_type = MedalData.OperaType.Wear,
            wear_pos = paramt.wear_pos
        }
    end 
end

function MedalData:GetMedalFulingMaterial()
    local list = {}
    for k,v in pairs( BagData.Instance:GetMedalList()) do 
        if v.param and v.param.sp_id == 0 then 
            v.param.is_equip = true
            table.insert(list,Item.Create(v))
        end 
    end 

    local bag = BagData.Instance:GetColumnGrids(ItemColumnType.Item)

    for k,v in pairs(bag) do 
        if v:IsMedal() and self:CheckCanBeCostFuling(v) then 
            v.param.is_equip = false
            table.insert( list,v)
        end 
    end 

    table.sort(list, function(a, b)
        if a.param.grade < b.param.grade then
            return true
        else
            return false
        end
    end)

    return list
end

function MedalData:CheckCanBeCostFuling(item)
    if item.param.grade == nil then return false end 

    return  item.param.sp_id == 0
end

function MedalData:GetMedalUpgradeMaterial()
    local bag = BagData.Instance:GetColumnGrids(ItemColumnType.Item)

    local list = {}
    for k,v in pairs(bag) do 
        if v:IsMedal() and self:CheckCanBeCostUpgrade(v) then 
            table.insert( list,v)
        end 
    end 

    -- 排序 按阶，特效种类id
    table.sort(list, function(a, b)
        local itemA = a.param.grade * 10000 + a.param.sp_id
        local itemB = b.param.grade * 10000 + b.param.sp_id
        if itemA > itemB then
            return true
        else
            return false
        end
    end)

    return list
end

function MedalData:CheckCanBeCostUpgrade(item)
    if item.param.grade == nil then return false end 
    local flag_oper = (self.upgrading_medal.oper_type == MedalData.OperaType.Bag and item.index ~= self.upgrading_medal.bag_pos.index) 
        or self.upgrading_medal.oper_type == MedalData.OperaType.Wear

    local opearta_item = self:GetOperateMedal()
    -- 此处的物品不应该缺失这个方法
    if opearta_item.GetMedalQua == nil or item.GetMedalQua == nil then return end 
    local flag_qua = opearta_item:GetMedalQua() == item:GetMedalQua()

    local flag_sp_id = true
    if opearta_item:WithSp() then 
        flag_sp_id = item:WithSp() and opearta_item:SpEffectCfg().sp_type == item:SpEffectCfg().sp_type
    else 
        flag_sp_id = not item:WithSp()
    end 

    return flag_oper and flag_qua -- and flag_sp_id
end

function MedalData:GetOperateMedal()
    if self.upgrading_medal.oper_type == MedalData.OperaType.Bag then 
        local bag = BagData.Instance:GetColumnGrids(ItemColumnType.Item)
        return bag[self.upgrading_medal.bag_pos.index]
    else 
        return Item.Create(BagData.Instance.view_data.medal_info[self.upgrading_medal.wear_pos])
    end 
end

function MedalData:GetOperMedalAttr() 
    local item = self:GetOperateMedal()
    local cur_cfg = self.medal_rank_up[item.param.grade]
    local next_cfg = self.medal_rank_up[item.param.grade + 1]

    local attr_list = {}
    for k,v in pairs(Language.Common.AttrList) do 
        if cur_cfg[AttrNameForType[k]] ~=nil and cur_cfg[AttrNameForType[k]] > 0 
            or ( cur_cfg[AttrNameForType[k]] == 0 and next_cfg ~= nil 
            and next_cfg[AttrNameForType[k]] ~=nil and next_cfg[AttrNameForType[k]] > 0 ) then 
            local vo = {}
            vo.attr_type = k
            vo.attr_value = cur_cfg[AttrNameForType[k]]
            vo.change = 0
            if next_cfg ~= nil and next_cfg[AttrNameForType[k]] ~= cur_cfg[AttrNameForType[k]] then 
                local change = next_cfg[AttrNameForType[k]] - cur_cfg[AttrNameForType[k]] 
                vo.change = change > 0 and change or 0
            end 

            table.insert( attr_list, vo)
        end 
    end 

    return attr_list
end 
function MedalData:GetOperMedalEffect()
    local item = self:GetOperateMedal()
    if not item:WithSp() then return {} end 
    local list = self:GetMedalEffectTypeAllList(item:SpEffectCfg().sp_type)

    local list_out = {}
    for k,v in pairs(list) do
        local vo = {}
        vo.cfg = v
        vo.sp_rank = v.sp_rank
        vo.is_dark = v.sp_rank > item.param.grade + 1 
        vo.is_cur = v.sp_rank == item.param.grade
        vo.is_pass = v.sp_rank < item.param.grade
        table.insert( list_out, vo) 
    end 
    table.sort(list_out, DataSorter.KeyLowerSorter("sp_rank"))
    return list_out
end
function MedalData:ClearUpgradeNotice()
    self.NoticeInfo.is_show = false
end

function MedalData:SetUpgradeNotice(data)
    self.NoticeInfo.Info = data
    self.NoticeInfo.is_show = true
end
function MedalData:SetSpecialExchangeNotice(protocol)
    self.sp_change_notice.info = protocol.info
    self.sp_change_notice.is_show = true
end

function MedalData:ClearSpecialExchangeNotice()
    self.sp_change_notice.is_show = false
end

function MedalData:MarkMedalInfo()
    self.operate_mark = self:GetOperateMedal()
end
function MedalData:SetCostMedal(item)
    self.cost_medal.item = item
end
function MedalData:GetCostMedal()
    return self.cost_medal.item
end
function MedalData:GetMedalFullName(_item)
    local operate_item = _item == nil and self:GetOperateMedal() or _item
    return operate_item.param.grade > 0 and string.format( Language.Medal.MedalShowName,
        string.format( Language.Medal.MedalRank, DataHelper.GetWord(operate_item.param.grade)),
        operate_item:ColorStr(),operate_item:NameId()) or string.format( Language.Medal.MedalZeroRankName, 
        operate_item:ColorStr(),operate_item:NameId())
end

function MedalData:GetRankUpTips()
    local item = self:GetOperateMedal()
    local grade = item.param.grade == nil and 1 or item.param.grade
    local max_flag = grade == 24
    local item_cost = self:GetCostMedal()
    local fail_down = math.floor(grade%5) > 0 and grade - math.floor( grade % 5 ) or grade
    if item_cost == nil then return {is_max = max_flag,succ_rank =  grade + 1 ,fail_rank = fail_down,rate = ""} end 

    local cfg = self.medal_rank_up[grade]
    local precent = cfg.rate + cfg["level"..item_cost.param.grade.."_add_rate"] 
    precent = precent > 10000 and 10000 or precent
    local up_grade = item_cost.param.grade > grade and item_cost.param.grade or grade + 1
    return {is_max = max_flag,succ_rank = up_grade ,fail_rank = fail_down,rate = string.format(Language.Medal.MedalRankUpTips,math.floor( precent /100 )) }
end

function MedalData:GetNoticeResult()
    local result_item = self:GetOperateMedal()

    local attr_list = {}
    local mark_cfg = self.medal_rank_up[self.operate_mark.param.grade]
    local cur_cfg = self.medal_rank_up[result_item.param.grade]

    local attr_list = {}
    for k,v in pairs(Language.Common.AttrList) do 
        if cur_cfg[AttrNameForType[k]] ~=nil and cur_cfg[AttrNameForType[k]] > 0 
            or ( cur_cfg[AttrNameForType[k]] == 0 and mark_cfg ~= nil 
            and mark_cfg[AttrNameForType[k]] ~=nil and mark_cfg[AttrNameForType[k]] > 0 ) then 
            local vo = {}
            vo.attr_type = k
            vo.attr_value = cur_cfg[AttrNameForType[k]]
            vo.change = 0
            if mark_cfg ~= nil and mark_cfg[AttrNameForType[k]] ~= cur_cfg[AttrNameForType[k]] then 
                local change =  cur_cfg[AttrNameForType[k]] - mark_cfg[AttrNameForType[k]]
                vo.change = change ~= 0 and change or 0
            end 

            table.insert( attr_list, vo)
        end 
    end 

    local effect_desc = ""

    if result_item.param.sp_id > 0 then 
        local effect_cfg = BagData.Instance:GetEquipEffectCfg(result_item.param.sp_id)
        if effect_cfg ~= nil then 
            effect_desc = effect_cfg.description
        else 
            LogError("Warning sp_id",result_item.param.sp_id,"is nil sp_id")
        end 
    end 


    local paramt_ = {
        attr = attr_list,
        is_show_effect = result_item.param.sp_id >0,
        is_new_effect = result_item.param.sp_id ~= self.operate_mark.param.sp_id,
        effect_desc = effect_desc,

        is_succ = self.NoticeInfo.Info.is_succ == 1,

        item_  = result_item,
        item_mark = self.operate_mark,
    }

    return paramt_
end

function MedalData:SetMarkAttrView(flag)
    self.need_mark_attr = flag
end

function MedalData:GetRewardRateShow(job_type)
    local list = TableCopy(self.build_reward_show_cfg[job_type])
    table.sort(list,function(a,b)return a.color > b.color end)
    return list
end

--============================附灵===============================
function MedalData:GetMedalSpCfg(item_id)
    local medal_sp_cfg = self.medal_sp_item[item_id]
    return BagData.Instance:GetEquipEffectCfg(medal_sp_cfg.sp_id)
end

function MedalData:GetMedalSpItemCfg(item_id)
    return self.medal_sp_item[item_id]
end

function MedalData:SetFulingTarget(Item)
    self.Fuling_Item = Item
end

function MedalData:GetFulingTarget()
    return self.Fuling_Item
end

function MedalData:GetFulingTargetCfg()
    local vo = {}
    for k,v in pairs(Config.medal_cfg_auto.sp_item_list) do 
        if v.item_id == self.Fuling_Item.item_id then 
            vo.sp_item_cfg = v
            break
        end 
    end  

    vo.effect_cfg = BagData.Instance:GetEquipEffectCfg(vo.sp_item_cfg.sp_id)
    
    return vo
end
function MedalData:GetMedalFulingMaterial()
    local list = {}
    for k,v in pairs( BagData.Instance:GetMedalList()) do 
        if v.param and v.param.sp_id == 0 then 
            v.param.is_equip = true
            table.insert(list,Item.Create(v))
        end 
    end 

    local bag = BagData.Instance:GetColumnGrids(ItemColumnType.Item)

    for k,v in pairs(bag) do 
        if v:IsMedal() and self:CheckCanBeCostFuling(v) then 
            v.param.is_equip = false
            table.insert( list,v)
        end 
    end 

    table.sort(list, function(a, b)
        if a.param.grade < b.param.grade then
            return true
        else
            return false
        end
    end)

    return list
end

function MedalData:CheckCanBeCostFuling(item)
    if item.param.grade == nil then return false end 

    return  item.param.sp_id == 0
end
--============================换装===============================

--============================展示===============================
function MedalData:GetBagEffectShowParam()
    if self.medal_effect_show_cache ~= nil then 
        return self.medal_effect_show_cache
    end 

    local data_list = {}   
    
    data_list.title_name = Language.Medal.BagSpShowTitle
    data_list.tab_list = Language.Conspiracy.TabList
    data_list.empty_lab = Language.Conspiracy.ShowListEmpty
    data_list.is_with = true
    data_list.show_list = {}
    for k,v in pairs(Config.special_effect_auto.spe_list) do 
        if v.is_medal_effect == 1 then 
            local vo = {}
            vo.sp_id = v.sp_id
            vo.name = v.reward_name
            vo.tab_index = v.page
            vo.qua = v.sp_priority
            vo.desc = v.reward_description
            vo.icon_type = ICON_TYPE.BUFF
            vo.icon = v.icon_id
            table.insert( data_list.show_list, vo )
        end
    end 

    if self.medal_effect_show_cache == nil then 
        self.medal_effect_show_cache = data_list
    end 

    return data_list
end
--============================兑换===============================
function MedalData:RoleNoticeComplete()
    self:CheckMedalMark()
end

function MedalData:FlushMedalFunOpen(_flag)
    if self.medal_open_flag == false and _flag == true then 
        self:SetMedalRedTips(0)
    end 

    if self.medal_open_flag == nil or self.medal_open_flag == false then 
        self.medal_open_flag = _flag
    end 

end

function MedalData:CheckMedalMark()
    self.medal_redtips.mark_point = UnityPlayerPrefs.GetString(PrefKeys.GetMedalBuildMark())
end

function MedalData:SetMedalRedTips(mark_point)
    local mark_list = Split(self.medal_redtips.mark_point,"|")

    local flag_add = true
    for k,v in pairs(mark_list) do 
        if v == tostring(mark_point) then 
            flag_add = false
            table.remove( mark_list, k)
            break
        end 
    end 

    if flag_add then 
        table.insert( mark_list, tostring(mark_point))
    end 

    local str_ = ""
    for k,v in pairs(mark_list) do 
        if k ~= #mark_list then 
            str_ = str_ .. v.."|"
        else 
            str_ = str_ .. v
        end 
    end 
    self.medal_redtips.mark_point = str_

    UnityPlayerPrefs.SetString(PrefKeys.GetMedalBuildMark(),self.medal_redtips.mark_point)
end

function MedalData:GetMedalRedTips()
    local mark_list = Split(self.medal_redtips.mark_point,"|")

    local result = {}
    for k,v in pairs(mark_list) do 
        local pos = tonumber(v)
        table.insert( result, pos )
    end 

    return result
end

function MedalData:GetMedalRedNumQuick()
    local mark_list = self:GetMedalRedTips()

    if #mark_list == 0 then 
        return 0
    end 

    local red_num = 0
    for k,v in pairs(mark_list) do 
        local cfg = self.medal_build_level_config[v]
        local num = Item.GetNum(CommonItemId.XianFen) >= cfg.one_num and 1 or 0
        red_num = red_num + num
    end 

    return red_num > 0 and 1 or 0
end

-- 直接输出本池中的天赐令牌
-- 注意，此处输出的是虚拟的物品
function MedalData:GetMedalSpRareShowList(_seq)
    if self.medal_sp_rare_show_cache ~= nil and self.medal_sp_rare_show_cache[_seq] then 
        return self.medal_sp_rare_show_cache[_seq]
    end 

    local data_list = {}   
    local seq_cfg = nil
    for k,v in pairs(Config.medal_cfg_auto.medal_exchange) do
        if v.seq == _seq then
            seq_cfg = v
            break
        end 
    end 

    for k,v in pairs(self.medal_sp_show_cfg) do 
        if v.index == _seq then 
            local sp_cfg = BagData.Instance:GetEquipEffectCfg(v.sp_id)
            if sp_cfg.is_special == 1 then 
                local item = Item.Create({
                    item_id = 8000,
                    is_bind = 1,
                    param = {sp_id = sp_cfg.sp_id,grade = sp_cfg.sp_rank}
                })
                table.insert(data_list,item)
            end 
        end
    end 

    if self.medal_sp_rare_show_cache == nil then 
        self.medal_sp_rare_show_cache = {}
    end 
    self.medal_sp_rare_show_cache[_seq] = data_list

    return data_list
end

function MedalData:GetMedalSpShowList(_seq)
    if self.medal_sp_page_cache ~= nil and self.medal_sp_page_cache[_seq] then 
        return self.medal_sp_page_cache[_seq]
    end 

    local data_list = {}   
    local seq_cfg = nil
    for k,v in pairs(Config.medal_cfg_auto.medal_exchange) do
        if v.seq == _seq then
            seq_cfg = v
            break
        end 
    end 
    
    data_list.title_name = Language.Medal.BuildSpShowTitle.."("..seq_cfg.name..")"
    data_list.tab_list = Language.Conspiracy.TabList
    data_list.empty_lab = Language.Conspiracy.ShowListEmpty
    data_list.show_list = {}
    for k,v in pairs(self.medal_sp_show_cfg) do 
        if v.index == _seq then 
            local sp_cfg = BagData.Instance:GetEquipEffectCfg(v.sp_id)
            local vo = {}
            vo.sp_id = sp_cfg.sp_id
            vo.name = sp_cfg.reward_name
            vo.tab_index = v.page
            vo.qua = sp_cfg.sp_priority
            vo.desc = sp_cfg.reward_description
            vo.icon_type = ICON_TYPE.BUFF
            vo.icon = sp_cfg.icon_id
            table.insert( data_list.show_list, vo )
        end
    end 

    if self.medal_sp_page_cache == nil then 
        self.medal_sp_page_cache = {}
    end 
    self.medal_sp_page_cache[_seq] = data_list
    return data_list
end

function MedalData:SetLottreyInfo(protocol)
    self.medal_lottrey_info = protocol

    self.medal_lottrey_flag.is_show = not self.medal_lottrey_flag.is_show
end

function MedalData:GetMedalLottreyInfo()
    local info = self.medal_lottrey_info
    local data = {}
    for i = 1,self.medal_lottrey_info.count do 
        local vo =  BagData.Instance:GetColumnList(info.reward_list[i].bay_type)[info.reward_list[i].bay_index]
        if vo.param.sp_id > 0 then 
            local sp_cfg = BagData.Instance:GetEquipEffectCfg(vo.param.sp_id)
            vo.sp_rank = sp_cfg.sp_rank
        else 
            vo.sp_rank = 1
        end 

        table.insert( data, vo )
    end 

    table.sort(data,function(a,b) return a.sp_rank > b.sp_rank  end)

    return data
end

--============================分解===============================
function MedalData:CheckMedalSmeltIsClicked(data)
    local flag = false
    for k,v in pairs(self.smelt_oper.select) do 
        if data.index == v.index then 
            flag = true
            break
        end 
    end 
    return flag
end
-- 选择当前选中的勋章
function MedalData:SetSelectSmeltMedal(item)
    if item == nil then 
        self.smelt_oper.select = {}
        return 
    end 
    local flag = 0
    for k,v in pairs(self.smelt_oper.select) do 
        if item.index == v.index then 
            flag = k
            break
        end 
    end 

    if flag == 0 then 
        table.insert(self.smelt_oper.select,item)
    else 
        table.remove(self.smelt_oper.select, flag)
    end 
    
end
function MedalData:GetSelectSmeltMedalDust()
    local dust_num = 0
    for k,v in pairs(self.smelt_oper.select) do 
        for i,j in pairs(Config.medal_cfg_auto.medal_resolve) do
            if j.rank == v.param.grade then 
                dust_num = dust_num + j.resolve_item.num    
            end 
        end 
    end 
    return dust_num
end
function MedalData:GetSelectSmeltMedal()
    return self.smelt_oper.select
end 
--limited = { func_str = "方法名",fun_value = 限制值 }
--noempty = 是否不填充空框
function MedalData:GetGridForSmeltMedal(paramt)
    local col_grids = BagData.Instance:GetGridByType({ItemType.Medal})
    local new_list = {}
    for k,v in pairs(col_grids) do 
        if paramt ~= nil and paramt.limited ~= nil then 
            if v[paramt.limited.func_str](v) == paramt.limited.fun_value then
                table.insert(new_list, v)
            end 
        else
            table.insert(new_list, v)
        end 
    end 

    if paramt == nil or paramt.noempty == nil then 
        if #new_list < 30 then 
            local plus = 30 - #new_list
            for i = plus ,1, -1 do 
                local item = {item_id = 0}
                table.insert(new_list,item)
            end 
        end 
    end 

    return new_list
end
--============================工具===============================
function MedalData:GetRankCfg(rank)
    return self.medal_rank_up[rank]
end

function MedalData:GetMedalMarketCfg(rank)
    return self.medal_market_config[rank]
end

function MedalData:CheckSpGroup(sp_group)
    local cfg = self.sp_group_cfg[sp_group]
    if cfg == nil then return nil end 
    if cfg.rate == 10000 then 
        return cfg.sp_id 
    else 
        return nil
    end 

end 

function MedalData:CatchChangeShowList(show_list, ignore)
    local mark_list = {}

    for k,v in pairs(self.effect_show_base_list) do 
        mark_list[k] = self:InitTypeInfo(v)
        mark_list[k].mark_list = {}
    end 

    for k,v in pairs(show_list) do 
        local sp_cfg = BagData.Instance:GetEquipEffectCfg(v.sp_id)
        for i,j in pairs(mark_list) do 
            if i == sp_cfg.sp_type then 
                table.insert( mark_list[sp_cfg.sp_type].mark_list, v.sp_id)
            end 
        end 
    end 
    
    local list_out = {}
    for k,v in pairs(mark_list) do 
        if not ignore then
            v.within = #v.mark_list > 0 and 1 or 0
        else
            v.within = 1
        end
        table.insert( list_out, v )
    end 

    table.sort(list_out, function(a, b)
        if ((10- a.role_id)*10 + a.within * 100 +(1 - a.is_special) ) > ((10 - b.role_id)*10 + b.within *100 + (1 - b.is_special)) then
            return true
        else
            return false
        end
    end)

    return list_out
end

function MedalData:GetEffectShowList(data, ignore)
    local list = self:GetMedalEffectTypeAllList(data.sp_type)

    local plus_list = {}
    for k,v in pairs(list) do 
        if v.is_medal_effect == 1 then 
            local vo = {}
            vo.Desc = v.description
            vo.level = string.format( Language.Medal.MedalRank,v.sp_rank)
            vo.sp_rank = v.sp_rank
            vo.pick_up = false
            if not ignore then
                for i,j in pairs(data.mark_list) do 
                    if j == v.sp_id then 
                        vo.pick_up = true 
                        break
                    end 
                end 
            else
                vo.pick_up = true
            end
            table.insert( plus_list, vo )
        end
    end 

    table.sort(plus_list, DataSorter.KeyLowerSorter("sp_rank"))

    return plus_list
end

function MedalData:GetMedalEffectTypeAllList(sp_type)
    return self.effect_type_all_list[sp_type]
end

--延迟显示属性变化工具
function MedalData:IsMarkAttrView()
    return self.need_mark_attr 
end

function MedalData:CatchAttrInfo(change_list, title_info)
    self.mark_change = {}
    self.mark_change.change_list = change_list
    self.mark_change.title_info = title_info
end

function MedalData:UnMarkAttrInfo()
    if not self:IsMarkAttrView() then return end 
    if self.mark_change == nil then self:SetMarkAttrView(false) return end 
    MainOtherCtrl.Instance:OpenAttrChange(self.mark_change.change_list, self.mark_change.title_info)
    self:SetMarkAttrView(false)
    RoleData.Instance.role_total_score.flush_score = not RoleData.Instance.role_total_score.flush_score
end

function MedalData.GetSpecialTypeByItem(item)
    if item and item:IsMedal() then
        if item:SpecialId() > 0 then
            local cfg = Cfg.MedalEffectById(item:SpecialId())
            return cfg.sp_type
        end
    end
    return 0
end

function MedalData.GetSpecialType(sp_id)
    if sp_id > 0 then
        local cfg = Cfg.MedalEffectById(sp_id)
        return cfg.sp_type
    end
end

function MedalData:SetSpMedalMarqueeMsg(param)
    if param == nil then 
        self.spmedal_marquee = {}
        return 
    end 

    table.insert(self.spmedal_marquee,param)
end

function MedalData:GetSpMedalMarqueeMsg()
    return self.spmedal_marquee
end

function MedalData:PlayMedalMarquee(data,is_all)
    if #self.spmedal_marquee == 0 then return end 

    if data == nil and is_all == nil then return end 

    local function check(k,v)
        local show = Split(v.str," ")
        PublicPopupCtrl.Instance:Marquee(show[1],v.delay_time)
        ChatCtrl.Instance:ChannelChatCustom{content = v.msg}
    end

    for k,v in pairs(self.spmedal_marquee) do 
        local list = Split(v.msg,";")
        if is_all == true then 
            check(k,v)
        else 
            check(k,v)
            table.remove(self.spmedal_marquee,k)
            break
        end 
    end 

    if is_all == true then 
        self.spmedal_marquee = {}
    end 
end

function MedalData:SetSelectedTargetSpChange(data,type,is_second)
    if type == 0 then 
        self.sp_change.select_data = data
    elseif type == 1 and not is_second then 
        self.sp_change.target_data = data
    elseif type == 1 and is_second then 
        self.sp_change.target_data_2 = data
    end 
end

function MedalData:ClearSpChangeTargetData()
    self.sp_change.target_data = nil
    self.sp_change.target_data_2 = nil
end

function MedalData:ClearSpChangeData()
    self.sp_change.select_data = nil 
    self.sp_change.target_data = nil
    self.sp_change.target_data_2 = nil
end

function MedalData:GetResultTargetData()
    if self.sp_change.target_data == nil then return {} end 
    local vo = {}
    vo.item_id = self.sp_change.target_data.item_id
    vo.param = {
        sp_id = self.sp_change.target_data.param.sp_id,
        grade = self.sp_change.target_data.param.grade
    }

    if self.sp_change.target_data_2 ~= nil then 
        vo.param.second_sp_id = self.sp_change.target_data_2.param.sp_id
        vo.param.special_medal_grade = self.sp_change.select_data.param.special_medal_grade
    end 

    return Item.Create(vo)
end

function MedalData:UpDataMixMainData()
    if self.mix_change.main_data == nil or self.mix_change.main_data.item_id == nil then return end 

    local neo_data = nil
    if self.mix_change.main_data.in_bag then 
        neo_data = BagData.Instance:GetItemByIndex(self.mix_change.main_data:GridColumn(),self.mix_change.main_data:GridIndex())
    else 
        neo_data = Item.Create(BagData.Instance:ViewData().medal_info[self.mix_change.main_data.vo.grid_index+1])
    end 

    if neo_data.param.second_sp_id ~= self.mix_change.main_data.param.second_sp_id then 
        self.mix_change.main_data = neo_data
    end 
end

function MedalData:GetMixChangeResult()
    if self.mix_change.plus_data == nil then 
        return self.mix_change.main_data
    end 
    local item_param = {}
    item_param.item_id = self.mix_change.main_data.item_id
    item_param.param = {}
    item_param.param.sp_id = self.mix_change.main_data.param.sp_id
    item_param.param.grade = self.mix_change.main_data.param.grade

    local is_plus = self.mix_change.main_data.param.second_sp_id > 0
    local cfg_plus = BagData.Instance:GetEquipEffectCfg(self.mix_change.plus_data.param.sp_id) 
    local list = KeyList(MedalData.Instance:GetMedalEffectTypeAllList(cfg_plus.sp_type),"sp_rank")

    local plus_result = is_plus and list[24] or list[15]

    item_param.param.second_sp_id = plus_result.sp_id
    item_param.param.special_medal_grade = is_plus and 2 or 1 -- plus_result.sp_rank

    return Item.Create(item_param)
end

function MedalData:SetSelectTargetMix(data,type)
    if type == 0 then 
        self.mix_change.select_data = data
    elseif type == 1 then 
        self.mix_change.plus_data = data
    elseif type == 2 then 
        self.mix_change.main_data = data
    end  
end

function MedalData:ClearMixChangeData()
    self.mix_change.select_data = nil 
    self.mix_change.plus_data = nil
    self.mix_change.main_data = nil
end

function MedalData:ClearCacheMixChangeData()
    self.mix_change.select_data = nil 
    self.mix_change.plus_data = nil
end

function MedalData:GetMedalMixBagList()
    local list = {}
    local bag = BagData.Instance:GetColumnGrids(ItemColumnType.Item)

    for k,v in pairs(bag) do 
        if v:IsMedal() and v:IsSpecialSp() and not BagData.Instance:CheckSameEquipByUniqueId(v,self.mix_change.main_data)
            and v.param.sp_id == self.mix_change.main_data.param.sp_id and v.param.second_sp_id == 0 then 
            table.insert( list,v )
        end 
    end 

    return list
end

function MedalData:GetMedalMixPlusList()
    local list = {}
    local is_plus = self.mix_change.main_data.param.second_sp_id > 0
    local plus_sp_id = is_plus and self.mix_change.main_data.param.second_sp_id or 0
    local sp_main_type = self.mix_change.main_data:EffectCfg().sp_type
    local sp_second_type = is_plus and self.mix_change.main_data:Effect2Cfg().sp_type or 0

    local bag = BagData.Instance:GetColumnGrids(ItemColumnType.Item)

    for k,v in pairs(bag) do 
        if v.param then--#15585653 LuaException
            local effect_cfg = BagData.Instance:GetEquipEffectCfg(v.param.sp_id)
            if v:IsMedal() and not v:IsSpecialSp() and v.param.grade == 24 and
            ((is_plus and effect_cfg.sp_type == sp_second_type) or (not is_plus and effect_cfg.sp_type ~= sp_main_type)) then 
                table.insert( list,v )
            end 
        end
    end 
    
    return list
end

function MedalData:GetMedalSpecialSpProfList()
    local list = {}
    -- 通用
    table.insert(list,{Prof= 0})
    -- 本职业
    local my_prof = RoleData.Instance:GetRoleProf()
    table.insert(list,{Prof= my_prof})
    -- 剩下的职业
    for i = 1, 8 do 
        if my_prof ~= i then 
            local vo = {Prof= i}
            table.insert(list,vo)
        end 
    end 

    return list
end

function MedalData:GetMedalSpecialSpBagList()
    local list = {}

    -- 开放为所有持有sp_id的令牌都可以换
    for k,v in pairs(BagData.Instance:ViewData().medal_info) do 
        local item = Item.Create(v)
        if v.item_id > 0 and item:IsMedal() and item:WithSp() then 
            table.insert( list,item)
        end 
    end 

    local bag = BagData.Instance:GetColumnGrids(ItemColumnType.Item)

    for k,v in pairs(bag) do 
        if v:IsMedal() and v:WithSp() then 
            table.insert( list,v)
        end 
    end 

    -- 排序 按阶，特效种类id
    table.sort(list, function(a, b)
        local itemA = a.param.sp_id + (a.in_bag and 0 or 10000)
        local itemB = b.param.sp_id + (b.in_bag and 0 or 10000)
        if itemB < itemA then
            return true
        else
            return false
        end
    end)

    return list
end

function MedalData:GetMedalSpecialSpTarget()
    local select_data = self.sp_change.select_data

    local list = Config.medal_cfg_auto.special_medal_exchange
    local result = {}
    for k,v in pairs(list) do 
        if select_data ~= nil then  -- and select_data.param.sp_id ~= v.sp_id
            local vo = Item.Create({item_id = v.special_medal_item,param = {sp_id = v.sp_id,
                grade =BagData.Instance:GetEquipEffectCfg(v.sp_id).sp_rank},is_bind = select_data.is_bind})
            table.insert(result, vo)
        end 
    end 

    return result
end

function MedalData:GetMedalSpecialSp2Target(mark_prof)
    local select_data = self.sp_change.select_data
    local list = select_data.param.special_medal_grade == 1 
        and self.sp_low_change_effect_list or self.sp_max_change_effect_list
    
    local result = {}
    for k,v in pairs(list[mark_prof]) do 
        if v.is_special == 0 then -- and select_data.param.second_sp_id ~= v.sp_id then 
            local vo = Item.Create({item_id = 8000,param = {sp_id = v.sp_id,
                grade =v.sp_rank},is_bind = select_data.is_bind})
            table.insert(result, vo)
        end 
    end 

    return result
end

function MedalData:GetMedalSpecialSpNomalTarget(mark_prof)
    local select_data = self.sp_change.select_data
    local cur_cfg = select_data:SpEffectCfg()
    local result = {}

    for k,v in pairs(self.sp_all_change_effect_list[mark_prof][cur_cfg.sp_rank]) do 
        if v.is_special == 0 and select_data.param.sp_id ~= v.sp_id then 
            local vo = Item.Create({item_id = 8000,param = {sp_id = v.sp_id,
                grade =v.sp_rank},is_bind = select_data.is_bind})
            table.insert(result, vo)
        end 
    end 

    return result
end


function MedalData:MedalPreViewAllUpgrade(select_data)
    -- 按照等级分组
    local change_list = {}
    for k,v in pairs(select_data) do 
        if v.item_id > 0 then 
            if change_list[v.param.grade] == nil then change_list[v.param.grade] = {} end 
            table.insert(change_list[v.param.grade],v.param.grade)
        end 
    end 

    self.all_up_grade_limit = 0
    local list = self:MedalPreCheck(change_list)

    local virtual_list = {}
    for k,v in pairs(list) do 
        for i,j in pairs(v) do 
            local vo = Item.Create({item_id = 8000})
            vo.param.sp_id = 0
            vo.param.grade = k
            table.insert(virtual_list,vo)
        end 
    end 

    table.sort(virtual_list,function(a,b)
        return a.param.grade < b.param.grade
    end)

    return virtual_list
end

function MedalData:MedalPreCheck(grade_list)
    local is_change = false

    self.all_up_grade_limit = self.all_up_grade_limit + 1
    if self.all_up_grade_limit > 50 then return grade_list end 

    -- 将等级分组中的升华（余2）去除，并提取结果
    local neo_check_list = {}
    for k,v in pairs(grade_list) do 
        -- if #v > 1 then 
            if k == 24 then 
                local last_num = #v
                if neo_check_list[k] == nil then neo_check_list[k] = {} end 
                for i = 1,last_num do 
                    table.insert(neo_check_list[k],k)
                end 
                -- break 
            end 
            local flag_change = k + 1 <= 24 
            local vo = flag_change and k + 1 or 24
            local plus_num = flag_change and math.floor(#v / 2) or 0
            local last_num = flag_change and  math.floor(#v % 2) or 0 

            -- 注入旧处理剩余
            if neo_check_list[k] == nil then neo_check_list[k] = {} end 
            for i = 1,last_num do 
                table.insert(neo_check_list[k],k)
            end 

            -- 注入新产生
            if neo_check_list[vo] == nil then neo_check_list[vo] = {} end 
            for i = 1,plus_num do 
                table.insert(neo_check_list[vo],vo)
            end 
            if not is_change then is_change = plus_num > 0 end 
        -- end
    end 

    for k,v in pairs(neo_check_list)do
        if #v == 0 then neo_check_list[k] = nil end 
    end

    if is_change then 
        return self:MedalPreCheck(neo_check_list)
    else
        return neo_check_list
    end 
end

function MedalData:SetPreUpgradeItem(data)
    self.pre_resolve_item = data
end

function MedalData:GetPreUpgradeItem()
    return self.pre_resolve_item
end

function MedalData:CacheMedalAllUpgrade(item)
    if self.delay_cache_all == nil then 
        self.delay_cache_all = TimeHelper:AddDelayFrameTimer(function ()
            table.sort(self.cache_all_medal_list,function(a,b) return a.param.grade < b.param.grade end)
            MainOtherCtrl.Instance:OpenGiftView(self.cache_all_medal_list,GIFT_OPEN_TYPE.SPECIAL,false)
            self.cache_all_medal_list = {}
            self.delay_cache_all = nil
		end,5)
        if self.cache_all_medal_list == nil then self.cache_all_medal_list = {} end 
    end  

    table.insert( self.cache_all_medal_list, item )
end

-- 0 无 1 激活 2 失效
function MedalData:GetActiveEquipMedal(medal_o_list)
    local medal_list = medal_o_list == nil and BagData.Instance:ViewData().medal_info or medal_o_list

    local list = {}
    local temp_list = {}
    for i = 1,3 do 
        if medal_list[i].item_id > 0 then 
            if medal_list[i].param.sp_id > 0 then
                local vo_cfg = Cfg.MedalEffectById(medal_list[i].param.sp_id)
                if vo_cfg ~= nil then 
                    local vo = {pos = i ,param = 1,sp_type = vo_cfg.sp_type,sp_rank = vo_cfg.sp_rank,sp_id = medal_list[i].param.sp_id}
                    if temp_list[vo_cfg.sp_type] == nil then temp_list[vo_cfg.sp_type] = {} end 
                    table.insert(temp_list[vo_cfg.sp_type],vo)
                end 
            end 

            if medal_list[i].param.second_sp_id > 0 then
                local vo_cfg = Cfg.MedalEffectById(medal_list[i].param.second_sp_id)
                if vo_cfg ~= nil then 
                    local vo = {pos = i ,param = 2,sp_type = vo_cfg.sp_type,sp_rank = vo_cfg.sp_rank,sp_id = medal_list[i].param.second_sp_id}

                    if temp_list[vo_cfg.sp_type] == nil then temp_list[vo_cfg.sp_type] = {} end 
                    table.insert(temp_list[vo_cfg.sp_type],vo)
                end 
            end 
        end 
    end 

    for k,v in pairs(temp_list)do 
        table.sort(v,function(a,b)return (b.sp_rank+(1-b.param)) < (a.sp_rank+(1-a.param)) end)
    end 

    for k,v in pairs(temp_list)do 
        for i = 1,3 do 
            if v[i] then 
                if list[v[i].pos] == nil then list[v[i].pos] = {} end 
                local prof_limit = RoleData.Instance:ProfType()
                local vo_cfg = Cfg.MedalEffectById(v[i].sp_id)

                list[v[i].pos][v[i].param] = (i == 1 and( (vo_cfg.role_id > 0 and vo_cfg.role_id == prof_limit) or vo_cfg.role_id ==0 )) and 1 or 2
            end 
        end 
    end 

    for i = 1,3 do 
        if list[i] == nil then list[i] = {} end 
        if list[i][1] == nil then list[i][1] = 0 end 
        if list[i][2] == nil then list[i][2] = 0 end 
    end 

    return list
end

-- 0 无 1 激活 2 失效
function MedalData:GetActiveEquipMedalSingle(param_int,medal_o_list)
    local act_list = self:GetActiveEquipMedal(medal_o_list)
    return act_list[param_int][1] == 2,act_list[param_int][2] == 2
end

--解析人物勋章列表信息
function MedalData.GetRoleMedalData(medal_info_list)
    local medal_list = {}
    for k,v in pairs(medal_info_list) do
        for _,v2 in pairs(medal_info_list) do
            if v.param.sp_type == nil and v.param.sp_id > 0 then
                local cfg =  Cfg.MedalEffectById(v.param.sp_id)
                v.param.sp_type = cfg ~= nil and cfg.sp_type or nil
            end
            if v2.param.sp_type == nil and v2.param.sp_id > 0 then
                local cfg = Cfg.MedalEffectById(v2.param.sp_id)
                v2.param.sp_type = cfg ~= nil and cfg.sp_type or nil
            end
            if v2.param.sp_type ~= nil and v.param.sp_type ~= nil 
                and v2.index ~= v.index and v.param.sp_type == v2.param.sp_type then
                if v.param.grade > v2.param.grade then
                    v.param.sp_fail = false
                elseif v.param.grade < v2.param.grade then
                    v.param.sp_fail = true
                    break
                elseif v.index > v2.index then
                    v.param.sp_fail = true
                    break
                end
            end
        end
        medal_list[k] = Medal.New(v)
        medal_list[k].in_bag = false
    end
    return medal_list
end
