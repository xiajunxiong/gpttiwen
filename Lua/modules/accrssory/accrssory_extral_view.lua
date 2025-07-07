AccrssoryExtralView = AccrssoryExtralView or DeclareView("AccrssoryExtralView", "accrssory/accrssory_extral_view")
VIEW_DECLARE_MASK(AccrssoryExtralView, ViewMask.BgBlock)
function AccrssoryExtralView:AccrssoryExtralView()

end

function AccrssoryExtralView:OnClickClose()
    ViewMgr:CloseView(AccrssoryExtralView)
end

--------------AccrssoryExtralPanel--------------
AccrssoryExtralPanel = AccrssoryExtralPanel or DeclareMono("AccrssoryExtralPanel", UIWFlushPanel)
function AccrssoryExtralPanel:AccrssoryExtralPanel()
    self.data = AccrssoryData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid,func = self.OnFlushPanel, init_call = false},
        {data = self.data.accrssory_extra_data,keys = {"flush"},func = self.OnFlushPanel},
    }

    local param_t = {mono = self,
		start_effect = "2161037",
        pre_pause = 1.5,
		end_func = BindTool.Bind(self.ShowComplete, self),
	    end_func_time = 0}
		self.EffectPos:SetData(param_t)
    
    self.old_level = nil
end

function AccrssoryExtralPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data.accrssory_extra_data.type = 0
end

function AccrssoryExtralPanel:OnFlushPanel()
    local main_cfg = self.data:GetExtralEquip(self.data.accrssory_extra_data.type)

    if self.old_level ~= nil then 
        if main_cfg.level ~= self.old_level then 
            self.EffectTool:Play("2161038",self.EffectPos.CompletePos)
        end 
    end 

    UH.SetIcon(self.TargetIcon,main_cfg.icon)
    UH.SetText(self.CurLevel,main_cfg.level_cfg.level..Language.Common.Level)

    self.old_level = main_cfg.level

    self.OnePart:SetActive(main_cfg.is_max or main_cfg.level == 0)
    self.UpPart:SetActive(not main_cfg.is_max and main_cfg.level > 0)

    self.StuffUp:SetActive(not main_cfg.is_max)
    self.StuffMax:SetActive(main_cfg.is_max)

    self.EffNextObj:SetActive(not main_cfg.is_max and main_cfg.level > 0)

    UH.SetText(self.BtnStr,main_cfg.level == 0 and Language.Partner.GiftBtnAct or Language.Partner.GiftBtnLevel)
    UH.SetText(self.EffCur,string.format(main_cfg.eff_desc,COLORSTR.Yellow6))
    for i = 1,2 do 
        self.OneAttrs[i]:SetData({
            type = main_cfg.level_cfg["att_type_"..i],
            value = main_cfg.level_cfg["attr_value_"..i],
            add = 0,
        })
    end 
    
    
    if not main_cfg.is_max then 
        local next_cfg = self.data:GetExtralEquip(self.data.accrssory_extra_data.type,main_cfg.level+1)
        for i = 1,2 do 
            --  and Percent(v.attr_value) or v.attr_value)
            local is_per = DataHelper.IsPercent(main_cfg.level_cfg["att_type_"..i])
            self.UpAttrs[i]:SetData({
                type = main_cfg.level_cfg["att_type_"..i],
                value = is_per and Percent(main_cfg.level_cfg["attr_value_"..i]) or main_cfg.level_cfg["attr_value_"..i],
                add = is_per and Percent(next_cfg.level_cfg["attr_value_"..i] - main_cfg.level_cfg["attr_value_"..i]) 
                    or next_cfg.level_cfg["attr_value_"..i] - main_cfg.level_cfg["attr_value_"..i],
            })
        end 
        UH.SetText(self.EffNext,string.format(next_cfg.eff_desc,COLORSTR.Green3))
        self.stuff_item = Item.Init(next_cfg.level_cfg.item_list[0].item_id,0)
        self.need_stuff = next_cfg.level_cfg.item_list[0].num
        self.StuffCall:SetData(self.stuff_item) 
        UH.SetText(self.StuffName,self.stuff_item:QuaName(true))

        local num_str = DataHelper.ConsumeNum(self.stuff_item:Num(), self.need_stuff, true)

        self.BtnInter.Interactable = self.need_stuff <= self.stuff_item:Num()
        UH.SetText(self.StuffNum,num_str)
    end 

end

function AccrssoryExtralPanel:OnEffectComplete()
    self.Block:SetActive(false)
end

function AccrssoryExtralPanel:OnClickExtralUp()
    if self.stuff_item == nil then 
        return 
    end 

    if self.stuff_item:Num() < self.need_stuff then
        PublicPopupCtrl.Instance:Center(Language.Live.Tip7)
        return 
    end 
    -- LogError("?haopopa ",AccrssoryData.Instance.accrssory_extra_data.type)
    self.EffectPos:Play("2161011")
    self.Block:SetActive(true)
end

function AccrssoryExtralPanel:ShowComplete()
    AccrssoryCtrl.Instance:SendAccrssoryReq({
        req_type = AccrssoryReqType.Extral,
        param_1 = AccrssoryData.Instance.accrssory_extra_data.type,
    })
end

function AccrssoryExtralPanel:OnCloseMaskClick()
    self.DescMask:SetActive(false)
end

function AccrssoryExtralPanel:OnClickEffTips()
    self.DescMask:SetActive(true)
    local tex_show = self.data:GetExtralAllDesc()
    UH.SetText(self.DescShow,tex_show)
end

AccrssoryExtralAttrShow = AccrssoryExtralAttrShow or DeclareMono("AccrssoryExtralAttrShow", UIWidgetBaseItem)
function AccrssoryExtralAttrShow:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.AttrType,Language.Common.AttrList2[data.type]..":")
    UH.SetText(self.AttrAdd,data.add)
    UH.SetText(self.AttrValue,data.value)
end 


AccrssoryExtralLevelInfo = AccrssoryExtralLevelInfo or DeclareMono("AccrssoryExtralLevelInfo", ItemInfoViewItem)

function AccrssoryExtralLevelInfo:OnEnable()
    ItemInfoViewItem.OnEnable(self)
    self.List:SetData(AccrssoryData.Instance:GetExtralAllDescByIdWithNomal(self.item1.item_id))
end

AccrssoryExtralItemLevelItem = AccrssoryExtralItemLevelItem or DeclareMono("AccrssoryExtralItemLevelItem", UIWidgetBaseItem)
function AccrssoryExtralItemLevelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Level,Format(Language.Bicycle.Level,data.level))
    UH.SetText(self.Desc,data.desc)
end

AccrssoryExtralAttriItemInfo = AccrssoryExtralAttriItemInfo or DeclareMono("AccrssoryExtralAttriItemInfo", ItemInfoViewItem)

function AccrssoryExtralAttriItemInfo:OnEnable()
    ItemInfoViewItem.OnEnable(self)
    UH.SetText(self.Title, Language.PetWeapon.ItemAttriTitle)
    self.AttriList:SetData(AccrssoryData.Instance:GetExtralBaseAttri(self.item1.item_id))
end
