hrtf.Data.Delay = zeros(size(hrtf.SourcePosition,1), 2);
hrtf = SOFAupdateDimensions(hrtf);

L = size(hrtf.Data.IR,3);

for n=1:size(hrtf.SourcePosition,1)
    hrtf.Data.IR(n,1,:) = n + [1:L];
    hrtf.Data.IR(n,2,:) = n - [1:L];
    
    hrtf.Data.Delay(n,1) = n-1;
    hrtf.Data.Delay(n,2) = -(n-1);
end
    
   