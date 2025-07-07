local equip_type_sp = {
    [0] = "star_TouKui",
    [1] = "star_KaiJia",
    [2] = "star_FaQi",
    [3] = "star_DunPai",
    [4] = "star_XueZi",
    [5] = "star_JieZhi",
    [6] = "star_ShouZhuo",
    [7] = "star_DiaoZhui",
}

WorkshopBlazeStar = WorkshopBlazeStar or DeclareMono("WorkshopBlazeStar", UIWFlushPanel)
function WorkshopBlazeStar:WorkshopBlazeStar()
    self.data = WorkshopData.Instance

    self.data_cares = {
        {data = self.data.blazestar_info, func = self.FlushInfo, keys = {"select_data"}, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushBagChange, init_call = false},
        {data = self.data.blazestar_info, func = self.FlushQuickTick,  keys = {"quick_tick"}, init_call = false},
        {data = self.data.blazestar_info, func = self.FlushMainInfo,  keys = {"obl_left","obl_right","operate"}, init_call = false},
    }

    self.oper_cache = {pop_mark = ""}
    self.send_data = {}

end 

function WorkshopBlazeStar:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    self.data:SetSelectBlazeItem()
    self.data:ClearBlazeInfo()

    if self.handle_delay ~= nil then 
        TimeHelper:CancelTimer(self.handle_delay)
        self.handle_delay = nil 
    end 

    if self.handle_delay_done ~= nil then 
        TimeHelper:CancelTimer(self.handle_delay_done)
        self.handle_delay_done = nil 
    end 
end 

function WorkshopBlazeStar:FlushInfo()
    self:FlushItemInfo()

    -- if self.data:GetBlazeQuick() and self.data:IsSelectCanQuickItem() then 
    --     self:OnlyOneKey()
    -- end 
end

-- 全更新
function WorkshopBlazeStar:FlushItemInfo()
    self:FlushMainInfo()
    self:FlushAttrChange()
    self:FlushQuickTick()
end

-- 四个装备的有无
function WorkshopBlazeStar:FlushMainInfo()
    if self.mark ~= nil then 
        local result = nil
        if self.mark.in_bag then 
            result = BagData.Instance:GetItemGridByIndex( self.mark:GridColumn(), self.mark:GridIndex() )
        else 
            result = self.mark:IsEquip() and BagData.Instance:GetEquipList(self.mark:GridIndex()) 
                or BagData.Instance:GetJewelryList(self.mark:GridIndex()) 
        end 
        -- 目标熔铸成功后，两者相同
        if result:BlazeStar() == self.data.blazestar_info.select_data.blaze.item_star then 
            ViewMgr:OpenView(BlazeStarDoneView,{item = result})

            self.data:ClearBlazeInfo()
            self.mark = nil
        end 
    end 

    local target_item = Item.Create(self.data.blazestar_info.select_data)
    target_item.show_star_level = self.data.blazestar_info.select_data.blaze.item_star
    self.target:SetData(target_item)
    self.obl_left:SetData(self.data.blazestar_info.obl_left)
    self.obl_right:SetData(self.data.blazestar_info.obl_right)
    self.operate:SetData(self.data.blazestar_info.operate)

    -- 随装备格子刷新刷新红点
    local operate_red = false
    local list_operate = self.data:CacheBlazeSelect("operate")
    if self.data.blazestar_info.operate == nil then 
        operate_red = #list_operate > 0
        self.operate:ShowRed(operate_red)
        -- 实际上左右材料位的材料是一致的
        local list_obl_left = self.data:CacheBlazeSelect("obl_left")
        self.obl_left:ShowRed(#list_obl_left >2 and self.data.blazestar_info.obl_left == nil)
        self.obl_right:ShowRed(#list_obl_left >2 and self.data.blazestar_info.obl_right == nil)
    elseif self.data.blazestar_info.operate ~= nil then 
        self.operate:ShowRed(false)
        -- 实际上左右材料位的材料是一致的
        local list_obl_left = self.data:CacheBlazeSelect("obl_left")
        self.obl_left:ShowRed(#list_obl_left > 2 and self.data.blazestar_info.obl_left == nil)
        self.obl_right:ShowRed(#list_obl_left > 2 and self.data.blazestar_info.obl_right == nil)
    end 
end

-- 该配置提供的属性变化
function WorkshopBlazeStar:FlushAttrChange()
    if self.data.blazestar_info.select_data == nil then return end 

    local target_item = Item.Create(self.data.blazestar_info.select_data)
    UH.SetText(self.target_name, target_item:QuaName())

    local oper_data = self.data.blazestar_info.select_data
    local last_cfg = self.data:GetBlazeData(oper_data.blaze.id,oper_data.blaze.item_star-1)

    -- 1星（item_star為2）時读取0星（item_star為1）的配置
    -- 该配置不会被列表化
    if oper_data.blaze.item_star == 2 then 
        -- 格式兼容
        last_cfg = {}
        last_cfg.blaze = self.data:GetBlazeCfg(oper_data.blaze.id,1)
    end 

    for i = 1,2 do 
        local vo = {
            attr_type = oper_data.blaze["attr_type_"..i],
            attr_next = oper_data.blaze["attr_min_"..i],
            attr_value = last_cfg ~= nil and last_cfg.blaze["attr_min_"..i] or 0
        }
        local flag = vo.attr_next > 0
        self["attr_"..i].gameObject:SetActive(flag)
        if flag then self["attr_"..i]:SetData(vo) end 
    end 

end

function WorkshopBlazeStar:FlushQuickTick()
    self.qucik_tick:SetActive(self.data:GetBlazeQuick())
end

-- 背包刷新
function WorkshopBlazeStar:FlushBagChange()
    self:FlushMainInfo()
    self:FlushAttrChange()
end

-- 是否快速查询
function WorkshopBlazeStar:OnClickQuickTick()
    local activeSelf = not self.qucik_tick.activeSelf
    self.data:SetBlazeQuick(activeSelf)

    -- 即时选中
    if activeSelf then 
        self:QuickTickHandle()
    end 
end

function WorkshopBlazeStar:QuickTickHandle()
    local target = self.data:SelectBlazeFromEquiplist()
    if target ~= nil then 
        local e_cfg = Item.GetConfig(target.id)
        local prof = RoleData.ProfIdToProfType(e_cfg.limit_prof)
        local level = e_cfg.limit_level
        local flag_prof = prof == self.data.blazestar_info.select_prof
        local flag_level = level == self.data.blazestar_info.select_level

        self.data.blazestar_info.select_prof = prof
        self.data.blazestar_info.select_level = level

        local mark = {item = target , callback = function ()
            self:OnlyOneKey()
            -- WorkshopData.Instance:ClearBlazeInfo()
        end,need_flush = not flag_prof or not flag_level}
        self.atlas:FlushInfo(mark) 
    end 
end

function WorkshopBlazeStar:AutoClickHandle()
    local info = self.data:GetAutoClickBlazeStar()
    if info ~= nil then 
        local e_cfg = Item.GetConfig(info.item_id)
        local prof = RoleData.ProfIdToProfType(e_cfg.limit_prof)
        local level = e_cfg.limit_level

        local flag_prof = prof == self.data.blazestar_info.select_prof
        local flag_level = level == self.data.blazestar_info.select_level

        self.data.blazestar_info.select_prof = prof
        self.data.blazestar_info.select_level = level

        local cfg = self.data:GetBlazeCfg(info.item.item_id,info.item:BlazeStar() + 1)
        local flag_change = cfg == nil 
        if flag_change then 
            cfg = self.data:GetBlazeCfg(info.item.item_id,info.item:BlazeStar())
        end 

        local mark = {item = cfg , callback = function ()
            self:OnlyOneKey(flag_change and nil or info.item)
        end,need_flush = not flag_prof or not flag_level}
        self.atlas:FlushInfo(mark) 

        self.data:SetAutoClickBlazeStar()
    end 
end

-- 点击被操作物品
function WorkshopBlazeStar:OnClickOperateCell()
    -- LogError("!?@OnClickOperateCell")
    self.pop_part.gameObject:SetActive(true)
    self.pop_part:SetData({pop_mark = "operate"})
end
-- 点击右侧祭品
function WorkshopBlazeStar:OnClickRightOblCell()
    -- LogError("!?@OnClickRightOblCell")
    self.pop_part.gameObject:SetActive(true)
    self.pop_part:SetData({pop_mark = "obl_right"})
end
-- 点击左侧祭品
function WorkshopBlazeStar:OnClickLeftOblCell()
    -- LogError("!?@OnClickLeftOblCell")
    self.pop_part.gameObject:SetActive(true)
    self.pop_part:SetData({pop_mark = "obl_left"})
end

function WorkshopBlazeStar:onFlush()
    self.atlas:FlushInfo()

    if not self.is_init then 
        if self.data:GetBlazeQuick() then self:QuickTickHandle() end 
        if self.data:GetAutoClickBlazeStar() ~= nil then self:AutoClickHandle() end 
        self.is_init = true
    end 
end

-- 只选中operate
function WorkshopBlazeStar:OnlyOneKey(item)
    self.data:ClearBlazeInfo()

    local list_operate = self.data:CacheBlazeSelect("operate")
    if self.data.blazestar_info.operate == nil and item == nil then 
        self.data.blazestar_info.operate= #list_operate > 0 and list_operate[1] or nil
    end 
end

-- 点击添加
function WorkshopBlazeStar:OnClickOneKey()
    self.data:ClearBlazeInfo()
    local list_operate = self.data:CacheBlazeSelect("operate")
    if self.data.blazestar_info.operate == nil then 
        self.data.blazestar_info.operate= #list_operate > 0 and list_operate[1] or nil
    end 
    local list_obl_left = self.data:CacheBlazeSelect("obl_left")
    if self.data.blazestar_info.obl_left == nil then 
        self.data.blazestar_info.obl_left= #list_obl_left > 0 and list_obl_left[1] or nil
    end
    local list_obl_right = self.data:CacheBlazeSelect("obl_right")
    if self.data.blazestar_info.obl_right == nil then 
        self.data.blazestar_info.obl_right= #list_obl_right > 0 and list_obl_right[1] or nil
    end 

    if #list_operate == 0 then 
        PublicPopupCtrl.Instance:Center(Language.Workshop.BlazeStar.LackOperateEquip)
    end 

    if #list_obl_left == 0 or #list_obl_right == 0 then 
        PublicPopupCtrl.Instance:Center(Language.Workshop.BlazeStar.LackOblEquip)
    end 
end

-- 点击提示
function WorkshopBlazeStar:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[198].desc})
end

-- 点击熔铸
function WorkshopBlazeStar:OnClickBlazeStar()
    if self.data.blazestar_info.operate == nil then 
        PublicPopupCtrl.Instance:Center(Language.Workshop.BlazeStar.SelectOperate)
        return 
    end 

    if self.data.blazestar_info.obl_left == nil or self.data.blazestar_info.obl_right == nil then 
        PublicPopupCtrl.Instance:Center(Language.Workshop.BlazeStar.SelectOblEquip)
        return 
    end 

    local flag_smelt = WorkshopData.Instance:CheckEquipWithSmeltDone(self.data.blazestar_info.obl_left) or 
        WorkshopData.Instance:CheckEquipWithSmeltDone(self.data.blazestar_info.obl_right)

    local flag_neveal = self.data.blazestar_info.obl_left:IsNeveal() or self.data.blazestar_info.obl_right:IsNeveal()

    if flag_smelt then
        local param_t = {
            content = Language.Workshop.BlazeStar.SelectOblWithSmelt,
            confirm = {
                func =  function()
                    ViewMgr:CloseView(DialogTipsView)
                    self.block:SetActive(true)
                    self.EffectTool:Play(3161059) 
                end 
            },
            cancel = {
                func =  function()
                    ViewMgr:CloseView(DialogTipsView)
                end 
            }
        }

        PublicPopupCtrl.Instance:DialogTips(param_t)
        return 
    end 

    if flag_neveal then
        local param_t = {
            content = Language.Workshop.BlazeStar.SelectOblWithNeveal,
            confirm = {
                func =  function()
                    ViewMgr:CloseView(DialogTipsView)
                    self.block:SetActive(true)
                    self.EffectTool:Play(3161059) 
                end 
            },
            cancel = {
                func =  function()
                    ViewMgr:CloseView(DialogTipsView)
                end 
            }
        }

        PublicPopupCtrl.Instance:DialogTips(param_t)
        return 
    end 

    self.block:SetActive(true)
    self.EffectTool:Play(3161059) 
end

function WorkshopBlazeStar:OnEffectDone()
    local with_stone = (self.data.blazestar_info.obl_left ~= nil and self.data.blazestar_info.obl_left:HadStone()) or (self.data.blazestar_info.obl_right ~= nil and self.data.blazestar_info.obl_right:HadStone())
    local with_spirit = (self.data.blazestar_info.obl_left ~= nil and self.data.blazestar_info.obl_left:HasSpirit()) or (self.data.blazestar_info.obl_right ~= nil and self.data.blazestar_info.obl_right:HasSpirit())

    self.data.blazestar_info.obl_left:UnStone()
    self.data.blazestar_info.obl_left:RemoveAllSpirit()

    self.data.blazestar_info.obl_right:UnStone()
    self.data.blazestar_info.obl_right:RemoveAllSpirit()

    if with_stone then 
        PublicPopupCtrl.Instance:Center(Language.Resolve.StoneUnFix)
    end 
    if with_spirit then 
        PublicPopupCtrl.Instance:Center(Language.Resolve.SpiritUnFix)
    end 

    local change_cfg = self.data:ChangeBlazeCfg(self.data.blazestar_info.select_data.blaze)

    -- LogError("?!",change_cfg)
    -- LogError("?operate!",self.data.blazestar_info.operate.param.star_level)
    -- LogError("?obl_left!",self.data.blazestar_info.obl_left.param.star_level)
    -- LogError("?obl_right!",self.data.blazestar_info.obl_right.param.star_level)
    WorkshopCtrl.Instance:SendOperateReqWithParamt(
        {
            op_type = WORLSHOP_OP.WORKSHOP_OP_UP_STAR,
            param1 = change_cfg.compose_seq,
            param2 = self.data.blazestar_info.operate:IsJewelry() and 1 or 0,
            param3 = self.data.blazestar_info.operate.in_bag == true and 1 or 0,
            param4 = self.data.blazestar_info.operate.index,
            param5 = self.data.blazestar_info.obl_left.index,
            param6 = self.data.blazestar_info.obl_right.index,
        }
    )

    self.handle_delay = TimeHelper:AddDelayTimer(
        function() self.block:SetActive(false) end,0.6)

    self.mark = self.data.blazestar_info.operate
    self.handle_delay_done = TimeHelper:AddDelayTimer(
        function() self.mark = nil end,2)
end 

function WorkshopBlazeStar:OnClickTarget()

    local data = Item.Create(self.data.blazestar_info.select_data)

    data.show_key = "workshop_blaze"
    local info = {
        item_list = {data}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
end 

BlazeAttrItem = BlazeAttrItem or DeclareMono("BlazeAttrItem", UIWidgetBaseItem)
function BlazeAttrItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.attr_name, Language.Common.AttrList[data.attr_type])
    UH.SetText(self.attr_value, DataHelper.IsPercent(data.attr_type) and Percent(data.attr_value) or data.attr_value)
    UH.SetText(self.attr_next, DataHelper.IsPercent(data.attr_type) and Percent(data.attr_next) or data.attr_next)
end 

BlazeStarItemShow = BlazeStarItemShow or DeclareMono("BlazeStarItemShow", UIWidgetBaseItem)
function BlazeStarItemShow:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.Cell:SetData(data or Item.New())
    self.BlazeLevelShow:SetActive(data ~= nil)
    
    if self.plus ~= nil then self.plus:SetActive(data == nil) end 
    if self.img_wear ~= nil then self.img_wear:SetActive(data ~= nil and not data.in_bag) end 
    
    if data ~= nil and data.item_id > 0 then 
        local str = data:Level()..Language.Common.Level
        if data:Color() == ItemColor.Star or data:Color() == ItemColor.God then
            str = data:StrengthenLevel()..Language.Common.StrengthenLevel
        end
        self.BlazeLevelText.text = str 
    end 
end 

function BlazeStarItemShow:ShowRed(flag)
    self.show_red = self.show_red or UiRedPoint.New(self.red_pos, Vector3.New(0,0,0))
    self.show_red:SetNum(flag and 1 or 0)
end

WorkshopBlazeStarPop = WorkshopBlazeStarPop or DeclareMono("WorkshopBlazeStarPop", UIWFlushPanel)
function WorkshopBlazeStarPop:WorkshopBlazeStarPop()
    self.data = WorkshopData.Instance

    self.cache_data = nil
    self.equip_list:SetCompleteCallBack(function()
        if not self.empty.activeSelf then 
            self.equip_list:ClickItem(1)
        end 
    end)
end 

function WorkshopBlazeStarPop:SetData(data)
    self.pop_mark = data.pop_mark

    local new_list = self.data:CacheBlazeSelect(self.pop_mark)
    self.equip_list:SetData(new_list)
    self.AreaClick:SetActive(true)

    self.empty:SetActive(#new_list == 0)
end

function WorkshopBlazeStarPop:OnClickClose()
    self.main:SetActive(false)    
    self.AreaClick:SetActive(false)
    self.toggle_group:SetAllTogglesOff()
    self.cache_data = nil
end

function WorkshopBlazeStarPop:OnClickEquipCell(data)
    self.cache_data = data
end

function WorkshopBlazeStarPop:OnClickSelectEquip()
    if self.cache_data == nil then 
        PublicPopupCtrl.Instance:Center(Language.Workshop.BlazeStar.HadNotSelect)
        return 
    end 

    self.data.blazestar_info[self.pop_mark]= self.cache_data

    self:OnClickClose()
end

BlazeSelectItem = BlazeSelectItem or  DeclareMono("BlazeSelectItem", UIWidgetBaseItem)
function BlazeSelectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.Cell:SetData(data)
    UH.SetText(self.Name,data:Name())
    self.img_wear:SetActive(not data.in_bag)
end 

function BlazeSelectItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end

WorkshopBlazeSatrAtlas = WorkshopBlazeSatrAtlas or DeclareMono("WorkshopBlazeSatrAtlas", UIWFlushPanel)
function WorkshopBlazeSatrAtlas:WorkshopBlazeSatrAtlas()
    self.data = WorkshopData.Instance
    self.data_cares = {
        {data = self.data.blazestar_info, func = self.FlushInfo, keys = {"select_prof", "select_level"}, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushBagChange, init_call = false},
    }

    self.equip_list.StepCount = 20
end 

function WorkshopBlazeSatrAtlas:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    -- if self.mark_delay ~= nil then 
    --     TimeHelper:CancelTimer(self.mark_delay)
    --     self.mark_delay = nil 
    -- end 
end 

function WorkshopBlazeSatrAtlas:FlushInfo(mark_plus)
    local screen_info,show_list = self.data:GetBlazeAtlasInfo()

    -- mark_plus 为空时为来自主界面变动产生刷新
    -- mark_plus 不为空但是不持有need_flush为来自blazestarAtlas的刷新
    -- mark_plus 不为空但持有need_flush为来自主界面的自动跳转
    if mark_plus == nil or (mark_plus ~= nil and 
       (mark_plus.need_flush ~= nil and mark_plus.need_flush == true) 
       or mark_plus.need_flush == nil) then 
        self.equip_list:SetData(show_list,true)
    end 

    UH.SetText(self.prof_screen,screen_info.prof.name)
    UH.SetText(self.level_screen,screen_info.level.name)

    self:MarkJump(mark_plus)
end

function WorkshopBlazeSatrAtlas:MarkJump(mark_info)
    if mark_info == nil or mark_info.item == nil then return end 

    local screen_info,show_list = self.data:GetBlazeAtlasInfo(true)
    local jump = {}
    for k,v in pairs(show_list) do 
        if v.item_id == mark_info.item.id or Equip.GetEquipType(v.item_id) == Equip.GetEquipType(mark_info.item.id) then 
            jump.parent = k
            for i,j in pairs(v.children) do 
                if j.blaze.item_star == mark_info.item.item_star then 
                    jump.child = i
                    break
                end 
            end 
            break
        end 
    end 

    if show_list[jump.parent] ~= nil and show_list[jump.parent].children ~= nil and 
        show_list[jump.parent].children[jump.child] ~= nil then 

        -- 此处选中后必须第一时间被主界面刷新信息，故需要主动选中目标项
        self.equip_list:ClickChild(show_list[jump.parent].children[jump.child])
        self.data:SetSelectBlazeItem(show_list[jump.parent].children[jump.child])

        if mark_info.callback ~= nil then 
            mark_info.callback()    
        end 
    end 
end

function WorkshopBlazeSatrAtlas:FlushBagChange()
    self:FlushInfo()
end

-- 点击具体筛选按钮
function WorkshopBlazeSatrAtlas:OnClickBtn(data)
    if self.data.blazestar_info[data.type] ~= nil then 
        self.data.blazestar_info[data.type] = data.key
    end 
    self:OnClickShuaiXuan()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)    
end

-- 点击职业筛选
function WorkshopBlazeSatrAtlas:OnClickEquipProf()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.ProfArrow:SetActive(not activeSelf)
    self.ClickAreaShuai:SetActive(activeSelf)
    if activeSelf then
        self.BtnList:SetData(self.data.screen_list.prof)
    else
        self.LevelArrow:SetActive(true)
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

-- 点击等级筛选
function WorkshopBlazeSatrAtlas:OnClickEquipLevel()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.LevelArrow:SetActive(not activeSelf)
    self.ClickAreaShuai:SetActive(activeSelf)
    if activeSelf then
        self.BtnList:SetData(self.data.screen_list.level)
    else
        self.ProfArrow:SetActive(true)
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function WorkshopBlazeSatrAtlas:OnClickShuaiXuan()
    if self.ProfArrow.activeSelf then 
        self:OnClickEquipProf()
    elseif self.LevelArrow.activeSelf then 
        self:OnClickEquipLevel()
    end 
end

function WorkshopBlazeSatrAtlas:OnClickItem(data)
    self.data:SetSelectBlazeItem(data)
end


WorkShopScreenBtnItem = WorkShopScreenBtnItem or DeclareMono("WorkShopScreenBtnItem", UIWidgetBaseItem)
function WorkShopScreenBtnItem:WorkShopScreenBtnItem() end
function WorkShopScreenBtnItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.name)
end 


BlazeStarParentItem = BlazeStarParentItem or DeclareMono("BlazeStarParentItem",UIWidgetBaseItem)
function BlazeStarParentItem:BlazeStarParentItem()
    self.ws_data = WorkshopData.Instance
end

function BlazeStarParentItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.LblTitle, data.name)

    local is_red = false
    for k,v in pairs(data.children) do 
        local operate_list = self.ws_data:CacheBlazeSelect("operate",v)
        if #operate_list >= 1 then 
            local obl_list = self.ws_data:CacheBlazeSelect(nil,v)
            is_red = #obl_list > 2
        end 

        if is_red then 
            break
        end 
    end 

    UH.SpriteName(self.icon, equip_type_sp[data.equip_type])

    self.show_red = self.show_red or UiRedPoint.New(self.red_pos, Vector3.New(0,0,0))
    self.show_red:SetNum(is_red and 1 or 0)
end

function BlazeStarParentItem:OnSelect(selected)
    if self.Arrow then
        self.Arrow:SetActive(not selected)
    end

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

BlazeStarItem = BlazeStarItem or DeclareMono("BlazeStarItem",UIWidgetBaseItem)
function BlazeStarItem:BlazeStarItem()
    self.ws_data = WorkshopData.Instance
end

function BlazeStarItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.Interactable.Interactable = true
    self:FlushName()
    -- UH.SetText(self.LblName, data.blaze.item_star..Language.Workshop.BlazeStar.Star)
    -- UH.SetText(self.LblSelectName, data.blaze.item_star..Language.Workshop.BlazeStar.Star)
end 

function BlazeStarItem:FlushName()
    local mate_num = 0
    local operate_list = self.ws_data:CacheBlazeSelect("operate",self.data)
    local mate_list = self.ws_data:CacheBlazeSelect("",self.data)

    mate_num = (#operate_list >= 1 and #mate_list > 2) and #mate_list or #operate_list
    -- if #operate_list >= 1 then 
    --     local obl_list = self.ws_data:CacheBlazeSelect(nil,self.data)
    --     mate_num = #obl_list -- operate_list[1].in_bag and #obl_list -1 or #obl_list
    -- end 

    local color_select = mate_num > 2 and COLORSTR.Green10 or COLORSTR.Red7
    local color_nomal = mate_num > 2 and COLORSTR.Green10 or COLORSTR.Red10

    -- 测试代码块
    -- Item.GetName(self.data.item_id).. 
    UH.SetText(self.LblName, (self.data.blaze.item_star - 1)..Language.Workshop.BlazeStar.Star..ColorStr(
        string.format(Language.Workshop.BlazeStar.matSelect,mate_num),color_nomal))

    UH.SetText(self.LblSelectName, (self.data.blaze.item_star - 1)..Language.Workshop.BlazeStar.Star..ColorStr(
        string.format(Language.Workshop.BlazeStar.matSelect,mate_num),color_select))

    self.show_red = self.show_red or UiRedPoint.New(self.red_pos, Vector3.New(0,0,0))
    self.show_red:SetNum(mate_num > 2 and 1 or 0)
end 

function BlazeStarItem:Click()
    -- if self.ItemClick then
    --     self.ItemClick:Trigger()
    -- end
    -- 一次点击就够了
    if self.Toggle then
        self.Toggle:Trigger()
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end 