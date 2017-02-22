for n=1:size(hrtf.SourcePosition,1)
    a = round(hrtf.SourcePosition(n,1) / 2);
    if a < 0 
        a = a + 180;
    end
    a = a + 1;
    
    for u=1:size(hrtf.Data.IR,3) 
        if u == a
            hrtf.Data.IR(n,1,u) = 1;
        elseif (u > a & u <= a+8)
            hrtf.Data.IR(n,1,u) = -bitget(a,u-a);
        else
            hrtf.Data.IR(n,1,u) = 0;
        end
    end
        
    a = round(hrtf.SourcePosition(n,2) / 2);
    if a < 0 
        a = a + 180;
    end
    a = a + 1;
 
    for u=1:size(hrtf.Data.IR,3) 
        if u == a
            hrtf.Data.IR(n,2,u) = 1;
        elseif (u > a & u <= a+8)
            hrtf.Data.IR(n,2,u) = -bitget(a,u-a);
        else
            hrtf.Data.IR(n,2,u) = 0;
        end
    end
            
end
    
   