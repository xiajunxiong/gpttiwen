LiveView = LiveView or DeclareView("LiveView", "live/live_view", Mod.Live.Main)
VIEW_DECLARE_LEVEL(LiveView, ViewLevel.L0)
function LiveView:LiveView()
    self.data = LiveData.Instance
    self.Curr:SetCurrency(CurrencyType.CoinBind,CurrencyType.GuildCotribution)

    GuideManager.Instance:RegisterGetGuideUi("LiveCurrInfo",function ()
        return self.LiveCurr
    end)
    GuideManager.Instance:RegisterGetGuideUi("TabListInfo",function ()
        return self.TabGuide
    end)
end

function LiveView:LoadCallback(param_t)
    
end

function LiveView:CloseCallback()
end

function LiveView:OnCloseClick()
    ViewMgr:CloseView(LiveView)
end

function LiveView:CloseCallback()
    self.data:SetSelSkillType(-1)
    self.data.sel_left_item_cell.data = {}
    self.data.view_open_info = {tab_index = -1, is_refine = false, item_id = nil, level = nil}
    self.data.other_data.heigth = 0
    self.data:SetSelSeason(0)
    self.data:SetSelCookType(1)
end

function LiveView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[194].desc)
end

LivePanel = LivePanel or DeclareMono("LivePanel", UIWFlushPanel)
function LivePanel:LivePanel()
    self.tab_list_data = Config.life_system_auto.life_skill -- 是个配置
    self.data = LiveData.Instance
    self.data_cares = {
        {data = self.data.skill_all_info, func = self.FlushPanel, init_call = false},
        {data = self.data.select_tab, func = self.FlushPanel, init_call = false},
    }
    GuideManager.Instance:RegisterGetGuideUi("LiveTab6",function ()
        return self.GuideTab6,function ()
            self.TabItems[6]:OnClick()
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("LiveContentInfo",function ()
        return self.ContentObj
    end)
end

function LivePanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function LivePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetSelSkillType(-1)
    TimeHelper:CancelTimer(self.time_ht)
end

function LivePanel:FlushPanel()
    local skill_type = self.data:GetSelSkillType()
    for i = 1, self.TabItems:Length() do
        local mono = self.TabItems[i]
        mono:SetData(self.tab_list_data[i])
    end

    if self.data.view_open_info.tab_index > 0 then
        TimeHelper:CancelTimer(self.time_ht)
        self.time_ht = Invoke(function ()
            local index = self.data.view_open_info.tab_index
            self.data.view_open_info.tab_index = -1
            self.TabItems[index]:OnClick()
        end,1)
        if self.anim_tab then
            self.data:SetSelSkillType(-1)
        end
    end

    if skill_type >= 0 then
        local cfg,tab_index = self.data:GetSkillTypeCfg(skill_type)
        if self.anim_tab ~= tab_index then
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ToggleOn)
            self.anim_tab = tab_index
            self.Animator:Play("TabOpen")
        end
    else
        if self.anim_tab then
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ToggleIn)
            self.anim_tab = nil
            self.Animator:Play("TabClose")
        end
    end
end

-- ============= 标签页 ==========
LiveTabItem = LiveTabItem or DeclareMono("LiveTabItem", UIWidgetBaseItem)
function LiveTabItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.name)
    local skill_level = LiveData.Instance:GetSkillTypeLevel(data.skill_type)
    if LiveCtrl.LOCK_SKILL_TYPE[data.skill_type] then
        UH.SetText(self.Level, Language.Live.Tip1)
    else
        UH.SetText(self.Level, string.format(Language.Live.Level, skill_level))
    end
    self.UpInfoObj:SetActive(LiveData.Instance:GetSelSkillType() == data.skill_type)
    --local up_cfg = LiveData.Instance:GetLevelAdditionCfg(skill_level)
    --UH.SetText(self.StuffNum, up_cfg.up_expend) -- 294 123 +47
    local flag = LiveData.Instance:GetSkillUpFlag(data.skill_type)
    self.Interactor.Interactable = not LiveCtrl.LOCK_SKILL_TYPE[data.skill_type]
    self.BtnInteractor.Interactable = flag ~= -1
    UH.SetText(self.Title, data.skill_type > 3 and Language.Live.Skill1 or Language.Live.Skill2)

    self.Stuff:SetActive(false)--(flag ~= -1) -- 屏蔽不显示
    UH.SetText(self.BtnTxt, flag == -1 and Language.Live.Txt7 or Language.Live.Txt6)

    UH.SpriteName(self.Icon, "Skill" .. data.skill_type)

    local is_pick=data.skill_type==LiveCtrl.LIVE_SKILL_TYPE.PICK_FLOWER
    self.ObjSeason:SetActive(is_pick)
    if is_pick then
        local season = LiveData.Instance:GetSeason()
        UH.SetText(self.TxtSeason,Language.Live.Season[season])
        if LiveData.Instance:GetSelSkillType() == data.skill_type then
            UH.SetRectPosition(self.RectSeason, {y = -103})
        else
            UH.SetRectPosition(self.RectSeason, {y = -135})
        end
    end
end

function LiveTabItem:OnClick()
    if LiveCtrl.LOCK_SKILL_TYPE[self.data.skill_type] then
        PublicPopupCtrl.Instance:Center(Language.Live.Tip2)
        return
    end
    if LiveData.Instance.view_open_info.tab_index > 0 then
        return
    end
    if LiveData.Instance:GetSelSkillType() >= 0 and not LiveData.Instance.view_open_info.is_refine then
        LiveData.Instance:SetSelSkillType(-1)
        return
    end
    LiveData.Instance.sel_left_item_cell.data = {}
    LiveData.Instance:SetSelSkillType(self.data.skill_type)
end

function LiveTabItem:OnUpClick()
    
end


LiveContentPanel = LiveContentPanel or DeclareMono("LiveContentPanel", UIWFlushPanel)
function LiveContentPanel:LiveContentPanel()
    self.data = LiveData.Instance
    self.data_cares = {
        {data = self.data.select_tab, func = self.FlushPanel, init_call = false},
        {data = self.data.sel_left_item_cell, func = self.FlushRight, init_call = false},
        {data = self.data.skill_all_info, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false},
        {data = LiveData.Instance.active_recipe_flag, func = self.FlushPanel, init_call = false},
        {data = self.data.other_data, func = self.FlushOther, init_call = false},
    }
    GuideManager.Instance:RegisterGetGuideUi("LiveContentClose",function ()
        return self.CloseBtn,function ()
            self:OnBackClick()
        end
    end)
end

function LiveContentPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function LiveContentPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
end

function LiveContentPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_ht)
end

function LiveContentPanel:FlushPanel()
    local sel_skill = self.data:GetSelSkillType()
    if sel_skill == nil or sel_skill < 0 then
        return 0
    end
    self.CloseBtn:SetActive(not self.data.view_open_info.is_refine)
    self:FlushLeft()
    self:FlushRight()
    local tab_data = self.data:GetSkillTypeCfg(sel_skill)
    self.TabItem:SetData(tab_data)

    local is_pick = self.data:GetSelSkillType() == LiveCtrl.LIVE_SKILL_TYPE.PICK_FLOWER
    local is_cook = self.data:GetSelSkillType() == LiveCtrl.LIVE_SKILL_TYPE.COOK
    self.ObjSeason:SetActive(is_pick)
    self.ObjCook:SetActive(is_cook)
    if is_pick or is_cook then
        self.RectLeftList.sizeDelta = Vector2.New(460, 380)
        UH.SetRectPosition(self.RectLeftList, {y = -41})
    else
        self.RectLeftList.sizeDelta = Vector2.New(460, 426)
        UH.SetRectPosition(self.RectLeftList, {y = -18})
    end
    if is_cook then
        self.TabCook[self.data:GetSelCookType()].isOn = true;
    end
    if is_pick and  self.TabSeason[self.data:GetSelSeason()] then
        self.TabSeason[self.data:GetSelSeason()].isOn = true;
    end
end

function LiveContentPanel:FlushOther()
    local height = self.data.other_data.height
    if height and height > 0 then
        self.LeftList:ForceJumpVertical(height)
    end
    -- 打开新解锁展示
    local left_data = self.data.other_data.left_data
    if not TableIsEmpty(left_data) then
        local param_t = {}
        param_t.title = 3
        local item_list = {}
        for i,v in pairs(left_data.list_data) do
            local item_id = v.item_id
            local item = Item.Create({item_id = item_id})
            table.insert(item_list, item)
        end
        param_t.item_list = item_list
        if left_data.level < LiveData.Instance:GetMaxLevel() then
            param_t.open_view = LiveUpLevelView
        else
            PublicPopupCtrl.Instance:Center(Language.Live.Tip9)
        end
        ViewMgr:OpenView(LiveRefineResultView, param_t)
        ViewMgr:CloseView(LiveUpLevelView)
    end
end

function LiveContentPanel:FlushLeft()
    if self.data:GetSelSkillType() < 0 then
        return
    end
    self.ConSizeFitter.enabled = false
    local list_data = self.data:GetContentLeftListData()
    self.LeftList:SetData(list_data)
    self.LeftList:SetCompleteCallBack(function()
        self.ConSizeFitter.enabled = true
        if self.data.view_open_info.item_id then
            TimeHelper:CancelTimer(self.time_ht)
            self.time_ht = Invoke(function()
                local jump_index
                for i, v in pairs(list_data) do
                    if self.data.view_open_info.level then
                        if v.level == self.data.view_open_info.level then
                            jump_index = i
                            break
                        end
                    end
                    for _, v2 in pairs(v.list_data) do
                        if v2.item_id == self.data.view_open_info.item_id then
                            jump_index = i
                            break
                        end
                    end
                    if jump_index and jump_index > 1 then
                        local height = self:GetJumpHeight(jump_index)
                        self.LeftList:ForceJumpVertical(height)
                        self.data.view_open_info.item_id = nil
                        self.data.view_open_info.level = nil
                        break
                    end
                end
            end, 0.2)
        end
    end)
end

function LiveContentPanel:FlushRight()
    if TableIsEmpty(self.data.sel_left_item_cell.data) then
        return
    end
    if not self.data.select_tab.skill_type or self.data.select_tab.skill_type < 0 then
        return
    end
    local data = self.data.sel_left_item_cell.data
    local cfg = data.cfg
    local skill_type = cfg.limit_skill or cfg.work_type or cfg.skill_type
    local skill_status = self.data:GetSkillStatus(skill_type)
    self.Right1:SetObjActive(skill_status == LiveData.SKILL_STATUS.SKILL)
    self.Right2:SetObjActive(skill_status == LiveData.SKILL_STATUS.JOB)
    if skill_status == LiveData.SKILL_STATUS.SKILL then
        self.Right1:SetData(data)
    else
        self.Right2:SetData(data)
    end
end

function LiveContentPanel:OnBackClick()
    self.data:SetSelSkillType(-1)
end

function LiveContentPanel:OnUpClick()
    if not self.data:IsInGuild() then
        PublicPopupCtrl.Instance:Error(Language.Live.Tip8)
        return
    end
    local skill_type = self.data:GetSelSkillType()
    -- 快速点击标签页并点击升级按钮会出现
    if not skill_type or skill_type < 0 then
        return
    end
    local flag = LiveData.Instance:GetSkillUpFlag(skill_type)
    if flag == -1 then
        PublicPopupCtrl.Instance:Error(Language.Live.Tip3)
        return
    end

    ViewMgr:OpenView(LiveUpLevelView)

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
end

function LiveContentPanel:GetJumpHeight(jump_index)
    local list_data = self.LeftList:GetData()
    jump_index = jump_index > #list_data - 2 and #list_data - 2 or jump_index
    local h = 0
    for i = 1, jump_index - 1 do
        local data = list_data[i]
        if #data.list_data > 4 then
            h = h + 150 + (math.floor(#data.list_data / 4) * 100)
        else
            h = h + 150
        end
    end
    return h
end

function LiveContentPanel:OnChangeSeason(season)
    if self.data:GetSelSeason() == season then
        self.data:SetSelSeason(0)
    else
        self.data:SetSelSeason(season)
    end
    self.data.sel_left_item_cell.data = nil 
    self:FlushLeft()
end

function LiveContentPanel:OnChangeCookType(type)
    LiveData.Instance:SetSelCookType(type)
    self.data.sel_left_item_cell.data = nil
    self:FlushLeft()
end

LiveContentLeftItem = LiveContentLeftItem or DeclareMono("LiveContentLeftItem", UIWidgetBaseItem)
-- data : {level , list_data : [{item_id,cfg}]}
function LiveContentLeftItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Level, data.title and data.title or string.format(Language.Live.Level, data.level))
    self.ItemList:SetData(data.list_data)
    if TableIsEmpty(LiveData.Instance.sel_left_item_cell.data) then
        if LiveData.Instance.view_open_info.item_id then
            local target_level = LiveData.Instance.view_open_info.level
            if (target_level and data.level == target_level) or not target_level then
                local jump_item_id = LiveData.Instance.view_open_info.item_id
                for i, v in pairs(data.list_data) do
                    if v.item_id == jump_item_id then
                        LiveData.Instance.sel_left_item_cell.data = data.list_data[i]
                        self.ItemList:SetectItem(i, true)
                        break
                    end
                end
            end
        else
            LiveData.Instance.sel_left_item_cell.data = data.list_data[1] or {}
            self.ItemList:SetectItem(1, true)
        end
    end
    local item_height = 50 + math.ceil(#data.list_data/4)*100
    self.RectTrans:SetSizeWithCurrentAnchors(1, item_height)
end

LiveContentLeftItemCell = LiveContentLeftItemCell or DeclareMono("LiveContentLeftItemCell", UIWidgetBaseItem)
-- data : {item_id,cfg}
function LiveContentLeftItemCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local cfg = data.cfg
    local skill_type = cfg.limit_skill or cfg.work_type
    local limit_level = cfg.limit_skill_level or cfg.work_level
    local item_id = data.item_id or 0
    local is_lock = LiveData.Instance:GetIsLock(skill_type, limit_level)
    if not is_lock then
        is_lock = LiveData.Instance:IsRecipeLock(cfg)
    end
    local item = Item.Create({item_id = item_id})
    self.LockObj:SetActive(is_lock)
    self.Cell:SetData(item)
    UH.SetText(self.CellName, item:Name())
    if data.need_num ~= nil and data.need_num ~= 0 then
        UH.SetText(self.CellNum,Item.GetNum(item_id).."/"..data.need_num )
    else
        UH.SetText(self.CellNum, "")
    end
    -- 解锁特效
    if not is_lock and item_id == self.item_id and self.is_lock then
        self.EffectTool:Play("3161050")
    end
    self.is_lock = is_lock
    self.item_id = item_id
end

function LiveContentLeftItemCell:OnLeftItemCellClick()
    LiveData.Instance.sel_left_item_cell.data = self.data
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
end


LiveContentRight1 = LiveContentRight1 or DeclareMono("LiveContentRight1", UIWidgetBaseItem)
-- data : {item_id,cfg}
function LiveContentRight1:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create({item_id = data.item_id})
    local cfg = data.cfg
    self.ItemCell:SetData(item)
    UH.SetText(self.ItemName, item:Name())
    local level = LiveData.Instance:GetSkillTypeLevel(cfg.limit_skill)
    local add_num = LiveData.Instance:GetLevelAdditionCfg(level).pro_double_rate or 0
    local skill_cfg = LiveData.Instance:GetSkillTypeCfg(cfg.limit_skill)
    add_num = add_num / 10 .. "%"

    local scene_id = data.cfg.scene_id
    local multiple,next_multiple_cfg = LiveData.Instance:GetSceneMultiple(scene_id, level)
    multiple = string.format(Language.Live.MultipleDesc, multiple)
    local next_multiple = ""
    if next_multiple_cfg then
        next_multiple = string.format(Language.Live.NextMultipleDesc, next_multiple_cfg.gather_skill_level,next_multiple_cfg.multiple)
    end

    UH.SetText(self.ItemDesc, item:Desc() .. string.format(Language.Live.ItemDesc,skill_cfg.name, add_num) .. multiple .. next_multiple)
    UH.SetText(self.SceneName, SceneData.Instance:GetSceneCfg(cfg.scene_id).name)
    UH.SetText(self.ConditionDesc, string.format(Language.Live.ConditionDesc, skill_cfg.name, cfg.limit_skill_level))
    UH.SetText(self.Num, cfg.expend_active)
    UH.SetText(self.BtnText, string.format(Language.Live.Txt1,skill_cfg.name))
    local level = LiveData.Instance:GetSkillTypeLevel(cfg.limit_skill)
    self.Interactor.Interactable = level >= cfg.limit_skill_level
end

function LiveContentRight1:OnGotoSceneClick()
    local level = LiveData.Instance:GetSkillTypeLevel(self.data.cfg.limit_skill)
    if level < self.data.cfg.limit_skill_level then
        PublicPopupCtrl.Instance:Error(Language.Live.Tip5)
        return
    end
    if self.data.cfg.season ~= 0 and self.data.cfg.season ~= LiveData.Instance:GetSeason() then
        PublicPopupCtrl.Instance:Error(Language.Live.TipSeason)
        return
    end
    if LiveCtrl.Instance:CheckEnergy(self.data.cfg.expend_active) then
        local scene_id = self.data.cfg.scene_id
        local scene_cfg = LiveData.Instance:GetSceneCfg(scene_id)
        if scene_cfg then
            BehaviorCtrl.ExeByCfgVo(BehaviorCtrl.GetCfg(scene_cfg.began_to_fight, true))
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.LoginStartClick)
            ViewMgr:CloseView(LiveView)
        end
    end
end


LiveContentRight2 = LiveContentRight2 or DeclareMono("LiveContentRight2", UIWidgetBaseItem)
function LiveContentRight2:LiveContentRight2()
    self.ctrl_data = LiveData.Instance
    GuideManager.Instance:RegisterGetGuideUi("LiveUseBtn2",function ()
        return self.UseBtn, function ()
            if not self:OnGotoSceneClick() then
                GuideManager.Instance:ForceStop()
            end
        end
    end)
end

function LiveContentRight2:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create({item_id = data.item_id})
    local cfg = data.cfg
    self.skill_type = cfg.work_type
    self.ItemCell:SetData(item)
    UH.SetText(self.ItemName, item:Name())
    local recipe_cfg = self.ctrl_data:GetRecipeCfgByItemId(data.item_id) or {}
    local desc = recipe_cfg.life_des
    if not desc or desc == "" then
        desc = item:Desc()
    end
    UH.SetText(self.ItemDesc, desc)
    local skill_cfg = LiveData.Instance:GetSkillTypeCfg(cfg.work_type)
    UH.SetText(self.ConditionText, string.format(Language.Live.ConditionDesc, skill_cfg.name, cfg.work_level))
    self.add_num = self.add_num or 1
    self.max_num = self:GetMaxNum()
    local level = LiveData.Instance:GetSkillTypeLevel(cfg.work_type)
    self.Interactor.Interactable = level >= cfg.work_level
    self.AddBtnObj:SetActive(LiveData.Instance.view_open_info.is_refine)
    self.GotoBtnObj:SetActive(false)--(not LiveData.Instance.view_open_info.is_refine)  -- 暂时屏蔽前往按钮
    UH.SetText(self.GotoTxt, string.format(Language.Live.Txt1, self.skill_type == LiveCtrl.LIVE_SKILL_TYPE.MEDICINE and Language.Live.Npc1 or Language.Live.Npc2))
    UH.SpriteName(self.Icon, self.skill_type == LiveCtrl.LIVE_SKILL_TYPE.MEDICINE and "DanLu" or "ZaoTai")
    self:FlushStuff()
    self:FlushTip()
    local is_recipe_lock=LiveData.Instance: IsRecipeLock(cfg)
    self.BtnGetRecipe:SetActive(is_recipe_lock)
    self.UseBtn:SetActive(not is_recipe_lock)
    self.ObjNum:SetActive(not is_recipe_lock)
end

function LiveContentRight2:FlushStuff()
    local list_data = self:GetStuffList(self.add_num)
    self.stuff_list_data = list_data
    self.StuffList:SetData(list_data)
    UH.SetText(self.Count, self.add_num or 1)
    self.need_num = self.data.cfg.expend_active * self.add_num
    UH.SetText(self.NeedNum, self.need_num)
end

function LiveContentRight2:FlushTip()
    local skill_type = self.data.cfg.work_type
    local level = LiveData.Instance:GetSkillTypeLevel(skill_type)
    local skill_cfg = LiveData.Instance:GetSkillTypeCfg(skill_type)
    local add_num = LiveData.Instance:GetLevelAdditionCfg(level).pro_double_rate or 0
    add_num = add_num / 10 .. "%"
    add_num = ColorStr(add_num, COLORSTR.Green3)
    local type_name = skill_type == LiveCtrl.LIVE_SKILL_TYPE.MEDICINE and Language.Live.Npc1 or Language.Live.Npc2
    local skill_info = LiveData.Instance:GetSkillTypeInfo(skill_type)
    local count_str = string.format("(%s/%s)", skill_info.trigger_double_count, LiveData.Instance:GetTriggerMaxCount())
    count_str = ColorStr(count_str, COLORSTR.Red3)
    local tip_str = string.format(Language.Live.AdditionDesc, type_name, add_num,count_str, skill_cfg.name)
    UH.SetText(self.TipText, tip_str)
end

function LiveContentRight2:GetStuffList(num)
    num = num or 1
    local cfg = self.data.cfg
    local len = cfg.materials_num
    local list = {}
    for i = 1, len do
        local item_id = cfg["materials_" .. i]
        local stuff_num = cfg["num" .. i] * num
        local has_num = Item.GetNum(item_id)
        local num_str = DataHelper.ConsumeNum(has_num, stuff_num, true)
        local item = Item.Create({item_id = item_id, num = num_str})
        if has_num < stuff_num then
            item.need_num = stuff_num - has_num
        end
        table.insert(list, item)
    end
    return list
end

function LiveContentRight2:GetMaxNum()
    local energy = LiveData.Instance:GetEnergy()
    local need_num = self.data.cfg.expend_active or 1
    local max_num = math.floor(energy / need_num)
    if max_num > 10 then
        max_num = 10
    end
    return max_num
end

function LiveContentRight2:OnGotoSceneClick()
    local cfg = self.data.cfg
    local level = LiveData.Instance:GetSkillTypeLevel(cfg.work_type)
    if level < cfg.work_level then
        PublicPopupCtrl.Instance:Error(Language.Live.Tip6)
        return
    end
    if not LiveCtrl.Instance:CheckEnergy(self.need_num) then
        return
    end
    -- local need_item_list = {}
    if self.stuff_list_data then
        for i,v in pairs(self.stuff_list_data) do
            if v.need_num and v.need_num > 0 then
                -- local item = Item.Create({item_id = v.item_id, num = v.need_num})
                -- table.insert(need_item_list, item)
                local item_id = v.item_id
                MainOtherCtrl.Instance:GetWayView({item = Item.New({item_id = item_id})})
                return
            end
        end
    end
    -- if not TableIsEmpty(need_item_list) then
    --     if not LiveData.Instance.live_quick_buy_flag then
    --         PublicPopupCtrl.Instance:Center(Language.Live.Tip7)
    --         ViewMgr:OpenView(LiveQuickBuy, {item_list = need_item_list})
    --         return
    --     else
    --         for i,v in pairs(need_item_list) do
    --             local info = ShopData.GetFastBuyData(v.item_id)
    --             if info then
    --                 local price = info.price * v.num
    --                 if not MallCtrl.IsNotCurrency(info.money_type, price) then
    --                     return
    --                 end
    --                 RoleCtrl.FastPurchase(v.item_id, v.num)
    --             end
    --         end
    --     end
    -- end

    local skill_type = self.data.cfg.work_type
    if not self.ctrl_data.view_open_info.is_refine then
        ViewMgr:OpenView(LiveRefineView, {cfg = self.data.cfg, count = self.add_num})
        return true
    else
        LiveCtrl.Instance:UseRefine(skill_type, self.data.cfg.sn, self.add_num)
        ViewMgr:CloseView(LiveView)
    end
end

function LiveContentRight2:OnMaxClick()
    self.add_num = self.max_num
    self:FlushStuff()
end

function LiveContentRight2:OnAddClick(num)
    self.add_num = self.add_num + num
    if self.add_num < 1 then
        self.add_num = 1
    end
    if self.add_num > self.max_num then
        self.add_num = self.max_num
        TimeHelper:CancelTimer(self.add_ht)
        PublicPopupCtrl.Instance:Center(string.format(Language.Live.Txt2, self.max_num))
    end
    self:FlushStuff()
end

function LiveContentRight2:OnAddDown(num)
    if not self.is_down then
        TimeHelper:CancelTimer(self.add_ht)
        self.is_down = true
        self.add_ht = TimeHelper:AddRunTimer(BindTool.Bind(self.OnAddClick,self, num),0.2)
    end
end

function LiveContentRight2:OnAddUp()
    self.is_down = false
    TimeHelper:CancelTimer(self.add_ht)
end

function LiveContentRight2:OnDestroy()
    UIWidgetBaseItem.OnDestroy(self)
    TimeHelper:CancelTimer(self.add_ht)
end

function LiveContentRight2:OnAdditionClick()
    local active = not self.TipObj.activeSelf
    self.TipObj:SetActive(active)
end

function LiveContentRight2:OnGotoNpcClick()
    local npc_id = 0
    if self.skill_type == LiveCtrl.LIVE_SKILL_TYPE.MEDICINE then
        npc_id = self.ctrl_data.live_cfg.other[1].npc_danlu
    else
        npc_id = self.ctrl_data.live_cfg.other[1].npc_pengren
    end
    SceneLogic.Instance:AutoToNpcs(npc_id)
    ViewMgr:CloseView(LiveView)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
end

function LiveContentRight2:OnClickGetRecipe()
    local item_id=self.data.cfg.unlock_item
    MainOtherCtrl.Instance:GetWayView({item = Item.New({item_id = item_id})})
end

LiveCurr = LiveCurr or DeclareMono("LiveCurr", UIWFlushPanel)
function LiveCurr:LiveCurr()
    self.data_cares = {
        {data = LiveData.Instance.gather_energy, func = self.FlushPanel, init_call = true},
    }
end

function LiveCurr:FlushPanel()
    local remind_num = LiveData.Instance:GetRemind()
    self.RedPoint:SetNum(remind_num)
    -- 上限
    local power_max = Config.collect_scene_auto.other[1].power_max
    UH.SetText(self.Value, string.format("%s/%s",LiveData.Instance.gather_energy.val,power_max))
end

function LiveCurr:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    local remind_num = LiveData.Instance:GetRemind()
    if remind_num > 0 then
        LiveData.Instance.gather_energy.is_remind = false
    end
end

function LiveCurr:OnAddExpClick()
    ViewMgr:OpenView(PetStampResolveSetView)
end

function LiveCurr:OnAddClick()
    ViewMgr:OpenView(ItemConsumeTipView,{item_id = LiveCtrl.CurrItemId})
end

-- 生活道具物品信息
LivePropItemInfo = LivePropItemInfo or DeclareMono("LivePropItemInfo", ItemInfoViewItem)

function LivePropItemInfo:OnEnable()
    ItemInfoViewItem.OnEnable(self)
    UH.SetText(self.Title, Language.Live.Txt3)
    self.item = self.info.item_list[self.ShowType]
    self:FlushPanel()
end

function LivePropItemInfo:FlushPanel()
    local prop_seq = self.item:Config().param1
    local attri_list = LiveData.Instance:GetAttriListByCardSeq(prop_seq) or {}
    self.AttriList:SetData(attri_list)
end


-- 属性加成预览
LiveAttriPreview = LiveAttriPreview or DeclareView("LiveAttriPreview", "live/live_attri_preview")
VIEW_DECLARE_MASK(LiveAttriPreview,ViewMask.BlockClose)
function LiveAttriPreview:LoadCallback(param_t)
    local list_data = self:GetListData()
    self.List:SetData(list_data)
    self.Scroll.enabled = #list_data > 3
    local h = #list_data * 60 + 20
    h = h > 200 and 200 or h
    self.ListTrans:SetSizeWithCurrentAnchors(1, h)
end

function LiveAttriPreview:GetListData()
    local prop_all_info = LiveData.Instance.prop_use_all_info
    local list = {}
    for i,v in pairs(prop_all_info) do
        if v.seq > 0 then
            table.insert(list, v)
        end
    end
    return list
end

LiveAttriPreviewItem = LiveAttriPreviewItem or DeclareMono("LiveAttriPreviewItem", UIWidgetBaseItem)
function LiveAttriPreviewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local seq = data.seq
    local cfg = LiveData.Instance:GetPropCfg(seq)
    self.Time:SetObjActive(cfg.type ~= LiveCtrl.PROP_ITEM_TYPE.CARD)
    self.PointText:SetObjActive(cfg.type == LiveCtrl.PROP_ITEM_TYPE.CARD)
    local title
    if cfg.type == LiveCtrl.PROP_ITEM_TYPE.CARD then
        title = string.format(Language.Live.Txt4,cfg.name)
        local num = LiveData.Instance:GetCardPoint(cfg.seq)
        UH.SetText(self.PointText, string.format(Language.Live.Txt13,num))
    else
        title = string.format(Language.Live.Txt5,cfg.name)
        self.Time:CloseCountDownTime()
        local timestamp = data.end_timestamp
        self.Time:StampTime(timestamp,TimeType.Type_Time_7)
    end
    UH.SetText(self.Title, title)
    local attri_list = LiveData.Instance:GetAttriListByCardSeq(seq)
    local attri_str = ""
    for i,v in pairs(attri_list) do
        local info = CommonStruct.AttrStruct(v.attri_type, v.attri_value)
        attri_str = attri_str .. info.attr_name .. "+" .. info.attr_percent_value .. "    "
    end
    UH.SetText(self.Desc, attri_str)
end

function LiveAttriPreviewItem:OnDestroy()
    self.Time:CloseCountDownTime()
end
