
FILE_REQ("modules/exchange/exchange_mono_data")

VIEW_REQ("modules/exchange/exchange_task_view")

ExchangeMonoCtrl = ExchangeMonoCtrl or BaseClass(BaseCtrl)

function ExchangeMonoCtrl:__init()
    if ExchangeMonoCtrl.Instance ~= nil then
        Debuger.LogError("[ExchangeMonoCtrl] attempt to create singleton twice!")
        return
    end
    ExchangeMonoCtrl.Instance = self
    self.data = ExchangeMonoData.New()
end

function ExchangeMonoCtrl:OnInit()
    BehaviorData.Instance:CustomNpcTalkBtnsSeq(self.data.medaltask_exchange_npc_seq, BindTool.Bind(self.SetupBehaviourBtn, self))
end

function ExchangeMonoCtrl:SetupBehaviourBtn(btn_list,obj_vo)
    if nil == obj_vo then return end
    if obj_vo and obj_vo.seq == self.data.medaltask_exchange_npc_seq then
        if self.data:MedalExchange() then
            local button = NpcDialogBehBtnData.FuncCreate(Language.ExchangeMono.MedalTaskBtnName, function ()
                    ViewMgr:OpenView(ExchangeTaskView)
                end, true)
            table.insert(btn_list, button)
        end
    end
end