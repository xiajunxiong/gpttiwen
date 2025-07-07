-- 新奇闻，游历诗人系列玩法
PoetryGameView = PoetryGameView or DeclareView("PoetryGameView", "anecdote_new/poetry_game_view", Mod.AnecdoteNew.Poetry)
VIEW_DECLARE_MASK(PoetryGameView, ViewMask.None)
function PoetryGameView:PoetryGameView()
    self.Timer2 = nil
    self.type = NewAnecdoteType.YLSR
end

function PoetryGameView:OpenCallback()
    local param = AnecdoteData.Instance:GetNewParam(self.type, true)
    if TableIsEmpty(param) then
        LogError("游历诗人玩法无参数")
        ViewMgr:CloseView(PoetryGameView)
        return
    end
    local seq = param.seq
    self.seq = seq
    local cfg = self:GetCfg(seq)
    if not cfg then
        LogError("游历诗人玩法无配置")
        ViewMgr:CloseView(PoetryGameView)
        return
    end

    PoetryData.Instance:DataReset(cfg)
	PoetryData.Instance:SetFinishCallback(BindTool.Bind1(self.PlayEffect, self))
	self.PoetryMono:InitData(nil, Language.Anecdote.PoetryWord3)
end

function PoetryGameView:GetCfg(seq)
    local config = Config.anecdotes_config_auto.poetry_list
    local group = DataHelper.ConvertGroup(config, "match_seq")
    local initial = group[seq]
    local group_2 = DataHelper.ConvertGroup(Config.anecdotes_config_auto.poetry_text, "match_seq")
    local poetry_number = group_2[seq]
    local cfg = {
        initial = initial,
        poetry_number = poetry_number,
    }
    return cfg
end

function PoetryGameView:OnCloseClick()
	if not PoetryData.Instance:GetIsCanDrag() then
		return
	end
	-- local param_t = {
	-- 	content = Language.Anecdote.CookSortExitTip,
	-- 	confirm = {
	-- 		func = function()
	-- 			ViewMgr:CloseView(DialogTipsView)
	-- 			local talk_id = AnecdoteData.Instance:GetFailTalkId(AnecdoteType.CookSort)
	-- 			NpcDialogCtrl.Instance:NpcDialogView({talk_id = talk_id})
	-- 			ViewMgr:CloseView(PoetryGameView)
	-- 		end
	-- 	}
	-- }
    -- PublicPopupCtrl.Instance:DialogTips(param_t)
    ViewMgr:CloseView(PoetryGameView)
end

function PoetryGameView:CloseCallback()
    TimeHelper:CancelTimer(self.Timer2)
    if PoetryData.Instance:GetIsSucceed() then
        -- 发送成功请求
        AnecdoteData.Instance:NewAnecdoteSucc(self.seq)
    end
end

function PoetryGameView:PlayEffect()
	self.ObjFinishMask:SetActive(true)
end

-- 点击全匹配遮罩
function PoetryGameView:OnFinishClick()
    ViewMgr:CloseView(PoetryGameView)
end