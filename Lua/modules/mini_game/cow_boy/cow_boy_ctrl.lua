FILE_REQ("modules/mini_game/cow_boy/cow_boy_config")
FILE_REQ("modules/mini_game/cow_boy/cow_boy_data")

VIEW_REQ("modules/mini_game/cow_boy/cow_boy_level")
VIEW_REQ("modules/mini_game/cow_boy/cow_boy_game")
VIEW_REQ("modules/mini_game/cow_boy/cow_boy_result")
VIEW_REQ("modules/mini_game/cow_boy/cow_boy_prof_select_view")

CowBoyCtrl = CowBoyCtrl or BaseClass(BaseCtrl)

function CowBoyCtrl:__init()
    if CowBoyCtrl.Instance ~= nil then
        Debuger.LogError("[CowBoyCtrl] attempt to create singleton twice!")
        return
    end
    CowBoyCtrl.Instance = self
    self.data = CowBoyData.New()
end

function CowBoyCtrl:OnInit()
    --主界面红点
    RemindCtrl.Instance:Remind():Register(Mod.Challenge.CowBoy, self.data.level_data, function ()
		return 0
    end)
end

function CowBoyCtrl:RegisterProtocols()

end

function CowBoyCtrl:__delete()
    if CowBoyCtrl.Instance == self then
        CowBoyCtrl.Instance = nil
    end
    RemindCtrl.Instance:Remind():UnRegister(Mod.Challenge.CowBoy)
end

function CowBoyCtrl:OnUnloadGameLogic()
end

-- 判断第一次角色登录时是否需要播放小游戏第x关的流程
function CowBoyCtrl:IsNeedFirstStart()
    return LoginData.Instance:GetMiniGame() == MINIGAME_TYPE.COW_BOY and self.data.level_data.now_level == self.data:OtherSeq(2)
end

function CowBoyCtrl:OpenSpecGift(item_id, close_func)
    ViewMgr:OpenView(TreasureMapShow, {item = Item.Create({item_id = item_id}), close_func = close_func})
end

-- 注册挑战界面下方显示方法
function CowBoyCtrl:RegisterChallengeNaviShow()
    ChallengeNaviCtrl.Instance:RegisterVariantShowFunc(Mod.Challenge.CowBoy, function(d)
        return self.data.level_max - self.data.level_data.now_level, self.data.level_max
    end)
end

-- 申请战斗
function CowBoyCtrl:GoBattle()
    local seq = self.data:NowFightSeq()
    local has_pet = self.data:NowHasType(CowBoyCfg.CellType.Pet) and 1 or 0
    local has_partner = self.data:NowHasType(CowBoyCfg.CellType.Partner) and 1 or 0
    local has_skill = self.data:NowHasType(CowBoyCfg.CellType.Skill) and 1 or 0
    self.mini_game_battle = true
    MiniGameCtrl.Instance:SendMiniGameStartBattle(seq, has_pet, has_partner, has_skill)
end