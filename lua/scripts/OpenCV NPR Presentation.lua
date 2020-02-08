require ("_prolog")
require ("_opencv")
function RequireParams()  
   lips_RequireParams("opencv_filter = list([Detail enhance, Edge preserving Filter, PencilSketch, Stylization, Oil painting] 0);")
end   
function main()
  if (not OcvEnabled) then
     lips_SendMessage(errOcvDisabled)  
     return     
  end     
  if (TargetImage.Plane == nil) then
      lips_SendMessage(errTargetEmpty)
      return
  end      
  -- set source and target image
  ocv_SetImage("source", SourceImage)
  ocv_SetImage("target", TargetImage)   
  if (opencv_filter == 0) then  
    ocv_Process("detailenhance", "10, 0.15")
  elseif (opencv_filter == 1) then    
    ocv_Process("EdgePreservingFilter", "1, 60, 0.4")  
  elseif (opencv_filter == 2) then    
    ocv_Process("PencilSketch", "true, 60, 0.10, 0.02")    
  elseif (opencv_filter == 3) then    
    ocv_Process("Stylization", "60, 0.45")      
  else
    ocv_Process("OilPainting", "10, 1")      
  end
end

