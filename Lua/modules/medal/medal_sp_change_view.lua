MedalSpChangeView = MedalSpChangeView or DeclareView("MedalSpChangeView", "medal/medal_sp_change",Mod.MedalSpChange.Main)
function MedalSpChangeView:MedalSpChangeView()end 
function MedalSpChangeView:LoadCallback(param_t)
    if param_t.item ~= nil then 
        MedalData.Instance:SetSelectedTargetSpChange(param_t.item,0)
    end   
end
function MedalSpChangeView:OnClickClose()
    ViewMgr:CloseView(MedalSpChangeView)
end

MedalSpChangePanel = MedalSpChangePanel or DeclareMono("MedalSpChangePanel", UIWFlushPanel)
function MedalSpChangePanel:MedalSpChangePanel()
    self.data = MedalData.Instance
    self.data_cares = {
        {data = self.data.sp_change_notice,func = self.ShowNotice ,keys = {"is_show"}},
        {data = self.data.sp_change,func = self.FlushShow,init_call = true},
        {data = BagData.Instance.item_grid, func = self.FlushShow, init_call = false},
    }
    
    self.select_data = nil
    self.select_mark = -1
    self.mark_prof = 0

    -- self.prof_click_list:SetData(self.data:GetMedalSpecialSpProfList())
end 

function MedalSpChangePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.wait_handle ~= nil then
        TimeHelper:CancelTimer(self.wait_handle)
        self.wait_handle = nil
    end
    self.data:ClearSpChangeData()
end

function MedalSpChangePanel:Awake()
    UIWFlushPanel.Awake(self)
    self.prof_click_list:SetData(self.data:GetMedalSpecialSpProfList())
    self.prof_click_list_n:SetData(self.data:GetMedalSpecialSpProfList())
end

function MedalSpChangePanel:FlushShow()
    self.CellSelect:SetData(self.data.sp_change.select_data or {})
    local target_item = self.data:GetResultTargetData()
    self.CellTarget:SetData(target_item)

    local flag_select = self.data.sp_change.select_data ~= nil
    self.NameSelectObj:SetActive(flag_select)
    UH.SetText(self.NameSelect,flag_select and self.data.sp_change.select_data:QuaName() or "")

    local flag_target = self.data.sp_change.target_data ~= nil
    self.NameTargetObj:SetActive(flag_target)
    UH.SetText(self.NameTarget,flag_target and target_item:QuaName() or "")

    UH.SetText(self.item_num, Item.GetNum(CommonItemId.MedalSpecialSpChange).."/1")
end

function MedalSpChangePanel:SelectFlush(data)
    self.select_data = data
end

function MedalSpChangePanel:OnClickChange(type)
    if type == 1 and self.data.sp_change.select_data == nil then 
        PublicPopupCtrl.Instance:Center(Language.Medal.MedalSpChangeSelectFirst)
        return 
    end 
    self.select_mark = type

    self.SelectPart:SetActive(true)
    self.Selectbg2:SetActive(false)
    self.Selectbgn:SetActive(false)
    
    if self.select_mark == 0 then 
        self.Selectbg1:SetActive(true)
        
        self.SelectList:SetData(self.data:GetMedalSpecialSpBagList())
        self.btnSelectNext:SetActive(false)
        self.btnSelectSure:SetActive(true)
    elseif self.select_mark == 1 then 
        self.Selectbg1:SetActive(self.data.sp_change.select_data:IsSpecialSp())
        self.Selectbgn:SetActive(not self.data.sp_change.select_data:IsSpecialSp())

        if self.data.sp_change.select_data:IsSpecialSp() then 
            self.SelectList:SetData(self.data:GetMedalSpecialSpTarget())
            self.btnSelectNext:SetActive(self.data.sp_change.select_data:WithSecond())
            self.btnSelectSure:SetActive(not self.data.sp_change.select_data:WithSecond())
        else 
            self.prof_click_list_n:GetItem(2).ItemClick:Trigger()
        end 
    end 
end

function MedalSpChangePanel:OnClickPanelClose()
    self.toggle:SetAllTogglesOff()
    self.toggle2:SetAllTogglesOff()
    self.SelectPart:SetActive(false)

    if self.select_mark == 1 then 
        self.data:ClearSpChangeTargetData()
    end 
    
    self.select_data = nil
end

function MedalSpChangePanel:OnClickSelectSure()
    self.data:SetSelectedTargetSpChange(self.select_data,self.select_mark,
        (self.select_mark == 1 and self.data.sp_change.select_data:WithSecond()))
    
    if self.select_mark == 0 then 
        self.data.sp_change.target_data = nil 
    end 

    self.toggle:SetAllTogglesOff()
    self.SelectPart:SetActive(false)
end

function MedalSpChangePanel:OnClickColumn(prof_info)
    self.mark_prof = prof_info.Prof
    if self.Selectbg2.activeSelf then 
        self.Select2List:SetData(self.data:GetMedalSpecialSp2Target(self.mark_prof)) 
    end
    if self.Selectbgn.activeSelf then
        self.SelectnList:SetData(self.data:GetMedalSpecialSpNomalTarget(self.mark_prof)) 
    end 
end

function MedalSpChangePanel:OnClickSelectNext()
    if self.select_data == nil then 
        PublicPopupCtrl.Instance:Center(Language.Medal.MedalSpChangePopLackTargetMedal)
        return 
    end 
    self.Selectbg1:SetActive(false)
    self.Selectbg2:SetActive(true)
    self.prof_click_list:GetItem(2).ItemClick:Trigger()

    self.data:SetSelectedTargetSpChange(self.select_data,self.select_mark,false)

    self.select_data = nil
end

function MedalSpChangePanel:OnClickSelectBack()
    self.Selectbg1:SetActive(true)
    self.Selectbg2:SetActive(false)

    self.toggle:SetAllTogglesOff()
    self.select_data = nil
end

function MedalSpChangePanel:OnClickSelect2Sure()
    if self.select_data == nil then 
        PublicPopupCtrl.Instance:Center(Language.Medal.MedalSpChangePopLackTargetMedal2)
        return 
    end 

    self.data:SetSelectedTargetSpChange(self.select_data,self.select_mark,true)

    self.toggle:SetAllTogglesOff()
    self.toggle2:SetAllTogglesOff()
    self.SelectPart:SetActive(false)
end

function MedalSpChangePanel:OnClickSelectnSure()
    if self.select_data == nil then 
        PublicPopupCtrl.Instance:Center(Language.Medal.MedalSpChangePopLackTargetMedal2)
        return 
    end 

    self.data:SetSelectedTargetSpChange(self.select_data,self.select_mark,false)
    self.togglen:SetAllTogglesOff()
    self.SelectPart:SetActive(false)
end

function MedalSpChangePanel:OnClickConfirm()
    if self.data.sp_change.select_data == nil then 
        PublicPopupCtrl.Instance:Center(Language.Medal.MedalSpChangeLackSelectMedal)
        return 
    end 

    if self.data.sp_change.target_data == nil then
        PublicPopupCtrl.Instance:Center(Language.Medal.MedalSpChangeLackTargetMedal)
        return 
    end 

    if Item.GetNum(CommonItemId.MedalSpecialSpChange) == 0 then 
        MainOtherCtrl.Instance:GetWayView({item = Item.Create({item_id = CommonItemId.MedalSpecialSpChange})})
        PublicPopupCtrl.Instance:Center(Language.Medal.MedalSpChangeItemLack)
        -- ItemInfoCtrl.Instance:ItemInfoView{item_list = {Item.Create({item_id = CommonItemId.MedalSpecialSpChange})}}
        return 
    end 

    local function send_msg()
        self.EffectTool:Play(3165000)
        self.Block:SetActive(true)
        self.Arrow:SetInteger(APH("state"), 1)
		ViewMgr:CloseView(DialogTipsView)
    end

    local target_item = self.data:GetResultTargetData()
	local info = {
		content = string.format(Language.Medal.SpecialSpMedalExchange, 
        self.data.sp_change.select_data:QuaName(),target_item:QuaName()),
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

function MedalSpChangePanel:EffectComplete()
    local select_data = self.data.sp_change.select_data
    local target_data = self.data.sp_change.target_data
    local target_data_2 = self.data.sp_change.target_data_2

    local param_t = { 
        req_type = MEDAL_REQ_TYPE.SPE_EXCHANGE,
        param1 = select_data.in_bag and select_data.column or 0, 
        param2 = select_data.in_bag and select_data.index or select_data.vo.grid_index, 
        param3 = target_data.param.sp_id, 
        param4 = target_data_2 and target_data_2.param.sp_id or 0,
        param5 = select_data.in_bag and 0 or 1,
    }

    self.Block:SetActive(false)
    self.Arrow:SetInteger(APH("state"), 0)
    BagCtrl.Instance:SendMedalReq(param_t)
end

function MedalSpChangePanel:OnClickChangeItem()
    ItemInfoCtrl.Instance:ItemInfoView{item_list = {Item.Create({item_id = CommonItemId.MedalSpecialSpChange})}}
end

function MedalSpChangePanel:ShowNotice()
    if self.data.sp_change_notice.is_show then
        self.data.sp_change.target_data = nil 
        self.data.sp_change.select_data = nil 
        self.data:ClearSpecialExchangeNotice()

        local item = (self.data.sp_change_notice.info.is_wear == 1) and Item.Create((BagData.Instance:ViewData().medal_info[self.data.sp_change_notice.info.index+1]))
            or (BagData.Instance:GetItemByIndex( self.data.sp_change_notice.info.column,self.data.sp_change_notice.info.index))

        MainOtherCtrl.Instance:OpenGiftView({[1]=item},GIFT_OPEN_TYPE.CONGRATULATIONS,true)

        if self.wait_handle ~= nil then
            TimeHelper:CancelTimer(self.wait_handle)
            self.wait_handle = nil
        end

        self.wait_handle = TimeHelper:AddDelayTimer(function()
            self.wait_handle = nil
            self:FlushShow()
        end,1.2)
    end 

end

MedalSpSelectItem = MedalSpSelectItem or DeclareMono("MedalSpSelectItem", UIWidgetBaseItem)
function MedalSpSelectItem:MedalSpSelectItem()end
function MedalSpSelectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    
    UH.SetText(self.name,data:QuaName())
    self.is_equip:SetActive(not data.in_bag)
end

MedalSpChangeProfClickCell = MedalSpChangeProfClickCell or DeclareMono("MedalSpChangeProfClickCell", UIWidgetBaseItem)
function MedalSpChangeProfClickCell:MedalSpChangeProfClickCell() end
function MedalSpChangeProfClickCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.name.text = Language.Medal.MedalSpChangeProf[data.Prof]
end 