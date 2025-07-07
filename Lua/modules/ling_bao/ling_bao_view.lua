LingBaoView = LingBaoView or DeclareView("LingBaoView", "ling_bao/ling_bao_view", Mod.LingBao.Main)
VIEW_DECLARE_LEVEL(LingBaoView, ViewLevel.L1)
VIEW_DECLARE_MASK(LingBaoView, ViewMask.BgBlock)
function LingBaoView:LingBaoView()
end

function LingBaoView:LoadCallback(param_t)
end

function LingBaoView:CloseCallback()
    ViewMgr:CloseView(LingBaoResonanceView)
end

function LingBaoView:OnCloseClick()
    ViewMgr:CloseView(LingBaoView)
end

function LingBaoView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[LingBaoData.Instance:MainDesc()].desc)
end

----------------------------LingBaoPanel----------------------------
LingBaoPanel = LingBaoPanel or DeclareMono("LingBaoPanel", UIWFlushPanel)
function LingBaoPanel:LingBaoPanel()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
    self.data = self.Data.info_data
    self.item_list = self.Data:ItemList()
    self.equip_list = self.Data:EquipList()
    self.data_cares = {
        {data = self.data, func = self.FlushLvUp, init_call = false},
        {data = self.item_list, func = self.FlushAll, init_call = false},
        {data = self.equip_list, func = self.FlushAll, init_call = false},
        {data = self.Data.ling_bao_flag, func = self.FlushLingBaoSelect, init_call = false},
        {data = RoleData.Instance.currencys, func = self.FlushAll, init_call = false},
    }
    -- 刷新道具数量
    if self.item_listen == nil then
        self.item_listen = BagData.Instance:ListenItems(self.Data:LingBaoRemindItemList(), BindTool.Bind(self.FlushAll, self))
    end

    for i = 1, self.ParabolaBoards:Length() do
        local x_offset = i < 4 and -2 or 2
        self.ParabolaBoards[i]:SetData({x_offset = x_offset, speed_time = 20, complete_func = function()
            self.ObjTargetEffects[i]:SetActive(false)
            self.ObjTargetEffects[i]:SetActive(true)
        end})
    end
end

function LingBaoPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.left_data, self.left_change = self.Data:ShengHenSort()
    self.now_select_shenghen = self.left_data[1].shenghen_type
    self.LeftList:SetData(self.left_data)
    self.LeftList:SetectItem(self.left_change[self.now_select_shenghen], true)
    self:FlushBg()
    self:FlushAll()
end

function LingBaoPanel:FlushLvUp()
    -- 判断一键升级后播特效
    local lingbao_str_change = self.Data:GetLingBaoStrChange(self.now_select_shenghen)
    for k, v in pairs(lingbao_str_change) do
        if v > 0 then
            self.ParabolaBoards[k + 1]:Play()
            self.Data:SetLingBaoStrChange(self.now_select_shenghen, k, 0)
        end
    end
    self:FlushAll()
end

function LingBaoPanel:SetTargetEffect(value)
    for i = 1, self.ObjTargetEffects:Length() do
        self.ObjTargetEffects[i]:SetActive(value)
    end
end

function LingBaoPanel:FlushAll()
    self.Data:MainShengHenType(self.now_select_shenghen)
    self.now_shenghen_info = self.Data:InfoData(self.now_select_shenghen)
    for i, v in ipairs(self.LeftList.item_list) do
        v:FlushShow()
    end
    UH.SetText(self.TxtScore, self.Data:ShengHenScore(self.now_select_shenghen))
    self.StartUpRemind:SetNum(self.Data:LingBaoLvRemind(self.now_select_shenghen))
    self.BagRemind:SetNum(self.Data:BagRemind(self.now_select_shenghen))
    self.AwakeRemind:SetNum(self.Data:ShengHenStrRemind(self.now_select_shenghen))
    UH.SetText(self.TxtLv, Format(self.language.Lv, self.now_shenghen_info.level))
    self.SkillList:SetData(self.Data:ShengHenSkillData(self.now_select_shenghen))

    self:FlushLingBaoStr()
    for i = 1, self.LingBaoCenterItems:Length() do
        self.LingBaoCenterItems[i]:SetData(self.Data:EquipList(self.now_select_shenghen, i - 1))
    end
end

function LingBaoPanel:FlushLingBaoStr()
    local is_all_full = self.Data:IsAllFullStr(self.now_select_shenghen)
    self.ObjStrAll:SetActive(not is_all_full)
    if not is_all_full then
        local spend_item = self.Data:LingBaoStrItem()
        local has_num = self.Data:LingBaoStrItemNum()
        local need_num = self.Data:LingBaoStrAllNeed(self.now_select_shenghen)
        local is_can = self.Data:LingBaoIsCanStrAll(self.now_select_shenghen)
        self.StrAllRemind:SetNum(is_can and 1 or 0)
        UH.SetIcon(self.ImgSpend, Item.GetIconId(spend_item), ICON_TYPE.ITEM)
        local color = has_num >= need_num and COLORSTR.Green4 or COLORSTR.Red8
        UH.SetText(self.TxtSpend, Format(Language.Common.Cstr, color, has_num, need_num))
    end
end

function LingBaoPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    BagData.Instance:RemoveListenItems(self.item_listen)
    TimeHelper:CancelTimer(self.bg_handler)
end

function LingBaoPanel:ShengHenItemSelect(mono, value)
    local mono_data = mono:GetData()
    if value then
        self.now_select_shenghen = mono_data.shenghen_type
    end
    self:FlushAll()
    self:FlushBg()
end

function LingBaoPanel:FlushBg()
    if self.bg_effect then
        UH.StopEffect(self.BgEffect, self.bg_effect)
    end
    TimeHelper:CancelTimer(self.bg_handler)
    self.bg_handler = TimeHelper:AddDelayFrameTimer(function()
        self.bg_effect = UH.PlayEffect(self.BgEffect, LingBaoConfig.ShengHenBg[self.now_select_shenghen])
    end)
end

function LingBaoPanel:OnResonanceClick()
    ViewMgr:OpenView(LingBaoResonanceView, {shenghen_type = self.now_select_shenghen})
end

function LingBaoPanel:OnStarUpClick()
    ViewMgr:OpenView(LingBaoLvUpView)
end

function LingBaoPanel:OnBagClick()
    ViewMgr:OpenView(LingBaoBagView)
end

function LingBaoPanel:OnAwakeClick()
    ViewMgr:OpenView(LingBaoAwakeView)
end

function LingBaoPanel:OnSpendShow()
    ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = self.Data:LingBaoStrItem()})})
end

function LingBaoPanel:OnStrAllClick()
    local is_can, str, p1, p2, p3 = self.Data:LingBaoIsCanStrAll(self.now_select_shenghen)
    if is_can then
        local dialog_info = {
            content = self.language.StrAllTip,
            confirm = {
                func = function()
                    self:SetTargetEffect(false)
                    LingBaoCtrl.Instance:SendReq(LingBaoReqType.LingBaoStrUpAll1, self.now_select_shenghen)
                    ViewMgr:CloseView(DialogTipsView)
                end
            },
            bg_v2 = Vector2.New(446,280),
            today_v2 = Vector2.New(-70,-16.3),
            btn_v2 = Vector2.New(0,-81.3),
            content_v2 = Vector2.New(0, 43),
            new_tnr = DTNewTodayNotRemind.LingBaoStrAll,
        }
        PublicPopupCtrl.Instance:DialogTips(dialog_info)
    else
        PublicPopupCtrl.Instance:Center(str)
        if p1 == 5 and p2 and p3 then
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = p2, jump_need_num = p3})})
        end
    end
end

function LingBaoPanel:OnLingBaoItemClick(mono)
    local mono_data = mono:GetData()
    if TableIsEmpty(mono_data.item) then
        local item_list = self.Data:ItemListByType(mono_data.lingbao_type)
        if TableIsEmpty(item_list) then
            ViewMgr:OpenView(LingBaoGetWayView)
        else
            ViewMgr:OpenView(LingBaoBagView)
        end
    else
        self.Data:MainLingBaoType(mono_data.lingbao_type)
        mono:SetSelectAct(true)
        ViewMgr:OpenView(LingBaoStrUpView)
    end
end

function LingBaoPanel:FlushLingBaoSelect()
    for i = 1, self.LingBaoCenterItems:Length() do
        self.LingBaoCenterItems[i]:SetSelectAct(false)
    end
end

--------------ShengHenItem--------------
ShengHenItem = ShengHenItem or DeclareMono("ShengHenItem", UIWidgetBaseItem)
function ShengHenItem:ShengHenItem()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
end

function ShengHenItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ObjLine:SetActive(data.order ~= 1)
    UH.SetText(self.TxtTitle, data.name)
    UH.SpriteName(self.DownIcon, "LeftShengHenDown" .. data.shenghen_type)
    UH.SpriteName(self.UpIcon, "LeftShengHenUp" .. data.shenghen_type)
    self:FlushShow()
end

function ShengHenItem:FlushShow()
    if TableIsEmpty(self.data) then
        return
    end
    local is_unlock = self.Data:ShengHenIsUnLock(self.data.shenghen_type)
    self.ObjLock:SetActive(not is_unlock)
    self.IHandler.Interactable = is_unlock
    self.Remind:SetNum(self.Data:ShengHenRemind(self.data.shenghen_type))
end

function ShengHenItem:OnUnLockClick()
    if self.data.open_condition > 0 then
        local need_shenghen = self.Data:ShengHenSort(self.data.open_condition)
        PublicPopupCtrl.Instance:Center(Format(self.language.ShengHenLockTip, need_shenghen.name, self.data.param))
    end
end

--------------LingBaoItem--------------
LingBaoItem = LingBaoItem or DeclareMono("LingBaoItem", UIWidgetBaseItem)
function LingBaoItem:LingBaoItem()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
end

function LingBaoItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if (not TableIsEmpty(data)) and data.item_id and data.item_id > 0 then
        local item = LingBao.New(data)
        UH.SpriteName(self.ImgBg, "LingBaoPinZhi" .. item:Color())
        UH.SetIcon(self.ImgIcon, item:IconId())
        self.ImgIcon:SetObjActive(true)
        self.ObjState:SetActive(true)
        self.StarList:SetObjActive(true)
        UH.SetText(self.TxtState, Format(self.language.ItemState, DataHelper.GetDaXie(item:StateLevel())))
        self.StarList:SetData(item:StarData())
    else
        UH.SpriteName(self.ImgBg, "LingBaoPinZhi1")
        self.ImgIcon:SetObjActive(false)
        self.ObjState:SetActive(false)
        self.StarList:SetObjActive(false)
    end
end

--------------LingBaoStarItem--------------
LingBaoStarItem = LingBaoStarItem or DeclareMono("LingBaoStarItem", UIWidgetBaseItem)
function LingBaoStarItem:LingBaoStarItem()
end

--------------LingBaoCenterItem--------------
LingBaoCenterItem = LingBaoCenterItem or DeclareMono("LingBaoCenterItem", UIWidgetBaseItem)
function LingBaoCenterItem:LingBaoCenterItem()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
    self:SetSelectAct(false)
    UH.SetText(self.TxtPartName, self.language.LingBaoName[self.LingBaoType])
    UH.SpriteName(self.ImgEmpty, "Empty"..self.LingBaoType)
end

function LingBaoCenterItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.LingBaoItem:SetData(data.item)
    self.Remind:SetNum(self.Data:LingBaoEquipRemind(data.shenghen_type, data.lingbao_type))
    if (not TableIsEmpty(data.item)) and data.item.item_id and data.item.item_id > 0 then
        self.ObjStr:SetActive(true)
        self.ObjNoItemShow:SetActive(false)
        local str_lv = self.Data:LingBaoStrLv(data.shenghen_type, data.lingbao_type)
        local is_full = str_lv == self.Data:LingBaoMaxStrLv()
        UH.SetText(self.TxtStr, Format(is_full and self.language.StrFull or self.language.Str, str_lv))
    else
        self.ObjStr:SetActive(false)
        self.ObjNoItemShow:SetActive(true)
    end
end

function LingBaoCenterItem:SetSelectAct(value)
    self.ObjSelect:SetActive(value)
end

--------------LingBaoSkill--------------
LingBaoSkill = LingBaoSkill or DeclareMono("LingBaoSkill", UIWidgetBaseItem)
function LingBaoSkill:LingBaoSkill()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
end

function LingBaoSkill:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local get_level = data.skill_level == 0 and 1 or data.skill_level
    local skill_cfg = self.Data:ShengHenSkillCfg(data.shenghen_type, data.skill_index, get_level)
    local sp_cfg = Cfg.SpecialEffect(skill_cfg.trace_skill)
    
    UH.SetIcon(self.Icon, sp_cfg.icon_id, ICON_TYPE.BUFF)
    UH.SetText(self.TxtLv, Format(self.language.Lv, data.skill_level))
    self.ObjBlock:SetActive(data.skill_level == 0)
    local can_up, str, p1, p2 = self.Data:ShengHenSkillIsCanUp(data.shenghen_type, data.skill_index)
    if data.skill_level == 0 then
        self.ObjCanAct:SetActive(can_up)
        if (not can_up) and p1 == 3 and p2 then
            UH.SetText(self.TxtCannotAct, Format(self.language.ShengHenSkillAct, p2))
        end
        self.TxtCannotAct:SetObjActive(not can_up)
    else
        self.ObjCanAct:SetActive(false)
        self.TxtCannotAct:SetObjActive(false)
    end
    self.Remind:SetNum(can_up and 1 or 0)
end

function LingBaoSkill:OnClickSkill()
    self.Data:MainShengHenSkillIndex(self.data.skill_index)
    ViewMgr:OpenView(ShengHenSkillView)
end