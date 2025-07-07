-- ============= 幻兽主界面 ==============
HuanShouView = HuanShouView or DeclareView("HuanShouView", "huanshou/huanshou_view", Mod.HuanShou.Main)
VIEW_DECLARE_LEVEL(HuanShouView, ViewLevel.L0)
function HuanShouView:HuanShouView()
    self.data = HuanShouData.Instance
    local lang = Language.HuanShou.TabText
    self.tab_list_data = {
        {name = lang[1][1], name2 = lang[1][2], tab_objs = {self.ListInfo, self.ModelInfo, self.AttriPanel}, open = Mod.HuanShou.Attri, mod = TabMod(Mod.HuanShou.Attri), is_self_img = true},
        {name = lang[2][1], name2 = lang[2][2], tab_objs = {self.ListInfo, self.ModelInfo, self.SkillPanel}, open = Mod.HuanShou.Skill, mod = TabMod(Mod.HuanShou.Skill), is_self_img = true},
        {name = lang[3][1], name2 = lang[3][2], tab_objs = {self.ListInfo, self.StarPanel}, open = Mod.HuanShou.Star, mod = TabMod(Mod.HuanShou.Star), is_self_img = true},
        {name = lang[4][1], name2 = lang[4][2], tab_objs = {self.EggPanel}, open = Mod.HuanShou.Egg, mod = TabMod(Mod.HuanShou.Egg), is_self_img = true},
    }
    self.TabbarList:SetData(self.tab_list_data)
    self.Board = {Tabbar = self.TabbarList}
end

function HuanShouView:OnTipClick()
    local tab_data = self.data:GetSelTab()
    local open = tab_data.open
    if not open then
        return
    end
    local cfg = {
        [Mod.HuanShou.Attri] = 362,
        [Mod.HuanShou.Skill] = 366,
        [Mod.HuanShou.Star] = 367,
        [Mod.HuanShou.Egg] = 368,
    }
    PublicPopupCtrl.Instance:HelpTip(cfg[open] or 0)
end

function HuanShouView:LoadCallback(param_t)
    param_t = param_t or {}
    local index = self:GetTabIndex(param_t.mod_key or 0)
    self.TabbarList:ClickItem(index)
end

function HuanShouView:CloseCallback()
    self.data:SelTab(nil)
end

function HuanShouView:GetTabIndex(mod_key)
    for i,v in pairs(self.tab_list_data) do
        if v.open == mod_key then
            return i
        end
    end
    return 1
end

function HuanShouView:OnTabClick(data)
    self.data:SelTab(data)
end


function HuanShouView:OnCloseClick()
    ViewMgr:CloseView(HuanShouView)
end


HuanShouListInfo = HuanShouListInfo or DeclareMono("HuanShouListInfo", UIWFlushPanel)
HuanShouListInfo.save_model_list = {}
function HuanShouListInfo:HuanShouListInfo()
    self.data = HuanShouData.Instance
    self.qua_list_data = {
        {qua = ItemColor.Blue},
        {qua = ItemColor.Purple},
        {qua = ItemColor.Orange},
        {qua = ItemColor.Red},
        {qua = ItemColor.Gold},
    }
    self.is_need_sel_item = 1
    self.data_cares = {
        {data = self.data.sel_huanshou, func = self.FlushPanel, init_call = false},
        {data = self.data.other_data, func = self.FlushPanel, init_call = false},
        {data = self.data.sel_tab, func = self.FlushPanel, init_call = false},
    }
    self.load_index = 0
    self.load_model_list = {}
end

function HuanShouListInfo:Awake()
    UIWFlushPanel.Awake(self)
    self.ItemList:SetCompleteCallBack(function ()
        local jump_data = self.data:GetJumpHuanShou()
        local jump_index = jump_data.jump_index
        self.is_need_sel_item = jump_index or self.is_need_sel_item
        if self.is_need_sel_item then
            self.ItemList:SetectItem(self.is_need_sel_item, true)
            self.is_need_sel_item = nil
        end
        self.data:JumpHuanShou(nil)
        self.load_model_list = {}
        for i,v in pairs(self.ItemList.item_list) do
            local data = v:GetData()
            local res_id = data:ResId()
            if not HuanShouListInfo.save_model_list[res_id] then
                table.insert(self.load_model_list, v)
            end
        end
        self.load_index = 1
    end)

    -- 一下子加载过多模型会卡顿，所以这样设计
    self.load_time_ht = TimeHelper:AddRunTimer(function ()
        local mono = self.load_model_list[self.load_index]
        if mono then
            mono:ShowModel()
            self.load_index = self.load_index + 1
        end
    end, 0.5)
end

function HuanShouListInfo:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushPanel()
end

function HuanShouListInfo:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetSelHuanShou({})
    TimeHelper:CancelTimer(self.load_time_ht)
    HuanShouListInfo.save_model_list = {}
end

function HuanShouListInfo:FlushPanel()
    self:FlushQuaList()
    self:FlushHuanShouList()
    self:FlushOther()
end

function HuanShouListInfo:FlushQuaList()
    local jump_data = self.data:GetJumpHuanShou()
    local jump_qua = jump_data.jump_qua
    if jump_qua then
        local jump_qua_index = 1
        for i,v in pairs(self.qua_list_data) do
            if v.qua == jump_qua then
                jump_qua_index = i
                break
            end
        end
        self.QuaList:SetData({})
        self.QuaList:SetData(self.qua_list_data)
        self.QuaList:SetectItem(jump_qua_index, true)
    else
        self.QuaList:SetData(self.qua_list_data)
        if not self.sel_tab then
            self.QuaList:SetectItem(1, true)
        end
    end
end

function HuanShouListInfo:FlushHuanShouList()
    if not self.sel_tab then
        return
    end
    local sel_huan_shou = self.data:GetSelHuanShou()
    local qua = self.sel_tab.qua
    local list = self.data:GetHuanShouList(qua)
    if not TableIsEmpty(sel_huan_shou) then
        for i,v in pairs(list) do
            if v.id == sel_huan_shou.id then
                self.is_need_sel_item = i
                break
            end
        end
    end
    self.ItemList:SetData(list)
end

function HuanShouListInfo:FlushOther()
    self.FuShenFlag:SetActive(not self.data:IsShowFuShen())
end

function HuanShouListInfo:OnQuaSelClick(data)
    self.sel_tab = data
    self.ItemList:SetData({})
    self.is_need_sel_item = 1
    self:FlushHuanShouList()
end

function HuanShouListInfo:OnUseFuShenClick()
    local p = self.data:IsShowFuShen() and 0 or 1
    HuanShouCtrl.Instance:SendReq(HuanShouReqType.ShowModel, p)
end

function HuanShouListInfo:OnResolveClick()
    ViewMgr:OpenView(HuanShouSoulResolve)
end

HuanShouListInfoTabItem = HuanShouListInfoTabItem or DeclareMono("HuanShouListInfoTabItem", UIWidgetBaseItem)
function HuanShouListInfoTabItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local qua = data.qua
    local name = Language.HuanShou.QuaStr[qua]
    UH.SetText(self.Text1, name)
    UH.SetText(self.Text2, name)
    self.XianImg:SetActive(data.qua ~= 5)

    local huanshou_list = HuanShouData.Instance:GetHuanShouList(qua)
    local tab_data = HuanShouData.Instance:GetSelTab()
    local open = tab_data.open
    local red_num = 0
    for i,v in pairs(huanshou_list) do
        if open == Mod.HuanShou.Attri then
            red_num = HuanShouData.Instance:GetAttriOneRemind(v)
        elseif open == Mod.HuanShou.Star then
            red_num = HuanShouData.Instance:GetStarOneRemind(v)
        end
        if red_num > 0 then
            break
        end
    end
    self.RedPoint:SetNum(red_num)
end

HuanShouListInfoItem = HuanShouListInfoItem or DeclareMono("HuanShouListInfoItem", UIWidgetBaseItem)
function HuanShouListInfoItem:HuanShouListInfoItem()
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    end
end

-- data = HuanShou
function HuanShouListInfoItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if HuanShouListInfo.save_model_list[self.data:ResId()] then
        self:ShowModel()
    end
    local qua = data:Qua()
    UH.SpriteName(self.BGImg, "FuTiDiKuang" .. qua)
    UH.SetText(self.Level, string.format(Language.HuanShou.Level2, data:Level()))
    UH.SetText(self.Name, data:Name())
    self.Flag:SetActive(data:IsFuShen())
    self.Lock:SetActive(not data:IsActive())
    self.LimitFlag:SetActive(not data:IsFuShen() and data:IsLimit())
    self:FlushRedPoint()
end

function HuanShouListInfoItem:FlushRedPoint()
    self.RedPoint:SetNum(0)
    local tab_data = HuanShouData.Instance:GetSelTab()
    local open = tab_data.open
    if open == Mod.HuanShou.Attri then
        self.RedPoint:SetNum(HuanShouData.Instance:GetAttriOneRemind(self.data))
    elseif open == Mod.HuanShou.Star then
        self.RedPoint:SetNum(HuanShouData.Instance:GetStarOneRemind(self.data))
    end
end

function HuanShouListInfoItem:ShowModel()
    local res_id = self.data:ResId()
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(res_id))
    HuanShouListInfo.save_model_list[res_id] = true
end

function HuanShouListInfoItem:OnDestroy()
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
	end
end

function HuanShouListInfoItem:OnSelClick()
    HuanShouData.Instance:SetSelHuanShou(self.data)
end




HuanShouModelInfo = HuanShouModelInfo or DeclareMono("HuanShouModelInfo", UIWFlushPanel)
function HuanShouModelInfo:HuanShouModelInfo()
    self.data = HuanShouData.Instance
    self.data_cares = {
        {data = self.data.sel_huanshou, func = self.FlushPanel, init_call = false},
        {data = self.data.other_data, func = self.FlushPanel, init_call = false}
    }
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    end
end

function HuanShouModelInfo:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function HuanShouModelInfo:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
	end
end

function HuanShouModelInfo:FlushPanel()
    local huanshou = self.data:GetSelHuanShou()
    if TableIsEmpty(huanshou) then
        return
    end
    self.huanshou = huanshou

    local res_id = huanshou:ResId()
    if res_id ~= self.last_id then
        self.ui_obj:SetData(DrawerHelper.GetNpcPath(res_id))
        self.ui_obj:PlayAnim(ChAnimType.Show)
        self.last_id = res_id
    end

    local qua = huanshou:Qua()
    --UH.SpriteName(self.QuaImg, "_LocJieType" .. qua)

    self.FuShenOn:SetActive(huanshou:IsFuShen())
    self.FuShenOff:SetActive(not huanshou:IsFuShen())

    self.LimitFlag:SetActive(huanshou:IsLimit())
end

function HuanShouModelInfo:OnFuShenClick()
    self.data:CtrlFuShen(self.huanshou)
end





-- 幻兽技能格子
HuanShouSkillCell = HuanShouSkillCell or DeclareMono("HuanShouSkillCell", UIWidgetBaseItem)--7165477
function HuanShouSkillCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local skill_id = data.skill_id
    self.Show:SetActive(skill_id > 0)
    self.Lock:SetActive(skill_id <= 0)
    if skill_id > 0 then
        UH.SetIcon(self.Icon, data:Icon(), ICON_TYPE.SKILL)
        UH.SetText(self.Name, data:Name())
        local qua = data:Qua()
        local level = data:Level()
        local skill_type = data.skill_type
        self.Qua:SetObjActive(skill_type == HuanShouData.SkillType.Book)
        self.Qua2:SetObjActive(skill_type ~= HuanShouData.SkillType.Book)
        if skill_type == HuanShouData.SkillType.Book then
            UH.SpriteName(self.Qua, ShengQiCfg.SkillQua[qua])
        else
            UH.SpriteName(self.Qua2, "ESkillQua" .. qua)
            UH.SetText(self.Level, "Lv" .. level)
        end
        self.NewFlag:SetActive(data.is_new == true)
        self.TypeFlag:SetActive(skill_type == HuanShouData.SkillType.Book and self.IsNotType ~= true)
        self.Useless:SetActive(data.useless == true)
    else
        self.LockLock:SetActive(data.not_lock_lock == nil or self.NotLockLock == nil)
        self.AddEffect:SetActive(data.is_add_effect == true and self.IsAddEffect == true)
    end
end

function HuanShouSkillCell:OnShowClick()
    if not self.IsCanClick or self.data.skill_id < 1 then
        return
    end
    local param = {
        skill = self.data
    }
    ViewMgr:OpenView(HuanShouSkillTip, param)
end

function HuanShouSkillCell:OnLockClick()
    if not self.IsCanClick or self.data.skill_id > 0 then
        return
    end
    PublicPopupCtrl.Instance:Center(Language.HuanShou.Tip1)
end



-- 幻兽技能详情
HuanShouSkillTip = HuanShouSkillTip or DeclareView("HuanShouSkillTip", "huanshou/huanshou_skill_tip")
VIEW_DECLARE_MASK(HuanShouSkillTip, ViewMask.BlockClose)
HuanShouSkillTip.ShowType = {
    Normal = 0,
    NotBook = 1,
    Study = 2,
}
function HuanShouSkillTip:LoadCallback(param_t)
    param_t = param_t or {}
    local skill = param_t.skill
    self.skill = skill
    if not skill then
        return
    end

    self.show_type = param_t.show_type or HuanShouSkillTip.ShowType.Normal
    self.NotTip:SetActive(self.show_type == HuanShouSkillTip.ShowType.NotBook)
    local is_show_btn = self:IsShowBtn()
    self.Btn:SetActive(is_show_btn)
    if is_show_btn then
        if self.show_type == HuanShouSkillTip.ShowType.NotBook then
            local item_id = self.skill:ItemId()
            local qua = Item.GetColor(item_id)
            if qua > ItemColor.Purple then
                UH.SetText(self.BtnText, Language.HuanShou.Text22)
            else
                UH.SetText(self.BtnText, Language.HuanShou.SkillShowTypeBtn[self.show_type] or "")
            end
        else
            UH.SetText(self.BtnText, Language.HuanShou.SkillShowTypeBtn[self.show_type] or "")
        end
    end
    
    UH.SetText(self.LblName, skill:Name())
    UH.SetText(self.LblDesc, string.format(Language.HuanShou.Text2,skill:Desc()))
    self.SkillItem:SetData(skill)
    if param_t.pos ~= nil  then
        self:CheckPos(param_t.pos)
        UH.LocalPos(self.PosRect,param_t.pos)
    end
end

function HuanShouSkillTip:IsShowBtn()
    return self.show_type == HuanShouSkillTip.ShowType.NotBook or self.show_type == HuanShouSkillTip.ShowType.Study
end

function HuanShouSkillTip:CheckPos(pos)
    if pos.x > 340 then
        pos.x = 340
    end
    if pos.x < -340 then
        pos.x = -340
    end
    if pos.y > 88 then
        pos.y = 88
    end
    if pos.y < -450 then
        pos.y = -450
    end
end

function HuanShouSkillTip:OnBtnClick()
    if self.show_type == HuanShouSkillTip.ShowType.NotBook then
        local item_id = self.skill:ItemId()
        local qua = Item.GetColor(item_id)
        if qua > ItemColor.Purple then
            PublicPopupCtrl.Instance:Center(Language.HuanShou.Tip8)
        else
            MarketData.Instance:SetJumpIndex(self.skill:ItemId())
            ViewMgr:OpenViewByKey(Mod.Mall.Market)
        end
    elseif self.show_type == HuanShouSkillTip.ShowType.Study then
        HuanShouSkillStudy.book_skill = self.skill
        local huanshou = HuanShouData.Instance:GetSelHuanShou()
        if not TableIsEmpty(huanshou) and huanshou:IsActive() then
            local book_skill_list = huanshou:BookSkillList()
            for i,v in pairs(book_skill_list) do
                if v.skill_id == self.skill.skill_id then
                    PublicPopupCtrl.Instance:Error(Language.HuanShou.Tip2)
                    return
                end
            end
            ViewMgr:OpenView(HuanShouSkillStudy)
        else
            PublicPopupCtrl.Instance:Error(Language.HuanShou.Tip3)
        end
    end
    ViewMgr:CloseView(HuanShouSkillTip)
end