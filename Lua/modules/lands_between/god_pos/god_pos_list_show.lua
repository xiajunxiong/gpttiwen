GodPosListShowView = GodPosListShowView or DeclareView("GodPosListShowView", LandsBetweenConfig.PrefabPath.."god_pos/god_pos_list_show")
VIEW_DECLARE_MASK(GodPosListShowView, ViewMask.BgBlock)
function GodPosListShowView:GodPosListShowView()
    self.Board:SetData(self:GetType(), Language.GodPos.Title[2], Vector2.New(530, 590))
end

function GodPosListShowView:LoadCallback(param_t)
    
end

function GodPosListShowView:CloseCallback()

end

function GodPosListShowView:OnCloseClick()
    ViewMgr:CloseView(GodPosListShowView)
end

----------------------------GodPosListShowPanel----------------------------
GodPosListShowPanel = GodPosListShowPanel or DeclareMono("GodPosListShowPanel", UIWFlushPanel)
function GodPosListShowPanel:GodPosListShowPanel()
    self.Data = GodPosData.Instance
    self.language = Language.GodPos
    self.data = self.Data.info_data
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
    }
end

function GodPosListShowPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushAll()
end

function GodPosListShowPanel:FlushAll()
    self.now_show_seq = self.Data:PosSeq()
    self.list_data, self.jump_data = self.Data:GetGodPosData()
    self.List:SetData(self.list_data)
    self.List:AutoSlide(self.jump_data, false)
end

function GodPosListShowPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

--------------GodPosListShowItem--------------
GodPosListShowItem = GodPosListShowItem or DeclareMono("GodPosListShowItem", UIWidgetBaseItem)
function GodPosListShowItem:GodPosListShowItem()
    self.Data = GodPosData.Instance
    self.language = Language.GodPos
end

function GodPosListShowItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ObjGeted:SetActive(self.Data:IsRewardGeted(data.class_seq))
    self.ObjCannotGet:SetActive(data.class_seq > self.Data:PosSeq())
    UH.SpriteName(self.Icon, "ShenWei" .. data.class_seq)
    UH.SetText(self.TxtName, data.name)
    UH.SetText(self.TxtName2, Format(self.language.NameDesc, data.name))
end

function GodPosListShowItem:OnBoxClick()
    ViewMgr:OpenView(GodPosRewardShowView, {item_list = self.data.rewards})
end