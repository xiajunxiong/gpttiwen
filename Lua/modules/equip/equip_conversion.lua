-- 1、该功能可消耗道具将橙装转换成其他不同职业的橙装，必须要相同部位、等级和品质，转换后保留强化效果。
-- 2、在可转换的装备信息右侧，加入【转换】按钮，位于点化和捐赠中间，点击后进入装备转换界面。（图1）
-- 3、装备转换界面，界面逻辑与令牌转换逻辑相似。左右两侧均可选择装备，但左侧不会有未选择状态，不能选择相同职业的装备。（图2）
-- 4、左侧当前装备的选择窗口为图3，右侧目标装备的选择窗口为图4。左侧选择窗口中展示所有可转换的装备，右侧目标装备的窗口中仅显示相同部位和等级的装备。
-- 5、选择完毕后点击确认转换后需要弹出确认弹窗进行确认，转换完成后弹出恭喜获得的窗口。（图5/6）
-- 6、界面上的动特效全部沿用令牌转换的动特效。


EquipConversion = EquipConversion or DeclareView("EquipConversion","equip_view/equip_conversion", Mod.Equip.Conversion)
function EquipConversion:EquipConversion()
    
end
function EquipConversion:CloseCallback()
    EquipData.Instance:ClearEquipChange()
end
--任意带特效且无计时符玉均可进行转换
EquipConversionPanel = EquipConversionPanel or DeclareMono("EquipConversionPanel", UIWFlushPanel)
function EquipConversionPanel:EquipConversionPanel()
    self.data = EquipData.Instance
    self.select_mark = -1
    self.select_data = nil
    self.need_num = self.data:EquipConvertNum()

    self.data_cares = {
        {data = self.data:GetChangeEquip(), func = self.FlushConsume},
        {data = self.data:GetChangeEquipTarget(), func = self.FlushTarget},
    }
end
function EquipConversionPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.ProfList:SetData(self.data:GetProfList())
    self.ProfList2:SetData(self.data:GetProfList())
    self.handle_lis = BagData.Instance:ListenItems({self.data:EquipConvert()},BindTool.Bind(self.FlushItem, self))
end
function EquipConversionPanel:OnDestroy()
    BagData.Instance:RemoveListenItems(self.handle_lis)
    UIWFlushPanel.OnDestroy(self)
end
function EquipConversionPanel:FlushItem()
    local item_id = self.data:EquipConvert()
    UH.SetText(self.CostNum, string.format(Language.Common.Slash, Item.GetNum(item_id), self.need_num))
end
function EquipConversionPanel:FlushConsume()
    local item = self.data:GetChangeEquip()
    if next(item) == nil then
        self.CanGroup1.alpha = 1
        UH.SetText(self.NameSelect, "")
        self.CellSelect:SetData({})
    else
        self.CanGroup1.alpha = 0
        UH.SetText(self.NameSelect, item:QuaName())
        self.CellSelect:SetData(item)
    end
    self:FlushItem()
end
function EquipConversionPanel:FlushTarget()
    --获取选中的转换目标
    local item = self.data:GetChangeEquip()
    if next(item) == nil then
        UH.SetText(self.NameTarget, "")
        self.CanGroup2.alpha = 1
        self.CellTarget:SetData({})
        return
    end
    item = self.data:GetChangeEquipResult()
    if next(item) == nil then
        self.CanGroup2.alpha = 1
        UH.SetText(self.NameTarget, "")
    else
        self.CanGroup2.alpha = 0
        UH.SetText(self.NameTarget, item:QuaName())
    end
    self.CellTarget:SetData(item)
end
function EquipConversionPanel:OnClickChangeItem()
    local item_id = self.data:EquipConvert()
    ItemInfoCtrl.Instance:ItemInfoView{item_list = {Item.Create({item_id = item_id})}}
end
function EquipConversionPanel:OnClickConfirm()
    local item = self.data:GetChangeEquip()
    if next(item) == nil then
        PublicPopupCtrl.Instance:Center(Language.Equip.SpChangeSelectFirst)
        return
    end
    local target = self.data:GetChangeEquipResult()
    if next(target) == nil then
        PublicPopupCtrl.Instance:Center(Language.Equip.SpChangeTarget)
        return
    end
    local item_id = self.data:EquipConvert()
    if Item.GetNum(item_id) == 0 then
        local convert = Item.Create({item_id = item_id})
        MainOtherCtrl.Instance:GetWayView({item = convert})
        PublicPopupCtrl.Instance:Center(string.format(Language.Equip.SpChangeConvert, convert:Name()))
        return 
    end

    local function send_msg()
        self.EffectTool:Play(3165000)
        self.Block:SetActive(true)
        self.Arrow:SetInteger(APH("state"), 1)
		ViewMgr:CloseView(DialogTipsView)
    end

    
	local info = {
		content = string.format(Language.Equip.SpecialSpEquipExchange, 
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
function EquipConversionPanel:EffectComplete()
    
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
    local item = self.data:GetChangeEquip()
    local target = self.data:GetChangeEquipResult()
    if item.in_bag then
        WorkshopCtrl.Instance:ChangeEquip2(item:GridColumn(), item:GridIndex(), target.item_id)
    else
        WorkshopCtrl.Instance:ChangeEquip1(item:IsJewelry() and 1 or 0, item.index, target.item_id)
    end
    self.Block:SetActive(false)
    self.Arrow:SetInteger(APH("state"), 0)
end
function EquipConversionPanel:OnClickChange(value)
    local item = self.data:GetChangeEquip()
    if value == 1 and next(item) == nil then
        PublicPopupCtrl.Instance:Center(Language.Equip.SpChangeSelectFirst)
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
        self.SelectList:SetData(self.data:GetEquipChangeList())
        self.BtnSelectNext:SetActive(false)
        self.BtnSelectSure:SetActive(true)
    elseif self.select_mark == 1 then
        -- 除非不是天赐否则第一步都是选择一个天赐
        local is_exc = false--item:IsExc()
        self.Selectbg1:SetActive(is_exc)
        self.Selectbgn:SetActive(not is_exc)
        self.select_data = nil
        self.data:SetChangeEquipTarget1(nil)
        self.data:SetChangeEquipTarget2(nil)
        self.Toggle1:SetAllTogglesOff()
        if is_exc then
            -- self.SelectList:SetData(self.data:GetExcFuYuChangeItems())
            -- local is_extra = item:ExtraId()
            -- self.BtnSelectNext:SetActive(is_extra > 0)
            -- self.BtnSelectSure:SetActive(is_extra <= 0)
        else
            --普通符玉
            self.ProfList2:GetItem(1).ItemClick:Trigger()
        end
    end
end
--更换想要改变的符玉
function EquipConversionPanel:OnClickSelectSure()
    if self.select_mark == 0 then
        self.data:SetChangeEquip(self.select_data)
        self.data:SetChangeEquipTarget1(nil)
        self.data:SetChangeEquipTarget2(nil)
    elseif self.select_mark == 1 then
        self.data:SetChangeEquipTarget1(self.select_data)
    end
    self.Toggle1:SetAllTogglesOff()
    self.SelectPart:SetActive(false)
end
-- 设置第一个特效
function EquipConversionPanel:OnClickSelectNext()
    if self.select_data == nil then 
        PublicPopupCtrl.Instance:Center(Language.Equip.SpChangeTargetExcSp)
        return 
    end 
    self.Selectbg1:SetActive(false)
    self.Selectbg2:SetActive(true)
    self.ProfList:GetItem(1).ItemClick:Trigger()
    self.data:SetChangeEquipTarget1(self.select_data)
    self.select_data = nil--用于next的判断
end

--设置第二个特效如果有的话
function EquipConversionPanel:OnClickSelect2Sure()
    if self.select_data == nil then
        PublicPopupCtrl.Instance:Center(Language.Equip.SpChangeTargetNormal)
        return
    end
    self.data:SetChangeEquipTarget2(self.select_data)
    self.Toggle1:SetAllTogglesOff()
    self.Toggle2:SetAllTogglesOff()
    self.SelectPart:SetActive(false)
end

function EquipConversionPanel:OnClickSelectBack()
    self.Selectbg1:SetActive(true)
    self.Selectbg2:SetActive(false)
    self.Toggle1:SetAllTogglesOff()
    self.select_data = nil
end

--选择普通符玉特效 设置第一个特效
function EquipConversionPanel:OnClickSelectnSure()
    if self.select_data == nil then--选择需要转换的普通效果
        PublicPopupCtrl.Instance:Center(Language.Equip.SpChangeTargetNormal)
        return 
    end 
    local item = self.data:GetChangeEquip()
    if self.select_data:LimitProf() == item:LimitProf() then
        PublicPopupCtrl.Instance:Center(Language.Equip.SameProfTip)
        self.Togglen:SetAllTogglesOff()
        self.select_data = nil
        return
    end
    self.data:SetChangeEquipTarget1(self.select_data)
    self.Togglen:SetAllTogglesOff()
    self.SelectPart:SetActive(false)
end

function EquipConversionPanel:OnClickPanelClose()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    self.Toggle1:SetAllTogglesOff()
    self.Toggle2:SetAllTogglesOff()
    self.Togglen:SetAllTogglesOff()
    self.SelectPart:SetActive(false)
    if self.select_mark == 1 then
        self.data:SetChangeEquipTarget1(nil)
        self.data:SetChangeEquipTarget2(nil)
    end 
    self.select_data = nil
end

function EquipConversionPanel:OnClickColumn(prof_info)
    local item = self.data:GetChangeEquip()
    local prof = prof_info.prof * 100
    --根据职业显示可转换的装备列表
    if self.Selectbg2.activeSelf then
        self.Toggle2:SetAllTogglesOff()
        self.Select2List:SetData(self.data:GetEquipChangeItems(prof, item:EquipType(), item:LimitLv()))
    end
    if self.Selectbgn.activeSelf then
        self.Togglen:SetAllTogglesOff()
        self.SelectnList:SetData(self.data:GetEquipChangeItems(prof, item:EquipType(), item:LimitLv())) 
    end 
end

function EquipConversionPanel:OnClickClose()
    ViewMgr:CloseView(EquipConversion)
end

function EquipConversionPanel:SelectFlush(data)
    self.select_data = data
end

--[[ FuYuSpSelectItem = FuYuSpSelectItem or DeclareMono("FuYuSpSelectItem", UIWidgetBaseItem)
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
end  ]]