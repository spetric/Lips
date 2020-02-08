-- Prolog functions. Modify with precaution.
-- input/output color channel order 0,1,2 => RGB, 2,1,0 => BGR 
local _chor, _chog, _chob = 2, 1, 0
function prolog0()
   -- create lua to host global object and ffi structures
   Lua2Host = TLuaHostWrap(cppHost) 
   ffi = require("ffi")
   ffi.cdef[[typedef struct { unsigned char ch[3];} TTypeByte3;]] -- 3 byte channels per pixel (usually rgb or bgr)
   ffi.cdef[[typedef struct { unsigned char ch[1];} TTypeByte1;]] -- 1 byte channel per pixel (grayscale image or alpha channel)   
   ffi.cdef[[typedef struct { float ch[3];} TTypeFloat3;]]        -- 3 float channels per pixel (for various color spaces)        
   ffi.cdef[[typedef struct { int id; int channels; int width; int height; int imageStride; int alphaStride; int align; int orientation; TTypeByte3 **plane; TTypeByte1 **alpha;} TLuaImageByte3;]]
   ffi.cdef[[typedef struct { int id; int channels; int width; int height; int imageStride; int alphaStride; int align; int orientation; TTypeByte1 **plane; TTypeByte1 **alpha;} TLuaImageByte1;]]  
   ffi.cdef[[typedef struct { int id; int channels; int width; int height; int imageStride; int alphaStride; int align; int orientation; TTypeFloat3 **plane; TTypeByte1 **alpha;} TLuaImageFloat3;]]   
   ffi.cdef[[typedef struct { int left; int right; int top; int bottom;} TLuaRoi;]]        -- 3 float channels per pixel (for various color spaces)
end
-- local functions
local lfGetImage = function(imgType, ptr)
  local image, data     
    if imgType == "rgb24" then
       data = ffi.cast("TLuaImageByte3*", ptr)
       image = {Id = data.id, Channels = data.channels, Width = data.width, Height = data.height, Plane = data.plane, Alpha = data.alpha}
    elseif imgType == "grayscale" then 
       data = ffi.cast("TLuaImageByte1*", ptr)
       image = {Id = data.id, Channels = data.channels, Width = data.width, Height = data.height, Plane = data.plane, Alpha = data.alpha}
    elseif imgType == "float3" then
       data = ffi.cast("TLuaImageFloat3*", ptr)
       image = {Id = data.id, Channels = data.channels, Width = data.width, Height = data.height, Plane = data.plane, Alpha = data.alpha}  
    else
       return nil   
    end     
return image
end
function prolog1(src24, tgt24, srcRoi, tgtRoi, src8, tgt8, cust24, cust8, sParams)
   -- get image data
   local data, roiRect
   if (src24 == nil) then
      SourceImage = {Id = 0, Channels = 0, Width = 0, Height = 0, Plane = nil, Alpha = nil}
   else
      SourceImage = lfGetImage("rgb24", src24)
   end
   if (tgt24 == nil) then
      TargetImage = {Id = 0, Channels = 0, Width = 0, Height = 0, Plane = nil, Alpha = nil}
   else
      TargetImage = lfGetImage("rgb24", tgt24) 
   end
   if (srcRoi == nil) then
      SourceRoi = {Left = 0, Right = SourceImage.Width - 1, Top = 0, Bottom = SourceImage.Height - 1}   
   else
      roiRect = ffi.cast("TLuaRoi*", srcRoi)   
      SourceRoi = {Left = roiRect.left, Right = roiRect.right - 1, Top = roiRect.top, Bottom = roiRect.bottom - 1}
   end   
   if (tgtRoi == nil) then
      TargetRoi = {Left = 0, Right = TargetImage.Width - 1, Top = 0, Bottom = TargetImage.Height - 1}   
   else
      roiRect = ffi.cast("TLuaRoi*", tgtRoi)   
      TargetRoi = {Left = roiRect.left, Right = roiRect.right - 1, Top = roiRect.top, Bottom = roiRect.bottom - 1}
   end     
   if (src8 == nil) then
      SourceMask = {Id = 0, Channels = 0, Width = 0, Height = 0, Plane = nil, Alpha = nil}
   else
      SourceMask = lfGetImage("grayscale", src8)       
   end 
   if (tgt8 == nil) then
      TargetMask = {Id = 0, Channels = 0, Width = 0, Height = 0, Plane = nil, Alpha = nil}
   else
      TargetMask = lfGetImage("grayscale", tgt8)       
   end  
   if (cust24 == nil) then
      CustomImage = {Id = 0, Channels = 0, Width = 0, Height = 0, Plane = nil, Alpha = nil}
   else
      CustomImage = lfGetImage("rgb24", cust24)       
   end   
   if (cust8 == nil) then
      CustomMask = {Id = 0, Channels = 0, Width = 0, Height = 0, Plane = nil, Alpha = nil}
   else
      CustomMask = lfGetImage("grayscale", cust8)       
   end   
   -- Silent params var
   SilentParams = sParams
   -- set common roi for both images
   --Roi = {Left = SourceRoi.Left, Right = SourceRoi.Right, Top = SourceRoi.Top, Bottom = SourceRoi.Bottom}   
   --Roi = {Left = TargetRoi.Left, Right = TargetRoi.Right, Top = TargetRoi.Top, Bottom = TargetRoi.Bottom} 
   Roi = {Left = math.max(SourceRoi.Left, TargetRoi.Left), Right = math.min(SourceRoi.Right, TargetRoi.Right), 
                Top = math.max(SourceRoi.Top, TargetRoi.Top), Bottom = math.min(SourceRoi.Bottom, TargetRoi.Bottom)}
   -- set additional global vars
   errTargetEmpty = "Error: target image is empty!"
   errOcvDisabled = "Error: OpenCV support disabled!"
   OnMouseEvents = {}
   MouseState = {["ssShift"] = false, ["ssAlt"] = false, ["ssCtrl"] = false, ["ssLeft"] = false, ["ssRight"] = false, ["ssMiddle"] = false} 
   -- execute main function
   main()
end
-- host functions block (additional functions called from host app)
function hostSetRoi(roi, roiType) -- force respective roi global var
   if (roi == nil) then
      roiRect = {Left = 0, Right = 0, Top = 0, Bottom = 0}   
   else
      roiRect = ffi.cast("TLuaRoi*", roi)   
   end         
   if (roiType == 0) then
      SourceRoi = {Left = roiRect.left, Right = roiRect.right - 1, Top = roiRect.top, Bottom = roiRect.bottom - 1}
   else     
      TargetRoi = {Left = roiRect.left, Right = roiRect.right - 1, Top = roiRect.top, Bottom = roiRect.bottom - 1}
   end  
end
-- lua functions block (functions called from lua)
function lips_ShowProgress(done, total)
  Lua2Host:ShowProgress(done, total)                 
end
function lips_SendMessage(msg_str)
  Lua2Host:SendMessage(msg_str)
end
function lips_RequireParams(par_str)
  Lua2Host:RequireParams(par_str)
end
function lips_RefreshParams()
  Lua2Host:RefreshParams()
end
function lips_HostDialog(par_str, par_int)
  local rc = Lua2Host:HostDialog(par_str, par_int)
  while (rc == nil) do
  end  
  return rc 
end
function lips_SendCommand(cmd_str, ...)
  return Lua2Host:SendCommand(cmd_str, ...) 
end
-- Is pixel inside image boundaries
--[[
function lips_IsPixelSafe(image, i, j)
  if (i < 0 or i >= image.Height or j < 0 or j >= image.Width) then
     return false
  end      
  return true
end 
]]--
-- Get channels value from an image 
function lips_GetPixel(image, i, j)
   if (image.Channels == 3) then  
      return image.Plane[i][j].ch[_chor], image.Plane[i][j].ch[_chog], image.Plane[i][j].ch[_chob] 
   else
      return image.Plane[i][j].ch[0]
   end         
end
-- Get channels value from an image (safe mode - return zeros if beyond boundaries)
function lips_GetPixelSafe(image, i, j)   
    if (image.Channels == 3) then
        if (i < 0 or i >= image.Height or j < 0 or j >= image.Width) then
          return 0, 0, 0
        end
        return image.Plane[i][j].ch[_chor], image.Plane[i][j].ch[_chog], image.Plane[i][j].ch[_chob]
    else
        if (i < 0 or i >= image.Height or j < 0 or j >= image.Width) then
          return 0
        end
        return image.Plane[i][j].ch[0]      
    end  
end
-- Get channels value from an image (wrap mode - return wrapped values if beyond boundaries)
function lips_GetPixelWrap(image, i, j)    
  local wrap_i = i % image.Height
  local wrap_j = j % image.Width
  if (wrap_i < 0) then
	   wrap_i = image.Height + wrap_i
  end   
  if (wrap_j < 0) then
	   wrap_j = image.Width +  wrap_j
  end   
  if (image.Channels == 3) then
      return  image.Plane[wrap_i][wrap_j].ch[_chor],
              image.Plane[wrap_i][wrap_j].ch[_chog],
              image.Plane[wrap_i][wrap_j].ch[_chob]             
  else
      return image.Plane[wrap_i][wrap_j].ch[0]  
  end         
end
-- Set channels value for an image
function lips_SetPixel(image, i, j, v1, v2, v3)
   if (image.Channels == 3) then
      image.Plane[i][j].ch[_chor] = v1
      image.Plane[i][j].ch[_chog] = v2
      image.Plane[i][j].ch[_chob] = v3
   else
      image.Plane[i][j].ch[0] = v1  
   end         
end
-- Set channels value for an image (safe mode)
function lips_SetPixelSafe(image, i, j, v1, v2, v3)
    if (i < 0 or i >= image.Height or j < 0 or j >= image.Width) then
       return
    end
    if (image.Channels == 3) then  
         image.Plane[i][j].ch[_chor] = v1
         image.Plane[i][j].ch[_chog] = v2
         image.Plane[i][j].ch[_chob] = v3                   
    else
         image.Plane[i][j].ch[0] = v1              
    end
end
-- Get luminance value from 24-bit image
function lips_GetLuminance(image, i, j)
   local luminance = image.Plane[i][j].ch[_chob]*0.11 + image.Plane[i][j].ch[_chog] * 0.59 + image.Plane[i][j].ch[_chor] * 0.30
   return luminance/255.0
end
-- Get luminance value from 24-bit image (safe mode)   
function lips_GetLuminanceSafe(image, i, j)
  if (i < 0 or i >= image.Height or j < 0 or j >= image.Width) then
     return 0.0
  end
  local luminance = image.Plane[i][j].ch[_chob]*0.11 + image.Plane[i][j].ch[_chog] * 0.59 + image.Plane[i][j].ch[_chor] * 0.30
  return luminance/255.0       
end
-- normalize rgb colors
function lips_NormalizeRGB(r, g, b)
  if r < 0 then r = 0
  elseif r > 255 then r = 255
  end
  if g < 0 then g = 0
  elseif g > 255 then g = 255
  end
  if b < 0 then b = 0
  elseif b > 255 then b = 255
  end
  return r, g, b      
end   
-- Create new HOST image 
-- Note: images are released on the HOST side after script execution
function lips_CreateImage(imgType, width, height)
    local ptr = Lua2Host:CreateImage(imgType, width, height)
    if ptr == nil then
       return nil
    else
      return lfGetImage(imgType, ptr)     
    end
end
-- Load HOST image 
-- Note: images are released on the HOST side after script execution 
-- Ask host to load an image and convert it to desired format
function lips_LoadImage(filename, outputType)
    local ptr = Lua2Host:LoadImage(filename, outputType)
    if ptr == nil then
       return nil
    else
      return lfGetImage(outputType, ptr)     
    end
end
-- Export image to HOST
-- Tell HOST to create and show a bitmap from internal image
function lips_ExportImage(image, asWhat)  
    local lRet = Lua2Host:ExportImage(image.Id, asWhat)
    return lRet
end
-- get image from pointer
function lips_GetImage(iType, iPtr)
  if iPtr == nil then
    return nil
  else
    return lfGetImage(iType, iPtr)         
  end
end
-- get image from id
function lips_GetImageFromId(id)
  local iType, iPtr = Lua2Host:GetImage(id)
  if iPtr == nil then
    return nil
  else
    return lfGetImage(iType, iPtr)         
  end
end
function lips_SetRoi(left, right, top, bottom)
  local roi = {Left = left, Right = right, Top = top, Bottom = bottom}
  return roi
end  
function lips_CopyImage(inpImage, outImage, inpRoi, outRoi)
  local srcRoi, dstRoi
  if inpRoi then
    srcRoi = ffi.new("TLuaRoi")
    srcRoi.left   = inpRoi.Left
    srcRoi.right  = inpRoi.Right
    srcRoi.top    = inpRoi.Top
    srcRoi.bottom = inpRoi.Bottom
  end
  if outRoi then
    dstRoi = ffi.new("TLuaRoi")
    dstRoi.left   = outRoi.Left
    dstRoi.right  = outRoi.Right
    dstRoi.top    = outRoi.Top
    dstRoi.bottom = outRoi.Bottom
  end 
  return Lua2Host:CopyImage(inpImage.Id, outImage.Id, srcRoi, dstRoi) 
end
-- color space conversions
function lips_Convert_rgb2grayscale(inpImage, outImage)
  return Lua2Host:ConvertColorSpace(inpImage.Id, outImage.Id, "rgb2grayscale") 
end
-- rgb to xyz
function lips_Convert_rgb2xyz(inpImage, outImage)
  if (inpImage.Channels ~= 3 and outImage.Channels ~= 3) then 
     return false    
  end
  return Lua2Host:ConvertColorSpace(inpImage.Id, outImage.Id, "rgb2xyz") 
end    
-- xyz to rgb
function lips_Convert_xyz2rgb(inpImage, outImage)
  if (inpImage.Channels ~= 3 and outImage.Channels ~= 3) then 
     return false    
  end
  return Lua2Host:ConvertColorSpace(inpImage.Id, outImage.Id, "xyz2rgb") 
end    
-- rgb to lab
function lips_Convert_rgb2lab(inpImage, outImage)
  if (inpImage.Channels ~= 3 and outImage.Channels ~= 3) then 
     return false    
  end
  return Lua2Host:ConvertColorSpace(inpImage.Id, outImage.Id, "rgb2lab") 
end    
-- lab to rgb
function lips_Convert_lab2rgb(inpImage, outImage)
  if (inpImage.Channels ~= 3 and outImage.Channels ~= 3) then 
     return false    
  end
  return Lua2Host:ConvertColorSpace(inpImage.Id, outImage.Id, "lab2rgb") 
end  
function lips_Check(cmd, param)
  return Lua2Host:Checker(cmd, param) 
end
function lips_Blur(inpImage, outImage, radius)
  if inpImage.Channels ~= outImage.Channels then
    return false    
  end
  return Lua2Host:ProcessImage(inpImage.Id, outImage.Id, "blur", tostring(radius)) 
end
-- test
function lips_RegisterMouseEvent(md, mm, mu) -- mouseDown, mouseMove, mouseUp 
  local regev
  OnMouseEvents["OnMouseDown"] = md   
  OnMouseEvents["OnMouseMove"] = mm
  OnMouseEvents["OnMouseUp"]   = mu   
  if (md ~= nil) then
      regev = "true"
  else
      regev = "false"
  end
  if (mm ~= nil) then
      regev = regev .. ", true"
  else
      regev = regev .. ", false"
  end
  if (mu ~= nil) then
      regev = regev .. ", true"
  else
      regev = regev .. ", false"
  end
  Lua2Host:SendCommand("RegisterMouseEvent", regev)   
end
