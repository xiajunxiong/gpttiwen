JoySeeksFavorsTip = JoySeeksFavorsTip or 
DeclareView("JoySeeksFavorsTip", "serveractivity/joy_seeks_favors_tip")
function JoySeeksFavorsTip:OnFlush()
    UH.SetText(self.Desc1, Config.language_cfg_auto.textdesc[62].desc)
    UH.SetText(self.Desc2, Config.language_cfg_auto.textdesc[63].desc)
end
function JoySeeksFavorsTip:OnClickClose()
    ViewMgr:CloseView(JoySeeksFavorsTip)
end