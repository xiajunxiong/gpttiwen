YZWishView = YZWishView or DeclareView("YZWishView", "serveractivity/yun_ze/yz_wish", Mod.YunZe.YzWish)
VIEW_DECLARE_LEVEL(YZWishView, ViewLevel.L1)
VIEW_DECLARE_MASK(YZWishView, ViewMask.BgBlock)
function YZWishView:YZWishView()
    UH.SetText(self.TxtDate, self:GetActTimeDesc())
end

function YZWishView:GetActTimeDesc()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_YZ_WISH) then
        return Language.Activity.NotOpenTip
    end
    local act_info = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_YZ_WISH)
    local open_time = act_info.param_1
    local end_time = act_info.param_2 - 1
    local str1 = os.date(Language.YZWish.Date, open_time)
    local str2 = os.date(Language.YZWish.Date, end_time)
    return Format(Language.YZWish.DateTip, str1, str2)
end

function YZWishView:OnCloseClick()
    ViewMgr:CloseView(YZWishView)
end

----------------------------YZWishPanel----------------------------
YZWishPanel = YZWishPanel or DeclareMono("YZWishPanel", UIWFlushPanel)
function YZWishPanel:YZWishPanel()
    self.Data = YunZeData.Instance
    self.reward_config = self.Data:GetYZWishRewardConfig()
    self.other = Config.yunzeqiyuan_auto.other[1]
    self.language = Language.YZWish
    self.data = self.Data.yz_wish_data
    self.data_cares = {
        {data = self.data, func = self.FlushShow},
    }
end

function YZWishPanel:Awake()
    UIWFlushPanel.Awake(self)
    UH.SetText(self.TxtStr, self.other.desc)
end

function YZWishPanel:FlushShow()
    self.ObjNext:SetActive(self.data.is_can_lottery == 0)
    self.Remind:SetNum(self.data.is_can_lottery)
    for i = 1, self.List:Length() do
        self.List[i]:SetData(self.reward_config[i])
    end
    self.NextTime:CloseCountDownTime()
    if self.data.next_refresh == 0 then
        self.NextTime:SetTime(self.language.End)
    else
        self.NextTime:StampTime(self.data.next_refresh)
    end
end

function YZWishPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function YZWishPanel:OnWishClick()
    if self.Data:IsYZWishCanLottery() then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_YZ_WISH, 3)
    else
        PublicPopupCtrl.Instance:Center(Format(self.language.NotEnoughtLock, self.other.reward_choice))
    end
end

function YZWishPanel:OnNextClick()
    if self.data.next_refresh == 0 then
        PublicPopupCtrl.Instance:Center(self.language.EndTip)
    else
        if self.Data:GetYZWishLockedNum() == self.other.reward_choice then
            PublicPopupCtrl.Instance:Center(self.language.NextTip)
        else
            PublicPopupCtrl.Instance:Center(Format(self.language.NextTip2, self.other.reward_choice))
        end
    end
end

----------------------------YZWishItem----------------------------
YZWishItem = YZWishItem or DeclareMono("YZWishItem", UIWidgetBaseItem)
function YZWishItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(Item.Create(data.reward_item))
    self.ObjLocked:SetActive(YunZeData.Instance:GetYZWishLock(self.data.seq) > 0)
end

function YZWishItem:OnLockClick()
    if YunZeData.Instance:GetYZWishLock(self.data.seq) == 0 then
        if YunZeData.Instance:YZWishIsCanLocked() then
            self:OpenDialog()
        else
            PublicPopupCtrl.Instance:Center(Language.YZWish.CannotLocked)
        end
    end
end

function YZWishItem:OpenDialog()
    local dialog_info = {
        content = Language.YZWish.Question,
        confirm = {
            func = function()
                ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_YZ_WISH, 1, self.data.seq)
                ViewMgr:CloseView(DialogTipsView)
            end
        },
        cancel = {
            name = Language.YZWish.Think,
        },
        new_tnr = DTNewTodayNotRemind.YZWish,
    }
    PublicPopupCtrl.Instance:DialogTips(dialog_info)
end