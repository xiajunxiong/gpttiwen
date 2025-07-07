FILE_REQ("modules/duel/duel_data")
VIEW_REQ("modules/duel/duel_start_view")
VIEW_REQ("modules/duel/duel_dialog_view")

DuelBattleType = {
    TYPE_DUEL = 1,          --决斗(红,白打白名)
    TYPE_ADJUDICATION = 2,  --裁决(红,白打红名)
    TYPE_ESCORT = 3,        --劫镖(红,白打运镖玩家)
}

--【决斗】
DuelCtrl = DuelCtrl or BaseClass(BaseCtrl)
function DuelCtrl:__init()
    if DuelCtrl.Instance ~= nil then
        Debuger.LogError("[DuelCtrl] attempt to create singleton twice!")
        return
    end
    DuelCtrl.Instance = self
    self.data = DuelData.New()

	self:RegisterProtocol(SCDuelBattleInfo, "OnSCDuelBattleInfo")
	self:RegisterProtocol(SCRoleDuelKillValueAOINotice, "OnSCRoleDuelKillValueAOINotice")
end

function DuelCtrl:__delete()
end

function DuelCtrl:OnUnloadGameLogic()
    self.data.last_scene_id = -1
end

function DuelCtrl:OnInit()
    self.data:OnInit()
end

function DuelCtrl:OpenTips()
    PublicPopupCtrl.Instance:HelpTip(self.data.other_cfg.duel_des)
end

function DuelCtrl:OnSCDuelBattleInfo(protocol)
    self.data:SetDuelInfo(protocol)
    self.is_display = true
    self:CheckDuelStartDisplay()
end

function DuelCtrl:OnSCRoleDuelKillValueAOINotice(protocol)
    VoMgr:ChangeRoleKillValue(protocol.obj_id, protocol.kill_value)
	if protocol.obj_id == SceneData.Instance:GetRoleObjId() then
		RoleData.Instance:GetBaseData().kill_value = protocol.kill_value
    end
end

function DuelCtrl:CheckDuelStartDisplay()
    if BattleData.Instance:BaseData().battle_mode == BATTLE_MODE.BATTLE_MODE_DUEL then
        if self.is_display then
            ViewMgr:OpenView(DuelStartView)
            self.is_display = false
        end
    end
end
