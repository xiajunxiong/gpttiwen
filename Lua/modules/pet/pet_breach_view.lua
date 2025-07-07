PetBreachView = PetBreachView or DeclareView("PetBreachView","pet/pet_breach_view")
function PetBreachView:PetBreachView()
	
end
function PetBreachView:OnCloseClick()
	ViewMgr:CloseView(PetBreachView)
end
PetBreachPanel = PetBreachPanel or DeclareMono("PetBreachPanel",UIWFlushPanel)
function PetBreachPanel:PetBreachPanel()
	self.data = PetData.Instance
	self.data_cares = {
		{data = self.data.pet_list,func = self.FlushPanel,init_call = false},
		{data = BagData.Instance.item_grid,func = self.FlushItem,init_call = false},
		{data = self.data.pet_select,func = self.FlushPanel, keys = {"index"},init_call = false},
		{data = self.data.pet_breach_info,func = self.FlushAttri,init_call = false},
	}
	if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    end
end
function PetBreachPanel:Awake()
	UIWFlushPanel.Awake(self)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
	self:FlushPanel()
	self:SetModel()
end

function PetBreachPanel:FlushPanel()
	self.pet = self.data:GetCurSelect()
	if not self.pet then
		ViewMgr:CloseView(PetBreachView)
		return
	end
	local pet_lv = self.pet:Level()
	self.breach_list = self.data:GetBreachList(self.pet)
	-- 等级变化或者突破标志变化，则打开突破成功界面
	if (self.last_breach_state and self.last_breach_state ~= self.pet.info.is_up_level_breach and self.pet.info.is_up_level_breach == 1) or 
	(self.last_lv and self.last_lv ~= self.pet:Level()) then
		self.last_breach_state = nil
		self.last_lv = nil
		self:ShowResultPanel()
	end
	if self.breach_list == nil or next(self.breach_list) == nil then
		self.PanelObj:SetActive(false)
		return
	end
	self.breach_index = 1
	PetCtrl.Instance:SendPetBreachAttri(self.breach_list[self.breach_index].level)
	self:SetPanel()
end

function PetBreachPanel:FlushItem()
	if self.breach_list == nil or next(self.breach_list) == nil then
		self.PanelObj:SetActive(false)
		return
	end
	local breach_cfg = self.breach_list[self.breach_index]
	local stuff_item = Item.Create({item_id = breach_cfg.breach_consume})
	self.ItemCell:SetData(stuff_item)
	UH.SetText(self.ItemName,stuff_item:QuaName(true))
	local stuff_item_num = stuff_item:Num()
	if stuff_item_num < breach_cfg.breach_consume_num then
		stuff_item_num = ColorStr(stuff_item_num .. "",COLORSTR.Red8)
	end
	UH.SetText(self.ItemNum,string.format("%s/%s",stuff_item_num,breach_cfg.breach_consume_num))
	self.breach_flag = self.data:IsCanBreach(self.pet,true,false)
	self.Interactable.Interactable = self.breach_flag == 1
end

function PetBreachPanel:SetPanel()
	local breach_cfg = self.breach_list[self.breach_index]
	UH.SetText(self.PetName,string.format("%s  Lv.%s",self.pet:Name(),self.pet:Level()))
	local breach_lv
	if self.breach_index == #self.breach_list then
		breach_lv = self.data:GetMaxLv()
	else
		breach_lv = self.breach_list[self.breach_index + 1].level
	end
	breach_lv = ColorStr(breach_lv .. "",COLORSTR.Red8)
	UH.SetText(self.Tip,string.format(Language.PetBreach.Tip1,breach_lv))
	self:FlushItem()
	-- 后面要求不可预览
	-- self.LeftBtnObj:SetActive(self.breach_index > 1)
	-- self.RightBtnObj:SetActive(self.breach_index < #self.breach_list)
	self.LeftBtnObj:SetActive(false)
	self.RightBtnObj:SetActive(false)
	self:FlushAttri()
end

function PetBreachPanel:FlushAttri()
	local attri_list_data = {}
	local pet_attri = self.data:RealAttr() or {}
	if TableIsEmpty(pet_attri) then
		return
	end
	local pet_breach_attri = self.data:RealAttr(-1,self.data:GetPetBreachAttri().attr_list or {})
	for k,v in pairs(pet_attri) do
		if Language.PetAttriNames[k] then
			local attri_data = {}
			attri_data.name = Language.PetAttriNames[k][1]
			attri_data.value = pet_attri[k] or 0
			attri_data.value_up = pet_breach_attri[k] - pet_attri[k]
			attri_data.seq = Language.PetAttriNames[k][2]
			table.insert(attri_list_data,attri_data)
		end
	end
	table.sort(attri_list_data,function (a,b)
		return a.seq < b.seq
	end)
	self.AttriList:SetData(attri_list_data)
end

function PetBreachPanel:SetModel()
	if not self.pet then
		return
	end
	self.ui_obj:SetData(DrawerHelper.GetNpcPath(self.pet:PetCfg().res_id))
    self.ui_obj:PlayAnim(ChAnimType.Show)
	self.PetLimitFlag:SetType(self.pet:LimitType())
end

function PetBreachPanel:ShowResultPanel()
	self.PanelObj:SetActive(false)
	self.ResultObj:SetActive(true)
	self.ResultPetItem:SetData(self.pet)
	UH.SetText(self.ResultPetName,self.pet:QuaName(true))
	local breach_lv
	if self.breach_index > #self.breach_list then
		breach_lv = self.data:GetMaxLv()
	else
		breach_lv = self.breach_list[self.breach_index].level
	end
	UH.SetText(self.ResultPetTip,string.format(Language.PetBreach.Tip2,breach_lv))
end

function PetBreachPanel:OnLeftClick()
	self.breach_index = self.breach_index - 1
	self.breach_index = self.breach_index < 1 and 1 or self.breach_index
	self:SetPanel()
end

function PetBreachPanel:OnRightClick()
	self.breach_index = self.breach_index + 1
	self.breach_index = self.breach_index > #self.breach_list and #self.breach_list or self.breach_index
	self:SetPanel()
end

function PetBreachPanel:OnBreachClick()
	local flag = self.breach_flag
	if flag == 1 then
		self.last_breach_state = self.pet.info.is_up_level_breach
		self.last_lv = self.pet:Level()
		PetCtrl.SendPetOp(PET_OP_TYPE.UpLevelBreak,self.pet.index)
		self.data:TriggerBreachRemind()
	elseif flag == 0 then
		PublicPopupCtrl.Instance:Center(string.format(Language.PetBreach.Tip3,self.breach_list[self.breach_index].level))
	elseif flag == -2 then
		local item_id = self.breach_list[self.breach_index].breach_consume
		local item_num = Item.GetNum(item_id)
		local item = Item.Create({item_id = item_id, jump_need_num = self.breach_list[self.breach_index].breach_consume_num - item_num})
		PublicPopupCtrl.Instance:Center(string.format(Language.PetBreach.Tip4,item:Name()))
		MainOtherCtrl.Instance:GetWayView({item = item})
	end
end

function PetBreachPanel:OnCloseResultClick()
	if self.breach_list == nil or next(self.breach_list) == nil then
		ViewMgr:CloseView(PetBreachView)
		return
	end
	self.ResultObj:SetActive(false)
	self.PanelObj:SetActive(true)
end

function PetBreachPanel:OnDestroy()
	if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
	self.ui_obj = nil
	self.data:SetPetBreachAttri({})
	UIWFlushPanel.OnDestroy(self)
end

PetBreachAttriItem = PetBreachAttriItem or DeclareMono("PetBreachAttriItem",UIWidgetBaseItem)

function PetBreachAttriItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	UH.SetText(self.Name,data.name)
	UH.SetText(self.Value,data.value)
	self.UpObj:SetActive(data.value_up ~= nil and data.value_up > 0)
	UH.SetText(self.ValueUp,tostring(data.value_up))
end