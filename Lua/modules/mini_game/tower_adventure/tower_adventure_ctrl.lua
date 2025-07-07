FILE_REQ("modules/mini_game/tower_adventure/tower_adventure_data")

VIEW_REQ("modules/mini_game/tower_adventure/tower_adventure_level_view")
VIEW_REQ("modules/mini_game/tower_adventure/tower_adventure_game_view")
VIEW_REQ("modules/mini_game/tower_adventure/tower_adventure_result_view")
VIEW_REQ("modules/mini_game/tower_adventure/tower_adventure_prof_select_view")

TowerAdventureCtrl = TowerAdventureCtrl or BaseClass(BaseCtrl)

function TowerAdventureCtrl:__init()
    if TowerAdventureCtrl.Instance ~= nil then
        Debuger.LogError("[TowerAdventureCtrl] attempt to create singleton twice!")
        return
    end
    TowerAdventureCtrl.Instance = self
    self.data = TowerAdventureData.New()
    self:RegisterProtocols()
end

function TowerAdventureCtrl:OnInit()
    --主界面红点
    RemindCtrl.Instance:Remind():Register(Mod.Challenge.TowerAdventure, self.data.level_data, function ()
		return self.data:AllRemind()
    end, "remind")
end

function TowerAdventureCtrl:RegisterProtocols()

end

function TowerAdventureCtrl:__delete()
    if TowerAdventureCtrl.Instance == self then
        TowerAdventureCtrl.Instance = nil
    end
    RemindCtrl.Instance:Remind():UnRegister(Mod.Challenge.TowerAdventure)
end

function TowerAdventureCtrl:OnUnloadGameLogic()
    
end

-- 判断第一次角色登录时是否需要播放小游戏第x关的流程
function TowerAdventureCtrl:IsNeedFirstStart()
    return LoginData.Instance:GetMiniGame() == MINIGAME_TYPE.TOWER_ADVENTURE and self.data.level_data.now_level == self.data:OtherSeq(2)
end

function TowerAdventureCtrl:OpenSpecGift(item_id, close_func)
    ViewMgr:OpenView(TreasureMapShow, {item = Item.Create({item_id = item_id}), close_func = close_func})
end

-- 注册挑战界面下方显示方法
function TowerAdventureCtrl:RegisterChallengeNaviShow()
    ChallengeNaviCtrl.Instance:RegisterVariantShowFunc(Mod.Challenge.TowerAdventure, function(d)
        return self.data.level_max - self.data.level_data.now_level, self.data.level_max
    end)
end