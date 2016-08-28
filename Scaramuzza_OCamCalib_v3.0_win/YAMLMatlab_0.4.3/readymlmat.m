function [m]=readymlmat(path)
tmppath='tmp.txt';
fid = fopen(path);
fido =fopen(tmppath,'wt');
tline = fgetl(fid);
tline = fgetl(fid);
fprintf(fido, 'm:\n');
tline = fgetl(fid);
while ischar(tline)
    fprintf(fido, '%s\n', tline);
    tline = fgetl(fid);
end
fclose(fido);
fclose(fid);
y_data = ReadYaml(tmppath);
ch=sscanf(y_data.m.dt,'%d');
data_cell=reshape(y_data.m.data,ch,y_data.m.cols,y_data.m.rows);
data=cell2mat(data_cell);
m=permute(data,[3,2,1]);
delete(tmppath);
end



