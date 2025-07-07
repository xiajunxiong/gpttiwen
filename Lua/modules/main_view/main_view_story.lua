MainViewStory = MainViewStory or DeclareMono("MainViewStory", UIWFlushPanel)
function MainViewStory:MainViewStory()
    self.battle_data = BattleData.Instance
    self.data_cares = {
        {data = self.battle_data.story_data, func = self.HideUI, keys = {"hide_ui"}},
        {data = self.battle_data.base, func = self.ShowUI, keys = {"server_fighting"}}
        -- {data = self.battle_data.story_data, func = self.AutoChange, keys = {"auto_anim"}, init_call = false},
        -- {data = self.battle_data.base, func = self.SelTargetStateChange, keys = {"sel_tar_type"}},
    }
end
function MainViewStory:ShowUI()
    -- if self.battle_data.base.server_fighting == true then
        for i = 1, self.HideObjs:Length() do
            if i == 5 then
                self.HideObjs[i]:SetActive(BattleData.Instance:IsShowStrategyBtn())
            elseif i == 1 then
                self.HideObjs[i]:SetActive(BattleData.Instance:BattleMode() ~= BATTLE_MODE.BATTLE_MODE_INITIAL_FIGHT)
            else
                self.HideObjs[i]:SetActive(true)
            end
        end
        for i = 1, self.HideCanvas:Length() do
            self.HideCanvas[i].enabled = true
        end
        self.RbFuncsObj.enabled = true
    -- end
end
function MainViewStory:HideUI()
    BattleData.Instance:BaseData().sel_tar_type = BATTLE_MOVE_TYPE.NONE
    BattleData.Instance:ViewInfo().opened_sub = BattleSubView.None
    for i = 1, self.HideObjs:Length() do
        if i == 1 then
            self.HideObjs[i]:SetActive((not self.battle_data.story_data.hide_ui) and (BattleData.Instance:BattleMode() ~= BATTLE_MODE.BATTLE_MODE_INITIAL_FIGHT))
        else
            if i == 6 then
                if BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_MINI_GAME_BATTLE then
                    self.HideObjs[i]:SetActive(true)
                else
                    self.HideObjs[i]:SetActive(not self.battle_data.story_data.hide_ui)
                end
            else
                self.HideObjs[i]:SetActive(not self.battle_data.story_data.hide_ui)
            end
        end
    end
    for i = 1, self.HideCanvas:Length() do
        self.HideCanvas[i].enabled = false
    end
    self.RbFuncsObj.enabled = not self.battle_data.story_data.hide_ui
    
end
function MainViewStory:AutoChange()
    --[[  self.RbFuncsObj:SetActive(not self.RbFuncsObj.activeSelf) ]]
end
function MainViewStory:SelTargetStateChange()
    --[[ local scene_data = BattleData.Instance:SceneData()
    if scene_data then
        scene_data:CameraSwitch(0)
    end ]]
end
