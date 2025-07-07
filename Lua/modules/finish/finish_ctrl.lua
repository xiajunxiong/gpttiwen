FILE_REQ("modules/finish/finish_data")
FILE_REQ("modules/finish/finish_config")
VIEW_REQ("modules/finish/finish_pass_view")
VIEW_REQ("modules/finish/finish_fail_view")
VIEW_REQ("modules/finish/finish_pass_view_am")
VIEW_REQ("modules/finish/finish_pass_view_ct")
VIEW_REQ("modules/finish/finish_pass_view_shitu_fb")
VIEW_REQ("modules/finish/finish_pass_view_gh")
VIEW_REQ("modules/finish/finish_hurt_view")
VIEW_REQ("modules/finish/finish_fail_new")

FinishCtrl = FinishCtrl or BaseClass(BaseCtrl)

function FinishCtrl:__init()
	if FinishCtrl.Instance ~= nil then
		Debuger.LogError("[FinishCtrl] attempt to create singleton twice!")
		return
	end	
	FinishCtrl.Instance = self
	self.data = FinishData.New()

    self.handle_monster_group = BattleData.Instance:BaseData():KeysCare({"monster_group_id"}, BindTool.Bind(self.OnMonsterGroupChange, self))


	
	self:RegisterProtocol(SCElementElfRewards, "SCElementElfRewards")
end

function FinishCtrl:__delete()
	FinishCtrl.Instance = nil
    BattleData.Instance:BaseData():Uncare(self.handle_monster_group)
end

function FinishCtrl:OnMonsterGroupChange()
	local group_id = BattleData.Instance:BaseData().monster_group_id
	if group_id ~= -1 then
		self.data:SetMonsterGroupId(group_id)
	end
end
function FinishCtrl:SCElementElfRewards(protocol)
	--seq大于0表示有元素奖励
	if protocol.reward_seq > 0 then
		local config = Cfg.ElementElfRewardSeq(protocol.reward_seq)
		local list = DataHelper.FormatItemList(config.reward)
		local str = ""
		for k, v in pairs(list) do
			v.ele_reward = true
			str = str .. v:QuaName() .. " "
		end
		self.data:SetEleRewards(list)
		PublicPopupCtrl.Instance:Center(Format(Language.Finish.EleReward, str))
	else
		self.data:SetEleRewards(nil)
	end
end
--[[
--战斗返回结果类型
RESULT_TYPE = {
	INVALID = 0,
	ATTACKER_WIN = 1,    --攻方胜
	DEFENDER_WIN = 2,    --守方胜
	DRAW = 3,            --和局（两边同时被打死）
	FORCED_FINISH = 4,   --战斗强制结束
	EXCEED_MAX_ROUND = 5,--超过最大回合数
	EXCEED_MAX_TIME = 6, --超过最大时间限制
	RUNAWAY = 7          --逃跑
}
param_t = {is_win,result_type}
战斗结果通知 is_win [0:失败 1:成功] ]]
function FinishCtrl:Finish(param_t,battle_mode,is_hurt)
	if battle_mode then
		self.data:SetBattleMode(battle_mode)
	end
	if param_t then
		self.data:SetFinishInfo(param_t)
	end
	if param_t.is_win == 0 and not FinishConfig.ServerBoss[self.data:GetBattleMode()]
	and battle_mode ~= BATTLE_MODE.BATTLE_MODE_GUILD_BOSS
	and not self.data:IsDraw() then
		ViewMgr:OpenView(FinishFailNew)
		return
	end
	if FinishConfig.HurtView[battle_mode] or is_hurt == true then
		ViewMgr:OpenView(FinishHurtView)
	else
		if battle_mode == BATTLE_MODE.BATTLE_MODE_GUILD_BOSS then --试炼结算都是胜利
			param_t.is_win = 1
		end
		ViewMgr:OpenView(param_t.is_win == 1 and FinishPassView or FinishFailView)
	end
end

--这里关闭结算界面 需要结算弹其他窗口加这里 
function FinishCtrl:CloseFinishView(is_win)
	if ViewMgr:IsOpen(WelcomeView) then
		return
	end
	if ViewMgr:IsOpen(DialogTipsView) then
		return
	end
	local param = self.data:GetCloseFunc() or {}
	if param.close_func then
		param.close_func(is_win)
	end
	self.data:SetBattleMode()
	self.data:SetFinishInfo()
	MainOtherCtrl.Instance:OnFinishNormalList()
end

--界面屏蔽 有自己的结算界面或者不需要结算加这里在FinishFunc
--is_win  [0:失败 1:成功] 不需要某个自行添加finish_config
function FinishCtrl.IsShowView(battle_mode,is_win)
	if is_win == 1 then
		if FinishConfig.IsWinHide[battle_mode] or 
		BattleData.Instance:IsMineMonster() or 
		FightData.Instance:IsCrystalReward(battle_mode) or 
		ChallengeTaskData.IsChallengeFinish(battle_mode) or 
		FinishConfig.MonsterGroupFinishFunc[FinishData.Instance:GetMonsterGroupId()] then
			return false
		end
	end
	if FinishConfig.IsHide[battle_mode]then
		return false
	end
	if ViewMgr:IsOpen(ActRewardTipView) then
		return false
	end
	return true
end