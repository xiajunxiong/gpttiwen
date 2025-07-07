
AutoCommandEscort = AutoCommandEscort or BaseClass(AutoCommand)
function AutoCommandEscort:__init(npc_seq)
    self.auto_state = AutoCommandState.Escort
    self.npc_seq = npc_seq
end

function AutoCommandEscort:DoExecute()
    SceneCtrl.Instance:Logic():AutoToNpc(self.npc_seq)
end

function AutoCommandEscort:DoStop()
    SceneLogic.Instance:ClearMoveCache()
end