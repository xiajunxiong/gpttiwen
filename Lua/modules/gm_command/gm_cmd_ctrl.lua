FILE_REQ("modules/gm_command/gm_behavior")
FILE_REQ("modules/gm_command/gm_data")

VIEW_REQ("modules/gm_command/gm_window")
GMCmdCtrl = GMCmdCtrl or BaseClass(BaseCtrl)

function GMCmdCtrl:__init()
    if GMCmdCtrl.Instance ~= nil then
        Debuger.LogError("[GMCmdCtrl] attempt to create singleton twice!")
        return
    end
    GMCmdCtrl.Instance = self
    self.data = GmData.New()
    self:RegisterProtocol(SCGMCommand, "OnGmCommandReturn")
    self:RegisterProtocol(CSGMCommand)
end

function GMCmdCtrl:SendGMCommand(cmd_type, cmd_params)
    local protocol = GetProtocol(CSGMCommand)
    protocol.type = cmd_type
    protocol.command = cmd_params
    SendProtocol(protocol)
    Debuger.Log("Send GM Cmd Type=%s,params=%s", protocol.type, protocol.command)
end

function GMCmdCtrl:OnGmCommandReturn(protocol)
    Debuger.Log("Recv GM Cmd Resut=%s,Type=%s", protocol.result, protocol.type)
end

--加载自定义函数字符串
function GMCmdCtrl:LoadCall(key,params)
    BehaviorGM.LoadCall(key,params)
end