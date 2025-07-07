--新奇闻，九宫拼图玩法
PuzzleGameView = PuzzleGameView or DeclareView("PuzzleGameView", "anecdote_new/puzzle_game_view", Mod.AnecdoteNew.Puzzle)
function PuzzleGameView:PuzzleGameView()
    self.type = NewAnecdoteType.JGPT
end

function PuzzleGameView:LoadCallback()
    local param = AnecdoteData.Instance:GetNewParam(self.type, true)
    if TableIsEmpty(param) then
        LogError("九宫拼图玩法无参数")
        ViewMgr:CloseView(PuzzleGameView)
        return
    end
    local seq = param.seq
    self.seq = seq
    local cfg = self:GetCfg(seq)
    if not cfg then
        LogError("九宫拼图玩法无配置")
        ViewMgr:CloseView(PuzzleGameView)
        return
    end

    local img = cfg.photo_res
    local img_type = PuzzleData.PuzzleImgType.Raw
    self.Panel:SetData(self:GetType(),AnecdoteType.Puzzle,
    BindTool.Bind(self.OnCloseFunc,self),img,img_type,false)

    local path = string.format("anecdote_new/puzzle_game/%s_r.png",img)
    self.ResultImg:SetByPath(path)
end

function PuzzleGameView:GetCfg(seq)
    for i,v in pairs(Config.anecdotes_config_auto.puzzle_list) do
        if v.seq == seq then
            return v
        end
    end
end

-- 游戏成功回调
function PuzzleGameView:OnCloseFunc(is_ok)
    if is_ok then
        AnecdoteData.Instance:NewAnecdoteSucc(self.seq)
    end
    ViewMgr:CloseView(PuzzleGameView)
end

function PuzzleGameView:OnCloseClick()
    ViewMgr:CloseView(PuzzleGameView)
end
