SealPromoteView = SealPromoteView or DeclareView("SealPromoteView","serveractivity/seal_promote_view",Mod.RandActivity.SealPromote)
VIEW_DECLARE_LEVEL(SealPromoteView, ViewLevel.L1)
function SealPromoteView:SealPromoteView()
    self.Board:SetData(self:GetType(), ActivityRandData.GetName(ACTIVITY_TYPE.RAND_SEAL_PROMOTE))
end

function SealPromoteView:LoadCallback()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_SEAL_PROMOTE,TimeType.Type_Special_4)
    local config = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SEAL_PROMOTE,Config.seal_promote_auto.probability_promote,"seq")
    for i=1,self.ImageList:Length() do 
        local pet_info = Pet.New({item_id = config[i].pets_id or 0})
        local scene_id = config[i].scene_id
        
        if config[i].scene_id == nil then
            local way_cfg = PetData.Instance:GetCatchWay(pet_info:CatchWay())
            if way_cfg and next(way_cfg) then
                local scene_cfg = MapData.Instance:GetMapShowData(way_cfg[1].catch_link)
                if scene_cfg then
                    pet_info.info.scene_id = scene_cfg.scene_id
                end
            end
        end
        self.ImageList[i]:SetData(pet_info)
        UH.SetText(self.PetNameList[i], pet_info:Name())
        UH.SpriteName(self.PetTypeList[i], PetData.PetLevel[pet_info.config.quality])
    end
end

function SealPromoteView:OnClickClose()
    ViewMgr:CloseView(SealPromoteView)
end

function SealPromoteView:OnClickEnter()
	ViewMgr:OpenViewByKey(Mod.Pet.PetBook)
    ViewMgr:CloseView(SealPromoteView)
end

function SealPromoteView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[45].desc)
end