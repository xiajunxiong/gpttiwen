--================================MainViewTopLeftBtn================================
MainViewTopLeftBtn = MainViewTopLeftBtn or DeclareMono("MainViewTopLeftBtn", UIWFlushPanel)

function MainViewTopLeftBtn:MainViewTopLeftBtn()
    self.data = ActivityRandData.Instance
    self.data_cares = {
        {data = self.data.rand_open_data, func = self.OnFlush, init_call = false},
        {data = RoleData.Instance:GetBaseData(), func = self.OnFlush, init_call = false, keys = {"level"}},
        {data = RoleData.Instance:GetBaseData(), func = self.CleckData, init_call = false, keys = {"role_id"}},
        {data = TimeCtrl.Instance.server_data, func = self.OnFlush, init_call = false, keys = {"server_open_day"}},
        {data = ActivityData.Instance:GetActivityEvent(),func = self.OnFlushEvent,init_call = false,keys = {"act_type", "status"}},
        {data = MainViewData.Instance.remind_data, func = self.FlushTopLRedPoint, keys = {"flush_lt"}, init_call = false},
        {data = MainViewData.Instance.view_data, func = self.FlushOnClickIconHide, keys = {"hide_lt"}, init_call = false},
        {data = MainViewData.Instance.view_data, func = self.FlushOnClickIconShow, keys = {"show_lt"}, init_call = false},
        {data = TaskData.Instance.task_id_list, func = self.OnFlushTaskView, init_call = false,keys = {"finish_list"}},
    }
end

function MainViewTopLeftBtn:Awake()
    UIWFlushPanel.Awake(self)
    if IS_STANDALONE() then
        UH.LocalScale(self.Pos2Rect, STANDALONE_UI_VECTOR3)
        UH.LocalScale(self.Pos1Rect, STANDALONE_UI_VECTOR3)
        UH.LocalScale(self.SubsRect, STANDALONE_UI_VECTOR3)
    end
end

function MainViewTopLeftBtn:OnEnable()
    UIWFlushPanel.OnEnable(self)
    if not BATTLE_TOP_MAIN_NOT_SHOW[SceneData.Instance:GetMode()] then
        if MainViewData.Instance.left_top_icons_show == true then
            self.LTBtnAnim:SetBool("state", true)
        end
    end
    self:OnFlush()
end

--监听任务
function MainViewTopLeftBtn:OnFlushTaskView()
    for k,v in pairs(self.data:GetFinishTaskConfig()) do
        if not TaskData.Instance:GetTaskIsCompleted(v) then
            self:OnFlush()
            break
        end
    end
end

--监听随机活动
function MainViewTopLeftBtn:OnFlushEvent()
	if ActivityData.Instance:GetActivityEventType() > SERVER_ACTIVITY_MIN_NUM then
		self:OnFlush()
	end
end

function MainViewTopLeftBtn:OnClickIconShow()
    self.LTBtnAnim:SetBool("state", true)
    MainViewData.Instance.left_top_icons_show = true
end
function MainViewTopLeftBtn:FlushOnClickIconShow()
    self.LTBtnAnim:SetBool("state", true)
end

function MainViewTopLeftBtn:OnClickIconHide()
    self:FlushTopLRedPoint()
    self.LTBtnAnim:SetBool("state", false)
    MainViewData.Instance.left_top_icons_show = false
end
function MainViewTopLeftBtn:FlushOnClickIconHide()
    self:FlushTopLRedPoint()
    self.LTBtnAnim:SetBool("state", false)
end

function MainViewTopLeftBtn:FlushTopLRedPoint()
    self.Arrow2Red:SetActive(MainViewData.Instance:IsHasTlRemindNums())
end

--TopLeftList固定列表 IconList不固定列表
function MainViewTopLeftBtn:OnFlush()
    for k,item in pairs(self.TopLeftList.item_list or {}) do
        if item ~= nil then
            item:OnRelease()
        end
    end
    for k,item in pairs(self.IconList.item_list or {}) do
        if item ~= nil then
            item:OnRelease()
        end
    end
    self.IconList:SetData(self.data:GetBtnList())
    self.TopLeftList:SetCompleteCallBack(function()
        for i=1,self.Arrow:Length() do
            self.Arrow[i].transform:SetAsLastSibling()--使该UI的层级显示在最前面
        end
    end)
    self.TopLeftList:SetData(self.data:GetRandMainList())
end

function MainViewTopLeftBtn:CleckData()
    for k,item in pairs(self.IconList.item_list or {}) do
        if item ~= nil then
            item:OnRelease()
        end
    end
    self.IconList:SetData({})
    for k,item in pairs(self.TopLeftList.item_list or {}) do
        if item ~= nil then
            item:OnRelease()
        end
    end
    self.TopLeftList:SetData({})
end

--================================MainViewTopLeftBtnItem================================
MainViewTopLeftBtnItem = MainViewTopLeftBtnItem or DeclareMono("MainViewTopLeftBtnItem", UIWidgetBaseItem)

function MainViewTopLeftBtnItem:MainViewTopLeftBtnItem()
    self.rand_data = ActivityRandData.Instance
    self.data = {}
end

function MainViewTopLeftBtnItem:SetData(data)
    self:FlushBlankParent()
    self:FlushBtnItem(data)
    UnRegisterRemind(self.data.mod,self.handle)
    self.RedPoint:SetNum(0, true)
    self.Effect:SetActive(false)
    --危险！！！！！！！！！！！！！！！！！特殊逻辑可以用sub代替 不可在这加特殊处理
    --设置图标特效 无法另外挂载
    if not IsEmpty(data.effect) and ((data.effect_type == nil) or (data.effect_type and data.effect_type == 0))  then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = RunUntilTrue(function()
            UH.StopEffect(self.EffectTool,self.handle_effect)
            self.handle_effect = UH.PlayEffect(self.EffectTool,data.effect,self.Effect)
            self.Effect:SetActive(true)
            return true
        end)
    else
        UH.StopEffect(self.EffectTool,self.handle_effect)
        self.Effect:SetActive(false)
    end
    --注册红点
    if data.mod then
        UH.StopEffect(self.EffectTool, self.handle_hundredgod)
        self.Icon.gameObject:SetActive(true)
        UnRegisterRemind(self.data.mod,self.handle)
        self.handle = RegisterRemind(data.mod,function(num)
            self.RedPoint:SetNum(num, true)
            --危险！！！！！！！！！！！！！！！！！特殊逻辑可以用sub代替 不可在这加特殊处理
            if data.mod == Mod.HundredGodBoss then

                if num == 1 then
                    self.Icon.gameObject:SetActive(false)
                    self.handle_hundredgod = UH.PlayEffect(self.EffectTool, 6161096, self.Effect)
                else
                    UH.StopEffect(self.EffectTool, self.handle_hundredgod)
                    self.Icon.gameObject:SetActive(true)
                end
            else
                --危险！！！！！！！！！！！！！！！！！特殊逻辑可以用sub代替 不可在这加特殊处理
                if num == 1 then
                    if not IsEmpty(data.effect) and data.effect_type and data.effect_type then
                        UH.StopEffect(self.EffectTool,self.handle_effect)
                        self.handle_effect = UH.PlayEffect(self.EffectTool,data.effect,self.Effect)
                    end    
                end
            end
            --危险！！！！！！！！！！！！！！！！！特殊逻辑可以用sub代替 不可在这加特殊处理
            if data.mod == Mod.PeriodActivity then
                self.RedPoint:SetNum(PeriodActivityCtrl.Instance:GetFirstOpen(num),true)
            end
        end,true)
    else
        UnRegisterRemind(self.data.mod,self.handle)
        self.RedPoint:SetNum(0, true)
    end
    --注册图标自己的监听
    if data.mod_key and data.mod_key ~= 0 then
        if self.rand_data:IsRegister(data.mod_key) then
            if self.rand_data:IsHasOrigin(data.mod_key) then
                self.rand_data:UnRegisterCare(self.rand_handle)
                self.rand_handle = self.rand_data:RegisterCare(data.mod_key,function()
                    self:SetActive(self.rand_data:GetRegisterOpen(data.mod_key))
                end)
            end
            self:SetActive(self.rand_data:GetRegisterOpen(data.mod_key))
        else
            self:SetActive(self.rand_data:IsRandOpen(data))
        end
    else
        self.rand_data:UnRegisterCare(self.rand_handle)
    end
    --根据图标配置sub_name显示自己的sub在SubPanel挂载
    if self.SubPanel then
        self.SubPanel:SetSubParent(self.SubParent,data.sub_name)
    end
    --显示新标识
    if data.act_type and data.new_sign == 1 then
        self.Tips:SetActive(ActivityData.Instance:IsHasSign(data.act_type))
    else
        self.Tips:SetActive(false)
    end
    self:FlushTimeView(data.mod_key)
    --危险！！！！！！！！！！！！！！！！！特殊逻辑可以用sub代替 不可在这加特殊处理
    UH.StopEffect(self.EffectTool, self.handle_uieffect1)
    UH.StopEffect(self.EffectTool, self.handle_uieffect2)
    UH.StopEffect(self.EffectTool, self.handle_uieffect3)
    self:UIEffect(data.mod_key)
    UIWidgetBaseItem.SetData(self, data)
end

--危险！！！！！！！！！！！！！！！！！特殊逻辑可以用sub代替 不可在这加特殊处理
function MainViewTopLeftBtnItem:UIEffect(mod_key)
    if mod_key == Mod.PlanActivity.Main then
        self.handle_uieffect1 = UH.PlayEffect(self.EffectTool, 1000000001, self.Effect)
        self.Icon.gameObject:SetActive(false)
    elseif mod_key == Mod.MountainSea.Main then
        self.handle_uieffect2 = UH.PlayEffect(self.EffectTool, 1000000002, self.Effect)
        self.Icon.gameObject:SetActive(false)
    elseif mod_key == Mod.YunZe.Main then
        self.handle_uieffect3 = UH.PlayEffect(self.EffectTool, 1000000003, self.Effect)
        self.Icon.gameObject:SetActive(false)
    end
end

--刷新自己的倒计时Time用ActivityRandData.Instance:RegisterActCountDown注册
function MainViewTopLeftBtnItem:FlushTimeView(mod_key)
    self.Time:CloseCountDownTime()
    local count_down = ActivityRandData.Instance:GetActCountDown(mod_key)
    if count_down then
        count_down(self.Time)
    end
    self.Time:SetObjActive(count_down ~= nil)
end

function MainViewTopLeftBtnItem:SetActive(is_active)
    self:CheckHundredGod(is_active)
    self.gameObject:SetActive(is_active)
    if self.BlankItem then
        self.BlankItem:SetActive(is_active)
    end
end
--危险！！！！！！！！！！！！！！！！！特殊点击可以用sub代替 不可在这加特殊处理
--百神之师显示的时候要自动打开
function MainViewTopLeftBtnItem:CheckHundredGod(is_active)
    if self.data.mod_key == Mod.HundredGodBoss.Main and not self.gameObject.activeSelf and is_active == true then
        HundredGodBossData.Instance:SetFlyPos(self.gameObject:GetPosition(), true)
        self:FlushTimeView(self.data.mod_key)
    end
end

function MainViewTopLeftBtnItem:OnClick(data)
    if self.Tips and self.Tips.activeSelf then
        ActivityData.Instance:SetHasSign(data.act_type)
        self.Tips:SetActive(false)
    end
    --活动自己的前往方式用ActivityRandData.Instance:CustomClickHandle注册
    if self.rand_data:OnClickHandle(data.act_type) then
        return
    end
    --危险！！！！！！！！！！！！！！！！！特殊点击可以用sub代替 不可在这加特殊处理
    if data.mod_key == Mod.HundredGodBoss.Main then
        HundredGodBossData.Instance:SetFlyPos(self.gameObject:GetPosition(), false)
    end
    if GetView(data.view_name) ~= nil then
        ViewMgr:OpenView(GetView(data.view_name))
    else
        ViewMgr:OpenViewByKey(data.mod_key)
    end
    if data.position ~= 1 then
        MainOtherData.Instance:MutualViewFunc()
    end
end

--刷新图标名字和图标
function MainViewTopLeftBtnItem:FlushBtnItem(data)
    local base_func = self.rand_data:GetBaseItemData(data.mod_key)
    local base_data = base_func and base_func(data) or nil
    
    local base_name = base_data and base_data.text or data.text
    local sprite_name = base_data and base_data.sprite or data.sprite
    local world_num = DataHelper.GetStringWordNum(base_name)
    -- if string.len(data.text) > 9 then
    if world_num >= 4 then
        UH.SetText(self.Space,base_name)
        UH.SetText(self.Label)
    else
        UH.SetText(self.Label,base_name)
        UH.SetText(self.Space)
    end
    UH.SpriteName(self.Icon,sprite_name)
end

function MainViewTopLeftBtnItem:OnRelease()
    if self.handle then
        UnRegisterRemind(self.data.mod,self.handle)
        self.RedPoint:SetNum(0, true)
    end
    if self.rand_handle then
        self.rand_data:UnRegisterCare(self.rand_handle)
    end
    if self.SubPanel then
        self.SubPanel:SetRelease(self.data.sub_name)
    end
    if self.Tips then
        self.Tips:SetActive(false)
    end
    UH.StopEffect(self.EffectTool,self.handle_effect)
    self.Effect:SetActive(false)
    self:SetActive(false)
end

function MainViewTopLeftBtnItem:FlushBlankParent()
    if self.BlankItem and self.BlankParent then
        self.BlankItem:SetParent(self.BlankParent,false)
        self.BlankItem.transform:SetAsFirstSibling()
    end
end

--================================MainViewTopLeftBtnSubItem================================
MainViewTopLeftBtnSubItem = MainViewTopLeftBtnSubItem or DeclareMono("MainViewTopLeftBtnSubItem", UIWidgetBaseItem)

function MainViewTopLeftBtnSubItem:SetSubParent(SubParent,sub_name)
    if not StringIsEmpty(sub_name) and self[sub_name] then
        self[sub_name]:SetParent(SubParent, false)
        self[sub_name]:SetActive(true)
    end
end

function MainViewTopLeftBtnSubItem:SetRelease(sub_name)
    if not StringIsEmpty(sub_name) and self[sub_name] then
        self[sub_name]:SetParent(self.gameObject, false)
        self[sub_name]:SetActive(false)
    end
end