-- 选择宠物附魂
YouMingPetSelectView = YouMingPetSelectView or DeclareView("YouMingPetSelectView", "youming/youming_pet_select_view")
VIEW_DECLARE_MASK(YouMingPetSelectView,ViewMask.BlockClose)
function YouMingPetSelectView:YouMingPetSelectView()
    self.Board:SetData(self:GetType(),Language.Bicycle.ChileViewName.PetSelect,Vector2.New(384, 637))
end

function YouMingPetSelectView:LoadCallback(param)
    local pos = param.pos
    if pos then
        UH.LocalPos(self.PosRect,pos)
    end
end


YouMingSelectPetPanel = YouMingSelectPetPanel or DeclareMono("YouMingSelectPetPanel", UIWFlushPanel)
function YouMingSelectPetPanel:YouMingSelectPetPanel()
    self.data = YouMingData.Instance
    self.data_cares = {
        {data = self.data.bless_pet_list, func = self.FlushPanel, init_call = false},
    }
end

function YouMingSelectPetPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function YouMingSelectPetPanel:FlushPanel()
    local youming = self.data:GetSelectYouMing()
    if not youming then
        return
    end
    local pet_list = self.data:GetPetList(youming)
    self.EmptyState:SetActive(TableIsEmpty(pet_list))
    self.PetList:SetData(pet_list)
    local pet = youming:BePet()
    local count = pet == nil and 0 or 1
    UH.SetText(self.Tip,string.format(Language.YouMing.Text1,count,1))
end

function YouMingSelectPetPanel:OnOkClick()
    ViewMgr:CloseView(YouMingPetSelectView)
end

YouMingPetSelectPetItem = YouMingPetSelectPetItem or DeclareMono("YouMingPetSelectPetItem",UIWidgetBaseItem)
-- data == pet
function YouMingPetSelectPetItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.PetItem:SetData(data)
    UH.SetText(self.Level,string.format(Language.Bicycle.Level,data:Level()))
    UH.SetText(self.Name,data:Name())
    if data:YouMing() then
        self.is_on = true
        self.ToggleFlag:SetActive(true)
    else
        self.is_on = false
        self.ToggleFlag:SetActive(false)
    end
end

function YouMingPetSelectPetItem:OnToggleClick()
    local youming = YouMingData.Instance:GetSelectYouMing()
    if not youming then
        return
    end
    if not self.is_on then
        YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.POSSESS, youming.index, self.data.index)
    else
        youming = self.data:YouMing()
        if youming then
            YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.CANCEL_POSSESS, youming.index)
        end
    end
end