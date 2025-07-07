
MedalFulingView = MedalFulingView or DeclareView("MedalFulingView", "medal/medal_fuling")
-- VIEW_DECLARE_MASK(MedalFulingView,ViewMask.BgBlockClose)
function MedalFulingView:MedalFulingView()
end 

function MedalFulingView:LoadCallback(param_t)
end

function MedalFulingView:CloseCallback()
    MedalData.Instance:SetCostMedal()
    MedalData.Instance:SetFulingTarget()
end

function MedalFulingView:OnClickClose()
    ViewMgr:CloseView(MedalFulingView)

end

MedalFulingFlushPanel = MedalFulingFlushPanel or DeclareMono("MedalFulingFlushPanel", UIWFlushPanel)
function MedalFulingFlushPanel:MedalFulingFlushPanel()
    self.data = MedalData.Instance

    self.data_cares = {
        {data = self.data.cost_medal, func = self.FlushPreView},
    }
end

function MedalFulingFlushPanel:Awake()
    UIWFlushPanel.Awake(self)
end


function MedalFulingFlushPanel:onFlush()
    local bag_list = self.data:GetMedalFulingMaterial()
    self.BagList:SetData(bag_list) 
    self.BagList:ClickItem(1)

    self.Empty:SetActive(#bag_list == 0)
    self.FulingInter.Interactable = #bag_list > 0
end

function MedalFulingFlushPanel:FlushPreView()
    local item_data = self.data:GetFulingTarget()
    local fuling_data = self.data:GetFulingTargetCfg()
    local rank = string.format(Language.Medal.MedalRank, DataHelper.GetWord(fuling_data.sp_item_cfg.rank))
    local stone_name = string.format(Language.Medal.FuLingStoneName,rank,item_data:QuaName())
    UH.SetText(self.ItemShow,string.format(Language.Medal.FuLingPreViewTitle,string.format(Language.Medal.MedalNameColor,item_data:ColorStr(),stone_name)))

    local flag = self.data.cost_medal.item == nil
    self.PreInfo:SetActive( not flag )
    self.PreTips:SetActive( flag )
    if flag then return end 

    local pre_item = Item.Create({
        item_id = self.data.cost_medal.item.item_id,
        param = {
            grade = fuling_data.sp_item_cfg.rank,
            sp_id = fuling_data.sp_item_cfg.sp_id,
        }
    })
    self.MedalCell:SetData(pre_item)
    UH.SpriteName(self.QuaSp,ItemInfoConfig.QualitySp[pre_item:Color()])

    UH.SetText(self.MedalName,pre_item:QuaName())
    UH.SetText(self.MedalGrade,pre_item:LevelDesc())

    for i = 1,4 do self.BaseAttr[i]:SetObjActive(false) end 
    for k,v in pairs(pre_item:BaseAttr()) do 
        self.BaseAttr[k]:SetObjActive(v.attr_value >0)
        local str = string.format(Language.ItemInfo.ItemTitles.EquipAttr2, 
			Language.Common.AttrList[v.attr_type], 
            DataHelper.IsPercent(v.attr_type) and Percent(v.attr_value) or v.attr_value)
        UH.SetText(self.BaseAttr[k], str)
    end 

    UH.SetText(self.MedalEffect,string.format(Language.Common.Xstr, QualityColorStr[pre_item:GetMedalQua()], fuling_data.effect_cfg.description))
    UH.SetText(self.MedalDesc,pre_item:Desc())

end

function MedalFulingFlushPanel:CatchFulingParam()
    self.item_data = {}
    if self.data.cost_medal.item.param.is_equip then 
        self.item_data.param2 = self.data.cost_medal.item.vo.grid_index
        self.item_data.param3 = 1
    else 
        self.item_data.param1 = self.data.cost_medal.item.column_type
        self.item_data.param2 = self.data.cost_medal.item.index
        self.item_data.param3 = 0
    end 
end

function MedalFulingFlushPanel:OnClickFuling()
    if self.data.cost_medal.item == nil then 
        PublicPopupCtrl.Instance:Center(Language.Medal.SmeltViewNotSelect)
        return
    end 

    local item_data = self.data:GetFulingTarget()
    local fuling_data = self.data:GetFulingTargetCfg()

    local pre_item = Item.Create({
        item_id = self.data.cost_medal.item.item_id,
        param = {
            grade = fuling_data.sp_item_cfg.rank,
            sp_id = fuling_data.sp_item_cfg.sp_id,
        }
    })

    self:CatchFulingParam()

    local rank = string.format(Language.Medal.MedalRank, DataHelper.GetWord(fuling_data.sp_item_cfg.rank))
    local stone_name = string.format(Language.Medal.FuLingStoneName,rank,item_data:QuaName())
    local sp_name = string.format(Language.Medal.MedalNameColor,item_data:ColorStr(),stone_name)
    local dialog_param = {
        content = string.format(
            Language.Medal.EffectChangeWaring,
            sp_name
        ),
        confirm = {
            func =  function()
                local param_t = {
                    column = item_data.column_type, 
                    index = item_data.index,
                    num = 1,
                    param1 = self.item_data.param1,
                    param2 = self.item_data.param2,
                    param3 = self.item_data.param3,
                }
                BagCtrl.Instance:SendUseItem(param_t)
                ViewMgr:CloseView(DialogTipsView)

                ViewMgr:CloseView(MedalFulingView)
                PublicPopupCtrl.Instance:Center(string.format(Language.Medal.MedalFulingSuccess,pre_item:QuaName()))
            end
        },
        no_cancel = false,
        tnr = DTTodayNotRemind.MedalSpFuling
    }
    PublicPopupCtrl.Instance:DialogTips(dialog_param)
end

MedalFulingCostCell = DeclareMono("MedalFulingCostCell", UIWidgetBaseItem)
function MedalFulingCostCell:MedalFulingCostCell()
    self.medal_data = MedalData.Instance
end

function MedalFulingCostCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local _item = Item.Create(data)
    self.Item:SetData(_item)
    self.Equiped:SetActive(_item.param.is_equip)
end

function MedalFulingCostCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end

function MedalFulingCostCell:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected and self.data.item_id == 0 then
        self.Selector.isOn = false
    end
end

function MedalFulingCostCell:OnClickItem()
    self.medal_data:SetCostMedal(self.data)
end