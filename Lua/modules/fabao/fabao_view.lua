FabaoView = FabaoView or DeclareView("FabaoView", "fabao/fabao_view", Mod.Fabao.Main)
VIEW_DECLARE_LEVEL(FabaoView, ViewLevel.L0)
local sp = {
    [0] = "xingxinghui",
    [1] = "xingxingzi",
    [2] = "xingxinghuang",
    [3] = "xingxinghong",
    [4] = "xingxinghong",
}
function FabaoView:FabaoView()
    -- body
end
function FabaoView:OnClickClose()
    ViewMgr:CloseView(FabaoView)
end
function FabaoView:OnClickLottery()
    ViewMgr:OpenView(FabaoLottery)
end

----------------------------------FabaoPanel--------------------------------------

FabaoPanel = FabaoPanel or DeclareMono("FabaoPanel", UIWFlushPanel)
--LotRedP==============UIWSubstitute[UIWRedPoint2]
--RedInitiative========UIWSubstitute[UIWRedPoint2]
--RedPassivity=========UIWSubstitute[UIWRedPoint2]
--LvupFlyStar===========FlyStarEffectCell
function FabaoPanel:FabaoPanel()
    self.data = FabaoData.Instance
    self.data_cares = {
        {data = self.data:ViewData(), func = self.FlushList, keys = {"select"}},
        {data = self.data.qiling_info, func = self.FlushList, init_call = false},
        {data = self.data:GetFabaoList(), func = self.FlushList, init_call = false},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Task), func = self.FlushCost},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushCost},
        {data = self.data:GetFabaoList(), func = self.FlushCost, init_call = false},
        {data = self.data:GetFabaoList(), func = self.FlushAttr, init_call = false},
        {data = self.data:GetFabaoList(), func = self.FlushEffect, init_call = false},
        {data = self.data:ViewData(), func = self.OnClickItem, keys = {"select_data"},init_call = false},

        {data = FabaoLotteryData.Instance:DrawInfo(), func = self.OnCheckLotRemind},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Material), func = self.OnCheckLotRemind},

        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Task), func = self.OnCheckTabRemind},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.OnCheckTabRemind},
        {data = self.data:GetFabaoList(), func = self.OnCheckTabRemind},
    }
    -- self.select_data = nil
    self.fabao_effect = {
        2161028,
        2161029,
        2161030,
        2161031,
    }
    self.LvupFlyStar:SetData({
        mono = self,
        start_effect = "2161041",
        complete_effect = "2161042",
        pre_pause = 1,
        end_func = BindTool.Bind1(self.OnFlyStartPlayComplete, self),
        end_func_time = 0  
    })
    self:RemindQiling()
end

function FabaoPanel:OnClickFetters()
    ViewMgr:OpenView(FabaoFetters)
end
function FabaoPanel:OnClickFettersPanel()
    if not self.FettersPanel.activeSelf then
        self:FlushFetters()
    end
    self.FettersPanel:SetActive(not self.FettersPanel.activeSelf)
end
function FabaoPanel:FlushFetters()
    local info = self.data:FettersInfo(self.data:ViewData().select_data.id)
    UH.SetText(self.FettersName, info.fetter_name)
    local next_jie = self.data:WillUpJie(info.fetter_group)
    if self.data:MaxJie() < next_jie then
        UH.SetText(self.FettersTip, Language.Fabao.FettersMax)
    else
        UH.SetText(self.FettersTip, string.format(Language.Fabao.FettersTip, DataHelper.GetDaXie(next_jie)))
    end
    local list = self.data:FettersGroup(info.fetter_group)
    self.FettersList:SetData(list)
end
function FabaoPanel:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[125].desc})
end

function FabaoPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    -- self.select_data = nil
    self.data:ViewData().select = 1
    self.data:ViewData().select_data = nil
    Remind.Instance:UnRegisterGroup(TabMod(Mod.Fabao.Qiling), self.qiling_handle)
end
function FabaoPanel:onFlush()
end

function FabaoPanel:OnCheckLotRemind()
    self.LotRedP:SetNum(FabaoLotteryData.Instance:RemindDrawInfo())
end

function FabaoPanel:OnCheckTabRemind()
    local initiative,passivity = self.data:RemindTypeUpgradEnable()
    self.RedInitiative:SetNum(initiative and 1 or 0)
    self.RedPassivity:SetNum(passivity and 1 or 0)
end


function FabaoPanel:FlushList()
    local select_type = self.data:ViewData().select
    local data = DataHelper.TableCopy(self.data:GetTypeList(select_type),0)
    if data == nil then
        return
    end
    table.sort(data,function(a,b)
        local order_a = 0
        local order_b = 0
        local fetter_a = self.data:FettersInfo(a.id)
        local fetter_b = self.data:FettersInfo(b.id)
        if fetter_a.fetter_group == fetter_b.fetter_group then
            if fetter_a.talisman_sort < fetter_b.talisman_sort then
                order_a = order_a + 100
            else
                order_b = order_b + 100
            end
        else
            if fetter_a.fetter_group_sort < fetter_b.fetter_group_sort then
                order_a = order_a + 10000
            else
                order_b = order_b + 10000
            end
        end
        return order_a > order_b
    end)


    self.List:SetData(data)
    self.BlockObj:SetActive(false)
    if self.data:ViewData().select_data then 
        if self.data:ViewData().jump then       --跳转到某个法宝
            self.data:ViewData().jump = false
            self.List:ClickItemData(self.data:ViewData().select_data)
            self.List:AutoSlide(self.data:ViewData().select_data,false)
        elseif (self.data:ViewData().select_data.type == 1 and select_type ~= 1) or  --主被动法定切换，2,3都归在被动栏里。。所以只判断是不是1。。
            (self.data:ViewData().select_data.type ~= 1 and select_type == 1) then 
                self.data:ViewData().select_data = data[1]
                self.List:ClickItemData(self.data:ViewData().select_data)
                self.List:AutoSlide(self.data:ViewData().select_data,false)
        end
    else
        self.data:ViewData().select_data = data[1]
        self.List:ClickItemData(self.data:ViewData().select_data)
        self.List:AutoSlide(self.data:ViewData().select_data,false)    
    end
    self.SelectBtn[self.data:ViewData().select].isOn = true
end
function FabaoPanel:OnClickType(type_select)
    self.data:ViewData().select = type_select
end
function FabaoPanel:OnClickItem()
    -- if self.select_data ~= self.data:ViewData().select_data then
        -- self.select_data = self.data:ViewData().select_data
        local sel_data = self.data:ViewData().select_data
        if self.data:ViewData().jump == true then
            local is_zhudong_seled = (self.SelectBtn[1].isOn == true)
            if (self.data:ViewData().select_data.type == 1 and is_zhudong_seled == true) or  --主被动法定切换，2,3都归在被动栏里。。所以只判断是不是1。。
                (self.data:ViewData().select_data.type ~= 1 and is_zhudong_seled ~= true) then
                self.data:ViewData().jump = false
                -- local index = self.List:GetSelectedIndex(self.select_data)
                self.List:ClickItemData(sel_data)
                self.List:AutoSlide(sel_data,false)
                -- self.List:JumpVerticalIndex(index, 111)
            end
        end
    -- end
    -- self.data:ViewData().select_data = data
    -- AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    -- LogDG(data)
    UH.SetText(self.NameTex, sel_data.name)
    UH.SetIcon(self.Icon, sel_data.big_icon)
    local is_lock = self.data:IsUnlock(sel_data.id)
    local is_max = self.data:IsMax(sel_data.id)
    self.IconLock.enabled = is_lock
    self.LevelLock:SetActive(is_lock)

    if is_max then
        local max_add_txt = FabaoData.Instance:FabaoAttrInfo(sel_data.id).max_addition_desc
        if StringIsEmpty(max_add_txt) then
            UH.SetText(self.LevelMax,Language.Fabao.MaxLevel)
        else
            UH.SetText(self.LevelMax,max_add_txt)
        end
    else
        UH.SetText(self.LevelMax,"")
    end
    

    self.Normal:SetActive(not is_max and not is_lock)
    self:FlushCost()
    self:FlushAttr()
    self:FlushEffect()
    self:FlushFettersNum()
end
function FabaoPanel:FlushFettersNum()
    local info = self.data:FettersInfo(self.data:ViewData().select_data.id)
    local a, b = self.data:FettersNum(info.fetter_group)
    local cur_jie = self.data:WillUpJie(info.fetter_group) - 1
    UH.SetText(self.FettersNum, string.format(Language.Fabao.FettersNum, a, b))
    UH.SetText(self.FettersJie, string.format(Language.Fabao.FettersJie,DataHelper.GetDaXie(cur_jie)))
    UH.SetText(self.FettersLevel, string.format(Language.Fabao.FettersLevel,info.fetter_name, DataHelper.GetDaXie(cur_jie)))
end
function FabaoPanel:FlushEffect()
    if self.data:ViewData().select_data == nil or self.data:ViewData().select_data.info == nil then
        return
    end
    local data = self.data:ViewData().select_data
    local jie_type = math.floor(data.info.fabao_jie / 4) + 1

    -- jie_type = jie_type == 0 and 1 or jie_type
    self.LevelBg:SetByPath("fabao/FaBaoYuanPan".. jie_type ..".png")
    local is_lock = self.data:IsUnlock(data.id)
    if self.handle_effect then
        self.EffectTool:Stop(self.handle_effect)
        self.handle_effect = nil
    end
    if not is_lock then
        self.handle_effect = self.EffectTool:Play(self.fabao_effect[jie_type])
    end
end
function FabaoPanel:FlushAttr()
    if self.data:ViewData().select_data == nil or self.data:ViewData().select_data.info == nil then
        return
    end
    local data = self.data:ViewData().select_data
    local id = data.id
    local is_lock = self.data:IsUnlock(id)
    local upgrade, next_upgrade
    local cur, next, cur2, next2
    local advance = self.data:AdvanceCfg(data.id, data.info.fabao_jie)[1]
    local is_percent = advance.percentage_show == 1
    cur = is_percent and ((advance.advanced_addition / 1000) .. "%") or advance.advanced_addition / 1000
    cur2 = is_percent and ((advance.advanced_addition2 / 1000) .. "%") or advance.advanced_addition2 / 1000
    if is_lock then
        upgrade = self.data:UpgradeCfg(id, 1)
        next = ""
        next2 = ""
    else
        local is_max = self.data:IsMax(id)
        if is_max then
            upgrade = self.data:UpgradeCfg(id)
            if data.max_advanced_addition > 0 then
                next = string.format("<color=#%s>(+%d",COLORSTR.Red8,data.max_advanced_addition/1000)
                if is_percent then
                    next = next .. "%"
                end
                next = next .. ")</color>"
            else
                next = ""
            end
            if data.max_advanced_addition2 > 0 then
                next2 = string.format("<color=#%s>(+%d",COLORSTR.Red8,data.max_advanced_addition2/1000)
                if is_percent then
                    next2 = next2 .. "%"
                end
                next2 = next2 .. ")</color>"
            else
                next2 = ""
            end
        else
            local is_up_jie = self.data:IsCanUp(data.id, data.info.fabao_level)
            upgrade = self.data:UpgradeCfg(id)
            if not is_up_jie then
                next_upgrade = self.data:NextUpgradeCfg(id)
                next = ""
                next2 = ""
            else
                advance = self.data:AdvanceCfg(data.id, data.info.fabao_jie + 1)[1]
                next =
                    "(" ..
                    (is_percent and ((advance.advanced_addition / 1000) .. "%") or
                        advance.advanced_addition / 1000) ..
                        ")"

                        next2 =
                        "(" ..
                        (is_percent and ((advance.advanced_addition2 / 1000) .. "%") or
                            advance.advanced_addition2 / 1000) ..
                            ")"                        
            end

        end
    end
    self:SetAttr(upgrade[1], next_upgrade)
    UH.SetText(self.EffectDesc, string.format(data.desc_1, cur, next, cur2, next2))
end
function FabaoPanel:SetAttr(upgrade, next_upgrade)
    local attr_type
    local value
    local show_next = next_upgrade ~= nil
    next_upgrade = show_next and next_upgrade[1] or next_upgrade
    for i = 1, 2 do
        attr_type = upgrade["attr_type_" .. i]
        value = upgrade["attr_num_" .. i]
        if value > 0 then
            UH.SetText(self.AttrType[i], Language.Common.AttrList[attr_type])
            UH.SetText(self.AttrType1[i], Language.Common.AttrList[attr_type])
            UH.SetText(self.AttrValue[i],"+" .. DataHelper.Percent(attr_type, value))
            if show_next then
                UH.SetText(self.AttrValue1[i],"+" .. DataHelper.Percent(attr_type, next_upgrade["attr_num_" .. i]))
                self.ArrowObj[i]:SetActive(true)
            else
                UH.SetText(self.AttrType1[i], "")
                UH.SetText(self.AttrValue1[i], "")
                self.ArrowObj[i]:SetActive(false)
            end
        else
            self.ArrowObj[i]:SetActive(false)
            UH.SetText(self.AttrType[i], "")
            UH.SetText(self.AttrType1[i], "")
            UH.SetText(self.AttrValue[i], "")
            UH.SetText(self.AttrValue1[i], "")
        end
    end
end
function FabaoPanel:OnClickTotal()
    ViewMgr:OpenView(FabaoUpAttrView)
end
function FabaoPanel:OnClickPre()
    -- if self.select_data == nil then
    --     return
    -- end
    -- local data = self.select_data
    -- FabaoCtrl.SendActive(data.id)
    -- self.data:AttrAddition()
    ViewMgr:OpenView(FabaoUpPreView)
end
function FabaoPanel:FlushCost()
    if self.data:ViewData().select_data == nil then
        self.CostList:SetData({})
        return
    end
    local data = self.data:ViewData().select_data
    if data.info == nil or data.info.fabao_level == 0 then
        self.CostList:SetData({})
        return
    end
    local is_lock = self.data:IsUnlock(data.id)
    local is_max = self.data:IsMax(data.id)
    if is_lock or is_max then
        self.CostList:SetData({})
        return
    end
    local is_up_jie = self.data:IsCanUp(data.id, data.info.fabao_level)
    UH.SetText(self.BtnDesc, is_up_jie and Language.Fabao.UpgradeJie or Language.Fabao.UpgradeLevel)
    if not is_max and not is_lock and not is_up_jie then
        local upgrade = self.data:UpgradeCfg(data.id, data.info.fabao_level)
        local item = Item.Create({item_id = upgrade[1].consume_item, need_num = upgrade[1].consume_num})
        self.CostList:SetData({item})
    elseif not is_max and not is_lock and is_up_jie then
        local advance = self.data:AdvanceCfg(data.id, data.info.fabao_jie)
        local item
        local temp = {}
        for i = 1, 2 do
            item = Item.Create({item_id = advance[1]["consume_item_" .. i], need_num = advance[1]["consume_num_" .. i]})
            table.insert(temp, item)
        end
        self.CostList:SetData(temp)
    end
end


function FabaoPanel:OnClickUp()
    if self.data:ViewData().select_data == nil then
        return
    end
    local data = self.data:ViewData().select_data
    local is_can_up = self.data:IsCanUp(data.id, data.info.fabao_level)
    if is_can_up then
        local advance = self.data:AdvanceCfg(data.id, data.info.fabao_jie)
        local ni_id_1 = advance[1].consume_item_1
        local ni_count_1 = advance[1].consume_num_1
        local ni_num_1 = BagData.Instance:GetNumByItemId(ni_id_1)
        if ni_num_1 < ni_count_1 then
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = ni_id_1,jump_need_num = ni_count_1 - ni_num_1})})
            return
        end
        local ni_id_2 = advance[1].consume_item_2
        local ni_count_2 = advance[1].consume_num_2
        local ni_num_2 = BagData.Instance:GetNumByItemId(ni_id_2)
        if ni_num_2 < ni_count_2 then
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = ni_id_2,jump_need_num = ni_count_2 - ni_num_2})})
            return
        end
        self.EffectTool:Play(2161032)
        self.BlockObj:SetActive(true)
    else
        local upgrade = self.data:UpgradeCfg(data.id, data.info.fabao_level)
        local ni_id = upgrade[1].consume_item
        local ni_count = upgrade[1].consume_num
        local ni_num = BagData.Instance:GetNumByItemId(ni_id)
        local fetters = self.data:FettersInfo(data.id)
        local res = self.data:FettersDesc(data.id, fetters.fetter_group)
        if res == 2 then
            if ni_num < ni_count then
                ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = ni_id,jump_need_num = ni_count - ni_num})})
                return
            end
            FabaoCtrl.SendUpLevel(data.id)
            return
        else
            if ni_num < ni_count then
                ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = ni_id,jump_need_num = ni_count - ni_num})})
                return
            end
        end
        self.LvupFlyStar:Play(2161011)
        self.BlockObj:SetActive(true)
    end
end


function FabaoPanel:OnEffectEvent(handle, time, key)
    if key == "boom" then
        local sel_data = self.data:ViewData().select_data
        FabaoData.Instance:PushJieupViewInfo(sel_data.id,sel_data.info)
        ViewMgr:OpenView(FabaoJieupView)
        FabaoCtrl.SendAdvanced(sel_data.id)
    elseif key == "end" then
        -- self.BlockObj:SetActive(false)
    end
end

function FabaoPanel:OnFlyStartPlayComplete()
    self.LvupFlyStar:AllHandleCancel()
    FabaoCtrl.SendUpLevel(self.data:ViewData().select_data.id)
    -- self.EffectTool:Play(2161042)
end

function FabaoPanel:OnClickQiling()
    ViewMgr:OpenView(FaBaoQiLingView)
end

function FabaoPanel:RemindQiling()
    local mod = TabMod( Mod.Fabao.Qiling)
    if self.qiling_handle == nil then
        self.qiling_handle = Remind.Instance:RegisterGroup(mod, function()
            local num = Remind.Instance:GetGroupNum(mod)
            if self.RedQiLing == nil then
                if self.qiling_handle then
                    Remind.Instance:UnRegisterGroup(self.data.mod, self.qiling_handle)
                end
                return
            end
            self.RedQiLing:SetNum(num)
        end, true)
    end
end


---------------------------------FabaoListItem-------------------------------------

FabaoListItem = FabaoListItem or DeclareMono("FabaoListItem", UIWidgetBaseItem)
--RedPoint=======UIWSubstitute[UIWRedPoint2]
function FabaoListItem:FabaoListItem()
    self.lang = Language.Fabao
    self.fabao_data = FabaoData.Instance
end

function FabaoListItem:SetData(data)
    self.FabaoCell:SetData(data)
    UH.SetText(self.LblName, data.name)
    UH.SetText(self.LblType, self.lang.FBType[data.type])
    local info = self.fabao_data:FabaoListInfo(data.id)
    if info == nil then
        BuglyReport("法宝数据为空id=", data.id)
        return
    end
    local lock = info.fabao_index <= -1
    self.LblActive:SetActive(lock)
    data.info = info
    UH.SetText(self.LblLevel, string.format("Lv.%d", info.fabao_level))
    UH.Color(self.LblName, lock and COLORS.Gray4 or COLORS.Yellow13)
    -- UH.Color(self.LblType, lock and COLORS.Gray4 or COLORS.Yellow13)
    UH.Color(self.LblLevel, lock and COLORS.Gray4 or COLORS.Yellow15)
    UIWidgetBaseItem.SetData(self, data)
    if info.play == true then
        self.EffectTool:Play(10010259)
        info.play = false
    end
    -- 根据阶数显示星星
    local jie_level = data.info.fabao_jie
    self.JieObj:SetActive(info.fabao_level ~= 0)
    self.JieBgObj:SetActive(false)
    --每4阶换换星星皮
    --余数亮几颗星
    local a = math.floor(jie_level / 4)
    local b = jie_level % 4
    local buttom_img = sp[a]
    local fill_img = sp[a + 1]
    for i = 1, self.JieImgList:Length() do
        -- self.JieList[i]:SetActive(i <= jie_level)
        UH.SpriteName(self.JieImgList[i], i <= b and fill_img or buttom_img)--a <= 0 and sp[1] or sp[a])
        -- if b == 0 and jie_level > 0 then
        --     self.JieList2[i].enabled = false
        -- else
        --     self.JieList2[i].enabled = i > b
        -- end
    end
    local fetter = self.fabao_data:FettersInfo(data.id)
    if fetter then
        UH.SetText(self.FetterTex, fetter.fetter_name)
    end
    local attr = self.fabao_data:FabaoAttrInfo(data.id)
    self.EffectObj:SetActive(attr.corner_icon ~= "")
    UH.SetText(self.LblEffect, attr.corner_icon)
    if FabaoData.Instance:ViewData().select_data == self.data then  ---触发刷新选中的格子，用于升级时刷新法宝展示表现
        FabaoData.Instance:ViewData().select_data = nil
        FabaoData.Instance:ViewData().select_data = self.data
    end
    self.RedPoint:SetNum(FabaoData.Instance:IsRemindUpgradEnable(data.id,data.info) and 1 or 0)
    self.ObjYuling:SetActive(data.type==1 and FabaoData.Instance:IsYuLing(data.id))
end

function FabaoListItem:OnEnable()
    local bag_item_change_func = BindTool.Bind(self.OnBagItemChange,self)
    self.data_care_handle1 = BagData.Instance:GetColumnGrids(ItemColumnType.Item):Care(bag_item_change_func)
    self.data_care_handle2 = BagData.Instance:GetColumnGrids(ItemColumnType.Task):Care(bag_item_change_func)
end

function FabaoListItem:OnDisable()
    BagData.Instance:GetColumnGrids(ItemColumnType.Item):Uncare(self.data_care_handle1)
    BagData.Instance:GetColumnGrids(ItemColumnType.Task):Uncare(self.data_care_handle2)
    self.data_care_handle1 = nil
    self.data_care_handle2 = nil
end

function FabaoListItem:OnBagItemChange()
    self.RedPoint:SetNum(FabaoData.Instance:IsRemindUpgradEnable(self.data.id,self.data.info) and 1 or 0)
end

function FabaoListItem:Click()
    -- AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    -- if self.ItemClick then
    --     self.ItemClick:Trigger()
    -- end
    FabaoData.Instance:ViewData().select_data = self.data
    self.Tog.isOn = true
end

function FabaoListItem:OnClick()
    -- LogError("1111",self.data:ViewData().select_data,self.data)
    FabaoData.Instance:ViewData().select_data = self.data
end

FabaoItem = FabaoItem or DeclareMono("FabaoItem", UIWidgetBaseItem)

function FabaoItem:FabaoItem()
    self.lang = Language.Fabao
    self.fabao_data = FabaoData.Instance
end

function FabaoItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Interactor.Interactable = true
    local info = self.fabao_data:FabaoAttrInfo(data.id)
    UH.SetIcon(self.Icon, info.icon, ICON_TYPE.FABAO)
    UH.SpriteName(self.QuaIcon,COLOR_FRAME_SPRITE[info.color])
    if info.color >= ItemColor.Orange then
        UISpriteAnimTool.AnimName(self.QuaAnimTool,Format(UISpriteAnimToolConfig.ItemCell.AnimName, info.color))
        self.QuaAnimTool:SetObjActive(true)
    else
        self.QuaAnimTool:SetObjActive(false)
    end
    self.color = info.color or 0
end

function FabaoItem:MakeGrey(grey)
    self.Interactor.Interactable = not grey
    if self.color >= ItemColor.Orange then
        self.QuaAnimTool:SetObjActive(not grey)
    end
end

function FabaoItem:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.Interactor.Interactable = true
end

FabaoFettersItem = FabaoFettersItem or DeclareMono("FabaoFettersItem", UIWidgetBaseItem)
function FabaoFettersItem:FabaoFettersItem()
    self.fabao_data = FabaoData.Instance
end
function FabaoFettersItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local info = self.fabao_data:FabaoAttrInfo(data.id)
    UH.SetText(self.Name, info.name)
    local desc = self.fabao_data:FettersDesc(data.id, data.fetter_group)
    UH.SetText(self.Tip, Language.Fabao.FettersDesc[desc])
end
function FabaoFettersItem:OnClickJump()
    local data = self.fabao_data:FabaoAttrInfo(self.data.id)
    local info = self.fabao_data:FabaoListInfo(self.data.id)
    data.info = info
    self.fabao_data:ViewData().jump = true
    self.fabao_data:ViewData().select = (data.type > 2 and 2 or data.type)
    self.fabao_data:ViewData().select_data = data
    self.Panel:SetActive(false)
end