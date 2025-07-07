ActPetSearchView = ActPetSearchView or DeclareView("ActPetSearchView", "serveractivity/openserver/pet_search_view",Mod.RandActivity.ActPetSearchView)

VIEW_DECLARE_LEVEL(ActPetSearchView,ViewLevel.L1)

function ActPetSearchView:ActPetSearchView()
    self.data = ServerActivityData.Instance.give_pet_draw_data
    self.info_change_handle = self.data:Care(BindTool.Bind(self.FlushItemCallback, self))
end

function ActPetSearchView:LoadCallback()
    self.config = Config.pet_search_auto
    local go_text = FunOpen.Instance:IsFunOpen(Mod.TreasureHunt.Main) and Language.PetSearch.BtnGo 
        or FunOpen.Instance:GetFunOpenLevel(Mod.TreasureHunt.Main)..Language.PetSearch.BtnClose
    
    UH.SetText(self.btn_text, go_text)
    for k,v in pairs(self.config.give_num) do 
        local vo = {}
        vo.act_lbl = Language.PetSearch["Act_"..k]
        vo.single_num = v.single_draw
        vo.fifth_num = v.fifth_company

        self.TextCells[k]:SetData(vo)
    end
    self:FlushItemCallback()
end

function ActPetSearchView:CloseCallback()
    self.data:Uncare(self.info_change_handle)
end

function ActPetSearchView:CanJump()
    local list = TreasureHuntData.Instance:GetOpenedTreasureID()
    return next(list) ~= nil
end

function ActPetSearchView:OnClickClose()
    ViewMgr:CloseView(ActPetSearchView)
end 

function ActPetSearchView:OnClickOperate()
    local is_open, open_str, open_level = FunOpen.Instance:IsFunOpen(Mod.TreasureHunt.Main)
    if not is_open then 
        PublicPopupCtrl.Instance:Center(string.format(Language.PetSearch.TipsFunClose, open_level or "30"))
    elseif self:CanJump() then  
        ViewMgr:OpenViewByKey(Mod.TreasureHunt.Main)
        ViewMgr:CloseView(ActPetSearchView)
    else
        PublicPopupCtrl.Instance:Center(Language.PetSearch.TipsCantOpen)
    end 
end 

function ActPetSearchView:OnClickReceive()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_GIVE_PET_DRAW, 1)
end

function ActPetSearchView:FlushItemCallback()
    local item_data = Item.New(self.config.other[1].reward_item)
    item_data.is_grey = self.data.give_mark ~= 0
    self.ItemCell:SetData(item_data)
    self.Receive_Block.enabled = self.data.give_mark == 0
    local str = self.data.give_mark == 0 and Language.PetSearch.CanGet or Language.PetSearch.Geted
    UH.SetText(self.Txt_Receive, str)
    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.RedPoint)
    end
    self.red_point:SetNum(self.data.give_mark == 0 and 1 or 0)
end

PetSearchTextCell = PetSearchTextCell or DeclareMono("PetSearchTextCell", UIWidgetBaseItem)

function PetSearchTextCell:SetData(data)
    UH.SetText(self.single_num,data.single_num)
    UH.SetText(self.fifth_num,data.fifth_num)
    self.fifth_num.gameObject:SetActive(data.fifth_num > 0)
end