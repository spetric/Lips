-- initialize data structures
-- call only once if needed
function ocv_InitFFI()
  -- contour data
  ffi.cdef[[typedef struct {int x; int y;} SocvPoint;]]  
  ffi.cdef[[typedef struct {int hierarchy[4]; unsigned int numPts; SocvPoint *points;} SocvContourPts;]] 
  -- homography data
  ffi.cdef[[typedef struct {int fType;	int  maxSrcKeys;	int  maxTgtKeys;	int mType;
            float matchParam;	unsigned int initMinMatches;	unsigned int minMatches;
            int hType;	bool exportImage;	bool addAlpha;	bool warpCrop;	int matches;
            int inliers;	int outliers;	int shiftX;	int shiftY;
            int bbTop;	int bbLeft;	int bbWidth;	int bbHeight;} SocvHomography;]]
end
-- set image (source, target, interm1...
function ocv_SetImage(setName, image)
  return Lua2Host:OpenCVSet(setName, image.Id) 
end
-- set OpenCv roi
function ocv_SetRoi(setName, inpRoi)
  local luaRoi;
  if inpRoi then
    luaRoi = ffi.new("TLuaRoi")
    luaRoi.left   = inpRoi.Left
    luaRoi.right  = inpRoi.Right
    luaRoi.top    = inpRoi.Top
    luaRoi.bottom = inpRoi.Bottom
  end  
  return Lua2Host:OpenCVSet(setName, luaRoi) 
end
-- create new DK and get id
function ocv_CreateDK()
  return Lua2Host:OpenCVGet("CreateDK")
end
-- set DK idx into lips engine 
function ocv_SetDK(idx)
  return Lua2Host:OpenCVSet("SetDkIdx", idx)
end
-- get last DK idx
function ocv_GetLastDKIdx()
  return Lua2Host:OpenCVGet("LastDKIdx")
end
-- clear DK
function ocv_ClearDK(idx)
  return Lua2Host:OpenCVSet("ClearDK", idx)
end
-- get last ocv exported image
function ocv_GetLastImageId(imageType)
  return Lua2Host:OpenCVGet("LastImageId")
end
function ocv_GetContoursNum()
  return Lua2Host:OpenCVGet("NumContours") 
end
function ocv_GetContour(idx)
  local contour = Lua2Host:OpenCVGet("Contour", tostring(idx))
  local retContour = nil
  if (contour ~= nil) then
      retContour = ffi.cast("SocvContourPts*", contour) 
      --retContour = {Hierarchy = data.hierarchy, NumPts = data.numPts, Points = data.points}
  end
  return retContour
end
-- set and process image
function ocv_SetAndProcess(procName, srcImg, tgtImag, ...)
  local rcSrc =  Lua2Host:OpenCVSet("source", srcImg.Id) 
  local rcTgt =  Lua2Host:OpenCVSet("target", tgtImg.Id)   
  if (rcSrc ~= 0 or rcTgt ~= 0) then
     return rcSrc + rcTgt
  end
  return Lua2Host:OpenCVProcess(procName, ...)   
end
-- process image 
function ocv_Process(procName, ...)
  return Lua2Host:OpenCVProcess(procName, ...)   
end
-- flip source image inplace 
function ocv_FlipSource(...)    
  local arg = {...}
  local nparams = select("#", ...)    
  local flag = "0" 
  if (nparams > 0) then
      flag = arg[1]
      if (nparams == 2) then
          local img = arg[2]
          local rcSrc =  Lua2Host:OpenCVSet("source", img.Id)     
      end      
  end
  return ocv_Process("__flip", "source", flag)      
end
-- flip target image inplace 
function ocv_FlipTarget(...)    
  local arg = {...}
  local nparams = select("#", ...)    
  local flag = "0" 
  if (nparams > 0) then
      flag = arg[1]
      if (nparams == 2) then
          local img = arg[2]
          local rcSrc =  Lua2Host:OpenCVSet("target", img.Id)     
      end 
  end
  return ocv_Process("__flip", "target", flag)            
  end
-- flip exported image inplace 
function ocv_FlipExport(...)    
  local arg = {...}
  local nparams = select("#", ...)    
  local flag = "0" 
  if (nparams > 0) then
      flag = arg[1]
  end  
  return ocv_Process("__flip", "export", flag)      
end
-- get homography data
function ocv_GetLastHomography()
  local homography = Lua2Host:OpenCVGet("HomographyData", "ptr")
  local retHomography = nil
  if (homography ~= nil) then
      retHomography = ffi.cast("SocvHomography*", homography) 
  end 
  return retHomography
end