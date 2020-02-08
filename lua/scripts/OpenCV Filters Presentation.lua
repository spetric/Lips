require ("_prolog")
require ("_opencv")
function RequireParams()  
   lips_RequireParams("opencv_filter = list([blur, median, gaussian, bilateral, laplacian, sobel, box, sqrbox, dilate, erode, open, close, threshold, adaptive threshold, distance transform, Canny edge detector, contour 7x7 - filter2D ] 0);")
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
  -- set host roi
  ocv_SetRoi("host")
  -- process image
  if (opencv_filter == 0) then
     ocv_Process("blur", "5, 5")
  elseif (opencv_filter == 1) then
     ocv_Process("median", "5") 
  elseif (opencv_filter == 2) then
     ocv_Process("gaussian", "5, 5, 12.2, 12.2, 1") 
  elseif (opencv_filter == 3) then   
    ocv_Process("bilateral", "3, 1600.0, 1400.0")
  elseif (opencv_filter == 4) then    
    ocv_Process("laplacian", "1, 1.0, 0.0, false")
  elseif (opencv_filter == 5) then    
    ocv_Process("sobel", "2, 1, 3, 1.0, 0.0, false")
  elseif (opencv_filter == 6) then    
    ocv_Process("box", "5, 5, true")
  elseif (opencv_filter == 7) then    
    ocv_Process("sqrbox", "3, 3, true")
  elseif (opencv_filter == 8) then      
    ocv_Process("dilate", "3, 3, 0, 1")                  
  elseif (opencv_filter == 9) then      
    ocv_Process("erode", "3, 3, 0, 1")                      
  elseif (opencv_filter == 10) then      
    ocv_Process("morphologyEx", "3, 3, 2, 0, 1")                          
  elseif (opencv_filter == 11) then      
    ocv_Process("morphologyEx", "3, 3, 3, 0, 1")                              
  elseif (opencv_filter == 12) then      
    ocv_Process("threshold", "127, 255, 3")                                  
  elseif (opencv_filter == 13) then      
    ocv_Process("adaptivethreshold", "255, 0, 0, 5, 2")                                      
  elseif (opencv_filter == 14) then      
    ocv_Process("distanceTransform", "false, 1, 3")                                          
  elseif (opencv_filter == 15) then      
    ocv_Process("canny", "1, 3, 3, false")                                              
  elseif (opencv_filter == 16) then      
    ocv_Process("filter2d", "7, 1, 0", "1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, 1, 1, -1, -1, -1, -1, -1, 1, 1, -1, -1, -1, -1, -1, 1, 1, -1, -1, -1, -1, -1, 1, 1, -1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1, 1, 1") 
  end  
end
