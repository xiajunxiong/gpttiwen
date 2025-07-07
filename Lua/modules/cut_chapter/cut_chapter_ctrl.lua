VIEW_REQ("modules/cut_chapter/cut_chapter_view")
VIEW_REQ("modules/cut_chapter/cut_blackwhite_view")

CutChapterCtrl = CutChapterCtrl or BaseClass(BaseCtrl)

function CutChapterCtrl:__init()
	if CutChapterCtrl.Instance ~= nil then
		Debuger.LogError("[CutChapterCtrl] attempt to create singleton twice!")
		return
    end
    
    CutChapterCtrl.Instance = self
	
	self.check_pos = -1
end

function CutChapterCtrl:SetCheckPos(_pos) 
	self.check_pos = _pos
end 

function CutChapterCtrl:CleanCheckPos() 
	self.check_pos = -1
end 

function CutChapterCtrl:GetCutChapterWorks()
	if self.check_pos == -1 then return nil end

	local pos_info = nil
	for k,v in pairs(Config.function_guide_auto.guochang_describe) do 
		if v.seq == self.check_pos then 
			pos_info = v
		end 
	end
	
	if pos_info == nil then return nil end 
	
	local string_list = nil
	if pos_info.type == 1 then 
		string_list = {}
		local data_string = Split(pos_info.describe, "#")
		local vo_end = {}
		vo_end.desc = ""
		vo_end.is_end =  true
		table.insert( string_list, vo_end)
		for k,v in pairs(data_string) do 
			local vo = {}
			vo.desc = v
			vo.is_end = false
			if v == "|" then 
				vo.desc = " "
			end 
	
			table.insert( string_list, vo)
		end 
	else 
		string_list = ""
		local data_string = Split(pos_info.describe, "#")
		for k,v in pairs(data_string) do 
			string_list = string_list .. v
			if k < #data_string then 
				string_list = string_list .. "\n"
			end 
		end 
	end 
	return string_list
end 

function CutChapterCtrl:CutChapterCloseFunc()
	if self.check_end_func then
		self.check_end_func()
		self.check_end_func = nil
	end
end

--外部打开断章接口
function CutChapterCtrl:OpenCutChapter(pos,end_func)
	self.check_pos = pos
	self.check_end_func = end_func
	ViewMgr:OpenView(CutBlackWhiteView)
end