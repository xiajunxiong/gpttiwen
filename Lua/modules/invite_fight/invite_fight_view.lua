InviteFightView = InviteFightView or DeclareView("InviteFightView", "invite_fight/invite_fight")

VIEW_DECLARE_LEVEL(InviteFightView,ViewLevel.L1)

function InviteFightView:LoadCallback()

end

function InviteFightView:CloseCallback()

end

function InviteFightView:OnClickClose()
    ViewMgr:CloseView(InviteFightView)
end

function InviteFightView:OnTiShengClick()
    ViewMgr:OpenViewByKey(Mod.BianQiang.Main)
end

function InviteFightView:OnClickGetReward()
	InviteFightCtrl.Instance:GetRewardReq()
	ViewMgr:CloseView(InviteFightView)
end

InviteFightPanel = InviteFightPanel or DeclareMono("InviteFightPanel", UIWFlushPanel)
--CountDown=====UIWTimeMeter
--Reward2Obj====GameObject
--RewardGetItem=GameObject
--RewardTxt====Text
--RewardItems1==UIWidgetList[UIWItemCell]
--RewardItems2==UIWidgetList[UIWItemCell]
--EffHandle=====UIEffectTool

--Model1========ModelUIShow
--ModelGray1====InteractorHandler
--Score1========UIWScoreItem
--Name1=========Text
--WinFlag1======UIImageSpriteSet
--LingXian1=====GameObject

--Model2=======ModelUIShow
--ModelGray2====InteractorHandler
--Score2========UIWScoreItem
--Name2=========Text
--WinFlag2======UIImageSpriteSet
--LingXian2=====GameObject


local vector_sp_name = "_LocShengLiYiShuZi"
local fail_sp_name = "_LocShiBaiYiShuZi"

function InviteFightPanel:InviteFightPanel()
	self.data = IllustrateData.Instance

	-- self.data_cares = {
	-- 	{data = TeamData.Instance:GetTeamInfo(), func = self.onFlush},
	-- 	{data = PetData.Instance:GetPetList(), func = self.onFlush, init_call = false},
	-- 	{data = TeamData.Instance.flush_menu, func = self.onFlush, init_call = false},
	-- }
	
	-- self.ch_drawer1 = nil
	-- self.ch_drawer2 = nil
	self.invite_data = InviteFightData.Instance
	self.EffHandle:PlayByEffectId(4161045,self.LingXian1)
	self.EffHandle:PlayByEffectId(4161045,self.LingXian2)

end

function InviteFightPanel:Awake()
	self.ch_drawer1 = UIChDrawer.CopyMainRoleData()
	self.Model1:SetShowData(
		{
			ui_obj = self.ch_drawer1,
			view = self
		}
	)
	UH.SetText(self.Name1,RoleData.Instance:GetRoleName())
	self.Score1:SetScore(self.invite_data:PlayerScore())
	
	self.ch_drawer2 = UIChDrawer.CreateByAppearance(self.invite_data:Robot().appearance)
	self.Model2:SetShowData(
		{
			ui_obj = self.ch_drawer2,
			view = self
		}
	)

	UH.SetText(self.Name2,self.invite_data:Robot().name)
	self.Score2:SetScore(self.invite_data:CurRobotScore())

	self.CountDown:StampTime(self.invite_data:EndTime(),TimeType.Type_Special_6,
		Language.InviteFight.CountDown,Language.InviteFight.CountDownComplete)

	self.CountDown:SetCallBack(BindTool.Bind(self.FlushState,self))
	self:FlushState()
end

--刷新窗口状态表现(比拼中，比拼结束可领奖)
function InviteFightPanel:FlushState()
	-- LogError("HAHAHAHAHAHA")
	local is_com = self.invite_data:EndTime() <= TimeHelper:GetServerTime()
	self.Reward2Obj:SetActive(not is_com)
	self.RewardGetItem:SetActive(is_com)

	if not is_com then
		UH.SetText(self.RewardTxt,Language.InviteFight.VectorReward)
		self.RewardItems1:SetData(self.invite_data:VectorRewardCfg())
		self.RewardItems2:SetData(self.invite_data:FailRewardCfg())
		local is_ling_xian = self.invite_data:PlayerScore() >= self.invite_data:CurRobotScore()
		self.LingXian1:SetActive(is_ling_xian)
		self.LingXian2:SetActive(not is_ling_xian)	
	else
		self.LingXian1:SetActive(false)
		self.LingXian2:SetActive(false)	
		local is_vector = self.invite_data:PlayerScore() >= self.invite_data:CurRobotScore()
		UH.SetText(self.RewardTxt,is_vector and Language.InviteFight.VectorReward or Language.InviteFight.FailReward)
		local reward_data = is_vector and self.invite_data:VectorRewardCfg() or self.invite_data:FailRewardCfg()
		self.RewardItems1:SetData(reward_data)
		self.WinFlag1:SetObjActive(true)
		self.WinFlag2:SetObjActive(true)
		self.WinFlag1.SpriteName = is_vector and vector_sp_name or fail_sp_name
		self.WinFlag2.SpriteName = not is_vector and vector_sp_name or fail_sp_name
		local gray_handler = is_vector and self.ModelGray2 or self.ModelGray1
		gray_handler.Interactable = false
	end
end

function InviteFightPanel:OnDestroy()
	if self.ch_drawer1 then
        self.ch_drawer1:DeleteMe()
        self.ch_drawer1 = nil
	end	
	if self.ch_drawer2 then
        self.ch_drawer2:DeleteMe()
        self.ch_drawer2 = nil
	end	
	UIWFlushPanel.OnDestroy(self)
end



