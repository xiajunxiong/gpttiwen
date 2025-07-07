FILE_REQ("modules/challenge_navi/challenge_navi_config")
FILE_REQ("modules/challenge_navi/challenge_navi_data")
VIEW_REQ("modules/challenge_navi/challenge_navi_view")

ChallengeNaviCtrl = ChallengeNaviCtrl or BaseClass(BaseCtrl)

function ChallengeNaviCtrl:__init()
    if ChallengeNaviCtrl.Instance ~= nil then
        Debuger.LogError("[ChallengeNaviCtrl] attempt to create singleton twice!")
        return
    end
    ChallengeNaviCtrl.Instance = self
    self.data = ChallengeNaviData.New()
end

function ChallengeNaviCtrl:__delete()
    ChallengeNaviCtrl.Instance = nil
end

--@@模块注册自己在挑战导航界面预显示的一些数据信息
--@@func function()
--  return1 数据1 (如界面显示[45级]则返回 45<R1>)
--  return2 数据2 (如界面显示[2/3]则返回 2<R1>,3<R2>)
--  return3 数据3 (如界面需要显示倒计时或重置时间，则返回 R1,R2,时间戳<R3>)
--  return4 数据4 (如界面需要显示倒计时描述，则返回 R1,R2,时间戳<R3>,倒计时描述<R4>)
function ChallengeNaviCtrl:RegisterVariantShowFunc(mod,func)
    for _,val in pairs(ChallengeNaviConfig.Config) do
        if val.mod == mod then
            val.variant_func = func
            return
        end
    end
end 
--@@模块注册自己在挑战导航界面被点击之后所执行的行为，未注册默认是直接打开模块界面
--@@func function() --挑战导航界面对应模块被点击时会调用func
function ChallengeNaviCtrl:RegisterOpenReqFunc(mod,func)
    for _,val in pairs(ChallengeNaviConfig.Config) do
        if val.mod == mod then
            val.open_req_func = func
            return
        end
    end
end