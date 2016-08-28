pt=cell(1,57);
for i=1:57
    
    buf=sprintf('b%02d.bmp.yml',i);
    
    m=readymlmat(['K:\2\',buf]);
    
    pt{i}=m;
end


for i=1:57

mx=pt{i}(:,end:-1:1,2)+1;
mxt=mx.';
mxv=mxt(:);

calib_data.Xp_abs(:,:,i)=mxv;

my=pt{i}(:,end:-1:1,1)+1;
myt=my.';
myv=myt(:);

calib_data.Yp_abs(:,:,i)=myv;

end