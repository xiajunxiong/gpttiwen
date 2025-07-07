AreasGodAnimalsPanel = AreasGodAnimalsPanel or DeclareMono("AreasGodAnimalsPanel", UIWFlushPanel)
function AreasGodAnimalsPanel:AreasGodAnimalsPanel()
    self.data = GodAreasActsData.Instance
    -- self.hp_keys = {
    --     "hp_80", "hp_60", "hp_40", "hp_20", "hp_0"
    -- }
    self.data_cares = {
        {data = self.data:GetBossInfo(), func = self.onFlush},
        {data = self.data.animals_rank, func = self.onFlush}
    }
end

function AreasGodAnimalsPanel:FlushModel(res_id)
    if self.ui_obj == nil then
		self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj,
                view = self,
                show_shadow = false,
			}
		)
    end
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(res_id))
end
function AreasGodAnimalsPanel:onFlush()
    local faction_id = self.data:FactionId()
    -- LogDG("当前阵营id = ", faction_id)
    self.data:GetBossInfo().seq = faction_id
    local boss_base = self.data:GetBossBase()
    local boss_info  = self.data:GetBossInfo()

    UH.SetText(self.MyHurt, boss_info.total_hurt or 0)
    self:FlushModel(boss_base.res_id)
    if GodAreasActsData.Instance:AnimalsIsOpen() == ActStatusType.Open then
        UH.SetText(self.Hp, (boss_info.hp / 100) .."%")
        self.XueTiao:SetProgress(1 - (boss_info.hp / 10000))
    else
        UH.SetText(self.Hp, "100%")
        self.XueTiao:SetProgress(1)
    end
    

    local box_list = self.data:GetBoxList()
    self.BoxList:SetData(box_list)
    UH.SetText(self.Name, boss_base.mythical_Animals)
    local color = true and COLORSTR.Green3 or COLORSTR.Red20
    local total_times = self.data:ChallengeTimes()
    UH.SetText(self.Count, string.format(Language.GodAreasActs.ChallengeCount, color, boss_info.fight_times, total_times))

    self.RedPoint:SetNum((boss_info.fight_times < total_times and boss_info.hp > 0) and 1 or 0)

    local preview = self.data:BossPreviewReward()
    self.RewardList:SetData(preview)

    local rank_data = self.data.animals_rank
    local rank_list = {}
    local temp
    local role_id = RoleData.Instance:GetRoleId()
    local my_rank = -1
    for i = 1, 10, 1 do
        temp = {}
        if rank_data[i] then
            temp = rank_data[i]
            if rank_data[i].role_id == role_id then
                my_rank = rank_data[i].rank_value
            end
        else
            temp.role_name = Language.GodAreasActs.NoName
            temp.total_hurt = "--"
        end
        temp.rank_value = i
        rank_list[i] = temp
    end
    UH.SetText(self.MyRank, my_rank == -1 and Language.GodAreasActs.NoRank or my_rank)
    self.RankList:SetData(rank_list)
    local buff_id = self.data:ChallengeBuff()
    local buff_cfg = Cfg.MedalEffectById(buff_id)--Config.buff_list_auto.buff_list[buff_id]
    if buff_cfg then
        UH.SetIcon(self.BuffIcon, buff_cfg.icon_id)
    end

    local open_time = self.data:AnimalsOpenTime()
    local end_time = self.data:AnimalsEndTime()
    UH.SetText(self.Time, string.format(Language.GodAreasActs.AnimalsTime, open_time.month, open_time.day, self:AddTime(open_time.hour), self:AddTime(open_time.min),
    end_time.month, end_time.day, self:AddTime(end_time.hour), self:AddTime(end_time.min)))
    self.BuffGet:SetActive((boss_info.fight_times >= total_times or boss_info.hp == 0) and GodAreasActsData.Instance:AnimalsIsOpen() == ActStatusType.Open)
end
function AreasGodAnimalsPanel:AddTime(time)
    return time < 10 and tostring("0"..time) or time
end
function AreasGodAnimalsPanel:OnClickTest()
    local temp = self.data:GetBossInfo().seq
    temp = temp + 1
    if temp >= 4 then
        temp = 0
    end
    self.data:GetBossInfo().seq = temp
end
function AreasGodAnimalsPanel:OnClickBuff()
    local buff_id = self.data:ChallengeBuff()
    ViewMgr:OpenView(BuffInfoTip, {buff_id = buff_id,from_view = "battle"})
end
function AreasGodAnimalsPanel:OnClickFight()
    --判断次数
    if GodAreasActsData.Instance:AnimalsIsOpen() == ActStatusType.Open then
        local boss_info  = self.data:GetBossInfo()
        local total_times = self.data:ChallengeTimes()
        if boss_info.fight_times >= total_times then
            PublicPopupCtrl.Instance:Center(Language.GodAreasActs.CantChallenge)
            return
        end
        if boss_info.hp <= 0 then
            PublicPopupCtrl.Instance:Center(Language.GodAreasActs.AnimalsFightOut)
            return
        end
        GodAreasActsCtrl.Instance:CSZhenYingShenShouInfo(GodAreasActsData.Animals_Req_Type.Fight)
        ViewMgr:CloseView(GodAreasActs)
        ViewMgr:CloseView(LandsBetweenMainView)
    else
        PublicPopupCtrl.Instance:Center(Language.GodAreasActs.ActTip)
    end
end

function AreasGodAnimalsPanel:OnClickBuZhen()
    ActFormationCtrl.Instance:OpenView(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_GOD_SPACE, {hide_challenge = true})
end

function AreasGodAnimalsPanel:OnClickHelp()
	PublicPopupCtrl.Instance:HelpTip(372)
end

function AreasGodAnimalsPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
	self.ui_obj = nil
end

AreasGodAnimalsBoxItem = AreasGodAnimalsBoxItem or DeclareMono("AreasGodAnimalsBoxItem", UIWidgetBaseItem)
function AreasGodAnimalsBoxItem:AreasGodAnimalsBoxItem()
    self.act_data = GodAreasActsData.Instance
end
function AreasGodAnimalsBoxItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local is_get = self.act_data:BoxIsGet(self.data.seq)
    self.Open:SetActive(is_get)
    self.Close:SetActive(not is_get)
    self.OutTime:SetActive(false)
    self.GetObj:SetActive(is_get)
    local boss_info  = self.act_data:GetBossInfo()
    UH.SetText(self.Value, data.surplus_hp.."%")
    if not is_get and (boss_info.hp / 100) <= data.surplus_hp then
        if self.handle_effect == nil then
            self.handle_effect = self.EffectTool:Play(3165226, self.Effect)
        end
        self.RedPoint:SetNum(1)
    else
        self.RedPoint:SetNum(0)
        if self.handle_effect then
            self.EffectTool:Stop(self.handle_effect)
            self.handle_effect = nil
        end
    end
end
function AreasGodAnimalsBoxItem:OnClickReward()
    -- LogDG("打开展示宝箱奖励或者领取奖励")
    local is_get = self.act_data:BoxIsGet(self.data.seq)
    if not is_get and GodAreasActsData.Instance:AnimalsIsOpen() == ActStatusType.Open then
        GodAreasActsCtrl.Instance:CSZhenYingShenShouInfo(GodAreasActsData.Animals_Req_Type.Fetch, self.data.seq)
    end
    local tab = {
        pos = UH.GetRootPosition(self.gameObject),
        item_list = DataHelper.FormatItemList(self.data.reward),
        pivot = 0
    }
    ViewMgr:OpenView(ShowItemView, tab)
    
end

AreasGodAnimalsRewardItem = AreasGodAnimalsRewardItem or DeclareMono("AreasGodAnimalsRewardItem", UIWidgetBaseItem)
function AreasGodAnimalsRewardItem:AreasGodAnimalsRewardItem()
    
end
function AreasGodAnimalsRewardItem:SetData(data)
    local item = Item.Create(data)
    UH.SetText(self.Name, item:QuaName())
    self.Cell:SetData(item)
end


AreasGodAnimalsRankItem = AreasGodAnimalsRankItem or DeclareMono("AreasGodAnimalsRankItem", UIWidgetBaseItem)
function AreasGodAnimalsRankItem:AreasGodAnimalsRankItem()
    self.rank_sp = {"DiYiMing", "DiErMing", "DiSanMing"}
end
function AreasGodAnimalsRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Head:SetData({avatar_id = data.headshot_id, avatar_type = data.avatar_type})
    if data.rank_value >= 4 then
        UH.SpriteName(self.RankSp, "Di4MingYiHou")
        UH.SetText(self.RankNum, data.rank_value)
    else
        UH.SetText(self.RankNum, "")
        UH.SpriteName(self.RankSp, self.rank_sp[data.rank_value])
    end
    UH.SetText(self.Name, data.role_name)
    UH.SetText(self.ShangHaiZhi, data.total_hurt)
end

--阵营神兽 神域界面替身
AreasGodAnimalsMainSub = AreasGodAnimalsMainSub or DeclareMono("AreasGodAnimalsMainSub", UIWFlushPanel)
function AreasGodAnimalsMainSub:AreasGodAnimalsMainSub()
    self.data = LandsBetweenData.Instance
    self.sp_img = {
        [0] = "_LocShuiZhiChengShenShou","_LocHuoZhiChengShenShou","_LocFengZhiChengShenShou", "_LocDiZhiChengShenShou"
    }
end
function AreasGodAnimalsMainSub:SetLinkMark(link_mark)
    local param = self.data:GetResOfSpPicByLinkMark(link_mark) or {}
    UH.SetText(self.TimeDesc, GodAreasActsData.Instance:AnimalsIsOpen() == ActStatusType.Open and  Language.GodAreasActs.OpenDesc or
    Language.GodAreasActs.CloseDesc)

    UH.SpriteName(self.TitleSp, self.sp_img[param.faction])
end