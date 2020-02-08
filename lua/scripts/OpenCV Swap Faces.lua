require ("_prolog")
require ("_opencv")
function RequireParams()  
   lips_RequireParams("single_face = bool([] true); extrapolate = bool([] true);"
    .. " result_as_new_image = bool([] true);"
    .. " scale_factor = float([1.0, 10.0] 1.05); min_neighbours = int([1, 15] 3);"
    .. " min_size = int([5, 150] 30);")
end   
function main()
  if (not OcvEnabled) then
     lips_SendMessage(errOcvDisabed)  
     return
  end     
  if (TargetImage.Plane == nil) then
      lips_SendMessage(errTargetEmpty)
      return
  end      
  -- collect parameters to params string
  local params = tostring(single_face)..", "..tostring(extrapolate)..", "..tostring(scale_factor)..", "
  ..tostring(min_neighbours)..", "..tostring(min_size)   
  -- path to cascade file (use ExePath)
  local cascade_name = ExePath.."data/face/lbpcascade_frontalface_improved.xml"
  if (not lips_Check("FileExists", cascade_name)) then
    lips_SendMessage("Error: Cascade file missing.")  
  lips_HostDialog('Cascade file missing. IMHO, face package is probably missing. Download face package: <A href="https://github.com/spetric/Lips/releases/download/v2.0/data_face.zip">data_face.zip</A><br> and unpack it under <b>data/face/</b> folder.', 2)           
    return
  end    
  -- path to model file
  local model_name  = ExePath.."data/face/face_landmark_model.dat"
  if (not lips_Check("FileExists", model_name)) then
    lips_SendMessage("Error: Model file missing.")  
  lips_HostDialog('Model file missing. IMHO, face package is probably missing. Download face package: <A href="https://github.com/spetric/Lips/releases/download/v2.0/data_face.zip">data_face.zip</A><br> and unpack it under <b>data/face/</b> folder.', 2)               
    return
  end      
  -- load classifier and face model
  lips_SendMessage("Loading classifier and face model.")
  -- create empty DK
  local dkIdx = ocv_CreateDK()
  -- set DK idx
  local rc = ocv_SetDK(dkIdx)
  -- "0" => OCW_FACE_KAZEMI (model type)
  if (not ocv_Process("LoadClassifierAndFaceModel", cascade_name, model_name, "0")) then     
     return
  end
  -- set source and target image
  local inpImage = lips_CreateImage("rgb24", SourceImage.Width, SourceImage.Height)  
  local outImage = lips_CreateImage("rgb24", TargetImage.Width, TargetImage.Height)    
  lips_CopyImage(SourceImage, inpImage)  
  lips_CopyImage(TargetImage, outImage)    
  -- set OpenCV src/tgt images
  ocv_SetImage("source", inpImage)
  ocv_SetImage("target", outImage) 
  --[[
    NOTE: OpenCV Mat is "as is", and our images are inverted (Delphi/BCB TBitmap or TIEBitmap).
    So, we need to flip images because face swapping does not work well on upside-down images.    
  --]]
  -- flip images 
  ocv_FlipSource()
  ocv_FlipTarget()  
  -- source image landmarks 
  lips_SendMessage("Detecting source image landmarks.")  
  if (not ocv_Process("FaceLandmarkDetector", "source", params)) then     
     return
  end  
  lips_SendMessage("Detecting target image landmarks.")  
  if (not ocv_Process("FaceLandmarkDetector", "target", params)) then     
     return
  end  
  -- swap faces (actually face2face)
  lips_SendMessage("Swapping faces.")    
  ocv_Process("SwapFaces")  
  -- flip output back, so that we can copy it to TBimtap or TIEBitmap
  ocv_FlipTarget()    
  if (result_as_new_image) then
      lips_ExportImage(outImage, "swapped face")
  else
      lips_ExportImage(outImage, "asTarget")
  end  
  ocv_ClearDK(dkIdx)
end