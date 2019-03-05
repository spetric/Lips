require ("_prolog")
function RequireParams()  
   lips_RequireParams("local_assumption = bool([] false); ")
end   
function main()
  if (TargetImage.Plane == nil) then
      lips_SendMessage(errTargetEmpty)
      return
  end    
   labImage = lips_CreateImage("float3", SourceImage.Width, SourceImage.Height)
   --convert image to lab
   lips_SendMessage("Converting image to lab...")
   lips_Convert_rgb2lab(SourceImage, labImage)

   grayworld_assumption()
   
     --convert image to lab
  lips_SendMessage("Converting image to rgb...")
  lips_Convert_lab2rgb(labImage, TargetImage)  

end
function get_avg_a_b ()
    sum_a=0
    sum_b=0

    local top, bottom, left, right
    if local_stretch then
       top, bottom, left, right = Roi.Top, Roi.Bottom, Roi.Left, Roi.Right; 
    else
       top, bottom, left, right = 0, SourceImage.Height-1, 0, SourceImage.Width-1; 
    end

    -- first find average color in CIE Lab space
    for y=top, bottom do
      for x=left, right do
        l,a,b = lips_GetPixel(labImage, y, x)
        sum_a, sum_b = sum_a+a, sum_b+b
      end
       lips_ShowProgress(y, labImage.Height-1)
    end

    avg_a=sum_a/(labImage.Width*labImage.Height)
    avg_b=sum_b/(labImage.Width*labImage.Height)

    return avg_a,avg_b
end
 
function shift_a_b(a_shift, b_shift)
    for y=Roi.Top, Roi.Bottom do
      for x=Roi.Left, Roi.Right do
        l,a,b = lips_GetPixel(labImage, y, x)
        
        -- scale the chroma distance shifted according to amount of
        -- luminance. The 1.1 overshoot is because we cannot be sure
        -- to have gotten the data in the first place.
        a_delta = a_shift * (l/100) * 1.1
        b_delta = b_shift * (l/100) * 1.1
        a,b = a+a_delta, b+b_delta

        lips_SetPixel(labImage, y, x, l, a, b)
      end
       lips_ShowProgress(y - Roi.Top, Roi.Bottom - Roi.Top)
    end
end

function grayworld_assumption()
  avg_a, avg_b = get_avg_a_b()
  shift_a_b(-avg_a, -avg_b)
end

