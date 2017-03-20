Ridx=1;

% read the HRIR saved by mysofa
f=fopen('hrtf.dat');
irM=fread(f,inf,'float');
fclose(f);

% read the SOFA file
X=SOFAload('../sofa_api_mo_test/ARI_NH4_hrtf_M_dtf 256.sofa');

pos=[0 0 1.2];
[~,Midx]=min(sum((X.SourcePosition-repmat(pos,size(X.SourcePosition,1),1)).^2,2));
irS=squeeze(X.Data.IR(Midx,Ridx,:));
X.SourcePosition(Midx,:)

plot(irS);
hold on;
plot(irM,'r');