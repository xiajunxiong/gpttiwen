FILE_REQ(LandsBetweenConfig.ModPath.."god_pos/god_pos_data")

VIEW_REQ(LandsBetweenConfig.ModPath.."god_pos/god_pos_view")
VIEW_REQ(LandsBetweenConfig.ModPath.."god_pos/god_pos_list_show")
VIEW_REQ(LandsBetweenConfig.ModPath.."god_pos/god_pos_reward_show")
-- 神域-神位
GodPosCtrl = GodPosCtrl or BaseClass(BaseCtrl)

function GodPosCtrl:__init()
    if GodPosCtrl.Instance ~= nil then
        Debuger.LogError("[GodPosCtrl] attempt to create singleton twice!")
        return
    end
    GodPosCtrl.Instance = self
    self.data = GodPosData.New()
end

function GodPosCtrl:OnInit()
    self:RegisterRemind()
    self:RegisterAllProtocol()
end

function GodPosCtrl:RegisterRemind()
    local remind_func = BindTool.Bind(self.FlushRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.LandsBetweenExtra.GodPos,
        self.data.info_data,
        remind_func
    )

    LandsBetweenData.Instance:ExtraRemindMark({
        mod_key = Mod.LandsBetweenExtra.GodPos,
        target = self.data.info_data,
        func = remind_func,
    })
end

function GodPosCtrl:RegisterAllProtocol()
    self:RegisterProtocol(SCGodSpaceUserGodLevelInfo, "OnSCGodSpaceUserGodLevelInfo")
end

function GodPosCtrl:__delete()
    if GodPosCtrl.Instance == self then
        GodPosCtrl.Instance = nil
    end
end

function GodPosCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function GodPosCtrl:FlushRemind()
    local num = self.data:Remind()
    return num
end

-- 发送请求(task_id)
function GodPosCtrl:SendReq(param_1)
    local param_t = {}
    param_t.oper_type = 8
    param_t.param_1 = param_1
    LandsBetweenCtrl.Instance:GodSpaceOperate(param_t)
end

-- 神位信息
function GodPosCtrl:OnSCGodSpaceUserGodLevelInfo(protocol)
    self:MyLogError(ColorStr("神位信息下发"..protocol.msg_type, COLORSTR.Red8), protocol)
    self.data:SetInfo(protocol)
end

function GodPosCtrl:MyLogError(...)
    -- LogError(...)
end