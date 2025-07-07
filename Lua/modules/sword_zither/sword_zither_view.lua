
SwordZitherView = SwordZitherView or DeclareView("SwordZitherView", SwordZitherConfig.ResPath .. "sword_zither", Mod.RandActivity.SwordZither)
VIEW_DECLARE_LEVEL(SwordZitherView, ViewLevel.L1)
function SwordZitherView:LoadCallback()
end

function SwordZitherView:OnClickClose()
    ViewMgr:CloseView(SwordZitherView)
end

SwordZitherViewOffset = SwordZitherViewOffset or DeclareMono("SwordZitherViewOffset", UIWFlushPanel)

function SwordZitherViewOffset:SwordZitherViewOffset()
    self.data = SwordZitherCtrl.Instance:Data()
    self.act_data = ActivityData.Instance
    self.language = Language.SwordZither

    self.data_cares = {
        {data = self.data.sz_info, func = self.FlushItems},
        {data = self.data.cur_sel_type, func = self.FlushItems},
        {data = self.act_data.activity_status_event, func = self.FlushState},
    }

    -- 0全开，1只显示高级，2只显示初级
    local open_type = self.data:GetCurOpenType()
    self.TypesNode:SetActive(open_type == 0)
    self.data.cur_sel_type.val = open_type ~= 1 and 0 or 1
    if open_type ~= 0 then
        self.ExtraBonus[open_type]:SetActive(true)
    end
end

function SwordZitherViewOffset:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushRemainTime()
end

function SwordZitherViewOffset:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_ct)
end

function SwordZitherViewOffset:FlushState()
    if self.act_data.activity_status_event.act_type ~= ACTIVITY_TYPE.RAND_SWORD_ZITHER then return end
    local activity_status = self.act_data:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_SWORD_ZITHER)
    local status = activity_status and activity_status.status or ActStatusType.Close
    if ActStatusType.Open == status then
        self:FlushRemainTime()
    else
        ViewMgr:CloseView(SwordZitherDetailView)
        ViewMgr:CloseView(SwordZitherView)
    end
end

function SwordZitherViewOffset:FlushRemainTime()
    TimeHelper:CancelTimer(self.timer_ct)
    local activity_status = self.act_data:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_SWORD_ZITHER)
    local end_time = activity_status and activity_status.param_2 or 0
    if end_time - TimeHelper.GetServerTime() <= 0 then
        self.RemainTime.text = string.format(self.language.RemainTime, 0, 0, 0)
        return
    end
    self.timer_ct = TimeHelper:AddCountDownCT(function ()
        local last_time = end_time - TimeHelper.GetServerTime()
        local ft = TimeHelper.FormatDHMS(last_time)
        self.RemainTime.text = string.format(self.language.RemainTime, ft.day, ft.hour, ft.min)
    end, function ()
        self.RemainTime.text = string.format(self.language.RemainTime, 0, 0, 0)
    end, end_time, 60)
end

function SwordZitherViewOffset:FlushItems()
    local sword_zither_list = self.data:GetSwordZitherList()
    for i = 1, self.InfoItems:Length() do
        self.InfoItems[i]:SetData(sword_zither_list[i])
    end
end

function SwordZitherViewOffset:OnClickTips()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[60].desc)
end

function SwordZitherViewOffset:OnClickType(value)
    self.data.cur_sel_type.val = value
end

function SwordZitherViewOffset:OnClickDetail(data, item_pos, is_left)
    if nil == data then return end
    local info = data[self.data.cur_sel_type.val]
    self.detail_info = info
    self.FightInter.Interactable = data.is_open
    self.MonsterName.text = info.boss_name
    self.MonsterFeature.text = info.attr
    self.MonsterStrategy.text = info.strategy
    self.DetailTarget:SetTarget(item_pos.pos)
    self.DetailTarget.PositionOffset = Vector3.New(is_left and -2.5 or 2.5, 0, 0)

    self.DetailObj:SetActive(true)
end

function SwordZitherViewOffset:OnClickFight()
    if 1 == self.data.cur_sel_type.val and not TeamData.Instance:InTeam() then
        PublicPopupCtrl.Instance:DialogTips{
            content = Language.PublicPopup.ResTask,
            cancel = {
                name = Language.SwordZither.Confirm.Challenge,
                func = function ()
                    ServerActivityCtrl.Instance:SendRandActivityOperaReq({
                        rand_activity_type = ACTIVITY_TYPE.RAND_SWORD_ZITHER,
                        opera_type = 1,
                        param_1 = self.detail_info.monster_group_id,
                    })
                    ViewMgr:CloseView(DialogTipsView)
                end
            },
            confirm = {
                name = Language.SwordZither.Confirm.Team,
                func = function()
                    TeamCtrl.Instance:TeamDetectByTargetId(self.data.sword_zither_target_id, function() end, 1)
                    ViewMgr:CloseView(DialogTipsView)
                end
            }
        }
    else
        ServerActivityCtrl.Instance:SendRandActivityOperaReq({
            rand_activity_type = ACTIVITY_TYPE.RAND_SWORD_ZITHER,
            opera_type = 1,
            param_1 = self.detail_info.monster_group_id,
        })
    end
end

function SwordZitherViewOffset:OnClickDetailClose()
    self.DetailObj:SetActive(false)
end


SwordZitherViewItem = DeclareMono("SwordZitherViewItem", UIWidgetBaseItem)

function SwordZitherViewItem:SwordZitherViewItem()
    self.sz_data = SwordZitherCtrl.Instance:Data()
    self.language = Language.SwordZither

    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self,
                show_shadow = false
            }
        )
    end
end

function SwordZitherViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local type = self.sz_data.cur_sel_type.val
    local info = data[type]

    local is_open = data.is_open
    -- UH.SetIcon(self.BossIcon, info.boss_icon, ICON_TYPE.ITEM)
    -- local day_desc = data.day_desc .. RichTextHelper.ColorStr("")
    local day_desc = data.day_desc
    if is_open then
        day_desc = day_desc .. RichTextHelper.ColorStr(self.language.Opened, "7ef218")
    else
        day_desc = day_desc .. RichTextHelper.ColorStr(self.language.NotOpen, COLORSTR.Red8)
    end
    UH.SetText(self.DayDesc, day_desc)
    self.RewardList:SetData(info.rewards)
    self.PassObj:SetActive(data.pass[type])
    self.ItemInter.Interactable = is_open
    self.BossBg.SpriteName = SwordZitherConfig.BgSp[type]

    self.res_id = info.res_id
    if self.model_timer ~= nil then
        TimeHelper:CancelTimer(self.model_timer)
    end
    self.model_timer = TimeHelper:AddDelayTimer(BindTool.Bind(self.DelayShowModel, self), math.floor(data.day / 3) * 0.1)
end

function SwordZitherViewItem:GetItemPos()
    return {pos = self.ItemPos}
end

function SwordZitherViewItem:OnDestroy()
    if self.model_timer ~= nil then
        TimeHelper:CancelTimer(self.model_timer)
        self.model_timer = nil
    end
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end
end

function SwordZitherViewItem:DelayShowModel()
    if self.res_id == nil or self.ui_obj == nil then
        return
    end
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(self.res_id))
end

