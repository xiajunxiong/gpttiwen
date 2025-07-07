LingBaoGetView = LingBaoGetView or DeclareView("LingBaoGetView", "ling_bao/ling_bao_get")
VIEW_DECLARE_MASK(LingBaoGetView, ViewMask.BgBlockClose)
function LingBaoGetView:LingBaoGetView()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
end

function LingBaoGetView:LoadCallback(param_t)
    local item = Item.Init(param_t.item_id, 1, true)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ComposeSucc)

    self.Cell:SetData(item)
    UH.SetText(self.TxtName, item:Name())
    UH.SetText(self.State, Format(self.language.GetShowState, DataHelper.GetDaXie(item:StateLevel()), item:StarLevel()))
    self.AttrList:SetData(item:BaseAttr())
    UH.SetText(self.TxtDesc, item:Desc())
end
