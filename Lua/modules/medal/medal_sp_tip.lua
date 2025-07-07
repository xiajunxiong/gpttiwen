MedalSpTip = MedalSpTip or DeclareView("MedalSpTip", "medal/medal_sp_tip")
VIEW_DECLARE_MASK(MedalSpTip, ViewMask.BlockClose)
-- BtnForget
function MedalSpTip:MedalSpTip()
    -- self.pet_data = PetData.Instance
    self.data = BagData.Instance

    self.send_data = {}
end
function MedalSpTip:LoadCallback(param)
    AudioMgr:PlayEffect(AudioType.UI, "view_open_l2")
    if param ~= nil and param.pos ~= nil  then 
        UH.AnchoredPosition(self.PosRect, param.pos)
    end 
end

function MedalSpTip:OnFlush()
    self.item_data = self.data:GetMedalSpTip()

    self.LblName.text = self.item_data.sp_data.sp_name

    local effect_cfg = BagData.Instance:GetEquipEffectCfg(self.item_data.sp_data.sp_id )
    local list = BagData.Instance:GetMedalEffectRankList(effect_cfg.sp_type)
    self.stone_list:SetData(list)
    local pos = 1
    for k,v in pairs(list) do 
        if v.sp_id == self.item_data.sp_data.sp_id then 
            pos = k
        end 
    end 
    self.stone_list:ClickItem(pos)

    if self.flush_left_item_handle ~= nil then
        TimeHelper:CancelTimer(self.flush_left_item_handle)
        self.flush_left_item_handle = nil
    end
    self.flush_left_item_handle = TimeHelper:AddDelayTimer(function()
        if pos <= #list -2 then 
            self.stone_list:JumpVerticalIndex(pos,110)        
        else 
            self.stone_list:JumpVerticalIndex(#list -2,110,78)
        end 
    end,0.2)
end

function MedalSpTip:SetSkillBookSellPrice(data)

end

function MedalSpTip:CloseCallback()
    if self.flush_left_item_handle ~= nil then
        TimeHelper:CancelTimer(self.flush_left_item_handle)
        self.flush_left_item_handle = nil
    end
end

function MedalSpTip:OnClickMask()
    ViewMgr:CloseView(MedalSpTip)
end

function MedalSpTip:OnClickFuling()

    local has_num = Item.GetNum(self.send_data.item_id)
    local index = BagData.Instance:GetBagIndexById(ItemColumnType.Item, self.send_data.item_id)
    if self.item_data.send_param_t == nil then 
        PublicPopupCtrl.Instance:Center(Language.Medal.FulingLackItem)
        return 
    end 
    if has_num > 0 then
        local target_cell = nil 
        if self.item_data.send_param_t.param3 == 1 then 
            for k,v in pairs(self.data:ViewData().medal_info) do 
                if v.grid_index == self.item_data.send_param_t.param2 then 
                    target_cell = v
                    break
                end
            end 
        else 
            target_cell = BagData.Instance:GetItemByIndex(self.item_data.send_param_t.param1,self.item_data.send_param_t.param2)
        end 


        if target_cell.param.sp_id and target_cell.param.sp_id == 0 then 
            self:SendFulingInfo()
        elseif target_cell.param.sp_id and target_cell.param.sp_id == self.send_data.sp_id then 
            PublicPopupCtrl.Instance:Center(Language.Medal.EffectChangeSame)
            ViewMgr:CloseView(MedalSpTip)
        elseif target_cell.param.sp_id and target_cell.param.sp_id ~= self.send_data.sp_id then 
           self:SendFulingInfo()
        end 
    else
        PublicPopupCtrl.Instance:Center(Language.Medal.FulingLackItem)
    end 
end

function MedalSpTip:SendFulingInfo()
    local item_list = BagData.Instance:GetItemsByItemId(self.send_data.item_id)

    -- local sp_cfg = BagData.Instance:GetEquipEffectCfg(self.send_data.sp_id)
    local _item = Item.Create({item_id = self.send_data.item_id})
    local sp_name = string.format( Language.Medal.MedalNameColor,QualityColorStr[_item:Color()],self.send_data.sp_name ) 
    local dialog_param = {
        content = string.format(
            Language.Medal.EffectChangeWaring,
            sp_name
        ),
        confirm = {
            func =  function()
                local param_t = {
                    column = item_list[1].column_type, 
                    index = item_list[1].index,
                    num = 1,
                    param1 = self.item_data.send_param_t.param1,
                    param2 = self.item_data.send_param_t.param2,
                    param3 = self.item_data.send_param_t.param3,
                }
                BagCtrl.Instance:SendUseItem(param_t)
                ViewMgr:CloseView(DialogTipsView)
                ViewMgr:CloseView(MedalSpTip)
            end
        },
        no_cancel = false,
        tnr = DTTodayNotRemind.PetSkillLearn
    }
    PublicPopupCtrl.Instance:DialogTips(dialog_param)
end

function MedalSpTip:OnClickItem(data)
    local num_str = string.format( Language.Medal.FulingItemNum,Item.GetNum(data.item_id)) 
    UH.SetText(self.BagNum, num_str)
    self.send_data = data
end

MedalStoneSpItem = DeclareMono("MedalStoneSpItem", UIWidgetBaseItem)
function MedalStoneSpItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    local item_ = Item.Create({item_id = data.item_id})
    self.Item:SetData(item_)
	UH.SetText(self.desc,data.sp_desc_1)
end

function MedalStoneSpItem:Click()
    if self.ItemClick then self.ItemClick:Trigger() end
end

MedalGetWayView = MedalGetWayView or DeclareView("MedalGetWayView", "medal/medal_getway")
VIEW_DECLARE_MASK(MedalGetWayView, ViewMask.None)

function MedalGetWayView:MedalGetWayView()
    self.data = BagData.Instance

end 

function MedalGetWayView:LoadCallback(param)

    local lock_pos = self.gameObject.transform:InverseTransformPoint(param.pos_obj.transform.position)
    self.Pos.localPosition = Vector2.New(lock_pos.x, lock_pos.y)
end 

function MedalGetWayView:OnFlush()
    local cfg = Config.medal_cfg_auto.other[1]

    self.text_1.text = cfg.desc1
    self.text_2.text = cfg.desc2

    local list = string.split(cfg.param1,"|")
    local ready_list = {}
    for k,v in pairs(list) do 
        local vo = {}
        vo.npc = tonumber(v)
        table.insert( ready_list, vo)
    end 
    self.BtnList1Obj:SetActive(#list > 0)
    self.BtnList1:SetData(ready_list)

    list = string.split(cfg.param2,"|")
    self.BtnList2Obj:SetActive(#list > 0)
    self.BtnList2:SetData(list)
end 

function MedalGetWayView:OnClickBlank()
    ViewMgr:CloseView(MedalGetWayView)
end

MedalModuleFuncItem = DeclareMono("MedalModuleFuncItem", UIWidgetBaseItem)

function MedalModuleFuncItem:MedalModuleFuncItem()
	self.language = Language.Medal
end 

function MedalModuleFuncItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.Name.text = self.language.MedalExchangeName
    if self.ButtonSp then
        self.Name.color = ItemInfoConfig.ButtonConfig[2].LbColor
        self.ButtonSp.SpriteName = ItemInfoConfig.ButtonConfig[2].SpName
    end
    if not FunOpen.Instance:GetFunIsOpened(Mod.Medal.Build) then
        self.ItemInter.Interactable = false
        self.LockObj:SetActive(true)
    end
end

function MedalModuleFuncItem:OnClick()
    -- GetWayData.OpenPanel(self.data)

    SceneLogic.Instance:AutoToNpc(self.data.npc,nil,false,true)
    ViewMgr:CloseView(MedalGetWayView)
    if ViewMgr:IsOpen(BagView) then ViewMgr:CloseView(BagView) end 
end
