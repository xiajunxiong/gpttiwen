FightMedalMenuView = FightMedalMenuView or DeclareMono("FightMedalMenuView", UIWFlushPanel)

function FightMedalMenuView:FightMedalMenuView()
    self.data = SceneData.Instance
    self.medal_data = MedalTaskData.Instance
    self.data_cares = {
        {data = self.data.base, func = self.FlushShow,keys = {"scene_id"}},
    }

end 

function FightMedalMenuView:FlushShow()
    local cfg = self.medal_data:GetCurSceneConfig()
    local _str_desc = ""
    local _str_progress = ""

    if cfg then 
        _str_desc = cfg.txt
        _str_progress = string.format( Language.Fight.MedalProgressTips,cfg.level,cfg.layer)
    end 

    self.desc.text = _str_desc
    self.progress.text = _str_progress
end