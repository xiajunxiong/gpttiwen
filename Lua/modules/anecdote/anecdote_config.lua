AnecdoteConfig = {
    CloseViewList = { --被邀请组队的时候需要关闭的界面
        Mod.Anecdote.SudokuBook,
        Mod.Anecdote.Puzzle,
        Mod.Anecdote.Vision,
        Mod.Anecdote.Couplet,
        Mod.Anecdote.Poetry,
        Mod.Anecdote.StarLife,
        Mod.Anecdote.FishAnswer,
        Mod.Anecdote.Klotski,
        Mod.Anecdote.CookSort,
        Mod.Anecdote.Decrypt,
        Mod.Anecdote.PackPoetry,
        Mod.Anecdote.PuzzleMechanical,
        Mod.Anecdote.PuzzleStorehouse,
        Mod.Anecdote.VisionDiscern,
        Mod.Anecdote.MillenniumPeachBlossom,
        Mod.Anecdote.PoetrySoul,
        Mod.Anecdote.Poetry2,
        Mod.Anecdote.Decrypt2,
        Mod.Anecdote.Decrypt3,
        Mod.Anecdote.Couplet2,
        Mod.Anecdote.PackPoetry2,
        Mod.Anecdote.Couplet3,
    },
    --主线任务进入奇闻
    OpenFunc = {
		[AnecdoteType.Couplet] = function(func)
			ViewMgr:OpenView(CoupletView,{config = Config.main_world_couplet_auto,img_type = 1,send_func = func})
		end,
		[AnecdoteType.FoxCatch] = function(func)
			CatchFoxData.SendTaskFinishReq = func
			SceneCtrl.Instance:RequestEnterFb(FB_TYPE.MAIN_CATCH_FOX)
		end,
        [AnecdoteType.StarLife] = function(func)
            SmxjData.SendTaskFinishReq = func
            local task_info = AnecdoteData.Instance:GetTaskInfoByType(AnecdoteType.StarLife)
            if task_info == nil or (task_info.task_id == 65160 and task_info.is_accepted == false) then
                SceneLogic.Instance:AutoToNpcs(59053)
            else
                SceneLogic.Instance:AutoToNpcs(59046)
            end
		end
	},
}