-- Kuwahara filter
-- Performs the Kuwahara Filter. This filter is an edge-preserving filter.
--
--
-- ( a  a  ab   b  b)
-- ( a  a  ab   b  b)
-- (ac ac abcd bd bd)
-- ( c  c  cd   d  d)
-- ( c  c  cd   d  d)
--
-- In each of the four regions (a, b, c, d), the mean brightness and the variance are calculated. The
-- output value of the center pixel (abcd) in the window is the mean value of that region that has the
-- smallest variance.
--
-- description copied from http://www.incx.nec.co.jp/imap-vision/library/wouter/kuwahara.html
--
-- implemented by Oeyvind Kolas <oeyvindk@hig.no> 2004
-- modified for lipsEngine by Siniša Petriæ http://www.sigampi-design.com, 2018  

-- the sampling window is:  width=size*2+1  height=size*2+1

-- local function to get the mean value, and
-- variance from the rectangular area specified

-- requires prolog 
require ("_prolog")
function RequireParams()  
   lips_RequireParams("radius = int([1, 20] 2); ")
end   

function main()
  if (TargetImage.Plane == nil) then
      lips_SendMessage(errTargetEmpty)
      return
  end    
   kuwahara(radius)
end

function rgb_mean_and_variance (x0,y0,x1,y1)
 local variance
 local mean
 local r_mean
 local g_mean
 local b_mean 
 
 local minv           = 1.0
 local maxv           = 0.0
 local accumulated_r = 0
 local accumulated_g = 0
 local accumulated_b = 0
 local count         = 0
   
 local x, y

 for y=y0,y1 do
   for x=x0,x1 do
       local v     = lips_GetLuminanceSafe(SourceImage, y, x)
       local r, g, b = lips_GetPixelSafe(SourceImage, y, x)
       accumulated_r   = accumulated_r + r
       accumulated_g   = accumulated_g + g
       accumulated_b   = accumulated_b + b
       count         = count + 1
      
       if v < minv then minv = v end
       if v > maxv then maxv = v end
    end
  end

  variance = maxv-minv
  r_mean   = accumulated_r /count
  g_mean   = accumulated_g /count
  b_mean   = accumulated_b /count
    
  return r_mean, g_mean, b_mean, variance
end


-- return the kuwahara computed value
function rgb_kuwahara(x, y, size)
  local best_r, best_g, best_b = 0, 0, 0
  local best_variance = 1.0

  local r,g,b, variance

  r,g,b, variance = rgb_mean_and_variance (x-size, y-size, x, y)

  if variance < best_variance then
    best_r, best_g, best_b = r, g, b
    
    best_variance = variance
  end

  r,g,b, variance = rgb_mean_and_variance (x, y-size, x+size,y)

  if variance < best_variance then
    best_r, best_g, best_b = r, g, b
    best_variance = variance
  end

  r,g,b, variance = rgb_mean_and_variance (x, y, x+size, y+size)

  if variance < best_variance then
    best_r, best_g, best_b = r, g, b
    best_variance = variance
  end

  r,g,b, variance = rgb_mean_and_variance (x-size, y, x,y+size)

  if variance < best_variance then
    best_r, best_g, best_b = r, g, b
    best_variance = variance
  end

  return math.floor(best_r + 0.5), math.floor(best_g + 0.5), math.floor(best_b +0.5)
end
function kuwahara(radius)  
  local total = Roi.Bottom - Roi.Top      
  for y = Roi.Top, Roi.Bottom do
     for x = Roi.Left, Roi.Right do
         local r,g,b = rgb_kuwahara (x, y, radius)   
         lips_SetPixel(TargetImage, y, x, r, g, b)                       
     end 
     lips_ShowProgress(y - Roi.Top, total)                 
  end 
end   
