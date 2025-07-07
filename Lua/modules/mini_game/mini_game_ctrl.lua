FILE_REQ("modules/mini_game/mini_game_data")

MiniGameCtrl = MiniGameCtrl or BaseClass(BaseCtrl)

function MiniGameCtrl:__init()
    if MiniGameCtrl.Instance ~= nil then
        Debuger.LogError("[MiniGameCtrl] attempt to create singleton twice!")
        return
    end
    MiniGameCtrl.Instance = self
    self.data = MiniGameData.New()
    self:RegisterProtocols()
end

function MiniGameCtrl:OnInit()
    -- 登录的时候需要确认挑战列表信息
    self.login_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.CheckChallengeNaviData, self))
end

function MiniGameCtrl:__delete()
    self:UnRegisterProtocol(CSMiniGameFinishNextStage)
    self:UnRegisterProtocol(CSMiniGameClaimPhaseReward)
    self:UnRegisterProtocol(CSMiniGameStartBattle)
    self:UnRegisterProtocol(SCMiniGameInfo)
    EventSys:UnBind(self.login_handle)
    if MiniGameCtrl.Instance == self then
        MiniGameCtrl.Instance = nil
    end
end

function MiniGameCtrl:CheckChallengeNaviData()
    self.data:CheckChallengeNaviData()
end

function MiniGameCtrl:RegisterProtocols()
    self:RegisterProtocol(CSMiniGameFinishNextStage)
    self:RegisterProtocol(CSMiniGameClaimPhaseReward)
    self:RegisterProtocol(CSMiniGameStartBattle)
    self:RegisterProtocol(SCMiniGameInfo,"OnSCMiniGameInfo")
end

function MiniGameCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

-- 干净账号第一次进入哪个小游戏
function MiniGameCtrl:GoToMiniGame()
	local mini_game = LoginData.Instance:GetMiniGame()
    if mini_game > 0 then
        -- 选关界面不通用，在scene里做区分
		SceneCtrl.Instance:GotoScene(MiniGameScene)
	else
		SceneCtrl.Instance:GotoScene(RoleCreateScene)
	end
end

-- 是否有打开中，MiniGameAllView里的view
function MiniGameCtrl:IsOpenMiniGameView(is_need_close)
    local is_open = false
    for k, v in pairs(MiniGameAllView) do
        if ViewMgr:IsOpen(_G[k]) then
            if is_need_close then
                ViewMgr:CloseView(_G[k])
                is_open = true
            else
                return true
            end
        end
    end
    return is_open
end

function MiniGameCtrl:OnSCMiniGameInfo(protocol)
    -- LogError("OnSCMiniGameInfo",protocol)
    local mini_game = LoginData.Instance:GetMiniGame()
    -- 协议共用，但具体还是分到不同的游戏去用去设置
    if mini_game == MINIGAME_TYPE.TOWER_ADVENTURE then
        TowerAdventureData.Instance:SetLevelData(protocol)
    elseif mini_game == MINIGAME_TYPE.COW_BOY then
        CowBoyData.Instance:SetLevelData(protocol)
    end
end

function MiniGameCtrl:SendMiniGameFinishNextStage()
    local protocol = GetProtocol(CSMiniGameFinishNextStage)
    SendProtocol(protocol)
end

function MiniGameCtrl:SendMiniGameGetPhaseReward(seq)
    local protocol = GetProtocol(CSMiniGameClaimPhaseReward)
    protocol.seq = seq
    SendProtocol(protocol)
end

function MiniGameCtrl:OpenDialogView(npc_id, str, end_func, talk_list_cfg, block_flush, block_dofunc)
    local param_t = {talk_list = {},block_dofunc = block_dofunc}
    if TableIsEmpty(talk_list_cfg) then
        table.insert(param_t.talk_list, {talker = npc_id, talk_text = str})
    else
        for i = 1, #talk_list_cfg do
            table.insert(param_t.talk_list,{talker = talk_list_cfg[i].npc_id_1, talk_text = talk_list_cfg[i].txt_1})
        end
    end
    NpcDialogCtrl.Instance:NpcDialogView(param_t, end_func, false, nil , true, block_flush)
end

function MiniGameCtrl:SendMiniGameStartBattle(seq, has_pet, has_partner, has_skill)
    local protocol = GetProtocol(CSMiniGameStartBattle)
    protocol.seq = seq
    protocol.has_pet = has_pet or 0
    protocol.has_partner = has_partner or 0
    protocol.has_skill = has_skill or 0
    SendProtocol(protocol)
end