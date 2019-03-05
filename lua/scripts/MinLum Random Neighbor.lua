-- random neighbor
-- take random pixel value from current pixel neighbour and replace current pixel 
-- with random pixel if difference of original and random luminance is less then tolerance
-- author: Siniša Petriæ http://www.sigmapi-design.com, 2018
require ("_prolog")
function RequireParams()
   lips_RequireParams("tolerance = int([1, 200] 40); radius = int([1, 100] 20 ); retries = int([1, 10] 3); ")
end   
function main()
  if (TargetImage.Plane == nil) then
      lips_SendMessage(errTargetEmpty)
      return
  end    
   --lum_tol = 40/255.0
   --random_neighbor(20, 3)
   lum_tol = tolerance/255
   random_neighbor(radius, retries)
end
-- get random pixel from neighborhood and check the luminance 
function get_random_rgb(x, y, window, retries)
   local rand_x, rand_y, r, g, b, lum1, lum2
   r, g, b = lips_GetPixel(SourceImage, y, x)    
   for k = 0, retries do
      rand_x =  x + math.floor((math.random(window + 1) - 1) - window/2) 
      rand_y =  y + math.floor((math.random(window + 1) - 1) - window/2)        
      lum1 = lips_GetLuminanceSafe(SourceImage, rand_y, rand_x)      
      lum2 = lips_GetLuminanceSafe(SourceImage, y, x)      
      if math.abs(lum1 - lum2) < lum_tol then
         r, g, b = lips_GetPixelSafe(SourceImage, rand_y, rand_x)    
         break
      end   
   end
   return r, g, b
end
-- main routine
function random_neighbor(seed_window, seed_retries)
math.randomseed( os.time() )
for y = Roi.Top, Roi.Bottom do
   for x = Roi.Left, Roi.Right do
       r, g, b = get_random_rgb(x, y, seed_window, seed_retries)   
       lips_SetPixel(TargetImage, y, x, r, g, b)
       end
   lips_ShowProgress(y - Roi.Top, Roi.Bottom - Roi.Top)       
   end
end 



