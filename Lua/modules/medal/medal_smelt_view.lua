MedalSmeltView = MedalSmeltView or DeclareView("MedalSmeltView", "medal/medal_smelt",Mod.Medal.Aterlier)
VIEW_DECLARE_MASK(MedalSmeltView,ViewMask.BgBlockClose)
function MedalSmeltView:MedalSmeltView()
    self.Board:SetData(self:GetType(), Language.Medal.SmeltView, Vector2.New(970, 592))
end 

function MedalSmeltView:CloseCallback()
    MedalData.Instance:SetSelectSmeltMedal()
end 

MedalSmeltSinglePanel = MedalSmeltSinglePanel or DeclareMono("MedalSmeltSinglePanel", UIWFlushPanel)
function MedalSmeltSinglePanel:MedalSmeltSinglePanel()
    self.data = MedalData.Instance
    self.data_cares = {
        {data = self.data.smelt_oper,func = self.FlushViewShow ,keys = {"select"}},
    }
end

function MedalSmeltSinglePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

end

function MedalSmeltSinglePanel:FlushViewShow()
    local data = self.data:GetSelectSmeltMedal()[1]

    self.Name.text = data:Name()
    self.NameShow.text = data:Name()
    self.Cell:SetData(data)
    self.CellShow:SetData(data)
    self.Level.text = data:LimitLv()..Language.Common.Level
    local sp_cfg = data:SpEffectCfg()
    self.Special.text = string.format(Language.ItemInfo.ItemTitles.EffectOtE, QualityColorStr[sp_cfg.sp_priority], sp_cfg.description)
    local sp_2_cfg = data:SpEffectSecondCfg()
    self.Special2.text = string.format(Language.ItemInfo.ItemTitles.EffectOtE, QualityColorStr[sp_2_cfg.sp_priority], sp_2_cfg.description)
    self.Desc.text = data:Desc()

    local color = data:Color() or 1
    self.Quality.SpriteName = ItemInfoConfig.QualitySp[color]

    for k, v in ipairs(data.param.attr_list) do
        if v.attr_value ~= 0 then
            local str = string.format(Language.ItemInfo.ItemTitles.EquipAttr2, 
            Language.Common.AttrList[v.attr_type], 
            DataHelper.IsPercent(v.attr_type) and Percent(v.attr_value) or v.attr_value)

            self.AttrObjList[k]:SetActive(true)
            UH.SetText(self.AttrList[k], str)
        else 
            self.AttrObjList[k]:SetActive(false)
        end 
    end 

    self.DustNum.text = BagData.Instance:GetCurSelectMedalDust()
end

function MedalSmeltSinglePanel:OnClickSmelt()
    local data = self.data:GetSelectSmeltMedal()[1]

    local param_t = {
        req_type = MEDAL_REQ_TYPE.DECOMPOSE,
        param1 = data.column_type, 
        param2 = data.index
    }

    self.data:SetCurSelectMedal(data)
    BagCtrl.Instance:SendMedalReq(param_t)

	ViewMgr:CloseView(MedalSmeltView)
end

function MedalSmeltSinglePanel:OnClickChange()
    self.Mulit:SetActive(true)
    self.Single:SetActive(false)
end


MedalSmeltMultiPanel = MedalSmeltMultiPanel or DeclareMono("MedalSmeltMultiPanel", UIWFlushPanel)
function MedalSmeltMultiPanel:MedalSmeltMultiPanel()
    self.data = MedalData.Instance
    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushViewDust},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushViewShow},
    }

    self.SelectedText.text = Language.Medal.SmeltViewAllShow
end

function MedalSmeltMultiPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

end 

function MedalSmeltMultiPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushViewShow()
end 

function MedalSmeltMultiPanel:FlushViewShow()
    self.Items:SetData(self.data:GetGridForSmeltMedal()) 
end

function MedalSmeltMultiPanel:OnClickCloseShaiXuan()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
end

function MedalSmeltMultiPanel:FlushViewDust()
    self.DustGet.text = self.data:GetSelectSmeltMedalDust()
    self.DustHas.text = Item.GetNum(CommonItemId.XianFen)
end 

function MedalSmeltMultiPanel:OnClickItem(data)
    if data == nil or data.item_id == 0 then 
        return 
    end 

    self.InfoObj:SetActive(true)
    self.EmptyObj:SetActive(false)
    -- flush info 
    self.Name.text = data:QuaName() -- string.format( Language.Medal.MedalNameColor,QualityColorStr[data:Color()],data:Name() ) 
    self.Cell:SetData(data)
    self.Level.text = data:LevelDesc()
    if data:WithSp() then 
        self.Special.text = string.format(Language.Common.Xstr, QualityColorStr[data:Color()], data:SpEffectCfg().description)
    else 
        self.Special.text = Language.ItemInfo.None
    end 

    if data:WithSecond() then 
        self.Special2.gameObject:SetActive(true)
        self.Special2.text = string.format(Language.Common.Xstr, QualityColorStr[data:GetMedalSecondQua()], data:SpEffectSecondCfg().description)
    else 
        self.Special2.gameObject:SetActive(false)
        self.Special2.text = ""
    end 

    self.Desc.text = data:Desc()

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

function MedalSmeltMultiPanel:OnClickLevelBtn(data)
    self.SelectedText.text = data.qua > 0 and string.format(data.name,QualityColorStrShallow[data.qua]) or data.name
    self.Items:CancelSelect()
    self.data:SetSelectSmeltMedal()


    for k,v in pairs(self.data:GetGridForSmeltMedal({noempty = true})) do
        if v:GetMedalQua() == data.qua then 
            self:CheckAndClickItem(k,v)
        elseif data.qua == -1 then 
            self:CheckAndClickItem(k,v)
        end 
    end 
    self:OnClickLevelSelect()
    self:FlushViewDust()
end

function MedalSmeltMultiPanel:CheckAndClickItem(k,v)
    -- local mono = self.Items:GetItemMonoByIndex(k)
    -- LogError("mono?", mono == nil )
    -- if mono == nil then 
    self:OnClickItem(v)
    self.Items:SetectItem(k,self.data:CheckMedalSmeltIsClicked(v))
    -- else 
    --     self.Items:ClickItem(k) -- ClickItemData(v)
    -- end 
end

function MedalSmeltMultiPanel:OnClickLevelSelect()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
    if activeSelf then
        self.BtnList:SetData(MedalConfig.smelt_quick_select)
        --(self.data:GetMedalSmeltShaiXuanList())
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function MedalSmeltMultiPanel:OnClickMultiSmelt()
    local data = self.data:GetSelectSmeltMedal()

    if #data == 0 then 
        PublicPopupCtrl.Instance:Center(Language.Medal.SmeltViewNotSelect)
        return 
    end 

    local function send_msg()

        for k,v in pairs(data) do 
            local param_t = {
                req_type = MEDAL_REQ_TYPE.DECOMPOSE,
                param1 = v.column_type, 
                param2 = v.index
            }
    
            self.data:SetSelectSmeltMedal()
            BagCtrl.Instance:SendMedalReq(param_t)
        end 
    
        if ViewMgr:IsOpen(MedalAtelierView) then
            self.Items:CancelSelect()
            self.InfoObj:SetActive(false)
            self.EmptyObj:SetActive(true)
        end 
		ViewMgr:CloseView(DialogTipsView)
	end
	local info = {
		content = string.format(Language.Medal.SmeltPopTips, #data or 0, self.data:GetSelectSmeltMedalDust() or 0),
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

MedalMultiSmeltCell = DeclareMono("MedalMultiSmeltCell", UIWidgetBaseItem)
function MedalMultiSmeltCell:MedalMultiSmeltCell()

end

function MedalMultiSmeltCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local _item = Item.Create(data)
    self.Item:SetData(_item)
end

function MedalMultiSmeltCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end

function MedalMultiSmeltCell:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected and self.data.item_id == 0 then
        self.Selector.isOn = false
    end
end

function MedalMultiSmeltCell:OnClickItem()
    self.Mono:OnClickItem(self.data)
end

MedalSmeltBtnItem = DeclareMono("MedalSmeltBtnItem", UIWidgetBaseItem)
function MedalSmeltBtnItem:MedalSmeltBtnItem()

end

function MedalSmeltBtnItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Name.text = data.qua > 0 and string.format(data.name,QualityColorStr[data.qua]) or data.name
end