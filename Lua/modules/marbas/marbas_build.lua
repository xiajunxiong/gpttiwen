MarbasBuild = MarbasBuild or DeclareMono("MarbasBuild", UIWFlushPanel)
function MarbasBuild:MarbasBuild()
    self.Data = MarbasData.Instance
    self.language = Language.Marbas.Build
    self.abuild_data = self.Data:ABuildData()
    self.Data:ResetBuildInfo()
    self.data = self.Data.build_info
    self.data_cares = {
        {data = self.data, func = self.FlushAll, keys = {"prof_type"}, init_call = false},
        {data = self.data, func = self.FlushRight, keys = {"equip_grade", "level"}, init_call = false},
        {data = self.Data.build_remind, func = self.FlushAll, init_call = false},
    }
    -- 刷新道具数量
    if self.item_listen == nil then
        self.item_listen = BagData.Instance:ListenItems(self.Data:BuildRemindItemList(), BindTool.Bind(self.FlushAll, self))
    end
end

function MarbasBuild:Awake()
    UIWFlushPanel.Awake(self)
    self.ALevelList:SetCreateCallBack(function()
        self.is_a_complete = true
        self:FlushARemind()
        self:FlushRight()
    end)
    self.ALevelList:SetData(self.abuild_data)
    self:OnProfCloseClick()
    self:FlushAll()
end

function MarbasBuild:FlushAll()
    self.ObjBlock:SetActive(false)
    UH.SetText(self.TxtProf, Language.Common.Prof[RoleData.Instance:GetRoleProf(self.data.prof_type)])

    self.TCanBuild.isOn = PrefsInt(PrefKeys.MarbasBuildCanBuild()) == 1
    self.TNotShowRemind.isOn = self.Data:PrefBuildRemind() == 1

    -- 这里需要创建完手风琴才能刷新
    if self.is_a_complete then
        self:FlushARemind()
        self:FlushRight()
    end
end

function MarbasBuild:FlushRight()
    if not self.is_a_complete then
        return
    end
    local right_data = self.Data:BuildData(self.data.prof_type, self.data.equip_grade, self.data.level, nil, self.TCanBuild.isOn)
    self.ObjHas:SetActive(not TableIsEmpty(right_data))
    self.List:SetData(right_data)
end

function MarbasBuild:FlushARemind()
    if not self.is_a_complete then
        return
    end
    for k, v in pairs(self.abuild_data) do
        local parent = self.ALevelList:GetParentMono(v)
        parent:FlushRemind()
        for l, w in pairs(v.children) do
            local child = self.ALevelList:GetChildMono(w)
            child:FlushRemind()
        end
    end
end

function MarbasBuild:OnProfClick()
    self.ObjArrow:SetActive(false)
    if not self.first_prof then
        self.first_prof = true
        self.ProfList:SetData(self.Data:BuildProfData())
    end
    self.ObjProfShow:SetActive(true)
end

function MarbasBuild:OnProfCloseClick()
    self.ObjArrow:SetActive(true)
    self.ObjProfShow:SetActive(false)
end

function MarbasBuild:OnProfItemClick(data)
    self.data.prof_type = data.prof_type
    self:OnProfCloseClick()
end

function MarbasBuild:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    BagData.Instance:RemoveListenItems(self.item_listen)
    TimeHelper:CancelTimer(self.ht)
end

function MarbasBuild:OnAChildItemClick(data)
    self.data.level = data.name
    self.data.equip_grade = data.parent_index
    self.ALevelList:OnClickChild(data)
end

function MarbasBuild:OnTCanBuildClick(value)
    PrefsInt(PrefKeys.MarbasBuildCanBuild(), value and 1 or 0)
    self:FlushRight()
end

function MarbasBuild:OnTNotShowRemindClick(value)
    self.Data:PrefBuildRemind(value and 1 or 0)
end

-- 点击打造
function MarbasBuild:OnBuildClickClick(mono)
    local data = mono:GetData()
    local flag, item_id = self.Data:IsCanBuild(data.seq)
    if flag then
        local param_t = {
            content = string.format(
                self.language.BuildTip,
                QualityColorStr[Item.GetColor(data.target)],
                Item.GetName(data.target)
            ),
            confirm = {
                func =  function()
                    self.ObjBlock:SetActive(true)
                    mono:PlayEffect()
                    self.ht = Invoke(function()
                        if data.stuff_num_1 > 0 then
                            -- 身上有并且装备正确，就用身上的，否则消耗背包的
                            if self.Data:IsEquipThis(data.stuff_id_1) then
                                local cfg = Item.GetConfig(data.stuff_id_1)
                                MarbasCtrl.Instance:SendEquipOper(MarbasReqType.Build, data.seq, 1, cfg.equip_type)
                            else
                                MarbasCtrl.Instance:SendEquipOper(MarbasReqType.Build, data.seq, 0, self.Data:BagOneIndex(data.stuff_id_1))
                            end
                        else
                            MarbasCtrl.Instance:SendEquipOper(MarbasReqType.Build, data.seq)
                        end
                        mono:StopEffect()
                    end, 1)
                    ViewMgr:CloseView(DialogTipsView)
                end 
            },
            new_tnr = DTNewTodayNotRemind.MarbasBuild
        }
        PublicPopupCtrl.Instance:DialogTips(param_t)
    else
        if item_id == CommonItemId.Coin then
            PublicPopupCtrl.Instance:AlertTip(Language.Mall.NotCurrency[2],function()
				ShoppingCtrl.Instance:OpenStoreView(0)
			end)
        else
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = item_id,param = {show_way = 1}})})
        end
    end
end

--------------MarbasBuildProfItem--------------
MarbasBuildProfItem = MarbasBuildProfItem or DeclareMono("MarbasBuildProfItem", UIWidgetBaseItem)
function MarbasBuildProfItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, data.name)
end

--------------MarbasBuildItem--------------
MarbasBuildItem = MarbasBuildItem or DeclareMono("MarbasBuildItem", UIWidgetBaseItem)
function MarbasBuildItem:MarbasBuildItem()
    self.Data = MarbasData.Instance
    self.language = Language.Marbas.Build
end

function MarbasBuildItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.item_target = Item.Create({item_id = data.target})

    local build_cell_data = {}
    build_cell_data.is_equip = true
    build_cell_data.show_has = MarbasData.Instance:ItemNum(data.target) > 0
    build_cell_data.item = Item.Init(data.target, 1, 1)
    self.BuildCell:SetData(build_cell_data)
    UH.SetText(self.TxtName, self.item_target:QuaName())
    UH.SetText(self.TxtPart, Format(self.language.Part, self.item_target:EquipTypeDesc()))
    UH.SetText(self.TxtPrice, data.price)
    self.List:SetData(self:ListData())

    self.IHandler.Interactable = MarbasData.Instance:IsCanBuild(data.seq)
    self.Remind:SetNum(self.Data:PrefBuildRemind() == 0 and MarbasData.Instance:IsSelectMyProf() and self.IHandler.Interactable and 1 or 0)
end

function MarbasBuildItem:ListData()
    local t = {}
    if self.data.stuff_num_1 > 0 then
        local tt = {}
        tt.is_equip = true
        tt.show_equip = MarbasData.Instance:IsEquipThis(self.data.stuff_id_1)
        if tt.show_equip then
            tt.item = Item.Init(self.data.stuff_id_1, 1, 1)
        else
            local num = DataHelper.ConsumeNum(MarbasData.Instance:ItemNum(self.data.stuff_id_1), self.data.stuff_num_1, true)
            tt.item = Item.Init(self.data.stuff_id_1, num, 1)
        end
        table.insert(t, tt)
    end
    if self.data.stuff_num_2 > 0 then
        local tt = {}
        local num = DataHelper.ConsumeNum(Item.GetNum(self.data.stuff_id_2), self.data.stuff_num_2, true)
        tt.item = Item.Init(self.data.stuff_id_2, num, 1)
        table.insert(t, tt)
    end
    return t
end

function MarbasBuildItem:OnShowClick()
    local info = {
		item_list = {self.item_target, MarbasData.Instance:MarbasEquip(self.item_target:EquipType())},
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function MarbasBuildItem:PlayEffect()
    self:StopEffect()
    self.effect = self.EffectTool:Play(3165127)
end

function MarbasBuildItem:StopEffect()
    if self.effect then
        self.EffectTool:Stop(self.effect)
        self.effect = nil
    end
end

--------------MarbasBuildCellItem--------------
MarbasBuildCellItem = MarbasBuildCellItem or DeclareMono("MarbasBuildCellItem", UIWidgetBaseItem)
function MarbasBuildCellItem:MarbasBuildCellItem()
    
end

function MarbasBuildCellItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data.item)
    self.ObjWared:SetActive(data.show_equip == true)
    self.ObjGeted:SetActive(data.show_has == true)
end

--------------MarbasBuildAParentItem--------------
MarbasBuildAParentItem = MarbasBuildAParentItem or DeclareMono("MarbasBuildAParentItem", UIWidgetBaseItem)

function MarbasBuildAParentItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.LblTitle, data.name)
end

function MarbasBuildAParentItem:FlushRemind()
    if MarbasData.Instance:IsSelectMyProf() then
        self.Remind:SetNum(MarbasData.Instance:BuildGradeRemind(self.data.id))
    else
        self.Remind:SetNum(0)
    end
end

--------------MarbasBuildAChildItem--------------
MarbasBuildAChildItem = MarbasBuildAChildItem or DeclareMono("MarbasBuildAChildItem", UIWidgetBaseItem)

function MarbasBuildAChildItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, Format(Language.Marbas.Build.ChildName, data.name))
end

function MarbasBuildAChildItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function MarbasBuildAChildItem:FlushRemind()
    if MarbasData.Instance:IsSelectMyProf() and self.data.is_show == 1 then
        self.Remind:SetNum(MarbasData.Instance:BuildLevelRemind(self.data.parent_index, self.data.name))
    else
        self.Remind:SetNum(0)
    end
end

