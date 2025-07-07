SkyTreasureHuntView = SkyTreasureHuntView or DeclareMono("SkyTreasureHuntView", UIWFlushPanel)

function SkyTreasureHuntView:SkyTreasureHuntView()
    self.Data = SkyTreasureHuntData.Instance
    self.language = Language.SkyTreasureHunt
    self.data = self.Data.info_data
    self.item_id = self.Data:ConsumeId()
    self.item_name = Item.GetName(self.item_id)
    -- 用于判断模型是否加载好
    self.is_mod_loading_finish = false
    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushItem},
        {data = self.data, func = self.StartRoller, init_call = false},
    }
end

function SkyTreasureHuntView:Awake()
    UIWFlushPanel.Awake(self)
    -- 模型
    if not self.dice_ui_obj then
        self.dice_ui_obj = UIObjDrawer.New()
        self.ModelUIShow:SetShowData({ui_obj = self.dice_ui_obj, view = self, show_shadow = false,})
    end
    -- 模型动画
    Runner.Instance:RemoveRunUntilTrue(self.anim_delay_handle)
    self.anim_delay_handle = Runner.Instance:RunUntilTrue(
        function()
            if self.dice_ui_obj.main_anim then
                self.is_mod_loading_finish = true
                return true
            end
            return false
        end,
    false)
    self.dice_ui_obj:SetData(DrawerHelper.GetNpcPath(1061))

    local config = Item.GetConfig(self.item_id)
    UH.SetIcon(self.ImgSetIcon1, config.icon_id, ICON_TYPE.ITEM)
    UH.SetIcon(self.ImgSetIcon2, config.icon_id, ICON_TYPE.ITEM)
    UH.SetText(self.TxtTip, Format(self.language.Tip, self.item_name))
    self:SetRollerData()
end

function SkyTreasureHuntView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Delete(self, "dice_ui_obj")
    Runner.Instance:RemoveRunUntilTrue(self.anim_delay_handle)
    TimeHelper:CancelTimer(self.delay_show)
end

function SkyTreasureHuntView:FlushRemind()
    self.Remind:SetNum(self.Data:Remind())
end

function SkyTreasureHuntView:FlushItem()
    local has_num = Item.GetNum(self.item_id)
    local need_num = self.Data:NowConsumeNum()
    local color = has_num >= need_num and COLORSTR.Green4 or COLORSTR.Red8
    UH.SetText(self.TxtHasNum, has_num)
    if self.data.is_free == 1 then
        UH.SetText(self.TxtNum, self.language.Free)
    else
        UH.SetText(self.TxtNum, Format(Language.Common.Cstr, color, has_num, need_num))
    end
    self:FlushRemind()
end

function SkyTreasureHuntView:OnAddClick()
    ViewMgr:OpenView(SkyTreasureHuntSelectView)
end

function SkyTreasureHuntView:SetRollerData()
    self.roller_reward_config = self.Data:GetRollerConfig()
    local roller_data = {}
    roller_data.config = {}
    for k, v in pairs(self.roller_reward_config) do
        roller_data.config[k] = v
    end
    roller_data.end_func = BindTool.Bind(self.RollerEnd, self)
    roller_data.now_select = self.data.pos
    self.Roller:SetData(roller_data)
end

function SkyTreasureHuntView:OnLotteryClick()
    -- 模型没好不给抽奖
    if not self.is_mod_loading_finish then
        return
    end
    local has_num = Item.GetNum(self.item_id)
    local need_num = self.Data:NowConsumeNum()
    if has_num >=  need_num then
        if BagData.Instance:GetEmptyNumInBag() == 0 then
            PublicPopupCtrl.Instance:Center(ErrorInfo[110])
            return
        end
        SkyTreasureHuntCtrl.Instance:SendReq()
        self.ObjBlock:SetActive(true)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.RollDice)
    else
        PublicPopupCtrl.Instance:Center(Format(self.language.NotEnoughtTip, self.item_name))
    end
end

function SkyTreasureHuntView:StartRoller()
    self:FlushItem()
    self:FlushRemind()
    self.ObjBlock:SetActive(true)
    -- 加个容错
    if self.is_mod_loading_finish then
        self.dice_ui_obj:PlayAnim(SkyTreasureDiceState[self.data.step])
        TimeHelper:CancelTimer(self.delay_show)
        self.delay_show = Invoke(function()
            self.Roller:PlayRoller(self.data.pos)
        end, 2)
    else
        self.Roller:PlayRoller(self.data.pos)
    end
end

function SkyTreasureHuntView:RollerEnd()
    self.ObjBlock:SetActive(false)
    MainOtherCtrl.Instance:OpenGiftView(DataHelper.FormatItemList({self.roller_reward_config[self.data.pos].reward}), GIFT_OPEN_TYPE.CONGRATULATIONS)
end

function SkyTreasureHuntView:OpenGetWay()
    ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = self.item_id})})
end


--------------SkyTreasureHuntSelectView--------------
SkyTreasureHuntSelectView = SkyTreasureHuntSelectView or DeclareView("SkyTreasureHuntSelectView", "period_activity_act/sky_treasure_hunt/sky_treasure_hunt_select")
VIEW_DECLARE_MASK(SkyTreasureHuntSelectView, ViewMask.BgBlock)
function SkyTreasureHuntSelectView:SkyTreasureHuntSelectView()
    self.Board:SetData(self:GetType(), Language.SkyTreasureHunt.GetWay, Vector2.New(581, 449))
    self.List:SetData(SkyTreasureHuntData.Instance:GetActivityConfig())
end

----------------------------SkyTreasureHuntItem----------------------------
SkyTreasureHuntItem = SkyTreasureHuntItem or DeclareMono("SkyTreasureHuntItem", UIWidgetBaseItem)
function SkyTreasureHuntItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, data.activity_name)
    UH.SetIcon(self.Icon, data.icon_id, ICON_TYPE.ACT)
    UH.SetText(self.TxtProgress, Format(Language.SkyTreasureHunt.Progress, data.total_num - data.progress, data.total_num))
    self.IHandler.Interactable = data.progress ~= 0
    UH.SetText(self.TxtBtn, Language.SkyTreasureHunt.BtnGoTo[data.progress ~= 0 and 1 or 2])
end

function SkyTreasureHuntItem:OnGoToClick()
    if IsEmpty(self.data.client_id) then
        return
    end
    if ActivityRandData.Instance:OnClickHandle(self.data.client_id) then
        ViewMgr:CloseView(PeriodActivityView)
        ViewMgr:CloseView(PeriodActivityTipView)
        ViewMgr:CloseView(SkyTreasureHuntSelectView)
    else
        ViewMgr:OpenViewByKey(self.data.client_id)
    end
end