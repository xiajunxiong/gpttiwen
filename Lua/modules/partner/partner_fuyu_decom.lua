PartnerFuYuDecom = PartnerFuYuDecom or DeclareView("PartnerFuYuDecom", PartnerConfig.ResPath .. "partner_fuyu_decom")
function PartnerFuYuDecom:PartnerFuYuDecom()
    self.Board:SetData(self:GetType(), Language.FuYu.SmeltView, Vector2.New(970, 592))
end

PartnerFuYuDecomPanel = PartnerFuYuDecomPanel or DeclareMono("PartnerFuYuDecomPanel", UIWFlushPanel)
function PartnerFuYuDecomPanel:PartnerFuYuDecomPanel()
    self.data = PartnerData.Instance
    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushViewDust, init_call = false},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.onFlush, init_call = false},
    }

    self.SelectedText.text = Language.FuYu.SmeltViewAllShow
end

function PartnerFuYuDecomPanel:onFlush()
    self.Items:SetData(self.data:GetGridForSmeltMedal()) 
end
function PartnerFuYuDecomPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetSelectSmeltMedal()
end

function PartnerFuYuDecomPanel:OnClickCloseShaiXuan()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
end

function PartnerFuYuDecomPanel:FlushViewDust()
    self.DustGet.text = self.data:GetSelectSmeltMedalDust()
    -- self.DustHas.text = Item.GetNum(CommonItemId.XianFen)
end 

function PartnerFuYuDecomPanel:OnClickItem(data)
    if data == nil or data.item_id == 0 then 
        return 
    end 

    self.InfoObj:SetActive(true)
    self.EmptyObj:SetActive(false)
    -- flush info 
    UH.SetText(self.Name, data:QuaName()) -- string.format( Language.Medal.MedalNameColor,QualityColorStr[data:Color()],data:Name() ) 
    self.Cell:SetData(data)
    UH.SetText(self.Level, Format("Lv.%s", data:FuYuLevel()))
    UH.SetText(self.Special, data:FuYuEffectDesc(nil, true))
    --[[ if data:WithSp() then 
        self.Special.text = string.format(Language.Common.Xstr, QualityColorStr[data:Color()], data:SpEffectCfg().description)
    else 
        self.Special.text = Language.ItemInfo.None
    end  ]]
    local extra_id = data:ExtraId()
    if extra_id > 0 then 
        self.Special2.gameObject:SetActive(true)
        local config = BagData.Instance:GetEquipEffectCfg(extra_id)
		local desc2 = config.description
		desc2 = data:SubExtraName(desc2)
        UH.SetText(self.Special2, desc2)
    else 
        self.Special2.gameObject:SetActive(false)
        UH.SetText(self.Special2, "")
    end 

    UH.SetText(self.Desc, data:Desc())

    local color = data:Color() or 1
    self.Quality.SpriteName = ItemInfoConfig.QualitySp[color]

    for i = 1,4 do self.AttrObjList[i]:SetActive(false) end 
    for k, v in ipairs(data:BaseAttr()) do
        if v.attr_value ~= 0 then
            local str = string.format(Language.ItemInfo.ItemTitles.EquipAttr2, 
            Language.Common.AttrList[v.attr_type], 
            DataHelper.IsPercent(v.attr_type) and Percent(v.attr_value) or v.attr_value)

            self.AttrObjList[k]:SetActive(true)
            UH.SetText(self.AttrList[k], str)
        end 
    end 

    self.data:SetSelectSmeltMedal(data)
    self:FlushViewDust()
end

function PartnerFuYuDecomPanel:OnClickLevelBtn(data)
    self.SelectedText.text = data.qua > 0 and string.format(data.name,QualityColorStrShallow[data.qua]) or data.name
    self.Items:CancelSelect()
    self.data:SetSelectSmeltMedal()


    for k,v in pairs(self.data:GetGridForSmeltMedal({noempty = true})) do
        if v:Color() == data.qua then 
            self:CheckAndClickItem(k,v)
        elseif data.qua == -1 then 
            self:CheckAndClickItem(k,v)
        end 
    end 
    self:OnClickLevelSelect()
    self:FlushViewDust()
end

function PartnerFuYuDecomPanel:CheckAndClickItem(k,v)
    -- local mono = self.Items:GetItemMonoByIndex(k)
    -- LogError("mono?", mono == nil )
    -- if mono == nil then 
    self:OnClickItem(v)
    self.Items:SetectItem(k,self.data:CheckMedalSmeltIsClicked(v))
    -- else 
    --     self.Items:ClickItem(k) -- ClickItemData(v)
    -- end 
end

function PartnerFuYuDecomPanel:OnClickLevelSelect()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
    if activeSelf then
        self.BtnList:SetData(PartnerConfig.fuyu_decom_quick_select)
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function PartnerFuYuDecomPanel:OnClickMultiSmelt()
    local data = self.data:GetSelectSmeltMedal()

    if #data == 0 then 
        PublicPopupCtrl.Instance:Center(Language.FuYu.SmeltViewNotSelect)
        return 
    end 

    local function send_msg()
        for k,v in pairs(data) do 
            PartnerCtrl.Instance:ReqFuYuDecompose(v.column_type, v.index)
            --BagCtrl.Instance:SendMedalReq(param_t)
        end 
        self.data:SetSelectSmeltMedal()
        --[[ if ViewMgr:IsOpen(MedalAtelierView) then
            self.Items:CancelSelect()
            self.InfoObj:SetActive(false)
            self.EmptyObj:SetActive(true)
        end  ]]
		ViewMgr:CloseView(DialogTipsView)
	end
	local info = {
		content = string.format(Language.FuYu.SmeltPopTips, #data or 0, self.data:GetSelectSmeltMedalDust() or 0),
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

FuYuMultiSmeltCell = DeclareMono("FuYuMultiSmeltCell", UIWidgetBaseItem)
function FuYuMultiSmeltCell:FuYuMultiSmeltCell()

end

function FuYuMultiSmeltCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local _item = Item.Create(data)
    self.Item:SetData(_item)
end

function FuYuMultiSmeltCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end

function FuYuMultiSmeltCell:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected and self.data.item_id == 0 then
        self.Selector.isOn = false
    end
end

function FuYuMultiSmeltCell:OnClickItem()
    self.Mono:OnClickItem(self.data)
end

FuYuSmeltBtnItem = DeclareMono("FuYuSmeltBtnItem", UIWidgetBaseItem)
function FuYuSmeltBtnItem:FuYuSmeltBtnItem()

end

function FuYuSmeltBtnItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Name.text = data.qua > 0 and string.format(data.name,QualityColorStr[data.qua]) or data.name
end