
NpcDialogSync = NpcDialogSync or DeclareMono("NpcDialogSync", UIWFlushPanel)

function NpcDialogSync:NpcDialogSync()
    self.data = NpcDialogCtrl.Instance:Data()

    self.data_cares = {
        {data = self.data.sync_info, func = self.FlushInfo, init_call = false},
    }
end

function NpcDialogSync:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_dt)
end

function NpcDialogSync:FlushInfo()
    TimeHelper:CancelTimer(self.timer_dt)
    local talk_id = self.data.sync_info.talk_id or 0
    if talk_id > 0 then
        local talk_seq = self.data.sync_info.talk_seq or 1
        if self.data.npc_talk_list_config[talk_id] then
            local co = self.data.npc_talk_list_config[talk_id][talk_seq]
            if co then
                if 0 == co.talker then
                    if TeamData.Instance:InTeam() then
                        local leader_info = TeamData.Instance:GetLeaderInfo()
                        if leader_info then
                            self.NameShow.text = leader_info.name
                            UH.SetAvatar(self.IconShow, leader_info.appearance.avatar_type, leader_info.appearance.avatar_id)
                            UIWPetHeadCell.EffAnimName(leader_info.appearance.avatar_quality, self.FSQuaAnim)
                        end
                    end
                else
                    local npc_co = NpcHelper.NpcCo(co.talker)
                    if npc_co then
                        self.NameShow.text = npc_co.name
                    end
                    UH.SetIcon(self.IconShow, npc_co.npc_icon, ICON_TYPE.ITEM)
                    UIWPetHeadCell.EffAnimName(0, self.FSQuaAnim)
                end
                self.ContentShow.text = RichTextHelper.ParseAllFunc(co.talk_text, true, RichTextFormat.FormatNpcBubbleSpecial)
            end
        end
    end
    self.ShowObj:SetActive(talk_id > 0)
    if talk_id > 0 then
        self.timer_dt = TimeHelper:AddDelayTimer(BindTool.Bind(self.data.NpcDialogSyncBlock, self.data), 5)
    end
end