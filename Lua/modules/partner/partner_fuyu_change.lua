PartnerFuYuChange = PartnerFuYuChange or DeclareView("PartnerFuYuChange","partner/partner_fuyu_change", Mod.Partner.FuyuChange)
function PartnerFuYuChange:PartnerFuYuChange()
    
end
function PartnerFuYuChange:CloseCallback()
    PartnerData.Instance:ClearFuYuChange()
end
--任意带特效且无计时符玉均可进行转换
PartnerFuYuChangePanel = PartnerFuYuChangePanel or DeclareMono("PartnerFuYuChangePanel", UIWFlushPanel)
function PartnerFuYuChangePanel:PartnerFuYuChangePanel()
    self.data = PartnerData.Instance
    self.select_mark = -1
    self.select_data = nil

    self.data_cares = {
        {data = self.data:GetChangeFuYu(), func = self.FlushConsume},
        {data = self.data:GetChangeFuyuTarget(), func = self.FlushTarget},
    }
end
function PartnerFuYuChangePanel:Awake()
    UIWFlushPanel.Awake(self)
    self.ProfList:SetData(self.data:GetProfList())
    self.ProfList2:SetData(self.data:GetProfList())
    self.handle_lis = BagData.Instance:ListenItems({self.data:FuYuConvert()},BindTool.Bind(self.FlushItem, self))
end
function PartnerFuYuChangePanel:OnDestroy()
    BagData.Instance:RemoveListenItems(self.handle_lis)
    UIWFlushPanel.OnDestroy(self)
end
function PartnerFuYuChangePanel:FlushItem()
    local item_id = self.data:FuYuConvert()
    UH.SetText(self.CostNum, string.format(Language.FuYu.CostDesc, Item.GetNum(item_id)))
end
function PartnerFuYuChangePanel:FlushConsume()
    local item = self.data:GetChangeFuYu()
    if next(item) == nil then
        self.CanGroup1.alpha = 1
        UH.SetText(self.NameSelect, "")
        self.CellSelect:SetData({})
    else
        self.CanGroup1.alpha = 0
        UH.SetText(self.NameSelect, item:QuaName())
        self.CellSelect:SetData(item)
    end
    local item_id = self.data:FuYuConvert()
    UH.SetText(self.CostNum, string.format(Language.FuYu.CostDesc, Item.GetNum(item_id)))
end
function PartnerFuYuChangePanel:FlushTarget()
    --通过目标虚构一个结果
    --先判断是一个还是两个特效
    local item = self.data:GetChangeFuYu()
    if next(item) == nil then
        UH.SetText(self.NameTarget, "")
        self.CanGroup2.alpha = 1
        self.CellTarget:SetData({})
        return
    end
    item = self.data:GetChangeFuYuResult()
    if next(item) == nil then
        self.CanGroup2.alpha = 1
        UH.SetText(self.NameTarget, "")
    else
        self.CanGroup2.alpha = 0
        UH.SetText(self.NameTarget, item:QuaName())
    end
    self.CellTarget:SetData(item)
end
function PartnerFuYuChangePanel:OnClickChangeItem()
    local item_id = self.data:FuYuConvert()
    ItemInfoCtrl.Instance:ItemInfoView{item_list = {Item.Create({item_id = item_id})}}
end
function PartnerFuYuChangePanel:OnClickConfirm()
    local item = self.data:GetChangeFuYu()
    if next(item) == nil then
        PublicPopupCtrl.Instance:Center(Language.FuYu.SpChangeSelectFirst)
        return
    end
    local target = self.data:GetChangeFuYuResult()
    if next(target) == nil then
        PublicPopupCtrl.Instance:Center(Language.FuYu.SpChangeTarget)
        return
    end
    local item_id = self.data:FuYuConvert()
    if Item.GetNum(item_id) == 0 then
        local convert = Item.Create({item_id = item_id})
        MainOtherCtrl.Instance:GetWayView({item = convert})
        PublicPopupCtrl.Instance:Center(string.format(Language.FuYu.SpChangeConvert, convert:Name()))
        return 
    end

    local function send_msg()
        self.EffectTool:Play(3165000)
        self.Block:SetActive(true)
        self.Arrow:SetInteger(APH("state"), 1)
		ViewMgr:CloseView(DialogTipsView)
    end

    
	local info = {
		content = string.format(Language.FuYu.SpecialSpFuYuExchange, 
        item:QuaName(), target:QuaName()),
		cancel = {
			name = nil,
			func = nil
		},
		confirm = {
			name = nil,
			func = send_msg
		}
	}
	PublicPopupCtrl.Instance:DialogTips(info)
end
function PartnerFuYuChangePanel:EffectComplete()
    
    --[[ local select_data = self.data.sp_change.select_data
    local target_data = self.data.sp_change.target_data
    local target_data_2 = self.data.sp_change.target_data_2

    local param_t = { 
        req_type = MEDAL_REQ_TYPE.SPE_EXCHANGE,
        param1 = select_data.in_bag and select_data.column or 0, 
        param2 = select_data.in_bag and select_data.index or select_data.vo.grid_index, 
        param3 = target_data.param.sp_id, 
        param4 = target_data_2 and target_data_2.param.sp_id or 0,
        param5 = select_data.in_bag and 0 or 1,
    } ]]
    local item = self.data:GetChangeFuYu()
    local target = self.data:GetChangeFuYuResult()
    if item.in_bag then
        PartnerCtrl:ReqFuYuConvert2(item:GridColumn(), item:GridIndex(), target:SpId(), target:ExtraId())
    else
        PartnerCtrl:ReqFuYuConvert(item.partner_id, item.wear_idx, target:SpId(), target:ExtraId())
    end
    self.Block:SetActive(false)
    self.Arrow:SetInteger(APH("state"), 0)
end
function PartnerFuYuChangePanel:OnClickChange(value)
    local item = self.data:GetChangeFuYu()
    if value == 1 and next(item) == nil then
        PublicPopupCtrl.Instance:Center(Language.FuYu.SpChangeSelectFirst)
        return
    end
    self.select_mark = value
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    self.SelectPart:SetActive(true)
    self.Selectbg1:SetActive(false)
    self.Selectbg2:SetActive(false)
    self.Selectbgn:SetActive(false)
    if self.select_mark == 0 then
        self.Selectbg1:SetActive(true)
        self.SelectList:SetData(self.data:GetFuYuChangeList())
        self.BtnSelectNext:SetActive(false)
        self.BtnSelectSure:SetActive(true)
    elseif self.select_mark == 1 then
        -- 除非不是天赐否则第一步都是选择一个天赐
        local is_exc = item:IsExc()
        self.Selectbg1:SetActive(is_exc)
        self.Selectbgn:SetActive(not is_exc)
        self.select_data = nil
        self.data:SetChangeFuYuTarget1(nil)
        self.data:SetChangeFuYuTarget2(nil)
        self.Toggle1:SetAllTogglesOff()
        if is_exc then
            self.SelectList:SetData(self.data:GetExcFuYuChangeItems())
            local is_extra = item:ExtraId()
            self.BtnSelectNext:SetActive(is_extra > 0)
            self.BtnSelectSure:SetActive(is_extra <= 0)
        else
            --普通符玉
            self.ProfList2:GetItem(2).ItemClick:Trigger()
        end
    end
end
--更换想要改变的符玉
function PartnerFuYuChangePanel:OnClickSelectSure()
    if self.select_mark == 0 then
        self.data:SetChangeFuYu(self.select_data)
        self.data:SetChangeFuYuTarget1(nil)
        self.data:SetChangeFuYuTarget2(nil)
    elseif self.select_mark == 1 then
        self.data:SetChangeFuYuTarget1(self.select_data)
    end
    self.Toggle1:SetAllTogglesOff()
    self.SelectPart:SetActive(false)
end
-- 设置第一个特效
function PartnerFuYuChangePanel:OnClickSelectNext()
    if self.select_data == nil then 
        PublicPopupCtrl.Instance:Center(Language.FuYu.SpChangeTargetExcSp)
        return 
    end 
    self.Selectbg1:SetActive(false)
    self.Selectbg2:SetActive(true)
    self.ProfList:GetItem(2).ItemClick:Trigger()
    self.data:SetChangeFuYuTarget1(self.select_data)
    self.select_data = nil--用于next的判断
end

--设置第二个特效如果有的话
function PartnerFuYuChangePanel:OnClickSelect2Sure()
    if self.select_data == nil then
        PublicPopupCtrl.Instance:Center(Language.FuYu.SpChangeTargetNormal)
        return
    end
    self.data:SetChangeFuYuTarget2(self.select_data)
    self.Toggle1:SetAllTogglesOff()
    self.Toggle2:SetAllTogglesOff()
    self.SelectPart:SetActive(false)
end

function PartnerFuYuChangePanel:OnClickSelectBack()
    self.Selectbg1:SetActive(true)
    self.Selectbg2:SetActive(false)
    self.Toggle1:SetAllTogglesOff()
    self.select_data = nil
end

--选择普通符玉特效 设置第一个特效
function PartnerFuYuChangePanel:OnClickSelectnSure()
    if self.select_data == nil then--选择需要转换的普通效果
        PublicPopupCtrl.Instance:Center(Language.FuYu.SpChangeTargetNormal)
        return 
    end 
    self.data:SetChangeFuYuTarget1(self.select_data)
    self.Togglen:SetAllTogglesOff()
    self.SelectPart:SetActive(false)
end

function PartnerFuYuChangePanel:OnClickPanelClose()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    self.Toggle1:SetAllTogglesOff()
    self.Toggle2:SetAllTogglesOff()
    self.SelectPart:SetActive(false)
    if self.select_mark == 1 then
        self.data:SetChangeFuYuTarget1(nil)
        self.data:SetChangeFuYuTarget2(nil)
    end 
    self.select_data = nil
end

function PartnerFuYuChangePanel:OnClickColumn(prof_info)
    local item = self.data:GetChangeFuYu()
    local prof = prof_info.prof
    if self.Selectbg2.activeSelf then
        self.Select2List:SetData(self.data:GetProfFuYuChangeItems(prof, item:SpId(), item:ExtraLevel()))
    end
    if self.Selectbgn.activeSelf then
        self.SelectnList:SetData(self.data:GetProfFuYuChangeItems(prof, item:SpId(), item:SpLevel())) 
    end 
end

function PartnerFuYuChangePanel:OnClickClose()
    ViewMgr:CloseView(PartnerFuYuChange)
end

function PartnerFuYuChangePanel:SelectFlush(data)
    self.select_data = data
end

FuYuSpSelectItem = FuYuSpSelectItem or DeclareMono("FuYuSpSelectItem", UIWidgetBaseItem)
function FuYuSpSelectItem:FuYuSpSelectItem()end
function FuYuSpSelectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    UH.SetText(self.Name, data:QuaName())
    self.IsEquip:SetActive(not data.in_bag)
end

FuYuSpChangeProfClickCell = FuYuSpChangeProfClickCell or DeclareMono("FuYuSpChangeProfClickCell", UIWidgetBaseItem)
function FuYuSpChangeProfClickCell:FuYuSpChangeProfClickCell() end
function FuYuSpChangeProfClickCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, Language.Medal.MedalSpChangeProf[data.prof])
end 