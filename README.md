### Raul P. Pelaez 2018. Periodic Boundary Conditions.  
  
  This utility applies PBC to the positions in a spunto-like file  
  
##  USAGE:  
  cat file | ./pbc [opt] > file.pbc  
    
  [opt]:  
    -L X                 To specify equal box length, X, in the three directions  
    -L X Y Z             To specify independent lengths along each direction  
    -Lx X -Ly Y -Lz Z    Same as above  
    If only Lx and Lz are present, 2D mode will be assumed.  
  
##  INPUT FORMAT  
```
  # Lines starting with # will be printed without modification  
  x y z WHATEVER  
  .  
  .  
  .  
  #  
  x2 y2 z2 WHATEVER  
  .  
  .  
  .  
  ```
  
  (any superpunto file with only xyz or xyrc columns will work, characters after xyz will be printed without modification)  
  
## OUTPUT FORMAT  
  
	Identical to input format.  
	  
	  
