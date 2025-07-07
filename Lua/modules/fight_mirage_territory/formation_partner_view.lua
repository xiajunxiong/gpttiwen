
FormationPartnerView = FormationPartnerView or DeclareView("FormationPartnerView","fight_mirage_territory/formation_partner")

function FormationPartnerView:OnClickClose()
	FightMirageTerritoryData.Instance:SaveFormation()
	ViewMgr:CloseView(FormationPartnerView)
end

--------------选择阵型--------------
MirageTerritoryFormationPanel = MirageTerritoryFormationPanel or DeclareMono("MirageTerritoryFormationPanel", UIWFlushPanel)
function MirageTerritoryFormationPanel:MirageTerritoryFormationPanel()
	self.data = FightMirageTerritoryData.Instance
    self.data_cares = {
		{data = self.data.formation_smart, func = self.FlushCurSelShow, keys = {"pos_changed"}},
		{data = self.data.formation_smart, func = self.FlushDetails, keys = {"pos_changed"}},
		{data = self.data.formation_smart, func = self.FlushFormationSel, keys = {"pos_change"}, init_call = false}, 
    }
end

function MirageTerritoryFormationPanel:FlushCurSelShow()
	local show_list = self.data:GetPartnerList()
	local data = self.data.cur_area_data
	UH.SetText(self.FormationTips,string.format(Language.MirageTerritory.BattleTip,Language.MirageTerritory.Profession[data.job1],Language.MirageTerritory.Profession[data.job2]))
	self.PartnerList:SetData(show_list)
end

function MirageTerritoryFormationPanel:FlushFormationSel()
	local lights = self.data.formation_smart.pos_change.lights
	local arrow = self.data.formation_smart.pos_change.arrow
	for i = 1, self.ShowItems:Length() do
		self.ShowItems[i]:ArrowShow(i == arrow)
		self.ShowItems[i]:LightShow(lights[i] and i ~= arrow, lights[i] and i == arrow)
	end
end

function MirageTerritoryFormationPanel:FlushDetails()
	local formation_info = self.data:GetFormationInfo()
	for i = 1, self.ShowItems:Length() do
		self.ShowItems[i]:SetData(formation_info[i])
	end
end

function MirageTerritoryFormationPanel:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	self.data.formation_smart.partner = 0
	self.data:PosChangeReset()
end

function MirageTerritoryFormationPanel:OnClickCrystal()
	PartnerData.Instance.crystal_open_form = PartnerData.CryStalOpenFormEmun.Mirage
	ViewMgr:OpenView(PartnerCrystalView)
end

function MirageTerritoryFormationPanel:OnClickSave()
	self.data:SaveFormation()
end

function MirageTerritoryFormationPanel:OnClickChallenge()
	if self.data.Instance.cur_layer_data.layer + self.data.cur_layer_data.sel_level <= self.data.cur_layer_data.pass_level then
		PublicPopupCtrl.Instance:Center(Language.MirageTerritory.PassTip)
		return 
	end
	if self.data:IsNeedFormation() then
		PublicPopupCtrl.Instance:Center(Language.MirageTerritory.SelectTip)
		return
	end
	if self.data.cur_layer_data.layer + self.data.cur_layer_data.sel_level > self.data.cur_layer_data.pass_level + 1 then
		PublicPopupCtrl.Instance:Center(Language.MirageTerritory.NotChallengeTip)
		return
	end
	local function challenge_func()
		self.data:SaveFormation()
		self.data.cur_pass_level = self.data.Instance.cur_layer_data.layer + self.data.cur_layer_data.sel_level
		self.data.is_wait = true	
		ViewMgr:CloseView(FormationPartnerView)
        ViewMgr:CloseView(FightMirageTerritoryLevelView)
	end
	if self.data:IsCanFormation() then
		--提示确认框
		PublicPopupCtrl.Instance:DialogTips{
			content = Language.MirageTerritory.GoToFormationTip,
			confirm = {
				name = Language.MirageTerritory.Confirm,
				func = function()
					challenge_func()
					ViewMgr:CloseView(DialogTipsView)
				end
			}
		}
		return
	end
	challenge_func()
end

MirageTerritoryPartnerItem = DeclareMono("MirageTerritoryPartnerItem", UIWidgetBaseItem)

function MirageTerritoryPartnerItem:MirageTerritoryPartnerItem()
	self.Data = FightMirageTerritoryData.Instance
end

function MirageTerritoryPartnerItem:OnDestroy()
    self:uncareData()
end

function MirageTerritoryPartnerItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

	self:uncareData()
	self.care_handles = {}

	self:careData(self.Data.formation_smart, BindTool.Bind(self.FlushSelect, self), "partner")

	local info = data.info
    local vo = data.vo
	local prof_sp = RoleData.GetProfSp(info.job)
	local lock_state = PartnerConfig.LockState
	
	self.Name.text = string.format("<color=#%s>%s</color>", vo.is_unlocked == lock_state.unlock and COLORSTR.Yellow13 or COLORSTR.Gray4, info.name)
    self.Prof.SpriteName = prof_sp
	UH.SetIcon(self.IconSp, PartnerData.IconId(data), ICON_TYPE.ITEM)
	local info_quality = PartnerData.Instance:GetCurPartnerQualityInfo(data.id)
	PartnerInfo.SetQuality(self.Quality, info_quality.quality)
	self.ItemInter.Interactable = vo.is_unlocked == lock_state.unlock
	local status_img_name = self.Data:PartnerOnFight(data.id) == nil and "" or PPStateSp.zhan
	status_img_name = vo.is_unlocked == lock_state.unlock and status_img_name or ""
	UH.SpriteName(self.StatusImg, status_img_name)

end

function MirageTerritoryPartnerItem:FlushSelect()
	self.BgSp.SpriteName = self.Data.formation_smart.partner == self.data.id and FormationConfig.PartnerBgSp.select or FormationConfig.PartnerBgSp.normal
end

function MirageTerritoryPartnerItem:OnClickItem()
	if self.Data.formation_smart.partner == self.data.id then
		self.Data:SetSelectPartner(self.data.id)
		self.Data:PosChangeReset()
	else
		local is_lock = self.data.vo.is_unlocked == PartnerConfig.LockState.lock
		if is_lock then
			self.Data:SetSelectPartner(self.data.id)
			self.Data:PosChangeReset()
			PublicPopupCtrl.Instance:Center(Language.Formation.PartnerNoLockTips)
			return
		end
		self.Data:SetSelectPartner(self.data.id)
		self.Data.formation_smart.pos_change.arrow = 0
		if self.Data:PartnerOnFight(self.data.id) then
			self.Data:PartnerChangePos(self.data.id)
		else
			self.Data:PartnerAddBegin()
			local index = self.Data:GetFormationIsEmptyPos()
			if index > 0 then --如果有位置空缺，则点击即可上阵
				self.Data:PartnerAddEnd(index,self.Data.formation_smart.partner)
			end
		end
	end
end

MirageTerritoryShowItem = DeclareMono("MirageTerritoryShowItem", UIWidgetBaseItem)

function MirageTerritoryShowItem:MirageTerritoryShowItem()
	self.Data = FightMirageTerritoryData.Instance
end

function MirageTerritoryShowItem:OnDestroy()
    self:uncareData()
end

function MirageTerritoryShowItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

	self:uncareData()
	self.care_handles = {}

	local is_empty = nil == data
	self.InfoObj:SetActive(not is_empty)
	if is_empty then return end

	self:careData(BagData.Instance:ViewData(), BindTool.Bind(self.FlushEleAttr, self), "element_crystal_id")
	self:careData(PartnerData.Instance.partner_crystal_change, BindTool.Bind(self.FlushEleAttr, self), "element_changed")

	self.Name.text = data.name
	if nil ~= data.ele_attr then self.EleAttr:SetData(data.ele_attr) end
	self.EleAttr:SetObjActive(nil ~= data.ele_attr)
	if data.member then self.ProfI.SpriteName = data.prof_sp end
	self.ProfI:SetObjActive(data.member)
	if not self.ui_obj then
		self.ui_obj = UIChDrawer.New()
		if data.role or data.member then
			self.ui_obj:SetData(data.appearance)
		elseif data.partner or data.pet  then
			self.ui_obj:SetMainSolo("actor/npc/" .. data.res_id ..  ".prefab")
		end
		self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj,
				view = self,
				show_shadow = false,
			}
		)
	else
		if data.role or data.member then
			self.ui_obj:SetData(data.appearance)
		elseif data.partner or data.pet then
			self.ui_obj:SetMainSolo("actor/npc/" .. data.res_id ..  ".prefab")
		end
	end
end

function MirageTerritoryShowItem:FlushEleAttr()
	local data = self.data
	if data.partner then
		local partner_info = PartnerData.Instance:GetPartnerInfoById(data.partner_id)
		data.ele_attr = partner_info.vo.crystal > 0 and EleCrystal.GetElement(partner_info.vo.crystal) or nil
		if nil ~= data.ele_attr then self.EleAttr:SetData(data.ele_attr) end
		self.EleAttr:SetObjActive(nil ~= data.ele_attr)
	end
end

function MirageTerritoryShowItem:OnClickItem(value)
	if self.data == nil and self.Data.formation_smart.partner == 0 then
		PublicPopupCtrl.Instance:Center(Language.MirageTerritory.EmptyTip)
	end
	if self.data ~= nil and self.Data.formation_smart.partner == self.data.partner_id then
		self.Data:SetSelectPartner(self.data.partner_id)
		return
	end
	local partner = self.Data.formation_smart.partner
	if partner > 0 then
		if self.Data:PartnerOnFight(partner) then
			self.Data:PosChange(value)
		else
			self.Data:PartnerAddEnd(value, partner)
		end
	else
		self.Data:SetSelectPartner(self.data and self.data.partner_id or 0)
		self.Data:PosChange(value)
	end
end

function MirageTerritoryShowItem:ArrowShow(value)
	self.RemoveObj:SetActive(value)
end

function MirageTerritoryShowItem:LightShow(value1, value2)
	self.LightObj:SetActive(value1 or false)
	self.Light2Obj:SetActive(value2 or false)
end

function MirageTerritoryShowItem:OnClickRemove()
	self.Data:RemoveShowItem(self.data)
end