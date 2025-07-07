-- ============== 历程 =================
AchieveCoursePanel = AchieveCoursePanel or DeclareMono("AchieveCoursePanel", UIWFlushPanel)

AchieveCoursePanel.IconStageCfg = {
    [0] = {
        on = "TuBiaoXuanZhong-LiChengZongLan",
        off = "TuBiao-LiChengZongLan",
        big = "",
    },
    [1] = {
        on = "TuBiaoXuanZhong-ChengChangDianDi",
        off = "TuBiao-ChengChangDianDi",
        big = "TuBiaoDa-ChengChangDianDi",
    },
    [2] = {
        on = "TuBiaoXuanZhong-WoDeJiaZu",
        off = "TuBiao-WoDeJiaZu",
        big = "TuBiaoDa-WoDeJiaZu",
    },
    [3] = {
        on = "TuBiaoXuanZhong-RiChangHuoDong",
        off = "TuBiao-RiChangHuoDong",
        big = "TuBiaoDa--RiChangHuoDongKaoBei2",
    },
    [4] = {
        on = "TuBiaoXuanZhong-JiQingHongChen",
        off = "TuBiao-JiQingHongChen",
        big = "TuBiaoDa-JiQingHongChen",
    },
    [5] = {
        on = "TuBiaoXuanZhong-GuangHuiShiJi",
        off = "TuBiao-GuangHuiShiJi",
        big = "TuBiaoDa-GuangHuiShiJiKaoBei",
    },
    [6] = {
        on = "TuBiaoXuanZhong-DaQianShiJie",
        off = "TuBiao-DaQianShiJie",
        big = "TuBiaoDa-DaQianShiJieKaoBei",
    },
}

function AchieveCoursePanel:AchieveCoursePanel()
    self.data = AchieveNewData.Instance
end

function AchieveCoursePanel:Awake()
    UIWFlushPanel.Awake(self)
    --此界面刘海屏特殊处理
	if IS_IPHONEX or EditorSettings:SimularIphoneX() then
		self.ContentRect.localScale = Vector3.New(0.91,0.91,1)
	end
    self:FlushPanel()
end

function AchieveCoursePanel:FlushPanel()
    self:FlushBtnList()
end

function AchieveCoursePanel:FlushBtnList()
    local list_data = self.data:GetCourseBtnList()
    self.BtnList:SetData(list_data)
end

function AchieveCoursePanel:OnParentClick(data)
    local jump_data = data.children[1]
    if jump_data then
        self.BtnList:ClickChild(jump_data)
    end
end

function AchieveCoursePanel:OnChildClick(data)
    if data.stage > 0 then
        self.ShowTypeMono1:SetObjActive(false)
        self.ShowTypeMono2:SetObjActive(true)
        self.ShowTypeMono2:SetData(data)
    else
        self.ShowTypeMono1:SetObjActive(true)
        self.ShowTypeMono2:SetObjActive(false)
    end
end

function AchieveCoursePanel:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[177].desc)
end


AchieveCourseParentItem = AchieveCourseParentItem or DeclareMono("AchieveCourseParentItem", UIWidgetBaseItem)

function AchieveCourseParentItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.name)
    local icon_cfg = AchieveCoursePanel.IconStageCfg[data.id]
    if icon_cfg then
        UH.SpriteName(self.FlagOn, icon_cfg.on)
        UH.SpriteName(self.FlagOff, icon_cfg.off)
    end
end


AchieveCourseChildItem = AchieveCourseChildItem or DeclareMono("AchieveCourseChildItem",UIWidgetBaseItem)

function AchieveCourseChildItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    UH.SetText(self.Name,data.star_name)
end

function AchieveCourseChildItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end


-- ========== 历程总览 ShowTypeMono1 ==========
AchieveCourseShowAll = AchieveCourseShowAll or DeclareMono("AchieveCourseShowAll", UIWFlushPanel)
function AchieveCourseShowAll:AchieveCourseShowAll()
    self.ctrl_data = AchieveNewData.Instance
    self.data_cares = {
        {data = self.ctrl_data.course_base_data, func = self.FlushList, keys = {"progress_list"}, init_call = false},
        {data = self.ctrl_data.course_base_data, func = self.FlushRemind, keys = {"level","fraction","level_reward_flag"}, init_call = false},
        {data = self.ctrl_data.course_base_data, func = self.FlushInfo, keys = {"level","fraction"}, init_call = false},
    }
end

function AchieveCourseShowAll:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function AchieveCourseShowAll:FlushPanel()
    self:FlushList()
    self:FlushRemind()
    self:FlushInfo()
end

function AchieveCourseShowAll:FlushRemind()
    local remind_num = self.ctrl_data:CourseRemind()
    self.Remind:SetNum(remind_num)
end

function AchieveCourseShowAll:FlushList()
    local list_data = {}
    for i,v in pairs(self.ctrl_data.cfg.course_group) do
        if not list_data[v.stage] then
            list_data[v.stage] = v
        end
    end
    table.sort(list_data, function (a,b)
        return a.sn < b.sn
    end)
    self.List:SetData(list_data)
end

function AchieveCourseShowAll:FlushInfo()
    local level = self.ctrl_data.course_base_data.level
    UH.SetText(self.Level, level)
    local next_cfg = self.ctrl_data.cfg.course_reward[level + 1]
    if next_cfg then
        local progress_num = self.ctrl_data.course_base_data.fraction
        local progress_max_num = next_cfg.need_fraction
        self.Progress:SetProgress(progress_num/progress_max_num)
        progress_num = ColorStr(tostring(progress_num), COLORSTR.Yellow16)
        self.Progress:SetText(string.format("%s/%s",progress_num, progress_max_num))
    else
        self.Progress:SetProgress(1)
        self.Progress:SetText(Language.AchieveCourse.MaxLevel)
    end
end

function AchieveCourseShowAll:OnRewardPreviewClick()
    ViewMgr:OpenView(AchieveCourseRewardPreview)
end

AchieveCourseShowAllItem = AchieveCourseShowAllItem or DeclareMono("AchieveCourseShowAllItem", UIWidgetBaseItem)
-- data : Config.accomplishment_auto.course
function AchieveCourseShowAllItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.stage_name)
    local icon_cfg = AchieveCoursePanel.IconStageCfg[data.stage]
    if icon_cfg then
        UH.SpriteName(self.Icon, icon_cfg.big)
    end
   
    local progress_num, progress_max_num = AchieveNewData.Instance:GetCourseBigGroupProgress(data.stage)
    self.Progress:SetProgress(progress_num/progress_max_num)
    progress_num = ColorStr(tostring(progress_num), COLORSTR.Red14)
    self.Progress:SetText(string.format("%s/%s",progress_num, progress_max_num))
end

-- ========== 历程信息 ShowTypeMono2 =========
AchieveCourseShow = AchieveCourseShow or DeclareMono("AchieveCourseShow", UIWFlushPanel)
function AchieveCourseShow:AchieveCourseShow()
    self.ctrl_data = AchieveNewData.Instance
    self.data_cares = {
        {data = self.ctrl_data.course_base_data, func = self.FlushPanel, keys = {"progress_list","finish_flag_list"}, init_call = false},
    }
end

function AchieveCourseShow:SetData(data)
    self.data = data
    self:FlushPanel()
end

function AchieveCourseShow:FlushPanel()
    if not self.data then
        return
    end
    UH.SetText(self.Title, self.data.star_name)
    local list_data = self.ctrl_data.course_child_group[self.data.sn] or {}
    list_data = TableCopy(list_data)
    table.sort(list_data, function (a, b)
        local a_flag = self.ctrl_data:GetCourseFinishFlag(a.sn)
        local b_flag = self.ctrl_data:GetCourseFinishFlag(b.sn)
        if a_flag ~= b_flag and (a_flag > 0 or b_flag > 0) then
            return a_flag < b_flag
        end
        return a.sn < b.sn
    end)
    self.List:SetData(list_data)

    local progress_num, progress_max_num = AchieveNewData.Instance:GetCourseGroupProgress(self.data.sn)
    self.Progress:SetProgress(progress_num/progress_max_num)
    progress_num = ColorStr(tostring(progress_num), COLORSTR.Yellow16)
    self.Progress:SetText(string.format(Language.AchieveCourse.Txt1,progress_num, progress_max_num))
end

AchieveCourseShowItem = AchieveCourseShowItem or DeclareMono("AchieveCourseShowItem", UIWidgetBaseItem)
-- data : Config.accomplishment_auto.course
function AchieveCourseShowItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.name)
    UH.SetText(self.Desc, data.desc)
    UH.SetText(self.PriceNum, data.course_reward)

    local role_count = AchieveNewData.Instance:GetCourseRoleCount(data.sn)
    local server_role_count = AchieveNewData.Instance:GetCourseBaseData().server_role_count
    local scale_num = math.floor((role_count / server_role_count * 100))
    UH.SetText(self.Rank, string.format(Language.AchieveCourse.Txt2, scale_num))

    UH.SpriteName(self.Icon, string.format("JieDuan%s", data.icon))

    local progress_num = AchieveNewData.Instance:GetCourseProgress(data.sn)
    if data.cond_type == 21 then    -- 人物等级完成条件
        progress_num = RoleData.Instance:GetRoleLevel()
    end

    local progress_max_num = data.achv_cond_param
    progress_num = progress_num > progress_max_num and progress_max_num or progress_num

    local finish_flag = AchieveNewData.Instance:GetCourseFinishFlag(data.sn)
    if finish_flag == 1 then
        progress_num = progress_max_num
        UH.SpriteName(self.Flag, "LvYinZhang")
        UH.SetText(self.FlagText, Language.AchieveCourse.Flag1)
    else
        UH.SpriteName(self.Flag, "HuiYinZhang")
        UH.SetText(self.FlagText, Language.AchieveCourse.Flag2)
    end
    self.Progress:SetProgress(progress_num/progress_max_num)
    progress_num = ColorStr(tostring(progress_num), COLORSTR.White)
    self.Progress:SetText(string.format("%s/%s",progress_num, progress_max_num))
end

-- 历程奖励预览
AchieveCourseRewardPreview = AchieveCourseRewardPreview or DeclareView("AchieveCourseRewardPreview", "achieve/achieve_course_reward_preview")
function AchieveCourseRewardPreview:OnCloseClick()
    ViewMgr:CloseView(AchieveCourseRewardPreview)
end

AchieveCourseRewardPreviewPanel = AchieveCourseRewardPreviewPanel or DeclareMono("AchieveCourseRewardPreviewPanel", UIWFlushPanel)
function AchieveCourseRewardPreviewPanel:AchieveCourseRewardPreviewPanel()
    self.data = AchieveNewData.Instance
    self.data_cares = {
        {data = self.data.course_base_data, func = self.FlushPanel, keys = {"level", "level_reward_flag"}, init_call = false},
    }
end

function AchieveCourseRewardPreviewPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function AchieveCourseRewardPreviewPanel:FlushPanel()
    local list_data,index = self:GetListData()
    self.List:SetData(list_data)
    if index > 1 and index < 17 then
        local height = (index - 1) * 138 - 50
        if height > 0 then
            self.List:SetCompleteCallBack(function ()
                self.List:ForceJumpVertical(height)
            end)
        end
    end
end

function AchieveCourseRewardPreviewPanel:GetListData()
    if self.list_data then
        return self.list_data , 0
    end
    self.list_data = {}
    local index = 0         -- 为完成的下标
    local can_get_index = 0 -- 可领取的下标
    for i,v in ipairs(self.data.cfg.course_reward) do
        if v.need_fraction > 0 then
            table.insert(self.list_data, v)
            if index == 0 then
                if self.data:GetCourseLevelRewardFlag(v.course_level) == 1 and can_get_index == 0 then
                    can_get_index = #self.list_data
                end
                if self.data:GetCourseLevelRewardFlag(v.course_level) == 0 and index == 0 then
                    index = #self.list_data
                end
            end
        end
    end
    local jump_index = can_get_index > 0 and can_get_index or index
    return self.list_data,jump_index
end

AchieveCourseRewardPreviewItem = AchieveCourseRewardPreviewItem or DeclareMono("AchieveCourseRewardPreviewItem", UIWidgetBaseItem)
-- data : Config.accomplishment_auto.course_reward
function AchieveCourseRewardPreviewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Level, string.format(Language.AchieveCourse.Level, data.course_level))
    local flag = AchieveNewData.Instance:GetCourseLevelRewardFlag(data.course_level)
    self.FlagBtn:SetActive(flag == 1)
    self.Flag0:SetActive(flag == 0)
    self.Flag1:SetActive(flag < 0)
    self.Remind:SetNum(flag == 1 and 1 or 0)
    local reward_item_list = {}
    if data.exp > 0 then
        local item = Item.Create({item_id = CommonItemId.Exp, num = data.exp})
        table.insert(reward_item_list, item)
    end
    if data.coin > 0 then
        local item = Item.Create({item_id = CommonItemId.Coin, num = data.coin})
        table.insert(reward_item_list, item)
    end
    if not TableIsEmpty(data.reward_item) then
        local item_list = DataHelper.FormatItemList(data.reward_item)
        reward_item_list = AndList(reward_item_list, item_list)
    end
    self.RewardList:SetData(reward_item_list)
end

function AchieveCourseRewardPreviewItem:OnGetClick()
    local level = self.data.course_level
    AchieveCtrl.Instance:RequestOperator(AchieveCtrl.OP_TYPE.COURSE_REWARD, level)
end


-- 历程达成提醒
AchieveCourseFinishRemind = AchieveCourseFinishRemind or DeclareView("AchieveCourseFinishRemind", "achieve/achieve_course_finish_remind")
VIEW_DECLARE_MASK(AchieveCourseFinishRemind,ViewMask.None)
-- param_t : {name,num}
function AchieveCourseFinishRemind:LoadCallback(param_t)
    param_t = param_t or {}
    local name = param_t.name or ""
    local num = param_t.num or 0
    name = ColorStr(name, COLORSTR.Yellow25)
    UH.SetText(self.Desc, string.format(Language.AchieveCourse.Txt3, name))
    UH.SetText(self.Num, num)
    self.time_ht = Invoke(function ()
        ViewMgr:CloseView(AchieveCourseFinishRemind)
    end,4)
end

function AchieveCourseFinishRemind:CloseCallback()
    TimeHelper:CancelTimer(self.time_ht)
end