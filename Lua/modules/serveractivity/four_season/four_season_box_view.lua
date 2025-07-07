FourSeasonBoxView = FourSeasonBoxView or DeclareView("FourSeasonBoxView", "serveractivity/four_season/four_season_box", Mod.PlanActivity.FourSeasonBox)
VIEW_DECLARE_LEVEL(FourSeasonBoxView, ViewLevel.L1)
VIEW_DECLARE_MASK(FourSeasonBoxView, ViewMask.BgBlock)
function FourSeasonBoxView:FourSeasonBoxView()
end

function FourSeasonBoxView:OnCloseClick()
    ViewMgr:CloseView(FourSeasonBoxView)
end

----------------------------FourSeasonBoxPanel----------------------------
FourSeasonBoxPanel = FourSeasonBoxPanel or DeclareMono("FourSeasonBoxPanel", UIWFlushPanel)
function FourSeasonBoxPanel:FourSeasonBoxPanel()
    self.Data = FourSeasonData.Instance
    self.language = Language.FourSeasonBox
    self.data = self.Data.box_data
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end

function FourSeasonBoxPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.now_page = 1
    self.box_style_num = self.Data:BoxStyleNum()
    self:InitStyle()
    self:FlushAll()
end

function FourSeasonBoxPanel:InitStyle()
    local style = self.Data:BoxStyle()
    local name = ActivityRandData.GetName(ACTIVITY_TYPE.RAND_FOUR_SEASON_BOX)
    for i = 1, self.box_style_num do
        self.ObjBgs[i]:SetActive(i == style)
        self.ObjTitles[i]:SetActive(i == style)
        UH.SetText(self.TxtNames[i], name)
        self.ObjDescs[i]:SetActive(i == style)
        self.Times[i]:SetObjActive(i == style)
    end
    UH.SpriteName(self.ImgZuoZi, "ZhuoZi" .. style)
    UH.SpriteName(self.ImgClose, "GuanBi" .. style)
    self.Times[style]:CreateActTime(ACTIVITY_TYPE.RAND_FOUR_SEASON_BOX, TimeType.Type_Time_3, Language.FourSeasonBox.Time)
end

function FourSeasonBoxPanel:FlushAll()
    -- 这里需要自己整理排序
    local box_data = self.Data:BoxBaseData()
    local box_max_show = self.Data:BoxMaxShow()
    table.sort(box_data, DataHelper.WeightSort("type", function(data)
        return self.Data:BoxIsCanBuy(data) and 0 or 1000
    end))
    local index = 0
    self.base_data = {}
    for k, v in pairs(box_data) do
        local index1, index2 = math.floor(index / box_max_show) + 1, index % box_max_show + 1
        if not self.base_data[index1] then
            self.base_data[index1] = {}
        end
        if not self.base_data[index1][index2] then
            self.base_data[index1][index2] = {}
        end
        v.my_index = index
        self.base_data[index1][index2] = v
        index = index + 1
    end
    self.max_page = #self.base_data
    self:OnChangeClick(0)
end

function FourSeasonBoxPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function FourSeasonBoxPanel:OnFreeClick()
    if not self:JudgeCommon() then
        return
    end
    if self.Data:BoxRemind() > 0 then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_FOUR_SEASON_BOX, FourSeasonBoxReqType.Free)
    end
end

function FourSeasonBoxPanel:OnChangeClick(value)
    self.now_page = self.now_page + value
    if self.now_page > self.max_page then
        self.now_page = self.max_page
    end
    if self.now_page < 1 then
        self.now_page = 1
    end
    self.ObjLeft:SetActive(self.now_page ~= 1)
    self.ObjRight:SetActive(self.now_page ~= self.max_page)
    self.List:SetData(self.base_data[self.now_page])

    local remind_num = self.Data:BoxRemind()
    self.RightRemind:SetNum(remind_num)
    -- 最后一页才显示免费礼袋
    if self.now_page == self.max_page then
        self.ObjFree:SetActive(remind_num > 0)
        self.Remind:SetNum(remind_num)
    else
        self.ObjFree:SetActive(false)
        self.Remind:SetNum(0)
    end
end

function FourSeasonBoxPanel:OnBuyClick(data)
    if not self:JudgeCommon() then
        return
    end
    if not self.Data:BoxIsCanBuy(data) then
        PublicPopupCtrl.Instance:Center(self.language.NotTimesTip)
        return false
    end
    self.now_buy_data = data
    local fee = data.buy_money / 10
    ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_FOUR_SEASON_BOX, fee, ACTIVITY_TYPE.RAND_FOUR_SEASON_BOX, FourSeasonBoxReqType.Buy, data.seq)
end

function FourSeasonBoxPanel:JudgeCommon()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_FOUR_SEASON_BOX) then
        PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
        return false
    end
    return true
end

function FourSeasonBoxPanel:SendBuy()
    if TableIsEmpty(self.now_buy_data) then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_FOUR_SEASON_BOX, self.now_buy_data.buy_money)
    self.now_buy_data = nil
end

--------------FourSeasonBoxItem--------------
FourSeasonBoxItem = FourSeasonBoxItem or DeclareMono("FourSeasonBoxItem", UIWidgetBaseItem)
function FourSeasonBoxItem:FourSeasonBoxItem()
    self.Data = FourSeasonData.Instance
    self.box_style_num = self.Data:BoxStyleNum()
    self:InitStyle()
end

function FourSeasonBoxItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local box_max_show = self.Data:BoxMaxShow()
    local index1, index2 = math.floor(data.my_index / box_max_show) + 1, data.my_index % box_max_show + 1
    self.ObjRoot.name = "FourSeasonBoxItem" .. index2

    UH.SpriteName(self.ImgBg, "TuoHe" .. data.gift_icon)
    UH.SpriteName(self.ImgDec, "ZhuangShi" .. data.gift_icon)
    UH.SpriteName(self.ImgDesc, "BiaoQian" .. data.gift_icon)
    UH.SetText(self.TxtTitle, data.gift_des)
    UH.SetText(self.TxtDesc, data.gift_name)
    UH.SetText(self.TxtPrice, data.buy_money / 10)
    local can_times, total_times = self.Data:BoxLimitTimes(data)
    UH.SetText(self.TxtLimit, Format(Language.FourSeasonBox.LimitType[data.limit_type], can_times < 0 and 0 or can_times, total_times))
    self.ObjLight:SetActive(data.gift_icon == 2)
    local reward, max_num = self.Data:DealItem(data)
    local is_show2 = max_num == 3
    self.List:SetData(is_show2 and {} or reward)
    local list1_data, list2_data = {}, {}
    if is_show2 then
        for i, v in ipairs(reward) do
            if i == 1 then
                table.insert(list1_data, v)
            else
                table.insert(list2_data, v)
            end
        end
    end
    self.List1:SetData(list1_data)
    self.List2:SetData(list2_data)
    self.IHandler.Interactable = can_times > 0
    self.ObjBtnEffect:SetActive(can_times > 0)
    for i = 1, self.ObjEffects:Length() do
        self.ObjEffects[i]:SetActive(i == data.gift_icon)
    end
end

function FourSeasonBoxItem:InitStyle()
    local style = self.Data:BoxStyle()
    if style == 3 then
        UH.SpriteName(self.ImgLight, "GuangShu" .. style)
    else
        UH.SpriteName(self.ImgLight, "GuangShu")
    end
end