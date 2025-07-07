FaBaoQiLingView = FaBaoQiLingView or DeclareView("FaBaoQiLingView", "fabao/fabao_qiling_view",Mod.Fabao.Qiling)

VIEW_DECLARE_LEVEL(FaBaoQiLingView, ViewLevel.Lt)
VIEW_DECLARE_MASK(FaBaoQiLingView, ViewMask.BgBlock)

function FaBaoQiLingView:FaBaoQiLingView()
end

function FaBaoQiLingView:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), Language.Fabao.QiLing.title, Vector2.New(1080, 616))
end

FaBaoQiLingPanel = FaBaoQiLingPanel or DeclareMono("FaBaoQiLingPanel", UIWFlushPanel)

function FaBaoQiLingPanel:FaBaoQiLingPanel()
    self.language= Language.Fabao.QiLing
    self.data = FabaoData.Instance
    self.data_cares = {
        {data = self.data.qiling_info, func = self.FlushData, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushCost},
    }
    self.List:SetData(self.data.qiling_info.qi_ling_list)
    self.List:SetSetectItem(0)
    UH.SetText(self.TxtNum, self.language.yuling_num .. self.data.qiling_info.yuling_num .. "/" .. FA_BAO_QI_LING_PUT_ON_MAX_NUM)
end

function FaBaoQiLingPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function FaBaoQiLingPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
end

function FaBaoQiLingPanel:OnClickItem(data)
    -- 选中数据改变
    self.sel_id =data.id
    self:OnSelectChange()
end

--选中器灵
function FaBaoQiLingPanel:OnSelectChange()
    self:FlushSelData()
    local info=self.data:FabaoAttrInfo(self.sel_qiling_cfg.fabao_id)
    UH.SetIcon(self.ImgQiLingIcon, info.icon, ICON_TYPE.FABAO)
    local item_id=self.sel_qiling_cfg.qiling_item_id
    local textName=Item.GetName(item_id)
    UH.SetText(self.TxtName, textName)
    UH.SetText(self.TxtCostName, textName)
    self.ItemCost:SetData(Item.Create({item_id = item_id}))
    self.ItemCost:SetData(Item.Create({item_id = item_id}))
    local fabao_cfg=self.data.talisman_attribute[self.sel_qiling_cfg.fabao_id]
    self.ItemFaBao:SetData(fabao_cfg)
    UH.SetText(self.TxtFaBaoName, fabao_cfg.name)
end

function FaBaoQiLingPanel:FlushSelData()
    self.sel_info = self.data.qiling_info.qi_ling_list[self.sel_id]
    self.sel_qiling_cfg = self.data.qiling_cfg[self.sel_id + 1]
    self.sel_qiling_up_cfg = self.data.qiling_up[self.sel_id][self.sel_info.level]
    self.sel_qiling_up_next_level_cfg = self.data.qiling_up[self.sel_id][self.sel_info.level + 1]
    local isYuling = self.sel_info.isPutOn
    if isYuling then
        self.BtnYuLing.isOn = true
    else
        self.BtnXiaZhen.isOn = true
    end
    UH.SetText(self.TxtEffect, self.sel_qiling_up_cfg.txt)
    if not self.sel_qiling_up_next_level_cfg then
        UH.SetText(self.TxtEffectTitle, self.language.effect_max)
        self.NextEffect:SetActive(false)
        self.ObjUpShow:SetActive(false)
        self.ObjMax:SetActive(true)
    elseif self.sel_info.level == 0 then
        UH.SetText(self.TxtEffectTitle, self.language.effect_active)
        self.NextEffect:SetActive(false)
        UH.SetText(self.TxtBtn, self.language.active)
        self.ObjUpShow:SetActive(true)
        self.ObjMax:SetActive(false)
    else
        UH.SetText(self.TxtEffectTitle, self.language.effect_curr)
        self.NextEffect:SetActive(true)
        UH.SetText(self.TxtNextEffect, self.sel_qiling_up_next_level_cfg.txt)
        UH.SetText(self.TxtBtn, Language.Common.LevelUp)
        self.ObjUpShow:SetActive(true)
        self.ObjMax:SetActive(false)
    end
    self.BtnRed:SetNum(self.data:CheckRemindById(self.sel_id))
    self:FlushCost()
    self.YuLingRed:SetNum(self.data.qiling_info.yuling_num < self.data.qiling_info.active_num and
        self.data.qiling_info.yuling_num < FA_BAO_QI_LING_PUT_ON_MAX_NUM and self.sel_info.level>0 and not self.sel_info.isPutOn
        and 1 or 0 )
end


function FaBaoQiLingPanel:FlushData()
    self.List:SetData(self.data.qiling_info.qi_ling_list)
    UH.SetText(self.TxtNum, self.language.yuling_num..self.data.qiling_info.yuling_num.."/"..FA_BAO_QI_LING_PUT_ON_MAX_NUM)
    self:FlushSelData()
end

function FaBaoQiLingPanel:FlushCost()
    if self.sel_qiling_up_next_level_cfg then
        local cost_num=self.sel_qiling_up_next_level_cfg.expend_num
        local num = Item.GetNum(self.sel_qiling_cfg.qiling_item_id)
        local color = num >= cost_num and COLORSTR.Green4 or COLORSTR.Red8
        UH.SetText(self.TxtCostNum, "<color=#" .. color .. ">" .. num .. "</color>" .. "/"..cost_num)
    end
end


function FaBaoQiLingPanel:OnYuLingClick()
    local isYuling = self.sel_info.isPutOn
    if not isYuling then
        self.BtnXiaZhen.isOn = true
        if self.sel_info.level == 0 then
            PublicPopupCtrl.Instance:Center(self.language.tip1)
        else
            if self.data.qiling_info.yuling_num == FA_BAO_QI_LING_PUT_ON_MAX_NUM then
                PublicPopupCtrl.Instance:Center(self.language.tip4)
            else
                FabaoCtrl.Instance:SendFabaoReq({req_type = FABAO_REQ_TYPE.QI_LING_PUT_ON, param1 = self.sel_id})
            end
        end
    end
end

function FaBaoQiLingPanel:PlayEffect()
    self.cur_step = 1
    self.default_step = 20
    TimeHelper:CancelTimer(self.move_handle)
    local start_pos = UH.GetRootPosition(self.BtnUp.gameObject)
    local end_pos = UH.GetRootPosition(self.ImgQiLingIcon.gameObject)
    UH.LocalPosG(self.ActiveEffects[1], start_pos)
    UH.LocalPosG(self.ActiveEffects[2], end_pos)

    self.ActiveEffectNode:SetActive(true)
    self.ActiveEffects[1]:SetActive(true)
    -- self.ActiveEffects[2]:SetActive(false)

    self.curve = Game.CurveParabola(start_pos, Vector3.New(-1, 2, 0), end_pos, Vector3.New(-1, -0.5, 0))
    self.move_handle = TimeHelper:AddRunFrameTimer(BindTool.Bind(self.RunEffect, self), 1, self.default_step)
end


function FaBaoQiLingPanel:RunEffect()
    local pos = self.curve:GetPos(self.cur_step / self.default_step)
    UH.LocalPosG(self.ActiveEffects[1], pos)
    self.cur_step = self.cur_step + 1
    if self.cur_step > self.default_step then
        TimeHelper:CancelTimer(self.move_handle)
        self.ActiveEffects[1]:SetActive(false)
        -- self.ActiveEffects[2]:SetActive(true)
        local function func_end()
            -- self.ActiveEffects[2]:SetActive(false)
            self.ActiveEffectNode:SetActive(false)
        end
        self.move_handle = TimeHelper:AddDelayTimer(func_end, 0.6)
    end
end


function FaBaoQiLingPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.move_handle)
end

function FaBaoQiLingPanel:OnXiaZhenClick()
    local isYuling =self.sel_info.isPutOn
    if isYuling then
        self.BtnYuLing.isOn = true
        FabaoCtrl.Instance:SendFabaoReq({req_type = FABAO_REQ_TYPE.QI_LING_TAKE_OFF, param1 =self.data.qiling_info.put_on_index_list[self.sel_id]})
    end
end

function FaBaoQiLingPanel:OnUpClick()
    if  self.data:IsUnlock(self.sel_qiling_cfg.fabao_id) then
        PublicPopupCtrl.Instance:Center(self.language.tip2)
        return
    end
    local cost_num = self.sel_qiling_up_next_level_cfg.expend_num
    local num = Item.GetNum(self.sel_qiling_cfg.qiling_item_id)
    if  num >= cost_num then
        FabaoCtrl.Instance:SendFabaoReq({req_type = FABAO_REQ_TYPE.QI_LING_UP_LEVEL, param1 = self.sel_id})
        self:PlayEffect()
    else
        PublicPopupCtrl.Instance:Center(self.language.tip3)
    end
end


function FaBaoQiLingPanel:OnHelpClick()
    --帮助文字
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[384].desc)
end


--===========================item==================
FaBaoQiLingItem = FaBaoQiLingItem or DeclareMono("FaBaoQiLingItem", UIWidgetBaseItem)

function FaBaoQiLingItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ObjUnActive:SetActive(data.level==0)
    self.ObjYuLing:SetActive(data.isPutOn)
    self.ItemInter.Interactable = data.level>0
    UH.SetIcon(self.ImgIcon,FabaoData.Instance.qiling_cfg[self.data.id+1].qiling_item_id, ICON_TYPE.ITEM)
    self:FlushRed()
end

function FaBaoQiLingItem:OnEnable()
    local bag_item_change_func = BindTool.Bind(self.FlushRed, self)
    self.data_care_handle = BagData.Instance:GetColumnGrids(ItemColumnType.Material):Care(bag_item_change_func)
end


function FaBaoQiLingItem:OnDisable()
    BagData.Instance:GetColumnGrids(ItemColumnType.Material):Uncare(self.data_care_handle)
    self.data_care_handle = nil
end

function FaBaoQiLingItem:FlushRed()
    self.RedPos:SetNum(FabaoData.Instance:CheckRemindById(self.data.id))
end

function FaBaoQiLingItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function FaBaoQiLingItem:OnDestroy()

end

