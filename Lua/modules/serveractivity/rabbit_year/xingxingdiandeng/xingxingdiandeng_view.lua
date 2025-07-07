--[[
    View加载完成函数执行顺序
    构造函数() //面板数据初始化
    LoadCallback()  //面板进行显示初始化(外部传递参数param_t)
    OpenCallBack() // 面板开启执行函数
    CloseCallBack() //面板关闭执行函数
]] XingXingDianDengView = XingXingDianDengView or
                              DeclareView("XingXingDianDengView", "xingxingdiandeng/xingxingdiandeng_view",
        Mod.RabbitYear.XingXingDianDeng)

function XingXingDianDengView:XingXingDianDengView()
end

function XingXingDianDengView:LoadCallback(param_t)
end

-- function XingXingDianDengView:OpenCallback()
-- end

-- function XingXingDianDengView:CloseCallback()
-- end

-- function XingXingDianDengView:OnClickClose()
--     ViewMgr:CloseView(XingXingDianDengView)
-- end

XingXingDianDengPanel = XingXingDianDengPanel or DeclareMono("XingXingDianDengPanel", UIWFlushPanel)
function XingXingDianDengPanel:XingXingDianDengPanel()
    self.data = XingXingDianDengData.Instance
    self.data_cares = {{
        data = self.data:GetSerInfo(),
        func = self.FlushBaseInfo
    }}
end
function XingXingDianDengPanel:OnClickClose()
    ViewMgr:CloseView(XingXingDianDengView)
end
function XingXingDianDengPanel:OnClickFive()
    local item_id = Config.xingxingdiandeng_auto.other[1].consume_item
    if Item.GetNum(item_id) <= 4 then
        local item = Item.Init(item_id)
        MainOtherCtrl.Instance:GetWayView({
            item = item
        })
        return
    end
    XingXingDianDengCtrl.Instance:SendReq(self.data.OP_TYPE.LOTERY5)
end
function XingXingDianDengPanel:OnClickOne()
    local item_id = Config.xingxingdiandeng_auto.other[1].consume_item
    if Item.GetNum(item_id) <= 0 then
        local item = Item.Init(item_id)
        MainOtherCtrl.Instance:GetWayView({
            item = item
        })
        return
    end
    XingXingDianDengCtrl.Instance:SendReq(self.data.OP_TYPE.LOTERY)
end
function XingXingDianDengPanel:PlayEffect()
    self.EffectTool:Play(6167011)
end

function XingXingDianDengPanel:FlushBaseInfo(old_val)
    if old_val and old_val.count then
        if old_val.count ~= self.data:GetSerInfo().count then
            self:PlayEffect()
        end
    end
    self.TimeMetter:CreateActTime(ACTIVITY_TYPE.RAND_STAR_LIGHTING, TimeType.Type_Time_3, Language.XingXingDianDeng.Time)
    local config = self.data:GetConfig()
    self.List:SetData(config)
    local item_id = Config.xingxingdiandeng_auto.other[1].consume_item
    self.item_id = item_id
    UH.SetIcon(self.Icon, Item.GetIconId(item_id))
    UH.SetText(self.Count, Item.GetNum(item_id))
    local add_light_num = config[#config].add_light_num
    self.ProBar:SetProgress(self.data:GetSerInfo().count / add_light_num)
    self.RedPointOne:SetNum(Item.GetNum(self.item_id) > 0 and 1 or 0)
    self.RedPointFive:SetNum(Item.GetNum(self.item_id) >= 5 and 1 or 0)
end
function XingXingDianDengPanel:OnClickGet(item)
    local data = item:GetData()
    if self.data:IsLock(data.add_light_num) or self.data:IsGet(data.seq) then
        CellClicks.BagGridNoButton(item.Cell)
        return
    end
    XingXingDianDengCtrl.Instance:SendReq(self.data.OP_TYPE.RECEIVE, data.seq)
end
function XingXingDianDengPanel:OnClickCost()
    local item_id = Config.xingxingdiandeng_auto.other[1].consume_item
    local item = Item.Init(item_id)
    MainOtherCtrl.Instance:GetWayView({
        item = item
    })
end
XingXingDianDengItem = XingXingDianDengItem or DeclareMono("XingXingDianDengItem", UIWidgetBaseItem)
function XingXingDianDengItem:XingXingDianDengItem()
    self.xing_data = XingXingDianDengData.Instance
end
function XingXingDianDengItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.item = Item.Create(data.reward_item)
    self.Cell:SetData(self.item)
    UH.SetText(self.Desc, Format(Language.XingXingDianDeng.Desc, data.add_light_num))
    self.LockObj:SetActive(self.xing_data:IsLock(data.add_light_num))
    self.GetObj:SetActive(self.xing_data:IsGet(data.seq))
    self.CountObj:SetActive(not self.xing_data:IsGet(data.seq))
    local can_get = (not self.xing_data:IsGet(data.seq) and not self.xing_data:IsLock(data.add_light_num))
    self.RedPoint:SetNum(can_get and 1 or 0)
    if can_get then
        if self.handle_effect == nil then
            self.handle_effect = self.EffectTool:Play(6167016)
        end
    else
        if self.handle_effect then
            self.EffectTool:Stop(self.handle_effect)
            self.handle_effect = nil
        end
    end
end
