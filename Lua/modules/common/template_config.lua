--===========================注意===========================
--注释的是非必要的东西，没有可以不用加，实际上可能不同
--需要根据需求复制某个模板，并把Template替换成自己的模块名
--===========================Encode===========================
CSTemplateReq = CSTemplateReq or BaseClass(BaseProtocolStruct)
function CSTemplateReq:__init()
    self.msg_type = 1000
end

function CSTemplateReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
end

--===========================Decode===========================
--接收服务端二进制形式解析 用MsgAdapter.IntFlag(n) 协议号msg_type记得修改
SCTemplateInfo = SCTemplateInfo or BaseClass(BaseProtocolStruct)
function SCTemplateInfo:__init()
    self.msg_type = 1001
end

function SCTemplateInfo:Decode()
end

--===========================Ctrl=========================== 
--[[
    Ctrl在 module_manager 注册 table.insert(self.ctrl_list,TemplateCtrl.New())
    Ctrl在 modules_require_list 加载 "modules/template/template_ctrl"
    OnInit() // 注册监听事件函数
    OnUnloadGameLogic() //切换账号重置数据回调函数
]]
FILE_REQ("modules/template/template_data")
VIEW_REQ("modules/template/template_view")

TemplateCtrl = TemplateCtrl or BaseClass(BaseCtrl)

function TemplateCtrl:__init()
	if TemplateCtrl.Instance ~= nil then
		Debuger.LogError("[TemplateCtrl] attempt to create singleton twice!")
		return
	end
	TemplateCtrl.Instance = self
	self.data = TemplateData.New()
    self:RegisterAllProtocols()
end

function TemplateCtrl:__delete()
    self:UnRegisterAllProtocols()
    TemplateCtrl.Instance = nil
end

-- function TemplateCtrl:OnUnloadGameLogic()
    -- self.data:ClearData() 
-- end

-- function TemplateCtrl:OnInit()
-- end

function TemplateCtrl:RegisterAllProtocols()
    -- self:RegisterProtocol(CSTemplateReq)
    -- self:RegisterProtocol(SCTemplateInfo, "OnTemplateInfo")
end

function TemplateCtrl:UnRegisterAllProtocols()
    -- self:UnRegisterProtocol(CSTemplateReq)
    -- self:UnRegisterProtocol(SCTemplateInfo)
end

-- function TemplateCtrl:OnTemplateInfo(protocol)
--     self.data:SetTemplateInfo(protocol)
-- end

-- function TemplateCtrl:SendTemplateReq(req_type,param1,param2)
-- 	local protocol = GetProtocol(CSTemplateReq)
-- 	protocol.req_type = req_type or 0
-- 	protocol.param1 = param1 or 0
-- 	protocol.param2 = param2 or 0
-- 	SendProtocol(protocol)
-- end

--===========================Data===========================
--ClearData() //清除数据 在OnUnloadGameLogic被调用
TemplateData = TemplateData or BaseClass()

function TemplateData:__init()
    if TemplateData.Instance ~= nil then
        Debuger.LogError("[TemplateData] attempt to create singleton twice!")
        return
    end
    TemplateData.Instance = self
    -- self.template_data = SmartData.New()
end

-- function TemplateData:ClearData()
-- end

-- function TemplateData:SetTemplateInfo(protocol)
-- end

--===========================View===========================
--[[
    View加载完成函数执行顺序
    构造函数() //面板数据初始化
    LoadCallback()  //面板进行显示初始化(外部传递参数param_t)
    OpenCallBack() // 面板开启执行函数
    CloseCallBack() //面板关闭执行函数
]]
TemplateView = TemplateView or DeclareView("TemplateView", "template/template_view")

function TemplateView:TemplateView()
end

function TemplateView:LoadCallback(param_t)
end

-- function TemplateView:OpenCallback()
-- end

-- function TemplateView:CloseCallback()
-- end

-- function TemplateView:OnClickClose()
--     ViewMgr:CloseView(TemplateView)
-- end

--===========================Panel===========================
--[[
    Panel加载完成函数执行顺序 
    构造函数()  //面板数据初始化
    Awake()     //面板进行显示初始化  (需要UIWFlushPanel.Awake(self))
    OnEnable()  //面板开启执行函数 (需要UIWFlushPanel.OnEnable(self))
    OnDestroy() //面板关闭执行函数(需要UIWFlushPanel.OnDestroy(self))
]]
--[[注册监听函数回调
    self.data_cares = {
        {data = 自身数据源,func = 自身刷新回调函数,init_call = 是否首次刷新,keys = {"需要监听的键名key"}}
    }
]]
TemplatePanel = TemplatePanel or DeclareMono("TemplatePanel", UIWFlushPanel)

function TemplatePanel:TemplatePanel()
    self.data = TemplateData.Instance
    -- self.data_cares = {
    --     {data = self.data.template_data, func = self.OnFlush, init_call = false,keys = {"val"}}
    -- }
end

function TemplatePanel:Awake()
    UIWFlushPanel.Awake(self)
end

-- function TemplatePanel:OnFlush()
-- end

-- function TemplatePanel:OnDestroy()
--     UIWFlushPanel.OnDestroy(self)
-- end

-- function TemplatePanel:OnEnable()
--     UIWFlushPanel.OnEnable(self)
-- end

--===========================TemplateItem===========================
--[Item也有OnDestroy 区别在于不用调用UIWFlushPanel.xxx(self)]

TemplateItem = TemplateItem or DeclareMono("TemplateItem", UIWidgetBaseItem)

function TemplateItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
end

--带选中可重写Click 默认不用加
-- function TemplateItem:Click()
--     self.ToggleClick:Trigger()
-- end

--没有勾选OnDestroy()不用加
-- function TemplateItem:OnDestroy()
-- end

--除GetData形式回调在对应界面类View或Panel或自身的FuncName上，下面可以不用
--点击回调带selected 搭配UIWScrollList
-- function TemplateItem:OnSelect(selected)
--     UIWidgetBaseItem.OnSelect(self,selected)
--     if selected then
--     end
-- end
--===========================Board控件===========================
TemplateBoard = TemplateBoard or DeclareMono("TemplateBoard")

function TemplateBoard:TemplateBoard()
	self.title_str = ""
	self.view_type = nil
end

function TemplateBoard:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
	end
end

function TemplateBoard:SetData(view_type,title_str)
	self.view_type = view_type
	self.title_str = title_str
    UH.SetText(self.Title,self.title_str)
end
--重置函数
function TemplateBoard:OnRelease()
end

--===========================Task===========================
--本地任务ID需要在CUSTOM_OBJID定义 其他在TaskConfig（任务菜单颜色和任务菜单底图颜色 不需要定义）
--配置 TaskConfig.NewType 
--[[ 添加本地任务
    TaskCtrl.Instance:CustomTask({
        task_name = Language.FairyWhere.TaskName,
        task_desc = Language.FairyWhere.TaskDesc,
        task_id = CUSTOM_OBJID.FAIRY_WHERE,
        task_type = TaskConfig.TaskType.FairyWhere,
    })
]]
--删除任务 TaskData.Instance:TaskInfoDeleteByTaskId(task_id)
--获取当前类型的任务 TaskData.Instance:GetTaskInfoByTaskId(task_id)
--CustomTask传入的其他数据通过self.vo.info拿到

TaskTemplate = TaskTemplate or BaseClass(TaskInfo)

function TaskTemplate:__init()
    self.skin_type = COLORSTR.Green5
    self.block_view = true --true 不显示在任务目标那边
end

--重写任务栏名字
function TaskTemplate:MenuTitle()
    return ColorStr(self.task_name, self.skin_type)
end

--重写任务描述 
function TaskTemplate:TaskGoal()
    return self.task_desc
end

--点击任务 可重写
function TaskTemplate:GoTask()
end

--是否追踪
function TaskTemplate:IsFollow()
    return true
end
--===========================场景模型对象类===========================
--在 SceneConfig.NewType 添加 默认 MainCh 其他见 scene_obj_config 配置 参考 ParadeFloat
--服务端创建可以在 SceneConfig.NetNpcsType 拓展 参考 ParadeFloat
TemplateCh = TemplateCh or BaseClass(SceneObj)

function TemplateCh:__init()
    self.vo = nil
end

function TemplateCh:__delete()
end

--刷新自己的模型 如果是角色 Npc 怪物那种可不用重写该函数(人物模型在 SceneConfig.RoleType 拓展)
function TemplateCh:FlushDrawer()
    self.body:SetData(Format(DrawerHelper.GetRidePath(self.vo.res_id)))
end

--模型创建完毕
function TemplateCh:InitEnd()
end

--重写点击事件 没有可以不用加 默认走 BehaviorCtrl.ExeByVo Npc模型会走配置自身的行为配置
function TemplateCh:InteractionBehavior()
end

--===========================客户端创建模型===========================
--可在自己的协议下发创建（如果是 SceneConfig.NetNpcsType 则不走下面 参考ParadeFloat）
--不是协议下发 vo.is_server = false
--删除模型用 VoMgr:DelGCObj(obj_id, true)
function TemplateCtrl:CreateTemplate(protocol)
    if SceneData.Instance:SceneId() == scene_id then
        local obj_id = SceneObjHolder.CalcClientObjId(SceneData.Instance:SceneId(), SceneObjType.Template, 1)
        local vo = CommonStruct.NpcVo(protocol)
        vo.obj_id = obj_id
        vo.obj_type = SceneObjType.Template
        vo.is_server = true
        vo.scene_id = scene_id
        VoMgr:AddGCObj(vo)
    end
end
--===========================翻牌使用===========================
--见 poker_info 参考 WildBossPoker

--===========================结算界面使用===========================
--成功结算(返回奖励列表) 参考 BATTLE_MODE_BATTLEFIELD
--如果是战力统计结算界面在 FinishConfig 的 HurtView 拓展
--如果战斗结束是走自己的结算界面 在 FinishConfig 的 FinishFunc 拓展
FinishData.Instance:PassRegister(battle_mode,{
    desc_func = function()
        return "成功结算描述"
    end,
    reward_func = function()
        local list = {}
        return DataHelper.FormatItemList(list)
    end,
})
--关闭结算界面回调 is_win返回结算 1 成功 0 失败
FinishData.Instance:SetCloseFunc(battle_mode,function(is_win)

end)

--===========================事件销毁销毁===========================(待补充)

--销毁计时器 比如 TimeHelper:CancelTimer(handle)

--销毁系统计时器 Runner.Instance:RemoveRunUntilTrue(self.handle_runner)

--销毁红点 UnRegisterRemind(group_id, handle)

--销毁特效 UH.StopEffect(self.EffectTool,effect_handle) 特效创建UH.PlayEffect(self.EffectTool,effect_id)

--New控件销毁 Delete(self,[[ModelGame]])

--倒计时关闭 self.Time:CloseCountDownTime()

--场景特效销毁 SE:Stop(self.effect) 场景特效创建 SE:Play(config, pos)

--===========================注册监听===========================(待补充)

--日常活动注册自己的前往事件 ActivityData.Instance:CustomClickHandle(act_type, BindTool.Bind(self.OnClickEvent, self))

--随机活动注册自己的前往事件 ActivityRandData.Instance:CustomClickHandle(act_type, BindTool.Bind(self.OnClickEvent, self))

--随机活动主界面图标注册开启显示条件 ActivityRandData.Instance:Register(modkey,check_func,origin)