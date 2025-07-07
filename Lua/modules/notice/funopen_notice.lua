FunopenNoticeConfig = {
    [Mod.FunOpenNotice.Arena] = 1,
    [Mod.FunOpenNotice.CourageChallenge] = 2,
    [Mod.FunOpenNotice.FieldBoss] = 3,
    [Mod.FunOpenNotice.RushDungeon] = 4,
    [Mod.FunOpenNotice.ChallengeTest] = 5,
    [Mod.FunOpenNotice.Ring] = 6,
    [Mod.FunOpenNotice.Modrons] = 7,
    [Mod.FunOpenNotice.Battlefield] = 8,
    [Mod.FunOpenNotice.CloudArena] = 9,
    [Mod.FunOpenNotice.Bicycle] = 10,
    [Mod.FunOpenNotice.PetIsland] = 11,
    [Mod.FunOpenNotice.DeepDarkFantasy] = 12,
    [Mod.FunOpenNotice.PetWeapon] = 13,
    [Mod.FunOpenNotice.PetStamp] = 14,
    [Mod.FunOpenNotice.RoleHeartSkill] = 15,
    [Mod.FunOpenNotice.PartnerGift] = 16,
    [Mod.FunOpenNotice.Accrssory] = 17,
    [Mod.FunOpenNotice.PetPray] = 18,
    [Mod.FunOpenNotice.GoThrough] = 19,
    [Mod.FunOpenNotice.PartnerContract] = 20,
    [Mod.FunOpenNotice.UnLimitTower] = 21,
    [Mod.FunOpenNotice.MirageTerritory] = 22,
    [Mod.FunOpenNotice.FlyUp] = 23,
    [Mod.FunOpenNotice.PetRefining] = 24,
    [Mod.FunOpenNotice.BlazeStar] = 25,
    [Mod.FunOpenNotice.Smelt] = 26,
    [Mod.FunOpenNotice.GhostBust] = 27,
    [Mod.FunOpenNotice.HarmonyHeaven] = 28,
    [Mod.FunOpenNotice.StarMap] = 29,
    [Mod.FunOpenNotice.HuanShou] = 30,
    [Mod.FunOpenNotice.TeamChallenge] = 31,
    [Mod.FunOpenNotice.YouMing] = 32,
}

FunOpenNoticeView = FunOpenNoticeView or DeclareView("FunOpenNoticeView", "notice/funopen_notice")
VIEW_DECLARE_LEVEL(FunOpenNoticeView, ViewLevel.L1)
function FunOpenNoticeView:LoadCallback(data)
    self.panel:SetData(data)
end

FunOpenNoticePanel = FunOpenNoticePanel or DeclareMono("FunOpenNoticePanel", UIWFlushPanel)
function FunOpenNoticePanel:FunOpenNoticePanel()
    self.config = Config.function_notice_auto.notice
    self.data_cares = {
        {data = GuideData.Instance.notice_info, func = self.FlushPage,init_call = false},
        {data = GuideData.Instance.notice_flush_info, func = self.FlushPage,init_call = false},
    }
end

function FunOpenNoticePanel:SetData(data)
    self.config_list = {}
    for k,v in pairs(self.config) do
        if (v.res_id == nil or v.res_id == 0) and v.not_use == 0 then 
            table.insert(self.config_list, v)
        end
    end 
    
    table.sort(self.config_list,function (a,b)
        return a.end_level < b.end_level
    end)
    for i = 1,self.page_num do 
        self.page_list[i].gameObject:SetActive(self.config_list[i] ~= nil)
        if self.config_list[i] then 
            self.page_list[i]:SetData(self.config_list[i])
        end
    end 
    for k,v in pairs(self.config_list) do 
        if data.mod == v.modkey then
            self:FlushPage(k)
        end 
    end 
    if data.mod == nil then
        self:FlushPage()
    end
end

function FunOpenNoticePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function FunOpenNoticePanel:OnClickClose()
    ViewMgr:CloseView(FunOpenNoticeView)
end

function FunOpenNoticePanel:FlushPage(index)
    local mark_index = index ~= nil and (index) or GuideData.Instance:GetCurNoticeIndex()

    if mark_index <=6 then 
        self.content.localPosition = Vector2.zero
    else 
        self.content.localPosition = Vector2.New(-(mark_index-2)*213,0)
    end 
    if self.page_list[mark_index] == nil then 
        self:OnClickClose()
        return 
    end 
    self.page_list[mark_index]:Click()
    self:OnClickPage(mark_index)
end

function FunOpenNoticePanel:OnClickPage(page_index)
    local select_config = self.config_list[page_index]
    if select_config == nil then return end 
    for i = 1,self.page_num do 
        self.fun_list[i].gameObject:SetActive(FunopenNoticeConfig[select_config.modkey] == i)
    end 

    self.fun_list[FunopenNoticeConfig[select_config.modkey]]:onFlush()

    for i = 1,self.page_num do 
        if self.config_list[i] ~= nil then 
            self.page_list[i]:FlushInteract()
        end 
    end 
end

----------------------------NoticePageCell----------------------------
NoticePageCell = NoticePageCell or DeclareMono("NoticePageCell", UIWidgetBaseItem)
function NoticePageCell:SetData(data)
    UH.SetText(self.funname,data.name)
    UH.SetText(self.funname_s,data.name)
    UH.SetText(self.level,string.format(Language.Common.LevelLimit,data.end_level))
    UH.SetText(self.level_s,string.format(Language.Common.LevelLimit,data.end_level))

    UH.SetIcon(self.func_icon,data.icon,ICON_TYPE.ACT)

    UIWidgetBaseItem.SetData(self, data)
    self.inter.Interactable = GuideData.Instance:NoticeInfo(self.data.notice_id) < FunPreNoticeEnum.Rewarded
        or GuideData.Instance:NoticeGuideDone(self.data.notice_id) 
end 

function NoticePageCell:Click()
    if self.ItemClick then 
        self.ItemClick:Trigger()
    end 
end

function NoticePageCell:FlushInteract()
    self.inter.Interactable = GuideData.Instance:NoticeInfo(self.data.notice_id) < FunPreNoticeEnum.Rewarded 
        or GuideData.Instance:NoticeGuideDone(self.data.notice_id) 
end

-------------------------------------- Panel da !---------------------------------------------------

CourageNoticePanel = CourageNoticePanel or DeclareMono("CourageNoticePanel", UIWFlushPanel)
function CourageNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.CourageChallenge)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
        or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function CourageNoticePanel:OnClickClose()
--     ViewMgr:CloseView(CloudArenaNotice)
-- end
function CourageNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
		SceneLogic.Instance:AutoToNpcs(CourageChallengeData.NPC)
        ViewMgr:CloseView(FunOpenNoticeView)
    end
    GuideData.Instance:CheckNext(self.config.sn)
end

-- 规则：任务完成后显示开启奖励
function CourageNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

BattlefieldNoticePanel = BattlefieldNoticePanel or DeclareMono("BattlefieldNoticePanel", UIWFlushPanel)
function BattlefieldNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.Battlefield)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function BattlefieldNoticePanel:OnClickClose()
--     ViewMgr:CloseView(CloudArenaNotice)
-- end
function BattlefieldNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.Battlefield.Main)  
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function BattlefieldNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

BicycleNoticePanel = BicycleNoticePanel or DeclareMono("BicycleNoticePanel", UIWFlushPanel)
function BicycleNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.Bicycle)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function BicycleNoticePanel:OnClickClose()
--     ViewMgr:CloseView(CloudArenaNotice)
-- end
function BicycleNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.Bicycle.Main)  
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function BicycleNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

PetIslandNoticePanel = PetIslandNoticePanel or DeclareMono("PetIslandNoticePanel", UIWFlushPanel)
function PetIslandNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.PetIsland)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function BicycleNoticePanel:OnClickClose()
--     ViewMgr:CloseView(CloudArenaNotice)
-- end
function PetIslandNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.FightFB.PetIsland)  
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function PetIslandNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

DeepDarkFantasyNoticePanel = DeepDarkFantasyNoticePanel or DeclareMono("DeepDarkFantasyNoticePanel", UIWFlushPanel)
function DeepDarkFantasyNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.DeepDarkFantasy)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function DeepDarkFantasyNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function DeepDarkFantasyNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        local end_func = function ()
            ViewMgr:OpenViewByKey(Mod.DeepDarkFantasy.Main)  
            ViewMgr:CloseView(FunOpenNoticeView)
		end

		if TeamData.Instance:InTeam() then
			PublicPopupCtrl.Instance:DialogTips{
				content = Language.ChallengeTest.InTeamTips,
				confirm = {
					func = function ()
						TeamCtrl.Instance:SendExitTeam()
						ViewMgr:CloseView(DialogTipsView)
						Call(end_func)
					end
				}
			}
		else
			Call(end_func)
		end
    end
end

-- 规则：任务完成后显示开启奖励
function DeepDarkFantasyNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

PetWeaponNoticePanel = PetWeaponNoticePanel or DeclareMono("PetWeaponNoticePanel", UIWFlushPanel)
function PetWeaponNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.PetWeapon)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)

    if not self.ui_obj then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self
            }
        )
        self.ui_obj:SetData(DrawerHelper.GetWeaponPathGray(29))
    end
    self:FlushGuideUnit() 
end

function PetWeaponNoticePanel:OnDestroy()
    if self.ui_obj then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end
    UIWFlushPanelGroup.OnDestroy(self)
end
-- function DeepDarkFantasyNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function PetWeaponNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.PetWeapon.Main)  
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function PetWeaponNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

PetStampNoticePanel = PetStampNoticePanel or DeclareMono("PetStampNoticePanel", UIWFlushPanel)
function PetStampNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.PetStamp)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function PetStampNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function PetStampNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.Pet.PetStamp)  
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function PetStampNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

RoleHeartSkillNoticePanel = RoleHeartSkillNoticePanel or DeclareMono("RoleHeartSkillNoticePanel", UIWFlushPanel)
function RoleHeartSkillNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.RoleHeartSkill)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function PetStampNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function RoleHeartSkillNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.Role.RoleHeartSkill)  
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function RoleHeartSkillNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

GivePetNoticeView = GivePetNoticeView or DeclareView("GivePetNoticeView", "notice/give_pet_notice", Mod.FunOpenNotice.GivePetNotice)
VIEW_DECLARE_MASK(GivePetNoticeView,ViewMask.BgBlockClose)
GivePetNoticeView.is_xing_tian_guide = false
function GivePetNoticeView:GivePetNoticeView()
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self,
                show_shadow = true,
                shadow_dir = ModelUIShow.ShadowCameraDir.Pet
            }
        )
    end

    GuideManager.Instance:RegisterGetGuideUi("GiveNoticeCloseBtn",function ()
        return self.CloseBtn,function ()
            ViewMgr:CloseView(GivePetNoticeView)
            ServerActivityData.Instance.login_reward_close:Notify()
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("GiveNoticePanel",function ()
        return self.GuideFrameObj
    end)
    self.handle_care =  FunOpen.Instance.show_notice_red_point:Care(BindTool.Bind(self.FlushRedPoint, self))
end
function GivePetNoticeView:FlushRedPoint()
    self.RedPoint:SetNum(FunOpen.Instance.show_notice_red_point.show and 1 or 0)
end

function GivePetNoticeView:LoadCallback(param_t)
    self.cfg = FunOpen.Instance.cur_open_func_notice_data
    self:FlushPanel()
    self:FlushRedPoint()
end

function GivePetNoticeView:CloseCallback()
    FunOpen.Instance.show_notice_red_point:Uncare(self.handle_care)
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
    TimeHelper:CancelTimer(self.time_handle)
end
function GivePetNoticeView:OnClickNotice()
    ViewMgr:OpenView(FunOpenNoticeView)
end
function GivePetNoticeView:FlushPanel()
    local task_cfg = TaskData.Instance:GetTaskConfig(self.cfg.task)
    local notice_id = self.cfg.notice_id

    UH.SetText(self.Name, string.format(Language.GivePetNotice.Name[self.cfg.notice_id], self.cfg.name))
    local res_path = DrawerHelper.GetNpcPath(self.cfg.res_id)
    if self.cfg.notice_id == FunOpen.ConstNotice.LingLu then
        res_path = DrawerHelper.GetRidePath(self.cfg.res_id)
    end
    self.ui_obj:SetData(res_path)
    if self.cfg.res_id == 226 then
        if GivePetNoticeView.is_xing_tian_guide then
            self.ui_obj:PlayAnim(ChAnimType.Show)
            GivePetNoticeView.is_xing_tian_guide = false
        else
            self.ui_obj:PlayAnim(ChAnimType.Show3)
        end
    end
    if self.cfg.notice_id == FunOpen.ConstNotice.MaoYao then
        UH.SetText(self.PetName, self.cfg.name)
        if task_cfg then
            UH.SetText(self.TaskName, task_cfg.task_name)
        else
            UH.SetText(self.TaskName, "")
        end
        UH.SpriteName(self.TipImg, Language.GivePetNotice.ImgSp[self.cfg.notice_id])
        UH.SetText(self.Tip1, Language.GivePetNotice.MainNoticeTip2[self.cfg.notice_id])-- string.format(Language.GivePetNotice.TaskTip, task_cfg.min_level))
        UH.SetText(self.Tip2, Language.GivePetNotice.MainNoticeTip3[self.cfg.notice_id])-- string.format(Language.GivePetNotice.TaskTip, task_cfg.min_level))
    end

    local skill_list = {}
    local skill_id_list = Split(self.cfg.pet_skill,"|")
    if self.cfg.notice_id ~= FunOpen.ConstNotice.LingLu then
        for i,v in pairs(skill_id_list) do
            local skill_id = tonumber(v)
            local skill_cfg = PetData.Instance:GetGiftDetail(skill_id)
            table.insert(skill_list, skill_cfg)
        end
        self.SkillList:SetData(skill_list)
    else
        for i,v in pairs(skill_id_list) do
            local skill_id = tonumber(v)
            local skill_cfg = BicycleSkill.New({skill_id = skill_id, skill_type = BicycleCfg.SKILL_TYPE.BASE, level = 1})
            self.SkillItems[i]:SetData(skill_cfg)
        end
    end

    self.ShowTip1:SetActive(notice_id == FunOpen.ConstNotice.MaoYao)
    self.ShowTip2:SetActive(notice_id == FunOpen.ConstNotice.XingTian)
    self.ShowTip3:SetActive(notice_id == FunOpen.ConstNotice.LingLu)
end

function GivePetNoticeView:OnCloseClick()
    -- if self.cfg.notice_id == 24 then
    --     local icon_mono = ServerActivityData.Instance:GetLoginRewardActIconMono()
    --     if icon_mono then
    --         icon_mono:SetEffectState(false)
    --         self:PlayCloseAnimation(icon_mono.gameObject.transform)
    --         return
    --     end
    -- end
    ViewMgr:CloseView(GivePetNoticeView)
end

function GivePetNoticeView:PlayCloseAnimation(icon_trans)
	self.play_closeing = true
	self.gameObject.transform:SetParent(icon_trans)
    TimeHelper:CancelTimer(self.time_handle)
    self.close_anima_speed = 12
	self.panel_scale = 1
	self.panel_scale_lerp = self.panel_scale / self.close_anima_speed
	self.view_pos = self.gameObject:GetLocalPosition()
	self.view_pos_x_lerp = (self.view_pos.x + 30) / self.close_anima_speed
	self.view_pos_y_lerp = self.view_pos.y / self.close_anima_speed
	self.canvas_group_alpha = 1 / self.close_anima_speed
	self.time_handle = TimeHelper:AddRunFrameTimer(BindTool.Bind(self.CloseAnimation,self),1,self.close_anima_speed)
end

function GivePetNoticeView:CloseAnimation()
	self.panel_scale = self.panel_scale - self.panel_scale_lerp
	self.panel_scale = self.panel_scale <= 0.1 and 0.1 or self.panel_scale
	UH.LocalScale(self.PanelTrans,Vector3.New(self.panel_scale,self.panel_scale,self.panel_scale))
	self.view_pos.x = self.view_pos.x - self.view_pos_x_lerp
	self.view_pos.y = self.view_pos.y - self.view_pos_y_lerp
	self.gameObject:SetLocalPosition(self.view_pos)
	self.CanvasGroup.alpha = self.CanvasGroup.alpha - self.canvas_group_alpha

	self.close_anima_speed = self.close_anima_speed - 1
	if self.close_anima_speed <= 0 then
		ViewMgr:CloseView(GivePetNoticeView)
		local icon_mono = ServerActivityData.Instance:GetLoginRewardActIconMono()
		if icon_mono then
			icon_mono:SetEffectState(true)
		end
	end
end



GivePetNoticeSkillItem = GivePetNoticeSkillItem or DeclareMono("GivePetNoticeSkillItem", UIWidgetBaseItem)
function GivePetNoticeSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetIcon(self.Icon,data.icon_id,ICON_TYPE.SKILL)
    UH.SetText(self.Name,"")
    UH.SetText(self.SkillType, data.corner_icon)
end

function GivePetNoticeSkillItem:OnSkillClick()
    PetData.Instance:SetClickSkill({skill_cfg = self.data,hide_mp = true})
    ViewMgr:OpenView(PetSkillTip)
end

PartnerGiftNoticePanel = PartnerGiftNoticePanel or DeclareMono("PartnerGiftNoticePanel", UIWFlushPanel)
function PartnerGiftNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.PartnerGift)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function PetStampNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function PartnerGiftNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.Partner.PartnerGift)  
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function PartnerGiftNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

AccrssoryNoticePanel = AccrssoryNoticePanel or DeclareMono("AccrssoryNoticePanel", UIWFlushPanel)
function AccrssoryNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.Accrssory)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function PetStampNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function AccrssoryNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.Accrssory.Main)  
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function AccrssoryNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

PetPrayNoticePanel = PetPrayNoticePanel or DeclareMono("PetPrayNoticePanel", UIWFlushPanel)
function PetPrayNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.PetPray)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function PetStampNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function PetPrayNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.Pet.PetAttr)  
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function PetPrayNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

GoThroughNoticePanel = GoThroughNoticePanel or DeclareMono("GoThroughNoticePanel", UIWFlushPanel)
function GoThroughNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.GoThrough)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function PetStampNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function GoThroughNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.Challenge.GoThrough)  
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function GoThroughNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

PartnerContractNoticePanel = PartnerContractNoticePanel or DeclareMono("PartnerContractNoticePanel", UIWFlushPanel)
function PartnerContractNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.PartnerContract)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function PetStampNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function PartnerContractNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.Partner.PartnerContract)  
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function PartnerContractNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

UnlimitTowerNoticePanel = UnlimitTowerNoticePanel or DeclareMono("UnlimitTowerNoticePanel", UIWFlushPanel)
function UnlimitTowerNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.UnLimitTower)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function PetStampNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function UnlimitTowerNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        local end_func = function ()
            ViewMgr:OpenViewByKey(Mod.Experience.EndlessTower)  
            ViewMgr:CloseView(FunOpenNoticeView)
		end

		if TeamData.Instance:InTeam() then
			PublicPopupCtrl.Instance:DialogTips{
				content = Language.ChallengeTest.InTeamTips,
				confirm = {
					func = function ()
						TeamCtrl.Instance:SendExitTeam()
						ViewMgr:CloseView(DialogTipsView)
						Call(end_func)
					end
				}
			}
		else
			Call(end_func)
		end
    end
end

-- 规则：任务完成后显示开启奖励
function UnlimitTowerNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

MirageTerritoryNoticePanel = MirageTerritoryNoticePanel or DeclareMono("MirageTerritoryNoticePanel", UIWFlushPanel)
function MirageTerritoryNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.MirageTerritory)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function PetStampNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function MirageTerritoryNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.Experience.MirageTerritory)  
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function MirageTerritoryNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

FlyUpNoticePanel = FlyUpNoticePanel or DeclareMono("FlyUpNoticePanel", UIWFlushPanel)
function FlyUpNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.FlyUp)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function PetStampNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function FlyUpNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.FlyUp.Main)  
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function FlyUpNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

PetRefiningNoticePanel = PetRefiningNoticePanel or DeclareMono("PetRefiningNoticePanel", UIWFlushPanel)
function PetRefiningNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.PetRefining)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function PetStampNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function PetRefiningNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.PetRefining.Main)  
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function PetRefiningNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

BlazeStarNoticePanel = BlazeStarNoticePanel or DeclareMono("BlazeStarNoticePanel", UIWFlushPanel)
function BlazeStarNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.BlazeStar)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function PetStampNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function BlazeStarNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.Workshop.BlazeStar)  
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function BlazeStarNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

SmeltNoticePanel = SmeltNoticePanel or DeclareMono("SmeltNoticePanel", UIWFlushPanel)
function SmeltNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.Smelt)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function PetStampNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function SmeltNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.Equip.Smelt)  
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function SmeltNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

GhostBustNoticePanel = GhostBustNoticePanel or DeclareMono("GhostBustNoticePanel", UIWFlushPanel)
function GhostBustNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.GhostBust)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function PetStampNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function GhostBustNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.Pet.PetAttr)
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function GhostBustNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

HarmonyHeavenNoticePanel = HarmonyHeavenNoticePanel or DeclareMono("HarmonyHeavenNoticePanel", UIWFlushPanel)
function HarmonyHeavenNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.HarmonyHeaven)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function MarbasNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function HarmonyHeavenNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.HarmonyHeaven.Main)
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function HarmonyHeavenNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

StarMapNoticePanel = StarMapNoticePanel or DeclareMono("StarMapNoticePanel", UIWFlushPanel)
function StarMapNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.StarMap)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function StarMapNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function StarMapNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.StarMap.Main)
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function StarMapNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

HuanShouNoticePanel = HuanShouNoticePanel or DeclareMono("HuanShouNoticePanel", UIWFlushPanel)
function HuanShouNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.HuanShou)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function HuanShouNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function HuanShouNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.HuanShou.Main)
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function HuanShouNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

TeamChallengeNoticePanel = TeamChallengeNoticePanel or DeclareMono("TeamChallengeNoticePanel", UIWFlushPanel)
function TeamChallengeNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.TeamChallenge)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function HuanShouNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function TeamChallengeNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.Experience.Task)
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function TeamChallengeNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end

YouMingNoticePanel = YouMingNoticePanel or DeclareMono("YouMingNoticePanel", UIWFlushPanel)
function YouMingNoticePanel:onFlush()    
    self.config = GuideData.Instance:GetFunctionNoticeData(Mod.FunOpenNotice.YouMing)
    self.List:SetData(DataHelper.FormatItemList(self.config.reward_item))
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    -- self.BtnInier.Interactable = RoleData.Instance:GetRoleLevel() >= self.config.end_level and flag < 2
    self.Level.text = string.format( Language.Notice.GetLevel, self.config.end_level)
    UH.SetText(self.btntext,flag == FunPreNoticeEnum.Lock and string.format(Language.Notice.WaitForReward,self.config.end_level) 
    or (flag == FunPreNoticeEnum.UnLock and Language.Notice.CanForReward or Language.Notice.Goto) )
    UH.LocalScale(self.btnrect, Vector3.New(1,1,1))
    UH.SetText(self.level_num, self.config.end_level)
    self.had_got:SetActive(flag == FunPreNoticeEnum.Rewarded)
    self:FlushGuideUnit()
end
-- function HuanShouNoticePanel:OnClickClose()
--     ViewMgr:CloseView(DeepDarkFantasyNotice)
-- end
function YouMingNoticePanel:OnClickGet()
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id)
    if flag == FunPreNoticeEnum.Lock then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    elseif flag == FunPreNoticeEnum.UnLock then
        FunOpen.Instance:SendFuncPreviewClaimReward(self.config.notice_id)
        GuideData.Instance:CheckNext(self.config.sn)
    else 
        ViewMgr:OpenViewByKey(Mod.YouMing.Main)
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end

-- 规则：任务完成后显示开启奖励
function YouMingNoticePanel:FlushGuideUnit()
    local unit_param = GuideData.Instance:GetFunopenNoticeGuideUnitParam(self.config.notice_id)
    local flag = GuideData.Instance:NoticeInfo(self.config.notice_id) == FunPreNoticeEnum.Rewarded
    if unit_param.is_empty then 
        self.GuideUnit.gameObject:SetActive(false)
        return 
    end 

    self.GuideUnit.gameObject:SetActive(not unit_param.done and flag )
    self.ItemBg:SetActive(unit_param.done or not flag ) 
    self.List.gameObject:SetActive(unit_param.done or not flag )  
    self.btnrect.gameObject:SetActive(unit_param.done or not flag ) 
    if self.had_got.activeSelf and (not unit_param.done and flag ) then
        self.had_got:SetActive(false) 
    end
    self.GuideUnit:SetData(unit_param)
end