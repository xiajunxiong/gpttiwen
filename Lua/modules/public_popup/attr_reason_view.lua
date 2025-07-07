AttrReasonView = AttrReasonView or DeclareView("AttrReasonView", "public_popup/attr_reason_view")
VIEW_DECLARE_MASK(AttrReasonView, ViewMask.BlockClose)

function AttrReasonView:AttrReasonView()
end

AttrReasonPanel = AttrReasonPanel or DeclareMono("AttrReasonPanel", UIWFlushPanel)
function AttrReasonPanel:AttrReasonPanel()
    self.data = AttrReasonData.Instance
	self.lang = Language.Finish
	self.data_cares = {
		{data = self.data:GetSelectData(), func = self.onFlush, init_call = false},
	}
	self.flush_events = {EventProtocol.ToEventKey(SCAttrListReason)}
end

function AttrReasonPanel:Awake()
	UIWFlushPanel.Awake(self)
end

function AttrReasonPanel:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
end

function AttrReasonPanel:onFlush()
	local select_data = self.data:GetSelectData()
	local reason_type = select_data.reason_type
	local param1 = select_data.param1
	local attr_type = select_data.attr_type
	local position = select_data.position
	UH.LocalPos(self.PosRect, position)
	local bg_y = position.y < -200 and math.abs(position.y + 200) or 0
	UH.AnchoredPosition(self.BGRect, Vector2.New(0, bg_y, 0))
	
	local reason_list = self.data:GetReasonShowList(reason_type, param1, attr_type)
	local str_t = Language.AttrListReason.AttrDesc[attr_type] or {}
	local desc
	if attr_type == BattleAttriType.Defense then
		local num = 0
		for i,v in pairs(reason_list) do
			num = num + v.value
		end
		-- 防御/（1.2*（等级+20））-----为千分比数值
		local level = RoleData.Instance:GetRoleLevel()
		local rate = num / (1.2 * (level + 20))
		rate = rate / 10
		rate = math.floor(rate * 100) / 100
		desc = string.format(str_t.desc, rate)
	else
		desc = str_t.desc
	end
	if str_t then
		UH.SetText(self.AttrName, str_t.name)
		UH.SetText(self.AttrDesc, desc)
	end

	
	if #reason_list == 0 then
		self.AttrListNode:SetActive(false)
	else
		self.AttrListNode:SetActive(true)
		self.AttrList:SetData(reason_list)
		if #reason_list < 4 then
			UH.SizeDelta(self.AttrListObj, Vector3.New(280, 25 + (#reason_list) * 37))
		else
			UH.SizeDelta(self.AttrListObj, Vector3.New(280, 125))
		end
	end
end

AttrReasonItem = AttrReasonItem or DeclareMono("AttrReasonItem", UIWidgetBaseItem)
function AttrReasonItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local select_data = AttrReasonData.Instance:GetSelectData()
	local lang_name = AttrReasonConfig.REASON_NAME[select_data.reason_type]
	UH.SetText(self.Name, Language.AttrListReason[lang_name][self.data.type])
	UH.SetText(self.Value, DataHelper.Percent(select_data.attr_type, self.data.value))
end
