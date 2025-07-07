ArenaMatchLastRecord = ArenaMatchLastRecord or DeclareView("ArenaMatchLastRecord", "arena_match/arena_match_last_record")
VIEW_DECLARE_MASK(ArenaMatchLastRecord, ViewMask.BgBlockCloseNoAnim)
function ArenaMatchLastRecord:ArenaMatchLastRecord()
    
end

function ArenaMatchLastRecord:LoadCallback(param_t)
    param_t = param_t or {}
    local rank_list = param_t.rank_list or {}
    local text = self:GetText(rank_list)
    UH.SetText(self.Text, text)
end

function ArenaMatchLastRecord:GetText(rank_list)
    local text = ""
    if TableIsEmpty(rank_list) then
        text = Language.ArenaMatch.LastRecord.Text1
    else
        text = Language.ArenaMatch.LastRecord.Text2
        local str = Language.ArenaMatch.LastRecord.Text3
        local txt = ""
        for i,v in pairs(rank_list) do
            local name = ArenaMatchData.Instance:GetArenaBySeq(v.platform_num).leitai_name or ""
            txt = txt .. string.format(str,name,v.rank)
        end
        text = string.format(text, txt)
    end
    local tab = RichTextHelper.ColorStr("ttttt", COLORSTR.Transparent)
    text = tab .. text
    return text
end