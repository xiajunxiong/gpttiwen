GuildTrialNew = GuildTrialNew or DeclareView("GuildTrialNew", "guild/guild_trial_new",Mod.GuildTrialNew.Main)
VIEW_DECLARE_MASK(GuildTrialNew, ViewMask.Block)
VIEW_DECLARE_LEVEL(GuildTrialNew, ViewLevel.L0)
function GuildTrialNew:GuildTrialNew()
	self.AudioOpen = CommonAudio.ViewOpenL1
    self.AudioClose = CommonAudio.ViewCloseL1
end

function GuildTrialNew:LoadCallback()
	GuildCtrl.SendGuildBossReq(GUILD_BOSS_REQ_TYPE.RANK)
	ActFormationCtrl.Instance:SendGetFormationInfo(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_GUILD_BOSS)
end

function GuildTrialNew:OnBackClick()
	ViewMgr:CloseView(GuildTrialNew)
end

GuildTrialNewPanel = GuildTrialNewPanel or DeclareMono("GuildTrialNewPanel", UIWFlushPanel)
function GuildTrialNewPanel:GuildTrialNewPanel()
	self.is_reward_get = false
	self.boss_info = {}
	self.data = GuildData.Instance
	self.login_data = LoginData.Instance:BaseData()
	self.data_cares = {
		{data = self.data.guild_boss_info, func = self.FlushBossInfo},
		{data = self.data:BossRankList(), func = self.FlushRankInfo}
    }
end 

function GuildTrialNewPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function GuildTrialNewPanel:FlushRankInfo()
    local list = self.data:BossRankList()
    self.NoneObj:SetActive(table.nums(list) == 0)
    self.RankList:SetData(list)
    local out_rank, my_rank_data = self.data:GetMyRankData()
	for i = 1, 3 do
		self.NumArray[i]:SetActive(false)
	end
    if out_rank then
        self.RankObj:SetActive(false)
		self.NoRankObj:SetActive(true)
        UH.SetText(self.LblRank, Language.Guild.TrialNoRank)
    else
        if my_rank_data.index and my_rank_data.index > 0 and my_rank_data.index <=3 then
			self.RankObj:SetActive(false)
            self.NumArray[my_rank_data.index]:SetActive(true)
        else
            self.RankObj:SetActive(true)
            UH.SetText(self.LblRank, my_rank_data.index)
        end
		self.NoRankObj:SetActive(false)
		local reward_data_list = GuildData.Instance:GetTrialRankRewardList(my_rank_data.index)
		local reward_item_list = {}
		for k,v in pairs(reward_data_list) do
			table.insert(reward_item_list,Item.Init(v.item_id, v.num, v.is_bind))
		end
       self.RewardList:SetData(reward_item_list)
    end
    UH.SetText(self.LblAttack, my_rank_data.total_damage)
    UH.SetText(self.LblName, my_rank_data.role_name)
	UH.SetAvatar(self.IconSp, RoleData.Instance:GetRoleAvatarType(),RoleData.Instance:GetRoleAvatarId(),RoleData.Instance:GetRoleAvatarQuality())

end

function GuildTrialNewPanel:OnClickFormation()
    ActFormationCtrl.Instance:OpenView(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_GUILD_BOSS)
end

function GuildTrialNewPanel:OnClickBox()
	self.JoinShow:SetActive(true)
end

function GuildTrialNewPanel:OnShowBlock()
	self.JoinShow:SetActive(false)
end

-- 提示
function GuildTrialNewPanel:OnClickTips()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[84].desc)
end

function GuildTrialNewPanel:OnClickRankTip()
    ViewMgr:OpenView(GuildTrialRankRewardPreview)
end


function GuildTrialNewPanel:OnClickGo()
	if TombRaiderData.Instance:InTombRaider() then 
		PublicPopupCtrl.Instance:Center(Language.TombRaider.FamilyCancel)  
	    return 
	end 

	self:OnClickBoss()
end

function GuildTrialNewPanel:OnClickBoss()
	local team_list = TeamCtrl.Instance:Data():GetMemberList()
	--�ж��Ƿ����
	if TeamCtrl.Instance:Data():InTeam() then
		local param_t = {
		content = Language.Guild.TrialInTeam,
		confirm = {
					func = function()
						ViewMgr:CloseView(DialogTipsView)
						TeamCtrl.Instance:SendExitTeam()
						self:EnterFight()
					end
				  },
		cancel = {
					func = function()
						ViewMgr:CloseView(DialogTipsView)
						ViewMgr:CloseView(GuildTrialNew)
					end
				}
		}
        BehaviorData.Instance.auto_behavior = 0
		PublicPopupCtrl.Instance:DialogTips(param_t)	
	else
		self:EnterFight()
	end	
end

function GuildTrialNewPanel:EnterFight()
	if self.login_data.is_on_crosssever  then
		LoginCtrl.Instance:SendBackToOriginServer()
		return
	end
	self:OnClickFormation()
	--self.data:EnterTrialFight()
end


function GuildTrialNewPanel:FlushBossInfo()
	local info = self.data:GuildBossInfo()
	self.boss_info = info
	self:FreshBossInfo()
end 

function GuildTrialNewPanel:FreshBossInfo()
	local boss_config = self.data:GetGuildTrialNewBossConfig(self.boss_info.guild_boss_chapter_id)
	local cfg = Cfg.MonsterGroupById(boss_config.boss_id)
	local boss_name_cfg = ""
	if cfg then
		boss_name_cfg = cfg.name
	end
	local boss_name = boss_name_cfg --string.format(Language.Guild.GuildTrialNewBossName[self.select_boss_seq][1],boss_name_cfg)
	UH.SetText(self.BossName,boss_name)
	self.is_reward_get = false
	self.ButtonInter.Interactable = true
	self.ModelInter.Interactable = true

	local get_flag = self.boss_info.boss_participate_reward_fetch_flag
	if get_flag == 1 then
		self.is_reward_get = true
	end	
	self.IsGetFlag:SetActive(self.is_reward_get)
	local world_level = RoleData.Instance:GetWorldLevel()
	UH.SetText(self.WorldLevel,string.format(Language.Guild.GuildTrialWorldLevel,world_level))
	self:FreshBossModel(boss_config.boss_id)
	self:FreshJoinReward()
end

function GuildTrialNewPanel:FreshJoinReward()
	local join_rewards = self.data:GetGuildTrialNewJoinReward(self.boss_info.guild_boss_chapter_id)
	self.JoinReward:SetData(join_rewards)
end

function GuildTrialNewPanel:FreshBossModel(boss_id)
	local id = boss_id or 6777
	if not self.ui_obj_current then
        self.ui_obj_current = UIObjDrawer.New()
        self.ModelShowCurrent:SetShowData(
            {
                ui_obj = self.ui_obj_current,
                view = self,
                show_shadow = false
            }
        )
	end
	self.ui_obj_current:SetData(DrawerHelper.GetNpcPath(ResHelper.MonsterResByGroup(id)))
end

GuildTrialNewRewardListItem = GuildTrialNewRewardListItem or DeclareMono("GuildTrialNewRewardListItem", UIWidgetBaseItem)
function GuildTrialNewRewardListItem:GuildTrialNewRewardListItem()

end

function GuildTrialNewRewardListItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local item_data = Item.Create({item_id =data.item_id, num = data.num,is_bind =data.is_bind})
	self.Cell:SetData(item_data)
	self.RedPoint:SetNum(0)
	if data.is_have_red and data.index == BigDipperData.Instance.first_reward_count then
		self.RedPoint:SetNum(1)
	end
	self.HasGet:SetActive(data.is_get)
	self.Cell:MakeGrey(data.is_get)
end



