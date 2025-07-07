
ChallengeView = ChallengeView or DeclareView("ChallengeView","challenge/challenge_fuben",Mod.Challenge.Fuben)

VIEW_DECLARE_LEVEL(ChallengeView,ViewLevel.L1)

--Board=======UIWSubstitute[UIWBoard8]
--BossList====UIWidgetList[BossItem]
--FreshTimer==UIWTimeMeter
function ChallengeView:LoadCallback()
    self.Currency:DefaultSet()
    self.challenge_data = ChallengeData.Instance

    self.List:SetData(self.challenge_data:GetFubenList())
    self.List:ClickItem(1)
    self.List:SetectItem(1,true)
    self.Board:SetData(ChallengeView,UIWBoard8.TitleImg.TiaoZhanFuBen)
    self.FreshTimer:StampTime(self.challenge_data:GetFreshTS(),TimeType.Type_Special_4)
end

function ChallengeView:ClickFubenBtn()
    self.totalOj:SetActive(true)
    self.detailOj:SetActive(false)
end

function ChallengeView:TipClick()
    PublicPopupCtrl.Instance:OpenTipsPopup{tips_str = Language.Challenge.Tips}
end

function ChallengeView:TeamClick()
    -- local team_target_cfg = Cfg.TeamTypeTarget(TeamConfig.TeamTargetType.ChallengeFb,ChallengeData.Instance:GetCurSelFbid())
    -- TeamData.Instance.cur_platform_target_id = team_target_cfg.target_id
    -- ViewMgr:OpenView(TeamPlatformView)
end

function ChallengeView:OnChapterClick(chapter_id)
    self.BossList:ClickItem(chapter_id)
end

function ChallengeView:OnClickEnterFb()
    ViewMgr:CloseView(ChallengeView)
    local fb_id = ChallengeData.Instance:GetCurSelFbid()
    local passed_chap = ChallengeData.Instance:GetFbPassedChapter(fb_id)
    SceneCtrl.Instance:RequestEnterFb(FB_TYPE.CHALLENGE_FB,fb_id,passed_chap + 1)
end

ChallengeListItem = ChallengeListItem or DeclareMono("ChallengeListItem",UIWidgetBaseItem)
--name============Text
--cell============UIWSubstitute
--task_name=======Text
--Interactor======InteractorHandle
--FbIcon==========RawImage
--FubenClickEve===LuaUIEventClick
--LockObj=========GameObject
function ChallengeListItem:SetData(data)
    self.data = data
    self.name.text = data.stage_name
    local unlocked = self.data.open_level <= RoleData.Instance:GetRoleLevel()
    self.Interactor.Interactable = unlocked
    self.LockObj:SetActive(not unlocked)
    -- self.LockClicker.enabled = not unlocked
    UH.SetIconToRawImage(self.FbIcon, data.fuben_icon, ICON_TYPE.MAP)
end

function ChallengeListItem:Click()
    UIWidgetBaseItem:Click()

    if self.LockObj.activeSelf then --未解锁
        PublicPopupCtrl.Instance:DialogTips({
            content = string.format(Language.Challenge.UnlockShowDialog,self.data.open_level),
            confirm = {
                func = function () PublicPopupCtrl.Instance:CloseDialogTips() end,
            },
            no_cancel = true
            })         
    else
        ChallengeData.Instance:SetCurSelFbid(self.data.fuben_id)
        self.boss_list:SetData(ChallengeData.Instance:GetBossList(self.data.fuben_id))
        self.chapter_list:SetData(ChallengeData.GetFbStageReward(self.data.fuben_id))
        self.open_level2.text = self.data.open_level .. Language.Common.Level
        self:SetChapterArray(self.data.fuben_id)
        self.FubenClickEve:Trigger()
    end

end

function ChallengeListItem:SetChapterArray(fuben_id)
    local boss_list = ChallengeData.Instance:GetBossList(fuben_id)
    for _, v in pairs(boss_list)do
        self.chapter_array[v.chapter]:SetData(v)
    end
end


TaskItem = TaskItem or DeclareMono("TaskItem",UIWidgetBaseItem)

function TaskItem:SetData(data)
    local item = Item.Create({item_id = data.item_id})
    self.cell:SetData(item)
    --self.cell:SetDataByItemId(data.item_id)
end

ChallengeBossItem = ChallengeBossItem or DeclareMono("ChallengeBossItem",UIWidgetBaseItem)
--EveClick========LuaUIEventClick
--BossModel=======ModelUIShow
--BossRewardTran==RectTransform

function ChallengeBossItem:SetData(data)
    self.data = data
    self.name.text = data.boss_name
    -- LogError("ChallengeBossItem",data)
end

function ChallengeBossItem:BossItemClick()
    local rew = self.data.chapter_reward or {}
    local re_count = 0
    for _,_ in pairs(rew) do
        re_count = re_count + 1
    end
    if re_count > 3 then --三个以下居中
        self.BossRewardTran.pivot = Vector2.New(0.5,1)
    else
        self.BossRewardTran.pivot = Vector2.New(0.5,0.5)
    end
    self.stage_list:SetData(rew)
    self.boss_desc.text = self.data.boss_desc
    self.detailOj:SetActive(true)
    self.totalOj:SetActive(false)

    local res_path = DrawerHelper.GetNpcPath(ResHelper.MonsterResId(self.data.monster_id))

    local uo = self.BossModel:UiObj()
    if uo == nil then
        uo = UIObjDrawer.CreateByPath(res_path)
        self.BossModel:SetShowData(
            {
                ui_obj = uo,
                view = ChallengeView,
            }
        )
    else
        uo:SetData(res_path)
    end
end

function ChallengeBossItem:Click()
    self.EveClick:Trigger()
end

StageItem = StageItem or DeclareMono("StageItem",UIWidgetBaseItem)
function StageItem:SetData(data)
    -- Debuger.LogError("chapter..")
    -- LogError(data)
    self.cell:SetDataByItemId(data.item_id)
end

ChapterAwardItem = ChapterAwardItem or DeclareMono("ChapterAwardItem",UIWidgetBaseItem)
function ChapterAwardItem:SetData(data)
    self.cell:SetDataByItemId(data.item_id)
end

ChallengeFbChapterItem = ChallengeFbChapterItem or DeclareMono("ChallengeFbChapterItem",UIWidgetBaseItem)
--Interactor=====InteractorHandle
--KilledImg======GameObject
--name===========Text
--Seled==========GameObject
--HeadImg========RawImage

function ChallengeFbChapterItem:SetData(data)
    self.name.text = data.chapter_desc
    local monster_cfg = Cfg.MonsterById(data.monster_id)
    UH.SetIconToRawImage(self.HeadImg, monster_cfg.icon_id, ICON_TYPE.ITEM)
    local passed_chapter_id = ChallengeData.Instance:GetFbPassedChapter(data.fuben_id)
    local self_chapter_id = data.chapter
    -- LogError("passed_chapter_id====",passed_chapter_id,",self_chapter_id===",self_chapter_id,",fb_id===",data.fuben_id)
    if self_chapter_id <= passed_chapter_id then        --已经挑战完成
        self.KilledImg:SetActive(true)
        self.Interactor.Interactable = true
        self.Seled:SetActive(false)
    elseif self_chapter_id - passed_chapter_id == 1 then --当前可挑战
        self.KilledImg:SetActive(false)
        self.Interactor.Interactable = true
        self.Seled:SetActive(true)
    else         --不可挑战
        self.KilledImg:SetActive(false)
        self.Interactor.Interactable = false
        self.Seled:SetActive(false)
    end
end
