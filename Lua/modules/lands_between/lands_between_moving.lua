LandsBetweenMovingView = LandsBetweenMovingView or DeclareView("LandsBetweenMovingView", "lands_between/lands_between_moving",Mod.LandsBetween.Moving)
function LandsBetweenMovingView:LandsBetweenMovingView()
    self.Board:SetData(self:GetType(),Language.LandsBetween.Title.Moving,Vector2.New(730,655))
    self.Board:SetCloseFunc(BindTool.Bind(self.OnCloseFunc, self))
end

function LandsBetweenMovingView:OnCloseFunc()
    ViewMgr:CloseView(LandsBetweenMovingView)
end

LandsBetweenMovingPanel = LandsBetweenMovingPanel or DeclareMono("LandsBetweenMovingPanel", UIWFlushPanel)
function LandsBetweenMovingPanel:LandsBetweenMovingPanel()
    self.data = LandsBetweenData.Instance

    self.data_cares = {
        {data = self.data.my_info, func = self.FlushPanel},
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false},
    }
end 

function LandsBetweenMovingPanel:FlushPanel()
    local mov_list = self.data:GetMovingParam()
    self.list:SetData(mov_list)

    local temp_param = self.data:CacheMovingTemp()

    UH.SetText(self.cur_moving,temp_param.cur_num.. "/"..temp_param.total)
    self.next_moving:StampTime(temp_param.next_temp,TimeType.Type_Time_0)
    self.all_moving:StampTime(temp_param.all_temp,TimeType.Type_Time_0)

    -- self.cur_moving 
    -- self.next_moving
    -- self.all_moving 
end

function LandsBetweenMovingPanel:OnClickUseItem(data)
    if data.use_item then 
        -- PublicPopupCtrl.Instance:Center("使用了道具"..Item.GetQuaName( data.target_id ))
        LandsBetweenCtrl.Instance:UseApItem({type = data.oper_index})
    else 

        if not data.is_limit then 
            PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.MovingBuyLimit)
            return 
        end 
        LandsBetweenCtrl.Instance:BuyAP({is_gold = data.is_gold})
    end 
end

LandsBetweenMovingCell = LandsBetweenMovingCell or DeclareMono("LandsBetweenMovingCell", UIWidgetBaseItem)
function LandsBetweenMovingCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    local mov_item = Item.Init(data.target_id,data.use_item and ColorStr(data.num,data.num > 0 and COLORSTR.White or COLORSTR.Red10 ) or 0)
    self.calls:SetData(mov_item)

    if data.use_item then 
        UH.SetText(self.name,mov_item:QuaName())
        UH.SetText(self.main_info,mov_item:Desc())
    else 
        UH.SetText(self.name,data.name_desc)
        UH.SetText(self.main_info,data.main_desc)
        UH.SetText(self.btn_price,data.price)
        UH.SetIcon(self.btn_icon, mov_item:IconId())
    end 

    self.btn_useitem:SetActive(data.use_item)
    self.btn_buy:SetActive(not data.use_item)
end 

