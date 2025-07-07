FileTool = {}
local systemIOFile = System.IO.File
local systemIOPath = System.IO.Path
local systemIODir = System.IO.Directory
function FileTool.Exists(path)
	return systemIOFile.Exists(path)
end

function FileTool.ReadAllText(path)
	return systemIOFile.ReadAllText(path);
end

function FileTool.WriteAllText(path,contents)
	systemIOFile.WriteAllText(path,contents)
end

function FileTool.Delete(path)
	systemIOFile.Delete(path)
end

function FileTool.Move(from_path,to_path)
	systemIOFile.Move(from_path,to_path)
end

function FileTool.GetFileName(path)
	return systemIOPath.GetFileName(path)
end

function FileTool.DirExists(dir)
	return systemIODir.Exists(dir)
end

function FileTool.CreateDir(dir)
	systemIODir.CreateDirectory(dir)
end

function FileTool.GetDirName(path)
	return systemIOPath.GetDirectoryName(path)
end