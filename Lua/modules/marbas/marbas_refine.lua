MarbasRefine = MarbasRefine or DeclareMono("MarbasRefine", UIWFlushPanel)
function MarbasRefine:MarbasRefine()
    self.Data = MarbasData.Instance
    self.language = Language.Marbas.Refine
    self.data = self.Data.refine_info
    self.prof_type = RoleData.Instance:GetRoleProfType()
    self.refine_cfg = self.Data:RefineCfg()
    self.refine_resonance_cfg = self.Data:RefineResonanceCfg()
    -- 右侧属性精炼的时候需要延迟刷
    self.is_need_delay = false
    -- 记录下来，如果改变需要弹窗
    self.now_resonance_level = self.data.resonance_level
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
        {data = self.Data.left_equip_info, func = self.FlushAll, keys = {"part"}, init_call = false},
    }
    -- 刷新道具数量
    if self.item_listen == nil then
        self.item_listen = BagData.Instance:ListenItems(self.Data:RefineRemindItemList(), BindTool.Bind(self.FlushAll, self))
    end

    --抛物线1
    self.ParabolaBoard:SetData({y_offset = 2, speed_time = 2, complete_func = function()
        self.ObjBgEffect:SetActive(true)
        self.ObjBgEffect3:SetActive(false)
        self.is_need_delay = false
        TimeHelper:CancelTimer(self.ht)
        self.ht = Invoke(function()
            self.ObjBlock:SetActive(false)
        end, 1.5)
        TimeHelper:CancelTimer(self.ht3)
        self.h3 = Invoke(function()
            self.ObjBgEffect3:SetActive(true)
            self:FlushAll()
        end, 0.5)
    end})
end

function MarbasRefine:Awake()
    UIWFlushPanel.Awake(self)
end

function MarbasRefine:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.Data.left_equip_info.mod_id = Mod.Marbas.Refine
    self.ObjBgEffect:SetActive(false)
    self.ObjBgEffect2:SetActive(false)
    self.ObjBgEffect3:SetActive(false)
    self.ObjBlock:SetActive(false)
    self.is_need_delay = false
    self:FlushAll()
end

function MarbasRefine:FlushAll()
    if self.is_need_delay then
        return
    end
    self.Data.left_equip_info:SetDirty("flush_flag")
    local now_part = self.Data.left_equip_info.part
    local now_equip = self.Data.equip_list[now_part] or {}
    if TableIsEmpty(now_equip) then
        UH.SetText(self.TxtName, self.language.NoWare)
    else
        UH.SetText(self.TxtName, now_equip:Name())
    end
    if self.data.resonance_level > 0 then
        UH.SetText(self.TxtGrade, Format(self.language.Grade[1], self.data.resonance_level))
    else
        UH.SetText(self.TxtGrade, self.language.Grade[2])
    end
    self:FlushBg()
    self:FlushButtom()
    self:FlushRight()
    if self.now_resonance_level ~= self.data.resonance_level then
        TimeHelper:CancelTimer(self.ht2)
        self.ht2 = Invoke(function()
            ViewMgr:OpenView(MarbasRefineResonanceView,{
                last = self.refine_resonance_cfg[self.now_resonance_level] or {}, 
                now = self.refine_resonance_cfg[self.data.resonance_level], 
            })
            self.now_resonance_level = self.data.resonance_level
        end, 1)
    end
end

function MarbasRefine:FlushBg()
    local now_part = self.Data.left_equip_info.part
    local now_equip = self.Data.equip_list[now_part] or {}

    local bg_path
    if TableIsEmpty(now_equip) then
        bg_path = self.Data:BigIconPath(1, now_part)
    else
        bg_path = self.Data:BigIconPath(now_equip:EquipGrade(), now_part)
    end
    ResMgr:LoadObject(bg_path, function(tex)
        if not self:Destroyed() then
            if self.Icon then
                self.Icon.texture = tex
            end
        end
    end, false)
    self.IconHandle.Interactable = not TableIsEmpty(now_equip)
end

function MarbasRefine:FlushButtom()
    local next_resonance_cfg = self.refine_resonance_cfg[self.data.resonance_level + 1] or {}
    if TableIsEmpty(next_resonance_cfg) then
        UH.SetText(self.TxtNextGrage, self.language.NextGrade[2])
    else
        UH.SetText(self.TxtNextGrage, Format(self.language.NextGrade[1], next_resonance_cfg.refine_level_all, self.data.num))
    end

    for i = 1, self.Progress:Length() do
        UH.SpriteName(self.Progress[i], self.data.num >= i and "mb_JinDuTiao1" or "mb_JinDuTiao2")
    end

    local now_refine_resonance_cfg = self.refine_resonance_cfg[self.data.resonance_level] or {}
    if self.data.resonance_level == 0 then
        now_refine_resonance_cfg = self.refine_resonance_cfg[1]
        self.DownList:SetData(self.Data:RefineResonanceAttrData(now_refine_resonance_cfg, COLORSTR.Gray2))
    else
        self.DownList:SetData(self.Data:RefineResonanceAttrData(now_refine_resonance_cfg))
    end
end

function MarbasRefine:FlushRight()
    local now_part = self.Data.left_equip_info.part
    local now_equip = self.Data.equip_list[now_part] or {}
    local now_level = self.data.info[now_part]
    local is_no_ware = TableIsEmpty(now_equip)
    UH.SetText(self.TxtLevel, is_no_ware and self.language.NoWare or Format(self.language.Level, now_level))

    local now_refine_cfg = self.refine_cfg[now_part][now_level] or {}
    local next_refine_cfg = self.refine_cfg[now_part][now_level + 1] or {}

    self.NowList:SetData(self.Data:RefineAttrData(now_refine_cfg))
    self.NowList:SetObjActive(not TableIsEmpty(now_refine_cfg))
    self.NextList:SetData(self.Data:RefineAttrData(next_refine_cfg))
    self.NextList:SetObjActive(not TableIsEmpty(next_refine_cfg))

    self.MaxList:SetObjActive(TableIsEmpty(next_refine_cfg))
    if TableIsEmpty(next_refine_cfg) then
        self.MaxList:SetData(self.Data:RefineAttrData(now_refine_cfg))
    end

    local remind_num, not_type, not_param1, not_param2 = self.Data:RefinePartRemind(now_part)
    self.IHandler.Interactable = not is_no_ware
    self.Remind:SetNum(remind_num)

    local next_refine_cfg = self.refine_cfg[now_part][now_level + 1] or {}
    UH.SetText(self.TxtBtn, self.language.Btn[TableIsEmpty(next_refine_cfg) and 2 or 1])
    self.ConsumeList:SetData(self:ConsumeItemList(next_refine_cfg))
    self.ObjNotWare:SetActive(remind_num == 0)
    self.ObjMaxShow:SetActive(TableIsEmpty(next_refine_cfg))

    if remind_num == 0 then
        if not_type == 3 then
            UH.SetText(self.TxtNotWare, Format(self.language.CannotTip[not_type], not_param1))
        else
            UH.SetText(self.TxtNotWare, self.language.CannotTip[not_type])
        end
    end
end

function MarbasRefine:ConsumeItemList(next_refine_cfg)
    local t = {}
    if not TableIsEmpty(next_refine_cfg) then
        if next_refine_cfg.refine_consum_1 > 0 then
            local has = Item.GetNum(next_refine_cfg.refine_consum_1)
            table.insert(t, Item.Init(next_refine_cfg.refine_consum_1, DataHelper.ConsumeNum(has, next_refine_cfg.refine_consum_num_1), 1))
        end
        if next_refine_cfg.refine_consum_2 > 0 then
            local has = Item.GetNum(next_refine_cfg.refine_consum_2)
            table.insert(t, Item.Init(next_refine_cfg.refine_consum_2, DataHelper.ConsumeNum(has, next_refine_cfg.refine_consum_num_2), 1))
        end
    end
    return t
end

function MarbasRefine:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    BagData.Instance:RemoveListenItems(self.item_listen)
    TimeHelper:CancelTimer(self.ht)
    TimeHelper:CancelTimer(self.ht2)
    TimeHelper:CancelTimer(self.ht3)
end

function MarbasRefine:OnShowClick()
    if GetActive(self.ObjResonanceShow) then
        self:CloseShow()
    else
        self.ObjResonanceShow:SetActive(true)
        local refine_data = self:RefineResonanceItemList()
        self.ResonanceList.StepCount = #refine_data
        self.ResonanceList:SetCompleteCallBack(function()
            local index = self.data.resonance_level > 0 and self.data.resonance_level or 1
            self.ResonanceList:AutoSlide(refine_data[index], false)
        end)
        self.ResonanceList:SetData(refine_data)
    end
end

-- 从下一级到上一级增加了多少，要自己减
function MarbasRefine:RefineResonanceItemList()
    local cfg = {}
    for i, v in ipairs(self.refine_resonance_cfg) do
        -- if v.resonance_stage == 1 then
        --     table.insert(cfg, {info = v, attr = self.Data:RefineResonanceAttrData(v, v.resonance_stage > self.data.resonance_level and COLORSTR.Gray2 or nil)})
        -- else
        --     local t = {info = v}
        --     local last_cfg = self.refine_resonance_cfg[i - 1]
        --     local attr = {}
        --     local index = 1
        --     while v["attr_add_"..index] do
        --         if v["attr_add_"..index] > 0 then
        --             local attr2 = CommonStruct.AttrStruct(v["attr_type_"..index], v["attr_add_"..index] - last_cfg["attr_add_"..index])
        --             attr2.color = v.resonance_stage > self.data.resonance_level and COLORSTR.Gray2 or nil
        --             table.insert(attr, attr2)
        --         end
        --         index = index + 1
        --     end
        --     t.attr = attr
        --     table.insert(cfg, t)
        -- end
        -- 干掉，现在不需要减了，加多少就显示多少
        table.insert(cfg, {info = v, attr = self.Data:RefineResonanceAttrData(v, v.resonance_stage > self.data.resonance_level and COLORSTR.Gray2 or nil)})
    end
    return cfg
end

function MarbasRefine:CloseShow()
    self.ObjResonanceShow:SetActive(false)
end

function MarbasRefine:OnRefineClick()
    local now_part = self.Data.left_equip_info.part
    local remind_num, not_type, not_param1, not_param2 = self.Data:RefinePartRemind(now_part)
    if remind_num == 1 then
        self.ObjBlock:SetActive(true)
        self.ObjBgEffect:SetActive(false)
        self.ObjBgEffect2:SetActive(false)
        self.ObjBgEffect2:SetActive(true)
        self.ObjBgEffect3:SetActive(false)
        self.ParabolaBoard:Play()
        self.is_need_delay = true
        MarbasCtrl.Instance:SendEquipOperPing(MarbasReqTypeP.Refine, self.Data.left_equip_info.part)
    else
        if not_type == 3 then
            PublicPopupCtrl.Instance:Center(Format(self.language.CannotTip[not_type], not_param1))
        elseif not_type == 4 then
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = not_param1, jump_need_num = not_param2})})
        else
            PublicPopupCtrl.Instance:Center(self.language.CannotTip[not_type])
        end
    end
end

--------------MarbasRefineAttrItem--------------
MarbasRefineAttrItem = MarbasRefineAttrItem or DeclareMono("MarbasRefineAttrItem", UIWidgetBaseItem)
function MarbasRefineAttrItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if data.color then
        UH.SetText(self.TxtAttrName, ColorStr(data.attr_name, data.color))
        UH.SetText(self.TxtAttrValue, ColorStr(Format(Language.Marbas.Refine.AttrAdd, data.attr_percent_value), data.color))
    else
        UH.SetText(self.TxtAttrName, data.attr_name)
        UH.SetText(self.TxtAttrValue, Format(Language.Marbas.Refine.AttrAdd, data.attr_percent_value))
    end
    if self.ObjRoot and data.index then
        self.ObjRoot.name = "MarbasRefineAttrItem ("..data.index..")"
    end
end

--------------MarbasRefineCellItem--------------
MarbasRefineCellItem = MarbasRefineCellItem or DeclareMono("MarbasRefineCellItem", UIWidgetBaseItem)
function MarbasRefineCellItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data)
    self.Cell:SetData(item)
    UH.SetText(self.TxtName, item:Name())
end

--------------MarbasRefineAddItem--------------
MarbasRefineAddItem = MarbasRefineAddItem or DeclareMono("MarbasRefineAddItem", UIWidgetBaseItem)
function MarbasRefineAddItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtDesc, Format(Language.Marbas.Refine.GradeAdd, DataHelper.GetDaXie(data.info.resonance_stage), data.info.refine_level_all))
    self.AttrList:SetData(data.attr)
end


--------------MarbasRefineResonanceView--------------
MarbasRefineResonanceView = MarbasRefineResonanceView or DeclareView("MarbasRefineResonanceView", "marbas/marbas_refine_resonance")
VIEW_DECLARE_MASK(MarbasRefineResonanceView, ViewMask.BgBlockClose)
function MarbasRefineResonanceView:MarbasRefineResonanceView()
    self.Data = MarbasData.Instance
    self.language = Language.Marbas.Refine
end

function MarbasRefineResonanceView:LoadCallback(param_t)
    self.LastList:SetData(self.Data:RefineAttrData(param_t.last))
    self.LastList:SetObjActive(not TableIsEmpty(param_t.last))
    self.NowList:SetData(self.Data:RefineAttrData(param_t.now))
    self.NowList:SetObjActive(not TableIsEmpty(param_t.now))
    UH.SetText(self.TxtGrade, Format(self.language.Grade[1], self.Data.refine_info.resonance_level))
    UH.SetText(self.TxtDesc, Format(self.language.ResonanceAdd, param_t.now.refine_level_all))
end