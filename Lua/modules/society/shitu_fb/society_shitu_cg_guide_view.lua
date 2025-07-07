
SocietyShiTuCGGuideView = SocietyShiTuCGGuideView or DeclareView("SocietyShiTuCGGuideView", SocietyConfig.ResPath .. "society_shitu_cg_guide")

function SocietyShiTuCGGuideView:SocietyShiTuCGGuideView()
    self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society
end

function SocietyShiTuCGGuideView:LoadCallback()
    self.Board:SetData(self:GetType(), self.language.SocietyShiTuCG, Vector2.New(877, 521))
end

SocietyShiTuCGGuideViewF = SocietyShiTuCGGuideViewF or DeclareMono("SocietyShiTuCGGuideViewF", UIWFlushPanel)

function SocietyShiTuCGGuideViewF:SocietyShiTuCGGuideViewF()
    self.data = SocietyCtrl.Instance:Data()
    self.language = Language.Society.ShiTuCG
end

function SocietyShiTuCGGuideViewF:Awake()
    UIWFlushPanel.Awake(self)

    local seek_master = SocietyConfig.ShiTuSeekingType.seek_master == SocietyViewMasterApprenticeGroupList.seek_type
    local info = self.data.shitu_info_sm
    local shitu_chuan_gong_reward_list = self.data.shitu_chuan_gong_reward_list
    local shitu_chuan_gong_score_max = self.data.shitu_chuan_gong_score_max

    self.GuideTips.text = self.language.GuideTips
    self.ExpShow:SetActive(seek_master)
    if seek_master then
        local item_data = Item.Init(CommonItemId.Exp)
        item_data.is_grey = 0 == info.week_chuan_gong_exp_flag
        self.ExpCell:SetData(item_data)
    end

    self.ProgressShow:SetProgress(info.week_chuan_gong_score_max / shitu_chuan_gong_score_max)
    for i = 1, self.ShowItems:Length() do
        self.ShowItems[i]:SetData(shitu_chuan_gong_reward_list[i], shitu_chuan_gong_score_max, seek_master, info.week_chuan_gong_score_max)
    end
    -- self.GuideTips.text = self.language.GuideTips
    -- self.RewardList:SetData(self.data:GetShiTuFBRewardShow())
end

function SocietyShiTuCGGuideViewF:OnClickInvite()
    -- SocietyCtrl.Instance:SendShiTuReqInvite()
    -- ViewMgr:OpenView(TeamInviteShiTuView)
    ViewMgr:OpenView(TeamInviteShiTuView)
end

function SocietyShiTuCGGuideViewF:OnClickFight()
    -- SceneCtrl.Instance:RequestEnterFb(FB_TYPE.SHITU_FB)
    -- ViewMgr:CloseView(SocietyShiTuFBGuideView)
    SceneLogic.Instance:AutoToNpc(self.data:GetShiTuFBNpcSeq(), nil, nil, true)
    ViewMgr:CloseView(SocietyShiTuCGGuideView)
    ViewMgr:CloseView(SocietyView)
end

SocietyShiTuCGGuideViewItem = DeclareMono("SocietyShiTuCGGuideViewItem", UIWidgetBaseItem)

function SocietyShiTuCGGuideViewItem:SocietyShiTuCGGuideViewItem()
	self.society_data = SocietyData.Instance
end

function SocietyShiTuCGGuideViewItem:SetData(data, score_max, seek_master, score_record)
	UIWidgetBaseItem.SetData(self, data)
    if nil == data then return end
    local item_list = seek_master and data.apprentice_reward_show or data.master_reward_show
    if item_list and item_list[1] then
        self.item_show = item_list[1]
        UH.SetIcon(self.IconSp, Item.GetIconId(self.item_show.item_id))
    end
    self.NumShow.text = data.score
    self.ItemInter.Interactable = score_record >= data.score 
    self.ItemPos.localPosition = Vector2.New(-311.5 + 623 * (data.score / score_max), 0)
    self.ItemPos:SetObjActive(true)
end

function SocietyShiTuCGGuideViewItem:OnClickItem()
    if self.item_show then
        local info = {
            item_list = {self.item_show},
            pos = Vector3.New(0, 140, 0)
        }
        ItemInfoCtrl.Instance:ItemInfoView(info)
    end
end